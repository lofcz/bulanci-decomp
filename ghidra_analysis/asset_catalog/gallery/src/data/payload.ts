// In-browser port of `ghidra_analysis/asset_catalog/serve.py::_build_payload`.
//
// Take the on-disk catalog + registry + coverage + overlay listing,
// produce the merged `CatalogPayload` the UI consumes.  Pure function
// over plain data — no DataSource calls happen here; the caller passes
// in everything we need so this stays trivially testable.

import type {
  AnimHint, BankSample, CatalogEntry,
  CatalogPayload, FolderMeta, FontMeta, Xrefs,
} from "@/types";
import { UNSORTED_FOLDER } from "@/types";
import type {
  RawCatalog, RawCatalogEntry, RawCoverage, RawRegistry,
} from "./raw";

// className -> preferred file extensions (first that exists wins).
//
// Order matters:
//   * .atlas.gif before .atlas.png — animated thumbnails for sprites
//     (the stitched strip is often 20:1 wide and would collapse to a
//     sliver under background-size: contain).
//   * .atlas.png before .frame000.jpg for JpegAnim — the per-asset
//     atlas is a better still preview than just frame 0.
const PREVIEW_RULES: Record<string, string[]> = {
  BitmapJPEG:     [".jpg"],
  BitmapSpecial:  [".png"],
  BitmapSprite:   [".atlas.gif", ".atlas.png"],
  BitmapJpegAnim: [".atlas.png", ".frame000.jpg"],
  Font:           [".png"],
  Mp3:            [".mp3"],
  AudioBank:      [".wav"],
  Poem:           [".txt"],
};

const ANIM_FRAME_RE = /\.frame(\d+)\.jpg$/;

interface BuildOpts {
  /** Optional async loader for an overlay file's raw bytes — used to
   *  inline Font + Poem sidecars so the UI can render them without an
   *  extra roundtrip per tile.  Skipped when `null` (the gallery still
   *  works; those tiles just fall back to placeholders). */
  readFile?: (filename: string) => Promise<Uint8Array> | null;
}

/** Build the runtime payload from raw inputs.
 *
 *  `overlayFiles` is the flat list of every filename in
 *  `unpacked/overlay/` and is used to:
 *    * resolve `.atlas.{gif,png}` / `.frame000.jpg` previews that the
 *      catalog doesn't track explicitly,
 *    * enumerate every BitmapJpegAnim's frames (`*.frameNNN.jpg`).
 *
 *  Pass an empty array if you can't enumerate; the gallery still loads,
 *  but anims and some sprite thumbnails degrade. */
