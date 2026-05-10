import {
  createEmptyWavefunction,
  normalizeWavefunction,
} from '@/lib/math/complex';
import type {
  MomentumWavefunction,
  SampledWavefunction,
  SimulationParameters,
} from '@/types/quantum';

function rotateHalf(values: Float64Array): Float64Array {
  const result = new Float64Array(values.length);
  const half = values.length / 2;

  result.set(values.subarray(half), 0);
  result.set(values.subarray(0, half), values.length - half);

  return result;
}

function bitReverse(index: number, bits: number): number {
  let reversed = 0;

  for (let bit = 0; bit < bits; bit += 1) {
    reversed = (reversed << 1) | ((index >>> bit) & 1);
  }

  return reversed;
}

function fftComplex(
  inputRe: Float64Array,
  inputIm: Float64Array,
  inverse = false,
): { re: Float64Array; im: Float64Array } {
  const size = inputRe.length;
  const bits = Math.log2(size);

  if (!Number.isInteger(bits)) {
    throw new Error('FFT input size must be a power of two.');
  }

  const re = new Float64Array(size);
  const im = new Float64Array(size);

  for (let index = 0; index < size; index += 1) {
    const reversed = bitReverse(index, bits);
    re[reversed] = inputRe[index];
    im[reversed] = inputIm[index];
  }

  for (let len = 2; len <= size; len <<= 1) {
    const angle = (inverse ? 2 : -2) * Math.PI / len;
    const wLenRe = Math.cos(angle);
    const wLenIm = Math.sin(angle);

    for (let start = 0; start < size; start += len) {
      let wRe = 1;
      let wIm = 0;

      for (let offset = 0; offset < len / 2; offset += 1) {
        const evenIndex = start + offset;
        const oddIndex = evenIndex + len / 2;

        const oddRe = re[oddIndex] * wRe - im[oddIndex] * wIm;
        const oddIm = re[oddIndex] * wIm + im[oddIndex] * wRe;

        re[oddIndex] = re[evenIndex] - oddRe;
        im[oddIndex] = im[evenIndex] - oddIm;
        re[evenIndex] += oddRe;
        im[evenIndex] += oddIm;

        const nextWRe = wRe * wLenRe - wIm * wLenIm;
        const nextWIm = wRe * wLenIm + wIm * wLenRe;
        wRe = nextWRe;
        wIm = nextWIm;
      }
    }
  }

  if (inverse) {
    for (let index = 0; index < size; index += 1) {
      re[index] /= size;
      im[index] /= size;
    }
  }

  return { re, im };
}

export function forwardMomentumTransform(
  wavefunction: SampledWavefunction,
  parameters: SimulationParameters,
): MomentumWavefunction {
  const { sampleCount, domain, hbar } = parameters;
  const scale = wavefunction.dx / Math.sqrt(2 * Math.PI * hbar);
  const shiftedRe = rotateHalf(wavefunction.re);
  const shiftedIm = rotateHalf(wavefunction.im);
  const transformed = fftComplex(shiftedRe, shiftedIm);
  const centeredRe = rotateHalf(transformed.re);
  const centeredIm = rotateHalf(transformed.im);
  const density = new Float64Array(sampleCount);
  const p = new Float64Array(sampleCount);
  const dp = (2 * Math.PI * hbar) / domain;

  for (let index = 0; index < sampleCount; index += 1) {
    centeredRe[index] *= scale;
    centeredIm[index] *= scale;
    density[index] = centeredRe[index] ** 2 + centeredIm[index] ** 2;
    p[index] = (index - sampleCount / 2) * dp;
  }

  return {
    p,
    re: centeredRe,
    im: centeredIm,
    density,
    dp,
  };
}

export function inverseMomentumTransform(
  momentum: MomentumWavefunction,
  parameters: SimulationParameters,
): SampledWavefunction {
  const wavefunction = createEmptyWavefunction(parameters);
  const scale = Math.sqrt(2 * Math.PI * parameters.hbar) / wavefunction.dx;
  const shiftedRe = rotateHalf(momentum.re);
  const shiftedIm = rotateHalf(momentum.im);
  const transformed = fftComplex(shiftedRe, shiftedIm, true);
  const centeredRe = rotateHalf(transformed.re);
  const centeredIm = rotateHalf(transformed.im);

  for (let index = 0; index < parameters.sampleCount; index += 1) {
    wavefunction.re[index] = centeredRe[index] * scale;
    wavefunction.im[index] = centeredIm[index] * scale;
  }

  return normalizeWavefunction(wavefunction);
}
