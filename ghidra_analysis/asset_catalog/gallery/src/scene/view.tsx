// Presentational scene-view widgets shared by the workbench panels.
//
// Pure rendering of the control tree: the CSS-laid-out preview (mirrors taffy),
// the static-frame sprite/text renderers, the design stage, and the scene-tree
// row. State + actions live in `@/scene/store`; these components only take
// props so they can be reused by the Preview and SceneTree dockview panels.

import { memo, useEffect, useMemo, useRef, useState } from "react";
import { ChevronRight, ChevronDown } from "lucide-react";

import { useDataSource, useOverlayUrl } from "@/data/DataSourceContext";
import { FontPreview } from "@/components/FontRenderer";
import { cn } from "@/lib/utils";
import { type FlatNode, fontBaseline, rgba, useSceneAssets } from "@/scene/model";
import {
  useSceneStore, useNode, useNodeVisible, useIsSelected, useChildIds,
  useCollapsed, useDragOffset,
} from "@/scene/store";

// --------------------------------------------------------------- preview

/** Render one control as a positioned DOM box. CSS flex/absolute layout
 *  mirrors taffy, so the preview matches the engine without a layout pass.
 *
 *  PERF: the node is addressed by `id` and subscribes to *only* its own
 *  normalized slices — its node entry (`useNode`), visibility, selection, child
 *  id list, and live drag offset. Editing one node, selecting it, or dragging
 *  it re-renders just that node (and, for a drag, nothing else at all), never
 *  the tree. `memo` keeps a parent re-render (e.g. a root reorder) from
 *  re-rendering unchanged subtrees. A live drag paints a transient `translate`
 *  on top of the committed x/y; the move lands in the document on pointer-up. */
export const PreviewNode = memo(function PreviewNode({ id }: { id: string }) {
  const n = useNode(id);
  const vis = useNodeVisible(id);
  const selected = useIsSelected(id);
  const childIds = useChildIds(id);
  const off = useDragOffset(id);
  const select = useSceneStore((s) => s.select);
  const beginDrag = useSceneStore((s) => s.beginDrag);
  const dragBy = useSceneStore((s) => s.dragBy);
  const endDrag = useSceneStore((s) => s.endDrag);
  const drag = useRef<{ x: number; y: number } | null>(null);
  if (!n || !vis) return null;

  const absolute = n.layout.mode !== "flex" && n.layout.mode !== "block";
  // Compose the transient drag translate (design px) with the node's own
  // scale/rotate. translate is listed first so it moves the (already
  // scaled/rotated) box by the raw delta in the parent's coordinate space.
  const transforms: string[] = [];
  if (off.dx !== 0 || off.dy !== 0) transforms.push(`translate(${off.dx}px,${off.dy}px)`);
  if (n.sx !== 1 || n.sy !== 1) transforms.push(`scale(${n.sx},${n.sy})`);
  if (n.rot !== 0) transforms.push(`rotate(${n.rot}deg)`);
  const style: React.CSSProperties = {
    position: absolute ? "absolute" : "relative",
    left: absolute ? n.layout.x : undefined,
    top: absolute ? n.layout.y : undefined,
    width: n.layout.width ?? undefined,
    height: n.layout.height ?? undefined,
    transform: transforms.length ? transforms.join(" ") : undefined,
    transformOrigin: "top left",
  };
  if (n.layout.mode === "flex") {
    style.display = "flex";
    style.flexDirection = n.layout.direction === "column" ? "column" : "row";
    style.gap = n.layout.gap;
    style.padding = n.layout.padding;
    style.justifyContent = mapJustify(n.layout.justify);
    style.alignItems = mapAlign(n.layout.align);
  }

  const onPointerDown = (e: React.PointerEvent) => {
    e.stopPropagation();
    select(id);
    if (!absolute) return;
    drag.current = { x: e.clientX, y: e.clientY };
    beginDrag(id);
    (e.target as HTMLElement).setPointerCapture(e.pointerId);
  };
  const onPointerMove = (e: React.PointerEvent) => {
    if (!drag.current) return;
    const scale = stageScale();
    const dx = (e.clientX - drag.current.x) / scale;
    const dy = (e.clientY - drag.current.y) / scale;
    if (Math.abs(dx) >= 1 || Math.abs(dy) >= 1) {
      dragBy(Math.round(dx), Math.round(dy));
      drag.current = { x: e.clientX, y: e.clientY };
    }
  };
  const onPointerUp = (e: React.PointerEvent) => {
    if (drag.current) { drag.current = null; endDrag(); }
    try { (e.target as HTMLElement).releasePointerCapture(e.pointerId); } catch { /* ignore */ }
  };

  const outline = selected ? "1px solid #6fb3ff" : undefined;

  return (
    <div
      style={{ ...style, outline, cursor: absolute ? "move" : "default" }}
      onPointerDown={onPointerDown}
      onPointerMove={onPointerMove}
      onPointerUp={onPointerUp}
    >
      {renderInner(n)}
      {childIds.map((cid) => <PreviewNode key={cid} id={cid} />)}
    </div>
  );
});

