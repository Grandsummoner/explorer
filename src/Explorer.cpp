#include "Explorer.hpp"

// Custom Stellar-style button widget: square button with rounded corners and latching lit color
struct StellarButton : ParamWidget {
    Module* mod = nullptr;
    int lightId = -1;
    NVGcolor litColor = nvgRGB(0x8A, 0x2A, 0x2A);   // Terracotta / Maroon accent
    NVGcolor unlitColor = nvgRGB(0xED, 0xE7, 0xDC); // Stellar cream button

    StellarButton() {
        box.size = mm2px(Vec(4.6, 4.6));
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
        bool lit = (mod && lightId >= 0) ? mod->lights[lightId].getBrightness() > 0.5f : false;

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.7f);
        nvgFillColor(args.vg, lit ? litColor : unlitColor);
        nvgFill(args.vg);
        nvgStrokeColor(args.vg, nvgRGB(0x1A, 0x18, 0x14));
        nvgStrokeWidth(args.vg, 0.8f);
        nvgStroke(args.vg);
    }
};

// Custom Stellar-style knob widget: dark gradient with white pointer indicator
struct StellarKnob : ParamWidget {
    StellarKnob() {
        box.size = mm2px(Vec(5.6, 5.6));
    }

    void onButton(const ButtonEvent& e) override {
        ParamWidget::onButton(e);
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
            e.consume(this);
        }
    }

    void onDragMove(const DragMoveEvent& e) override {
        ParamWidget::onDragMove(e);
        ParamQuantity* pq = getParamQuantity();
        if (!pq) return;
        float range = pq->getMaxValue() - pq->getMinValue();
        if (!std::isfinite(range) || range <= 0.f) range = 1.f;
        float delta = -e.mouseDelta.y * range / 200.f;
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
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    // SPACE Card (6 Knobs)
    configParam(TIME_PARAM, 0.0f, 1.0f, 0.35f, "Delay Time");
    configParam(FDBK_PARAM, 0.0f, 1.0f, 0.45f, "Delay Feedback");
    configParam(DIFF_PARAM, 0.0f, 1.0f, 0.60f, "Reverb Diffusion");
    configParam(DAMP_PARAM, 0.0f, 1.0f, 0.50f, "Damping Filter");
    configParam(DMIX_PARAM, 0.0f, 1.0f, 0.50f, "Delay Mix");
    configParam(RMIX_PARAM, 0.0f, 1.0f, 0.40f, "Reverb Mix");

    // SCULPT Card (3 Knobs)
    configParam(WIDTH_PARAM, 0.0f, 1.0f, 0.50f, "Stereo Panorama Width");
    configParam(COLOR_PARAM, 0.0f, 1.0f, 0.50f, "Tilt Color EQ");
    configParam(FLUTTER_PARAM, 0.0f, 1.0f, 0.20f, "Tape Wow & Flutter");

    // CTRL Card (3 Pushbuttons)
    configButton(STEREO_PARAM, "Stereo Ping-Pong / Parallel");
    configButton(FREEZE_PARAM, "Buffer Freeze");
    configButton(SHIMMER_PARAM, "Octave Shimmer Bloom");

    // Audio Ports
    configInput(AUDIO_IN_L, "Audio In L (Mono)");
    configInput(AUDIO_IN_R, "Audio In R");
    configOutput(AUDIO_OUT_L, "Audio Out L");
    configOutput(AUDIO_OUT_R, "Audio Out R");

    // FIELD Card (6 CV Inputs)
    configInput(TIME_CV_INPUT, "Time CV");
    configInput(FDBK_CV_INPUT, "Feedback CV");
    configInput(DIFF_CV_INPUT, "Diffusion CV");
    configInput(RMIX_CV_INPUT, "Reverb Mix CV");
    configInput(WIDTH_CV_INPUT, "Width CV");
    configInput(FLUTTER_CV_INPUT, "Flutter CV");

    explorer_dsp::Init(48000.f);
}

