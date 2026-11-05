#include "Explorer.hpp"

// Custom NanoVG widget that renders all silk-screened text labels, titles, and section headers
struct ExplorerLabelsWidget : TransparentWidget {
    Explorer* module;

    ExplorerLabelsWidget(Explorer* m) : module(m) {}

    void draw(const DrawArgs& args) override {
        nvgSave(args.vg);

        // Load standard VCV Rack system fonts
        std::shared_ptr<Font> fontBold = APP->window->loadFont(asset::system("res/fonts/Nunito-Bold.ttf"));
        if (!fontBold) {
            fontBold = APP->window->loadFont(asset::system("res/fonts/ShareTechMono-Regular.ttf"));
        }
        std::shared_ptr<Font> fontRegular = APP->window->loadFont(asset::system("res/fonts/ShareTechMono-Regular.ttf"));

        if (fontBold) {
            nvgFontFaceId(args.vg, fontBold->handle);

            // 1. Module Header Title (matching Stellar / Spaces Command aesthetic)
            nvgFontSize(args.vg, 12.5f);
            nvgFillColor(args.vg, nvgRGB(0x1C, 0x19, 0x17)); // Deep charcoal
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            nvgText(args.vg, 90.0f, 23.0f, "EXPLORER", NULL);

            // Subtitle
            nvgFontSize(args.vg, 6.2f);
            nvgFillColor(args.vg, nvgRGB(0x78, 0x71, 0x6C)); // Muted stone
            nvgText(args.vg, 90.0f, 33.5f, "field-programmable dsp", NULL);

            // Brand
            nvgFontSize(args.vg, 5.0f);
            nvgFillColor(args.vg, nvgRGB(0xA8, 0xA2, 0x9E));
            nvgText(args.vg, 90.0f, 41.0f, "HERMETIC MODULAR", NULL);

            // 2. Rotated Vertical Section Header Labels on Left Margin
            nvgFontSize(args.vg, 5.8f);
            nvgFillColor(args.vg, nvgRGB(0x44, 0x40, 0x3C));

            // Section 1: AUDIO
            nvgSave(args.vg);
            nvgTranslate(args.vg, 10.0f, 72.0f);
            nvgRotate(args.vg, -M_PI / 2.0f);
            nvgText(args.vg, 0.0f, 0.0f, "AUDIO", NULL);
            nvgRestore(args.vg);

            // Section 2: CTRL
            nvgSave(args.vg);
            nvgTranslate(args.vg, 10.0f, 113.0f);
            nvgRotate(args.vg, -M_PI / 2.0f);
            nvgText(args.vg, 0.0f, 0.0f, "CTRL", NULL);
            nvgRestore(args.vg);

            // Section 3: DSP 1 & DSP 2
            nvgSave(args.vg);
            nvgTranslate(args.vg, 10.0f, 160.0f);
            nvgRotate(args.vg, -M_PI / 2.0f);
            nvgText(args.vg, 0.0f, 0.0f, "DSP 1", NULL);
            nvgRestore(args.vg);

            nvgSave(args.vg);
            nvgTranslate(args.vg, 10.0f, 220.0f);
            nvgRotate(args.vg, -M_PI / 2.0f);
            nvgText(args.vg, 0.0f, 0.0f, "DSP 2", NULL);
            nvgRestore(args.vg);

            // Section 4: FIELD
            nvgSave(args.vg);
            nvgTranslate(args.vg, 10.0f, 327.0f);
            nvgRotate(args.vg, -M_PI / 2.0f);
            nvgText(args.vg, 0.0f, 0.0f, "FIELD", NULL);
            nvgRestore(args.vg);

            // 3. Audio Jack Labels (Y: 72 -> labels at Y: 86)
            nvgFontSize(args.vg, 6.0f);
            nvgFillColor(args.vg, nvgRGB(0x29, 0x25, 0x24));
            nvgText(args.vg, 30.0f, 86.5f, "IN L", NULL);
            nvgText(args.vg, 70.0f, 86.5f, "IN R", NULL);
            nvgFillColor(args.vg, nvgRGB(0xC2, 0x41, 0x0C)); // Terracotta orange
            nvgText(args.vg, 110.0f, 86.5f, "OUT L", NULL);
            nvgText(args.vg, 150.0f, 86.5f, "OUT R", NULL);

            // 4. Button Labels (Y: 111 -> labels at Y: 123.5)
            nvgFillColor(args.vg, nvgRGB(0x44, 0x40, 0x3C));
            nvgFontSize(args.vg, 5.5f);
            nvgText(args.vg, 40.0f, 123.5f, "PAGE", NULL);
            nvgText(args.vg, 90.0f, 123.5f, "LOCK", NULL);
            nvgText(args.vg, 140.0f, 123.5f, "MODE", NULL);

            // 5. Knob Labels (Row 1: Y 160 -> Y 188; Row 2: Y 220 -> Y 248)
            nvgFontSize(args.vg, 6.5f);
            nvgFillColor(args.vg, nvgRGB(0x1C, 0x19, 0x17));
            
            int mode = (module) ? module->activeMode : 0;
            if (mode == 0) {
                // Cloud Delay & Reverb primary mode
                nvgText(args.vg, 34.0f, 188.0f, "TIME", NULL);
                nvgText(args.vg, 90.0f, 188.0f, "FDBK", NULL);
                nvgText(args.vg, 146.0f, 188.0f, "DIFF", NULL);

                nvgText(args.vg, 34.0f, 248.0f, "TONE", NULL);
                nvgText(args.vg, 90.0f, 248.0f, "MOD", NULL);
                nvgText(args.vg, 146.0f, 248.0f, "MIX", NULL);
            } else {
                // Stereo 3-Band Parametric EQ & Drive mode
                nvgText(args.vg, 34.0f, 188.0f, "LOW", NULL);
                nvgText(args.vg, 90.0f, 188.0f, "MID", NULL);
                nvgText(args.vg, 146.0f, 188.0f, "HIGH", NULL);

                nvgText(args.vg, 34.0f, 248.0f, "FREQ", NULL);
                nvgText(args.vg, 90.0f, 248.0f, "DRIVE", NULL);
                nvgText(args.vg, 146.0f, 248.0f, "LEVEL", NULL);
            }

            // 6. Field CV Jack Labels (Row 1: Y 309 -> Y 323; Row 2: Y 345 -> Y 359)
            nvgFontSize(args.vg, 5.5f);
            nvgFillColor(args.vg, nvgRGB(0x03, 0x69, 0xA1)); // Cyan/blue
            nvgText(args.vg, 34.0f, 323.0f, "CV 1", NULL);
            nvgText(args.vg, 90.0f, 323.0f, "CV 2", NULL);
            nvgText(args.vg, 146.0f, 323.0f, "CV 3", NULL);

            nvgFillColor(args.vg, nvgRGB(0x5B, 0x21, 0xB6)); // Violet
            nvgText(args.vg, 34.0f, 359.0f, "CV 4", NULL);
            nvgText(args.vg, 90.0f, 359.0f, "CV 5", NULL);
            nvgText(args.vg, 146.0f, 359.0f, "CV 6", NULL);
        }

        nvgRestore(args.vg);
    }
};

