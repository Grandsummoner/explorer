/**
 * main_daisy.cpp
 * Electrosmith Daisy Seed embedded entry point for Hermetic Modular "Explorer".
 * 
 * Target: STM32H750 (480 MHz ARM Cortex-M7)
 * License: MIT
 */

#include "daisy_seed.h"
#include "daisysp.h"
#include "VirtualAlchemyHAL.h"

using namespace daisy;
using namespace daisysp;

static DaisySeed hw;
static hermetic::AlchemyHardware alchemyHw;

namespace cloud_delay {
    void Init(float sampleRate);
    void ProcessBlock(float** in, float** out, size_t size, hermetic::AlchemyHardware& hw);
}

namespace stereo_eq {
    void Init(float sampleRate);
    void ProcessBlock(float** in, float** out, size_t size, hermetic::AlchemyHardware& hw);
}

static float inBuffers[2][32];
static float outBuffers[2][32];

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    // Fill buffers
    for (size_t i = 0; i < size; i++) {
        inBuffers[0][i] = in[0][i];
        inBuffers[1][i] = in[1][i];
    }

    float* inPtrs[2] = {inBuffers[0], inBuffers[1]};
    float* outPtrs[2] = {outBuffers[0], outBuffers[1]};

    // Run DSP
    cloud_delay::ProcessBlock(inPtrs, outPtrs, size, alchemyHw);

    // Write out
    for (size_t i = 0; i < size; i++) {
        out[0][i] = outBuffers[0][i];
        out[1][i] = outBuffers[1][i];
    }
}

int main(void) {
    hw.Init();
    hw.SetAudioBlockSize(32);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    alchemyHw.Init(hw.AudioSampleRate());
    cloud_delay::Init(hw.AudioSampleRate());
    stereo_eq::Init(hw.AudioSampleRate());

    hw.StartAudio(AudioCallback);

    while (1) {
        // Main UI / LED refresh loop
    }
}
