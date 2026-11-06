# Explorer (VCV Rack v2 Plugin)

**Explorer** is a 12 HP field-programmable DSP companion module for VCV Rack v2, developed by **Grandsummoner** to match the visual language and companion integration of **Stellar**, **Spaces Command**, **Intel**, and **Beams**.

---

## Features
* **Companion Adjacency Detection**: Dual blue status lights illuminate independently when placed adjacent to a family module (`Spaces Command`, `Intel`, `Beams`, `Stellar`).
* **Clean Hardware Aesthetic**: Cream matte finish, micro-typography, and solid tactile knobs matching Stellar.
* **10 Eurorack Jacks**:
  * 4 Audio Jacks: Stereo In L/R and Stereo Out L/R with 0 dB calibrated unity gain and soft-saturation limiting.
  * 6 Field-Programmable CV Jacks: 17 mm vertical row clearance with high-legibility vector labeling.
* **Dual DSP Engines**:
  * **Stereo Cloud Delay & Reverb**: Cross-feedback ping-pong delay with damping and 4-stage diffusion reverb.
  * **Stereo 3-Band Parametric EQ**: Low shelf, sweepable mid peak, high shelf, and warm analog diode soft-clipping.

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

## License & Acknowledgments
Released under the **GNU General Public License v3.0 or later** (`GPL-3.0-or-later`), matching the VCV Rack SDK licensing standards.
* Built using the **VCV Rack SDK** (GPLv3).
* VCV® is a registered trademark of VCV (https://vcvrack.com).
* Designed by **Grandsummoner** as a companion module to **Stellar**, **Spaces Command**, **Intel**, and **Beams**.
* See `LICENSE` for details.
