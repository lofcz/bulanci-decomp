// Edits over `registry.json`.
//
// The on-disk format is plain JSON now (see `registry.schema.json`),
// so editing is just `JSON.parse` → mutate the object → `JSON.stringify`
// with a stable key order.  No regex, no AST library — the format is
// strict enough that round-tripping never loses information.
//
// Stable ordering matters for diff hygiene: every save produces the
// same byte layout regardless of how the editor walked the tree, so
// `git status` only flags actual changes.

import type {
  RawRegistry, RegistryAsset, RegistryFolder,
} from "./raw";

// ---------------------------------------------------------------------
// Serialisation
// ---------------------------------------------------------------------

/** 2-space indent, deterministic key order so consecutive saves
 *  produce zero-noise diffs.  Top-level sections come out in the same
 *  order as `registry.schema.json` describes them. */
export function serializeRegistry(reg: RawRegistry): string {
  // We can't trust `JSON.stringify(obj, null, 2)` for ordering — V8
  // preserves insertion order on string-keyed objects but the editor
  // mutates in place, so we rebuild a fresh, fully-sorted object.
  const out: Record<string, unknown> = {};
  if (reg.$schema) out.$schema = reg.$schema;
  else             out.$schema = "./registry.schema.json";

  out.scripts = sortByKey(reg.scripts || {}, (a, b) => Number(a) - Number(b));
  out.assets  = sortByKey(reg.assets  || {}, lexCompare);
  out.folders = sortByKey(reg.folders || {}, lexCompare);

  return JSON.stringify(out, null, 2) + "\n";
}

const lexCompare = (a: string, b: string) => a.localeCompare(b);

function sortByKey<V>(obj: Record<string, V>, cmp: (a: string, b: string) => number): Record<string, V> {
  const out: Record<string, V> = {};
  for (const k of Object.keys(obj).sort(cmp)) out[k] = obj[k];
  return out;
}

// ---------------------------------------------------------------------
// Folder ops — the tree view's primary surface
// ---------------------------------------------------------------------

const PATH_RE = /^[A-Za-z0-9_/-]+$/;
const SLUG_RE = /^[a-z][a-z0-9_]*$/;

function validatePath(p: string): string {
  const cleaned = p.split("/").map(s => s.trim()).filter(Boolean).join("/");
  if (!cleaned) throw new Error("folder path may not be empty");
  if (!PATH_RE.test(cleaned)) {
    throw new Error(`folder path ${JSON.stringify(cleaned)} may only contain letters, digits, _, -, /`);
  }
  return cleaned;
}

/** Set / clear an asset's folder.  Pass `null` to drop the field
 *  (the asset becomes "(unsorted)").  Throws if the asset isn't in
 *  the registry yet — use `upsertAsset` first. */
export function setAssetFolder(reg: RawRegistry, idHex: string, folder: string | null): RawRegistry {
  const assets = reg.assets ?? {};
  const entry  = assets[idHex];
  if (!entry) throw new Error(`asset ${idHex} not found in registry`);
  if (folder === null) {
    const { folder: _, ...rest } = entry;
    assets[idHex] = rest;
  } else {
    assets[idHex] = { ...entry, folder: validatePath(folder) };
  }
  return { ...reg, assets };
}

/** Move every reference to `oldPath` (and any descendant
 *  `oldPath/x/y`) over to `newPath`.  Rewrites both asset folders and
 *  the `folders` metadata map. */
export function renameFolder(reg: RawRegistry, oldPath: string, newPath: string): RawRegistry {
  const from = validatePath(oldPath);
  const to   = validatePath(newPath);
  if (from === to) return reg;

  const rename = (path: string): string => {
    if (path === from) return to;
    if (path.startsWith(from + "/")) return to + path.slice(from.length);
    return path;
  };

  const assets: Record<string, RegistryAsset> = {};
  for (const [id, a] of Object.entries(reg.assets ?? {})) {
    assets[id] = a.folder ? { ...a, folder: rename(a.folder) } : a;
  }

  const folders: Record<string, RegistryFolder> = {};
  for (const [p, meta] of Object.entries(reg.folders ?? {})) {
    folders[rename(p)] = meta;
  }

  return { ...reg, assets, folders };
}

/** Drop a folder from every asset that points at it (or any
 *  descendant) — those assets become "(unsorted)".  Also removes any
 *  matching `folders` metadata entries.  The tree view's "Delete
 *  folder" action calls this. */
export function deleteFolder(reg: RawRegistry, path: string, opts: {
  /** When true, descendant folders (`path/x/y`) are dropped too —
   *  matches the "delete subtree" semantics most file explorers use. */
  recursive?: boolean;
} = {}): RawRegistry {
  const target = validatePath(path);
  const inSubtree = (p: string) =>
    p === target || (opts.recursive && p.startsWith(target + "/"));

  const assets: Record<string, RegistryAsset> = {};
  for (const [id, a] of Object.entries(reg.assets ?? {})) {
    if (a.folder && inSubtree(a.folder)) {
      const { folder: _, ...rest } = a;
      assets[id] = rest;
    } else {
      assets[id] = a;
    }
  }

  const folders: Record<string, RegistryFolder> = {};
  for (const [p, meta] of Object.entries(reg.folders ?? {})) {
    if (!inSubtree(p)) folders[p] = meta;
  }

  return { ...reg, assets, folders };
}

/** Set / clear folder-level notes.  Creates the `folders` entry if
 *  needed; deletes it when both notes and tags would be empty so the
 *  sparse-map invariant holds. */