void Explorer::process(const ProcessArgs& args) {
    // 1. Companion Adjacency Light Detection
    auto isFamily = [](Module* m) -> bool {
        if (!m || !m->model || !m->model->plugin) return false;
        const std::string& pslug = m->model->plugin->slug;
        const std::string& mslug = m->model->slug;
        return (pslug == "SpacesCommand" || mslug == "SpacesCommand" ||
                pslug == "Intel" || mslug == "Intel" ||
                pslug == "Beams" || mslug == "Beams" ||
                pslug == "Stellar" || mslug == "Stellar" ||
                pslug == "Explorer" || mslug == "Explorer" ||
                pslug == "HermeticModular");
    };

    bool linkedLeft = isFamily(leftExpander.module);
    bool linkedRight = isFamily(rightExpander.module);
    lights[LINK_LEFT_LIGHT].setBrightness(linkedLeft ? 1.0f : 0.0f);
    lights[LINK_RIGHT_LIGHT].setBrightness(linkedRight ? 1.0f : 0.0f);

    // 2. Latching Button Triggers
    if (stereoTrigger.process(params[STEREO_PARAM].getValue() > 0.5f)) {
        isStereo = !isStereo;
    }
    if (freezeTrigger.process(params[FREEZE_PARAM].getValue() > 0.5f)) {
        isFrozen = !isFrozen;
    }
    if (shimmerTrigger.process(params[SHIMMER_PARAM].getValue() > 0.5f)) {
        isShimmer = !isShimmer;
    }

    lights[STEREO_LIGHT].setBrightness(isStereo ? 1.0f : 0.0f);
    lights[FREEZE_LIGHT].setBrightness(isFrozen ? 1.0f : 0.0f);
    lights[SHIMMER_LIGHT].setBrightness(isShimmer ? 1.0f : 0.0f);

    // 3. Read Knobs + CV Modulation
    auto getCv = [&](int cvInputId) -> float {
        return inputs[cvInputId].isConnected() ? (inputs[cvInputId].getVoltage() / 5.0f) : 0.0f;
    };

    dspState.time = clamp(params[TIME_PARAM].getValue() + getCv(TIME_CV_INPUT), 0.0f, 1.0f);
    dspState.feedback = clamp(params[FDBK_PARAM].getValue() + getCv(FDBK_CV_INPUT), 0.0f, 1.0f);
    dspState.diffusion = clamp(params[DIFF_PARAM].getValue() + getCv(DIFF_CV_INPUT), 0.0f, 1.0f);
    dspState.damping = clamp(params[DAMP_PARAM].getValue(), 0.0f, 1.0f);
    dspState.delayMix = clamp(params[DMIX_PARAM].getValue(), 0.0f, 1.0f);
    dspState.reverbMix = clamp(params[RMIX_PARAM].getValue() + getCv(RMIX_CV_INPUT), 0.0f, 1.0f);

    dspState.width = clamp(params[WIDTH_PARAM].getValue() + getCv(WIDTH_CV_INPUT), 0.0f, 1.0f);
    dspState.color = clamp(params[COLOR_PARAM].getValue(), 0.0f, 1.0f);
    dspState.flutter = clamp(params[FLUTTER_PARAM].getValue() + getCv(FLUTTER_CV_INPUT), 0.0f, 1.0f);

    dspState.stereoPingPong = isStereo;
    dspState.freeze = isFrozen;
    dspState.shimmer = isShimmer;

    // 4. Audio Processing (Mono-to-Stereo Normaling on IN L)
    float inL = inputs[AUDIO_IN_L].getVoltage() / 5.0f;
    float inR = inputs[AUDIO_IN_R].isConnected() ? (inputs[AUDIO_IN_R].getVoltage() / 5.0f) : inL;

    float outL = 0.f, outR = 0.f;
    explorer_dsp::ProcessSample(inL, inR, outL, outR, dspState, args.sampleRate);

    // Scaled output to Eurorack standard ±5V
    outputs[AUDIO_OUT_L].setVoltage(clamp(outL * 5.0f, -10.0f, 10.0f));
    outputs[AUDIO_OUT_R].setVoltage(clamp(outR * 5.0f, -10.0f, 10.0f));
}

json_t* Explorer::dataToJson() {
    json_t* rootJ = json_object();
    json_object_set_new(rootJ, "isStereo", json_boolean(isStereo));
    json_object_set_new(rootJ, "isFrozen", json_boolean(isFrozen));
    json_object_set_new(rootJ, "isShimmer", json_boolean(isShimmer));
    json_object_set_new(rootJ, "delayRangeMode", json_integer(dspState.delayRangeMode));
    return rootJ;
}

void Explorer::dataFromJson(json_t* rootJ) {
    json_t* stJ = json_object_get(rootJ, "isStereo");
    if (stJ) isStereo = json_boolean_value(stJ);

    json_t* frJ = json_object_get(rootJ, "isFrozen");
    if (frJ) isFrozen = json_boolean_value(frJ);

    json_t* shJ = json_object_get(rootJ, "isShimmer");
    if (shJ) isShimmer = json_boolean_value(shJ);

    json_t* rgJ = json_object_get(rootJ, "delayRangeMode");
    if (rgJ) dspState.delayRangeMode = json_integer_value(rgJ);
}

