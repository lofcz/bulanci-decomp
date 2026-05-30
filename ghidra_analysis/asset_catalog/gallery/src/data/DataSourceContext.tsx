// React glue around the `DataSource` interface — exposes the current
// source to every component without prop-drilling, plus a couple of
// convenience hooks (`useOverlayUrl`, `useBankSampleUrl`).

import {
  createContext, useContext, useEffect, useMemo, useRef, useState,
} from "react";
import type { DataSource } from "./DataSource";
import { sliceWavSample } from "./wav";

interface Ctx {
  source: DataSource;
}

const DataSourceCtx = createContext<Ctx | null>(null);

export function DataSourceProvider({ source, children }: { source: DataSource; children: React.ReactNode }) {
  const value = useMemo<Ctx>(() => ({ source }), [source]);
  return <DataSourceCtx.Provider value={value}>{children}</DataSourceCtx.Provider>;
}

export function useDataSource(): DataSource {
  const ctx = useContext(DataSourceCtx);
  if (!ctx) throw new Error("useDataSource must be inside <DataSourceProvider>");
  return ctx.source;
}

// ---------------------------------------------------------------------------
// hooks
// ---------------------------------------------------------------------------

/** Resolve a single overlay filename to a URL.  Returns `null` until
 *  the URL is available (FS source has to read the file first, HTTP
 *  source resolves synchronously but we still pass through Promises to
 *  keep the API uniform). */
export function useOverlayUrl(filename: string | null | undefined): string | null {
  const source = useDataSource();
  // Sync fast path first — HTTP sources always answer, FS sources
  // answer only on cache hit; either way no first-paint flash.
  const initial = filename ? (source.overlayUrlSync?.(filename) ?? null) : null;
  const [url, setUrl] = useState<string | null>(initial);

  useEffect(() => {
    if (!filename) { setUrl(null); return; }
    const sync = source.overlayUrlSync?.(filename) ?? null;
    if (sync) { setUrl(sync); return; }
    let cancelled = false;
    source.overlayUrl(filename).then(u => {
      if (!cancelled) setUrl(u);
    }).catch(() => {
      if (!cancelled) setUrl(null);
    });
    return () => { cancelled = true; };
  }, [source, filename]);

  return url;
}

/** Resolve a list of overlay filenames in parallel.  Useful for
 *  BitmapJpegAnim where the player wants every frame's URL up-front. */
export function useOverlayUrls(filenames: string[] | null | undefined): string[] {
  const source = useDataSource();
  const key = useMemo(() => filenames?.join("|") ?? "", [filenames]);

  // Sync fast path — collect any URLs we can already answer.  Falls
  // back to async fetch for the rest.
  const initial = useMemo(() => {
    if (!filenames || !source.overlayUrlSync) return [];
    return filenames.map(f => source.overlayUrlSync!(f) ?? "");
  }, [source, key]); // eslint-disable-line react-hooks/exhaustive-deps
  const [urls, setUrls] = useState<string[]>(initial);

  useEffect(() => {
    if (!filenames || filenames.length === 0) { setUrls([]); return; }
    let cancelled = false;
    Promise.all(filenames.map(f => source.overlayUrl(f))).then(arr => {
      if (!cancelled) setUrls(arr);
    }).catch(() => {
      if (!cancelled) setUrls([]);
    });
    return () => { cancelled = true; };
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [source, key]);

  return urls;
}

/** Slice an audio bank in-browser and expose the sub-WAV as a blob URL.
 *  The bank bytes are cached at the DataSource level, so siblings
 *  share the parse cost. */
export function useBankSampleUrl(bankFilename: string | null | undefined,
                                 offsetInBank: number,
                                 byteLen: number): string | null {
  const source = useDataSource();
  const [url, setUrl] = useState<string | null>(null);
  const ownedRef = useRef<string | null>(null);

  useEffect(() => {
    if (!bankFilename || byteLen <= 0) { setUrl(null); return; }
    let cancelled = false;
    (async () => {
      try {
        const bankBytes = await source.overlayBytes(bankFilename);
        const slice     = sliceWavSample(bankBytes, offsetInBank, byteLen);
        const blob      = new Blob([slice as BlobPart], { type: "audio/wav" });
        const objectUrl = URL.createObjectURL(blob);
        if (cancelled) { URL.revokeObjectURL(objectUrl); return; }
        ownedRef.current = objectUrl;
        setUrl(objectUrl);
      } catch (err) {
        if (!cancelled) {
          console.warn("[useBankSampleUrl]", bankFilename, offsetInBank, byteLen, err);
          setUrl(null);
        }
      }
    })();
    return () => {
      cancelled = true;
      // Revoke when the consumer (e.g. <audio>) unmounts or asks for
      // a different slice.  The DataSource keeps the bank bytes in
      // its own cache so re-slicing is cheap.
      if (ownedRef.current) {
        URL.revokeObjectURL(ownedRef.current);
        ownedRef.current = null;
      }
    };
  }, [source, bankFilename, offsetInBank, byteLen]);

  return url;
}