function renderInner(n: FlatNode): React.ReactNode {
  const w = n.layout.width ?? undefined;
  const h = n.layout.height ?? undefined;
  switch (n.kind) {
    case "quad":
      return <div style={{ position: "absolute", inset: 0, width: w, height: h ?? 12, background: rgba(n.color || n.tint) }} />;
    case "text":
      return <TextView n={n} />;
    case "sprite":
      return <SpriteView handle={n.handle} previewFrame={n.previewFrame} />;
    case "input_region":
      return <div style={{ position: "absolute", inset: 0, width: w ?? 40, height: h ?? 20, border: "1px dashed rgba(120,200,120,0.7)" }} />;
    case "button":
      return (
        <div style={{ width: w ?? 120, height: h ?? 30, background: rgba(n.color || "202c3aff"), color: "#e6edf3", fontSize: n.size, display: "flex", alignItems: "center", paddingLeft: 8, border: "1px solid rgba(255,255,255,0.15)" }}>
          {n.label || "button"}
        </div>
      );
    default:
      return null;
  }
}

function mapJustify(s: string): React.CSSProperties["justifyContent"] {
  return ({ center: "center", end: "flex-end", between: "space-between", around: "space-around", evenly: "space-evenly", stretch: "stretch" } as const)[s] ?? "flex-start";
}
function mapAlign(s: string): React.CSSProperties["alignItems"] {
  return ({ center: "center", end: "flex-end", stretch: "stretch", baseline: "baseline" } as const)[s] ?? "flex-start";
}

interface AtlasInfo { url: string; fw: number; fh: number; cols: number }

/** Derive the BitmapSprite atlas sidecar from a preview filename, e.g.
 *  `menu/menu_button.gif` -> `menu/menu_button.atlas.json`. Lets us crop a
 *  static frame even when the catalog entry doesn't list the sidecar.
 *
 *  Catalog previews for BitmapSprite are double-extension
 *  (`res_..._BitmapSprite.atlas.gif` / `.atlas.png`); collapse that whole
 *  sidecar suffix to `.atlas.json`. Replacing only the trailing extension
 *  would yield a non-existent `*.atlas.atlas.json`, the fetch would fail,
 *  and the view would fall back to rendering the animated gif. */
function deriveAtlas(file: string | null | undefined): string | null {
  if (!file) return null;
  if (/\.atlas\.(gif|png)$/i.test(file)) return file.replace(/\.atlas\.(gif|png)$/i, ".atlas.json");
  return file.replace(/\.[^./]+$/, ".atlas.json");
}

/** Renders a sprite control as a STATIC frame of the real catalog asset.
 *  Multi-frame atlas sprites (BitmapSprite) crop the strip PNG to the editor's
 *  `previewFrame` (no animated GIF); single-image sprites resolve to a
 *  native-size <img>. The cropped geometry is exactly what the engine draws,
 *  so positions/sizes match the game. */
