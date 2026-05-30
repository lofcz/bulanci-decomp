// Raw on-disk shapes — kept separate from the runtime `CatalogPayload`
// in `src/types.ts` so the merge logic can be unit-tested without the
// rendering stack.  Mirrors what `ghidra_analysis/asset_catalog/serve.py`
// reads from disk before `_build_payload` munges everything into the
// single blob the UI consumes.

import type { AudioBankMeta, Xrefs } from "@/types";

/** A single row from `catalog.json` produced by `build_asset_catalog.py`. */
export interface RawCatalogEntry {
  id:         number;
  idHex:      string;
  classId:    number;
  className:  string;
  files?:     string[];
  name?:      string | null;
  size?:      number;
  audioBank?: AudioBankMeta | null;
  orphanOnly?: boolean;
  deadOnly?:   boolean;
  adjacencyOnly?: boolean;
  overlayOnly?:   boolean;
  xrefs?:    Xrefs;
}

export interface RawCatalog {
  entries: RawCatalogEntry[];
  /** Index by className -> id list; we currently don't consume it. */
  byClass?: Record<string, number[]>;
  /** Coverage summary, present in newer catalog files. */
  coverage?: Record<string, number>;
}

/** A single `assets["0x..."]` entry from `registry.json`. */
export interface RegistryAsset {
  slug?:    string;
  notes?:   string;
  tags?:    string[];
  /** Whether the asset pipeline should ship this asset (default false). */
  ship?:    boolean;
  /** Project-level folder.  Forward-slash separated path, e.g.
   *  `"menu/widgets/radio"`.  Purely organisational — see FOLDERS.md
   *  for how this becomes a strongly-typed Rust module path while the
   *  shipped binary references assets only via hashed runtime IDs. */
  folder?:  string;
  /** AudioBank only: `<sampleIndex>` -> production slug. */
  samples?: Record<string, string>;
}

/** A single `scripts["<dec>"]` entry from `registry.json`. */
export interface RegistryScript {
  slug?:  string;
  notes?: string;
}

/** A single `folders["path/here"]` entry — sparse metadata table that
 *  attaches notes / aggregate tags to a folder.  Only folders that
 *  actually need notes appear here; an asset may sit in a folder that
 *  has no `folders[…]` entry. */
export interface RegistryFolder {
  notes?: string;
  tags?:  string[];
}

/** Shape of `registry.json` after `JSON.parse`.  Mirrors
 *  `registry.schema.json` — keep them in sync. */
export interface RawRegistry {
  $schema?: string;
  assets?:  Record<string, RegistryAsset>;
  scripts?: Record<string, RegistryScript>;
  folders?: Record<string, RegistryFolder>;
}

export type RawCoverage = Record<string, number | Record<string, number>>;
