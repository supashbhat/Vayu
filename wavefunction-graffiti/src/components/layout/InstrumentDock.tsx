import { motion } from 'framer-motion';
import { MetricCard } from '@/components/controls/MetricCard';
import { SegmentedControl } from '@/components/controls/SegmentedControl';
import { EducationalOverlay } from '@/components/overlays/EducationalOverlay';
import type {
  DrawMode,
  HamiltonianMode,
  Observables,
  OverlayConcept,
} from '@/types/quantum';

function formatMetric(value: number, digits = 3): string {
  return value.toFixed(digits);
}

interface InstrumentDockProps {
  drawMode: DrawMode;
  onDrawModeChange: (mode: DrawMode) => void;
  hamiltonian: HamiltonianMode;
  onHamiltonianChange: (mode: HamiltonianMode) => void;
  isPlaying: boolean;
  onTogglePlayback: () => void;
  onResetTime: () => void;
  brushRadius: number;
  onBrushRadiusChange: (value: number) => void;
  brushStrength: number;
  onBrushStrengthChange: (value: number) => void;
  overlayConcept: OverlayConcept;
  onOverlayConceptChange: (value: OverlayConcept) => void;
  observables: Observables;
  time: number;
}

export function InstrumentDock({
  drawMode,
  onDrawModeChange,
  hamiltonian,
  onHamiltonianChange,
  isPlaying,
  onTogglePlayback,
  onResetTime,
  brushRadius,
  onBrushRadiusChange,
  brushStrength,
  onBrushStrengthChange,
  overlayConcept,
  onOverlayConceptChange,
  observables,
  time,
}: InstrumentDockProps) {
  return (
    <motion.section
      initial={{ opacity: 0, y: 24 }}
      animate={{ opacity: 1, y: 0 }}
      transition={{ duration: 0.55, delay: 0.1 }}
      className="mt-6 grid gap-6 xl:grid-cols-[1.1fr_1fr_1.25fr]"
    >
      <div className="rounded-[28px] border border-white/10 bg-white/6 p-5 shadow-panel backdrop-blur-xl">
        <p className="font-mono text-[10px] uppercase tracking-[0.26em] text-cyan/80">
          Authoring
        </p>
        <h3 className="mt-3 font-display text-2xl text-ink">
          Brush the complex field
        </h3>

        <div className="mt-5">
          <SegmentedControl
            value={drawMode}
            onChange={onDrawModeChange}
            options={[
              { value: 'amplitude', label: 'Amplitude' },
              { value: 'phase', label: 'Phase' },
            ]}
          />
        </div>

        <div className="mt-6 space-y-5">
          <label className="block">
            <div className="mb-2 flex items-center justify-between">
              <span className="font-mono text-[11px] uppercase tracking-[0.2em] text-mist">
                Brush radius
              </span>
              <span className="font-mono text-xs text-ink">
                {brushRadius.toFixed(3)}
              </span>
            </div>
            <input
              type="range"
              min="0.025"
              max="0.2"
              step="0.005"
              value={brushRadius}
              onChange={(event) =>
                onBrushRadiusChange(Number(event.target.value))
              }
              className="w-full accent-cyan"
            />
          </label>

          <label className="block">
            <div className="mb-2 flex items-center justify-between">
              <span className="font-mono text-[11px] uppercase tracking-[0.2em] text-mist">
                Brush strength
              </span>
              <span className="font-mono text-xs text-ink">
                {brushStrength.toFixed(2)}
              </span>
            </div>
            <input
              type="range"
              min="0.15"
              max="0.95"
              step="0.01"
              value={brushStrength}
              onChange={(event) =>
                onBrushStrengthChange(Number(event.target.value))
              }
              className="w-full accent-coral"
            />
          </label>
        </div>
      </div>

      <div className="rounded-[28px] border border-white/10 bg-white/6 p-5 shadow-panel backdrop-blur-xl">
        <p className="font-mono text-[10px] uppercase tracking-[0.26em] text-cyan/80">
          Evolution
        </p>
        <h3 className="mt-3 font-display text-2xl text-ink">
          Animate the state
        </h3>
        <p className="mt-2 text-sm leading-7 text-mist">
          The current scaffold supports stationary viewing or free-particle
          dispersion directly in momentum space.
        </p>

        <div className="mt-5">
          <SegmentedControl
            value={hamiltonian}
            onChange={onHamiltonianChange}
            options={[
              { value: 'static', label: 'Static' },
              { value: 'free', label: 'Free particle' },
            ]}
          />
        </div>

        <div className="mt-6 flex flex-wrap gap-3">
          <button
            type="button"
            onClick={onTogglePlayback}
            className="rounded-full border border-cyan/25 bg-cyan/12 px-4 py-2 font-mono text-[11px] uppercase tracking-[0.22em] text-cyan transition hover:bg-cyan/18"
          >
            {isPlaying ? 'Pause evolution' : 'Play evolution'}
          </button>
          <button
            type="button"
            onClick={onResetTime}
            className="rounded-full border border-white/10 bg-black/20 px-4 py-2 font-mono text-[11px] uppercase tracking-[0.22em] text-mist transition hover:text-ink"
          >
            Reset time
          </button>
        </div>

        <div className="mt-6 rounded-2xl border border-white/8 bg-black/20 px-4 py-4">
          <p className="font-mono text-[11px] uppercase tracking-[0.22em] text-mist">
            Current time
          </p>
          <p className="mt-2 font-mono text-2xl text-amber">
            t = {time.toFixed(2)}
          </p>
        </div>
      </div>

      <div className="space-y-6">
        <div className="rounded-[28px] border border-white/10 bg-white/6 p-5 shadow-panel backdrop-blur-xl">
          <p className="font-mono text-[10px] uppercase tracking-[0.26em] text-cyan/80">
            Uncertainty Instrument
          </p>
          <h3 className="mt-3 font-display text-2xl text-ink">
            Watch the reciprocal tradeoff
          </h3>

          <div className="mt-5 grid gap-3 sm:grid-cols-3">
            <MetricCard
              label="Delta x"
              value={formatMetric(observables.deltaX)}
            />
            <MetricCard
              label="Delta p"
              value={formatMetric(observables.deltaP)}
              accent="text-amber"
            />
            <MetricCard
              label="Delta x Delta p"
              value={formatMetric(observables.uncertainty)}
              accent="text-coral"
            />
          </div>

          <div className="mt-3 grid gap-3 sm:grid-cols-3">
            <MetricCard
              label="Normalization"
              value={formatMetric(observables.norm)}
              accent="text-teal"
            />
            <MetricCard
              label="Mean x"
              value={formatMetric(observables.meanX)}
              accent="text-mist"
            />
            <MetricCard
              label="Mean p"
              value={formatMetric(observables.meanP)}
              accent="text-mist"
            />
          </div>
        </div>

        <div className="space-y-4">
          <SegmentedControl
            value={overlayConcept}
            onChange={onOverlayConceptChange}
            options={[
              { value: 'duality', label: 'Duality' },
              { value: 'uncertainty', label: 'Uncertainty' },
              { value: 'phase', label: 'Phase' },
            ]}
          />
          <EducationalOverlay concept={overlayConcept} />
        </div>
      </div>
    </motion.section>
  );
}
