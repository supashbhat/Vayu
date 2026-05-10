import { motion } from 'framer-motion';
import { PresetStrip } from '@/components/controls/PresetStrip';
import { InstrumentDock } from '@/components/layout/InstrumentDock';
import { TopBar } from '@/components/layout/TopBar';
import { MomentumPanel } from '@/components/panels/MomentumPanel';
import { PositionPanel } from '@/components/panels/PositionPanel';
import { useWavefunctionInstrument } from '@/lib/state/useWavefunctionInstrument';

export function AppShell() {
  const instrument = useWavefunctionInstrument();

  return (
    <div className="relative min-h-screen overflow-hidden bg-obsidian text-ink">
      <div className="pointer-events-none absolute inset-0 bg-haze opacity-90" />
      <div className="pointer-events-none absolute left-[-10%] top-[-10%] h-[34rem] w-[34rem] rounded-full bg-cyan/10 blur-[140px]" />
      <div className="pointer-events-none absolute bottom-[-12%] right-[-8%] h-[28rem] w-[28rem] rounded-full bg-coral/10 blur-[150px]" />

      <motion.main
        initial={{ opacity: 0 }}
        animate={{ opacity: 1 }}
        transition={{ duration: 0.6 }}
        className="relative mx-auto max-w-[1500px] px-4 py-6 sm:px-6 lg:px-8"
      >
        <TopBar
          sampleCount={instrument.parameters.sampleCount}
          domain={instrument.parameters.domain}
          hbar={instrument.parameters.hbar}
        />

        <PresetStrip
          presets={instrument.presets}
          activePresetId={instrument.presetId}
          onSelect={instrument.loadPreset}
        />

        <section className="mt-6 grid gap-6 xl:grid-cols-[1.18fr_0.82fr]">
          <PositionPanel
            wavefunction={instrument.displayState}
            density={instrument.positionDensity}
            phase={instrument.positionPhase}
            drawMode={instrument.drawMode}
            onPaint={instrument.paintWavefunction}
          />

          <MomentumPanel momentum={instrument.momentum} />
        </section>

        <InstrumentDock
          drawMode={instrument.drawMode}
          onDrawModeChange={instrument.setDrawMode}
          hamiltonian={instrument.hamiltonian}
          onHamiltonianChange={(mode) => {
            instrument.setHamiltonian(mode);
            instrument.setIsPlaying(false);
            instrument.setTime(0);
          }}
          isPlaying={instrument.isPlaying}
          onTogglePlayback={() =>
            instrument.setIsPlaying((current) => !current)
          }
          onResetTime={() => {
            instrument.setTime(0);
            instrument.setIsPlaying(false);
          }}
          brushRadius={instrument.brushRadius}
          onBrushRadiusChange={instrument.setBrushRadius}
          brushStrength={instrument.brushStrength}
          onBrushStrengthChange={instrument.setBrushStrength}
          overlayConcept={instrument.overlayConcept}
          onOverlayConceptChange={instrument.setOverlayConcept}
          observables={instrument.observables}
          time={instrument.time}
        />
      </motion.main>
    </div>
  );
}
