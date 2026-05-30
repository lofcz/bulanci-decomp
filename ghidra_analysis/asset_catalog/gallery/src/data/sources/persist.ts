// Persist the last-used FileSystemDirectoryHandle in IndexedDB so the
// app remembers the picked folder across sessions.  Permission still
// has to be re-granted on each load (browser security), but the user
// avoids the "navigate to folder" dialog every time.

import { get, set, del } from "idb-keyval";

const KEY = "bulanci.catalog.rootHandle";

export async function loadPersistedHandle(): Promise<FileSystemDirectoryHandle | null> {
  try {
    const handle = await get<FileSystemDirectoryHandle>(KEY);
    return handle ?? null;
  } catch {
    return null;
  }
}

export async function savePersistedHandle(handle: FileSystemDirectoryHandle): Promise<void> {
  try { await set(KEY, handle); } catch { /* ignore quota / private-mode errors */ }
}

export async function clearPersistedHandle(): Promise<void> {
  try { await del(KEY); } catch { /* ignore */ }
}

/** Ask the browser to (re-)grant `read` (and optionally `readwrite`)
 *  permission for a previously stored handle.  Returns true on grant. */
export async function ensurePermission(handle: FileSystemDirectoryHandle,
                                       mode: "read" | "readwrite" = "read"): Promise<boolean> {
  const opts = { mode } as const;
  // The TS lib types still flag these as nullable / experimental — cast
  // to a narrow ad-hoc interface to keep things clean.
  const h = handle as unknown as {
    queryPermission(o: typeof opts):   Promise<PermissionState>;
    requestPermission(o: typeof opts): Promise<PermissionState>;
  };
  if ((await h.queryPermission(opts)) === "granted") return true;
  return (await h.requestPermission(opts)) === "granted";
}
