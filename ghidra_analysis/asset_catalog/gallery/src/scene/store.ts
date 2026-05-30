// Scene-editor workspace store.
//
// One Zustand store drives the whole declarative-scene editor, built on a
// NORMALIZED document so edits and live drags never churn the whole tree:
//
//   * The **document** is the only undoable thing. It lives in a `travels`
//     instance (Mutative JSON-patch history) as a `DocStore` — a flat
//     `nodes: Record<id, FlatNode>` map plus an ordered `rootIds` list and the
//     scalar `meta` (name/scripts/states/viewport). It is mirrored into the
//     reactive store field-for-field. EVERY document edit goes through the
//     single `applyEdit` funnel, which mutates a Travels draft and records
//     exactly one history step. Because Mutative shares structure, editing one
//     node produces a new entry for ONLY that node — so a component subscribed
//     to `nodes[id]` (see the `useNode`/`useChildIds`/… selectors) re-renders
//     alone, not the whole tree.
//
//   * The **drag** is an isolated slice (`dragId` + `dragVec`). A live drag
//     updates only that slice — never `nodes` — so only the dragged node's
//     component re-renders (it reads `useDragOffset` and paints a transient CSS
//     translate). The move is committed to the document as ONE history step on
//     pointer-up (`endDrag`), which is also the only point it streams live.
//
//   * Everything else (selection, collapse, preview state toggles, the active
//     scene/script, the save target, status text) is UI-only and never enters
//     history.
//
// The Mutative middleware (not Immer, per project convention) lets actions
// mutate drafts directly while keeping immutable structural sharing.

import { create } from "zustand";
import { mutative } from "zustand-mutative";
import { useShallow } from "zustand/react/shallow";
import { createTravels, type Travels } from "travels";

import { handleHash } from "@/data/handleHash";
import type { DataSource, SceneRef } from "@/data/DataSource";
import type { EngineBridge } from "@/data/EngineBridge";
import {
  type ControlRegistry, type DocStore, type FlatNode, type Kind, type StateInit,
  FALLBACK_REGISTRY, cloneSubtreeInto, defaultFlatNode, denormalize, docToJson,
  insertFlatNode, isDescendantId, normDoc, normalize, removeFlatNode, reparentFlatNode,
} from "./model";
import { diffDocs } from "./diff";

export type SceneView = "design" | "scripts";

/** The non-node document scalars mirrored into the store (everything in a
 *  `DocStore` except the `nodes`/`rootIds`). */
export interface DocMeta {
  name: string;
  scripts: string[];
  states: StateInit[];
  viewport: [number, number] | null;
}

interface DragVec { dx: number; dy: number }

// Shared zero-offset reference: every non-dragged node's `useDragOffset`
// returns this exact object, so its identity never changes and the selector
// bails out (no re-render) while another node is being dragged.
const NO_DRAG: DragVec = { dx: 0, dy: 0 };
const EMPTY_IDS: string[] = [];

/** Normalize a user-typed state name to the engine's identifier shape
 *  (`b.state.set("…")` keys): lowercase-ish word chars only. */
export function cleanStateName(raw: string): string {
  return raw.trim().replace(/[^A-Za-z0-9_]/g, "");
}

/** Count how many nodes reference each state name (for the CRUD UI's
 *  "used by N" hint and safe-delete affordance). */
export function countStateUsage(nodes: Iterable<FlatNode>): Record<string, number> {
  const into: Record<string, number> = {};
  for (const n of nodes) if (n.state) into[n.state] = (into[n.state] ?? 0) + 1;
  return into;
}

// ---- module-scoped history engine -------------------------------------------
//
// The editor is a single workspace, so the Travels instance is a singleton
// closure variable rather than reactive state (it holds non-serializable
// caches we never want to draft). `savedPosition` lets `dirty` compare the
// current history position against the last save/load, so undoing back to a
// saved state correctly clears the dirty flag.

