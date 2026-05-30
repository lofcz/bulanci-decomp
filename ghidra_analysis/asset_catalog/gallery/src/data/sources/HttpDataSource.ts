// Fetches everything from a static URL tree, expected to be laid out as:
//
//   <baseUrl>/
//       catalog/
//           catalog.json
//           registry.json
//           coverage.json
//           overlay-index.json    (optional — flat list of overlay files)
//       overlay/
//           res_*.bin, *.png, *.json, ...
//
// The two existing serving paths both satisfy this layout:
//   * `serve.py` (in static-shim mode — see the slim rewrite) maps
//     `/data/catalog/*` to `ghidra_analysis/asset_catalog/*` and
//     `/data/overlay/*` to `unpacked/overlay/*`.
//   * `bun run dev` (Vite) proxies the same prefixes (see `vite.config.ts`).
//   * For an actual static deploy, drop those folders into `dist/data/`.

import type { DataSource, DataSourceInfo, SceneRef } from "../DataSource";
import type { RawCatalog, RawRegistry, RawCoverage } from "../raw";
import type { HandleManifest } from "../handleManifest";

export class HttpDataSource implements DataSource {
  readonly info: DataSourceInfo;
  private overlayBytesCache = new Map<string, Promise<Uint8Array>>();
  private overlayListCache: Promise<string[]> | null = null;

  constructor(private baseUrl: string = "./data") {
    // Strip trailing slashes once so we can always concatenate with "/".
    this.baseUrl = baseUrl.replace(/\/+$/, "");
    this.info = {
      label:    this.baseUrl === "./data" ? "Static (./data)" : this.baseUrl,
      kind:     "http",
      // `writable` flips once `probeWritable()` resolves — start
      // false so a static deploy never accidentally exposes the
      // editor toolbar before the probe completes.
      writable: false,
    };
  }

  /** Ask the backing server whether it accepts writes.  The Vite dev
   *  plugin answers `200 OK` at `/data/api/writable`; the slim
   *  `serve.py` static shim 404s, leaving the source read-only. */
  async probeWritable(): Promise<boolean> {
    try {
      const r = await fetch(`${this.baseUrl}/api/writable`, { cache: "no-store" });
      if (r.ok) {
        this.info.writable = true;
        return true;
      }
    } catch { /* network error → read-only */ }
    return false;
  }

  async loadCatalog(): Promise<RawCatalog> {
    const r = await fetch(`${this.baseUrl}/catalog/catalog.json`, { cache: "no-store" });
    if (!r.ok) throw new Error(`catalog.json: HTTP ${r.status}`);
    return await r.json();
  }

  async loadRegistry(): Promise<RawRegistry> {
    const r = await fetch(`${this.baseUrl}/catalog/registry.json`, { cache: "no-store" });
    if (!r.ok) {
      // Registry is optional — gallery still works with empty names.
      if (r.status === 404) return {};
      throw new Error(`registry.json: HTTP ${r.status}`);
    }
    return await r.json() as RawRegistry;
  }

  async loadCoverage(): Promise<RawCoverage> {
    const r = await fetch(`${this.baseUrl}/catalog/coverage.json`, { cache: "no-store" });
    if (!r.ok) {
      if (r.status === 404) return {};
      throw new Error(`coverage.json: HTTP ${r.status}`);
    }
    return await r.json();
  }

  async loadManifest(): Promise<HandleManifest | null> {
    // Optional — only present when the server maps it (the Vite dev
    // plugin and serve.py expose `catalog/manifest.json`).  Missing on
    // a bare static deploy, so 404 → null (editor falls back to
    // client-side handleHash).
    try {
      const r = await fetch(`${this.baseUrl}/catalog/manifest.json`, { cache: "no-store" });
      if (!r.ok) return null;
      return await r.json() as HandleManifest;
    } catch {
      return null;
    }
  }

