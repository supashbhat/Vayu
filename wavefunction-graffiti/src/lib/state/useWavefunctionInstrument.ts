import { useEffect, useMemo, useState } from 'react';
import {
  cloneWavefunction,
  fromPolar,
  getAmplitude,
  getDensity,
  getPhase,
  normalizeWavefunction,
  smoothWavefunction,
  createSimulationParameters,
} from '@/lib/math/complex';
import { evolveWavefunction } from '@/lib/math/evolution';
import { forwardMomentumTransform } from '@/lib/math/fft';
import { computeObservables } from '@/lib/math/observables';
import { createPresetLibrary } from '@/lib/math/presets';
import type {
  DrawMode,
  HamiltonianMode,
  OverlayConcept,
  PresetId,
  SampledWavefunction,
} from '@/types/quantum';

function wrapAngle(angle: number): number {
  return Math.atan2(Math.sin(angle), Math.cos(angle));
}

function blendAngles(current: number, target: number, alpha: number): number {
  const delta = wrapAngle(target - current);
  return wrapAngle(current + delta * alpha);
}

function applyBrush(
  wavefunction: SampledWavefunction,
  xNorm: number,
  yNorm: number,
  drawMode: DrawMode,
  radius: number,
  strength: number,
): SampledWavefunction {
  const next = cloneWavefunction(wavefunction);
  const centerIndex = Math.round(xNorm * (next.sampleCount - 1));
  const sigma = Math.max(1, radius * next.sampleCount);
  const amplitude = getAmplitude(next);
  const phase = getPhase(next);
  const targetAmplitude = Math.max(0.02, (1 - yNorm) ** 1.35 * 1.4);
  const targetPhase = (0.5 - yNorm) * 2 * Math.PI;

  for (let index = 0; index < next.sampleCount; index += 1) {
    const distance = (index - centerIndex) / sigma;
    const falloff = Math.exp(-(distance ** 2) * 1.8);

    if (falloff < 0.002) {
      continue;
    }

    if (drawMode === 'amplitude') {
      amplitude[index] =
        amplitude[index] * (1 - falloff * strength) +
        targetAmplitude * falloff * strength;
    } else {
      phase[index] = blendAngles(
        phase[index],
        targetPhase,
        falloff * strength,
      );
    }
  }

  for (let index = 0; index < next.sampleCount; index += 1) {
    const polar = fromPolar(amplitude[index], phase[index]);
    next.re[index] = polar.re;
    next.im[index] = polar.im;
  }

  return smoothWavefunction(normalizeWavefunction(next), 1);
}

export function useWavefunctionInstrument() {
  const parameters = useMemo(() => createSimulationParameters(), []);
  const presets = useMemo(() => createPresetLibrary(), []);
  const presetMap = useMemo(
    () =>
      new Map(
        presets.map((preset) => [preset.id, preset] as const),
      ),
    [presets],
  );
  const [presetId, setPresetId] =
    useState<PresetId>('gaussian-packet');
  const [drawMode, setDrawMode] =
    useState<DrawMode>('amplitude');
  const [overlayConcept, setOverlayConcept] =
    useState<OverlayConcept>('duality');
  const [hamiltonian, setHamiltonian] =
    useState<HamiltonianMode>('static');
  const [isPlaying, setIsPlaying] = useState(false);
  const [time, setTime] = useState(0);
  const [brushRadius, setBrushRadius] = useState(0.075);
  const [brushStrength, setBrushStrength] = useState(0.58);
  const [baseState, setBaseState] = useState(() =>
    presetMap.get('gaussian-packet')!.create(parameters),
  );

  useEffect(() => {
    if (!isPlaying || hamiltonian === 'static') {
      return undefined;
    }

    let animationFrame = 0;
    let lastTime = performance.now();

    const tick = (now: number) => {
      const deltaSeconds = (now - lastTime) / 1000;
      lastTime = now;
      setTime((current) => current + deltaSeconds * 0.9);
      animationFrame = window.requestAnimationFrame(tick);
    };

    animationFrame = window.requestAnimationFrame(tick);

    return () => window.cancelAnimationFrame(animationFrame);
  }, [hamiltonian, isPlaying]);

  const displayState = useMemo(
    () => evolveWavefunction(baseState, time, hamiltonian, parameters),
    [baseState, time, hamiltonian, parameters],
  );

  const positionDensity = useMemo(
    () => getDensity(displayState),
    [displayState],
  );
  const positionPhase = useMemo(
    () => getPhase(displayState),
    [displayState],
  );
  const positionAmplitude = useMemo(
    () => getAmplitude(displayState),
    [displayState],
  );
  const momentum = useMemo(
    () => forwardMomentumTransform(displayState, parameters),
    [displayState, parameters],
  );
  const observables = useMemo(
    () => computeObservables(displayState, momentum),
    [displayState, momentum],
  );

  const loadPreset = (nextPresetId: PresetId) => {
    const preset = presetMap.get(nextPresetId);

    if (!preset) {
      return;
    }

    setPresetId(nextPresetId);
    setBaseState(preset.create(parameters));
    setTime(0);
    setIsPlaying(false);
  };

  const paintWavefunction = (xNorm: number, yNorm: number) => {
    setBaseState((current) =>
      applyBrush(
        current,
        xNorm,
        yNorm,
        drawMode,
        brushRadius,
        brushStrength,
      ),
    );
    setTime(0);
    setIsPlaying(false);
  };

  return {
    parameters,
    presets,
    presetId,
    drawMode,
    overlayConcept,
    hamiltonian,
    isPlaying,
    time,
    brushRadius,
    brushStrength,
    displayState,
    positionDensity,
    positionPhase,
    positionAmplitude,
    momentum,
    observables,
    loadPreset,
    paintWavefunction,
    setDrawMode,
    setOverlayConcept,
    setHamiltonian,
    setIsPlaying,
    setTime,
    setBrushRadius,
    setBrushStrength,
  };
}
