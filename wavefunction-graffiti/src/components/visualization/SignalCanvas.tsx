import { useEffect, useMemo, useRef, useState } from 'react';
import { phaseToColor } from '@/lib/rendering/colors';

type NumericSeries = Float64Array | number[];

interface SeriesDefinition {
  data: NumericSeries;
  color: string;
  width?: number;
  opacity?: number;
}

interface SignalCanvasProps {
  fillData?: NumericSeries;
  phaseData?: NumericSeries;
  series: SeriesDefinition[];
  editable?: boolean;
  onPaint?: (xNorm: number, yNorm: number) => void;
  helperText?: string;
  accent?: string;
}

interface PointerState {
  x: number;
  y: number;
  drawing: boolean;
}

function toArray(data: NumericSeries | undefined): number[] {
  if (!data) {
    return [];
  }

  return Array.from(data);
}

export function SignalCanvas({
  fillData,
  phaseData,
  series,
  editable = false,
  onPaint,
  helperText,
  accent = '#58d6ff',
}: SignalCanvasProps) {
  const wrapperRef = useRef<HTMLDivElement>(null);
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [size, setSize] = useState({ width: 0, height: 0 });
  const [pointer, setPointer] = useState<PointerState | null>(null);

  const lines = useMemo(() => series.map((entry) => toArray(entry.data)), [series]);
  const fill = useMemo(() => toArray(fillData), [fillData]);
  const phases = useMemo(() => toArray(phaseData), [phaseData]);

  useEffect(() => {
    const element = wrapperRef.current;

    if (!element) {
      return undefined;
    }

    const observer = new ResizeObserver((entries) => {
      const entry = entries[0];

      if (!entry) {
        return;
      }

      setSize({
        width: entry.contentRect.width,
        height: entry.contentRect.height,
      });
    });

    observer.observe(element);
    return () => observer.disconnect();
  }, []);

  useEffect(() => {
    const canvas = canvasRef.current;

    if (!canvas || size.width === 0 || size.height === 0) {
      return;
    }

    const context = canvas.getContext('2d');

    if (!context) {
      return;
    }

    const dpr = window.devicePixelRatio || 1;
    canvas.width = Math.floor(size.width * dpr);
    canvas.height = Math.floor(size.height * dpr);
    context.setTransform(dpr, 0, 0, dpr, 0, 0);

    const width = size.width;
    const height = size.height;
    const paddingX = 18;
    const topInset = 18;
    const bottomInset = 34;
    const plotHeight = height - topInset - bottomInset;
    const centerY = topInset + plotHeight * 0.44;
    const densityBaseline = topInset + plotHeight * 0.9;

    context.clearRect(0, 0, width, height);

    const background = context.createLinearGradient(0, 0, width, height);
    background.addColorStop(0, 'rgba(10, 21, 37, 0.96)');
    background.addColorStop(1, 'rgba(6, 10, 17, 0.98)');
    context.fillStyle = background;
    context.fillRect(0, 0, width, height);

    context.strokeStyle = 'rgba(154, 179, 200, 0.14)';
    context.lineWidth = 1;

    for (let grid = 0; grid <= 6; grid += 1) {
      const y = topInset + (plotHeight / 6) * grid;
      context.beginPath();
      context.moveTo(paddingX, y);
      context.lineTo(width - paddingX, y);
      context.stroke();
    }

    for (let grid = 0; grid <= 8; grid += 1) {
      const x =
        paddingX + ((width - paddingX * 2) / 8) * grid;
      context.beginPath();
      context.moveTo(x, topInset);
      context.lineTo(x, topInset + plotHeight);
      context.stroke();
    }

    context.strokeStyle = 'rgba(234, 247, 255, 0.22)';
    context.beginPath();
    context.moveTo(paddingX, centerY);
    context.lineTo(width - paddingX, centerY);
    context.stroke();

    if (fill.length > 0) {
      const maxFill = Math.max(...fill, 1e-6);
      const fillScale = plotHeight * 0.34 / maxFill;
      const area = context.createLinearGradient(0, topInset, 0, densityBaseline);
      area.addColorStop(0, 'rgba(62, 230, 198, 0.22)');
      area.addColorStop(1, 'rgba(62, 230, 198, 0.03)');

      context.beginPath();
      context.moveTo(paddingX, densityBaseline);

      fill.forEach((value, index) => {
        const x =
          paddingX +
          (index / Math.max(fill.length - 1, 1)) *
            (width - paddingX * 2);
        const y = densityBaseline - value * fillScale;
        context.lineTo(x, y);
      });

      context.lineTo(width - paddingX, densityBaseline);
      context.closePath();
      context.fillStyle = area;
      context.fill();
    }

    const maxAbs = Math.max(
      0.000001,
      ...lines.flatMap((data) => data.map((value) => Math.abs(value))),
    );
    const lineScale = plotHeight * 0.34 / maxAbs;

    series.forEach((entry, seriesIndex) => {
      const values = lines[seriesIndex];

      if (values.length === 0) {
        return;
      }

      context.beginPath();
      values.forEach((value, index) => {
        const x =
          paddingX +
          (index / Math.max(values.length - 1, 1)) *
            (width - paddingX * 2);
        const y = centerY - value * lineScale;

        if (index === 0) {
          context.moveTo(x, y);
        } else {
          context.lineTo(x, y);
        }
      });

      context.strokeStyle = entry.color;
      context.globalAlpha = entry.opacity ?? 1;
      context.lineWidth = entry.width ?? 2.1;
      context.shadowColor = entry.color;
      context.shadowBlur = 14;
      context.stroke();
      context.shadowBlur = 0;
      context.globalAlpha = 1;
    });

    if (phases.length > 0) {
      const stripTop = height - 13;
      const stripHeight = 6;
      const stripWidth = (width - paddingX * 2) / phases.length;

      phases.forEach((value, index) => {
        context.fillStyle = phaseToColor(value, 0.92);
        context.fillRect(
          paddingX + index * stripWidth,
          stripTop,
          Math.ceil(stripWidth) + 1,
          stripHeight,
        );
      });
    }

    if (pointer) {
      const cursorX = paddingX + pointer.x * (width - paddingX * 2);
      const cursorY = topInset + pointer.y * plotHeight;

      context.strokeStyle = 'rgba(88, 214, 255, 0.48)';
      context.beginPath();
      context.moveTo(cursorX, topInset);
      context.lineTo(cursorX, topInset + plotHeight);
      context.stroke();

      context.fillStyle = accent;
      context.shadowColor = accent;
      context.shadowBlur = 18;
      context.beginPath();
      context.arc(cursorX, cursorY, 5, 0, Math.PI * 2);
      context.fill();
      context.shadowBlur = 0;
    }
  }, [accent, fill, lines, phaseData, phases, pointer, series, size]);

  const updatePointer = (
    clientX: number,
    clientY: number,
    drawing: boolean,
  ) => {
    const element = wrapperRef.current;

    if (!element) {
      return;
    }

    const bounds = element.getBoundingClientRect();
    const x = Math.min(Math.max((clientX - bounds.left) / bounds.width, 0), 1);
    const y = Math.min(Math.max((clientY - bounds.top) / bounds.height, 0), 1);

    setPointer({ x, y, drawing });

    if (drawing && editable && onPaint) {
      onPaint(x, y);
    }
  };

  return (
    <div
      ref={wrapperRef}
      className="group relative h-[360px] overflow-hidden rounded-[28px] border border-white/8 bg-black/30"
    >
      <canvas
        ref={canvasRef}
        className="h-full w-full touch-none"
        onPointerDown={(event) => {
          if (!editable) {
            return;
          }

          event.currentTarget.setPointerCapture(event.pointerId);
          updatePointer(event.clientX, event.clientY, true);
        }}
        onPointerMove={(event) => {
          updatePointer(
            event.clientX,
            event.clientY,
            pointer?.drawing ?? false,
          );
        }}
        onPointerUp={() => {
          setPointer((current) =>
            current ? { ...current, drawing: false } : null,
          );
        }}
        onPointerLeave={() => {
          setPointer((current) =>
            current?.drawing ? current : null,
          );
        }}
      />

      {helperText ? (
        <div className="pointer-events-none absolute left-4 top-4 rounded-full border border-white/10 bg-black/25 px-3 py-1.5 font-mono text-[10px] uppercase tracking-[0.24em] text-mist">
          {helperText}
        </div>
      ) : null}

      {editable ? (
        <div className="pointer-events-none absolute inset-x-0 bottom-4 flex justify-center">
          <div className="rounded-full border border-cyan/20 bg-black/25 px-4 py-2 font-mono text-[10px] uppercase tracking-[0.24em] text-cyan/90">
            Drag directly on the field to sculpt the state
          </div>
        </div>
      ) : null}
    </div>
  );
}
