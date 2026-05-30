import { memo, useCallback, useEffect, useMemo, useRef, useState } from "react";
import type { CatalogEntry, CatalogPayload } from "./types";
import { UNSORTED_FOLDER } from "./types";
import { Sidebar, type Filters } from "./components/Sidebar";
import { TileGrid } from "./components/TileGrid";
import { DetailSheet } from "./components/DetailSheet";
import { SourcePicker } from "./components/SourcePicker";
import { DataSourceProvider } from "./data/DataSourceContext";
import { autodetectSource } from "./data/sources/detect";
import { buildPayload, reconcilePayload } from "./data/payload";
import type { DataSource } from "./data/DataSource";
import type { RawCatalog, RawCoverage, RawRegistry } from "./data/raw";
import type { HandleManifest } from "./data/handleManifest";
import { Button } from "./components/ui/button";
import { clearPersistedHandle } from "./data/sources/persist";
import { FolderOpen, ServerCog, Images, Layers } from "lucide-react";
import {
  EditableRegistryProvider, SaveStateProvider, useEditableRegistryStore,
} from "./data/EditableRegistry";
import { EngineBridgeProvider } from "./data/EngineBridge";
import { SceneWorkbench } from "./scene/Workbench";
import { cn } from "./lib/utils";

type AppView = "assets" | "scenes";

interface LoadedBundle {
  raw:      RawCatalog;
  registry: RawRegistry;
  coverage: RawCoverage;
  overlay:  string[];
  payload:  CatalogPayload;
  /** Codegen handle table for live-engine pushes; null when the source
   *  can't reach `manifest.json`. */
  manifest: HandleManifest | null;
}

type BootState =
  | { kind: "init" }
  | { kind: "picking"; reason: string | null }
  | { kind: "loading"; source: DataSource }
  | { kind: "ready";   source: DataSource; bundle: LoadedBundle }
  | { kind: "error";   source: DataSource | null; message: string };

export default function App() {
  const [boot, setBoot] = useState<BootState>({ kind: "init" });

  // ------------------------------------------------------------------ boot
  useEffect(() => {
    let cancelled = false;
    (async () => {
      const { source, reason } = await autodetectSource();
      if (cancelled) return;
      if (source) setBoot({ kind: "loading", source });
      else        setBoot({ kind: "picking", reason });
    })();
    return () => { cancelled = true; };
  }, []);

  // ----------------------------------------------------------- load payload
  useEffect(() => {
    if (boot.kind !== "loading") return;
    const { source } = boot;
    let cancelled = false;
    (async () => {
      try {
        const [raw, registry, coverage, overlay, manifest] = await Promise.all([
          source.loadCatalog(),
          source.loadRegistry(),
          source.loadCoverage(),
          source.listOverlayFiles(),
          source.loadManifest?.() ?? Promise.resolve(null),
        ]);
        const payload = await buildPayload(raw, registry, coverage, overlay, {
          readFile: (fn) => source.overlayBytes(fn),
        });
        if (!cancelled) {
          setBoot({ kind: "ready", source, bundle: { raw, registry, coverage, overlay, payload, manifest } });
        }
      } catch (err) {
        if (!cancelled) setBoot({ kind: "error", source, message: String(err) });
      }
    })();
    return () => { cancelled = true; };
  }, [boot]);

  const reset = useCallback(async () => {
    // dispose old source's blob URLs / caches first
    if (boot.kind === "ready" || boot.kind === "loading" || boot.kind === "error") {
      boot.source?.dispose?.();
    }
    await clearPersistedHandle();
    setBoot({ kind: "picking", reason: null });
  }, [boot]);

  const onSourcePicked = useCallback((source: DataSource) => {
    setBoot({ kind: "loading", source });
  }, []);

  // ---------------------------------------------------------------- render
  if (boot.kind === "init") {
    return <FullPageStatus label="Looking for assets…" />;
  }
  if (boot.kind === "picking") {
    return <SourcePicker reason={boot.reason} onSourceReady={onSourcePicked} />;
  }
  if (boot.kind === "loading") {
    return <FullPageStatus label={`Loading catalog from ${boot.source.info.label}…`} />;
  }
  if (boot.kind === "error") {
    return (
      <FullPageError
        title="Failed to load catalog"
        message={boot.message}
        sourceLabel={boot.source?.info.label}
        onPickAgain={reset}
      />
    );
  }
  return <ReadyApp source={boot.source} bundle={boot.bundle} onChangeSource={reset} />;
}

