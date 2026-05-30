// `DataSource` abstracts where the gallery reads from.  Two backends:
//
//   * `HttpDataSource`  — fetches `./data/catalog/*` + `./data/overlay/*`
//                         (handy for static-hosted deploys, the existing
//                         Python serve.py, or `python -m http.server`).
//
//   * `FsAccessDataSource` — talks to a `FileSystemDirectoryHandle`
//                         that the user picked via `showDirectoryPicker`
//                         (browser-native, no server required, and the
//                         eventual asset-editor write path goes here).
//
// Both shapes return the same raw blobs.  The runtime payload merge
// (registry.json + font/poem inlining + sample-slug merging, etc.)
// happens once in `payload.ts` and is shared by both backends.

import type { RawCatalog, RawRegistry, RawCoverage } from "./raw";
import type { HandleManifest } from "./handleManifest";

/** One scene as declared by a pak's `pak.json` `scenes` table. The editor's
 *  scene browser is built from a flat list of these across every pak in the
 *  load-order profile, so a base scene (e.g. `main_menu` in `base_ui`) and a
 *  mod's override of it both surface, tagged by their owning pak. */
export interface SceneRef {
  /** Owning pak id (the load-order profile key). */
  pak: string;
  /** Scene name — the `scenes` table key the SceneManager loads by. */
  name: string;
  /** `"declarative"` (doc + scripts) or `"module"` (legacy single Luau). */
  kind: "declarative" | "module";
  /** Declarative control-tree doc path, relative to the pak folder. */
  doc?: string;
  /** Attached code-behind script paths, relative to the pak folder. */
  scripts: string[];
  /** Legacy single-module scene path, relative to the pak folder. */
  file?: string;
  /** True when this pak is a writable folder mod (edits persist here). */
  writable: boolean;
}

/** Information shown in the source-status indicator. */
export interface DataSourceInfo {
  /** Short human label, e.g. "Static (./data)" or "C:\path\to\bulanci". */
  label: string;
  /** "http" or "fs"; used to render a small icon + show whether the
   *  picker should still be offered. */
  kind: "http" | "fs";
  /** True if this source is allowed to write (FS handle granted
   *  read-write permission).  Editor features check this. */
  writable: boolean;
}

export interface DataSource {
  readonly info: DataSourceInfo;

  // ---- catalog manifest files ------------------------------------------
  loadCatalog():  Promise<RawCatalog>;
  loadRegistry(): Promise<RawRegistry>;
  loadCoverage(): Promise<RawCoverage>;

  /** The codegen's fat handle table (`open_bulanci/assets/manifest.json`):
   *  `{ "<hash_hex>": { class, folder, slug, source } }`.  Optional —
   *  sources that can't reach it (or where it's absent) resolve to
   *  `null`, and the editor falls back to client-side `handleHash`.
   *  Used by the live engine bridge to map an asset to its push
   *  target hash(es). */
  loadManifest?(): Promise<HandleManifest | null>;

  /** Flat list of every filename inside `unpacked/overlay/`.  Used by
   *  `payload.ts` to enumerate BitmapJpegAnim frames + BitmapSprite
   *  atlas extras that the catalog itself doesn't track. */
  listOverlayFiles(): Promise<string[]>;

  // ---- overlay resources ----------------------------------------------
  /** Synchronous fast path — return a stable URL if one is already
   *  known.  HTTP sources can always answer (the URL is just a path
   *  concat); FS sources return `null` on the first call for a given
   *  file, then the cached blob URL on every subsequent call.  Used
   *  by the React hooks so HTTP-backed runs don't flash empty
   *  components on first paint. */
  overlayUrlSync?(filename: string): string | null;

  /** Return a URL usable in `<img src>`, `<audio src>`, `<a href>` for
   *  an overlay file.  For FS sources this is a `blob:` URL created on
   *  demand; the source owns the URL and revokes it on `dispose()`.
   *  HTTP sources return a stable same-origin URL. */
  overlayUrl(filename: string): Promise<string>;

  /** Raw bytes for client-side processing (WAV slicing, font sidecar
   *  parsing, etc.).  Implementations should cache to avoid re-reading
   *  on every consumer. */
  overlayBytes(filename: string): Promise<Uint8Array>;

  /** Release any cached blob URLs / file handles.  Called when the
   *  user picks a different source. */
  dispose?(): void;

  // ---- writes (asset editor) ------------------------------------------
  //
  // Sources that report `info.writable === true` MUST implement
  // `writeRegistry`.  The contract is:
  //
  //   * `text` is the full new contents of `registry.json` (already
  //     serialised — caller does the JSON.parse / mutate / stringify
  //     dance in `data/registryEdits.ts`).
  //   * Implementations must be atomic from the caller's point of view:
  //     either the new bytes are durable or the on-disk file is left
  //     untouched.  Throw on failure.
  //   * After resolving, the next `loadRegistry()` call should observe
  //     the new contents.  Internal caches should be invalidated.
  //
  // Read-only sources can leave `writeRegistry` undefined; the editor
  // UI in App.tsx checks `info.writable` before exposing controls.
  writeRegistry?(text: string): Promise<void>;

  // ---- scenes (scene editor) ------------------------------------------
  //
  // Scene discovery + file IO for the declarative scene designer + the
  // Luau script panel. A scene is a `*.scene.json` control tree plus
  // attached scripts, both living inside a pak folder and addressed by
  // their path relative to that folder (e.g. `scenes/main_menu.scene.json`,
  // `scenes/main_menu/dials.luau`). The same relative path in a
  // higher-priority mod pak overlays the base one at runtime.

  /** Enumerate every scene declared across the load-order profile's paks.
   *  Sources that can't reach the pak tree resolve to `[]`. */
  listScenes?(): Promise<SceneRef[]>;

  /** Enumerate every `.luau` script inside a pak (recursively), as
   *  pak-relative paths (e.g. `lib/startgame1.luau`,
   *  `scenes/main_menu/boot.luau`). Lets the Scripts panel browse + edit
   *  any pak script, not just a scene's declared code-behind. Sources that
   *  can't reach the pak tree resolve to `[]`. */
  listPakScripts?(pak: string): Promise<string[]>;

  /** Read a pak-relative text file (a scene doc or a script). Throws if
   *  the file is missing. */
  readPakText?(pak: string, rel: string): Promise<string>;

  /** Write a pak-relative text file (scene doc or script), creating parent
   *  folders as needed. Only sources with `info.writable` implement this;
   *  the contract matches `writeRegistry` (atomic, durable, throws on
   *  failure). */
  writePakText?(pak: string, rel: string, text: string): Promise<void>;
}
