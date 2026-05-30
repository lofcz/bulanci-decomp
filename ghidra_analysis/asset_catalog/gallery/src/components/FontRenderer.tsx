import { useEffect, useMemo, useRef, useState } from "react";
import type { FontMeta } from "@/types";
import { cn } from "@/lib/utils";
import { useDataSource } from "@/data/DataSourceContext";
import type { DataSource } from "@/data/DataSource";

/** Canvas-based bitmap font renderer.  Loads the engine's PNG atlas
 *  once, slices it by per-glyph rectangles (offsetX/Y, width/height),
 *  and blits a given string by mapping each Unicode code-point to its
 *  cp1250 byte (which is the row index in `characters[]`). */

// Cache the decoded `HTMLImageElement` per (DataSource, filename) so a
// glyph table opening on a Font (218 cells) only spins up one fetch
// total.  Keyed by source so switching sources (FS folder ↔ HTTP base)
// doesn't return stale blob URLs from the previous source.
const _atlasCache = new WeakMap<DataSource, Map<string, Promise<HTMLImageElement>>>();

/** Parse a CSS colour into [r,g,b,a(0..1)]. Handles the `rgba(r,g,b,a)`
 *  strings our scene model emits plus `#rgb[a]`/`#rrggbb[aa]`; anything
 *  unrecognised falls back to opaque white (the atlas's native colour). */
function parseColor(css?: string): [number, number, number, number] {
  if (!css) return [255, 255, 255, 1];
  const m = css.match(/rgba?\(([^)]+)\)/i);
  if (m) {
    const p = m[1].split(",").map((s) => parseFloat(s.trim()));
    return [p[0] || 0, p[1] || 0, p[2] || 0, p[3] === undefined ? 1 : p[3]];
  }
  let hex = css.replace(/^#/, "");
  if (hex.length === 3 || hex.length === 4) hex = hex.split("").map((c) => c + c).join("");
  if (/^[0-9a-f]{6,8}$/i.test(hex)) {
    return [
      parseInt(hex.slice(0, 2), 16),
      parseInt(hex.slice(2, 4), 16),
      parseInt(hex.slice(4, 6), 16),
      hex.length >= 8 ? parseInt(hex.slice(6, 8), 16) / 255 : 1,
    ];
  }
  return [255, 255, 255, 1];
}

function loadAtlas(source: DataSource, filename: string): Promise<HTMLImageElement> {
  let perSource = _atlasCache.get(source);
  if (!perSource) {
    perSource = new Map();
    _atlasCache.set(source, perSource);
  }
  const hit = perSource.get(filename);
  if (hit) return hit;

  const p = (async () => {
    const url = await source.overlayUrl(filename);
    return await new Promise<HTMLImageElement>((resolve, reject) => {
      const img = new Image();
      img.onload  = () => resolve(img);
      img.onerror = () => {
        perSource!.delete(filename);
        reject(new Error(`failed to load atlas ${filename}`));
      };
      img.src = url;
    });
  })();
  perSource.set(filename, p);
  return p;
}

interface FontPreviewProps {
  font:  FontMeta;
  text:  string;
  /** Optional integer upscale for crisp pixel art (default 1, max 4). */
  scale?: number;
  /** Pad around the rendered text in pixels (in CSS units). */
  padding?: number;
  /** If set, glyphs are truncated to fit within this CSS-pixel width
   *  (an ellipsis is appended when truncation happens). */
  maxWidth?: number;
  /** Optional CSS colour to recolour the (white) atlas glyphs, matching
   *  the engine's per-draw tint. Applied via `source-in` so glyph alpha
   *  (incl. anti-aliasing) is preserved. */
  tint?: string;
  className?: string;
}