// ---------------------------------------------------------------------------
// the actual app (only mounted once a source + payload are ready)
// ---------------------------------------------------------------------------

function ReadyApp({ source, bundle, onChangeSource }: {
  source: DataSource;
  bundle: LoadedBundle;
  onChangeSource: () => void;
}) {
  // Editable registry — owns the mutable copy + debounced save.  All
  // tree-edit / detail-edit actions route through `editable.*Action`.
  // `save` is kept *separate* from `editable` so its short-lived
  // transitions (dirty→saving→saved→idle) only re-render the save badge
  // via context, not every `editable` consumer.
  const { editable, save } = useEditableRegistryStore(bundle.registry, source);

  // Re-derive the payload whenever the editable registry changes.  We
  // deliberately reuse the static `raw` + `coverage` + `overlay`
  // listings from the initial load — only the registry mutates at
  // runtime, and per-asset font/poem sidecars hit the DataSource's
  // own cache so subsequent builds are cheap.
  const [payload, setPayload] = useState<CatalogPayload>(bundle.payload);
  const payloadRef = useRef<CatalogPayload>(bundle.payload);
  useEffect(() => {
    let cancelled = false;
    (async () => {
      const built = await buildPayload(bundle.raw, editable.registry, bundle.coverage, bundle.overlay, {
        readFile: (fn) => source.overlayBytes(fn),
      });
      if (cancelled) return;
      // Splice prior object identities back in for unchanged entries so
      // moving one asset doesn't re-render the entire gallery + tree.
      const next = reconcilePayload(payloadRef.current, built);
      payloadRef.current = next;
      setPayload(next);
    })();
    return () => { cancelled = true; };
  }, [editable.registry, bundle.raw, bundle.coverage, bundle.overlay, source]);

  const [filters, setFilters] = useState<Filters>({
    named: null, category: null, class: null, folder: null, search: "",
  });
  const [selectedKey, setSelectedKey] = useState<string | null>(null);

  // Stable folder-filter setter — passed deep into the folder tree so
  // rows don't have to close over the whole (changing) `filters` object.
  const setFolder = useCallback(
    (folder: string | null) => setFilters((f) => ({ ...f, folder })),
    [],
  );
  const handleDetailOpenChange = useCallback((open: boolean) => {
    if (!open) setSelectedKey(null);
  }, []);
  const selectCategory = useCallback((cat: string) => {
    setFilters((f) => ({ ...f, category: cat }));
    setSelectedKey(null);
  }, []);
  const selectFolderFromDetail = useCallback((folder: string) => {
    setFilters((f) => ({ ...f, folder }));
    setSelectedKey(null);
  }, []);

  const visible = useMemo(() => {
    const q = filters.search.toLowerCase().trim();
    const folderPrefix = filters.folder && filters.folder !== UNSORTED_FOLDER
      ? `${filters.folder}/` : null;
    return payload.entries.filter((e: CatalogEntry) => {
      if (filters.named === "named"   && !e.slug) return false;
      if (filters.named === "unnamed" &&  e.slug) return false;
      if (filters.category && !e.categories.includes(filters.category)) return false;
      if (filters.class    && e.className !== filters.class) return false;
      // Folder filter — prefix match so clicking a parent path acts
      // as "show this subtree".  `__unsorted__` matches entries that
      // have no folder set.
      if (filters.folder) {
        if (filters.folder === UNSORTED_FOLDER) {
          if (e.folder) return false;
        } else if (e.folder !== filters.folder
                && !(folderPrefix && e.folder?.startsWith(folderPrefix))) {
          return false;
        }
      }
      if (q) {
        const hay = [
          e.idHex, String(e.id), e.slug || "", e.notes || "",
          (e.tags || []).join(" "), e.className, e.categories.join(" "),
          e.folder || "", e.poemText || "",
        ].join(" ").toLowerCase();
        if (!hay.includes(q)) return false;
      }
      return true;
    });
  }, [payload, filters]);

  const selected = selectedKey
    ? payload.entries.find(e => e.key === selectedKey) ?? null
    : null;

  const [view, setView] = useState<AppView>("assets");

  return (
    <DataSourceProvider source={source}>
     <EditableRegistryProvider value={editable}>
      <SaveStateProvider value={save}>
      <EngineBridgeProvider>
      <div className="flex h-screen flex-col overflow-hidden">
        <ViewTabs view={view} onChange={setView} />
        {view === "assets" ? (
          <div className="grid min-h-0 flex-1 grid-cols-[300px_1fr] overflow-hidden">
            <Sidebar
              payload={payload}
              filters={filters}
              onFiltersChange={setFilters}
              onFolderChange={setFolder}
              visibleCount={visible.length}
              sourceInfo={source.info}
              onChangeSource={onChangeSource}
            />
            <main className="min-h-0 overflow-hidden">
              <TileGrid
                entries={visible}
                selectedKey={selectedKey}
                onSelect={setSelectedKey}
                writable={editable.writable}
              />
            </main>
            <DetailSheet
              entry={selected}
              manifest={bundle.manifest}
              scriptRegistry={payload.scriptRegistry}
              folderMeta={payload.folderMeta}
              open={!!selected}
              onOpenChange={handleDetailOpenChange}
              onCategoryClick={selectCategory}
              onFolderClick={selectFolderFromDetail}
            />
          </div>
        ) : (
          <div className="min-h-0 flex-1 overflow-hidden">
            <SceneWorkbench manifest={bundle.manifest} entries={payload.entries} />
          </div>
        )}
      </div>
      </EngineBridgeProvider>
      </SaveStateProvider>
     </EditableRegistryProvider>
    </DataSourceProvider>
  );
}

