// Dockview panels for the scene workbench.
//
// Each panel is a thin, store-driven view: it reads the Zustand workspace
// store (`@/scene/store`) and renders one concern (tree, preview, inspector,
// scripts). They are registered in the DockviewReact `components` map in
// `Workbench.tsx`. Global actions (scene select, save, undo/redo) live in the
// workbench app bar, not here.

import { createContext, useContext, useEffect, useState } from "react";
import { useShallow } from "zustand/react/shallow";
import { Plus, FileJson, FolderSearch, ChevronRight, ChevronDown } from "lucide-react";

import { cn } from "@/lib/utils";
import { useSceneStore, useRootIds, useNode } from "@/scene/store";
import { DEFAULT_VIEWPORT, type Kind } from "@/scene/model";
import { PreviewNode, Stage, TreeRow } from "@/scene/view";
import { Inspector } from "@/scene/inspector";
import { StatesSection } from "@/scene/states";
import { ScriptEditor } from "@/components/ScriptEditor";

/** Lets panels drive the dockview layout (e.g. the tree's code-behind links
 *  focus the Scripts panel). Provided by `Workbench` over the live api. */
export const WorkbenchCtx = createContext<{ focusPanel: (id: string) => void }>({ focusPanel: () => {} });

/** Shared datalist id for sprite-handle autocomplete (the <datalist> itself is
 *  rendered once by the Workbench from the handle manifest). */
export const ASSET_LIST_ID = "scene-editor-assets";

// Stable empty array so `useShallow` selectors that fall back to it don't churn.
const EMPTY: string[] = [];

function EmptyHint({ children }: { children: React.ReactNode }) {
  return <div className="flex h-full items-center justify-center p-4 text-center text-[11px] text-muted-foreground">{children}</div>;
}

// --------------------------------------------------------------- scene tree

export function SceneTreePanel() {
  const { focusPanel } = useContext(WorkbenchCtx);
  const hasDoc = useSceneStore((s) => s.meta != null);
  const scripts = useSceneStore(useShallow((s) => s.meta?.scripts ?? EMPTY));
  const rootIds = useRootIds();
  const active = useSceneStore((s) => s.active);
  const registry = useSceneStore((s) => s.registry);
  const openScript = useSceneStore((s) => s.openScript);
  // Selection/collapse are *not* read here — each TreeRow subscribes to its
  // own slice, so picking a node re-renders only the affected rows, not this
  // whole panel (and its recursive tree). The states CRUD section subscribes
  // independently (see <StatesSection />).
  const a = useSceneStore(useShallow((s) => ({
    addNode: s.addNode, openScriptFile: s.openScriptFile,
  })));

  if (!hasDoc) return <EmptyHint>Pick a scene in the app bar to edit its control tree.</EmptyHint>;

  return (
    <div className="flex h-full min-h-0 flex-col bg-card/10">
      {/* palette */}
      <div className="flex flex-wrap gap-1 border-b border-border/60 p-1.5">
        {Object.keys(registry.kinds).map((k) => (
          <button key={k} type="button" onClick={() => a.addNode(k as Kind)}
            className="inline-flex items-center gap-0.5 rounded border border-border bg-card px-1.5 py-0.5 text-[10px] text-muted-foreground hover:text-foreground">
            <Plus className="size-3" />{k}
          </button>
        ))}
      </div>

      {/* tree */}
      <div className="min-h-0 flex-1 overflow-auto p-1">
        {rootIds.length
          ? rootIds.map((id) => <TreeRow key={id} id={id} depth={0} />)
          : <p className="p-2 text-[11px] text-muted-foreground">Empty scene — add a control above.</p>}
      </div>

      {/* states (full CRUD) */}
      <StatesSection />

      {/* code-behind */}
      {scripts.length > 0 && (
        <div className="border-t border-border/60 p-2">
          <div className="mb-1 flex items-center gap-1 text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground">
            <FileJson className="size-3" /> code-behind
          </div>
          <div className="space-y-0.5">
            {scripts.map((s) => (
              <button key={s} type="button"
                onClick={() => { if (active) a.openScriptFile(active.pak, s); focusPanel("scripts"); }}
                className={cn("block w-full truncate text-left font-mono text-[10px] hover:text-foreground",
                  openScript && active && openScript.pak === active.pak && openScript.rel === s
                    ? "text-sky-300" : "text-muted-foreground")}
                title={s}>
                {s.split("/").pop()}
              </button>
            ))}
          </div>
        </div>
      )}
    </div>
  );
}

// --------------------------------------------------------------- preview

export function PreviewPanel() {
  const hasDoc = useSceneStore((s) => s.meta != null);
  const viewport = useSceneStore(useShallow((s) => s.meta?.viewport ?? null));
  const rootIds = useRootIds();
  // Selection, per-node visibility and live drags live on each PreviewNode's
  // own subscriptions, so this panel only re-renders when the root list or the
  // viewport changes — never on a node edit, selection, or drag. `select` is a
  // stable action ref.
  const select = useSceneStore((s) => s.select);

  if (!hasDoc) return <EmptyHint>No scene loaded.</EmptyHint>;
  const stageW = viewport?.[0] ?? DEFAULT_VIEWPORT[0];
  const stageH = viewport?.[1] ?? DEFAULT_VIEWPORT[1];

  return (
    <div className="h-full min-h-0 overflow-auto p-3">
      <Stage width={stageW} height={stageH}>
        <div
          style={{ position: "relative", width: stageW, height: stageH, background: "#0b0f14", overflow: "hidden" }}
          onPointerDown={() => select(null)}
        >
          {rootIds.map((id) => <PreviewNode key={id} id={id} />)}
        </div>
      </Stage>
    </div>
  );
}

