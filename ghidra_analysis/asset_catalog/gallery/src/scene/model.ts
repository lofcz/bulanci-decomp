// Declarative scene model (mirrors core/src/scene/{document,control}.rs).
//
// Pure data + transforms shared by the zustand workspace store and the editor
// panels. All Rust fields carry `#[serde(default)]`, so a partial JSON loads
// fine and unknown keys are ignored — that lets us persist editor-only hints
// (e.g. `previewFrame`) inside the node JSON without the engine choking on
// them. The editor normalizes to full nodes in-memory and re-emits a compact
// doc that omits defaults (matching the clean hand-authored style).
//
// Two node shapes coexist on purpose:
//   * `CNode` / `Doc` — the *nested* interchange used only at the edges: load
//     (`normDoc`) and save (`docToJson`). Trees are easy to (de)serialize.
//   * `FlatNode` / `DocStore` — the *normalized* runtime shape the store and
//     UI actually use: every node is a separate `nodes[id]` entry with
//     `parentId` + `childIds`, so editing one node touches one entry (Mutative
//     structural sharing) and a component subscribed to `nodes[id]` re-renders
//     alone. `normalize`/`denormalize` bridge the two at the edges.

import { createContext, useContext } from "react";
import type { CatalogEntry, FontMeta } from "@/types";

// --------------------------------------------------------------- types

export type Kind = "group" | "sprite" | "text" | "quad" | "input_region" | "button";

export interface Layout {
  mode: string;            // "absolute" | "flex" | "block"
  x: number; y: number;
  width: number | null; height: number | null;
  direction: string;      // "row" | "column"
  justify: string; align: string;
  gap: number; padding: number;
}

export interface Binding { prop: string; key: string }

/** Scalar/visual props shared by both node shapes (everything except the
 *  structural links). Keeps `FlatNode` and the nested `CNode` in lock-step. */
export interface NodeProps {
  id: string;
  name: string;
  kind: Kind;
  layout: Layout;
  sx: number; sy: number; rot: number;
  state: string;
  tint: string;
  handle: string; frame: number;
  text: string; size: number; spacing: number; anchor: string;
  color: string;
  label: string;
  // Editor-only: the atlas/anim frame the *designer* previews (default 0).
  // Persisted in node JSON and ignored by the engine (serde default).
  previewFrame: number;
  bindings: Binding[];
}

/** Nested interchange node — used only by load/save (de)serialization. */
export interface CNode extends NodeProps {
  children: CNode[];
}

/** Normalized runtime node — one entry per id in `DocStore.nodes`. */
export interface FlatNode extends NodeProps {
  parentId: string | null;   // null = a root node
  childIds: string[];
}

export interface StateInit { name: string; on: boolean }

/** Nested document — load/save interchange only. */
export interface Doc {
  name: string;
  scripts: string[];
  states: StateInit[];
  viewport: [number, number] | null;
  root: CNode[];
}

/** Normalized document — the undoable runtime shape (held by Travels and
 *  mirrored into the store). `nodes` is the flat id->node map; `rootIds` is the
 *  ordered top-level list. The non-node scalars mirror `Doc`. */
export interface DocStore {
  name: string;
  scripts: string[];
  states: StateInit[];
  viewport: [number, number] | null;
  nodes: Record<string, FlatNode>;
  rootIds: string[];
}

export interface ControlRegistry {
  version: number;
  kinds: Record<string, string[]>;
  common: string[];
  bindable: string[];
}

// Committed fallback (mirrors core/src/scene/registry.rs) so the designer
// works even when the live `control-registry.json` isn't served (e.g. the
// File System Access source with no backend). Overridden by the fetched file.
export const FALLBACK_REGISTRY: ControlRegistry = {
  version: 1,
  kinds: {
    group: [],
    sprite: ["handle", "frame"],
    text: ["text", "size", "spacing", "anchor"],
    quad: ["color"],
    input_region: [],
    button: ["label", "size", "spacing", "color"],
  },
  common: ["name", "state", "tint", "sx", "sy", "rot", "layout", "bindings"],
  bindable: ["visible", "x", "y", "frame", "text", "tint", "color"],
};