export async function buildPayload(
  catalog:      RawCatalog,
  registry:     RawRegistry,
  coverage:     RawCoverage,
  overlayFiles: string[],
  opts: BuildOpts = {},
): Promise<CatalogPayload> {
  const overlaySet = new Set(overlayFiles);
  // Precompute per-stem frame lists once so each JpegAnim is O(1).
  const framesByStem = collectAnimFrames(overlayFiles);

  const assetReg  = registry.assets  || {};
  const scriptReg = registry.scripts || {};
  const rawEntries = catalog.entries || [];

  // ---- AudioBank ↔ AudioBankIndex merge -------------------------------
  // The index carries the sample table that actually slices the bank's
  // WAV.  We collapse the pair into a single tile per bank.
  const indexByBank = new Map<number, RawCatalogEntry>();
  for (const e of rawEntries) {
    if (e.className === "AudioBankIndex") {
      const bid = (e.audioBank || {}).bankResourceID;
      if (typeof bid === "number") indexByBank.set(bid, e);
    }
  }

  const out: CatalogEntry[] = [];
  const categoryCounts: Record<string, number> = {};
  const classCounts:    Record<string, number> = {};
  const folderCounts:   Record<string, number> = {};
  let   named = 0;

  for (let entry of rawEntries) {
    if (entry.className === "AudioBankIndex") continue;

    // Merge in the matching AudioBankIndex if present.
    let indexResourceID: number | undefined;
    let indexResourceIDHex: string | undefined;
    if (entry.className === "AudioBank") {
      const idx = indexByBank.get(entry.id);
      if (idx) {
        entry = {
          ...entry,
          audioBank: idx.audioBank,
          xrefs:     mergeXrefs(entry.xrefs, idx.xrefs),
        };
        indexResourceID    = idx.id;
        indexResourceIDHex = idx.idHex || `0x${idx.id.toString(16).padStart(8, "0")}`;
      }
    }

    const keyHex = `0x${entry.id.toString(16).padStart(8, "0")}`;
    const reg    = assetReg[keyHex] || {};
    const cats   = categorize(entry, scriptReg);
    const preview = previewFileFor(entry, overlaySet);
    const anim    = animHintFor(entry, overlaySet, framesByStem);

    // ---- Per-class sidecar inlining ----------------------------------
    let poemText: string | null = null;
    let fontMeta: FontMeta | null = null;
    if (opts.readFile) {
      if (entry.className === "Poem") {
        const fn = (entry.files || []).find(f => f.endsWith(".txt"));
        if (fn) poemText = await readText(opts.readFile, fn);
      } else if (entry.className === "Font") {
        const fn = (entry.files || []).find(f => f.endsWith(".font.json"))
                 ?? canonicalName(entry, ".font.json");
        if (fn && overlaySet.has(fn)) {
          const json = await readJson<{
            width: number; height: number; lineHeight: number;
            defaultWidth: number; characters: FontMeta["characters"];
          }>(opts.readFile, fn);
          if (json) {
            fontMeta = {
              atlas:        canonicalName(entry, ".png"),
              width:        json.width,
              height:       json.height,
              lineHeight:   json.lineHeight,
              defaultWidth: json.defaultWidth,
              characters:   json.characters || [],
            };
          }
        }
      }
    }

    // ---- Per-sample slug merging (AudioBank only) --------------------
    let audioBank = entry.audioBank ?? null;
    const sampleNames = reg.samples || {};
    if (audioBank && Object.keys(sampleNames).length > 0) {
      audioBank = {
        ...audioBank,
        samples: (audioBank.samples || []).map((s: BankSample) => {
          const slug = sampleNames[String(s.index)];
          return slug ? { ...s, slug } : s;
        }),
      };
    }

    const merged: CatalogEntry = {
      id:        entry.id,
      idHex:     entry.idHex || keyHex,
      key:       keyHex,
      className: entry.className,
      classId:   entry.classId,
      size:      entry.size,
      files:     entry.files || [],
      preview,
      anim,
      categories: cats,
      orphanOnly: !!entry.orphanOnly,
      deadOnly:   !!entry.deadOnly,
      xrefs:      entry.xrefs || {},
      audioBank,
      indexResourceID,
      indexResourceIDHex,
      poemText,
      fontMeta,
      slug:   reg.slug  || null,
      notes:  reg.notes || null,
      tags:   reg.tags  || [],
      folder: normaliseFolder(reg.folder),
    };
    if (merged.slug) named += 1;
    out.push(merged);

    classCounts[merged.className || "?"] = (classCounts[merged.className || "?"] || 0) + 1;
    for (const c of cats) categoryCounts[c] = (categoryCounts[c] || 0) + 1;

    // Folder bucket — count every ancestor so clicking a parent path
    // in the sidebar acts as a "show this subtree" filter.
    if (merged.folder) {
      for (const p of folderAncestors(merged.folder)) {
        folderCounts[p] = (folderCounts[p] || 0) + 1;
      }
    } else {
      folderCounts[UNSORTED_FOLDER] = (folderCounts[UNSORTED_FOLDER] || 0) + 1;
    }
  }

  // Build folderMeta from the registry — only keep folders that
  // actually contain assets (avoid stale notes from old paths).
  const folderMeta: Record<string, FolderMeta> = {};
  for (const [path, meta] of Object.entries(registry.folders || {})) {
    if (folderCounts[path]) folderMeta[path] = meta as FolderMeta;
  }

  return {
    totals: {
      assets:  out.length,
      named,
      unnamed: out.length - named,
    },
    // Coverage is opaque to the UI right now — just pass it through.
    coverage: coverage as Record<string, number>,
    categoryCounts: sortByKey(categoryCounts),
    classCounts:    sortByKey(classCounts),
    folderCounts:   sortByKey(folderCounts),
    folderMeta,
    scriptRegistry: scriptReg as CatalogPayload["scriptRegistry"],
    entries: out,
  };
}

// ---------------------------------------------------------------------------
// identity-preserving reconciliation
// ---------------------------------------------------------------------------
//
// `buildPayload` is a pure rebuild — it allocates a brand-new
// `CatalogEntry` for *every* asset on every call.  The only inputs that
// actually change at runtime are the registry-derived fields (slug /
// notes / tags / folder / categories / per-sample slugs); the catalog,
// coverage and overlay listing are constant for a session.  So after a
// rebuild we splice the previous payload's object identities back in for
// every entry whose registry-derived fields are unchanged.  That keeps
// `Tile`'s `a.entry === b.entry` memo true for the 341 untouched tiles
// when a single asset's folder changes, instead of re-rendering the
// whole gallery + folder tree.