// Custom NanoVG widget that renders the 102 addressable RGB LEDs & rings
struct ExplorerLedRingsWidget : TransparentWidget {
    Explorer* module;

    ExplorerLedRingsWidget(Explorer* m) : module(m) {}

    void draw(const DrawArgs& args) override {
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

                uint8_t r = 0, g = 0, b = 0;
                if (module) {
                    uint32_t rgb = module->hw.leds[startLed + i];
                    r = (rgb >> 16) & 0xFF;
                    g = (rgb >> 8) & 0xFF;
                    b = rgb & 0xFF;
                }

                // If unlit, render subtle sand/cream guide dot
                if (r <= 30 && g <= 30 && b <= 30) {
                    nvgBeginPath(args.vg);
                    nvgCircle(args.vg, x, y, 1.1f);
                    nvgFillColor(args.vg, nvgRGBA(180, 172, 158, 160));
                    nvgFill(args.vg);
                } else {
                    NVGcolor col = nvgRGBA(r, g, b, 255);

                    // Lit LED dot
                    nvgBeginPath(args.vg);
                    nvgCircle(args.vg, x, y, 1.7f);
                    nvgFillColor(args.vg, col);
                    nvgFill(args.vg);

                    // Soft Bloom Halo
                    nvgBeginPath(args.vg);
                    nvgCircle(args.vg, x, y, 3.6f);
                    nvgFillColor(args.vg, nvgRGBA(r, g, b, 65));
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

        // 1. Add Silk-screened Labels and Text Overlay (drawn under knobs & ports)
        ExplorerLabelsWidget* labels = new ExplorerLabelsWidget(module);
        labels->box.size = box.size;
        addChild(labels);

        // 2. Add 102 Addressable LED Rings Renderer Widget
        ExplorerLedRingsWidget* ledRings = new ExplorerLedRingsWidget(module);
        ledRings->box.size = box.size;
        addChild(ledRings);

        // Screws (Classic Eurorack)
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // Top Audio I/O Jacks: In L, In R, Out L, Out R
        addInput(createInputCentered<PJ301MPort>(Vec(30, 72), module, Explorer::AUDIO_IN_L));
        addInput(createInputCentered<PJ301MPort>(Vec(70, 72), module, Explorer::AUDIO_IN_R));
        addOutput(createOutputCentered<PJ301MPort>(Vec(110, 72), module, Explorer::AUDIO_OUT_L));
        addOutput(createOutputCentered<PJ301MPort>(Vec(150, 72), module, Explorer::AUDIO_OUT_R));

        // 3 Pushbuttons: Page, Lock, Preset
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
    }
};

Model* modelExplorer = createModel<Explorer, ExplorerWidget>("Explorer");