export function setFolderNotes(reg: RawRegistry, path: string, notes: string | null): RawRegistry {
  const target  = validatePath(path);
  const folders = { ...(reg.folders ?? {}) };
  const cur     = folders[target] ?? {};
  if (notes === null || notes.trim() === "") {
    const { notes: _, ...rest } = cur;
    if (!rest.tags || rest.tags.length === 0) delete folders[target];
    else folders[target] = rest;
  } else {
    folders[target] = { ...cur, notes };
  }
  return { ...reg, folders };
}

// ---------------------------------------------------------------------
// Asset ops — slug / notes from the detail panel
// ---------------------------------------------------------------------

/** Upsert an asset block.  When the entry doesn't exist yet we
 *  require a slug (the schema's only mandatory field).  Existing
 *  entries get a shallow merge — pass `undefined` to leave fields
 *  untouched, `null` to clear, value to overwrite. */
export function upsertAsset(reg: RawRegistry, idHex: string, patch: {
  slug?:   string | null;
  notes?:  string | null;
  folder?: string | null;
  ship?:   boolean | null;
  tags?:   string[] | null;
}): RawRegistry {
  if (!/^0x[0-9a-f]+$/i.test(idHex)) {
    throw new Error(`asset id ${idHex} must look like "0x000100ae"`);
  }
  const assets = { ...(reg.assets ?? {}) };
  const existing = assets[idHex];
  if (!existing && !patch.slug) {
    throw new Error(`asset ${idHex} not in registry yet — provide a slug to create it`);
  }
  const next: RegistryAsset = { ...(existing ?? {}) };

  for (const key of ["slug", "notes", "folder"] as const) {
    const v = patch[key];
    if (v === undefined) continue;
    if (v === null || v === "") delete next[key];
    else                        next[key] = key === "folder" ? validatePath(v) : v;
  }
  if (patch.slug !== undefined && patch.slug !== null && !SLUG_RE.test(patch.slug)) {
    throw new Error(`slug ${JSON.stringify(patch.slug)} must match ${SLUG_RE}`);
  }
  if (patch.ship !== undefined) {
    if (patch.ship === null) delete next.ship;
    else                     next.ship = patch.ship;
  }
  if (patch.tags !== undefined) {
    if (patch.tags === null || patch.tags.length === 0) delete next.tags;
    else                                                next.tags = [...patch.tags];
  }
  assets[idHex] = next;
  return { ...reg, assets };
}

// ---------------------------------------------------------------------
// Tree projection — what the FolderTree consumes
// ---------------------------------------------------------------------

export interface FolderNode {
  /** Path from the root, e.g. `"menu/widgets/radio"`.  The empty
   *  string is the synthetic root holding everything; children
   *  inherit its empty prefix. */
  path:     string;
  /** Display label = last segment of `path` (or `""` for root). */
  name:     string;
  /** Notes from the `folders` map, if any. */
  notes:    string | null;
  /** Direct children only (`menu/widgets` lists `radio` and
   *  `menu_button` but not `radio/foo`). */
  children: FolderNode[];
  /** Total asset count under this node, including descendants. */
  assetCount: number;
}

/** Build the folder tree from a registry + the union of folders
 *  seen on any asset.  Includes folders that only have metadata
 *  (in `folders`) but no assets yet, so empty folders show up.  Also
 *  surfaces ancestor folders that aren't explicitly listed (e.g.
 *  `menu` shows up even if only `menu/sfx/*` is populated). */
export function buildFolderTree(reg: RawRegistry): FolderNode {
  const counts:   Map<string, number>      = new Map();
  const notesMap: Map<string, string>      = new Map();
  const paths:    Set<string>              = new Set();

  for (const a of Object.values(reg.assets ?? {})) {
    if (!a.folder) continue;
    for (const anc of ancestors(a.folder)) {
      counts.set(anc, (counts.get(anc) ?? 0) + 1);
      paths.add(anc);
    }
  }
  for (const [p, meta] of Object.entries(reg.folders ?? {})) {
    paths.add(p);
    if (meta.notes) notesMap.set(p, meta.notes);
  }

  const root: FolderNode = { path: "", name: "", notes: null, children: [], assetCount: counts.get("") ?? 0 };
  const byPath: Map<string, FolderNode> = new Map([["", root]]);

  const sorted = [...paths].sort(lexCompare);
  for (const p of sorted) {
    const segments = p.split("/");
    let   parent   = "";
    for (let i = 0; i < segments.length; i++) {
      const cur = segments.slice(0, i + 1).join("/");
      if (!byPath.has(cur)) {
        const node: FolderNode = {
          path: cur,
          name: segments[i],
          notes: notesMap.get(cur) ?? null,
          children: [],
          assetCount: counts.get(cur) ?? 0,
        };
        byPath.set(cur, node);
        byPath.get(parent)!.children.push(node);
      }
      parent = cur;
    }
  }
  // Root's `assetCount` is the total over every folder; assets without
  // a folder are NOT counted here (those live under the "unsorted"
  // pseudo-bucket in the gallery sidebar).
  let total = 0;
  for (const n of byPath.values()) if (n.path && !n.path.includes("/")) total += n.assetCount;
  root.assetCount = total;
  return root;
}

function ancestors(path: string): string[] {
  const parts = path.split("/");
  const out: string[] = [];
  for (let i = 1; i <= parts.length; i++) out.push(parts.slice(0, i).join("/"));
  return out;
}
