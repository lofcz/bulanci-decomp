// Scene-editor workbench shell.
//
// A DockviewReact docking workbench (panes/tabs/splitters + persisted layout)
// hosting the four scene panels (tree, preview, inspector, scripts). The
// global, document-level actions — scene selection, undo/redo, save, live
// push, duplicate/delete — live in the app bar above the dock; the panels
// themselves are pure views over the Zustand workspace store.
//
// All panels read the store, so the workbench only has to: inject the active
// data source + live bridge into the store (`configure`), boot the registry +
// scene list, own the keyboard shortcuts, and provide the SceneAssets context
// (so the preview renders real catalog art) plus a tiny api context (so the
// tree's code-behind links can focus the Scripts panel).

import { useCallback, useEffect, useMemo, useRef } from "react";
import { useShallow } from "zustand/react/shallow";
import {
  DockviewReact, type DockviewApi, type DockviewReadyEvent, themeAbyss,
} from "dockview-react";
import {
  Plus, RefreshCw, Save, UploadCloud, Copy, Trash2, Undo2, Redo2,
  Layers, Loader2, Check, AlertTriangle, Zap,
} from "lucide-react";
import { Select, SelectItem } from "@galacean/editor-ui";

import type { HandleManifest } from "@/data/handleManifest";
import type { CatalogEntry } from "@/types";
import { useDataSource } from "@/data/DataSourceContext";
import { useEngineBridge } from "@/data/EngineBridge";
import { Button } from "@/components/ui/button";
import { cn } from "@/lib/utils";
import { SceneAssetsCtx, buildSceneAssets } from "@/scene/model";
import { computeWritablePaks, useSceneStore } from "@/scene/store";
import {
  ASSET_LIST_ID, WorkbenchCtx,
  SceneTreePanel, PreviewPanel, InspectorPanel, ScriptsPanel,
} from "@/scene/panels";
import { LiveGamePanel } from "@/scene/LiveGamePanel";

// Bumped to v2 when the Live (embedded engine) panel was added, so existing
// users get it in their default layout instead of a stale saved layout.
const LAYOUT_KEY = "bulanci.scene.dockview.layout.v2";

const PANELS = {
  tree: SceneTreePanel,
  preview: PreviewPanel,
  live: LiveGamePanel,
  inspector: InspectorPanel,
  scripts: ScriptsPanel,
};

function defaultLayout(api: DockviewApi): void {
  api.addPanel({ id: "tree", component: "tree", title: "Scene" });
  api.addPanel({ id: "preview", component: "preview", title: "Design", position: { referencePanel: "tree", direction: "right" } });
  api.addPanel({ id: "live", component: "live", title: "Live", position: { referencePanel: "preview", direction: "within" } });
  api.addPanel({ id: "scripts", component: "scripts", title: "Scripts", position: { referencePanel: "preview", direction: "within" } });
  api.addPanel({ id: "inspector", component: "inspector", title: "Inspector", position: { referencePanel: "preview", direction: "right" } });
  api.getPanel("preview")?.api.setActive();
  try { api.getPanel("tree")?.group.api.setSize({ width: 260 }); } catch { /* sizing best-effort */ }
  try { api.getPanel("inspector")?.group.api.setSize({ width: 320 }); } catch { /* sizing best-effort */ }
}

export function SceneWorkbench({ manifest, entries }: { manifest: HandleManifest | null; entries: CatalogEntry[] }) {
  const source = useDataSource();
  const bridge = useEngineBridge();
  const sceneAssets = useMemo(() => buildSceneAssets(entries), [entries]);
  const apiRef = useRef<DockviewApi | null>(null);

  const configure = useSceneStore((s) => s.configure);
  const loadRegistry = useSceneStore((s) => s.loadRegistry);
  const reloadScenes = useSceneStore((s) => s.reloadScenes);
  const undo = useSceneStore((s) => s.undo);
  const redo = useSceneStore((s) => s.redo);

  // Inject the React data-source/bridge services into the store, then boot the
  // control registry + scene list. Re-runs when the active source/bridge swap.
  useEffect(() => {
    configure({ source, bridge });
    void loadRegistry();
    void reloadScenes();
  }, [configure, loadRegistry, reloadScenes, source, bridge]);

  // ---- undo/redo keyboard shortcuts (ignore editable targets) -------------
  useEffect(() => {
    const onKey = (e: KeyboardEvent) => {
      const mod = e.metaKey || e.ctrlKey;
      if (!mod) return;
      const t = e.target as HTMLElement | null;
      if (t && (t.tagName === "INPUT" || t.tagName === "TEXTAREA" || t.isContentEditable || t.closest(".monaco-editor"))) return;
      const k = e.key.toLowerCase();
      if (k === "z" && !e.shiftKey) { e.preventDefault(); undo(); }
      else if ((k === "z" && e.shiftKey) || k === "y") { e.preventDefault(); redo(); }
    };
    window.addEventListener("keydown", onKey);
    return () => window.removeEventListener("keydown", onKey);
  }, [undo, redo]);

  const onReady = useCallback((event: DockviewReadyEvent) => {
    apiRef.current = event.api;
    let restored = false;
    const saved = localStorage.getItem(LAYOUT_KEY);
    if (saved) {
      try { event.api.fromJSON(JSON.parse(saved)); restored = true; }
      catch { restored = false; }
    }
    if (!restored || event.api.panels.length === 0) {
      event.api.clear();
      defaultLayout(event.api);
    }
    event.api.onDidLayoutChange(() => {
      try { localStorage.setItem(LAYOUT_KEY, JSON.stringify(event.api.toJSON())); } catch { /* quota */ }
    });
  }, []);

  const focusPanel = useCallback((id: string) => {
    const p = apiRef.current?.getPanel(id);
    p?.api.setActive();
  }, []);
  const wbCtx = useMemo(() => ({ focusPanel }), [focusPanel]);

  // Sprite-handle autocomplete options from the codegen handle manifest.
  const assetOptions = useMemo(() => {
    if (!manifest) return [];
    const seen = new Set<string>();
    for (const rec of Object.values(manifest)) {
      const key = rec.folder ? `${rec.folder}/${rec.slug}` : rec.slug;
      if (key) seen.add(key);
    }
    return [...seen].sort().slice(0, 3000);
  }, [manifest]);

  return (
    <SceneAssetsCtx.Provider value={sceneAssets}>
      <WorkbenchCtx.Provider value={wbCtx}>
        <div className="flex h-full min-h-0 flex-col">
          <AppBar />
          <div className="relative min-h-0 flex-1">
            <DockviewReact components={PANELS} onReady={onReady} theme={themeAbyss} />
          </div>
        </div>
        <datalist id={ASSET_LIST_ID}>
          {assetOptions.map((opt) => <option key={opt} value={opt} />)}
        </datalist>
      </WorkbenchCtx.Provider>
    </SceneAssetsCtx.Provider>
  );
}