export const DEFAULT_VIEWPORT: [number, number] = [800, 600];

// --------------------------------------------------------------- ids

// Stable, persisted node identity (mirrors `ControlNode.id` in control.rs). It
// is written to the `.scene.json` and is the address space the engine's HMR
// patches use to find the live NodeId across reorders/reparents, AND the key of
// the normalized `nodes` map + the React key. Generated short + random so it's
// unique within a doc and stable once saved. Nodes loaded with an existing id
// keep it (deduped on collision in `normalize`); id-less nodes (new, cloned, or
// legacy files) mint a fresh one.
let _idSeq = 0;
export function nextId(): string {
  _idSeq += 1;
  return `c${_idSeq.toString(36)}${Math.random().toString(36).slice(2, 6)}`;
}

export function defaultLayout(): Layout {
  return {
    mode: "absolute", x: 0, y: 0, width: null, height: null,
    direction: "row", justify: "start", align: "start", gap: 0, padding: 0,
  };
}

/** A fresh, detached normalized node (root by default; link it with
 *  `insertFlatNode`). */
export function defaultFlatNode(kind: Kind): FlatNode {
  return {
    id: nextId(), name: "", kind, layout: defaultLayout(),
    sx: 1, sy: 1, rot: 0, state: "", tint: "", handle: "", frame: 0,
    text: "", size: 16, spacing: 0, anchor: "baseline", color: "", label: "",
    previewFrame: 0, bindings: [], parentId: null, childIds: [],
  };
}

function num(v: unknown, d: number): number {
  return typeof v === "number" && Number.isFinite(v) ? v : d;
}
function str(v: unknown, d: string): string {
  return typeof v === "string" ? v : d;
}

// --------------------------------------------------------------- parse (nested)

function normLayout(raw: Record<string, unknown> | undefined): Layout {
  const d = defaultLayout();
  if (!raw) return d;
  return {
    mode: str(raw.mode, d.mode),
    x: num(raw.x, 0), y: num(raw.y, 0),
    width: raw.width == null ? null : num(raw.width, 0),
    height: raw.height == null ? null : num(raw.height, 0),
    direction: str(raw.direction, d.direction),
    justify: str(raw.justify, d.justify),
    align: str(raw.align, d.align),
    gap: num(raw.gap, 0), padding: num(raw.padding, 0),
  };
}

export function normNode(raw: Record<string, unknown>): CNode {
  const kind = (str(raw.kind, "group") as Kind);
  const bindingsRaw = Array.isArray(raw.bindings) ? raw.bindings : [];
  const childrenRaw = Array.isArray(raw.children) ? raw.children : [];
  return {
    // Keep a persisted id when present; mint one for legacy/id-less nodes so
    // every in-memory node is HMR-addressable (it's serialized on next save).
    id: str(raw.id, "") || nextId(),
    name: str(raw.name, ""),
    kind,
    layout: normLayout(raw.layout as Record<string, unknown> | undefined),
    sx: num(raw.sx, 1), sy: num(raw.sy, 1), rot: num(raw.rot, 0),
    state: str(raw.state, ""),
    tint: str(raw.tint, ""),
    handle: str(raw.handle, ""), frame: num(raw.frame, 0),
    text: str(raw.text, ""), size: num(raw.size, 16),
    spacing: num(raw.spacing, 0), anchor: str(raw.anchor, "baseline"),
    color: str(raw.color, ""), label: str(raw.label, ""),
    // editor-only hint; the engine ignores it (serde default)
    previewFrame: num(raw.previewFrame, 0),
    bindings: bindingsRaw.map((b) => ({
      prop: str((b as Record<string, unknown>).prop, ""),
      key: str((b as Record<string, unknown>).key, ""),
    })),
    children: childrenRaw.map((c) => normNode(c as Record<string, unknown>)),
  };
}

