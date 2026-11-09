/**
 * cloud_delay.cpp
 * Explorer: Pristine, Hi-Fi Spatial Cloud Delay & Reverb with Stereo Width & Shimmer.
 * Zero distortion / zero overdrive.
 */

#include "Explorer.hpp"
#include <cmath>
#include <cstring>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace explorer_dsp {

static constexpr size_t MAX_DELAY = 192000; // 4.0 seconds @ 48kHz
static float delayBufL[MAX_DELAY] = {};
static float delayBufR[MAX_DELAY] = {};
static size_t writeIdx = 0;

// Reverb allpass diffusers
struct Allpass {
    float buf[4096] = {};
    size_t idx = 0;
    size_t size = 1000;
    float fb = 0.55f;

    void setSize(size_t s, float f = 0.55f) {
        size = (s < 4096) ? s : 4095;
        fb = f;
    }

    float process(float in) {
        float out = buf[idx];
        buf[idx] = in + out * fb;
        idx = (idx + 1) % size;
        return -in + out;
    }
};

static Allpass diffL[4];
static Allpass diffR[4];

// One-pole damping filters
static float dampL = 0.f;
static float dampR = 0.f;

// Flutter LFO phases
static float lfoPhase1 = 0.f;
static float lfoPhase2 = 0.f;

// Octave Shimmer pitch-shifter buffers (grain pitch shift +12 semitones)
static constexpr size_t SHIMMER_BUF = 4096;
static float shimBufL[SHIMMER_BUF] = {};
static float shimBufR[SHIMMER_BUF] = {};
static size_t shimWrite = 0;
static float shimRead = 0.f;

// Tilt EQ one-pole state
static float tiltStateL = 0.f;
static float tiltStateR = 0.f;

// Tape motor inertia state (smooth analog delay time slewing)
static float smoothedDelayL = 4800.f;
static float smoothedDelayR = 4800.f;

void Init(float sampleRate) {
    (void)sampleRate;
    std::memset(delayBufL, 0, sizeof(delayBufL));
    std::memset(delayBufR, 0, sizeof(delayBufR));
    std::memset(shimBufL, 0, sizeof(shimBufL));
    std::memset(shimBufR, 0, sizeof(shimBufR));
    writeIdx = 0;
    dampL = dampR = 0.f;
    lfoPhase1 = lfoPhase2 = 0.f;
    shimWrite = 0;
    shimRead = 0.f;
    tiltStateL = tiltStateR = 0.f;
    smoothedDelayL = smoothedDelayR = 4800.f;

    // Mutually prime delays for dense, non-metallic spatial diffusion
    diffL[0].setSize(251, 0.60f);
    diffL[1].setSize(641, 0.58f);
    diffL[2].setSize(1237, 0.55f);
    diffL[3].setSize(1987, 0.52f);

    diffR[0].setSize(269, 0.60f);
    diffR[1].setSize(619, 0.58f);
    diffR[2].setSize(1279, 0.55f);
    diffR[3].setSize(1933, 0.52f);
}

// Hermite cubic interpolation for silky smooth tape-speed transitions
static inline float readDelayCubic(const float* buffer, float readPos) {
    int iPos = static_cast<int>(readPos);
    float frac = readPos - iPos;

    int idx0 = (iPos - 1 + MAX_DELAY) % MAX_DELAY;
    int idx1 = (iPos + MAX_DELAY) % MAX_DELAY;
    int idx2 = (iPos + 1 + MAX_DELAY) % MAX_DELAY;
    int idx3 = (iPos + 2 + MAX_DELAY) % MAX_DELAY;

    float y0 = buffer[idx0];
    float y1 = buffer[idx1];
    float y2 = buffer[idx2];
    float y3 = buffer[idx3];

    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

    return ((c3 * frac + c2) * frac + c1) * frac + c0;
}

