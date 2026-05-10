interface TopBarProps {
  sampleCount: number;
  domain: number;
  hbar: number;
}

function StatusChip({
  label,
  value,
}: {
  label: string;
  value: string;
}) {
  return (
    <div className="rounded-full border border-white/10 bg-white/6 px-4 py-2">
      <span className="font-mono text-[10px] uppercase tracking-[0.24em] text-mist/80">
        {label}
      </span>
      <span className="ml-2 font-mono text-xs text-ink">{value}</span>
    </div>
  );
}

export function TopBar({
  sampleCount,
  domain,
  hbar,
}: TopBarProps) {
  return (
    <header className="relative overflow-hidden rounded-[32px] border border-white/10 bg-white/6 px-6 py-6 shadow-panel backdrop-blur-xl">
      <div className="absolute inset-x-0 top-0 h-px bg-gradient-to-r from-transparent via-white/30 to-transparent" />
      <div className="flex flex-col gap-6 lg:flex-row lg:items-end lg:justify-between">
        <div className="max-w-3xl">
          <p className="font-mono text-[11px] uppercase tracking-[0.3em] text-cyan/80">
            Berkeley Physics 137A Enrichment
          </p>
          <h1 className="mt-4 font-display text-4xl tracking-[0.08em] text-ink sm:text-5xl">
            Wavefunction Graffiti
          </h1>
          <p className="mt-4 max-w-2xl text-base leading-7 text-mist sm:text-lg">
            Paint a complex state in position space and watch its reciprocal
            life appear instantly in momentum space. The instrument is tuned
            for Fourier duality, uncertainty, and visual intuition.
          </p>
        </div>

        <div className="flex flex-wrap gap-3 lg:max-w-md lg:justify-end">
          <StatusChip label="Grid" value={`${sampleCount} samples`} />
          <StatusChip label="Domain" value={`${domain.toFixed(1)} units`} />
          <StatusChip label="Units" value={`hbar = ${hbar}`} />
        </div>
      </div>
    </header>
  );
}