// Module Widget UI
struct ExplorerWidget : ModuleWidget {
    ExplorerWidget(Explorer* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Explorer.svg")));

        auto inPort = [&](float x, float y, int id) {
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, id));
        };
        auto outPort = [&](float x, float y, int id) {
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, id));
        };

        // 1. Audio I/O Jacks (Row 1): IN L, IN R, OUT L, OUT R (y = 17.4 mm)
        inPort(15.57f, 17.4f, Explorer::AUDIO_IN_L);
        inPort(27.11f, 17.4f, Explorer::AUDIO_IN_R);
        outPort(38.65f, 17.4f, Explorer::AUDIO_OUT_L);
        outPort(50.19f, 17.4f, Explorer::AUDIO_OUT_R);

        // 2. Control Buttons (Row 2): STEREO, FREEZE, SHIMMER (y = 32.2 mm)
        {
            auto addBtn = [&](float x, float y, int paramId, int lightId, NVGcolor litCol) {
                auto* btn = createParamCentered<StellarButton>(mm2px(Vec(x, y)), module, paramId);
                btn->mod = module;
                btn->lightId = lightId;
                btn->litColor = litCol;
                addParam(btn);
            };

            addBtn(17.49f, 32.2f, Explorer::STEREO_PARAM, Explorer::STEREO_LIGHT, nvgRGB(0x8A, 0x64, 0x23));  // Brass
            addBtn(32.88f, 32.2f, Explorer::FREEZE_PARAM, Explorer::FREEZE_LIGHT, nvgRGB(0x8A, 0x2A, 0x2A));  // Terracotta / Maroon
            addBtn(48.27f, 32.2f, Explorer::SHIMMER_PARAM, Explorer::SHIMMER_LIGHT, nvgRGB(0x8A, 0x64, 0x23)); // Brass
        }

        // 3. SPACE Card: 6 Knobs
        // Row 1 (y = 48.0 mm): TIME, FDBK, DIFF
        addParam(createParamCentered<StellarKnob>(mm2px(Vec(17.49f, 48.0f)), module, Explorer::TIME_PARAM));
        addParam(createParamCentered<StellarKnob>(mm2px(Vec(32.88f, 48.0f)), module, Explorer::FDBK_PARAM));
        addParam(createParamCentered<StellarKnob>(mm2px(Vec(48.27f, 48.0f)), module, Explorer::DIFF_PARAM));

        // Row 2 (y = 64.0 mm): DAMP, D-MIX, R-MIX
        addParam(createParamCentered<StellarKnob>(mm2px(Vec(17.49f, 64.0f)), module, Explorer::DAMP_PARAM));
        addParam(createParamCentered<StellarKnob>(mm2px(Vec(32.88f, 64.0f)), module, Explorer::DMIX_PARAM));
        addParam(createParamCentered<StellarKnob>(mm2px(Vec(48.27f, 64.0f)), module, Explorer::RMIX_PARAM));

        // 4. SCULPT Card: 3 Knobs (y = 82.4 mm): WIDTH, COLOR, FLUTTER
        addParam(createParamCentered<StellarKnob>(mm2px(Vec(17.49f, 82.4f)), module, Explorer::WIDTH_PARAM));
        addParam(createParamCentered<StellarKnob>(mm2px(Vec(32.88f, 82.4f)), module, Explorer::COLOR_PARAM));
        addParam(createParamCentered<StellarKnob>(mm2px(Vec(48.27f, 82.4f)), module, Explorer::FLUTTER_PARAM));

        // 5. FIELD Card: 6 CV Jacks
        // Row 1 (y = 100.6 mm): TIME CV, FDBK CV, DIFF CV
        inPort(17.49f, 100.6f, Explorer::TIME_CV_INPUT);
        inPort(32.88f, 100.6f, Explorer::FDBK_CV_INPUT);
        inPort(48.27f, 100.6f, Explorer::DIFF_CV_INPUT);

        // Row 2 (y = 114.2 mm): R-MIX CV, WIDTH CV, FLUTTER CV
        inPort(17.49f, 114.2f, Explorer::RMIX_CV_INPUT);
        inPort(32.88f, 114.2f, Explorer::WIDTH_CV_INPUT);
        inPort(48.27f, 114.2f, Explorer::FLUTTER_CV_INPUT);

        // 6. Corner Adjacency Status Lights
        addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(Vec(5.00f, 5.00f)), module, Explorer::LINK_LEFT_LIGHT));
        addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(Vec(55.96f, 5.00f)), module, Explorer::LINK_RIGHT_LIGHT));
    }

    void appendContextMenu(Menu* menu) override {
        Explorer* explorer = dynamic_cast<Explorer*>(module);
        if (!explorer) return;

        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Explorer Settings"));

        struct RangeItem : MenuItem {
            Explorer* explorer;
            int mode;
            void onAction(const event::Action& e) override {
                explorer->dspState.delayRangeMode = mode;
            }
        };

        auto* itemNormal = new RangeItem;
        itemNormal->text = "Delay Range: Normal (50ms - 1.5s)";
        itemNormal->rightText = (explorer->dspState.delayRangeMode == 0) ? "✔" : "";
        itemNormal->explorer = explorer;
        itemNormal->mode = 0;
        menu->addChild(itemNormal);

        auto* itemDeep = new RangeItem;
        itemDeep->text = "Delay Range: Deep Space (up to 4.0s)";
        itemDeep->rightText = (explorer->dspState.delayRangeMode == 1) ? "✔" : "";
        itemDeep->explorer = explorer;
        itemDeep->mode = 1;
        menu->addChild(itemDeep);
    }
};

Model* modelExplorer = createModel<Explorer, ExplorerWidget>("Explorer");