function SpriteView({ handle, previewFrame }: { handle: string; previewFrame: number }) {
  const assets = useSceneAssets();
  const source = useDataSource();
  const entry = handle ? assets.resolve(handle) : null;

  const atlasCandidate = useMemo(() => {
    if (!entry) return null;
    const listed = (entry.files || []).find((f) => f.endsWith(".atlas.json"));
    return listed ?? deriveAtlas(entry.preview);
  }, [entry]);

  const imageFile = useMemo(() => {
    if (!entry) return null;
    if (entry.anim.kind === "frames" && entry.anim.frames.length) {
      const i = Math.max(0, Math.min(previewFrame, entry.anim.frames.length - 1));
      return entry.anim.frames[i];
    }
    return entry.preview ?? null;
  }, [entry, previewFrame]);
  const imageUrl = useOverlayUrl(imageFile);

  const [atlas, setAtlas] = useState<AtlasInfo | "loading" | "none">(atlasCandidate ? "loading" : "none");
  useEffect(() => {
    if (!atlasCandidate) { setAtlas("none"); return; }
    let cancelled = false;
    setAtlas("loading");
    (async () => {
      try {
        const bytes = await source.overlayBytes(atlasCandidate);
        const meta = JSON.parse(new TextDecoder().decode(bytes)) as
          { atlas: string; frameWidth: number; frameHeight: number; frameCount?: number };
        const url = await source.overlayUrl(meta.atlas);
        const cols = await new Promise<number>((resolve) => {
          const img = new Image();
          img.onload = () => resolve(Math.max(1, Math.round(img.naturalWidth / meta.frameWidth)));
          img.onerror = () => resolve(Math.max(1, meta.frameCount ?? 1));
          img.src = url;
        });
        if (!cancelled) setAtlas({ url, fw: meta.frameWidth, fh: meta.frameHeight, cols });
      } catch { if (!cancelled) setAtlas("none"); }
    })();
    return () => { cancelled = true; };
  }, [source, atlasCandidate]);

  if (atlas === "loading") return null;
  if (atlas !== "none") {
    const f = Math.max(0, previewFrame || 0);
    const col = f % atlas.cols, row = Math.floor(f / atlas.cols);
    return (
      <div style={{
        width: atlas.fw, height: atlas.fh,
        backgroundImage: `url(${atlas.url})`,
        backgroundPosition: `-${col * atlas.fw}px -${row * atlas.fh}px`,
        backgroundRepeat: "no-repeat",
        imageRendering: "pixelated",
      }} />
    );
  }
  if (imageUrl) {
    return <img src={imageUrl} alt={handle} draggable={false} style={{ display: "block", imageRendering: "pixelated" }} />;
  }
  return (
    <div style={{ minWidth: 24, minHeight: 16, background: "rgba(120,90,160,0.28)", border: "1px solid rgba(180,150,220,0.7)", color: "#cbb6e6", fontSize: 9, fontFamily: "ui-monospace, monospace", padding: 2, overflow: "hidden", whiteSpace: "nowrap" }}>
      {handle || "sprite"}{previewFrame ? `#${previewFrame}` : ""}
    </div>
  );
}

/** Renders a text control with the engine's actual bitmap font (picked by
 *  size) and tint, shifted so the glyph baseline sits on the node's y for
 *  baseline-anchored runs. */
function TextView({ n }: { n: FlatNode }) {
  const assets = useSceneAssets();
  const font = assets.pickFont(n.size || 12);
  const text = n.text || "";
  if (font?.fontMeta) {
    const top = n.anchor === "top" ? 0 : -fontBaseline(font.fontMeta);
    return (
      <div style={{ position: "relative", top, whiteSpace: "nowrap" }}>
        <FontPreview font={font.fontMeta} text={text || " "} scale={1} padding={0} tint={rgba(n.tint || "ffffffff")} />
      </div>
    );
  }
  return (
    <span style={{ color: rgba(n.tint || "ffffffff"), fontSize: n.size, lineHeight: 1, whiteSpace: "nowrap", fontFamily: "ui-monospace, monospace" }}>
      {text || "text"}
    </span>
  );
}

