/**
 * cloud_delay.cpp
 * Hermetic Modular reference DSP firmware: Stereo Ping-Pong Delay & Lush Diffusion Reverb.
 * Compile-ready for both VCV Rack and Daisy hardware.
 * 
 * License: MIT
 */

#include "VirtualAlchemyHAL.h"
#include <cmath>
#include <vector>

namespace cloud_delay {

static constexpr size_t MAX_DELAY_SAMPLES = 96000; // 2 seconds @ 48kHz
static float delayBufferL[MAX_DELAY_SAMPLES] = {};
static float delayBufferR[MAX_DELAY_SAMPLES] = {};
static size_t writeIdxL = 0;
static size_t writeIdxR = 0;

// Filter state for damping
static float dampFilterL = 0.0f;
static float dampFilterR = 0.0f;

// Simple all-pass diffuser network for reverb bloom
struct AllpassFilter {
    float buffer[4096] = {};
    size_t idx = 0;
    size_t size = 1000;
    float feedback = 0.5f;

    void SetSize(size_t s, float fb = 0.6f) {
        size = (s < 4096) ? s : 4095;
        feedback = fb;
    }

    float Process(float in) {
        float bufOut = buffer[idx];
        float out = -in + bufOut;
        buffer[idx] = in + (bufOut * feedback);
        idx = (idx + 1) % size;
        return out;
    }
};

static AllpassFilter diffusersL[4];
static AllpassFilter diffusersR[4];

void Init(float sampleRate) {
    (void)sampleRate;
    std::memset(delayBufferL, 0, sizeof(delayBufferL));
    std::memset(delayBufferR, 0, sizeof(delayBufferR));
    writeIdxL = 0;
    writeIdxR = 0;
    dampFilterL = 0.0f;
    dampFilterR = 0.0f;

    // Prime numbers for natural non-metallic diffusion
    diffusersL[0].SetSize(223, 0.65f);
    diffusersL[1].SetSize(563, 0.62f);
    diffusersL[2].SetSize(1151, 0.58f);
    diffusersL[3].SetSize(1787, 0.55f);

    diffusersR[0].SetSize(239, 0.65f);
    diffusersR[1].SetSize(547, 0.62f);
    diffusersR[2].SetSize(1187, 0.58f);
    diffusersR[3].SetSize(1753, 0.55f);
}

void ProcessBlock(float** in, float** out, size_t size, hermetic::AlchemyHardware& hw) {
    float delayTimeSec = 0.06f + std::pow(hw.GetKnobValue(0), 1.5f) * 1.1f;
    float feedback = std::min(0.93f, hw.GetKnobValue(1) * 0.95f);
    float delayMix = hw.GetKnobValue(2);
    float reverbSize = hw.GetKnobValue(3);
    float damping = 0.2f + hw.GetKnobValue(4) * 0.75f;
    float reverbMix = hw.GetKnobValue(5);

    size_t delaySamplesL = static_cast<size_t>(delayTimeSec * hw.sample_rate);
    size_t delaySamplesR = static_cast<size_t>(delayTimeSec * 1.38f * hw.sample_rate);
    if (delaySamplesL >= MAX_DELAY_SAMPLES) delaySamplesL = MAX_DELAY_SAMPLES - 1;
    if (delaySamplesR >= MAX_DELAY_SAMPLES) delaySamplesR = MAX_DELAY_SAMPLES - 1;

    // Update LED rings: Cyan for Delay, Purple for Reverb
    hw.SetKnobRingArc(0, hw.GetKnobValue(0), 6, 182, 212);
    hw.SetKnobRingArc(1, hw.GetKnobValue(1), 6, 182, 212);
    hw.SetKnobRingArc(2, hw.GetKnobValue(2), 6, 182, 212);
    hw.SetKnobRingArc(3, hw.GetKnobValue(3), 217, 70, 239);
    hw.SetKnobRingArc(4, hw.GetKnobValue(4), 217, 70, 239);
    hw.SetKnobRingArc(5, hw.GetKnobValue(5), 217, 70, 239);

    for (size_t i = 0; i < size; i++) {
        float inL = in[0][i];
        float inR = in[1][i];

        // Read delay outputs
        size_t readIdxL = (writeIdxL + MAX_DELAY_SAMPLES - delaySamplesL) % MAX_DELAY_SAMPLES;
        size_t readIdxR = (writeIdxR + MAX_DELAY_SAMPLES - delaySamplesR) % MAX_DELAY_SAMPLES;
        float delayedL = delayBufferL[readIdxL];
        float delayedR = delayBufferR[readIdxR];

        // Cross-feedback ping-pong with damping
        dampFilterL += damping * (delayedR - dampFilterL);
        dampFilterR += damping * (delayedL - dampFilterR);

        delayBufferL[writeIdxL] = inL + dampFilterL * feedback;
        delayBufferR[writeIdxR] = inR + dampFilterR * feedback;

        writeIdxL = (writeIdxL + 1) % MAX_DELAY_SAMPLES;
        writeIdxR = (writeIdxR + 1) % MAX_DELAY_SAMPLES;

        // Diffusion reverb stage with normalized send
        float revSendL = (inL * 0.5f + delayedL * 0.5f) * reverbSize;
        float revSendR = (inR * 0.5f + delayedR * 0.5f) * reverbSize;
        float verbL = revSendL;
        float verbR = revSendR;
        for (int d = 0; d < 4; d++) {
            verbL = diffusersL[d].Process(verbL);
            verbR = diffusersR[d].Process(verbR);
        }

        // Balanced dry/wet gain staging calibrated for unity (0 dB) output
        float dryAmount = 1.0f - delayMix * 0.5f;
        float delayWetAmount = delayMix * 0.6f;
        float delayMixedL = inL * dryAmount + delayedL * delayWetAmount;
        float delayMixedR = inR * dryAmount + delayedR * delayWetAmount;

        // Reverb blend
        float finalL = delayMixedL * (1.0f - reverbMix * 0.35f) + verbL * (reverbMix * 0.5f);
        float finalR = delayMixedR * (1.0f - reverbMix * 0.35f) + verbR * (reverbMix * 0.5f);

        // Musical soft limiter to prevent clipping and keep output strictly within Eurorack limits
        out[0][i] = std::tanh(finalL);
        out[1][i] = std::tanh(finalR);
    }
}

} // namespace cloud_delay