export function FontPreview({ font, text, scale = 1, padding = 4, maxWidth, tint, className }: FontPreviewProps) {
  const source = useDataSource();
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const [atlas, setAtlas]   = useState<HTMLImageElement | null>(null);
  const [failed, setFailed] = useState(false);

  // Reverse-lookup: unicode char → atlas row index.
  const charToIndex = useMemo(() => {
    const m = new Map<string, number>();
    for (const c of font.characters) {
      if (c.unicodeChar) m.set(c.unicodeChar, c.index);
    }
    return m;
  }, [font]);

  useEffect(() => {
    let cancelled = false;
    loadAtlas(source, font.atlas).then(img => {
      if (!cancelled) setAtlas(img);
    }).catch(() => {
      if (!cancelled) setFailed(true);
    });
    return () => { cancelled = true; };
  }, [source, font.atlas]);

  useEffect(() => {
    if (!atlas) return;
    const canvas = canvasRef.current;
    if (!canvas) return;

    // First pass: measure the rendered string width.
    type Glyph = { meta: typeof font.characters[number] | null; advance: number };
    const fallbackAdvance = Math.max(1, Math.floor(font.defaultWidth / 2));
    const glyphFor = (ch: string): Glyph => {
      const idx = charToIndex.get(ch);
      const meta = idx !== undefined ? font.characters[idx] : null;
      if (!meta || meta.width === 0) return { meta: null, advance: fallbackAdvance };
      return { meta, advance: meta.width };
    };

    let glyphs: Glyph[] = [];
    let width = 0;
    for (const ch of text) {
      const g = glyphFor(ch);
      glyphs.push(g);
      width += g.advance;
    }

    // Truncate if a `maxWidth` budget is set.
    if (maxWidth !== undefined) {
      const budget = (maxWidth - padding * 2) / Math.max(1, Math.min(scale, 4));
      if (width > budget) {
        const ellipsis = glyphFor("…").meta?.width ? [glyphFor("…")] : [glyphFor("."), glyphFor("."), glyphFor(".")];
        const ellW = ellipsis.reduce((a, g) => a + g.advance, 0);
        let used = 0;
        const trimmed: Glyph[] = [];
        for (const g of glyphs) {
          if (used + g.advance + ellW > budget) break;
          trimmed.push(g);
          used += g.advance;
        }
        glyphs = [...trimmed, ...ellipsis];
        width = used + ellW;
      }
    }
    const totalW = width + padding * 2;
    const totalH = font.lineHeight + padding * 2;

    const eff = Math.max(1, Math.min(scale, 4));
    canvas.width  = totalW * eff;
    canvas.height = totalH * eff;
    canvas.style.width  = `${totalW}px`;
    canvas.style.height = `${totalH}px`;

    const ctx = canvas.getContext("2d");
    if (!ctx) return;
    ctx.imageSmoothingEnabled = false;
    ctx.scale(eff, eff);
    ctx.clearRect(0, 0, totalW, totalH);

    let x = padding;
    for (const { meta, advance } of glyphs) {
      if (meta && meta.width > 0) {
        ctx.drawImage(
          atlas,
          meta.offsetX, 0, meta.width, font.lineHeight,
          x,            padding, meta.width, font.lineHeight,
        );
      }
      x += advance;
    }

    // The engine's bitmap-font atlas stores each glyph's coverage as
    // *luminance* on an OPAQUE (black) background — the alpha channel is a
    // flat 255, so the glyph shape lives in RGB, not alpha. Convert that
    // luminance into the alpha channel and paint the requested tint
    // (defaulting to white). This makes glyphs composite over the page as
    // premultiplied coverage exactly like the engine draws them, instead of
    // a `source-in` fill turning every opaque cell into a solid box.
    const [tr, tg, tb, ta] = parseColor(tint);
    if (canvas.width > 0 && canvas.height > 0) {
      const buf  = ctx.getImageData(0, 0, canvas.width, canvas.height);
      const data = buf.data;
      for (let i = 0; i < data.length; i += 4) {
        const cover = Math.max(data[i], data[i + 1], data[i + 2]) / 255;
        const drawn = data[i + 3] / 255; // 0 where no glyph cell was blitted
        data[i]     = tr;
        data[i + 1] = tg;
        data[i + 2] = tb;
        data[i + 3] = Math.round(cover * drawn * ta * 255);
      }
      ctx.putImageData(buf, 0, 0);
    }
  }, [atlas, text, font, scale, padding, maxWidth, tint, charToIndex]);

  if (failed) {
    return (
      <div className={cn("text-xs text-destructive italic", className)}>
        failed to load atlas
      </div>
    );
  }
  return <canvas ref={canvasRef} className={cn("pixelated", className)} />;
}

interface FontGlyphTableProps {
  font: FontMeta;
  className?: string;
}

/** A 16-column grid of every cp1250 byte with a defined glyph.
 *  Each cell shows: the rendered bitmap glyph + the cp1250 byte +
 *  the Unicode codepoint label. */
export function FontGlyphTable({ font, className }: FontGlyphTableProps) {
  const printable = useMemo(
    () => font.characters.filter(c => c.unicodeChar !== null && c.width > 0),
    [font],
  );
  return (
    <div className={cn(
      "grid grid-cols-[repeat(auto-fill,minmax(56px,1fr))] gap-1.5",
      className,
    )}>
      {printable.map(c => (
        <FontGlyphCell key={c.index} font={font} char={c} />
      ))}
    </div>
  );
}

function FontGlyphCell({ font, char }: { font: FontMeta; char: typeof font.characters[number] }) {
  if (!char.unicodeChar) return null;
  const cpHex = (char.unicodeCodepoint ?? 0).toString(16).padStart(4, "0").toUpperCase();
  // Tooltip carries the canonical U+XXXX label so the info is still
  // one hover away when we need to disambiguate look-alikes (e.g.
  // U+00A0 vs U+0020, or the two µ codepoints).
  return (
    <div
      className="flex flex-col items-center gap-0.5 rounded-md border border-border bg-card/40 p-1.5 text-center"
      title={`cp1250 0x${char.index.toString(16).padStart(2, "0").toUpperCase()}  ·  U+${cpHex}`}
    >
      <FontPreview
        font={font}
        text={char.unicodeChar}
        scale={2}
        padding={0}
        className="h-auto"
      />
      <div className="font-mono text-[9px] leading-tight text-muted-foreground">
        0x{char.index.toString(16).padStart(2, "0").toUpperCase()}
      </div>
      <div className="text-[14px] leading-tight text-foreground/90">
        {char.unicodeChar}
      </div>
    </div>
  );
}
