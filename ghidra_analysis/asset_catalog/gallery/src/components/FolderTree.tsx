// VS-Code-style folder tree built on `react-arborist`.
//
// Capabilities:
//   * virtualised rendering (handles a deep / wide tree without
//     mounting every row up front),
//   * collapse / expand chevrons (multi-level indent),
//   * inline rename (F2 / double-click — react-arborist's `onRename`),
//   * drag-and-drop reparenting between folders,
//   * right-click context menu (New / Rename / Delete / Edit notes)
//     via shadcn `ContextMenu`,
//   * accepts external HTML5 drops from `<Tile>` so a single asset
//     can be moved into a folder by drag,
//   * "(unsorted)" pseudo-folder at the bottom that's not editable,
//   * search-filter integration — clicking a node sets `filters.folder`.

import { useCallback, useMemo, useRef, useState, useEffect } from "react";
import { Tree, type NodeApi, type TreeApi, type NodeRendererProps } from "react-arborist";
import {
  ChevronRight, ChevronDown, Folder, FolderOpen,
  Plus, Pencil, Trash2, StickyNote, FileWarning, Box,
} from "lucide-react";

import type { CatalogPayload } from "@/types";
import { UNSORTED_FOLDER } from "@/types";
import { buildFolderTree, type FolderNode } from "@/data/registryEdits";
import { useEditableRegistry, useSaveState } from "@/data/EditableRegistry";
import { cn } from "@/lib/utils";
import {
  ContextMenu, ContextMenuTrigger, ContextMenuContent,
  ContextMenuItem, ContextMenuSeparator, ContextMenuLabel,
} from "@/components/ui/context-menu";

/** MIME type carried on `dataTransfer` when a `<Tile>` is dragged.
 *  Tile.tsx sets this and stores the asset's 0x-id as the value. */
export const ASSET_DRAG_TYPE = "application/x-bulanci-asset-id";

/** What we feed `react-arborist`. */
interface UiNode {
  id:        string;
  name:      string;
  /** Original folder path — empty string for the synthetic root. */
  path:      string;
  notes:     string | null;
  count:     number;
  children?: UiNode[];
  /** When true: the synthetic "(unsorted)" bucket — read-only,
   *  doesn't accept rename / delete / DnD reparenting. */
  unsorted?: boolean;
  /** Render flag — newly created folders open in rename mode. */
  initialEdit?: boolean;
}

interface Props {
  payload: CatalogPayload;
  /** Currently-filtered folder path (drives the row highlight). */
  activeFolder: string | null;
  /** Stable folder-only setter (no dependency on the wider filters). */
  onFolderChange: (folder: string | null) => void;
}

// ---------------------------------------------------------------------
// data shaping
// ---------------------------------------------------------------------

function projectTree(folderRoot: FolderNode, unsortedCount: number): UiNode[] {
  // react-arborist takes a forest, so we flatten the synthetic root's
  // children into the top level and append the "(unsorted)" bucket.
  const recurse = (n: FolderNode): UiNode => ({
    id:    n.path,
    name:  n.name,
    path:  n.path,
    notes: n.notes,
    count: n.assetCount,
    children: n.children.length
      ? n.children.map(recurse)
      : undefined,
  });
  const out = folderRoot.children.map(recurse);
  if (unsortedCount > 0) {
    out.push({
      id:       UNSORTED_FOLDER,
      name:     "(unsorted)",
      path:     UNSORTED_FOLDER,
      notes:    null,
      count:    unsortedCount,
      unsorted: true,
    });
  }
  return out;
}

// ---------------------------------------------------------------------
// component
// ---------------------------------------------------------------------