// --------------------------------------------------------------- app bar

// Auto-managed live-engine indicator: the bridge probes the studio broker on
// its own (both web tabs and native dev clients subscribe to it), so this is a
// passive read-out, not a connect/disconnect control. It also surfaces the
// Vite-style "rebuilding" state while a full-doc re-sync is in flight.
function EngineChip() {
  const { status, clients } = useEngineBridge();
  const rebuilding = useSceneStore((s) => s.rebuilding);
  let tone = "bg-muted-foreground/40", label = "offline", title = "no running game found";
  if (status.kind === "online") {
    if (rebuilding) {
      tone = "bg-sky-400 animate-pulse";
      label = "rebuilding";
      title = "Re-syncing the full scene to the attached game (new client or HMR fallback)";
    } else {
      tone = "bg-emerald-400";
      label = clients > 1 ? `live · ${clients}` : "live";
      title = `Live engine attached (${clients} game${clients === 1 ? "" : "s"}) — scene edits hot-reload in place`;
    }
  } else if (status.kind === "waiting") {
    tone = "bg-amber-400 animate-pulse"; label = "no game"; title = status.detail;
  } else if (status.kind === "connecting") {
    tone = "bg-amber-400 animate-pulse"; label = "…"; title = "Detecting a running game…";
  } else {
    title = status.error;
  }
  return (
    <span
      className="inline-flex shrink-0 items-center gap-1 rounded border border-border/60 px-1.5 py-0.5 text-[10px] text-muted-foreground"
      title={title}
    >
      <span className={cn("size-2 rounded-full", tone)} /> {label}
    </span>
  );
}

