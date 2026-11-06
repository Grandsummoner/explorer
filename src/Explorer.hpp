#pragma once
#include "plugin.hpp"
#include <vector>
#include <cmath>

namespace explorer_dsp {
    struct EngineState {
        // Space parameters (Engine 1)
        float time = 0.35f;
        float feedback = 0.45f;
        float diffusion = 0.60f;
        float damping = 0.50f;
        float delayMix = 0.50f;
        float reverbMix = 0.40f;

        // Sculpt parameters (Engine 2)
        float width = 0.50f;
        float color = 0.50f;
        float flutter = 0.20f;

        // Latching toggles
        bool stereoPingPong = true;
        bool freeze = false;
        bool shimmer = false;

        // Context menu options
        int delayRangeMode = 0; // 0 = Normal (50ms - 1.5s), 1 = Deep Space (up to 4.0s)
    };

    void Init(float sampleRate);
    void ProcessSample(float inL, float inR, float& outL, float& outR, const EngineState& st, float sampleRate);
}

struct Explorer : Module {
    enum ParamIds {
        // SPACE card (6 Knobs)
        TIME_PARAM,
        FDBK_PARAM,
        DIFF_PARAM,
        DAMP_PARAM,
        DMIX_PARAM,
        RMIX_PARAM,

        // SCULPT card (3 Knobs)
        WIDTH_PARAM,
        COLOR_PARAM,
        FLUTTER_PARAM,

        // CTRL card (3 Pushbuttons)
        STEREO_PARAM,
        FREEZE_PARAM,
        SHIMMER_PARAM,

        PARAMS_LEN
    };

    enum InputIds {
        AUDIO_IN_L,
        AUDIO_IN_R,

        // FIELD card (6 CV Inputs)
        TIME_CV_INPUT,
        FDBK_CV_INPUT,
        DIFF_CV_INPUT,
        RMIX_CV_INPUT,
        WIDTH_CV_INPUT,
        FLUTTER_CV_INPUT,

        INPUTS_LEN
    };

    enum OutputIds {
        AUDIO_OUT_L,
        AUDIO_OUT_R,

        OUTPUTS_LEN
    };

    enum LightIds {
        LINK_LEFT_LIGHT,
        LINK_RIGHT_LIGHT,
        STEREO_LIGHT,
        FREEZE_LIGHT,
        SHIMMER_LIGHT,

        LIGHTS_LEN
    };

    explorer_dsp::EngineState dspState;

    // Latch states
    bool isStereo = true;
    bool isFrozen = false;
    bool isShimmer = false;

    // Button triggers
    dsp::BooleanTrigger stereoTrigger;
    dsp::BooleanTrigger freezeTrigger;
    dsp::BooleanTrigger shimmerTrigger;

    Explorer();
    void process(const ProcessArgs& args) override;
    json_t* dataToJson() override;
    void dataFromJson(json_t* rootJ) override;
};
