/**
 * stereo_eq.cpp
 * Hermetic Modular reference DSP firmware: 3-Band Parametric EQ & Diode Saturation.
 * Compile-ready for both VCV Rack and Daisy hardware.
 * 
 * License: MIT
 */

#include "VirtualAlchemyHAL.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace stereo_eq {

struct BiquadFilter {
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;
    float z1 = 0.0f, z2 = 0.0f;

    void SetLowShelf(float freq, float gainDb, float sampleRate) {
        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * (float)M_PI * freq / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / 2.0f * std::sqrt((A + 1.0f / A) * (1.0f / 0.707f - 1.0f) + 2.0f);

        float a0 = (A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * std::sqrt(A) * alpha;
        b0 = (A * ((A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * std::sqrt(A) * alpha)) / a0;
        b1 = (2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosw0)) / a0;
        b2 = (A * ((A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * std::sqrt(A) * alpha)) / a0;
        a1 = (-2.0f * ((A - 1.0f) + (A + 1.0f) * cosw0)) / a0;
        a2 = ((A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * std::sqrt(A) * alpha) / a0;
    }

    void SetPeaking(float freq, float gainDb, float Q, float sampleRate) {
        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * (float)M_PI * freq / sampleRate;
        float alpha = std::sin(w0) / (2.0f * Q);
        float cosw0 = std::cos(w0);

        float a0 = 1.0f + alpha / A;
        b0 = (1.0f + alpha * A) / a0;
        b1 = (-2.0f * cosw0) / a0;
        b2 = (1.0f - alpha * A) / a0;
        a1 = (-2.0f * cosw0) / a0;
        a2 = (1.0f - alpha / A) / a0;
    }

    void SetHighShelf(float freq, float gainDb, float sampleRate) {
        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * (float)M_PI * freq / sampleRate;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / 2.0f * std::sqrt((A + 1.0f / A) * (1.0f / 0.707f - 1.0f) + 2.0f);

        float a0 = (A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * std::sqrt(A) * alpha;
        b0 = (A * ((A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * std::sqrt(A) * alpha)) / a0;
        b1 = (-2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0)) / a0;
        b2 = (A * ((A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * std::sqrt(A) * alpha)) / a0;
        a1 = (2.0f * ((A - 1.0f) - (A + 1.0f) * cosw0)) / a0;
        a2 = ((A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * std::sqrt(A) * alpha) / a0;
    }

    float Process(float in) {
        float out = b0 * in + z1;
        z1 = b1 * in - a1 * out + z2;
        z2 = b2 * in - a2 * out;
        return out;
    }
};

static BiquadFilter lowShelfL, lowShelfR;
static BiquadFilter midPeakL, midPeakR;
static BiquadFilter highShelfL, highShelfR;

void Init(float sampleRate) {
    lowShelfL.SetLowShelf(180.0f, 0.0f, sampleRate);
    lowShelfR.SetLowShelf(180.0f, 0.0f, sampleRate);
    midPeakL.SetPeaking(1200.0f, 0.0f, 1.4f, sampleRate);
    midPeakR.SetPeaking(1200.0f, 0.0f, 1.4f, sampleRate);
    highShelfL.SetHighShelf(6500.0f, 0.0f, sampleRate);
    highShelfR.SetHighShelf(6500.0f, 0.0f, sampleRate);
}

void ProcessBlock(float** in, float** out, size_t size, hermetic::AlchemyHardware& hw) {
    float lowGain = (hw.GetKnobValue(0) - 0.5f) * 36.0f; // -18dB to +18dB
    float midGain = (hw.GetKnobValue(1) - 0.5f) * 36.0f;
    float highGain = (hw.GetKnobValue(2) - 0.5f) * 36.0f;
    float midFreq = 150.0f + std::pow(hw.GetKnobValue(3), 2.2f) * 4850.0f;
    float drive = hw.GetKnobValue(4) * 3.5f;
    float level = hw.GetKnobValue(5) * 1.5f;

    lowShelfL.SetLowShelf(180.0f, lowGain, hw.sample_rate);
    lowShelfR.SetLowShelf(180.0f, lowGain, hw.sample_rate);
    midPeakL.SetPeaking(midFreq, midGain, 1.4f, hw.sample_rate);
    midPeakR.SetPeaking(midFreq, midGain, 1.4f, hw.sample_rate);
    highShelfL.SetHighShelf(6500.0f, highGain, hw.sample_rate);
    highShelfR.SetHighShelf(6500.0f, highGain, hw.sample_rate);

    // Update LED rings
    hw.SetKnobRingArc(0, hw.GetKnobValue(0), 245, 158, 11);
    hw.SetKnobRingArc(1, hw.GetKnobValue(1), 245, 158, 11);
    hw.SetKnobRingArc(2, hw.GetKnobValue(2), 245, 158, 11);
    hw.SetKnobRingArc(3, hw.GetKnobValue(3), 6, 182, 212);
    hw.SetKnobRingArc(4, hw.GetKnobValue(4), 239, 68, 68);
    hw.SetKnobRingArc(5, hw.GetKnobValue(5), 16, 185, 129);

    for (size_t i = 0; i < size; i++) {
        float l = lowShelfL.Process(in[0][i]);
        l = midPeakL.Process(l);
        l = highShelfL.Process(l);

        float r = lowShelfR.Process(in[1][i]);
        r = midPeakR.Process(r);
        r = highShelfR.Process(r);

        // Warm Diode Soft-clipping curve
        if (drive > 0.01f) {
            l = std::tanh(l * (1.0f + drive));
            r = std::tanh(r * (1.0f + drive));
        }

        out[0][i] = l * level;
        out[1][i] = r * level;
    }
}

} // namespace stereo_eq
