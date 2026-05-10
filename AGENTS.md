# AGENTS.md

## Project Scope

This repository now contains two distinct efforts:

- The original JUCE/C++ audio plugin work.
- `wavefunction-graffiti/`, a static React + TypeScript educational web applet for undergraduate quantum mechanics.

When working on Wavefunction Graffiti, avoid changing the JUCE project unless a task explicitly asks for it.

## Product Intent

Wavefunction Graffiti should feel like a premium scientific instrument:

- mathematically honest
- visually memorable
- interactive first
- elegant rather than textbook-like
- deployable as a fully static site

The educational emphasis is Fourier duality, uncertainty, and conjugate representations, not a general-purpose Schrödinger solver.

## Engineering Principles

- Keep the app modular. Prefer small files with clear ownership.
- Treat rendering, math, and UI state as separate layers.
- Preserve a smooth interactive feel over adding many controls.
- Make every numerical approximation explicit in docs and code comments.
- Default to dimensionless units with `hbar = 1` unless a task requires otherwise.
- Ensure all core functionality runs client-side and works on GitHub Pages.

## Expected Structure

- `docs/wavefunction-graffiti/`: architecture, roadmap, design, and physics docs
- `wavefunction-graffiti/src/app/`: app shell and composition
- `wavefunction-graffiti/src/components/`: UI and panel components
- `wavefunction-graffiti/src/lib/math/`: wavefunction math, FFT, observables, evolution
- `wavefunction-graffiti/src/lib/rendering/`: canvas/WebGL rendering helpers
- `wavefunction-graffiti/src/lib/state/`: central app state and presets
- `wavefunction-graffiti/src/styles/`: Tailwind and design token layers

## Physics Guardrails

- Always normalize `psi(x)` after user edits and before derived calculations.
- Keep discrete-grid conventions consistent across position space, momentum space, and FFT scaling.
- Distinguish between visual intuition and strict physical fidelity in the UI and docs.
- Time evolution should only expose models whose approximations are documented.

## UX Guardrails

- Avoid generic dashboard visuals.
- Avoid cluttered control panels and stock chart styling.
- Favor cinematic contrast, restrained color, and legible scientific annotation.
- Interactions should be discoverable without long instructions.

## Delivery Guardrails

- Verify the frontend builds locally before closing work.
- Keep the deploy target static and GitHub Pages friendly.
- Do not commit generated `dist/` output unless explicitly asked.
