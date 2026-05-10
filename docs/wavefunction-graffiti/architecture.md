# Wavefunction Graffiti Architecture

## Goal

Wavefunction Graffiti is a browser-based quantum mechanics instrument in which the user paints a complex wavefunction in position space and immediately sees how that choice reshapes momentum space, uncertainty, and time evolution.

## Top-Level System Design

The app is split into four cooperating layers:

1. Presentation layer
   - React component tree
   - panel layout
   - controls, overlays, and motion

2. State layer
   - central wavefunction state
   - preset loading
   - interaction modes
   - evolution playback state

3. Math layer
   - discrete complex field representation
   - normalization
   - FFT transforms
   - expectation values and uncertainties
   - analytical time propagation for supported Hamiltonians

4. Rendering layer
   - high-performance canvas drawing
   - glow and trail effects
   - phase color mapping
   - layered graph rendering

## Proposed Layout

The interface should read like a scientific instrument rather than a form.

- Top bar
  - title, subtitle, preset selector, play or pause transport, overlay toggle
- Main deck
  - left: position-space drawing stage
  - right: momentum-space response stage
- Bottom dock
  - uncertainty meters
  - Hamiltonian selector
  - brush and phase controls
  - compact explanatory cards

On desktop, the two main panels dominate the viewport and feel like matched optical instruments. On smaller screens, the panels stack vertically with the dock converted into horizontally scrollable cards.

## Core User Flow

1. User draws amplitude and optionally phase in the position-space panel.
2. Input points are resampled onto the simulation grid.
3. The wavefunction is smoothed and normalized.
4. Derived views update immediately:
   - `psi(x)`
   - `|psi(x)|^2`
   - `psi(p)`
   - `|psi(p)|^2`
   - `Delta x`, `Delta p`, and `Delta x Delta p`
5. If time evolution is active, the evolved state is computed from the current base state and animation time.

## Component Hierarchy

- `App`
  - `AppShell`
  - `TopBar`
  - `MainWorkspace`
    - `PositionPanel`
      - `WaveCanvas`
      - `ProbabilityOverlay`
      - `PhaseLegend`
    - `MomentumPanel`
      - `SpectrumCanvas`
      - `DistributionStats`
  - `InstrumentDock`
    - `PresetCarousel`
    - `BrushControls`
    - `EvolutionControls`
    - `UncertaintyCard`
    - `OverlayToggleGroup`
  - `EducationalOverlay`

## Data Model

The canonical simulation state is a sampled complex array:

- `xGrid: Float64Array`
- `psiRe: Float64Array`
- `psiIm: Float64Array`
- `dx: number`
- `meta: interaction, preset, evolution settings`

Derived data is computed in selectors or hooks:

- normalized position wavefunction
- probability density
- momentum-space transform
- expectation values
- uncertainty values
- basis coefficients for supported preset bases

## Interaction Model

Drawing is treated as signal authoring, not freehand raster paint.

- Pointer input writes into a brush stroke buffer.
- Brush strokes are projected onto the one-dimensional spatial domain.
- Amplitude mode edits magnitude.
- Phase mode edits argument.
- Smoothing and normalization run after each stroke update.

This gives fluid drawing while preserving a mathematically meaningful sampled state.

## Rendering Pipeline

Each visualization panel uses layered rendering:

1. background grid and glow
2. primary curve
3. filled density or spectral area
4. phase color accents
5. animated highlights, cursors, and tooltips

Canvas is the default rendering target for the first implementation because it is lightweight, GitHub Pages friendly, and sufficient for smooth 1D scientific plots. The rendering layer should stay abstract enough to swap in WebGL later if needed for bloom or dense particle effects.

## Math Pipeline

1. Resample or edit `psi(x)` on a finite uniform grid.
2. Apply smoothing if enabled.
3. Normalize so `sum |psi|^2 dx = 1`.
4. Compute position observables.
5. FFT into momentum space using a consistent discrete convention.
6. Compute momentum observables and uncertainty product.
7. For time evolution, apply phase evolution in the appropriate basis for the selected Hamiltonian.

## Time Evolution Strategy

Supported Hamiltonians in the initial educational scope:

- free particle
  - evolve in momentum basis with `exp(-i p^2 t / 2m hbar)`
- harmonic oscillator
  - evolve in oscillator eigenbasis with `exp(-i E_n t / hbar)`

The first release should favor stable, documented approximations over maximal generality.

## Deployment Strategy

- Vite static build
- relative or configured base path for GitHub Pages
- all assets client-side
- no server runtime