void ProcessSample(float inL, float inR, float& outL, float& outR, const EngineState& st, float sampleRate) {
    if (sampleRate <= 0.f) sampleRate = 48000.f;

    // 1. Wow & Flutter tape modulation (pure subtle pitch drift, zero distortion)
    float lfo1 = std::sin(lfoPhase1);
    float lfo2 = std::sin(lfoPhase2);
    lfoPhase1 += 2.0f * (float)M_PI * 0.65f / sampleRate;
    lfoPhase2 += 2.0f * (float)M_PI * 2.15f / sampleRate;
    if (lfoPhase1 > 2.0f * (float)M_PI) lfoPhase1 -= 2.0f * (float)M_PI;
    if (lfoPhase2 > 2.0f * (float)M_PI) lfoPhase2 -= 2.0f * (float)M_PI;

    float flutterOffset = (lfo1 * 0.7f + lfo2 * 0.3f) * (st.flutter * 0.0035f * sampleRate);

    // 2. Delay Time Mapping (Normal: 0.05s to 1.5s; Deep: up to 4.0s)
    float maxSec = (st.delayRangeMode == 1) ? 3.9f : 1.45f;
    float timeSec = 0.05f + std::pow(st.time, 1.6f) * maxSec;
    float delaySamplesL = timeSec * sampleRate + flutterOffset;
    float delaySamplesR = st.stereoPingPong 
        ? (timeSec * 1.333f * sampleRate - flutterOffset)
        : (timeSec * sampleRate - flutterOffset);

    float targetSamplesL = rack::math::clamp(delaySamplesL, 10.f, (float)(MAX_DELAY - 5));
    float targetSamplesR = rack::math::clamp(delaySamplesR, 10.f, (float)(MAX_DELAY - 5));

    // Analog tape motor inertia (1-pole slew rate)
    // ~16 Hz tracking bandwidth yields rich musical Doppler pitch transitions
    float timeSlew = 1.0f - std::exp(-2.0f * (float)M_PI * 16.0f / sampleRate);
    smoothedDelayL += timeSlew * (targetSamplesL - smoothedDelayL);
    smoothedDelayR += timeSlew * (targetSamplesR - smoothedDelayR);

    float readPosL = (float)writeIdx - smoothedDelayL;
    if (readPosL < 0.f) readPosL += MAX_DELAY;
    float readPosR = (float)writeIdx - smoothedDelayR;
    if (readPosR < 0.f) readPosR += MAX_DELAY;

    float dlyL = readDelayCubic(delayBufL, readPosL);
    float dlyR = readDelayCubic(delayBufR, readPosR);

    // 3. Gentle Damping (High-frequency air absorption)
    float dampAlpha = rack::math::clamp(1.0f - st.damping * 0.75f, 0.05f, 0.99f);
    dampL += dampAlpha * (dlyL - dampL);
    dampR += dampAlpha * (dlyR - dampR);

    // 4. Freeze & Feedback Loop
    float inGain = st.freeze ? 0.0f : 1.0f;
    float fb = st.freeze ? 1.0f : std::min(0.96f, st.feedback * 0.98f);

    if (st.stereoPingPong) {
        // Cross-feedback ping-pong
        delayBufL[writeIdx] = inL * inGain + dampR * fb;
        delayBufR[writeIdx] = inR * inGain + dampL * fb;
    } else {
        // Dual parallel delay
        delayBufL[writeIdx] = inL * inGain + dampL * fb;
        delayBufR[writeIdx] = inR * inGain + dampR * fb;
    }

    // 5. Octave Shimmer Generator (clean granular 2x playback pitch shift)
    shimBufL[shimWrite] = inL * 0.4f + dampL * 0.6f;
    shimBufR[shimWrite] = inR * 0.4f + dampR * 0.6f;
    shimWrite = (shimWrite + 1) % SHIMMER_BUF;

    shimRead += 2.0f; // 2x speed for +1 octave
    if (shimRead >= (float)SHIMMER_BUF) shimRead -= (float)SHIMMER_BUF;
    float shimL = shimBufL[static_cast<size_t>(shimRead) % SHIMMER_BUF];
    float shimR = shimBufR[static_cast<size_t>(shimRead) % SHIMMER_BUF];

    // 6. 4-Stage Diffusion Reverb Network
    float revInL = (inL * 0.4f + dlyL * st.delayMix * 0.6f) * st.diffusion;
    float revInR = (inR * 0.4f + dlyR * st.delayMix * 0.6f) * st.diffusion;
    if (st.shimmer) {
        revInL += shimL * 0.45f;
        revInR += shimR * 0.45f;
    }

    float vL = revInL;
    float vR = revInR;
    for (int d = 0; d < 4; d++) {
        vL = diffL[d].process(vL);
        vR = diffR[d].process(vR);
    }

    // 7. Space Bus Mixing (Calibrated 0 dB unity gain)
    float dryGain = 1.0f - st.delayMix * 0.45f;
    float dlyGain = st.delayMix * 0.55f;
    float revGain = st.reverbMix * 0.50f;

    float mixedL = inL * dryGain + dlyL * dlyGain + vL * revGain;
    float mixedR = inR * dryGain + dlyR * dlyGain + vR * revGain;

    // 8. SCULPT: Pristine Gentle Tilt EQ (Color)
    // Low-pass one-pole filter at ~850 Hz
    float tiltFreqAlpha = 0.09f;
    tiltStateL += tiltFreqAlpha * (mixedL - tiltStateL);
    tiltStateR += tiltFreqAlpha * (mixedR - tiltStateR);

    // Color: 0.0 = warm dark bass, 0.5 = flat, 1.0 = crystalline airy high sheen
    float tiltOffset = (st.color - 0.5f) * 1.5f;
    float highComponentL = mixedL - tiltStateL;
    float highComponentR = mixedR - tiltStateR;

    float sculptedL = mixedL + highComponentL * tiltOffset - tiltStateL * (tiltOffset * 0.5f);
    float sculptedR = mixedR + highComponentR * tiltOffset - tiltStateR * (tiltOffset * 0.5f);

    // 9. SCULPT: Mid/Side Stereo Panorama Expander (Width)
    float mid = (sculptedL + sculptedR) * 0.5f;
    float side = (sculptedL - sculptedR) * 0.5f;
    float widthFactor = st.width * 2.0f; // 0.0 = mono, 1.0 = normal stereo, 2.0 = ultra-wide

    outL = mid + side * widthFactor;
    outR = mid - side * widthFactor;

    // Advance circular delay index
    writeIdx = (writeIdx + 1) % MAX_DELAY;
}

} // namespace explorer_dsp
