import { PanelFrame } from '@/components/panels/PanelFrame';
import { SignalCanvas } from '@/components/visualization/SignalCanvas';
import type { MomentumWavefunction } from '@/types/quantum';

interface MomentumPanelProps {
  momentum: MomentumWavefunction;
}

export function MomentumPanel({ momentum }: MomentumPanelProps) {
  return (
    <PanelFrame
      eyebrow="Momentum Space"
      title="Reciprocal structure appears immediately"
      description="This panel is the live Fourier partner of the drawing surface. When the wavefunction tightens in position space, the spectrum spreads out here."
      badge="FFT view"
    >
      <SignalCanvas
        fillData={momentum.density}
        phaseData={undefined}
        helperText="Live transform"
        series={[
          {
            data: momentum.re,
            color: '#58d6ff',
            width: 2.2,
          },
          {
            data: momentum.im,
            color: '#f7c66c',
            width: 1.7,
            opacity: 0.86,
          },
        ]}
      />

      <div className="mt-4 flex flex-wrap gap-3 text-sm text-mist">
        <div className="rounded-full border border-white/8 bg-white/5 px-4 py-2">
          Fourier amplitude in cyan and amber
        </div>
        <div className="rounded-full border border-white/8 bg-white/5 px-4 py-2">
          Spectral density fill in teal
        </div>
        <div className="rounded-full border border-white/8 bg-white/5 px-4 py-2">
          Reciprocal broadening is the key lesson
        </div>
      </div>
    </PanelFrame>
  );
}