/** Reuse object identities from `prev` for every structurally-unchanged
 *  entry in `next`.  Returns `prev` verbatim when nothing changed at all
 *  so `useMemo`/`memo` consumers see a stable payload reference. */
export function reconcilePayload(
  prev: CatalogPayload | null | undefined,
  next: CatalogPayload,
): CatalogPayload {
  if (!prev) return next;

  const prevByKey = new Map(prev.entries.map((e) => [e.key, e]));
  let reusedAll = next.entries.length === prev.entries.length;

  const entries = next.entries.map((e, i) => {
    const old = prevByKey.get(e.key);
    if (old && entriesEqual(old, e)) {
      if (old !== prev.entries[i]) reusedAll = false; // order shifted
      return old;
    }
    reusedAll = false;
    return e;
  });

  if (reusedAll && shallowRecordEqual(prev.folderCounts, next.folderCounts)
      && shallowRecordEqual(prev.categoryCounts, next.categoryCounts)
      && shallowRecordEqual(prev.classCounts, next.classCounts)
      && prev.totals.named === next.totals.named
      && prev.totals.assets === next.totals.assets) {
    return prev; // nothing observable changed — keep the old reference
  }

  return { ...next, entries };
}

/** Compare only the registry-derived fields — everything else on a
 *  `CatalogEntry` is a deterministic function of the (constant) catalog. */
function entriesEqual(a: CatalogEntry, b: CatalogEntry): boolean {
  return (
    a.slug === b.slug &&
    a.notes === b.notes &&
    a.folder === b.folder &&
    a.poemText === b.poemText &&
    strArrEqual(a.tags, b.tags) &&
    strArrEqual(a.categories, b.categories) &&
    audioBankSlugsEqual(a.audioBank, b.audioBank)
  );
}

function strArrEqual(a: readonly string[] | undefined, b: readonly string[] | undefined): boolean {
  if (a === b) return true;
  if (!a || !b || a.length !== b.length) return false;
  for (let i = 0; i < a.length; i++) if (a[i] !== b[i]) return false;
  return true;
}

function audioBankSlugsEqual(
  a: CatalogEntry["audioBank"], b: CatalogEntry["audioBank"],
): boolean {
  if (a === b) return true;
  if (!a || !b) return a === b;
  const sa = a.samples || [], sb = b.samples || [];
  if (sa.length !== sb.length) return false;
  for (let i = 0; i < sa.length; i++) {
    if (sa[i]?.slug !== sb[i]?.slug) return false;
  }
  return true;
}

function shallowRecordEqual(a: Record<string, number>, b: Record<string, number>): boolean {
  if (a === b) return true;
  const ka = Object.keys(a), kb = Object.keys(b);
  if (ka.length !== kb.length) return false;
  for (const k of ka) if (a[k] !== b[k]) return false;
  return true;
}

/** Strip leading/trailing slashes, collapse repeats, drop empty strings. */
function normaliseFolder(raw: string | undefined | null): string | null {
  if (typeof raw !== "string") return null;
  const cleaned = raw.split("/").map(s => s.trim()).filter(Boolean).join("/");
  return cleaned.length > 0 ? cleaned : null;
}

/** ["menu", "menu/widgets", "menu/widgets/radio"] for input
 *  "menu/widgets/radio".  Empty array for falsy input. */
function folderAncestors(path: string): string[] {
  const parts = path.split("/");
  const out: string[] = [];
  for (let i = 1; i <= parts.length; i++) out.push(parts.slice(0, i).join("/"));
  return out;
}

// ---------------------------------------------------------------------------
// helpers
// ---------------------------------------------------------------------------

function categorize(entry: RawCatalogEntry, scriptReg: NonNullable<RawRegistry["scripts"]>): string[] {
  const cats = new Set<string>();
  const xrefs = (entry.xrefs || {}) as Xrefs;

  const seen = new Set<number>();
  for (const hit of xrefs.script || []) {
    const sid = hit.scriptId;
    if (typeof sid !== "number" || seen.has(sid)) continue;
    seen.add(sid);
    const slug = scriptReg[String(sid)]?.slug;
    cats.add(slug || `script/${sid}`);
  }

  const hasEngineRef = ["insn", "data", "ghidraText", "idaText"].some(
    k => Array.isArray((xrefs as Record<string, unknown>)[k]) && (xrefs as Record<string, unknown[]>)[k].length > 0,
  );
  if (hasEngineRef) cats.add("engine");

  if (entry.deadOnly)   cats.add("dead");
  if (entry.orphanOnly) cats.add("orphan");
  if (cats.size === 0)  cats.add("unattributed");

  return [...cats].sort();
}

