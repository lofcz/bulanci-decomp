// Scene-document differ — the editor's half of HMR.
//
// `diffDocs(prev, next)` computes the minimal list of patch ops that turns one
// scene-document revision into the next, keyed by each node's stable `id` (see
// model.ts / control.rs). The ops mirror `core/src/scene/patch.rs`'s
// `PatchOp` serde shape exactly, so the engine applies them in place against
// the live scene — preserving the Luau VM, runtime states and every surviving
// NodeId. This is the Vite "what can hot-patch" calculation.
//
// It consumes the *normalized* `DocStore` directly: that store is already the
// id->node map this differ needs, so there's no re-flatten, and unchanged nodes
// are skipped by a reference-equality check (Mutative structural sharing means
// an untouched node keeps its entry identity across revisions), making the prop
// pass O(changed nodes) rather than O(all nodes).
//
// The HMR boundary (mirrors the plan): everything in the declarative doc is a
// live patch. Only a change to the attached `scripts` list (or the scene
// identity) sets `needsReload` — closures can't be hot-swapped, so the caller
// falls back to the existing save+overlay full rebuild. Editing a `.luau`
// file's *code* is a separate channel (the Scripts panel's overlay push) and
// never reaches this differ.

import type { CNode, DocStore, FlatNode } from "./model";
import { flatNodeToJson, nodeToJson } from "./model";

export interface PatchOpProp { op: "prop"; id: string; node: Record<string, unknown> }
export interface PatchOpAdd { op: "add"; parent: string; index: number; node: Record<string, unknown> }
export interface PatchOpRemove { op: "remove"; id: string }
export interface PatchOpReparent { op: "reparent"; id: string; parent: string; index: number }
export interface PatchOpState { op: "state"; name: string; on: boolean }
export interface PatchOpStates { op: "states"; list: { name: string; on: boolean }[] }
export interface PatchOpBindings { op: "bindings"; id: string; bindings: { prop: string; key: string }[] }
export interface PatchOpViewport { op: "viewport"; w: number; h: number }

export type PatchOp =
  | PatchOpProp | PatchOpAdd | PatchOpRemove | PatchOpReparent
  | PatchOpState | PatchOpStates | PatchOpBindings | PatchOpViewport;

/** The wire envelope POSTed to `…/scene/patch`. `surface` must equal a live
 *  surface's routing key (a scene name today) or the engine ignores the patch
 *  (it's editing something not currently shown). The engine still accepts the
 *  legacy `scene` key as an alias during the migration. */
export interface ScenePatch { surface: string; rev: number; ops: PatchOp[] }

export interface DiffResult {
  ops: PatchOp[];
  /** True when the change can't be hot-patched (scripts list / scene id) and
   *  the caller must do a full save+rebuild instead of streaming ops. */
  needsReload: boolean;
}

/** Ordered child ids of `parent` ("" = root) in the given store. */
function childIdsOf(ds: DocStore, parent: string): string[] {
  return parent === "" ? ds.rootIds : (ds.nodes[parent]?.childIds ?? []);
}

/** A node's wire parent ("" for a root node). */
function wireParent(n: FlatNode): string {
  return n.parentId ?? "";
}

/** A node's serialized props *without* its subtree (the engine's `prop` op
 *  ignores children — structure is driven by add/remove/reparent) and without
 *  editor-only hints (`previewFrame`), so tweaking the designer's preview
 *  frame never streams a pointless patch to the running game. */
function shallowJson(n: FlatNode): Record<string, unknown> {
  const j = nodeToJson({ ...n, children: [] } as unknown as CNode);
  delete (j as { children?: unknown }).children;
  delete (j as { previewFrame?: unknown }).previewFrame;
  return j;
}

export function diffDocs(prev: DocStore, next: DocStore): DiffResult {
  // A scripts-list change (or a different scene) can't hot-patch: the engine
  // needs a fresh host to (re)load the closures. Bail to the rebuild path.
  if (
    prev.name !== next.name ||
    prev.scripts.length !== next.scripts.length ||
    prev.scripts.some((s, i) => s !== next.scripts[i])
  ) {
    return { ops: [], needsReload: true };
  }

  const ops: PatchOp[] = [];

  // ---- removes: only the top-most gone node (its subtree goes with it) ----
  for (const id in prev.nodes) {
    if (next.nodes[id]) continue;
    const parent = wireParent(prev.nodes[id]);
    if (parent !== "" && !next.nodes[parent]) continue; // ancestor already removed
    ops.push({ op: "remove", id });
  }

  // ---- structural reconcile, parent by parent (pre-order so a parent is
  //      placed before its children move into it). For each existing parent
  //      we only touch children that genuinely changed: new ones become an
  //      `add` (subtree inline), moved-in ones a `reparent`, and a `reparent`
  //      per child only when the *relative order* of the surviving children
  //      changed. A plain append / mid-insert / remove shifts the doc Vec on
  //      its own, and the engine's relayout reorders the scene to match — so
  //      no sibling churn is needed for those. ----
  const orderedParents: string[] = [""];
  const collectParents = (ids: string[]): void => {
    for (const id of ids) {
      const n = next.nodes[id];
      if (n && n.childIds.length) { orderedParents.push(id); collectParents(n.childIds); }
    }
  };
  collectParents(next.rootIds);

  for (const parent of orderedParents) {
    // A brand-new parent is created via its ancestor's `add` subtree, so we
    // never process it here (its children ride along in that JSON).
    if (parent !== "" && !prev.nodes[parent]) continue;

    const nextChildren = childIdsOf(next, parent);
    const prevChildren = childIdsOf(prev, parent);
    const nextSet = new Set(nextChildren);
    const prevSet = new Set(prevChildren);
    const commonPrev = prevChildren.filter((id) => nextSet.has(id)).join(",");
    const commonNext = nextChildren.filter((id) => prevSet.has(id)).join(",");
    const reordered = commonPrev !== commonNext;

    nextChildren.forEach((id, index) => {
      const inPrev = prev.nodes[id];
      if (!inPrev) {
        ops.push({ op: "add", parent, index, node: flatNodeToJson(next.nodes, id) });
      } else if (wireParent(inPrev) !== parent || reordered) {
        ops.push({ op: "reparent", id, parent, index });
      }
    });
  }

  // ---- prop edits: any surviving node whose scalar/visual props changed.
  //      Structural sharing lets us skip untouched nodes by identity before
  //      the (costlier) JSON compare. The engine's `prop` op also re-applies
  //      bindings + name, so binding and state-tag edits ride along here. ----
  for (const id in next.nodes) {
    const p = prev.nodes[id];
    if (!p) continue;
    const n = next.nodes[id];
    if (p === n) continue; // referentially identical → definitely unchanged
    const before = shallowJson(p);
    const after = shallowJson(n);
    if (JSON.stringify(before) !== JSON.stringify(after)) {
      ops.push({ op: "prop", id, node: after });
    }
  }

  // ---- declared states (defaults + bit order) ----
  if (JSON.stringify(prev.states) !== JSON.stringify(next.states)) {
    ops.push({ op: "states", list: next.states.map((s) => ({ name: s.name, on: s.on })) });
  }

  // ---- viewport ----
  const pw = prev.viewport?.[0] ?? 800, ph = prev.viewport?.[1] ?? 600;
  const nw = next.viewport?.[0] ?? 800, nh = next.viewport?.[1] ?? 600;
  if (pw !== nw || ph !== nh) ops.push({ op: "viewport", w: nw, h: nh });

  return { ops, needsReload: false };
}
