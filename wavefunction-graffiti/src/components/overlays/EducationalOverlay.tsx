import type { OverlayConcept } from '@/types/quantum';

const overlayCopy: Record<
  OverlayConcept,
  { eyebrow: string; title: string; body: string; equation: string }
> = {
  duality: {
    eyebrow: 'Fourier Duality',
    title: 'Narrow in x means wide in p.',
    body:
      'A sharply localized wave packet needs many momentum components to build it, so reciprocal space broadens as position space tightens.',
    equation: 'phi(p) <-> psi(x)',
  },
  uncertainty: {
    eyebrow: 'Uncertainty',
    title: 'Localization has a spectral cost.',
    body:
      'The app continuously estimates standard deviations from the sampled state, so you can watch the uncertainty product rise when the packet is squeezed.',
    equation: 'Delta x Delta p >= hbar / 2',
  },
  phase: {
    eyebrow: 'Complex Phase',
    title: 'Phase is invisible in density but not in evolution.',
    body:
      'Two states can share the same probability density while carrying different internal phase structure, which changes interference and later motion.',
    equation: 'psi = |psi| exp(i theta)',
  },
};

interface EducationalOverlayProps {
  concept: OverlayConcept;
}

export function EducationalOverlay({
  concept,
}: EducationalOverlayProps) {
  const content = overlayCopy[concept];

  return (
    <div className="rounded-[28px] border border-white/10 bg-white/6 p-5 shadow-panel backdrop-blur-xl">
      <p className="font-mono text-[10px] uppercase tracking-[0.26em] text-cyan/80">
        {content.eyebrow}
      </p>
      <h3 className="mt-3 font-display text-2xl text-ink">
        {content.title}
      </h3>
      <p className="mt-3 max-w-xl text-sm leading-7 text-mist">
        {content.body}
      </p>
      <p className="mt-4 font-mono text-sm text-amber">{content.equation}</p>
    </div>
  );
}
