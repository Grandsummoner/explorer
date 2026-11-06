#include "Explorer.hpp"

// Custom Stellar-style button widget: square button with rounded corners and clean border
struct ExplorerButton : ParamWidget {
    Module* mod = nullptr;
    int lightId = -1;
    NVGcolor litColor = nvgRGB(0x8A, 0x2A, 0x2A);   // Maroon accent
    NVGcolor unlitColor = nvgRGB(0xED, 0xE7, 0xDC); // Stellar cream button

    ExplorerButton() {
        box.size = mm2px(Vec(4.8, 4.8));
    }

    void onButton(const ButtonEvent& e) override {
        ParamWidget::onButton(e);
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
            ParamQuantity* pq = getParamQuantity();
            if (pq) pq->setValue(1.f);
            e.consume(this);
        }
    }

    void onDragEnd(const DragEndEvent& e) override {
        ParamWidget::onDragEnd(e);
        ParamQuantity* pq = getParamQuantity();
        if (pq) pq->setValue(0.f);
    }

    void draw(const DrawArgs& args) override {
        ParamQuantity* pq = getParamQuantity();
        bool pressed = pq ? pq->getValue() > 0.5f : false;

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.8f);
        nvgFillColor(args.vg, pressed ? litColor : unlitColor);
        nvgFill(args.vg);
        nvgStrokeColor(args.vg, nvgRGB(0x1A, 0x18, 0x14));
        nvgStrokeWidth(args.vg, 0.8f);
        nvgStroke(args.vg);
    }
};

// Custom Stellar-style knob widget: dark gradient with white pointer indicator
struct ExplorerKnob : ParamWidget {
    ExplorerKnob() {
        box.size = mm2px(Vec(6.4, 6.4));
    }