let travels: Travels<DocStore> | null = null;
let savedPosition = 0;

// ---- live HMR streaming (Vite-style) ----------------------------------------
//
// When `liveMode` is on, every committed doc change is diffed against
// `lastSynced` (the last DocStore revision the running game knows) and the
// minimal patch ops are streamed to the engine via the bridge — preserving the
// Luau VM so you can play and edit at once. `lastSynced` is a non-reactive
// closure ref (a whole-doc snapshot we never want to draft into reactive state).
let lastSynced: DocStore | null = null;
let liveTimer: ReturnType<typeof setTimeout> | null = null;
let liveRev = 0;
// After edits settle, the broker's stored snapshot is stale by every patch
// streamed since the last full sync. This timer refreshes it (one CACHE-ONLY
// `liveSyncFull`) once editing pauses, so a game that (re)connects later
// replays a CURRENT doc — closing the reopen-divergence gap for late joiners
// WITHOUT ever rebuilding the game you're currently editing against.
let settleTimer: ReturnType<typeof setTimeout> | null = null;
// Last broker client count we reacted to, so `configure` can tell when a new
// game attaches (count rose) and re-push the full doc for it.
let lastClients = 0;

/** Writable paks the active doc can be saved/overridden into: any folder mod
 *  pak in the load order, plus the scene's own pak. Saving to a pak other
 *  than the scene's own writes a layered override at the same path. */
export function computeWritablePaks(
  scenes: SceneRef[], bridge: EngineBridge | null, active: SceneRef | null,
): string[] {
  const ids = new Set<string>();
  for (const s of scenes) if (s.writable) ids.add(s.pak);
  if (bridge) for (const p of bridge.paks) if (p.kind === "mod") ids.add(p.id);
  if (active) ids.add(active.pak);
  return [...ids];
}

export interface SceneStore {
  // ---- injected services (wired from React via `configure`) ----------------
  source: DataSource | null;
  bridge: EngineBridge | null;

  // ---- undoable mirror (source of truth is the Travels singleton) ----------
  // The normalized document, mirrored field-for-field so per-node selectors can
  // subscribe to a single `nodes[id]` entry.
  nodes: Record<string, FlatNode>;
  rootIds: string[];
  meta: DocMeta | null;
  canUndo: boolean;
  canRedo: boolean;
  dirty: boolean;

  // ---- ui-only (never recorded in history) ---------------------------------
  registry: ControlRegistry;
  scenes: SceneRef[];
  active: SceneRef | null;
  selectedId: string | null;
  collapsed: Set<string>;
  /** Editor-preview toggles for declared states (UI-only, never persisted). */
  states: Record<string, boolean>;
  view: SceneView;
  openScript: { pak: string; rel: string } | null;
  pakScripts: Record<string, string[]>;
  saveTarget: string;
  busy: boolean;
  msg: string | null;
  err: string | null;
  liveMode: boolean;
  /** True while a full-doc re-sync is in flight (Live turned on, scene
   *  changed, a game attached, or a patch couldn't be hot-applied so the
   *  game must rebuild). Surfaced as a "rebuilding" status, Vite-style. */
  rebuilding: boolean;

  // ---- isolated drag slice (no document churn until endDrag) ---------------
  /** The id of the node currently being dragged, or null. */
  dragId: string | null;
  /** The live, uncommitted drag delta (design px) of `dragId`. */
  dragVec: DragVec;

  // ---- service wiring + loading --------------------------------------------
  configure(svc: { source: DataSource; bridge: EngineBridge }): void;
  loadRegistry(): Promise<void>;
  reloadScenes(): Promise<SceneRef[]>;
  loadScene(ref: SceneRef): Promise<void>;
  loadPakScripts(): Promise<void>;

