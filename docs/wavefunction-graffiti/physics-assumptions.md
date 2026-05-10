# Wavefunction Graffiti Physics Assumptions

## Purpose

This document defines the educational and numerical simplifications used by Wavefunction Graffiti so the app stays honest about what it is and is not simulating.

## Educational Scope

Wavefunction Graffiti is a one-dimensional intuition builder for undergraduate quantum mechanics. It emphasizes:

- complex amplitudes
- normalization
- Fourier duality between position and momentum
- uncertainty relations
- phase-sensitive evolution

It is not intended to be a general-purpose PDE solver or a research-grade simulator.

## Core Assumptions

- One spatial dimension.
- Finite uniform grid in position space.
- Dimensionless default units with `hbar = 1`.
- Default mass `m = 1` unless otherwise exposed.
- Periodic discrete Fourier transform conventions for the FFT layer.

## Normalization Convention

The sampled wavefunction is normalized numerically so that:

`sum_j |psi(x_j)|^2 dx = 1`

All derived observables are computed from the normalized state.

## Momentum Representation

Momentum space is computed from the discrete Fourier transform of the sampled position-space state. Because the app uses a finite grid:

- momentum samples are discrete
- the displayed spectrum depends on domain size and sample spacing
- wraparound and aliasing are possible for extreme inputs

The UI should communicate these as numerical-model limitations rather than hidden errors.

## Uncertainty Calculations

The app computes approximate expectation values using the sampled grid:

- `<x>`
- `<x^2>`
- `Delta x`
- `<p>`
- `<p^2>`
- `Delta p`
- `Delta x Delta p`

These are instructional approximations derived from the current discrete state and transform convention.

## Drawing Model

User drawing is interpreted as authoring a complex wavefunction envelope on a finite domain. To keep the state well behaved:

- strokes are resampled to the simulation grid
- optional smoothing may be applied
- normalization occurs after edits
- a minimum floor may be used to avoid unstable phase behavior where amplitude vanishes

## Time Evolution Models

### Free Particle

The free-particle mode evolves each momentum component by its phase factor:

`exp(-i p^2 t / 2m hbar)`

This is appropriate for demonstrating dispersion and phase winding.

### Harmonic Oscillator

The harmonic-oscillator mode uses a decomposition into supported oscillator basis states, then evolves each coefficient by:

`exp(-i E_n t / hbar)`

For v1, the basis may be truncated to a finite number of states for stability and performance.

## Known Limitations

- Finite grid means finite resolution.
- FFT implies periodic boundary assumptions.
- Sharp spikes can create aliasing or ringing.
- Harmonic-oscillator evolution is basis-truncated, not exact in the infinite Hilbert space.
- The app prioritizes intuition and responsiveness over arbitrary-potential generality.

## Messaging Guidance

When the UI explains results, it should distinguish:

- physically meaningful qualitative behavior
- numerically approximate implementation details

The experience should build good intuition without overstating precision.
