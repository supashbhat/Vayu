# Vayu

![Vayu Icon](Assets/Vayu_Icon.png)

Vayu is a modular multi-effect plugin for movement, color, width, and atmosphere. It combines a five-band EQ with a drag-to-reorder rack of modulation, space, tone-shaping, and dynamics tools inside a single DAW-first interface.

## Overview

Vayu includes:

- Five-band EQ with live curve feedback
- Distortion
- Chorus
- Flanger
- Phaser
- Delay
- Stereo imaging
- Reverb
- Compressor
- Drag-to-reorder signal chain
- Factory presets and user presets

## Downloads

Official nightly assets are published from the main branch.

- `Vayu-macOS-Installer.pkg`
- `Vayu-Windows-Installer.exe`
- `Vayu-macOS-AU.zip`
- `Vayu-macOS-VST3.zip`
- `Vayu-Windows-VST3.zip`
- `Vayu-Linux-VST3.tar.gz`

## Installation

### macOS

Recommended:

1. Download `Vayu-macOS-Installer.pkg`.
2. Run the installer.
3. Rescan plugins in your DAW if needed.

Manual paths:

- AU: `/Library/Audio/Plug-Ins/Components`
- VST3: `/Library/Audio/Plug-Ins/VST3`

User-level alternatives:

- AU: `~/Library/Audio/Plug-Ins/Components`
- VST3: `~/Library/Audio/Plug-Ins/VST3`

### Windows

Recommended:

1. Download `Vayu-Windows-Installer.exe`.
2. Run the installer.
3. Rescan plugins in your DAW if needed.

Manual path:

- VST3: `C:\Program Files\Common Files\VST3`

### Linux

Linux releases are currently distributed as VST3 archives.

Common install paths:

- `~/.vst3`
- `/usr/lib/vst3`
- `/usr/local/lib/vst3`

## Rack Modules

Current modules:

1. Distortion
2. Chorus
3. Flanger
4. Phaser
5. Delay
6. Stereo
7. Reverb
8. Compressor

## Presets

Vayu supports:

- Factory EQ presets
- Factory rack presets
- User rack presets
- User effect presets

On macOS, user presets are stored in:

- `~/Library/Vayu/Presets/Rack`
- `~/Library/Vayu/Presets/Effects`

## Notes

- Vayu is intended to be downloaded and installed directly into a DAW workflow.
- End users do not need JUCE, Xcode, CMake, or any developer tooling.
- If your DAW does not show Vayu immediately, run a plugin rescan.
- Official downloads are provided through GitHub Releases and the Vayu landing page.

## Links

- Website: [supashbhat.github.io/vayu.html](https://supashbhat.github.io/vayu.html)
- GitHub: [github.com/supashbhat/Vayu](https://github.com/supashbhat/Vayu)

See [LICENSE](LICENSE) for the current source-availability and binary-use notice.