  // ---- selection / ui (non-undoable) ---------------------------------------
  select(id: string | null): void;
  setView(v: SceneView): void;
  openScriptFile(pak: string, rel: string): void;
  setSaveTarget(pak: string): void;
  toggleCollapse(id: string): void;
  toggleState(name: string): void;
  setStatePreview(name: string, on: boolean): void;
  setLiveMode(on: boolean): void;

  // ---- state CRUD (undoable document edits) --------------------------------
  addState(name: string): string;
  renameState(oldName: string, newName: string): void;
  deleteState(name: string): void;
  setStateDefault(name: string, on: boolean): void;

  // ---- document mutations (ALL routed through applyEdit → one history step) -
  updateSelected(fn: (n: FlatNode) => FlatNode): void;
  addNode(kind: Kind): void;
  deleteSelected(): void;
  duplicateSelected(): void;
  reparent(dragId: string, dropId: string): void;
  beginDrag(id: string): void;
  dragBy(dx: number, dy: number): void;
  endDrag(): void;

  // ---- history -------------------------------------------------------------
  undo(): void;
  redo(): void;

  // ---- persistence ---------------------------------------------------------
  save(push: boolean): Promise<void>;
  createScene(): Promise<void>;
}

export const useSceneStore = create<SceneStore>()(
  mutative((set, get) => {
    // Mirror the Travels doc + history flags into reactive state, and reconcile
    // the (non-undoable) selection + preview toggles against the new tree.
    // `extra` lets callers fold in unrelated ui-only updates in the same commit
    // (e.g. clearing the drag slice atomically with the committed move).
    const sync = (extra?: Partial<SceneStore>): void => {
      set((s) => {
        if (travels) {
          const d = travels.getState();
          s.nodes = d.nodes;
          s.rootIds = d.rootIds;
          s.meta = { name: d.name, scripts: d.scripts, states: d.states, viewport: d.viewport };
          s.canUndo = travels.canBack();
          s.canRedo = travels.canForward();
          s.dirty = travels.getPosition() !== savedPosition;
          if (s.selectedId != null && !d.nodes[s.selectedId]) s.selectedId = null;
          // Keep the editor-preview toggle map in lock-step with the declared
          // states: new ones seed from their default `on`, deleted ones drop
          // out — survivors keep their value (correct across undo/redo for free).
          const declared = new Set(d.states.map((x) => x.name));
          for (const st of d.states) if (!(st.name in s.states)) s.states[st.name] = st.on;
          for (const k of Object.keys(s.states)) if (!declared.has(k)) delete s.states[k];
        } else {
          s.nodes = {};
          s.rootIds = [];
          s.meta = null;
          s.canUndo = false;
          s.canRedo = false;
          s.dirty = false;
        }
        if (extra) Object.assign(s, extra);
      });
      // Any committed doc change streams to the live game (throttled) when Live
      // is on. Cheap no-op otherwise.
      scheduleLivePush();
    };

    // The ONE document-edit funnel. `mutate` receives the Travels draft and
    // mutates `nodes`/`rootIds`/`states`/… in place via the normalized helpers.
    // Records exactly one undoable step labelled for the (future) history panel.
    const applyEdit = (mutate: (d: DocStore) => void, label: string, extra?: Partial<SceneStore>): void => {
      if (!travels) return;
      travels.setState((d) => { mutate(d as DocStore); }, { label });
      sync(extra);
    };

    // Edit a single node through a pure `fn`. The committed (plain, frozen) node
    // is read first and `fn` applied off-draft, so the result assigned into the
    // draft is a clean plain object (no leaked Mutative proxies).
    const editNode = (id: string, fn: (n: FlatNode) => FlatNode, label: string): void => {
      if (!travels) return;
      const cur = travels.getState().nodes[id];
      if (!cur) return;
      const next = fn(cur);
      applyEdit((d) => { d.nodes[id] = next; }, label);
    };

    // Push the *whole* current doc to the engine as a transient overlay (no
    // disk write), so the engine rebuilds from the doc (which carries the
    // editor's stable ids) and subsequent granular patches can address every
    // node.
    //
    // `cacheOnly` (the idle settle) refreshes the broker's stored snapshot for
    // FUTURE joiners without broadcasting — attached games keep their live
    // state, so editing never triggers a rebuild. The default (broadcast) is
    // for the events that legitimately rebuild the running game: Live turned
    // on, the edited scene changed, a game attached, or a patch that couldn't
    // be hot-applied.
    const liveSyncFull = async (cacheOnly = false): Promise<void> => {
      const ds = travels?.getState();
      const { active, bridge } = get();
      if (!ds || !active?.doc || !bridge || bridge.status.kind !== "online") return;
      // A pending settle-sync is now redundant — we're doing the full push.
      if (settleTimer) { clearTimeout(settleTimer); settleTimer = null; }
      if (!cacheOnly) set({ rebuilding: true });
      try {
        const json = docToJson(denormalize(ds));
        const rel = active.doc;
        const slash = rel.lastIndexOf("/");
        const folder = slash >= 0 ? rel.slice(0, slash) : "";
        const slug = slash >= 0 ? rel.slice(slash + 1) : rel;
        const hash = handleHash(folder, slug);
        await bridge.push(hash, new TextEncoder().encode(json), { ext: ".json", slug, folder, transient: true, cacheOnly });
        lastSynced = ds;
      } catch (e) {
        set({ err: `live sync failed: ${String((e as Error).message ?? e)}` });
      } finally {
        if (!cacheOnly) set({ rebuilding: false });
      }
    };

    // After edits settle (no change for ~500ms), refresh the broker's stored
    // snapshot — CACHE-ONLY, so a late-joining game replays a current doc while
    // the game you're editing against keeps its live-patched state (no rebuild).
    const runLiveSettle = async (): Promise<void> => {
      const { bridge, liveMode } = get();
      if (!liveMode || !bridge || bridge.status.kind !== "online") return;
      await liveSyncFull(true);
    };

    // Diff the current doc against the last revision the game knows and stream
    // the minimal ops; fall back to a transient full re-sync when a change
    // can't be hot-patched (scripts list). Reads the committed Travels doc,
    // which on pointer-up already holds the committed move.
    const runLivePush = async (): Promise<void> => {
      const ds = travels?.getState();
      const { bridge, liveMode } = get();
      if (!liveMode || !ds || !bridge) return;
      // No game attached → drop the baseline so the next edit after a game
      // (re)connects re-establishes a full, patchable doc.
      if (bridge.status.kind !== "online") { lastSynced = null; return; }
      if (!lastSynced) { await liveSyncFull(); return; }
      const { ops, needsReload } = diffDocs(lastSynced, ds);
      if (needsReload) { await liveSyncFull(); return; }
      if (!ops.length) { lastSynced = ds; return; }
      try {
        await bridge.pushScenePatch(ds.name, ops, ++liveRev);
        lastSynced = ds;
      } catch (e) {
        set({ err: `live patch failed: ${String((e as Error).message ?? e)}` });
      }
    };

    // Rate-limit live pushes to at most one per ~60ms (a *throttle*, not a
    // resetting debounce): the first committed edit schedules a push 60ms out;
    // further edits inside that window are folded in (runLivePush always diffs
    // the latest doc), so a continuous property scrub streams ≤1×/60ms. Each
    // push re-arms on the next edit, so the final state is always delivered.
    const scheduleLivePush = (): void => {
      if (!get().liveMode) return;
      // Throttled incremental patch: ≤1 push per ~60ms window.
      if (!liveTimer) liveTimer = setTimeout(() => { liveTimer = null; void runLivePush(); }, 60);
      // Settle-sync: a resetting debounce that fires ~500ms after the LAST edit
      // to refresh the broker's replay snapshot for late joiners.
      if (settleTimer) clearTimeout(settleTimer);
      settleTimer = setTimeout(() => { settleTimer = null; void runLiveSettle(); }, 500);
    };

    // Discrete state preview toggle → an immediate live `State` op (no debounce;
    // toggles are deliberate, single events). UI-only otherwise.
    const pushStateOp = (name: string, on: boolean): void => {
      const { liveMode, bridge } = get();
      const ds = travels?.getState();
      if (!liveMode || !bridge || !ds) return;
      void bridge.pushScenePatch(ds.name, [{ op: "state" as const, name, on }], ++liveRev).catch(() => {});
    };

    return {
      source: null,
      bridge: null,

      nodes: {},
      rootIds: [],
      meta: null,
      canUndo: false,
      canRedo: false,
      dirty: false,

      registry: FALLBACK_REGISTRY,
      scenes: [],
      active: null,
      selectedId: null,
      collapsed: new Set<string>(),
      states: {},
      view: "design",
      openScript: null,
      pakScripts: {},
      saveTarget: "",
      busy: false,
      msg: null,
      err: null,
      // Live streaming is on by default: with the bridge auto-detecting an
      // attached game, edits flow the moment one connects (no extra opt-in).
      // It's a no-op until a game attaches; the toolbar toggle pauses it.
      liveMode: true,
      rebuilding: false,

      dragId: null,
      dragVec: NO_DRAG,

      // ---- service wiring + loading -----------------------------------------

      configure: (svc) => {
        const prevClients = lastClients;
        lastClients = svc.bridge.clients;
        set({ source: svc.source, bridge: svc.bridge });
        // A game just (re)connected to the broker (client count rose). Re-push
        // the whole current doc so the broker's replay snapshot — and the
        // freshly-attached game — reflect the editor's live state, including
        // unsaved edits, rather than the stale disk paks it booted from.
        if (svc.bridge.clients > prevClients && get().liveMode) {
          lastSynced = null;        // force the full-resync path on next diff too
          void liveSyncFull();
        }
      },

      loadRegistry: async () => {
        try {
          const r = await fetch("./data/catalog/control-registry.json", { cache: "no-store" });
          if (r.ok) {
            const reg = (await r.json()) as Partial<ControlRegistry>;
            if (reg.kinds && reg.common && reg.bindable) set({ registry: reg as ControlRegistry });
          }
        } catch { /* fallback registry already set */ }
      },

      reloadScenes: async () => {
        const { source } = get();
        try {
          const list = (await source?.listScenes?.()) ?? [];
          set({ scenes: list });
          return list;
        } catch {
          set({ scenes: [] });
          return [];
        }
      },

      loadScene: async (ref) => {
        const { source } = get();
        set({ busy: true, err: null, msg: null });
        try {
          if (ref.kind !== "declarative" || !ref.doc) {
            throw new Error(`'${ref.name}' is a legacy module scene (no declarative doc to edit)`);
          }
          const text = await source?.readPakText?.(ref.pak, ref.doc);
          if (text == null) throw new Error("this data source can't read pak files");
          const parsed = normalize(normDoc(JSON.parse(text), ref.name));
          // Fresh history per scene — undo never crosses a scene load.
          travels = createTravels(parsed, { maxHistory: 200 });
          savedPosition = travels.getPosition();
          // New scene = new baseline: the next live push does a full re-sync
          // (the running game must rebuild from this doc's ids before deltas).
          lastSynced = null;
          const st: Record<string, boolean> = {};
          for (const s of parsed.states) st[s.name] = s.on;
          sync({
            active: ref,
            selectedId: null,
            collapsed: new Set<string>(),
            states: st,
            view: "design",
            openScript: parsed.scripts[0] ? { pak: ref.pak, rel: parsed.scripts[0] } : null,
            saveTarget: ref.pak,
            busy: false,
            dragId: null,
            dragVec: NO_DRAG,
          });
        } catch (e) {
          set({ err: String((e as Error).message ?? e), busy: false });
        }
      },

      loadPakScripts: async () => {
        const { source, scenes } = get();
        if (!source?.listPakScripts) return;
        const paks = [...new Set(scenes.map((s) => s.pak))];
        try {
          const lists = await Promise.all(
            paks.map(async (pak) => [pak, await source.listPakScripts!(pak)] as const),
          );
          set((s) => { for (const [pak, scripts] of lists) s.pakScripts[pak] = scripts; });
        } catch { /* best-effort browser fuel */ }
      },

      // ---- selection / ui ---------------------------------------------------

      select: (id) => set({ selectedId: id }),
      setView: (v) => set({ view: v }),
      openScriptFile: (pak, rel) => set({ openScript: { pak, rel } }),
      setSaveTarget: (pak) => set({ saveTarget: pak }),

      toggleCollapse: (id) => set((s) => {
        if (s.collapsed.has(id)) s.collapsed.delete(id);
        else s.collapsed.add(id);
      }),

      toggleState: (name) => {
        const on = !get().states[name];
        set((s) => { s.states[name] = on; });
        pushStateOp(name, on);
      },
      setStatePreview: (name, on) => { set((s) => { s.states[name] = on; }); pushStateOp(name, on); },

      setLiveMode: (on) => {
        set({ liveMode: on });
        if (on) void liveSyncFull();
      },

      // ---- state CRUD -------------------------------------------------------

      addState: (name) => {
        const clean = cleanStateName(name);
        if (!clean) return "";
        if (travels?.getState().states.some((s) => s.name === clean)) {
          set({ err: `state '${clean}' already exists` });
          return "";
        }
        applyEdit((d) => { d.states.push({ name: clean, on: false }); }, `Add state ${clean}`);
        return clean;
      },

      renameState: (oldName, newName) => {
        const clean = cleanStateName(newName);
        if (!clean || clean === oldName) return;
        if (travels?.getState().states.some((s) => s.name === clean)) {
          set({ err: `state '${clean}' already exists` });
          return;
        }
        applyEdit((d) => {
          for (const s of d.states) if (s.name === oldName) s.name = clean;
          for (const id in d.nodes) if (d.nodes[id].state === oldName) d.nodes[id].state = clean;
        }, `Rename state ${oldName}→${clean}`);
        // Carry the editor-preview value across the rename (sync only seeds
        // brand-new keys; preserve the live toggle here).
        set((s) => {
          if (oldName in s.states) { s.states[clean] = s.states[oldName]; delete s.states[oldName]; }
        });
      },

      deleteState: (name) => {
        applyEdit((d) => {
          d.states = d.states.filter((s) => s.name !== name);
          for (const id in d.nodes) if (d.nodes[id].state === name) d.nodes[id].state = "";
        }, `Delete state ${name}`);
      },

      setStateDefault: (name, on) => {
        applyEdit((d) => { for (const s of d.states) if (s.name === name) s.on = on; }, `Set state ${name} default ${on ? "on" : "off"}`);
      },

      // ---- document mutations ----------------------------------------------

      updateSelected: (fn) => {
        const id = get().selectedId;
        if (id == null) return;
        editNode(id, fn, "Edit node");
      },

      addNode: (kind) => {
        const n = defaultFlatNode(kind);
        if (kind === "quad") { n.layout.width = 120; n.layout.height = 30; n.color = "334155ff"; }
        if (kind === "button") { n.layout.width = 160; n.layout.height = 30; n.label = "Button"; n.color = "202c3aff"; }
        if (kind === "input_region") { n.layout.width = 120; n.layout.height = 30; }
        if (kind === "text") { n.text = "Text"; }
        const { selectedId } = get();
        const sel = selectedId ? travels?.getState().nodes[selectedId] : null;
        const parentId = sel && sel.kind === "group" ? sel.id : null;
        applyEdit((d) => insertFlatNode(d, parentId, n), `Add ${kind}`);
        set({ selectedId: n.id });
      },

      deleteSelected: () => {
        const id = get().selectedId;
        if (id == null) return;
        applyEdit((d) => removeFlatNode(d, id), "Delete node");
        set({ selectedId: null });
      },

      duplicateSelected: () => {
        const { selectedId } = get();
        const src = selectedId ? travels?.getState().nodes[selectedId] : null;
        if (!src) return;
        let newId = "";
        applyEdit((d) => {
          newId = cloneSubtreeInto(d, src.id, src.parentId);
          const n = d.nodes[newId];
          n.layout = { ...n.layout, x: n.layout.x + 12, y: n.layout.y + 12 };
        }, "Duplicate node");
        if (newId) set({ selectedId: newId });
      },

      reparent: (dragId, dropId) => {
        if (dragId === dropId) return;
        const ds = travels?.getState();
        if (!ds) return;
        const moving = ds.nodes[dragId];
        const target = ds.nodes[dropId];
        if (!moving || !target || target.kind !== "group") return;
        if (isDescendantId(ds, dropId, dragId)) return; // refuse cycles (drop into own subtree)
        applyEdit((d) => reparentFlatNode(d, dragId, dropId), "Reparent node");
      },

      // ---- isolated drag slice ----------------------------------------------

      beginDrag: (id) => {
        if (!travels?.getState().nodes[id]) return;
        set({ dragId: id, dragVec: { dx: 0, dy: 0 } });
      },

      dragBy: (dx, dy) => {
        // Touch ONLY the drag slice — no `nodes`, no history, no live push.
        // Only the dragged node subscribes to this (via `useDragOffset`), so it
        // re-renders alone and paints a transient translate.
        set((s) => {
          if (s.dragId == null) return;
          s.dragVec = { dx: s.dragVec.dx + dx, dy: s.dragVec.dy + dy };
        });
      },

      endDrag: () => {
        const { dragId, dragVec } = get();
        if (dragId == null) return;
        const { dx, dy } = dragVec;
        if (dx === 0 && dy === 0) { set({ dragId: null, dragVec: NO_DRAG }); return; }
        // Commit the move AND clear the drag slice in the SAME mirror commit, so
        // the node never renders a frame with both the new x/y and a stale
        // offset (which would double the displacement / flicker).
        const cur = travels?.getState().nodes[dragId];
        if (!cur) { set({ dragId: null, dragVec: NO_DRAG }); return; }
        const moved: FlatNode = { ...cur, layout: { ...cur.layout, x: cur.layout.x + dx, y: cur.layout.y + dy } };
        applyEdit((d) => { d.nodes[dragId] = moved; }, "Move node", { dragId: null, dragVec: NO_DRAG });
      },

      // ---- history ----------------------------------------------------------

      undo: () => { if (!travels) return; travels.back(); sync(); },
      redo: () => { if (!travels) return; travels.forward(); sync(); },

      // ---- persistence ------------------------------------------------------

      save: async (push) => {
        const ds = travels?.getState();
        const { active, source, bridge, saveTarget } = get();
        if (!ds || !active) return;
        set({ busy: true, err: null, msg: null });
        try {
          const json = docToJson(denormalize(ds));
          const rel = active.doc!;
          const target = saveTarget || active.pak;
          if (!source?.writePakText) {
            throw new Error("this data source is read-only — pick a folder with write access or run the dev broker");
          }
          await source.writePakText(target, rel, json);
          const override = target !== active.pak ? ` (override in ${target})` : "";
          let extra = "";
          if (push && bridge && bridge.status.kind === "online") {
            const slash = rel.lastIndexOf("/");
            const folder = slash >= 0 ? rel.slice(0, slash) : "";
            const slug = slash >= 0 ? rel.slice(slash + 1) : rel;
            const hash = handleHash(folder, slug);
            await bridge.push(hash, new TextEncoder().encode(json), { ext: ".json", slug, folder, transient: true });
            extra = ` + live ${hash}`;
          }
          if (travels) savedPosition = travels.getPosition();
          sync({ msg: `saved ${target}/${rel} (${json.length} bytes)${override}${extra}`, busy: false });
        } catch (e) {
          set({ err: String((e as Error).message ?? e), busy: false });
        }
      },

      createScene: async () => {
        const { source, saveTarget, scenes, bridge, active } = get();
        if (!source?.writePakText || !source?.readPakText) {
          set({ err: "read-only source — can't create scenes" });
          return;
        }
        const rawName = window.prompt("New scene name (e.g. options_menu):")?.trim();
        if (!rawName) return;
        const name = rawName.replace(/[^a-z0-9_/-]/gi, "");
        if (!name) { set({ err: "invalid scene name" }); return; }
        const target = saveTarget || computeWritablePaks(scenes, bridge, active)[0];
        if (!target) { set({ err: "no writable pak to create the scene in" }); return; }
        set({ busy: true, err: null, msg: null });
        try {
          if (scenes.some((s) => s.pak === target && s.name === name)) {
            throw new Error(`'${name}' already exists in ${target}`);
          }
          const rel = `scenes/${name}.scene.json`;
          await source.writePakText(target, rel, docToJson({ name, scripts: [], states: [], viewport: null, root: [] }));
          let manifest: Record<string, unknown> = {};
          try { manifest = JSON.parse(await source.readPakText(target, "pak.json")); } catch { manifest = {}; }
          manifest.schema ??= 1;
          manifest.id ??= target;
          manifest.kind ??= "mod";
          const scenesTable = (manifest.scenes as Record<string, unknown>) ?? {};
          scenesTable[name] = { doc: rel, scripts: [] };
          manifest.scenes = scenesTable;
          await source.writePakText(target, "pak.json", JSON.stringify(manifest, null, 2) + "\n");
          const list = await get().reloadScenes();
          const ref = list.find((s) => s.pak === target && s.name === name);
          set({ busy: false, msg: `created ${target}/${rel}` });
          if (ref) await get().loadScene(ref);
        } catch (e) {
          set({ err: String((e as Error).message ?? e), busy: false });
        }
      },
    };
  }),
);

