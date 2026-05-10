import { getDensity } from '@/lib/math/complex';
import type {
  MomentumWavefunction,
  Observables,
  SampledWavefunction,
} from '@/types/quantum';

export function computeObservables(
  wavefunction: SampledWavefunction,
  momentum: MomentumWavefunction,
): Observables {
  const densityX = getDensity(wavefunction);
  let norm = 0;
  let meanX = 0;
  let meanX2 = 0;
  let meanP = 0;
  let meanP2 = 0;

  for (let index = 0; index < wavefunction.sampleCount; index += 1) {
    const rhoX = densityX[index];
    const x = wavefunction.x[index];

    norm += rhoX * wavefunction.dx;
    meanX += x * rhoX * wavefunction.dx;
    meanX2 += x * x * rhoX * wavefunction.dx;
  }

  for (let index = 0; index < momentum.p.length; index += 1) {
    const rhoP = momentum.density[index];
    const p = momentum.p[index];

    meanP += p * rhoP * momentum.dp;
    meanP2 += p * p * rhoP * momentum.dp;
  }

  const varianceX = Math.max(0, meanX2 - meanX ** 2);
  const varianceP = Math.max(0, meanP2 - meanP ** 2);
  const deltaX = Math.sqrt(varianceX);
  const deltaP = Math.sqrt(varianceP);

  return {
    norm,
    meanX,
    meanP,
    deltaX,
    deltaP,
    uncertainty: deltaX * deltaP,
  };
}
