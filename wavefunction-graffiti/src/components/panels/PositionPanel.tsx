import { PanelFrame } from '@/components/panels/PanelFrame';
import { SignalCanvas } from '@/components/visualization/SignalCanvas';
import type { DrawMode, SampledWavefunction } from '@/types/quantum';

interface PositionPanelProps {
  wavefunction: SampledWavefunction;
  density: Float64Array;
  phase: Float64Array;
  drawMode: DrawMode;
  onPaint: (xNorm: number, yNorm: number) => void;
}

export function PositionPanel({
  wavefunction,
  density,
  phase,
  drawMode,
  onPaint,
}: PositionPanelProps) {
  return (
    <PanelFrame
      eyebrow="Position Space"
      title="Draw the state directly"
      description="The real and imaginary components ride on top of the probability density, while the spectral strip below encodes the local complex phase."
      badge={`Editing ${drawMode}`}
    >
      <SignalCanvas
        editable
        onPaint={onPaint}
        fillData={density}
        phaseData={phase}
        helperText={drawMode === 'amplitude' ? 'Amplitude brush' : 'Phase brush'}
        series={[
          {
            data: wavefunction.re,
            color: '#58d6ff',
            width: 2.4,
          },
          {
            data: wavefunction.im,
            color: '#ff7f9f',
            width: 1.8,
            opacity: 0.86,
          },
        ]}
      />

      <div className="mt-4 flex flex-wrap gap-3 text-sm text-mist">
        <div className="rounded-full border border-white/8 bg-white/5 px-4 py-2">
          Real part in cyan
        </div>
        <div className="rounded-full border border-white/8 bg-white/5 px-4 py-2">
          Imaginary part in coral
        </div>
        <div className="rounded-full border border-white/8 bg-white/5 px-4 py-2">
          Density fill in teal
        </div>
        <div className="rounded-full border border-white/8 bg-white/5 px-4 py-2">
          Phase strip below
        </div>
      </div>
    </PanelFrame>
  );
}
