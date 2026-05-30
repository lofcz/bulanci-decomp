// Source autodetection.  Order of preference:
//
//   1. Same-origin HTTP at `./data/catalog/catalog.json` — works for
//      static deploys and the dev-server proxy.  This is the cheapest
//      path (single HEAD-ish probe) so we try it first.
//
//   2. Previously-picked FileSystem handle restored from IndexedDB,
//      provided the user re-grants permission.
//
//   3. Otherwise: the landing page prompts the user to either pick a
//      folder (browser FS Access API) or, for environments without
//      that API, point the app at a static base URL.

import type { DataSource } from "../DataSource";
import { HttpDataSource } from "./HttpDataSource";
import { FsAccessDataSource } from "./FsAccessDataSource";
import { loadPersistedHandle, ensurePermission } from "./persist";

export const DEFAULT_HTTP_BASE = "./data";

export interface AutodetectResult {
  source: DataSource | null;
  /** Why we returned null — surfaced in the landing UI. */
  reason: string | null;
}

export async function autodetectSource(): Promise<AutodetectResult> {
  // 1. Probe same-origin HTTP first.
  if (await probeHttp(DEFAULT_HTTP_BASE)) {
    const src = new HttpDataSource(DEFAULT_HTTP_BASE);
    // Best-effort writability probe — caps `info.writable` at the
    // backend's actual capability (Vite dev plugin: yes; static
    // `serve.py` or generic CDN: no).  Failure leaves the source
    // read-only.
    await src.probeWritable();
    return { source: src, reason: null };
  }

  // 2. Try the persisted FS handle.
  if (supportsFsAccess()) {
    const handle = await loadPersistedHandle();
    if (handle) {
      try {
        // Request readwrite up front so the editor toolbar can light
        // up without an extra prompt later.  Falls back to read-only
        // if the user declines.
        if (await ensurePermission(handle, "readwrite")) {
          return { source: new FsAccessDataSource(handle, /*writable*/ true), reason: null };
        }
        if (await ensurePermission(handle, "read")) {
          return { source: new FsAccessDataSource(handle, /*writable*/ false), reason: null };
        }
        return { source: null, reason: "Permission needed for the previously-opened folder." };
      } catch (err) {
        return { source: null, reason: `Stored folder unavailable: ${err}` };
      }
    }
  }

  return {
    source: null,
    reason: supportsFsAccess()
      ? "No data at `./data/catalog/catalog.json`. Pick the bulanci repository folder to continue."
      : "This browser doesn't support the File System Access API and `./data/` isn't reachable from this origin.",
  };
}

/** HEAD-style probe — using GET with `Range` to download only one byte
 *  so origins that don't allow HEAD still answer.  Treat any 2xx as
 *  success (some static hosts ignore the Range header and stream the
 *  whole file, which is fine for the catalog.json probe). */
async function probeHttp(base: string): Promise<boolean> {
  try {
    const r = await fetch(`${base}/catalog/catalog.json`, {
      method:  "GET",
      headers: { Range: "bytes=0-0" },
      cache:   "no-store",
    });
    return r.ok || r.status === 206;
  } catch {
    return false;
  }
}

export function supportsFsAccess(): boolean {
  return typeof window !== "undefined"
    && typeof (window as { showDirectoryPicker?: unknown }).showDirectoryPicker === "function";
}
