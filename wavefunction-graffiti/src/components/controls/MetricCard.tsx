interface MetricCardProps {
  label: string;
  value: string;
  accent?: string;
}

export function MetricCard({
  label,
  value,
  accent = 'text-cyan',
}: MetricCardProps) {
  return (
    <div className="rounded-2xl border border-white/8 bg-white/5 px-4 py-3 shadow-panel">
      <p className="font-mono text-[11px] uppercase tracking-[0.22em] text-mist/80">
        {label}
      </p>
      <p className={`mt-2 font-mono text-xl ${accent}`}>{value}</p>
    </div>
  );
}
