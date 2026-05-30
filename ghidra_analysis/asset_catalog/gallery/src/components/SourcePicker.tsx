// Landing screen that appears when there's no auto-detected data
// source.  Two entry points: pick a local folder (browser-native FS
// Access API) or point the app at a static base URL.

import { useState } from "react";
import { FolderOpen, Globe, AlertTriangle } from "lucide-react";
import type { DataSource } from "@/data/DataSource";
import { HttpDataSource } from "@/data/sources/HttpDataSource";
import { FsAccessDataSource } from "@/data/sources/FsAccessDataSource";
import { DEFAULT_HTTP_BASE, supportsFsAccess } from "@/data/sources/detect";
import { savePersistedHandle, ensurePermission } from "@/data/sources/persist";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";

interface Props {
  reason: string | null;
  onSourceReady: (source: DataSource) => void;
}

export function SourcePicker({ reason, onSourceReady }: Props) {
  const [baseUrl, setBaseUrl] = useState(DEFAULT_HTTP_BASE);
  const [error, setError] = useState<string | null>(null);
  const fsSupported = supportsFsAccess();

  async function pickFolder() {
    setError(null);
    try {
      // The "id" parameter persists per-origin so the OS remembers
      // where the user last picked.
      const picker = (window as unknown as {
        showDirectoryPicker: (opts: { id?: string; mode?: "read" | "readwrite" }) => Promise<FileSystemDirectoryHandle>;
      }).showDirectoryPicker;
      const handle  = await picker({ id: "bulanciRoot", mode: "readwrite" });

      // Sanity check: does it look like the repo root?
      const looksLikeRoot = await probeRepoRoot(handle);
      if (!looksLikeRoot) {
        setError("Pick the bulanci repository root (the folder that contains `ghidra_analysis/` and `unpacked/`).");
        return;
      }

      await ensurePermission(handle, "readwrite").catch(() => undefined);
      await savePersistedHandle(handle);
      onSourceReady(new FsAccessDataSource(handle, true));
    } catch (err) {
      if (err instanceof DOMException && err.name === "AbortError") return;
      setError(String(err));
    }
  }

  async function useStatic() {
    setError(null);
    try {
      const r = await fetch(`${baseUrl}/catalog/catalog.json`, { method: "GET", cache: "no-store" });
      if (!r.ok) {
        setError(`Couldn't reach ${baseUrl}/catalog/catalog.json (HTTP ${r.status}).`);
        return;
      }
      onSourceReady(new HttpDataSource(baseUrl));
    } catch (err) {
      setError(String(err));
    }
  }

  return (
    <div className="flex h-screen items-center justify-center p-8">
      <div className="w-full max-w-xl space-y-6 rounded-xl border border-border bg-card p-8 shadow-xl">
        <div>
          <h1 className="text-2xl font-semibold">Bulanci Asset Catalog</h1>
          <p className="mt-1 text-sm text-muted-foreground">
            The gallery needs to know where the catalog + unpacked assets live.
          </p>
        </div>

        {reason && (
          <div className="flex items-start gap-2 rounded-md border border-amber-400/40 bg-amber-400/10 p-3 text-xs text-amber-200">
            <AlertTriangle className="size-4 shrink-0 mt-0.5" />
            <span>{reason}</span>
          </div>
        )}

        {error && (
          <div className="rounded-md border border-destructive/40 bg-destructive/10 p-3 text-xs text-destructive-foreground">
            {error}
          </div>
        )}

        <section className="space-y-2">
          <h2 className="text-sm font-semibold uppercase tracking-wider text-muted-foreground">
            Open a local repository
          </h2>
          <p className="text-xs text-muted-foreground">
            Pick the <span className="font-mono">bulanci</span> folder (the one with
            <span className="font-mono"> ghidra_analysis/</span> and
            <span className="font-mono"> unpacked/</span>).
            Your browser will remember the choice; permission has to be re-granted
            after each restart for security.
          </p>
          <Button
            onClick={pickFolder}
            disabled={!fsSupported}
            className="w-full justify-start gap-2"
          >
            <FolderOpen className="size-4" />
            {fsSupported ? "Pick repository folder…" : "FS Access API not supported in this browser"}
          </Button>
        </section>

        <section className="space-y-2 border-t border-border pt-6">
          <h2 className="text-sm font-semibold uppercase tracking-wider text-muted-foreground">
            Or load from a static URL
          </h2>
          <p className="text-xs text-muted-foreground">
            The catalog can also be hosted as plain files at
            <span className="font-mono"> &lt;base&gt;/catalog/catalog.json</span> and
            <span className="font-mono"> &lt;base&gt;/overlay/…</span>
            — useful for read-only static deploys.
          </p>
          <div className="flex gap-2">
            <Input
              value={baseUrl}
              onChange={ev => setBaseUrl(ev.target.value)}
              placeholder="./data or https://example.com/bulanci"
              className="font-mono text-xs"
            />
            <Button variant="outline" onClick={useStatic} className="gap-2">
              <Globe className="size-4" />
              Load
            </Button>
          </div>
        </section>
      </div>
    </div>
  );
}

/** Cheap sanity check — confirm we got the right folder by trying to
 *  reach `ghidra_analysis/asset_catalog/catalog.json`. */
async function probeRepoRoot(root: FileSystemDirectoryHandle): Promise<boolean> {
  try {
    const a = await root.getDirectoryHandle("ghidra_analysis");
    const b = await a.getDirectoryHandle("asset_catalog");
    await b.getFileHandle("catalog.json");
    return true;
  } catch {
    return false;
  }
}
