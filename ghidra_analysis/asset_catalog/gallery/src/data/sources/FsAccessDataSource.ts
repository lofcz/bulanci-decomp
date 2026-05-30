// File System Access API backend.  The user picks the *repository root*
// (the folder that contains both `ghidra_analysis/` and `unpacked/`);
// we then walk to:
//
//   <root>/ghidra_analysis/asset_catalog/{catalog.json, registry.json, coverage.json}
//   <root>/unpacked/overlay/<filename>
//
// We aggressively cache resolved directory handles + decoded file
// blobs.  Blob URLs are created on demand and revoked on `dispose()`
// — components that render `<img src>` keep the URL stable so the
// browser cache the actual image.

import type { DataSource, DataSourceInfo, SceneRef } from "../DataSource";
import type { RawCatalog, RawRegistry, RawCoverage } from "../raw";
import type { HandleManifest } from "../handleManifest";

export class FsAccessDataSource implements DataSource {
  readonly info: DataSourceInfo;
  private overlayDirP: Promise<FileSystemDirectoryHandle> | null = null;
  private catalogDirP: Promise<FileSystemDirectoryHandle> | null = null;
  private paksDirP: Promise<FileSystemDirectoryHandle> | null = null;
  private overlayBytesCache = new Map<string, Promise<Uint8Array>>();
  private overlayUrlCache   = new Map<string, string>();
  private overlayListCache:  Promise<string[]> | null = null;

  constructor(private root: FileSystemDirectoryHandle, writable = false) {
    this.info = {
      label:    root.name,
      kind:     "fs",
      writable,
    };
  }

  // -----------------------------------------------------------------
  // path resolution
  // -----------------------------------------------------------------

  private async catalogDir(): Promise<FileSystemDirectoryHandle> {
    if (!this.catalogDirP) {
      this.catalogDirP = (async () => {
        const ghidra  = await this.root.getDirectoryHandle("ghidra_analysis");
        return ghidra.getDirectoryHandle("asset_catalog");
      })();
    }
    return this.catalogDirP;
  }

  private async overlayDir(): Promise<FileSystemDirectoryHandle> {
    if (!this.overlayDirP) {
      this.overlayDirP = (async () => {
        const unpacked = await this.root.getDirectoryHandle("unpacked");
        return unpacked.getDirectoryHandle("overlay");
      })();
    }
    return this.overlayDirP;
  }

  private async paksDir(): Promise<FileSystemDirectoryHandle> {
    if (!this.paksDirP) {
      this.paksDirP = (async () => {
        const ob = await this.root.getDirectoryHandle("open_bulanci");
        return ob.getDirectoryHandle("paks");
      })();
    }
    return this.paksDirP;
  }

  /** Resolve a `<pakFolder>/<rel>` chain to its file handle. When
   *  `create` is set, intermediate dirs and the leaf are created. */
  private async pakFileHandle(
    folder: string, rel: string, create: boolean,
  ): Promise<FileSystemFileHandle> {
    let dir = await (await this.paksDir()).getDirectoryHandle(folder, { create });
    const parts = rel.split("/").filter(Boolean);
    const leaf = parts.pop();
    if (!leaf) throw new Error(`bad pak path: ${rel}`);
    for (const seg of parts) {
      dir = await dir.getDirectoryHandle(seg, { create });
    }
    return dir.getFileHandle(leaf, { create });
  }

  // -----------------------------------------------------------------
  // small helpers
  // -----------------------------------------------------------------

  private async readJsonFile<T>(dir: FileSystemDirectoryHandle, name: string,
                                fallback: T | null = null): Promise<T> {
    try {
      const fh   = await dir.getFileHandle(name);
      const file = await fh.getFile();
      const txt  = await file.text();
      return JSON.parse(txt) as T;
    } catch (err) {
      if (fallback !== null && err instanceof DOMException && err.name === "NotFoundError") {
        return fallback;
      }
      throw err;
    }
  }

  // -----------------------------------------------------------------
  // DataSource impl
  // -----------------------------------------------------------------

  async loadCatalog(): Promise<RawCatalog> {
    return this.readJsonFile<RawCatalog>(await this.catalogDir(), "catalog.json");
  }