export function normDoc(raw: Record<string, unknown>, fallbackName: string): Doc {
  const statesRaw = Array.isArray(raw.states) ? raw.states : [];
  const rootRaw = Array.isArray(raw.root) ? raw.root : [];
  const vp = raw.viewport;
  return {
    name: str(raw.name, fallbackName),
    scripts: Array.isArray(raw.scripts) ? raw.scripts.map((s) => String(s)) : [],
    states: statesRaw.map((s) => ({
      name: str((s as Record<string, unknown>).name, ""),
      on: !!(s as Record<string, unknown>).on,
    })),
    viewport: Array.isArray(vp) && vp.length === 2 ? [num(vp[0], 800), num(vp[1], 600)] : null,
    root: rootRaw.map((c) => normNode(c as Record<string, unknown>)),
  };
}

// --------------------------------------------------------------- normalize / denormalize

/** Flatten a nested `Doc` into the normalized `DocStore`, assigning
 *  `parentId`/`childIds` and de-duplicating any id collisions (a malformed
 *  file with repeated ids would otherwise clobber map entries). */
export function normalize(doc: Doc): DocStore {
  const nodes: Record<string, FlatNode> = {};
  const seen = new Set<string>();
  const walk = (cnodes: CNode[], parentId: string | null): string[] => {
    const ids: string[] = [];
    for (const c of cnodes) {
      let id = c.id || nextId();
      if (seen.has(id)) id = nextId();   // collision → re-mint
      seen.add(id);
      const flat: FlatNode = {
        id, name: c.name, kind: c.kind, layout: c.layout,
        sx: c.sx, sy: c.sy, rot: c.rot, state: c.state, tint: c.tint,
        handle: c.handle, frame: c.frame, text: c.text, size: c.size,
        spacing: c.spacing, anchor: c.anchor, color: c.color, label: c.label,
        previewFrame: c.previewFrame, bindings: c.bindings,
        parentId, childIds: [],
      };
      nodes[id] = flat;
      flat.childIds = walk(c.children, id);
      ids.push(id);
    }
    return ids;
  };
  const rootIds = walk(doc.root, null);
  return { name: doc.name, scripts: doc.scripts, states: doc.states, viewport: doc.viewport, nodes, rootIds };
}

function flatToCNode(nodes: Record<string, FlatNode>, id: string): CNode {
  const n = nodes[id];
  return {
    id: n.id, name: n.name, kind: n.kind, layout: n.layout,
    sx: n.sx, sy: n.sy, rot: n.rot, state: n.state, tint: n.tint,
    handle: n.handle, frame: n.frame, text: n.text, size: n.size,
    spacing: n.spacing, anchor: n.anchor, color: n.color, label: n.label,
    previewFrame: n.previewFrame, bindings: n.bindings,
    children: n.childIds.map((c) => flatToCNode(nodes, c)),
  };
}

/** Rebuild the nested `Doc` for serialization / diffing. Only called at the
 *  edges (save, full live-sync) — never on a per-edit or per-drag path. */
export function denormalize(ds: DocStore): Doc {
  return {
    name: ds.name, scripts: ds.scripts, states: ds.states, viewport: ds.viewport,
    root: ds.rootIds.map((id) => flatToCNode(ds.nodes, id)),
  };
}

/** Serialize one normalized node and its subtree to the engine's nested node
 *  JSON (used by the differ's `add` op). */
export function flatNodeToJson(nodes: Record<string, FlatNode>, id: string): Record<string, unknown> {
  return nodeToJson(flatToCNode(nodes, id));
}

// --------------------------------------------------------------- serialize (nested)

function layoutToJson(l: Layout): Record<string, unknown> | undefined {
  const o: Record<string, unknown> = {};
  if (l.mode !== "absolute") o.mode = l.mode;
  if (l.x !== 0) o.x = l.x;
  if (l.y !== 0) o.y = l.y;
  if (l.width != null) o.width = l.width;
  if (l.height != null) o.height = l.height;
  if (l.mode === "flex") {
    if (l.direction !== "row") o.direction = l.direction;
    if (l.justify !== "start") o.justify = l.justify;
    if (l.align !== "start") o.align = l.align;
    if (l.gap !== 0) o.gap = l.gap;
    if (l.padding !== 0) o.padding = l.padding;
  }
  return Object.keys(o).length ? o : undefined;
}