export function FolderTree({ payload, activeFolder, onFolderChange }: Props) {
  const editable = useEditableRegistry();
  const treeRef  = useRef<TreeApi<UiNode> | null>(null);
  const wrapperRef = useRef<HTMLDivElement | null>(null);
  const [size, setSize] = useState<{ width: number; height: number }>({ width: 0, height: 0 });

  // ResizeObserver — react-arborist needs explicit width/height for
  // its virtualisation, so we measure the container and feed those.
  useEffect(() => {
    if (!wrapperRef.current) return;
    const el = wrapperRef.current;
    const obs = new ResizeObserver(([entry]) => {
      const r = entry.contentRect;
      setSize({ width: r.width, height: r.height });
    });
    obs.observe(el);
    return () => obs.disconnect();
  }, []);

  const data = useMemo(() => {
    const folderRoot = buildFolderTree(editable.registry);
    return projectTree(folderRoot, payload.folderCounts[UNSORTED_FOLDER] ?? 0);
  }, [editable.registry, payload.folderCounts]);

  // Selection — react-arborist treats selection as an array; we want
  // single-select bound to the active folder filter.
  const selection = activeFolder ?? "";

  const onSelect = useCallback((nodes: NodeApi<UiNode>[]) => {
    const n = nodes[0];
    onFolderChange(n ? n.data.path : null);
  }, [onFolderChange]);

  // Stable row renderer — react-arborist re-renders all rows whenever
  // the children render-prop identity changes, so we must NOT close a
  // fresh closure over per-render values here.  Rows read their own
  // selection state via `node.isSelected`; folder edits go through the
  // (stable) editable-registry context, and folder *filtering* through
  // the (stable) `onFolderChange`.
  const renderRow = useCallback(
    (props: NodeRendererProps<UiNode>) => (
      <TreeRow {...props} onFolderChange={onFolderChange} />
    ),
    [onFolderChange],
  );

  // Drag-and-drop: react-arborist's internal DnD covers folder-to-
  // folder reparenting; we wire it via `onMove`.  External drops
  // (tile → folder) are HTML5 native — handled inside the renderer.
  const onMove = useCallback(({ dragIds, parentId }: {
    dragIds:  string[];
    parentId: string | null;
    index:    number;
  }) => {
    // No multi-select reparent in the UI today.
    if (dragIds.length !== 1) return;
    const drag = dragIds[0];
    if (drag === UNSORTED_FOLDER) return;          // immutable bucket
    const parent = parentId && parentId !== UNSORTED_FOLDER ? parentId : "";
    const newPath = parent ? `${parent}/${drag.split("/").pop()}` : drag.split("/").pop()!;
    if (newPath === drag) return;
    try {
      editable.renameFolderAction(drag, newPath);
    } catch (err) {
      console.error(err);
    }
  }, [editable]);

  const onRename = useCallback(({ id, name }: { id: string; name: string }) => {
    if (id === UNSORTED_FOLDER) return;
    const parent = id.includes("/") ? id.slice(0, id.lastIndexOf("/")) : "";
    const target = parent ? `${parent}/${name}` : name;
    try {
      editable.renameFolderAction(id, target);
    } catch (err) {
      console.error(err);
    }
  }, [editable]);

  // -- toolbar actions ------------------------------------------------
  const createSiblingAtRoot = useCallback(() => {
    if (!editable.writable) return;
    let i = 1, name = "new_folder";
    while (data.some(n => n.path === name)) name = `new_folder_${i++}`;
    // Anchor it: we add a folder by giving an existing asset a folder,
    // OR by setting an empty-notes folder block — we use the latter
    // since it shows up even without assets.
    editable.setFolderNotesAction(name, "");
    // Schedule a rename right after the tree re-renders.  We delay one
    // microtask so react-arborist sees the new node first.
    requestAnimationFrame(() => {
      treeRef.current?.edit(name);
    });
  }, [editable, data]);

  return (
    <div className="flex h-full min-h-0 flex-col">
      <TreeToolbar
        onNewFolder={createSiblingAtRoot}
        writable={editable.writable}
      />
      <div ref={wrapperRef} className="min-h-0 flex-1 overflow-hidden px-1">
        {size.width > 0 && size.height > 0 && (
          <Tree<UiNode>
            ref={treeRef}
            data={data}
            openByDefault={false}
            width={size.width}
            height={size.height}
            rowHeight={24}
            indent={14}
            // Selection: single + click toggles, also driven from
            // outside via the controlled `selection` value.
            selection={selection}
            disableMultiSelection
            disableDrag={(n) => !!n.unsorted}
            disableDrop={({ parentNode, dragNodes }) =>
              !!(parentNode?.data.unsorted) ||
              dragNodes.some(d => !!d.data.unsorted)}
            onSelect={onSelect}
            onMove={onMove}
            onRename={onRename}
          >
            {renderRow}
          </Tree>
        )}
      </div>
    </div>
  );
}

// ---------------------------------------------------------------------
// row renderer
// ---------------------------------------------------------------------

