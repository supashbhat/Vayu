# Wavefunction Graffiti Deployment

## Current Repository Context

Wavefunction Graffiti currently lives inside the `supashbhat/Vayu` repository.

That matters because GitHub Pages treats this as a project site, not the root user site repository.

## Option 1: Deploy From This Repo

Use this when you want the app published directly from `supashbhat/Vayu`.

### Setup

1. Push the current branch to `main`.
2. In GitHub, open `supashbhat/Vayu`.
3. Go to `Settings -> Pages`.
4. Under `Build and deployment`, set `Source` to `GitHub Actions`.
5. The workflow `.github/workflows/deploy-wavefunction-graffiti.yml` will build and deploy the app automatically on future pushes that touch the app.

### Resulting URL

`https://supashbhat.github.io/Vayu/`

## Option 2: Publish Under The Root Site

Use this when you want the cleaner root-site path:

`https://supashbhat.github.io/wavefunction-graffiti/`

Because that URL belongs to the separate `supashbhat.github.io` repository, this repo cannot publish there by itself unless you add a cross-repo deployment workflow and credentials.

The simplest approach is:

1. Build here with `npm run build` inside `wavefunction-graffiti/`.
2. Copy the contents of `wavefunction-graffiti/dist/` into the root site repo under `wavefunction-graffiti/`.
3. Commit and publish from the `supashbhat.github.io` repo.

## Why The Same Build Works In Both Places

The Vite config uses a relative base path:

`base: './'`

That keeps asset URLs relative to the deployed directory, so the app can live at:

- `/Vayu/`
- `/wavefunction-graffiti/`
- or another static subdirectory

without rebuilding for each path.

## Workflow Notes

- The dedicated Pages workflow only watches the app and its docs.
- The existing JUCE build workflow now ignores Wavefunction Graffiti-only changes, so web deploys do not trigger the full plugin build matrix.
