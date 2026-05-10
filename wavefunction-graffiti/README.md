# Wavefunction Graffiti

Wavefunction Graffiti is a static React + TypeScript quantum mechanics applet focused on drawing a complex wavefunction and building intuition for Fourier duality, momentum-space structure, and uncertainty.

## Current Scaffold

- Vite + React + TypeScript + Tailwind foundation
- Canvas-based scientific visualization shell
- Preset quantum states
- Live position-space and momentum-space panels
- Numerical normalization, FFT, and uncertainty metrics
- Free-particle evolution scaffold
- Design system and documentation for the next implementation phases

## Commands

```bash
npm install
npm run typecheck
npm run dev
npm run build
```

## GitHub Pages

This repo is `supashbhat/Vayu`, not the root `supashbhat/supashbhat.github.io` site repo.

That means there are two valid publish paths:

- Repo Pages from this repo:
  - enable `Settings -> Pages -> Source -> GitHub Actions`
  - push to `main`
  - the workflow at `.github/workflows/deploy-wavefunction-graffiti.yml` will publish the app as this repository's Pages site
  - expected URL: `https://supashbhat.github.io/Vayu/`

- Root site integration:
  - build locally with `npm run build`
  - copy the contents of `dist/` into the separate `supashbhat.github.io` repository under a folder such as `wavefunction-graffiti/`
  - expected URL there: `https://supashbhat.github.io/wavefunction-graffiti/`

The Vite config uses a relative base (`./`), so the same build artifact can be served from either a repository Pages URL or a subdirectory inside the root site repo.

## Key Paths

- `src/app/`: app shell and composition
- `src/components/`: UI, panel, and visualization components
- `src/lib/math/`: numerical wavefunction logic, FFT, observables, presets
- `src/lib/state/`: interaction and simulation state
- `src/styles/`: global styling
- `../docs/wavefunction-graffiti/`: architecture, roadmap, design, and physics notes
