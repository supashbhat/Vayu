# Vayu

![Vayu Icon](Assets/Vayu_Icon.png)

Vayu is a modular audio effect rack for shaping movement, colour, width, and atmosphere. It combines a five-band EQ with a drag-to-reorder effect chain, live visual feedback, rack presets, effect presets, and a dedicated standalone app.

## What Vayu Does

Vayu includes:

- Five-band EQ
- Distortion
- Chorus
- Flanger
- Phaser
- Delay
- Stereo imaging
- Reverb
- Compressor
- Drag-to-reorder effect chain
- Factory and user presets
- Standalone app
- AU and VST3 plugin formats

## Available Downloads

Current release targets:

- `Vayu.app`
- `Vayu.component`
- `Vayu.vst3`

`AU` is available on macOS only. `VST3` is the plugin format used for cross-platform releases.

## Installation

### macOS Standalone

1. Download `Vayu.app`.
2. Move it into `/Applications` or any folder you prefer.
3. Open the app and choose your audio device in the standalone audio settings.

### macOS AU

1. Download `Vayu.component`.
2. Copy it to:
   `~/Library/Audio/Plug-Ins/Components`
3. Open your DAW and rescan plugins if needed.

For a system-wide install, use:

`/Library/Audio/Plug-Ins/Components`

### macOS VST3

1. Download `Vayu.vst3`.
2. Copy it to:
   `~/Library/Audio/Plug-Ins/VST3`
3. Open your DAW and rescan plugins if needed.

For a system-wide install, use:

`/Library/Audio/Plug-Ins/VST3`

### Windows

When a Windows release is available, install `Vayu.vst3` into:

`C:\Program Files\Common Files\VST3`

If a standalone Windows app is included in a release, it can be placed anywhere you want or bundled in an installer.

### Linux

When a Linux release is available, install `Vayu.vst3` into one of these common locations:

- `~/.vst3`
- `/usr/lib/vst3`
- `/usr/local/lib/vst3`

If a standalone Linux build is included in a release, it can be launched directly or distributed in an archive package.

## Effect Rack

Vayu processes audio through a serial chain of modules that can be reordered from the interface.

Current modules:

1. Distortion
2. Chorus
3. Flanger
4. Phaser
5. Delay
6. Stereo
7. Reverb
8. Compressor

## Compressor

The compressor includes multiple styles for different use cases:

- `Clean`
- `Glue`
- `Punch`
- `Pump`

It also supports sidechain-ready host routing where available.

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

- Vayu is meant to be installed and used directly. End users do not need JUCE, Xcode, or any extra setup tools.
- Official builds are intended to be downloaded from GitHub Releases or from the linked website download page.
- If your DAW does not show Vayu immediately after installation, run a plugin rescan.

## Official Downloads

Vayu is distributed for normal end-user installation through official releases. If you only want to use the plugin, download the packaged release for your operating system and install it like any other plugin or desktop app.

See [LICENSE](LICENSE) for the current source-availability and binary-use notice.

## Links

- GitHub: [github.com/supashbhat/vayu](https://github.com/supashbhat/vayu)
- Portfolio: [supashbhat.github.io](https://supashbhat.github.io/)