function TreeRow({ node, style, dragHandle, onFolderChange }: NodeRendererProps<UiNode> & {
  onFolderChange: (folder: string | null) => void;
}) {
  const editable = useEditableRegistry();
  const [dropHover, setDropHover] = useState(false);

  // `node.isSelected` mirrors the controlled `selection` prop, so
  // react-arborist re-renders only the rows whose selection flips —
  // not the whole tree on every folder click.
  const active = node.isSelected;

  const onClick = () => {
    onFolderChange(active ? null : node.data.path);
  };

  // External HTML5 drop — a Tile is being dragged in.  We accept the
  // `ASSET_DRAG_TYPE` MIME and move that asset into this folder.
  const onDragEnter = (e: React.DragEvent) => {
    if (node.data.unsorted) return;                // dropping into "(unsorted)" handled separately
    if (e.dataTransfer.types.includes(ASSET_DRAG_TYPE)) {
      e.preventDefault();
      setDropHover(true);
    }
  };
  const onDragOver = (e: React.DragEvent) => {
    if (e.dataTransfer.types.includes(ASSET_DRAG_TYPE)) {
      e.preventDefault();                          // signal "drop allowed"
      e.dataTransfer.dropEffect = "move";
    }
  };
  const onDragLeave = () => setDropHover(false);
  const onDrop = (e: React.DragEvent) => {
    setDropHover(false);
    const id = e.dataTransfer.getData(ASSET_DRAG_TYPE);
    if (!id) return;
    e.preventDefault();
    try {
      editable.setAssetFolderAction(
        id,
        node.data.unsorted ? null : node.data.path,
      );
    } catch (err) {
      console.error(err);
    }
  };

  const isOpen   = node.isOpen;
  const hasKids  = !!node.data.children?.length;
  const editing  = node.isEditing;

  return (
    <ContextMenu>
      <ContextMenuTrigger asChild>
        <div
          ref={dragHandle}
          style={style}
          onClick={onClick}
          onDragEnter={onDragEnter}
          onDragOver={onDragOver}
          onDragLeave={onDragLeave}
          onDrop={onDrop}
          className={cn(
            "group flex h-6 select-none items-center gap-1 rounded px-1 text-xs",
            active && "bg-primary/85 text-primary-foreground",
            !active && "hover:bg-accent/10",
            dropHover && !active && "outline outline-1 outline-dashed outline-primary/70 bg-primary/10",
            node.data.unsorted && !active && "italic text-muted-foreground",
          )}
          title={node.data.path}
        >
          {hasKids ? (
            <button
              type="button"
              className="grid size-4 place-items-center text-muted-foreground hover:text-foreground"
              onClick={(e) => { e.stopPropagation(); node.toggle(); }}
            >
              {isOpen ? <ChevronDown className="size-3" /> : <ChevronRight className="size-3" />}
            </button>
          ) : (
            <span className="size-4" />
          )}
          <FolderIcon node={node.data} isOpen={isOpen} />
          {editing ? (
            <RenameInput
              defaultValue={node.data.name}
              onCommit={(name) => node.submit(name)}
              onCancel={() => node.reset()}
            />
          ) : (
            <span
              className="flex-1 truncate"
              onDoubleClick={(e) => {
                if (node.data.unsorted) return;
                e.stopPropagation();
                node.edit();
              }}
            >
              {node.data.name}
            </span>
          )}
          <span className={cn(
            "ml-auto pr-1 text-[9px] tabular-nums",
            active ? "text-primary-foreground/70" : "text-muted-foreground",
          )}>
            {node.data.count}
          </span>
        </div>
      </ContextMenuTrigger>
      <TreeRowMenu node={node} />
    </ContextMenu>
  );
}

function FolderIcon({ node, isOpen }: { node: UiNode; isOpen: boolean }) {
  if (node.unsorted) {
    return <FileWarning className="size-3.5 text-amber-400/80" />;
  }
  return isOpen
    ? <FolderOpen className="size-3.5 text-sky-300/90" />
    : <Folder     className="size-3.5 text-sky-400/70" />;
}

function RenameInput({ defaultValue, onCommit, onCancel }: {
  defaultValue: string;
  onCommit: (name: string) => void;
  onCancel: () => void;
}) {
  const [val, setVal] = useState(defaultValue);
  return (
    <input
      autoFocus
      value={val}
      onChange={(e) => setVal(e.target.value)}
      onBlur={() => onCommit(val.trim() || defaultValue)}
      onKeyDown={(e) => {
        if (e.key === "Enter")  { e.preventDefault(); onCommit(val.trim() || defaultValue); }
        if (e.key === "Escape") { e.preventDefault(); onCancel(); }
        e.stopPropagation();
      }}
      onClick={(e) => e.stopPropagation()}
      className="h-5 flex-1 min-w-0 rounded border border-primary/70 bg-background px-1 text-xs"
    />
  );
}

