import { cloneWavefunction } from '@/lib/math/complex';
import {
  forwardMomentumTransform,
  inverseMomentumTransform,
} from '@/lib/math/fft';
import type {
  HamiltonianMode,
  SampledWavefunction,
  SimulationParameters,
} from '@/types/quantum';

export function evolveWavefunction(
  baseState: SampledWavefunction,
  time: number,
  hamiltonian: HamiltonianMode,
  parameters: SimulationParameters,
): SampledWavefunction {
  if (hamiltonian === 'static' || time === 0) {
    return cloneWavefunction(baseState);
  }

  if (hamiltonian === 'free') {
    const spectrum = forwardMomentumTransform(baseState, parameters);

    for (let index = 0; index < spectrum.p.length; index += 1) {
      const phase =
        -((spectrum.p[index] ** 2) * time) /
        (2 * parameters.mass * parameters.hbar);
      const cosPhase = Math.cos(phase);
      const sinPhase = Math.sin(phase);
      const re = spectrum.re[index];
      const im = spectrum.im[index];

      spectrum.re[index] = re * cosPhase - im * sinPhase;
      spectrum.im[index] = re * sinPhase + im * cosPhase;
    }

    return inverseMomentumTransform(spectrum, parameters);
  }

  return cloneWavefunction(baseState);
}
