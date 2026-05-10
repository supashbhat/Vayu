export function phaseToColor(phase: number, alpha = 0.9): string {
  const normalized = (phase + Math.PI) / (2 * Math.PI);
  const hue = 190 + normalized * 150;

  return `hsla(${hue.toFixed(1)} 88% 67% / ${alpha})`;
}

export const instrumentPalette = {
  cyan: '#58d6ff',
  teal: '#3ee6c6',
  amber: '#f7c66c',
  coral: '#ff7f9f',
  mist: '#9ab3c8',
};