function AppBar() {
  const scenes = useSceneStore((s) => s.scenes);
  const active = useSceneStore((s) => s.active);
  // PERF: with the normalized store the toolbar never subscribes to node data
  // at all — it reads only the *facts* it displays, each a stable scalar during
  // a drag: whether a doc is loaded (`meta != null`) and whether a node is
  // selected (selection is cleared by `sync` when its node disappears, so
  // `selectedId != null` ⇒ a live node is selected). A drag touches only the
  // isolated drag slice, so the app bar (heavy Selects included) never re-runs.
  const hasDoc = useSceneStore((s) => s.meta != null);
  const hasSelection = useSceneStore((s) => s.selectedId != null);
  const saveTarget = useSceneStore((s) => s.saveTarget);
  const busy = useSceneStore((s) => s.busy);
  const msg = useSceneStore((s) => s.msg);
  const err = useSceneStore((s) => s.err);
  const canUndo = useSceneStore((s) => s.canUndo);
  const canRedo = useSceneStore((s) => s.canRedo);
  const dirty = useSceneStore((s) => s.dirty);
  const liveMode = useSceneStore((s) => s.liveMode);
  const source = useSceneStore((s) => s.source);
  const bridge = useSceneStore((s) => s.bridge);

  const a = useSceneStore(useShallow((s) => ({
    loadScene: s.loadScene, createScene: s.createScene, reloadScenes: s.reloadScenes,
    setSaveTarget: s.setSaveTarget, undo: s.undo, redo: s.redo, save: s.save,
    duplicateSelected: s.duplicateSelected, deleteSelected: s.deleteSelected,
    setLiveMode: s.setLiveMode,
  })));

  const writablePaks = useMemo(() => computeWritablePaks(scenes, bridge, active), [scenes, bridge, active]);
  const writable = !!source?.writePakText && (!!source?.info.writable || source?.info.kind === "http");

  const sceneKey = active ? `${active.pak}::${active.name}` : undefined;
  const onSelectScene = (value: string) => {
    const ref = scenes.find((s) => `${s.pak}::${s.name}` === value);
    if (ref) void a.loadScene(ref);
  };

  return (
    <div className="flex shrink-0 items-center gap-2 border-b border-border/60 bg-card/30 px-2 py-1.5">
      <Layers className="size-3.5 shrink-0 text-sky-400/80" />
      <div className="w-64 shrink-0">
        <Select size="sm" value={sceneKey} valueType="string"
          placeholder={scenes.length ? "select scene…" : "no scenes found"}
          onValueChange={onSelectScene}>
          {scenes.map((s) => (
            <SelectItem key={`${s.pak}::${s.name}`} value={`${s.pak}::${s.name}`}>
              {s.pak} / {s.name}{s.kind === "module" ? " (legacy)" : ""}
            </SelectItem>
          ))}
        </Select>
      </div>
      <IconBtn title="new scene" onClick={() => void a.createScene()}><Plus className="size-3.5" /></IconBtn>
      <IconBtn title="rescan paks" onClick={() => void a.reloadScenes()}><RefreshCw className="size-3.5" /></IconBtn>

      <div className="mx-1 h-5 w-px bg-border/60" />

      <div className="flex overflow-hidden rounded border border-border">
        <IconBtn title="Undo (Ctrl+Z)" disabled={!canUndo} onClick={() => a.undo()} bare><Undo2 className="size-3.5" /></IconBtn>
        <IconBtn title="Redo (Ctrl+Shift+Z)" disabled={!canRedo} onClick={() => a.redo()} bare><Redo2 className="size-3.5" /></IconBtn>
      </div>

      {hasDoc && writablePaks.length > 1 && (
        <label className="flex items-center gap-1 text-[10px] text-muted-foreground">
          to
          <div className="w-40">
            <Select size="sm" value={saveTarget || writablePaks[0]} valueType="string"
              onValueChange={(p: string) => a.setSaveTarget(p)}>
              {writablePaks.map((p) => (
                <SelectItem key={p} value={p}>{p}{active && p !== active.pak ? " (override)" : ""}</SelectItem>
              ))}
            </Select>
          </div>
        </label>
      )}

      <Button type="button" size="sm" onClick={() => void a.save(false)} disabled={busy || !hasDoc || !writable} className="h-7 gap-1.5">
        {busy ? <Loader2 className="size-3.5 animate-spin" /> : <Save className="size-3.5" />} save{dirty ? " *" : ""}
      </Button>
      <Button type="button" size="sm" variant="outline" onClick={() => void a.save(true)} disabled={busy || !hasDoc || !writable} className="h-7 gap-1.5">
        <UploadCloud className="size-3.5" /> save + live
      </Button>

      <Button
        type="button" size="sm"
        variant={liveMode ? "default" : "outline"}
        onClick={() => a.setLiveMode(!liveMode)}
        disabled={!hasDoc}
        title={liveMode
          ? "Live HMR on — edits stream to the attached game (transient; use Save to persist). The connection is auto-managed."
          : "Stream edits to the running game in place (no rebuild), preserving its state. Auto-connects to whatever game is running."}
        className={cn("h-7 gap-1.5", liveMode && "bg-amber-500/90 text-black hover:bg-amber-500")}
      >
        <Zap className={cn("size-3.5", liveMode && "fill-current")} /> live{liveMode ? " ●" : ""}
      </Button>
      <EngineChip />

      {hasSelection && (
        <>
          <Button type="button" size="sm" variant="ghost" onClick={() => a.duplicateSelected()} className="h-7 gap-1.5 text-muted-foreground"><Copy className="size-3.5" /> dup</Button>
          <Button type="button" size="sm" variant="ghost" onClick={() => a.deleteSelected()} className="h-7 gap-1.5 text-muted-foreground hover:text-rose-300"><Trash2 className="size-3.5" /> del</Button>
        </>
      )}

      {msg && <span className="inline-flex min-w-0 items-center gap-1 truncate text-[11px] text-emerald-300"><Check className="size-3 shrink-0" /> {msg}</span>}
      {err && <span className="inline-flex min-w-0 items-center gap-1 truncate text-[11px] text-rose-300"><AlertTriangle className="size-3 shrink-0" /> {err}</span>}
    </div>
  );
}

function IconBtn({ title, onClick, disabled, bare, children }: {
  title: string; onClick: () => void; disabled?: boolean; bare?: boolean; children: React.ReactNode;
}) {
  return (
    <button
      type="button" title={title} onClick={onClick} disabled={disabled}
      className={cn(
        "inline-flex items-center justify-center text-muted-foreground hover:text-foreground disabled:opacity-30",
        bare ? "px-1.5 py-1" : "size-7 rounded hover:bg-accent/50",
      )}
    >
      {children}
    </button>
  );
}
