#pragma once
#include "plugin.hpp"
#include "VirtualAlchemyHAL.h"

namespace stereo_eq {
    void Init(float sampleRate);
    void ProcessBlock(float** in, float** out, size_t size, hermetic::AlchemyHardware& hw);
}

namespace cloud_delay {
    void Init(float sampleRate);
    void ProcessBlock(float** in, float** out, size_t size, hermetic::AlchemyHardware& hw);
}

struct Explorer : Module {
    enum ParamIds {
        KNOB_1,
        KNOB_2,
        KNOB_3,
        KNOB_4,
        KNOB_5,
        KNOB_6,
        BTN_PAGE,
        BTN_LOCK,
        BTN_PRESET,
        NUM_PARAMS
    };
    enum InputIds {
        AUDIO_IN_L,
        AUDIO_IN_R,
        FIELD_IN_1,
        FIELD_IN_6 = FIELD_IN_1 + 5,
        NUM_INPUTS
    };
    enum OutputIds {
        AUDIO_OUT_L,
        AUDIO_OUT_R,
        FIELD_OUT_1,
        FIELD_OUT_6 = FIELD_OUT_1 + 5,
        NUM_OUTPUTS
    };
    enum LightIds {
        // 102 RGB LEDs: 96 knob ring LEDs + 6 status LEDs (3 channels per LED: R, G, B)
        LEDS_START,
        NUM_LIGHTS = LEDS_START + (102 * 3)
    };

    hermetic::AlchemyHardware hw;

    // Active DSP mode: 0 = Cloud Delay & Reverb, 1 = Stereo 3-Band EQ
    int activeMode = 0;

    // 32-sample block buffer matching Daisy audio callbacks
    static constexpr size_t BLOCK_SIZE = 32;
    float blockInL[BLOCK_SIZE] = {};
    float blockInR[BLOCK_SIZE] = {};
    float blockOutL[BLOCK_SIZE] = {};
    float blockOutR[BLOCK_SIZE] = {};
    size_t blockIdx = BLOCK_SIZE;

    // Pot Catch and Motion Recording state
    bool isCaught[6] = {true, true, true, true, true, true};
    float storedPreset[6] = {0.4f, 0.6f, 0.65f, 0.7f, 0.75f, 0.65f};
    bool isLocked[6] = {false};
    std::vector<float> motionBuffer[6];
    size_t motionPlaybackIdx[6] = {};

    Explorer();
    void process(const ProcessArgs& args) override;
    json_t* dataToJson() override;
    void dataFromJson(json_t* rootJ) override;
};
