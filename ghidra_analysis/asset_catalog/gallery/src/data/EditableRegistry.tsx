// Editable registry state.
//
// Owns the live `RawRegistry` object, all mutation entry points, and
// the debounced write-back to the underlying `DataSource`.  Everything
// that edits folders / slugs / notes calls one of the exposed
// `mutate*` actions; saving is automatic on a short debounce.
//
// Why a custom store and not zustand/jotai/etc.: the editable surface
// is tiny (one object, ~10 actions) and lives only inside one tab of
// the gallery.  React's built-in `useReducer` keeps the dependency
// graph clean and the type inference loud.

import {
  createContext, useCallback, useContext, useEffect, useMemo, useReducer, useRef,
} from "react";
import type { RawRegistry } from "./raw";
import type { DataSource } from "./DataSource";
import {
  deleteFolder, renameFolder, serializeRegistry,
  setAssetFolder, setFolderNotes, upsertAsset,
} from "./registryEdits";

/** Lifecycle of an in-flight save. */
export type SaveState =
  | { kind: "idle";    lastSavedAt: number | null }
  | { kind: "dirty"                              } // pending debounce
  | { kind: "saving"                             }
  | { kind: "saved";   at: number                }
  | { kind: "error";   message: string            };

export interface EditableRegistry {
  /** Current draft.  Always reflects the latest mutation. */
  registry: RawRegistry;
  /** Whether the underlying source allows writes — when false every
   *  `mutate*` action throws.  Surfaced to the UI so the editor
   *  toolbar can hide its action buttons. */
  writable: boolean;

  // ---- actions ------------------------------------------------------
  // NB: every action has a *stable* identity (it reads the live
  // registry through a ref), so this object's reference only changes
  // when `registry` or `writable` change — never on a mere save-state
  // transition.  `save` is intentionally NOT a member; it lives in its
  // own context (`useSaveState`) so the badge can update in isolation.
  setAssetFolderAction(idHex: string, folder: string | null): void;
  upsertAssetAction(idHex: string, patch: Parameters<typeof upsertAsset>[2]): void;
  renameFolderAction(oldPath: string, newPath: string): void;
  deleteFolderAction(path: string, opts?: { recursive?: boolean }): void;
  setFolderNotesAction(path: string, notes: string | null): void;
}

/** What `useEditableRegistryStore` hands back: the stable editable
 *  surface plus the volatile save state, kept apart on purpose. */
export interface EditableRegistryStore {
  editable: EditableRegistry;
  save:     SaveState;
}

// ---------------------------------------------------------------------
// reducer
// ---------------------------------------------------------------------

type Action =
  | { kind: "replace";        next: RawRegistry }
  | { kind: "save-start"                        }
  | { kind: "save-success";   at: number        }
  | { kind: "save-error";     message: string   }
  | { kind: "save-idle";      at: number | null };

interface State {
  registry: RawRegistry;
  save:     SaveState;
}

function reducer(prev: State, a: Action): State {
  switch (a.kind) {
    case "replace":      return { registry: a.next, save: { kind: "dirty" } };
    case "save-start":   return { ...prev, save: { kind: "saving" } };
    case "save-success": return { ...prev, save: { kind: "saved", at: a.at } };
    case "save-error":   return { ...prev, save: { kind: "error", message: a.message } };
    case "save-idle":    return { ...prev, save: { kind: "idle",  lastSavedAt: a.at } };
  }
}

const DEBOUNCE_MS  = 300;
const SAVED_HOLD_MS = 1500;   // how long "saved ✓" stays before fading

// ---------------------------------------------------------------------
// hook
// ---------------------------------------------------------------------

