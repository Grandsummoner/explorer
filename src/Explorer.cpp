#include "Explorer.hpp"

// Custom NanoVG widget that renders the 102 addressable RGB LEDs & rings
struct ExplorerLedRingsWidget : TransparentWidget {
    Explorer* module;

    ExplorerLedRingsWidget(Explorer* m) : module(m) {}

    void draw(const DrawArgs& args) override {
        if (!module) return;

        nvgSave(args.vg);

        // Knob Centers: matching Explorer panel SVG layout
        // Row 1: (34, 160), (90, 160), (146, 160)
        // Row 2: (34, 220), (90, 220), (146, 220)
        Vec knobCenters[6] = {
            Vec(34, 160), Vec(90, 160), Vec(146, 160),
            Vec(34, 220), Vec(90, 220), Vec(146, 220)
        };

        const float radius = 18.0f;
        const int numLeds = 16;
        const float minAngle = -135.0f * (M_PI / 180.0f);
        const float maxAngle = 135.0f * (M_PI / 180.0f);

        for (int k = 0; k < 6; k++) {
            Vec center = knobCenters[k];
            int startLed = k * 16;

            for (int i = 0; i < numLeds; i++) {
                float angle = minAngle + ((float)i / (numLeds - 1)) * (maxAngle - minAngle) - (M_PI / 2.0f);
                float x = center.x + radius * std::cos(angle);
                float y = center.y + radius * std::sin(angle);

                // Fetch LED color from hardware
                uint32_t rgb = module->hw.leds[startLed + i];
                uint8_t r = (rgb >> 16) & 0xFF;
                uint8_t g = (rgb >> 8) & 0xFF;
                uint8_t b = rgb & 0xFF;

                NVGcolor col = nvgRGBA(r, g, b, 255);

                // LED dot
                nvgBeginPath(args.vg);
                nvgCircle(args.vg, x, y, (r > 30 || g > 30 || b > 30) ? 1.6f : 1.1f);
                nvgFillColor(args.vg, col);
                nvgFill(args.vg);

                // Glow if active
                if (r > 60 || g > 60 || b > 60) {
                    nvgBeginPath(args.vg);
                    nvgCircle(args.vg, x, y, 3.2f);
                    nvgFillColor(args.vg, nvgRGBA(r, g, b, 60));
                    nvgFill(args.vg);
                }
            }
        }

        nvgRestore(args.vg);
    }
};

Explorer::Explorer() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

    // 6 Main Knobs
    for (int i = 0; i < 6; i++) {
        configParam(KNOB_1 + i, 0.0f, 1.0f, storedPreset[i], "Knob " + std::to_string(i + 1));
    }

    // 3 Buttons
    configButton(BTN_PAGE, "Page (Primary / Secondary tone)");
    configButton(BTN_LOCK, "Lock (Parameter Motion Loop)");
    configButton(BTN_PRESET, "Preset / Mode Select");

    // Audio Ports
    configInput(AUDIO_IN_L, "Audio In L (Mono)");
    configInput(AUDIO_IN_R, "Audio In R");
    configOutput(AUDIO_OUT_L, "Audio Out L");
    configOutput(AUDIO_OUT_R, "Audio Out R");

    // 6 Field-Programmable Jacks
    for (int i = 0; i < 6; i++) {
        configInput(FIELD_IN_1 + i, "Field Jack " + std::to_string(i + 1) + " (CV In)");
        configOutput(FIELD_OUT_1 + i, "Field Jack " + std::to_string(i + 1) + " (CV Out)");
    }

    hw.Init(48000.0f);
    stereo_eq::Init(48000.0f);
    cloud_delay::Init(48000.0f);
}

