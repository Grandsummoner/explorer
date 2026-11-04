/**
 * VirtualAlchemyHAL.h
 * Hardware Abstraction Layer for Hermetic Modular Alchemy SDK in VCV Rack v2.
 * Allows firmware written for Alchemy Lab / Electrosmith Daisy to run natively in VCV Rack.
 * 
 * License: MIT (Matching Alchemy SDK)
 */

#pragma once
#include <vector>
#include <cstdint>
#include <cmath>
#include <cstring>

namespace hermetic {

enum JackMode {
    JACK_MODE_IN = 0,
    JACK_MODE_OUT = 1
};

struct AlchemyHardware {
    // 6 Main Rotary Potentiometers
    float knobs[6] = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    // 3 Tactile Pushbuttons
    bool buttons[3] = {false, false, false};
    
    // 10 Jacks: 2 Audio In, 2 Audio Out, 6 Field-Programmable
    float audio_in[2] = {0.0f, 0.0f};
    float audio_out[2] = {0.0f, 0.0f};
    float field_cv_in[6] = {0.0f};
    float field_cv_out[6] = {0.0f};
    JackMode field_jack_mode[6] = {
        JACK_MODE_OUT, JACK_MODE_OUT,
        JACK_MODE_IN,  JACK_MODE_IN,
        JACK_MODE_IN,  JACK_MODE_IN
    };
    
    // 102 Addressable RGB LEDs (RGB packed 0x00RRGGBB)
    // 16 LEDs per knob ring * 6 knobs = 96 LEDs + 6 status LEDs = 102 LEDs
    uint32_t leds[102] = {0};

    // System sample rate
    float sample_rate = 48000.0f;

    void Init(float sr = 48000.0f) {
        sample_rate = sr;
        std::memset(leds, 0, sizeof(leds));
    }

    float SampleRate() const {
        return sample_rate;
    }

    // High-level Alchemy Framework controls
    float GetKnobValue(int index) const {
        if (index >= 0 && index < 6) return knobs[index];
        return 0.0f;
    }

    bool GetButton(int index) const {
        if (index >= 0 && index < 3) return buttons[index];
        return false;
    }

    void SetLed(int index, uint8_t r, uint8_t g, uint8_t b) {
        if (index >= 0 && index < 102) {
            leds[index] = (static_cast<uint32_t>(r) << 16) |
                          (static_cast<uint32_t>(g) << 8)  |
                          static_cast<uint32_t>(b);
        }
    }

    // Set one of the 6 Knob Rings (16 LEDs per ring)
    void SetKnobRingArc(int knobIdx, float fillRatio, uint8_t r, uint8_t g, uint8_t b) {
        if (knobIdx < 0 || knobIdx >= 6) return;
        int startLed = knobIdx * 16;
        int activeCount = static_cast<int>(std::round(fillRatio * 15.0f));
        for (int i = 0; i < 16; i++) {
            if (i <= activeCount) {
                SetLed(startLed + i, r, g, b);
            } else {
                SetLed(startLed + i, 6, 6, 8); // subtle unlit halo
            }
        }
    }

    // Pot Catch helper
    bool CheckPotCatch(int knobIdx, float physicalValue, float storedPresetValue, float threshold = 0.04f) {
        return std::abs(physicalValue - storedPresetValue) <= threshold;
    }
};

} // namespace hermetic
