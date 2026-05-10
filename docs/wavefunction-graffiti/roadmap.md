# Wavefunction Graffiti Development Roadmap

## Phase 0: Foundations

- Define architecture and physics scope.
- Lock the visual direction and interaction principles.
- Scaffold the static frontend workspace.
- Establish deploy and build tooling for GitHub Pages.

## Phase 1: Instrument Shell

- Build the glassmorphism app shell.
- Implement responsive layout and panel composition.
- Add typography, color tokens, and animation system.
- Create placeholder canvases and metrics cards.

## Phase 2: Wavefunction Authoring

- Implement the 1D sampled wavefunction model.
- Add position-space drawing for amplitude.
- Add optional phase painting mode.
- Normalize automatically after each edit.
- Show real, imaginary, amplitude, phase, and density views.

## Phase 3: Fourier Duality

- Integrate browser FFT pipeline.
- Render momentum-space amplitude and density live.
- Add intuitive scale labels and spread indicators.
- Connect drawing changes to visible reciprocal broadening.

## Phase 4: Uncertainty and Intuition

- Compute expectation values and standard deviations.
- Visualize `Delta x`, `Delta p`, and `Delta x Delta p`.
- Add concise overlays about conjugate variables and uncertainty.
- Tune the UI so localization and spectral broadening feel immediate.

## Phase 5: Time Evolution

- Add animation clock and transport controls.
- Implement free-particle evolution.
- Implement harmonic-oscillator evolution.
- Add stable reset and scrub behavior.

## Phase 6: Presets and Pedagogy

- Ship preset states:
  - Gaussian packet
  - plane wave
  - double-slit style packet
  - localized spike
  - superposition state
  - harmonic oscillator eigenstates
- Add short educational annotations for each preset.

## Phase 7: Polish and Release

- Refine motion, trails, and glow.
- Optimize canvas rendering and interaction latency.
- Improve mobile and trackpad behavior.
- Add GitHub Pages deployment workflow.
- Write usage notes for Berkeley enrichment delivery.

## Definition of Done for v1

- Users can draw a wavefunction and see immediate position and momentum responses.
- Normalization and uncertainty update live.
- Time evolution works for supported Hamiltonians.
- The experience is visually polished and performs smoothly on modern laptops.
- The project deploys statically without a backend.
