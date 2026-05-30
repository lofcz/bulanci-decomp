// Catalog payload typings — mirrors the merged shape emitted by
// `serve.py::_build_payload` (which itself merges catalog.json +
// registry.toml + coverage.json plus per-asset preview / anim hints).

export interface XrefScript {
  sourcePack: string;
  scriptClass: string;
  scriptId: number;
  offset: number;
  snippet?: string;
}

export interface XrefCode {
  file?: string;
  line?: number | string;
  func?: string;
  snippet?: string;
}

export interface Xrefs {
  insn?:       XrefCode[];
  data?:       XrefCode[];
  ghidraText?: XrefCode[];
  idaText?:    XrefCode[];
  analysisMd?: XrefCode[];
  script?:     XrefScript[];
  [k: string]: unknown;
}

export interface BankSample {
  index: number;
  offsetInBank: number;
  byteLen: number;
  /** Production slug (e.g. "sfx_radio_click") when registry.toml names this slice. */
  slug?: string | null;
}

export interface AudioBankMeta {
  bankResourceID: number;
  reserved?: number;
  sampleCount: number;
  totalBankBytes: number;
  samples: BankSample[];
}

export interface AnimGif {
  kind: "gif";
  filename: string;
  strip?: string | null;
}

export interface AnimFrames {
  kind: "frames";
  frames: string[];
  audioFile?: string | null;
}

export interface AnimNone { kind: "none" }

export type AnimHint = AnimGif | AnimFrames | AnimNone;

export interface CatalogEntry {
  id: number;
  idHex: string;
  key: string;
  className: string;
  classId: number;
  size?: number;
  files: string[];
  preview: string | null;
  anim: AnimHint;
  categories: string[];
  orphanOnly: boolean;
  deadOnly: boolean;
  xrefs: Xrefs;
  audioBank: AudioBankMeta | null;
  indexResourceID?: number;
  indexResourceIDHex?: string;
  poemText?: string | null;
  fontMeta?: FontMeta | null;
  slug?: string | null;
  notes?: string | null;
  tags: string[];
  /** Project-level folder, e.g. `"menu/widgets/radio"`.  Null when
   *  the asset hasn't been sorted into a folder yet — those show up
   *  under the special `(unsorted)` bucket in the gallery. */
  folder?: string | null;
}

/** Optional folder-level metadata from `[folder."path"]` tables in
 *  registry.toml.  Sparse: only folders with actual notes appear. */
export interface FolderMeta {
  notes?: string;
  tags?:  string[];
}

export interface FontCharMeta {
  index: number;
  /** Original engine-side character (null for unmapped cp1250 slots). */
  char: string | null;
  /** Unicode rendering of the character, or null if the cp1250 slot is unassigned. */
  unicodeChar: string | null;
  unicodeCodepoint: number | null;
  offsetX: number;
  width:   number;
  offsetY: number;
  height:  number;
}

export interface FontMeta {
  /** Filename of the atlas PNG within unpacked/overlay/. */
  atlas:        string;
  width:        number;
  height:       number;
  lineHeight:   number;
  defaultWidth: number;
  /** 256 entries, one per cp1250 byte. */
  characters:   FontCharMeta[];
}

export interface CatalogPayload {
  totals: { assets: number; named: number; unnamed: number };
  coverage: Record<string, number>;
  categoryCounts: Record<string, number>;
  classCounts:    Record<string, number>;
  /** Per-folder asset count, with every ancestor path aggregated.
   *  E.g. if 3 assets live in `menu/widgets/radio`, this map has
   *  entries for `menu` (3), `menu/widgets` (3) and
   *  `menu/widgets/radio` (3).  The sentinel key
   *  `"__unsorted__"` counts entries that have no folder assigned. */
  folderCounts:   Record<string, number>;
  /** Folder-level metadata from `[folder."path"]` tables. */
  folderMeta:     Record<string, FolderMeta>;
  scriptRegistry: Record<string, { slug?: string; notes?: string }>;
  entries:        CatalogEntry[];
}

/** Sentinel folder filter for assets without a folder. */
export const UNSORTED_FOLDER = "__unsorted__";