  async loadRegistry(): Promise<RawRegistry> {
    return this.readJsonFile<RawRegistry>(await this.catalogDir(), "registry.json", {});
  }

  async loadCoverage(): Promise<RawCoverage> {
    return this.readJsonFile<RawCoverage>(await this.catalogDir(), "coverage.json", {});
  }

  async loadManifest(): Promise<HandleManifest | null> {
    // Lives at <root>/open_bulanci/assets/manifest.json — emitted by
    // build_typed_handles.py.  Absent on older checkouts, so degrade
    // to null rather than failing the whole boot.
    try {
      const ob     = await this.root.getDirectoryHandle("open_bulanci");
      const assets = await ob.getDirectoryHandle("assets");
      return this.readJsonFile<HandleManifest>(assets, "manifest.json", {});
    } catch {
      return null;
    }
  }

  async listOverlayFiles(): Promise<string[]> {
    if (this.overlayListCache) return this.overlayListCache;
    this.overlayListCache = (async () => {
      const dir = await this.overlayDir();
      const names: string[] = [];
      // The overlay directory is flat — no recursion required.  Use
      // `values()` so TypeScript is happy in browsers where the lib
      // typings only list the (string, handle) tuple variant.
      const iter = (dir as unknown as {
        values(): AsyncIterableIterator<FileSystemHandle>;
      }).values();
      for await (const entry of iter) {
        if (entry.kind === "file") names.push(entry.name);
      }
      return names;
    })();
    return this.overlayListCache;
  }

  overlayUrlSync(filename: string): string | null {
    return this.overlayUrlCache.get(filename) ?? null;
  }

  async overlayUrl(filename: string): Promise<string> {
    const hit = this.overlayUrlCache.get(filename);
    if (hit) return hit;
    const bytes = await this.overlayBytes(filename);
    // Mime-sniff by extension — keeps `<img>` and `<audio>` happy.
    const mime = guessMime(filename);
    const url  = URL.createObjectURL(new Blob([bytes as BlobPart], { type: mime }));
    this.overlayUrlCache.set(filename, url);
    return url;
  }

  async overlayBytes(filename: string): Promise<Uint8Array> {
    let hit = this.overlayBytesCache.get(filename);
    if (!hit) {
      hit = (async () => {
        const dir   = await this.overlayDir();
        const fh    = await dir.getFileHandle(filename);
        const file  = await fh.getFile();
        const buf   = await file.arrayBuffer();
        return new Uint8Array(buf);
      })();
      this.overlayBytesCache.set(filename, hit);
    }
    return hit;
  }

  async writeRegistry(text: string): Promise<void> {
    if (!this.info.writable) {
      throw new Error("data source granted read-only access — re-pick the folder and allow writes");
    }
    const dir = await this.catalogDir();
    // `create: true` upserts — registry.json always exists today but
    // future tooling may decide to seed an empty one.
    const fh = await dir.getFileHandle("registry.json", { create: true });
    // `FileSystemFileHandle.createWritable` is the FS Access API write
    // entry point; closing the stream flushes to disk.  Cast to `any`
    // because TypeScript's DOM lib only added this type recently and
    // some browsers still ship the older typings.
    const writable = await (fh as unknown as {
      createWritable(opts?: { keepExistingData?: boolean }): Promise<FileSystemWritableFileStream>;
    }).createWritable({ keepExistingData: false });
    try {
      await writable.write(text);
    } finally {
      await writable.close();
    }
  }

  // -----------------------------------------------------------------
  // scenes
  // -----------------------------------------------------------------

  async listScenes(): Promise<SceneRef[]> {
    let paks: FileSystemDirectoryHandle;
    try {
      paks = await this.paksDir();
    } catch {
      return [];
    }
    // The load-order profile lists every pak; folder mods carry scenes.
    const profile = await this.readJsonFile<{ paks?: PakProfileEntry[] }>(paks, "profile.json", { paks: [] });
    const out: SceneRef[] = [];
    for (const entry of profile.paks ?? []) {
      if (!entry?.id || entry.kind === "master") continue; // master ships embedded
      const folder = entry.path ?? entry.id;
      let dir: FileSystemDirectoryHandle;
      try {
        dir = await paks.getDirectoryHandle(folder);
      } catch {
        continue;
      }
      const manifest = await this.readJsonFile<PakManifest>(dir, "pak.json", {});
      for (const [name, scene] of Object.entries(manifest.scenes ?? {})) {
        out.push(toSceneRef(entry.id, name, scene, this.info.writable));
      }
    }
    return out;
  }