export function nodeToJson(n: CNode): Record<string, unknown> {
  const o: Record<string, unknown> = {};
  if (n.id) o.id = n.id;
  o.kind = n.kind;
  if (n.name) o.name = n.name;
  const L = layoutToJson(n.layout);
  if (L) o.layout = L;
  if (n.sx !== 1) o.sx = n.sx;
  if (n.sy !== 1) o.sy = n.sy;
  if (n.rot !== 0) o.rot = n.rot;
  if (n.state) o.state = n.state;
  if (n.tint) o.tint = n.tint;
  if (n.handle) o.handle = n.handle;
  if (n.frame !== 0) o.frame = n.frame;
  if (n.text) o.text = n.text;
  if (n.size !== 16) o.size = n.size;
  if (n.spacing !== 0) o.spacing = n.spacing;
  if (n.anchor !== "baseline") o.anchor = n.anchor;
  if (n.color) o.color = n.color;
  if (n.label) o.label = n.label;
  // Editor-only preview hint. Persisted so the chosen frame survives reloads;
  // the engine's serde model ignores the unknown key.
  if (n.previewFrame !== 0) o.previewFrame = n.previewFrame;
  const binds = n.bindings.filter((b) => b.prop && b.key);
  if (binds.length) o.bindings = binds.map((b) => ({ prop: b.prop, key: b.key }));
  if (n.children.length) o.children = n.children.map(nodeToJson);
  return o;
}

export function docToJson(doc: Doc): string {
  const o: Record<string, unknown> = { name: doc.name };
  if (doc.scripts.length) o.scripts = doc.scripts;
  if (doc.states.length) o.states = doc.states.map((s) => ({ name: s.name, on: s.on }));
  if (doc.viewport && (doc.viewport[0] !== 800 || doc.viewport[1] !== 600)) o.viewport = doc.viewport;
  o.root = doc.root.map(nodeToJson);
  return JSON.stringify(o, null, 2) + "\n";
}

// --------------------------------------------------------------- normalized tree ops
//
// Mutating helpers meant to run *inside* a Mutative recipe (Travels) against a
// `DocStore` draft. Each touches only the entries it must — the parent's
// `childIds` and the affected node(s) — so structural sharing keeps every other
// node entry referentially stable across the edit.

/** The ordered child-id list a node lives in (`null` parent = the root list). */
function childListOf(ds: DocStore, parentId: string | null): string[] {
  return !parentId ? ds.rootIds : ds.nodes[parentId].childIds;
}

/** Every id in the subtree rooted at `id` (inclusive), pre-order. */
export function collectDescendants(ds: DocStore, id: string, into: string[] = []): string[] {
  into.push(id);
  for (const c of ds.nodes[id]?.childIds ?? []) collectDescendants(ds, c, into);
  return into;
}

/** Is `id` inside the subtree rooted at `ancestor` (inclusive)? Walks the
 *  parent chain, so it's O(depth) and used as the reparent cycle guard. */
export function isDescendantId(ds: DocStore, id: string, ancestor: string): boolean {
  let p: string | null = id;
  while (p) {
    if (p === ancestor) return true;
    p = ds.nodes[p]?.parentId ?? null;
  }
  return false;
}

/** Insert an already-built node as a child of `parentId` (null = root). */
export function insertFlatNode(ds: DocStore, parentId: string | null, node: FlatNode, index?: number): void {
  const pid = parentId || null;
  node.parentId = pid;
  ds.nodes[node.id] = node;
  const list = childListOf(ds, pid);
  if (index == null || index < 0 || index >= list.length) list.push(node.id);
  else list.splice(index, 0, node.id);
}

/** Remove a node and its whole subtree, unlinking it from its parent. */
export function removeFlatNode(ds: DocStore, id: string): void {
  const node = ds.nodes[id];
  if (!node) return;
  const list = childListOf(ds, node.parentId);
  const i = list.indexOf(id);
  if (i >= 0) list.splice(i, 1);
  for (const d of collectDescendants(ds, id)) delete ds.nodes[d];
}

