# Grandsummoner Explorer

> **12 HP Field-Programmable Tape-Cloud Delay, Diffusion Reverb & Shimmer Bloom**

<p align="left">
  <img src="res/Explorer.svg" alt="Grandsummoner Explorer Front Panel" width="380" />
</p>

---

## Overview

**Explorer** is a 12 HP spatial effects module for [VCV Rack v2](https://vcvrack.com/), combining an analog-modeled tape delay, an expansive diffusion reverb network, and an octave-up pitch-shifted shimmer bloom. Built around tape-loop physics and dense multi-tap delay networks, Explorer delivers everything from subtle vintage tape warmth and slapback echoes to infinite frozen ambient soundscapes.

Explorer is engineered for hands-on live performance and deep modular generative patches. It features genuine tape-motor inertia emulation (smoothing delay-time modulation into authentic mechanical pitch glides), dedicated CV inputs for every primary axis, and a trio of tactile latching performance buttons.

---

## The Grandsummoner Module Family

Explorer is a core member of the **Grandsummoner Eurorack Family**, designed to form a complete, unified modular synthesis ecosystem:

| Module | Width | Primary Function | Role in Ecosystem |
| :--- | :--- | :--- | :--- |
| **Spaces Command** | 38 HP | Dual-Scene Probabilistic CV/Gate Sequencer & Arpeggiator | Master Composition & Clock Brain |
| **Beams** | 18 HP | Quad Algorithmic Euclidean Drum Engine & Sampler | Rhythm, Percussion & Clock Sync |
| **Stellar** | 14 HP | Dual-Voice Multi-Model Synthesizer Companion | Primary Melodic & Harmonic Voice |
| **Intel** | 14 HP | Modulation Matrix, Dual FX Engine & Metering Ladder | Utility, Inter-Module Modulation & Modulation Router |
| **Explorer** | 12 HP | Tape-Cloud Delay, Diffusion Reverb & Octave Shimmer | Master Spatialization, Diffusion & Ambient Textures |

### Shared Design Language
Every Grandsummoner module shares an elegant vintage-laboratory aesthetic:
- **Warm Sand Cards** (`#F5F1E9` top gradient down to `#EDE7DC`) framed by hairline `0.4–0.5pt` bronze borders (`#8A7F6A`).
- **Deep Obsidian Knobs** with ivory indicator lines (`#E8E0D0`).
- **Tactile Color-Coded Performance Buttons**: Navy/Teal (`#2E4A6E`) for stereo imaging, Maroon (`#8A2A2A`) for freeze/muting, and Brass (`#8A6423`) for harmonic enrichment.
- **Center-Detent Micro-Dots** on bipolar knobs (`WIDTH`, `COLOR`) for instant zero-position alignment.

### Standalone vs. Family Operation

- **100% Standalone**: Explorer operates completely independently. All audio I/O and CV jacks adhere to standard Eurorack electrical specifications. Patch any external audio into `IN L/R` and modulate parameters using standard 0–10V or bipolar ±5V CV.
- **Universal Adjacency Link**: When placed directly adjacent to any Grandsummoner family module (**Spaces Command**, **Intel**, **Beams**, or **Stellar**) on either the left or right side, Explorer's side connector LEDs (`LINK L` and `LINK R`) automatically illuminate, confirming physical enclosure alignment.

### Cross-Module Synergies
- **With Stellar**: Send Stellar's `MASTER L/R` outputs directly into Explorer's `IN L/R`. Engaging `SHIMMER` and `FREEZE` transforms Stellar's Moog-style ladder filter plucks into sustained orchestral pads.
- **With Spaces Command**: Modulate Explorer's `TIME CV` and `REGEN CV` from Spaces Command's Voice 2 auxiliary pitch and gate outputs for rhythmic delay flutter locked to sequence transitions.
- **With Intel**: When Intel's modulation channels are active, use Intel's `TRIG 1–4` outputs to trigger Explorer's `FREEZE` or `SHIMMER` CV jacks on selected rhythmic beats.
- **With Beams**: Route Beams' stereo drum output into Explorer with a subtle `WIDTH` boost and 15% `MIX` for tape-warmed studio room reverberation.

---

## User Guide & Controls

Explorer is organized into three intuitive functional cards: **TIME / REGEN**, **SPACE / REVERB**, and **I/O**.

```
┌───────────────────────────────────────────────┐
│              EXPLORER  12 HP                  │
│                                               │
│  [TIME]       [REGEN]       [MIX]             │
│   delay time   feedback      dry/wet          │
│                                               │
│  [STEREO]     [FREEZE]      [SHIMMER]         │
│   width btn    buffer lock   octave bloom     │
│                                               │
│  [SIZE]       [DECAY]       [SHIMMER LVL]     │
│   room size    tail length   pitch amount     │
│                                               │
│  [WIDTH]      [COLOR]       [FLUTTER]         │
│   stereo spread tone damp    tape wobble      │
│                                               │
│  (TIME)       (REGEN)       (MIX)      (DAMP) │
│   cv input     cv input      cv input   cv in │
│                                               │
│  (FREEZE)     (SHIM)        [IN L]     [IN R] │
│   gate cv      gate cv       audio in   audio │
│                                               │
│               [OUT L]       [OUT R]           │
│                audio out     audio out        │
└───────────────────────────────────────────────┘
```

### 1. Delay & Master Controls (Top Card)
- **TIME (10 ms – 2.5 s)**: Sets the primary delay delay-line length. Equipped with a modeled **analog tape motor inertia slew filter**: twisting the knob creates natural pitch ramps and Doppler warping rather than digital clicks or zipper noise.
- **REGEN (0% – 115%)**: Adjusts feedback repeats. Beyond 100% (~4 o'clock), the feedback path enters soft-clipped analog self-oscillation with gentle tape saturation.
- **MIX (0% – 100%)**: Continuous dry/wet blend. At 0%, audio passes dry with zero latency; at 100%, only the processed delay/reverb wet signal is heard.

### 2. Performance Latching Buttons
- **STEREO Button (Navy/Teal `#2E4A6E`)**: Toggles the stereo spatial width engine. When unlit, the delay runs in true dual-mono; when lit, the L/R delay lines cross-modulate for immersive spatial imaging.
- **FREEZE Button (Maroon `#8A2A2A`)**: Instantly locks the current audio recirculation buffer into an infinite sustained loop. While active, input signal can continue to play over the top without overwriting the frozen pad.
- **SHIMMER Button (Brass `#8A6423`)**: Engages the granular pitch-shifting feedback network (+1 octave), injecting shimmering harmonic overtones into the diffusion tank.

### 3. Space & Diffusion Controls (Middle Card)
- **SIZE (0% – 100%)**: Controls the virtual acoustic room dimensions and diffusion density of the all-pass filter network.
- **DECAY (0.2 s – 30 s)**: Sets the reverb tail decay time. At maximum settings, tails persist for over half a minute.
- **SHIMMER LEVEL (0% – 100%)**: Sets the amplitude of the +1 octave harmonic signal fed back into the reverberation tank.
- **WIDTH (Bipolar: 0% Mono to 200% Wide)**: Center-detented knob. Turn left for focused mono compatibility; turn right to widen the psychoacoustic stereo field.
- **COLOR (Bipolar High-Frequency Damping)**: Center-detented knob. Turn left for warm, dark 1970s tape dampening; turn right for airy, crystalline reflections.
- **FLUTTER (0% – 100%)**: Introduces mechanical capstan wow and flutter, simulating tape tension variations.

### 4. Patch Jacks & CV Modulation (Bottom Card)
- **TIME CV Input (±5V)**: External voltage control over delay time, additive to the TIME knob.
- **REGEN CV Input (±5V)**: Voltage control over feedback amount.
- **MIX CV Input (0–10V)**: Voltage control over dry/wet blend.
- **DAMP CV Input (±5V)**: Voltage control over the COLOR / high-frequency absorption filter.
- **FREEZE CV Input (0–10V Trigger / Gate)**: Toggles buffer freeze on rising gate edges.
- **SHIMMER CV Input (0–10V Trigger / Gate)**: Toggles shimmer mode on rising gate edges.
- **IN L / IN R**: High-impedance audio inputs. If only `IN L` is patched, the signal normals to both channels.
- **OUT L / OUT R**: Low-impedance audio outputs. **Intelligent Mono Auto-Summing**: If `OUT R` is unpatched, `OUT L` automatically sums both stereo channels using constant-power `0.707` gain compensation to prevent clipping.

---

## Technical Specifications

- **Width**: 12 HP (60.96 mm)
- **Height**: 3U (128.5 mm physical standard / bit-for-bit 380px VCV grid height)
- **Depth**: 30 mm
- **Internal DSP**: 64-bit double-precision floating-point signal path
- **Sample Rates Supported**: 44.1 kHz, 48 kHz, 88.2 kHz, 96 kHz, 192 kHz
- **Delay Range**: 10 ms to 2,500 ms (2.5 seconds)
- **Dynamic Range**: > 110 dB

---

## Building and Installation

### Prebuilt Plugin Package
Download the latest `Explorer-2.0.0-<platform>.vcvplugin` from the repository's [Releases](https://github.com/Grandsummoner/explorer/releases) section and extract it into your VCV Rack `plugins` directory:
- **Windows**: `Documents/Rack2/plugins/`
- **macOS**: `Documents/Rack2/plugins/`
- **Linux**: `~/.Rack2/plugins/`

### Building from Source
Ensure you have the [VCV Rack SDK v2](https://vcvrack.com/downloads/) installed.

```bash
# Clone the repository
git clone https://github.com/Grandsummoner/explorer.git
cd explorer

# Set your Rack SDK directory
export RACK_DIR=/path/to/Rack-SDK

# Build the distributable package
make -j2 dist
```

The compiled package will be located in `dist/Explorer-2.0.0-<platform>.vcvplugin`.

---

## License

This software is released under the **GNU General Public License v3.0 or later** (GPL-3.0-or-later).  
Copyright © 2026 Grandsummoner. All rights reserved.