function previewFileFor(entry: RawCatalogEntry, overlaySet: Set<string>): string | null {
  const rules = PREVIEW_RULES[entry.className];
  if (!rules) return null;

  // First, honour anything explicitly tracked in the catalog.
  for (const ext of rules) {
    for (const f of entry.files || []) {
      if (f.endsWith(ext) && overlaySet.has(f)) return f;
    }
  }
  // Then probe canonical names that the unpacker would have produced.
  for (const ext of rules) {
    const c = canonicalName(entry, ext);
    if (overlaySet.has(c)) return c;
  }
  return null;
}

function animHintFor(entry: RawCatalogEntry, overlaySet: Set<string>,
                     framesByStem: Map<string, string[]>): AnimHint {
  const cls = entry.className;
  if (cls === "BitmapSprite") {
    const gif  = canonicalName(entry, ".atlas.gif");
    const png  = canonicalName(entry, ".atlas.png");
    if (overlaySet.has(gif)) {
      return { kind: "gif", filename: gif, strip: overlaySet.has(png) ? png : null };
    }
  }
  if (cls === "BitmapJpegAnim") {
    const stem   = canonicalStem(entry);
    const frames = framesByStem.get(stem) || [];
    if (frames.length > 0) {
      const audio = `${stem}.audio.wav`;
      return {
        kind:      "frames",
        frames,
        audioFile: overlaySet.has(audio) ? audio : null,
      };
    }
  }
  return { kind: "none" };
}

function canonicalStem(entry: RawCatalogEntry): string {
  const id10 = String(entry.id).padStart(10, "0");
  return `res_${id10}_${entry.classId}_${entry.className}`;
}

function canonicalName(entry: RawCatalogEntry, suffix: string): string {
  return canonicalStem(entry) + suffix;
}

function collectAnimFrames(overlayFiles: string[]): Map<string, string[]> {
  // Map of <stem> -> ["<stem>.frame000.jpg", ...] sorted by frame index.
  const byStem = new Map<string, Array<{ idx: number; name: string }>>();
  for (const name of overlayFiles) {
    const m = ANIM_FRAME_RE.exec(name);
    if (!m) continue;
    const stem = name.slice(0, m.index);
    const idx  = Number(m[1]);
    let arr = byStem.get(stem);
    if (!arr) byStem.set(stem, arr = []);
    arr.push({ idx, name });
  }
  const out = new Map<string, string[]>();
  for (const [stem, entries] of byStem) {
    entries.sort((a, b) => a.idx - b.idx);
    out.set(stem, entries.map(e => e.name));
  }
  return out;
}

function mergeXrefs(a: Xrefs | undefined, b: Xrefs | undefined): Xrefs {
  const out: Record<string, unknown[]> = {};
  for (const src of [a, b]) {
    if (!src) continue;
    for (const [k, vs] of Object.entries(src)) {
      if (!Array.isArray(vs)) continue;
      const arr = (out[k] ||= []);
      arr.push(...vs);
    }
  }
  // Dedup by (file, line, offset, scriptId, snippet) tuple.
  for (const k of Object.keys(out)) {
    const seen = new Set<string>();
    out[k] = out[k].filter(v => {
      const x = v as Record<string, unknown>;
      const key = JSON.stringify([x.file, x.line, x.offset, x.scriptId, x.snippet]);
      if (seen.has(key)) return false;
      seen.add(key);
      return true;
    });
  }
  return out as Xrefs;
}

function sortByKey<T>(o: Record<string, T>): Record<string, T> {
  return Object.fromEntries(Object.entries(o).sort(([a], [b]) => a.localeCompare(b)));
}

async function readText(read: NonNullable<BuildOpts["readFile"]>, filename: string): Promise<string | null> {
  try {
    const bytes = await read(filename);
    if (!bytes) return null;
    return new TextDecoder("utf-8").decode(bytes);
  } catch {
    return null;
  }
}

async function readJson<T>(read: NonNullable<BuildOpts["readFile"]>, filename: string): Promise<T | null> {
  const txt = await readText(read, filename);
  if (txt == null) return null;
  try { return JSON.parse(txt) as T; } catch { return null; }
}

// (no public surface beyond `buildPayload`)
export type _internal_BuildOpts = BuildOpts;
