import type { SampledWavefunction, SimulationParameters } from '@/types/quantum';

export function createSimulationParameters(): SimulationParameters {
  return {
    sampleCount: 256,
    domain: 20,
    mass: 1,
    hbar: 1,
  };
}

export function createPositionGrid({
  sampleCount,
  domain,
}: SimulationParameters): { x: Float64Array; dx: number } {
  const dx = domain / sampleCount;
  const x = new Float64Array(sampleCount);

  for (let index = 0; index < sampleCount; index += 1) {
    x[index] = -domain / 2 + index * dx;
  }

  return { x, dx };
}

export function createEmptyWavefunction(
  parameters: SimulationParameters,
): SampledWavefunction {
  const { x, dx } = createPositionGrid(parameters);

  return {
    sampleCount: parameters.sampleCount,
    domain: parameters.domain,
    dx,
    x,
    re: new Float64Array(parameters.sampleCount),
    im: new Float64Array(parameters.sampleCount),
  };
}

export function cloneWavefunction(
  wavefunction: SampledWavefunction,
): SampledWavefunction {
  return {
    sampleCount: wavefunction.sampleCount,
    domain: wavefunction.domain,
    dx: wavefunction.dx,
    x: wavefunction.x.slice(),
    re: wavefunction.re.slice(),
    im: wavefunction.im.slice(),
  };
}

export function fromPolar(
  amplitude: number,
  phase: number,
): { re: number; im: number } {
  return {
    re: amplitude * Math.cos(phase),
    im: amplitude * Math.sin(phase),
  };
}

export function getDensity(
  wavefunction: SampledWavefunction,
): Float64Array {
  const density = new Float64Array(wavefunction.sampleCount);

  for (let index = 0; index < wavefunction.sampleCount; index += 1) {
    density[index] =
      wavefunction.re[index] ** 2 + wavefunction.im[index] ** 2;
  }

  return density;
}

export function getAmplitude(
  wavefunction: SampledWavefunction,
): Float64Array {
  const amplitude = new Float64Array(wavefunction.sampleCount);

  for (let index = 0; index < wavefunction.sampleCount; index += 1) {
    amplitude[index] = Math.hypot(
      wavefunction.re[index],
      wavefunction.im[index],
    );
  }

  return amplitude;
}

export function getPhase(wavefunction: SampledWavefunction): Float64Array {
  const phase = new Float64Array(wavefunction.sampleCount);

  for (let index = 0; index < wavefunction.sampleCount; index += 1) {
    phase[index] = Math.atan2(wavefunction.im[index], wavefunction.re[index]);
  }

  return phase;
}

export function normalizeWavefunction(
  wavefunction: SampledWavefunction,
): SampledWavefunction {
  const normalized = cloneWavefunction(wavefunction);
  let norm = 0;

  for (let index = 0; index < normalized.sampleCount; index += 1) {
    norm +=
      (normalized.re[index] ** 2 + normalized.im[index] ** 2) *
      normalized.dx;
  }

  const scale = norm > 0 ? 1 / Math.sqrt(norm) : 1;

  for (let index = 0; index < normalized.sampleCount; index += 1) {
    normalized.re[index] *= scale;
    normalized.im[index] *= scale;
  }

  return normalized;
}

export function smoothWavefunction(
  wavefunction: SampledWavefunction,
  passes = 1,
): SampledWavefunction {
  let smoothed = cloneWavefunction(wavefunction);

  for (let pass = 0; pass < passes; pass += 1) {
    const nextRe = smoothed.re.slice();
    const nextIm = smoothed.im.slice();

    for (let index = 1; index < smoothed.sampleCount - 1; index += 1) {
      nextRe[index] =
        smoothed.re[index - 1] * 0.2 +
        smoothed.re[index] * 0.6 +
        smoothed.re[index + 1] * 0.2;
      nextIm[index] =
        smoothed.im[index - 1] * 0.2 +
        smoothed.im[index] * 0.6 +
        smoothed.im[index + 1] * 0.2;
    }

    smoothed = {
      ...smoothed,
      re: nextRe,
      im: nextIm,
    };
  }

  return normalizeWavefunction(smoothed);
}