// --------------------------------------------------------------- inspector

export function InspectorPanel() {
  const hasDoc = useSceneStore((s) => s.meta != null);
  const selectedId = useSceneStore((s) => s.selectedId);
  // Subscribes to only the selected node's entry — re-renders when that node
  // changes (e.g. its live x/y on drop) or the selection moves, nothing else.
  const node = useNode(selectedId);
  const registry = useSceneStore((s) => s.registry);
  const updateSelected = useSceneStore((s) => s.updateSelected);
  const addState = useSceneStore((s) => s.addState);
  // Declared state names for the node "state" dropdown. `useShallow` keeps the
  // array stable while the names don't change, so the inspector isn't re-run by
  // unrelated state edits (default toggles, etc.).
  const stateNames = useSceneStore(useShallow((s) => (s.meta?.states ?? []).map((x) => x.name)));

  return (
    <div className="h-full min-h-0 overflow-auto bg-card/10 p-2">
      {node
        ? <Inspector node={node} registry={registry} assetList={ASSET_LIST_ID}
            states={stateNames} onAddState={addState} onChange={updateSelected} />
        : <p className="p-2 text-[11px] text-muted-foreground">{hasDoc ? "Select a node to edit its properties." : "No scene loaded."}</p>}
    </div>
  );
}

// --------------------------------------------------------------- scripts

/** A collapsible per-pak group in the script browser. */
function ScriptPakGroup({
  pak, scripts, openScript, onOpen,
}: {
  pak: string; scripts: string[];
  openScript: { pak: string; rel: string } | null;
  onOpen: (pak: string, rel: string) => void;
}) {
  const [open, setOpen] = useState(true);
  return (
    <div className="mb-1">
      <button type="button" onClick={() => setOpen((v) => !v)}
        className="flex w-full items-center gap-1 text-muted-foreground hover:text-foreground">
        {open ? <ChevronDown className="size-3" /> : <ChevronRight className="size-3" />}
        <span className="font-bold uppercase tracking-[0.06em]">{pak}</span>
        <span className="text-[9px] opacity-60">{scripts.length}</span>
      </button>
      {open && (
        <div className="ml-3 mt-0.5 space-y-0.5">
          {scripts.map((rel) => (
            <button key={rel} type="button" onClick={() => onOpen(pak, rel)}
              className={cn("block w-full truncate text-left font-mono hover:text-foreground",
                openScript && openScript.pak === pak && openScript.rel === rel ? "text-sky-300" : "text-muted-foreground")}
              title={rel}>
              {rel}
            </button>
          ))}
        </div>
      )}
    </div>
  );
}

export function ScriptsPanel() {
  const hasDoc = useSceneStore((s) => s.meta != null);
  const scripts = useSceneStore(useShallow((s) => s.meta?.scripts ?? EMPTY));
  const active = useSceneStore((s) => s.active);
  const openScript = useSceneStore((s) => s.openScript);
  const pakScripts = useSceneStore((s) => s.pakScripts);
  const a = useSceneStore(useShallow((s) => ({
    openScriptFile: s.openScriptFile, loadPakScripts: s.loadPakScripts,
  })));
  const [browse, setBrowse] = useState(false);

  // Index every pak's `.luau` the first time the browser opens.
  useEffect(() => { if (browse) void a.loadPakScripts(); }, [browse, a]);

  if (!hasDoc) return <EmptyHint>No scene loaded.</EmptyHint>;

  const isOpen = (pak: string, rel: string) =>
    !!openScript && openScript.pak === pak && openScript.rel === rel;

  return (
    <div className="flex h-full min-h-0 flex-col">
      {/* code-behind tabs + browse toggle */}
      <div className="flex flex-wrap items-center gap-1 border-b border-border/60 p-1.5">
        {active && scripts.map((rel) => (
          <button key={rel} type="button" onClick={() => a.openScriptFile(active.pak, rel)}
            className={cn("rounded px-2 py-0.5 font-mono text-[10px]",
              isOpen(active.pak, rel) ? "bg-accent text-foreground" : "bg-card text-muted-foreground hover:text-foreground")}>
            {rel.split("/").pop()}
          </button>
        ))}
        <button type="button" onClick={() => setBrowse((v) => !v)}
          className={cn("ml-auto inline-flex items-center gap-1 rounded px-2 py-0.5 text-[10px]",
            browse ? "bg-accent text-foreground" : "bg-card text-muted-foreground hover:text-foreground")}>
          <FolderSearch className="size-3" /> browse
        </button>
      </div>

      {/* pak-wide script browser */}
      {browse && (
        <div className="max-h-48 overflow-auto border-b border-border/60 p-1.5 text-[10px]">
          {Object.keys(pakScripts).length === 0
            ? <p className="text-muted-foreground">indexing pak scripts…</p>
            : Object.entries(pakScripts).map(([pak, scripts]) => (
              <ScriptPakGroup key={pak} pak={pak} scripts={scripts} openScript={openScript} onOpen={a.openScriptFile} />
            ))}
        </div>
      )}

      <div className="min-h-0 flex-1">
        {openScript
          ? <ScriptEditor key={`${openScript.pak}:${openScript.rel}`} pak={openScript.pak} rel={openScript.rel} />
          : <EmptyHint>Pick a script above, or open the browser to edit any pak <code>.luau</code>.</EmptyHint>}
      </div>
    </div>
  );
}