  async listPakScripts(pak: string): Promise<string[]> {
    let folder: string;
    let dir: FileSystemDirectoryHandle;
    try {
      folder = await this.folderForPak(pak);
      dir = await (await this.paksDir()).getDirectoryHandle(folder);
    } catch {
      return [];
    }
    const out: string[] = [];
    const walk = async (d: FileSystemDirectoryHandle, prefix: string): Promise<void> => {
      const iter = (d as unknown as {
        values(): AsyncIterableIterator<FileSystemHandle>;
      }).values();
      for await (const entry of iter) {
        const rel = prefix ? `${prefix}/${entry.name}` : entry.name;
        if (entry.kind === "directory") {
          await walk(entry as FileSystemDirectoryHandle, rel);
        } else if (entry.name.endsWith(".luau")) {
          out.push(rel);
        }
      }
    };
    await walk(dir, "");
    out.sort();
    return out;
  }

  async readPakText(pak: string, rel: string): Promise<string> {
    const folder = await this.folderForPak(pak);
    const fh = await this.pakFileHandle(folder, rel, false);
    return (await fh.getFile()).text();
  }

  async writePakText(pak: string, rel: string, text: string): Promise<void> {
    if (!this.info.writable) {
      throw new Error("data source granted read-only access — re-pick the folder and allow writes");
    }
    const folder = await this.folderForPak(pak);
    const fh = await this.pakFileHandle(folder, rel, true);
    const writable = await (fh as unknown as {
      createWritable(opts?: { keepExistingData?: boolean }): Promise<FileSystemWritableFileStream>;
    }).createWritable({ keepExistingData: false });
    try {
      await writable.write(text);
    } finally {
      await writable.close();
    }
  }

  /** Map a pak id to its on-disk folder name via the profile (defaults to
   *  the id when no explicit `path` is set). */
  private async folderForPak(pak: string): Promise<string> {
    const paks = await this.paksDir();
    const profile = await this.readJsonFile<{ paks?: PakProfileEntry[] }>(paks, "profile.json", { paks: [] });
    const entry = (profile.paks ?? []).find(p => p.id === pak);
    return entry?.path ?? pak;
  }

  dispose(): void {
    for (const url of this.overlayUrlCache.values()) URL.revokeObjectURL(url);
    this.overlayUrlCache.clear();
    this.overlayBytesCache.clear();
    this.overlayListCache = null;
  }
}

interface PakProfileEntry { id: string; kind?: string; path?: string; enabled?: boolean }
interface PakSceneEntry { file?: string; doc?: string; scripts?: string[] }
interface PakManifest { scenes?: Record<string, PakSceneEntry> }

function toSceneRef(pak: string, name: string, scene: PakSceneEntry, writable: boolean): SceneRef {
  return {
    pak,
    name,
    kind: scene.doc ? "declarative" : "module",
    doc: scene.doc,
    scripts: scene.scripts ?? [],
    file: scene.file,
    writable,
  };
}

function guessMime(filename: string): string {
  const lower = filename.toLowerCase();
  if (lower.endsWith(".png"))  return "image/png";
  if (lower.endsWith(".gif"))  return "image/gif";
  if (lower.endsWith(".jpg") || lower.endsWith(".jpeg")) return "image/jpeg";
  if (lower.endsWith(".wav"))  return "audio/wav";
  if (lower.endsWith(".mp3"))  return "audio/mpeg";
  if (lower.endsWith(".json")) return "application/json";
  if (lower.endsWith(".txt"))  return "text/plain; charset=utf-8";
  if (lower.endsWith(".lua") || lower.endsWith(".asm")) return "text/plain; charset=utf-8";
  return "application/octet-stream";
}
