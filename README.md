# Hermetic Modular — Explorer (VCV Rack v2 & Daisy Hardware Dual-Target Plugin)

**Explorer** is a 12 HP field-programmable digital Eurorack module emulator and firmware codebase powered by Hermetic Modular's **Alchemy SDK** and the **Electrosmith Daisy** platform.

This repository is dual-target ready:
1. **VCV Rack v2 Plugin**: Compile as a desktop virtual module for Linux, macOS (x64 & ARM64 Apple Silicon), and Windows.
2. **Daisy Seed Hardware Firmware**: Compile for the physical Alchemy Lab / Electrosmith Daisy Seed (STM32H750, 480 MHz ARM Cortex-M7) and flash via USB DFU.

---

## Features
* **102 Addressable RGB LEDs**:
  * 16-LED radial rings encircle each of the 6 main potentiometers (96 LEDs total) with dynamic arc/dot fill, ghost targets, and motion playback animations.
  * 6 status LEDs for page indication and jack modes.
* **Pot Catch (Anti-Jump Protection)**:
  * When swapping pages or loading presets, parameters will not jump until the physical knob position crosses ("catches") the stored preset value.
* **Parameter Lock (Motion Recording)**:
  * Capture real-time knob motion into a cycling circular memory buffer with live LED playback loops.
* **10 Jacks**:
  * 4 Fixed Audio Jacks: Stereo In L/R and Stereo Out L/R.
  * **6 Field-Programmable Dynamic Jacks**: Software-configurable as CV Inputs or CV Outputs with active RGB halos.
* **DSP Engines Included**:
  * **Cloud Delay & Reverb**: Stereo cross-feedback ping-pong delay with damping and lush 4-stage diffusion shimmer reverb.
  * **Stereo 3-Band Parametric EQ**: Low shelf, sweepable mid peak, high shelf, and warm analog diode soft-clipping saturation.

---

## Panel Design & Palette Options
The `res/` directory includes 3 panel design palettes:
1. **`res/Explorer_ObsidianGold.svg` (Default `Explorer.svg`)**:
   * Deep matte anodized black aluminum faceplate with warm alchemical gold accents and white typography.
2. **`res/Explorer_SilverCyan.svg`**:
   * Industrial brushed natural silver aluminum faceplate with electric cyan accents and dark slate typography.
3. **`res/Explorer_MidnightVoid.svg`**:
   * Midnight aerospace deep indigo finish with fluorescent neon magenta/violet rings.

*To change the active panel palette:*
Simply replace `res/Explorer.svg` with your preferred SVG file before compiling:
```bash
cp res/Explorer_SilverCyan.svg res/Explorer.svg
```

---

## Compiling for VCV Rack v2

### Prerequisites
* Download and extract the [VCV Rack v2 SDK](https://vcvrack.com/downloads/) for your operating system.
* A standard C++17 build environment:
  * **macOS**: Xcode Command Line Tools (`xcode-select --install`).
  * **Linux**: `build-essential`, `libgl-dev`, `libglu-dev`, `libasound2-dev`.
  * **Windows**: MSYS2 (Mingw64).

### Build Commands
1. Point `RACK_DIR` to your extracted Rack-SDK:
   ```bash
   export RACK_DIR=/path/to/Rack-SDK
   ```
2. Compile and package the `.vcvplugin` bundle:
   ```bash
   make -j4 dist
   ```
3. Copy the resulting `.vcvplugin` file from the `dist/` folder into your VCV Rack `plugins/` directory:
   * **macOS**: `~/Documents/Rack2/plugins/`
   * **Linux**: `~/.Rack2/plugins/`
   * **Windows**: `Documents/Rack2/plugins/`

---

## Automated GitHub Actions CI Releases

This repository includes `.github/workflows/release.yml`. When you push this repository to GitHub and publish a tag (e.g. `git tag v1.0.0 && git push origin v1.0.0`), GitHub Actions will automatically:
1. Cross-compile the plugin across 4 platforms:
   * **Linux x64**
   * **macOS x64 (Intel)**
   * **macOS ARM64 (Apple Silicon M1/M2/M3/M4)**
   * **Windows x64**
2. Create a GitHub Release and attach the pre-compiled `.vcvplugin` binaries for everyone to download!

---

## Compiling for Physical Hardware (Electrosmith Daisy)

### Prerequisites
* ARM GNU Toolchain (`arm-none-eabi-gcc`).
* `dfu-util` for flashing over USB.
* [libDaisy](https://github.com/electro-smith/libDaisy) and [DaisySP](https://github.com/electro-smith/DaisySP).

### Build Commands
```bash
make -f Makefile.daisy
make -f Makefile.daisy program-dfu
```

---

## License
Released under the **MIT License**, fully compatible with and respecting the Hermetic Modular Alchemy SDK.
See `LICENSE` for details.
