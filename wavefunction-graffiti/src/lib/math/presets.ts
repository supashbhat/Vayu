import {
  createEmptyWavefunction,
  fromPolar,
  normalizeWavefunction,
} from '@/lib/math/complex';
import type {
  PresetDefinition,
  SampledWavefunction,
  SimulationParameters,
} from '@/types/quantum';

function factorial(value: number): number {
  if (value <= 1) {
    return 1;
  }

  let result = 1;

  for (let index = 2; index <= value; index += 1) {
    result *= index;
  }

  return result;
}

function hermite(order: number, x: number): number {
  if (order === 0) {
    return 1;
  }

  if (order === 1) {
    return 2 * x;
  }

  let hPrev = 1;
  let hCurr = 2 * x;

  for (let n = 1; n < order; n += 1) {
    const next = 2 * x * hCurr - 2 * n * hPrev;
    hPrev = hCurr;
    hCurr = next;
  }

  return hCurr;
}

function gaussianPacket(
  parameters: SimulationParameters,
  options: { sigma: number; center: number; momentum: number },
): SampledWavefunction {
  const wavefunction = createEmptyWavefunction(parameters);

  for (let index = 0; index < wavefunction.sampleCount; index += 1) {
    const x = wavefunction.x[index];
    const gaussian = Math.exp(
      -((x - options.center) ** 2) / (4 * options.sigma ** 2),
    );
    const phase = options.momentum * x;
    const polar = fromPolar(gaussian, phase);

    wavefunction.re[index] = polar.re;
    wavefunction.im[index] = polar.im;
  }

  return normalizeWavefunction(wavefunction);
}

function harmonicEigenstate(
  parameters: SimulationParameters,
  order: number,
): SampledWavefunction {
  const wavefunction = createEmptyWavefunction(parameters);
  const normalization =
    1 /
    Math.sqrt(
      Math.sqrt(Math.PI) * (2 ** order) * factorial(order),
    );

  for (let index = 0; index < wavefunction.sampleCount; index += 1) {
    const x = wavefunction.x[index];
    const envelope = Math.exp(-(x ** 2) / 2);
    wavefunction.re[index] =
      normalization * hermite(order, x) * envelope;
    wavefunction.im[index] = 0;
  }

  return normalizeWavefunction(wavefunction);
}

function addWavefunctions(
  first: SampledWavefunction,
  second: SampledWavefunction,
  weight = 1,
): SampledWavefunction {
  const result = createEmptyWavefunction({
    sampleCount: first.sampleCount,
    domain: first.domain,
    mass: 1,
    hbar: 1,
  });

  for (let index = 0; index < first.sampleCount; index += 1) {
    result.re[index] = first.re[index] + second.re[index] * weight;
    result.im[index] = first.im[index] + second.im[index] * weight;
  }

  return normalizeWavefunction(result);
}

export function createPresetLibrary(): PresetDefinition[] {
  return [
    {
      id: 'gaussian-packet',
      name: 'Gaussian Packet',
      description: 'Minimum-uncertainty launch state with gentle phase drift.',
      badge: 'Foundational',
      create: (parameters) =>
        gaussianPacket(parameters, {
          sigma: 0.9,
          center: -1.4,
          momentum: 2.8,
        }),
    },
    {
      id: 'plane-wave',
      name: 'Plane Wave',
      description: 'Nearly delocalized state with narrow momentum support.',
      badge: 'Fourier limit',
      create: (parameters) =>
        gaussianPacket(parameters, {
          sigma: 5,
          center: 0,
          momentum: 5.2,
        }),
    },
    {
      id: 'double-packet',
      name: 'Double Packet',
      description: 'Two separated lobes for a double-slit style intuition.',
      badge: 'Interference',
      create: (parameters) => {
        const left = gaussianPacket(parameters, {
          sigma: 0.65,
          center: -2.2,
          momentum: 1.5,
        });
        const right = gaussianPacket(parameters, {
          sigma: 0.65,
          center: 2.2,
          momentum: 1.5,
        });

        return addWavefunctions(left, right);
      },
    },
    {
      id: 'localized-spike',
      name: 'Localized Spike',
      description: 'Tightly squeezed state with broad reciprocal response.',
      badge: 'Uncertainty',
      create: (parameters) =>
        gaussianPacket(parameters, {
          sigma: 0.22,
          center: 0,
          momentum: 0,
        }),
    },
    {
      id: 'superposition',
      name: 'Superposition',
      description: 'Asymmetric blend of packets with visible phase structure.',
      badge: 'Coherence',
      create: (parameters) => {
        const first = gaussianPacket(parameters, {
          sigma: 0.85,
          center: -2.5,
          momentum: 2.4,
        });
        const second = gaussianPacket(parameters, {
          sigma: 0.6,
          center: 1.4,
          momentum: -1.7,
        });

        return addWavefunctions(first, second, 0.78);
      },
    },
    {
      id: 'ho-ground',
      name: 'HO Ground',
      description: 'The smooth ground state of the harmonic oscillator.',
      badge: 'Eigenstate',
      create: (parameters) => harmonicEigenstate(parameters, 0),
    },
    {
      id: 'ho-first',
      name: 'HO First Excited',
      description: 'Odd parity with a node at the origin.',
      badge: 'Eigenstate',
      create: (parameters) => harmonicEigenstate(parameters, 1),
    },
  ];
}