export function useEditableRegistryStore(
  initial: RawRegistry,
  source:  DataSource,
): EditableRegistryStore {
  const [state, dispatch] = useReducer(reducer, undefined, () => ({
    registry: initial,
    save:     { kind: "idle", lastSavedAt: null } as SaveState,
  }));

  // Live registry ref so the action callbacks below can stay stable
  // (empty-ish deps) instead of being re-created on every mutation.
  const registryRef = useRef(state.registry);
  registryRef.current = state.registry;

  // Reset the draft if the source changes (parent typically remounts
  // the editor in this case, but be defensive).
  const initialRef = useRef(initial);
  useEffect(() => {
    if (initialRef.current !== initial) {
      initialRef.current = initial;
      dispatch({ kind: "replace", next: initial });
    }
  }, [initial]);

  const writable = !!source.info.writable && typeof source.writeRegistry === "function";

  // ---- save debounce -----------------------------------------------
  const saveTimerRef  = useRef<number | null>(null);
  const savedHoldRef  = useRef<number | null>(null);
  const inFlightRef   = useRef<Promise<void> | null>(null);
  const latestTextRef = useRef<string>(serializeRegistry(initial));

  const runSave = useCallback(async () => {
    if (!source.writeRegistry) return;
    if (inFlightRef.current) await inFlightRef.current;
    dispatch({ kind: "save-start" });
    const promise = (async () => {
      try {
        await source.writeRegistry!(latestTextRef.current);
        dispatch({ kind: "save-success", at: Date.now() });
        // Fade "saved" → "idle" after the hold.
        if (savedHoldRef.current) window.clearTimeout(savedHoldRef.current);
        savedHoldRef.current = window.setTimeout(() => {
          dispatch({ kind: "save-idle", at: Date.now() });
        }, SAVED_HOLD_MS);
      } catch (err) {
        dispatch({ kind: "save-error", message: String(err) });
      } finally {
        inFlightRef.current = null;
      }
    })();
    inFlightRef.current = promise;
    return promise;
  }, [source]);

  // Re-arm the debounce every time the draft changes.
  useEffect(() => {
    latestTextRef.current = serializeRegistry(state.registry);
    if (state.save.kind !== "dirty") return;       // only schedule once
    if (saveTimerRef.current) window.clearTimeout(saveTimerRef.current);
    saveTimerRef.current = window.setTimeout(runSave, DEBOUNCE_MS);
    return () => {
      if (saveTimerRef.current) window.clearTimeout(saveTimerRef.current);
    };
  }, [state.registry, state.save.kind, runSave]);

  // ---- public action wrappers --------------------------------------
  const guardWritable = () => {
    if (!writable) throw new Error("data source is read-only");
  };

  const apply = useCallback((next: RawRegistry) => {
    dispatch({ kind: "replace", next });
  }, []);

  const flush = useCallback(async () => {
    if (saveTimerRef.current) {
      window.clearTimeout(saveTimerRef.current);
      saveTimerRef.current = null;
    }
    await runSave();
  }, [runSave]);

  const setAssetFolderAction = useCallback((idHex: string, folder: string | null) => {
    guardWritable();
    apply(setAssetFolder(registryRef.current, idHex, folder));
  }, [apply, writable]);

  const upsertAssetAction = useCallback((idHex: string, patch: Parameters<typeof upsertAsset>[2]) => {
    guardWritable();
    apply(upsertAsset(registryRef.current, idHex, patch));
  }, [apply, writable]);

  const renameFolderAction = useCallback((oldPath: string, newPath: string) => {
    guardWritable();
    apply(renameFolder(registryRef.current, oldPath, newPath));
  }, [apply, writable]);

  const deleteFolderAction = useCallback((path: string, opts?: { recursive?: boolean }) => {
    guardWritable();
    apply(deleteFolder(registryRef.current, path, opts));
  }, [apply, writable]);

  const setFolderNotesAction = useCallback((path: string, notes: string | null) => {
    guardWritable();
    apply(setFolderNotes(registryRef.current, path, notes));
  }, [apply, writable]);

  // Save before the page unloads so a quick edit + close doesn't lose
  // the draft.  beforeunload is fire-and-forget (we can't await
  // anything reliably), but most browsers do honour synchronous
  // fetch+keepalive — `writeRegistry` should already be quick.
  useEffect(() => {
    const handler = () => {
      if (state.save.kind === "dirty" || state.save.kind === "saving") {
        flush();
      }
    };
    window.addEventListener("beforeunload", handler);
    return () => window.removeEventListener("beforeunload", handler);
  }, [state.save.kind, flush]);

  const editable = useMemo<EditableRegistry>(() => ({
    registry: state.registry,
    writable,
    setAssetFolderAction,
    upsertAssetAction,
    renameFolderAction,
    deleteFolderAction,
    setFolderNotesAction,
  }), [
    state.registry, writable,
    setAssetFolderAction, upsertAssetAction, renameFolderAction,
    deleteFolderAction, setFolderNotesAction,
  ]);

  return { editable, save: state.save };
}

// ---------------------------------------------------------------------
// context glue
// ---------------------------------------------------------------------

const Ctx = createContext<EditableRegistry | null>(null);
Ctx.displayName = "EditableRegistry";

export function EditableRegistryProvider({ value, children }: {
  value: EditableRegistry;
  children: React.ReactNode;
}) {
  return <Ctx.Provider value={value}>{children}</Ctx.Provider>;
}

export function useEditableRegistry(): EditableRegistry {
  const ctx = useContext(Ctx);
  if (!ctx) throw new Error("useEditableRegistry called outside provider");
  return ctx;
}

// ---------------------------------------------------------------------
// save-state context (kept apart from the editable surface so that a
// save transition only re-renders the badge, never the editor body)
// ---------------------------------------------------------------------

const SaveCtx = createContext<SaveState>({ kind: "idle", lastSavedAt: null });
SaveCtx.displayName = "SaveState";

export function SaveStateProvider({ value, children }: {
  value: SaveState;
  children: React.ReactNode;
}) {
  return <SaveCtx.Provider value={value}>{children}</SaveCtx.Provider>;
}

export function useSaveState(): SaveState {
  return useContext(SaveCtx);
}