    void onButton(const ButtonEvent& e) override {
        ParamWidget::onButton(e);
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
            e.consume(this);
        }
    }

    void onDragMove(const DragMoveEvent& e) override {
        ParamQuantity* pq = getParamQuantity();
        if (!pq) return;
        float delta = -e.mouseDelta.y * 0.005f;
        pq->setValue(clamp(pq->getValue() + delta, pq->getMinValue(), pq->getMaxValue()));
    }

    void onDoubleClick(const DoubleClickEvent& e) override {
        ParamQuantity* pq = getParamQuantity();
        if (pq) pq->reset();
    }

    void draw(const DrawArgs& args) override {
        float cx = box.size.x / 2.f, cy = box.size.y / 2.f;
        float r = box.size.x / 2.f;

        // Knob body with subtle gradient
        nvgBeginPath(args.vg);
        nvgCircle(args.vg, cx, cy, r);
        NVGpaint grad = nvgLinearGradient(args.vg, cx, cy - r, cx, cy + r,
                                          nvgRGB(0x26, 0x26, 0x28), nvgRGB(0x1A, 0x1A, 0x1C));
        nvgFillPaint(args.vg, grad);
        nvgFill(args.vg);
        nvgStrokeColor(args.vg, nvgRGB(0x0A, 0x0A, 0x0A));
        nvgStrokeWidth(args.vg, 0.6f);
        nvgStroke(args.vg);

        // Indicator line
        float value = 0.f;
        ParamQuantity* pq = getParamQuantity();
        if (pq) value = pq->getScaledValue();
        float angle = rescale(value, 0.f, 1.f, -0.8f * (float)M_PI, 0.8f * (float)M_PI) - (float)M_PI / 2.f;

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, cx, cy);
        nvgLineTo(args.vg, cx + std::cos(angle) * r * 0.8f, cy + std::sin(angle) * r * 0.8f);
        nvgStrokeColor(args.vg, nvgRGB(0xE8, 0xE0, 0xD0));
        nvgStrokeWidth(args.vg, 0.9f);
        nvgStroke(args.vg);
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

    // Adjacency detection matching Stellar's exact family integration
    // The link lights only turn on if placed next to a family module (left, right, or both)
    auto isFamily = [](Module* m) -> bool {
        if (!m || !m->model || !m->model->plugin) return false;
        const std::string& pslug = m->model->plugin->slug;
        const std::string& mslug = m->model->slug;
        return (pslug == "SpacesCommand" || mslug == "SpacesCommand" ||
                pslug == "Intel" || mslug == "Intel" ||
                pslug == "Beams" || mslug == "Beams" ||
                pslug == "Stellar" || mslug == "Stellar" ||
                pslug == "HermeticModular" || mslug == "Explorer");
    };

    bool linkedLeft = isFamily(leftExpander.module);
    bool linkedRight = isFamily(rightExpander.module);
    lights[LINK_LEFT_LIGHT].setBrightness(linkedLeft ? 1.0f : 0.0f);
    lights[LINK_RIGHT_LIGHT].setBrightness(linkedRight ? 1.0f : 0.0f);

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

    outputs[AUDIO_OUT_L].setVoltage(clamp(blockOutL[blockIdx] * 5.0f, -10.0f, 10.0f));
    outputs[AUDIO_OUT_R].setVoltage(clamp(blockOutR[blockIdx] * 5.0f, -10.0f, 10.0f));

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

        // Automatically size from panel SVG (exactly matching Stellar convention)
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Explorer.svg")));

        // Helper lambdas for positioning in exact mm coordinates
        auto inPort = [&](float x, float y, int id) {
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, id));
        };
        auto outPort = [&](float x, float y, int id) {
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, id));
        };

        // 1. Audio I/O Jacks (Row 1): IN L, IN R, OUT L, OUT R (y = 17.50 mm)
        inPort(14.00f, 17.50f, Explorer::AUDIO_IN_L);
        inPort(26.65f, 17.50f, Explorer::AUDIO_IN_R);
        outPort(39.31f, 17.50f, Explorer::AUDIO_OUT_L);
        outPort(51.96f, 17.50f, Explorer::AUDIO_OUT_R);

        // 2. Control Buttons (Row 2): PAGE, LOCK, MODE (y = 33.90 mm)
        {
            auto* btnPage = createParamCentered<ExplorerButton>(mm2px(Vec(16.50f, 33.90f)), module, Explorer::BTN_PAGE);
            btnPage->mod = module;
            addParam(btnPage);

            auto* btnLock = createParamCentered<ExplorerButton>(mm2px(Vec(32.98f, 33.90f)), module, Explorer::BTN_LOCK);
            btnLock->mod = module;
            addParam(btnLock);

            auto* btnMode = createParamCentered<ExplorerButton>(mm2px(Vec(49.46f, 33.90f)), module, Explorer::BTN_PRESET);
            btnMode->mod = module;
            addParam(btnMode);
        }

        // 3. DSP Knobs: 6 Knobs without LED rings (exactly matching Stellar)
        // Sub-row 1 (TIME, FDBK, DIFF) at y = 52.30 mm
        addParam(createParamCentered<ExplorerKnob>(mm2px(Vec(15.50f, 52.30f)), module, Explorer::KNOB_1));
        addParam(createParamCentered<ExplorerKnob>(mm2px(Vec(32.98f, 52.30f)), module, Explorer::KNOB_2));
        addParam(createParamCentered<ExplorerKnob>(mm2px(Vec(50.46f, 52.30f)), module, Explorer::KNOB_3));

        // Sub-row 2 (TONE, MOD, MIX) at y = 68.30 mm
        addParam(createParamCentered<ExplorerKnob>(mm2px(Vec(15.50f, 68.30f)), module, Explorer::KNOB_4));
        addParam(createParamCentered<ExplorerKnob>(mm2px(Vec(32.98f, 68.30f)), module, Explorer::KNOB_5));
        addParam(createParamCentered<ExplorerKnob>(mm2px(Vec(50.46f, 68.30f)), module, Explorer::KNOB_6));

        // 4. Field CV Jacks: 6 CV Ports with 17mm vertical row spacing (completely legible!)
        // Sub-row 1 (CV 1, CV 2, CV 3) at y = 91.70 mm
        inPort(15.50f, 91.70f, Explorer::FIELD_IN_1);
        inPort(32.98f, 91.70f, Explorer::FIELD_IN_1 + 1);
        inPort(50.46f, 91.70f, Explorer::FIELD_IN_1 + 2);

        // Sub-row 2 (CV 4, CV 5, CV 6) at y = 108.70 mm
        inPort(15.50f, 108.70f, Explorer::FIELD_IN_1 + 3);
        inPort(32.98f, 108.70f, Explorer::FIELD_IN_1 + 4);
        inPort(50.46f, 108.70f, Explorer::FIELD_IN_1 + 5);

        // 5. Corner Status/Link Lights (Matching Stellar's small blue link lights)
        // Only illuminate when adjacent to family modules
        addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(Vec(5.00f, 5.00f)), module, Explorer::LINK_LEFT_LIGHT));
        addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(Vec(55.96f, 5.00f)), module, Explorer::LINK_RIGHT_LIGHT));

        // Notice: NO Screws added! Exactly like Stellar, eliminating any protrusion outside the side panel!
    }
};

Model* modelExplorer = createModel<Explorer, ExplorerWidget>("Explorer");