void Explorer::process(const ProcessArgs& args) {
    if (hw.sample_rate != args.sampleRate) {
        hw.sample_rate = args.sampleRate;
        stereo_eq::Init(args.sampleRate);
        cloud_delay::Init(args.sampleRate);
    }

    // 1. Read Knobs with Pot Catch
    for (int i = 0; i < 6; i++) {
        float paramVal = params[KNOB_1 + i].getValue();
        if (!isCaught[i]) {
            if (std::abs(paramVal - storedPreset[i]) < 0.04f) {
                isCaught[i] = true;
            }
        }
        if (isCaught[i]) {
            hw.knobs[i] = paramVal;
        }
    }

    // 2. Read Buttons
    for (int i = 0; i < 3; i++) {
        hw.buttons[i] = params[BTN_PAGE + i].getValue() > 0.5f;
    }

    // Toggle mode on Preset button edge
    if (params[BTN_PRESET].getValue() > 0.5f) {
        activeMode = (activeMode == 0) ? 1 : 0;
    }

    // 3. Audio Block Buffer Accumulator
    if (blockIdx >= BLOCK_SIZE) {
        float* inBuffers[2] = {blockInL, blockInR};
        float* outBuffers[2] = {blockOutL, blockOutR};

        if (activeMode == 0) {
            cloud_delay::ProcessBlock(inBuffers, outBuffers, BLOCK_SIZE, hw);
        } else {
            stereo_eq::ProcessBlock(inBuffers, outBuffers, BLOCK_SIZE, hw);
        }

        blockIdx = 0;
    }

    // Sample-by-sample audio transfer
    blockInL[blockIdx] = inputs[AUDIO_IN_L].getVoltage() / 5.0f;
    blockInR[blockIdx] = inputs[AUDIO_IN_R].isConnected()
        ? inputs[AUDIO_IN_R].getVoltage() / 5.0f
        : blockInL[blockIdx];

    outputs[AUDIO_OUT_L].setVoltage(blockOutL[blockIdx] * 5.0f);
    outputs[AUDIO_OUT_R].setVoltage(blockOutR[blockIdx] * 5.0f);

    // CV Field Jacks
    for (int i = 0; i < 6; i++) {
        if (hw.field_jack_mode[i] == hermetic::JACK_MODE_OUT) {
            outputs[FIELD_OUT_1 + i].setVoltage(hw.field_cv_out[i] * 5.0f);
        }
    }

    blockIdx++;
}

json_t* Explorer::dataToJson() {
    json_t* rootJ = json_object();
    json_object_set_new(rootJ, "activeMode", json_integer(activeMode));
    return rootJ;
}

void Explorer::dataFromJson(json_t* rootJ) {
    json_t* modeJ = json_object_get(rootJ, "activeMode");
    if (modeJ) {
        activeMode = json_integer_value(modeJ);
    }
}

// Module Widget UI
struct ExplorerWidget : ModuleWidget {
    ExplorerWidget(Explorer* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Explorer.svg")));

        // Screws
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // Top Audio I/O Jacks: In L, In R, Out L, Out R
        addInput(createInputCentered<PJ301MPort>(Vec(30, 72), module, Explorer::AUDIO_IN_L));
        addInput(createInputCentered<PJ301MPort>(Vec(70, 72), module, Explorer::AUDIO_IN_R));
        addOutput(createOutputCentered<PJ301MPort>(Vec(110, 72), module, Explorer::AUDIO_OUT_L));
        addOutput(createOutputCentered<PJ301MPort>(Vec(150, 72), module, Explorer::AUDIO_OUT_R));

        // 3 Buttons: Page, Lock, Preset
        addParam(createParamCentered<VCVButton>(Vec(40, 111), module, Explorer::BTN_PAGE));
        addParam(createParamCentered<VCVButton>(Vec(90, 111), module, Explorer::BTN_LOCK));
        addParam(createParamCentered<VCVButton>(Vec(140, 111), module, Explorer::BTN_PRESET));

        // 6 Knobs: RoundBlackKnob positioned on centers
        Vec knobCenters[6] = {
            Vec(34, 160), Vec(90, 160), Vec(146, 160),
            Vec(34, 220), Vec(90, 220), Vec(146, 220)
        };
        for (int i = 0; i < 6; i++) {
            addParam(createParamCentered<RoundBlackKnob>(knobCenters[i], module, Explorer::KNOB_1 + i));
        }

        // 6 Field-Programmable Jacks (Row 1: CV 1-3, Row 2: CV 4-6)
        Vec fieldJackCenters[6] = {
            Vec(34, 309), Vec(90, 309), Vec(146, 309),
            Vec(34, 345), Vec(90, 345), Vec(146, 345)
        };
        for (int i = 0; i < 6; i++) {
            addInput(createInputCentered<PJ301MPort>(fieldJackCenters[i], module, Explorer::FIELD_IN_1 + i));
        }

        // Add 102 Addressable LED Rings Renderer Widget
        if (module) {
            ExplorerLedRingsWidget* ledRings = new ExplorerLedRingsWidget(module);
            ledRings->box.size = box.size;
            addChild(ledRings);
        }
    }
};

Model* modelExplorer = createModel<Explorer, ExplorerWidget>("Explorer");
