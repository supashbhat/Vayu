import type { PresetDefinition, PresetId } from '@/types/quantum';

interface PresetStripProps {
  presets: PresetDefinition[];
  activePresetId: PresetId;
  onSelect: (presetId: PresetId) => void;
}

export function PresetStrip({
  presets,
  activePresetId,
  onSelect,
}: PresetStripProps) {
  return (
    <section className="mt-6">
      <div className="mb-3 flex items-center justify-between">
        <div>
          <p className="font-mono text-[11px] uppercase tracking-[0.24em] text-mist/80">
            Preset States
          </p>
          <p className="mt-1 text-sm text-mist">
            Start from a clean quantum gesture, then paint over it.
          </p>
        </div>
      </div>

      <div className="grid gap-3 md:grid-cols-2 xl:grid-cols-3 2xl:grid-cols-4">
        {presets.map((preset) => {
          const active = preset.id === activePresetId;

          return (
            <button
              key={preset.id}
              type="button"
              onClick={() => onSelect(preset.id)}
              className={[
                'rounded-3xl border px-4 py-4 text-left transition duration-300',
                active
                  ? 'border-cyan/50 bg-cyan/10 shadow-glow'
                  : 'border-white/8 bg-white/5 hover:border-white/20 hover:bg-white/8',
              ].join(' ')}
            >
              <p className="font-mono text-[10px] uppercase tracking-[0.24em] text-mist">
                {preset.badge}
              </p>
              <p className="mt-2 font-display text-lg text-ink">
                {preset.name}
              </p>
              <p className="mt-2 text-sm leading-6 text-mist">
                {preset.description}
              </p>
            </button>
          );
        })}
      </div>
    </section>
  );
}