const ViewTabs = memo(function ViewTabs({ view, onChange }: { view: AppView; onChange: (v: AppView) => void }) {
  return (
    <div className="flex shrink-0 items-center gap-1 border-b border-border/60 bg-card/30 px-2 py-1">
      {([
        { id: "assets" as const, label: "Assets", icon: Images },
        { id: "scenes" as const, label: "Scenes", icon: Layers },
      ]).map(({ id, label, icon: Icon }) => (
        <button
          key={id}
          type="button"
          onClick={() => onChange(id)}
          className={cn(
            "inline-flex items-center gap-1.5 rounded px-2.5 py-1 text-xs font-medium transition-colors",
            view === id
              ? "bg-accent text-foreground"
              : "text-muted-foreground hover:bg-accent/50 hover:text-foreground",
          )}
        >
          <Icon className="size-3.5" /> {label}
        </button>
      ))}
    </div>
  );
});

function FullPageStatus({ label }: { label: string }) {
  return (
    <div className="flex h-screen items-center justify-center">
      <div className="text-sm text-muted-foreground animate-pulse">{label}</div>
    </div>
  );
}

function FullPageError({ title, message, sourceLabel, onPickAgain }: {
  title: string; message: string; sourceLabel?: string; onPickAgain: () => void;
}) {
  return (
    <div className="flex h-screen items-center justify-center p-8 text-center">
      <div className="max-w-lg space-y-4">
        <h1 className="text-xl font-semibold text-destructive">{title}</h1>
        {sourceLabel && (
          <p className="text-xs uppercase tracking-wider text-muted-foreground flex items-center justify-center gap-1.5">
            <ServerCog className="size-3.5" /> {sourceLabel}
          </p>
        )}
        <p className="break-all rounded-md bg-card/40 p-3 text-left text-xs font-mono text-muted-foreground">
          {message}
        </p>
        <Button variant="outline" onClick={onPickAgain} className="gap-2">
          <FolderOpen className="size-4" /> Pick a different folder…
        </Button>
      </div>
    </div>
  );
}
