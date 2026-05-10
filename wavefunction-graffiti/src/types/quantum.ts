export type DrawMode = 'amplitude' | 'phase';
export type HamiltonianMode = 'static' | 'free';
export type OverlayConcept = 'duality' | 'uncertainty' | 'phase';

export type PresetId =
  | 'gaussian-packet'
  | 'plane-wave'
  | 'double-packet'
  | 'localized-spike'
  | 'superposition'
  | 'ho-ground'
  | 'ho-first';

export interface SimulationParameters {
  sampleCount: number;
  domain: number;
  mass: number;
  hbar: number;
}

export interface SampledWavefunction {
  sampleCount: number;
  domain: number;
  dx: number;
  x: Float64Array;
  re: Float64Array;
  im: Float64Array;
}

export interface MomentumWavefunction {
  p: Float64Array;
  re: Float64Array;
  im: Float64Array;
  density: Float64Array;
  dp: number;
}

export interface Observables {
  norm: number;
  meanX: number;
  meanP: number;
  deltaX: number;
  deltaP: number;
  uncertainty: number;
}

export interface PresetDefinition {
  id: PresetId;
  name: string;
  description: string;
  badge: string;
  create: (parameters: SimulationParameters) => SampledWavefunction;
}
