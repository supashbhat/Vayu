interface SegmentedOption<T extends string> {
  value: T;
  label: string;
}

interface SegmentedControlProps<T extends string> {
  value: T;
  onChange: (value: T) => void;
  options: SegmentedOption<T>[];
}

export function SegmentedControl<T extends string>({
  value,
  onChange,
  options,
}: SegmentedControlProps<T>) {
  return (
    <div className="inline-flex rounded-full border border-white/10 bg-black/20 p-1">
      {options.map((option) => {
        const active = option.value === value;

        return (
          <button
            key={option.value}
            type="button"
            onClick={() => onChange(option.value)}
            className={[
              'rounded-full px-4 py-2 text-sm transition',
              active
                ? 'bg-white/12 text-ink shadow-glow'
                : 'text-mist hover:text-ink',
            ].join(' ')}
          >
            {option.label}
          </button>
        );
      })}
    </div>
  );
}