// ---- per-node selector hooks ------------------------------------------------
//
// These are the heart of the perf design: each subscribes to the narrowest
// possible slice, so a node edit (or a live drag) re-renders only the
// component(s) that actually changed.

/** The node entry for `id` (undefined when missing). Re-renders only when that
 *  one node's entry changes (Mutative structural sharing). */
export const useNode = (id: string | null | undefined): FlatNode | undefined =>
  useSceneStore((s) => (id ? s.nodes[id] : undefined));

/** A node's ordered child ids — re-renders only when the child list changes
 *  (add/remove/reorder), not on a child's prop edit. */
export const useChildIds = (id: string): string[] =>
  useSceneStore(useShallow((s) => s.nodes[id]?.childIds ?? EMPTY_IDS));

/** The top-level node ids — re-renders only when the root list changes. */
export const useRootIds = (): string[] =>
  useSceneStore(useShallow((s) => s.rootIds));

export const useIsSelected = (id: string): boolean =>
  useSceneStore((s) => s.selectedId === id);

export const useCollapsed = (id: string): boolean =>
  useSceneStore((s) => s.collapsed.has(id));

/** Whether a node is visible under the current editor-preview state toggles. */
export const useNodeVisible = (id: string): boolean =>
  useSceneStore((s) => { const n = s.nodes[id]; return !n || !n.state || !!s.states[n.state]; });

/** The live drag offset for `id` — the moving node gets the live delta; every
 *  other node gets the shared `NO_DRAG` (stable identity ⇒ no re-render). */
export const useDragOffset = (id: string): DragVec =>
  useSceneStore((s) => (s.dragId === id ? s.dragVec : NO_DRAG));
