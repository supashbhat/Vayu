import { motion } from 'framer-motion';
import type { ReactNode } from 'react';

interface PanelFrameProps {
  eyebrow: string;
  title: string;
  description: string;
  badge: string;
  children: ReactNode;
}

export function PanelFrame({
  eyebrow,
  title,
  description,
  badge,
  children,
}: PanelFrameProps) {
  return (
    <motion.section
      initial={{ opacity: 0, y: 20 }}
      animate={{ opacity: 1, y: 0 }}
      transition={{ duration: 0.5 }}
      className="rounded-[32px] border border-white/10 bg-white/6 p-5 shadow-panel backdrop-blur-xl"
    >
      <div className="mb-4 flex flex-col gap-4 sm:flex-row sm:items-start sm:justify-between">
        <div>
          <p className="font-mono text-[10px] uppercase tracking-[0.28em] text-cyan/80">
            {eyebrow}
          </p>
          <h2 className="mt-3 font-display text-2xl text-ink">
            {title}
          </h2>
          <p className="mt-2 max-w-xl text-sm leading-7 text-mist">
            {description}
          </p>
        </div>
        <div className="rounded-full border border-white/10 bg-black/20 px-4 py-2 font-mono text-[10px] uppercase tracking-[0.24em] text-mist">
          {badge}
        </div>
      </div>
      {children}
    </motion.section>
  );
}