// Module-level so PreviewNode's pointer math can read the current scale
// without threading it through every node.
let _stageScale = 1;
const stageScale = () => _stageScale;

/** Scales the fixed-size design stage to fit its container, keeping the
 *  engine's design pixels 1:1 (so coordinates in the inspector match). */
export function Stage({ width, height, children }: { width: number; height: number; children: React.ReactNode }) {
  const ref = useRef<HTMLDivElement>(null);
  const [scale, setScale] = useState(1);
  useEffect(() => {
    const el = ref.current;
    if (!el) return;
    const ro = new ResizeObserver(() => {
      const w = el.clientWidth;
      const s = Math.min(2, Math.max(0.2, w / width));
      _stageScale = s;
      setScale(s);
    });
    ro.observe(el);
    return () => ro.disconnect();
  }, [width]);
  return (
    <div ref={ref} className="w-full">
      <div style={{ width: width * scale, height: height * scale }}>
        <div style={{ transform: `scale(${scale})`, transformOrigin: "top left" }}>
          {children}
        </div>
      </div>
    </div>
  );
}

// --------------------------------------------------------------- tree view

/** One scene-tree row. Like `PreviewNode`, it is addressed by `id` and
 *  subscribes to only its own slices (node entry, selection, collapse, child
 *  ids), and is memoized on `{ id, depth }` — so selecting or collapsing a node
 *  re-renders just the rows that actually changed instead of the entire tree. */
export const TreeRow = memo(function TreeRow({ id, depth }: { id: string; depth: number }) {
  const n = useNode(id);
  const selected = useIsSelected(id);
  const isCollapsed = useCollapsed(id);
  const childIds = useChildIds(id);
  const select = useSceneStore((s) => s.select);
  const toggleCollapse = useSceneStore((s) => s.toggleCollapse);
  const reparent = useSceneStore((s) => s.reparent);
  const [dropHint, setDropHint] = useState(false);
  if (!n) return null;
  const hasKids = childIds.length > 0;
  return (
    <>
      <div
        draggable
        onDragStart={(e) => { e.dataTransfer.setData("text/plain", id); e.dataTransfer.effectAllowed = "move"; }}
        onDragOver={(e) => { if (n.kind === "group") { e.preventDefault(); setDropHint(true); } }}
        onDragLeave={() => setDropHint(false)}
        onDrop={(e) => {
          e.preventDefault(); setDropHint(false);
          const k = e.dataTransfer.getData("text/plain");
          if (k) reparent(k, id);
        }}
        className={cn(
          "flex w-full items-center gap-1 rounded px-1 py-0.5 text-[11px] hover:bg-accent",
          selected && "bg-accent text-foreground",
          dropHint && "ring-1 ring-sky-400",
        )}
        style={{ paddingLeft: 4 + depth * 12 }}
      >
        <button type="button" onClick={() => toggleCollapse(id)} className="text-muted-foreground" style={{ visibility: hasKids ? "visible" : "hidden" }}>
          {isCollapsed ? <ChevronRight className="size-3" /> : <ChevronDown className="size-3" />}
        </button>
        <button type="button" onClick={() => select(id)} className="flex flex-1 items-center gap-1.5 text-left">
          <span className="font-mono text-muted-foreground">{n.kind}</span>
          {n.name && <span className="text-foreground">{n.name}</span>}
          {n.kind === "button" && n.label && <span className="text-muted-foreground">"{n.label}"</span>}
          {n.kind === "text" && n.text && <span className="text-muted-foreground truncate max-w-28">"{n.text}"</span>}
          {n.state && <span className="ml-auto rounded bg-sky-500/15 px-1 text-[9px] text-sky-300">{n.state}</span>}
        </button>
      </div>
      {!isCollapsed && childIds.map((cid) => (
        <TreeRow key={cid} id={cid} depth={depth + 1} />
      ))}
    </>
  );
});