// ---------------------------------------------------------------------
// context menu + toolbar
// ---------------------------------------------------------------------

function TreeRowMenu({ node }: { node: NodeApi<UiNode> }) {
  const editable = useEditableRegistry();
  const path     = node.data.path;
  const unsorted = node.data.unsorted;
  const writable = editable.writable;

  const newSubfolder = () => {
    if (!writable || unsorted) return;
    let i = 1, name = `${path}/new_folder`;
    // Trivial collision avoidance.
    while ((node.children || []).some(c => c.data.path === name)) {
      name = `${path}/new_folder_${i++}`;
    }
    editable.setFolderNotesAction(name, "");
    requestAnimationFrame(() => node.tree.edit(name));
    node.open();
  };

  const rename = () => {
    if (!writable || unsorted) return;
    node.edit();
  };

  const remove = () => {
    if (!writable || unsorted) return;
    const confirmed = window.confirm(
      `Delete folder "${path}" and any descendants?\n\nAssets inside become (unsorted).`,
    );
    if (!confirmed) return;
    editable.deleteFolderAction(path, { recursive: true });
  };

  const editNotes = () => {
    if (!writable || unsorted) return;
    const next = window.prompt("Folder notes", node.data.notes ?? "");
    if (next === null) return;
    editable.setFolderNotesAction(path, next.trim() ? next : null);
  };

  return (
    <ContextMenuContent className="w-48">
      <ContextMenuLabel>{unsorted ? "(unsorted)" : path}</ContextMenuLabel>
      <ContextMenuSeparator />
      <ContextMenuItem onSelect={newSubfolder} disabled={!writable || unsorted}>
        <Plus className="size-3" /> New subfolder
      </ContextMenuItem>
      <ContextMenuItem onSelect={rename} disabled={!writable || unsorted}>
        <Pencil className="size-3" /> Rename…
      </ContextMenuItem>
      <ContextMenuItem onSelect={editNotes} disabled={!writable || unsorted}>
        <StickyNote className="size-3" /> Edit notes…
      </ContextMenuItem>
      <ContextMenuSeparator />
      <ContextMenuItem onSelect={remove} disabled={!writable || unsorted} destructive>
        <Trash2 className="size-3" /> Delete folder
      </ContextMenuItem>
    </ContextMenuContent>
  );
}

function TreeToolbar({ onNewFolder, writable }: {
  onNewFolder: () => void;
  writable:    boolean;
}) {
  return (
    <div className="flex items-center gap-1 px-3 pb-1 pt-2">
      <span className="text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground">
        Folders
      </span>
      <SaveBadge writable={writable} />
      <button
        type="button"
        onClick={onNewFolder}
        disabled={!writable}
        className={cn(
          "ml-auto grid size-5 place-items-center rounded text-muted-foreground transition-colors",
          "hover:bg-accent/10 hover:text-foreground",
          !writable && "opacity-40 hover:bg-transparent hover:text-muted-foreground",
        )}
        title={writable ? "New folder at root" : "Open the repo via the File System Access API to enable editing"}
      >
        <Plus className="size-3.5" />
      </button>
    </div>
  );
}

function SaveBadge({ writable }: {
  writable: boolean;
}) {
  // Subscribes to the *separate* save-state context so a save
  // transition re-renders this badge alone, not the whole sidebar.
  const state = useSaveState();
  if (!writable) {
    return (
      <span className="ml-2 inline-flex items-center gap-1 rounded bg-muted/50 px-1.5 py-0.5 text-[9px] text-muted-foreground" title="Pick the repo folder via the File System Access API (or run the Vite dev server) to enable editing">
        <Box className="size-2.5" /> read-only
      </span>
    );
  }
  switch (state.kind) {
    case "idle":
      return null;
    case "dirty":
      return <span className="ml-2 text-[9px] text-amber-300">edited</span>;
    case "saving":
      return <span className="ml-2 text-[9px] text-sky-300 animate-pulse">saving…</span>;
    case "saved":
      return <span className="ml-2 text-[9px] text-emerald-300">saved ✓</span>;
    case "error":
      return (
        <span className="ml-2 text-[9px] text-rose-300" title={state.message}>
          save failed
        </span>
      );
  }
}
