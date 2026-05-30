// Reads the codegen's fat `manifest.json` (hash → metadata) and turns
// a catalog entry into the set of live-overlay *push targets* the
// engine bridge understands.
//
// `manifest.json` is the authoritative handle table emitted by
// `build_typed_handles.py`.  It enumerates every handle — including
// the siblings that don't get their own catalog entry, like an atlas's
// PNG (`<slug>_png`) or a bitmap font's metrics JSON (`<slug>_json`).
// Resolving against it means the editor never has to re-derive the
// per-class path layout; it just matches the catalog entry's
// `(folder, slug)` family and reads each handle's real `source`.
//
// When the manifest isn't reachable (e.g. a static HTTP deploy that
// doesn't serve it) we fall back to hashing the entry's own
// `(folder, slug)` with `handleHash`, which the parity test proves is
// byte-identical to the codegen.

import type { CatalogEntry } from "@/types";
import { handleHash } from "./handleHash";

/** One row of the fat `manifest.json`, keyed in the file by hash hex. */
export interface ManifestRecord {
  class:  string;
  folder: string | null;
  slug:   string;
  source: string;
}

/** `{ "<hash_hex>": { class, folder, slug, source } }`. */
export type HandleManifest = Record<string, ManifestRecord>;

/** A single asset stream the editor can overwrite live. */
export interface PushTarget {
  /** 16-hex handle hash → `PUT /overlay/hash/<hashHex>`. */
  hashHex: string;
  /** Short label, e.g. `atlas.png` or `image (jpg)`. */
  label: string;
  /** Pack-relative source path, for the tooltip. */
  source: string;
  /** `<input accept>` list, e.g. `[".png"]`.  Empty = any file. */
  accept: string[];
  /** The runtime consumes a transcoded (MsgPack) form of this asset,
   *  so raw file bytes can't be pushed directly — the UI disables the
   *  picker and explains why. */
  needsTranscode: boolean;
  /** Leaf identifier (carried through to the persisted pak entry). */
  slug: string;
  /** Project folder path, or null for root-level assets. */
  folder: string | null;
  /** Catalog class name (BitmapJpeg, AudioBank, …). */
  className: string;
  /** Lower-cased file extension incl. dot, e.g. `.jpg` ("" if unknown). */
  ext: string;
}

/** Classes whose on-disk JSON is transcoded to MsgPack inside the pack
 *  (see `pack_assets.py`).  Pushing the raw JSON wouldn't parse — the
 *  editor would need to MsgPack-encode first, which it doesn't yet. */
const TRANSCODED_CLASSES = new Set(["BitmapSprite", "CursorComposition"]);

function targetFromRecord(hashHex: string, rec: ManifestRecord): PushTarget | null {
  const src = rec.source;
  if (src.endsWith("/")) return null;            // bank/anim directory pseudo-entry
  const ext = extOf(src);
  return {
    hashHex,
    label: baseName(src),
    source: src,
    accept: ext ? [ext] : [],
    needsTranscode: TRANSCODED_CLASSES.has(rec.class),
    slug: rec.slug,
    folder: rec.folder ?? null,
    className: rec.class,
    ext,
  };
}

/** Push targets for a catalog entry, richest-first.  Requires a slug
 *  (unnamed assets have no stable handle).  Uses the manifest when
 *  given; otherwise emits a single best-effort target from the entry's
 *  own `(folder, slug)`. */
export function resolveTargets(
  entry: CatalogEntry,
  manifest: HandleManifest | null,
): PushTarget[] {
  if (!entry.slug) return [];
  const folder = entry.folder ?? null;
  const slug = entry.slug;

  if (manifest) {
    const family = new Set([slug, `${slug}_png`, `${slug}_json`]);
    const out: PushTarget[] = [];
    for (const [hashHex, rec] of Object.entries(manifest)) {
      if ((rec.folder ?? null) !== folder) continue;
      if (!family.has(rec.slug)) continue;
      const t = targetFromRecord(hashHex, rec);
      if (t) out.push(t);
    }
    if (out.length > 0) {
      // Directly-pushable streams first, base slug before siblings.
      out.sort((a, b) =>
        Number(a.needsTranscode) - Number(b.needsTranscode) ||
        a.source.length - b.source.length);
      return out;
    }
  }

  // Fallback: hash the entry's own (folder, slug).  We don't know the
  // exact extension without the manifest, so accept anything.
  return [{
    hashHex: handleHash(folder, slug),
    label: slug,
    source: `${folder ?? "_root"}/${slug}`,
    accept: [],
    needsTranscode: false,
    slug,
    folder,
    className: entry.className ?? "",
    ext: "",
  }];
}

function extOf(path: string): string {
  const dot = path.lastIndexOf(".");
  const slash = path.lastIndexOf("/");
  if (dot <= slash) return "";
  return path.slice(dot).toLowerCase();
}

function baseName(path: string): string {
  const slash = path.lastIndexOf("/");
  return slash >= 0 ? path.slice(slash + 1) : path;
}