/** Move an existing node under `newParentId` (null = root) at `index`. */
export function reparentFlatNode(ds: DocStore, id: string, newParentId: string | null, index?: number): void {
  const node = ds.nodes[id];
  if (!node) return;
  const from = childListOf(ds, node.parentId);
  const fi = from.indexOf(id);
  if (fi >= 0) from.splice(fi, 1);
  const pid = newParentId || null;
  node.parentId = pid;
  const to = childListOf(ds, pid);
  if (index == null || index < 0 || index >= to.length) to.push(id);
  else to.splice(index, 0, id);
}

/** Deep-clone the subtree rooted at `srcId` into `ds` with fresh ids, link the
 *  copy under `parentId` (null = root), and return the new root id. */
export function cloneSubtreeInto(ds: DocStore, srcId: string, parentId: string | null): string {
  const build = (sid: string, pid: string | null): string => {
    const src = ds.nodes[sid];
    const copy: FlatNode = {
      ...src,
      id: nextId(),
      layout: { ...src.layout },
      bindings: src.bindings.map((b) => ({ ...b })),
      parentId: pid,
      childIds: [],
    };
    ds.nodes[copy.id] = copy;
    copy.childIds = src.childIds.map((c) => build(c, copy.id));
    return copy.id;
  };
  const newRoot = build(srcId, parentId || null);
  childListOf(ds, parentId || null).push(newRoot);
  return newRoot;
}

// --------------------------------------------------------------- color utils

export function rgba(hex: string, fallback = "ffffffff"): string {
  const h = (hex || fallback).replace(/[^0-9a-f]/gi, "").padEnd(8, "f").slice(0, 8);
  const r = parseInt(h.slice(0, 2), 16), g = parseInt(h.slice(2, 4), 16);
  const b = parseInt(h.slice(4, 6), 16), a = parseInt(h.slice(6, 8), 16) / 255;
  return `rgba(${r},${g},${b},${a})`;
}

// --------------------------------------------------------------- scene assets
//
// Resolves the control tree's sprite handles and text sizes to the *real*
// catalog assets so the preview shows the actual menu art at native pixel
// size (the same pixels the engine uploads) instead of placeholder boxes.

export interface SceneAssets {
  /** Sprite handle ("folder/slug") -> merged catalog entry, or null. */
  resolve: (handle: string) => CatalogEntry | null;
  /** Text size -> the bitmap font the engine picks (small/medium/large). */
  pickFont: (size: number) => CatalogEntry | null;
}

export const SceneAssetsCtx = createContext<SceneAssets>({ resolve: () => null, pickFont: () => null });
export const useSceneAssets = (): SceneAssets => useContext(SceneAssetsCtx);

export function buildSceneAssets(entries: CatalogEntry[]): SceneAssets {
  const byHandle = new Map<string, CatalogEntry>();
  for (const e of entries) {
    if (e.folder && e.slug) byHandle.set(`${e.folder}/${e.slug}`, e);
  }
  const fontBySlug = (slug: string) =>
    entries.find((e) => e.className === "Font" && e.slug === slug) ?? null;
  const small = fontBySlug("font_small");
  const medium = fontBySlug("font_medium");
  const large = fontBySlug("font_large");
  return {
    resolve: (h) => byHandle.get(h) ?? null,
    // Mirrors BitmapFontProvider::pick — <=12 small, 13..=15 medium, >=16 large.
    pickFont: (size) =>
      (size <= 12 ? small : size <= 15 ? medium : large) ?? large ?? medium ?? small,
  };
}

/** ASCII baseline (cap height) of a bitmap font: the engine places a
 *  baseline-anchored string so this many pixels sit above the node's y.
 *  Mirrors `font.rs::estimate_baseline`. */
export function fontBaseline(font: FontMeta): number {
  for (const ch of ["A", "M", "H", "0", "I"]) {
    const c = font.characters.find((x) => x.unicodeChar === ch);
    if (c && c.width !== 0 && c.height > c.offsetY) return c.height;
  }
  return font.lineHeight;
}