  async listOverlayFiles(): Promise<string[]> {
    if (this.overlayListCache) return this.overlayListCache;
    // The index file is optional.  If absent, we degrade gracefully:
    // BitmapJpegAnim frame-enumeration will only see frame000.jpg
    // (whatever the catalog tracks) and other anims won't work.
    this.overlayListCache = (async () => {
      const r = await fetch(`${this.baseUrl}/catalog/overlay-index.json`, { cache: "no-store" });
      if (!r.ok) {
        console.warn("[HttpDataSource] overlay-index.json missing — JpegAnim frame enumeration will be limited");
        return [];
      }
      return await r.json();
    })();
    return this.overlayListCache;
  }

  overlayUrlSync(filename: string): string {
    return `${this.baseUrl}/overlay/${encodeURIComponent(filename)}`;
  }

  async overlayUrl(filename: string): Promise<string> {
    return this.overlayUrlSync(filename);
  }

  async overlayBytes(filename: string): Promise<Uint8Array> {
    let hit = this.overlayBytesCache.get(filename);
    if (!hit) {
      hit = (async () => {
        const r = await fetch(`${this.baseUrl}/overlay/${encodeURIComponent(filename)}`);
        if (!r.ok) throw new Error(`${filename}: HTTP ${r.status}`);
        return new Uint8Array(await r.arrayBuffer());
      })();
      this.overlayBytesCache.set(filename, hit);
    }
    return hit;
  }

  async writeRegistry(text: string): Promise<void> {
    if (!this.info.writable) throw new Error("data source is not writable");
    const r = await fetch(`${this.baseUrl}/catalog/registry.json`, {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body:    text,
    });
    if (!r.ok) throw new Error(`registry.json PUT failed: HTTP ${r.status}`);
    // No payload cache lives in here, but a re-read should produce the
    // new bytes — `cache: "no-store"` on `loadRegistry` already ensures
    // that.  Caller (App.tsx) refreshes the merged catalog after each
    // successful write.
  }

  // ---- scenes ----------------------------------------------------------

  async listScenes(): Promise<SceneRef[]> {
    try {
      const r = await fetch(`${this.baseUrl}/scenes`, { cache: "no-store" });
      if (!r.ok) return [];
      const body = await r.json() as { scenes?: SceneRef[] };
      return body.scenes ?? [];
    } catch {
      return [];
    }
  }

  async listPakScripts(pak: string): Promise<string[]> {
    try {
      const r = await fetch(`${this.baseUrl}/pak-scripts/${encodeURIComponent(pak)}`, { cache: "no-store" });
      if (!r.ok) return [];
      const body = await r.json() as { scripts?: string[] };
      return body.scripts ?? [];
    } catch {
      return [];
    }
  }

  async readPakText(pak: string, rel: string): Promise<string> {
    const r = await fetch(this.pakFileUrl(pak, rel), { cache: "no-store" });
    if (!r.ok) throw new Error(`${pak}/${rel}: HTTP ${r.status}`);
    return await r.text();
  }

  async writePakText(pak: string, rel: string, text: string): Promise<void> {
    if (!this.info.writable) throw new Error("data source is not writable");
    const r = await fetch(this.pakFileUrl(pak, rel), {
      method: "PUT",
      headers: { "Content-Type": "text/plain; charset=utf-8" },
      body: text,
    });
    if (!r.ok) throw new Error(`${pak}/${rel} PUT failed: HTTP ${r.status}`);
  }

  /** `/data/pak/<pak>/<rel>` with each path segment encoded but the
   *  separators preserved (so `scenes/main_menu/dials.luau` survives). */
  private pakFileUrl(pak: string, rel: string): string {
    const safeRel = rel.split("/").map(encodeURIComponent).join("/");
    return `${this.baseUrl}/pak/${encodeURIComponent(pak)}/${safeRel}`;
  }

  dispose(): void {
    this.overlayBytesCache.clear();
    this.overlayListCache = null;
  }
}
