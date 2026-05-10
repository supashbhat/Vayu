import type { Config } from 'tailwindcss';

const config: Config = {
  content: ['./index.html', './src/**/*.{ts,tsx}'],
  theme: {
    extend: {
      colors: {
        obsidian: '#060b14',
        graphite: '#0d1727',
        frost: 'rgba(214, 237, 255, 0.16)',
        cyan: '#58d6ff',
        teal: '#3ee6c6',
        amber: '#f7c66c',
        coral: '#ff7f9f',
        ink: '#eaf7ff',
        mist: '#9ab3c8',
      },
      fontFamily: {
        display: ['"Space Grotesk"', 'sans-serif'],
        mono: ['"IBM Plex Mono"', 'monospace'],
      },
      boxShadow: {
        panel:
          '0 24px 64px rgba(0, 0, 0, 0.42), inset 0 1px 0 rgba(255, 255, 255, 0.06)',
        glow: '0 0 32px rgba(88, 214, 255, 0.2)',
      },
      backgroundImage: {
        haze:
          'radial-gradient(circle at top left, rgba(88, 214, 255, 0.16), transparent 38%), radial-gradient(circle at top right, rgba(255, 127, 159, 0.1), transparent 26%), radial-gradient(circle at bottom center, rgba(62, 230, 198, 0.1), transparent 36%)',
      },
    },
  },
  plugins: [],
};

export default config;
