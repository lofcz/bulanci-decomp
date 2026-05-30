// Live engine preview panel.
//
// Embeds the REAL game — the existing `open_bulanci` wasm build — in an iframe,
// same-origin with the broker. The editor owns the engine's whole lifecycle:
// the `bulanciWebEngine` Vite plugin builds the wasm on startup when stale and
// rebuilds it on any Rust change, deploying into `open_bulanci/web/` (served at
// `/play/`). This panel polls `/play/__status` for build progress and reloads
// the iframe when the build `generation` bumps — so editing Rust recompiles and
// refreshes the live engine automatically, and editing a scene/Luau hot-reloads
// in place over the broker WebSocket with no reload at all.
//
// `?scene=` boots the embedded engine straight into the scene under edit (the
// shell calls the engine's `ob_goto_scene` export once the runtime is up).

import { useCallback, useEffect, useRef, useState } from "react";
import { Loader2, RefreshCw, AlertTriangle, Hammer } from "lucide-react";

import { useSceneStore } from "@/scene/store";

interface EngineStatus {
  built: boolean;
  status: "idle" | "building" | "ready" | "error" | "disabled";
  generation: number;
  durationMs: number;
  log: string[];
  enabled: boolean;
  profile: "debug" | "release";
}

const POLL_MS = 1500;

export function LiveGamePanel() {
  const active = useSceneStore((s) => s.active);
  const [status, setStatus] = useState<EngineStatus | null>(null);
  // The build generation currently shown in the iframe. Reloading the iframe
  // when the server's generation moves past this is what makes a Rust edit
  // refresh the live engine automatically.
  const [loadedGen, setLoadedGen] = useState(0);
  const pollRef = useRef<number | null>(null);

  const poll = useCallback(() => {
    fetch("/play/__status")
      .then((r) => r.json())
      .then((s: EngineStatus) => setStatus(s))
      .catch(() => setStatus((prev) => prev ?? { built: false, status: "idle", generation: 0, durationMs: 0, log: [], enabled: false, profile: "release" }));
  }, []);

  useEffect(() => {
    poll();
    pollRef.current = window.setInterval(poll, POLL_MS);
    return () => { if (pollRef.current) window.clearInterval(pollRef.current); };
  }, [poll]);

  // Adopt the latest ready build (initial load + after each rebuild).
  useEffect(() => {
    if (status && status.built && status.generation > loadedGen && status.status !== "building") {
      setLoadedGen(status.generation);
    }
  }, [status, loadedGen]);

  const rebuild = useCallback(() => {
    fetch("/play/__build", { method: "POST" }).then(poll).catch(() => {});
  }, [poll]);

  const building = status?.status === "building";
  const errored = status?.status === "error";
  const showFrame = loadedGen > 0 && !!active?.name;

  const broker = `ws://${window.location.host}/__engine/subscribe`;
  const params = new URLSearchParams({ broker });
  if (active?.name) params.set("scene", active.name);
  // Trailing slash matters: the shell loads `bulanci_client.js` relative to
  // `/play/`. Re-key on scene OR build generation so either reloads the engine.
  const src = `/play/?${params.toString()}`;
  const frameKey = `${active?.name ?? ""}:${loadedGen}`;

  return (
    <div className="relative h-full min-h-0 bg-black">
      {showFrame && (
        <iframe
          key={frameKey}
          src={src}
          title="Live engine preview"
          className="h-full w-full border-0"
          allow="autoplay; fullscreen"
        />
      )}

      {!showFrame && !building && !errored && (
        <Center>
          {!active?.name
            ? "Pick a scene in the app bar to preview it in the live engine."
            : status?.status === "disabled"
              ? <DisabledHint />
              : "Preparing the live engine…"}
        </Center>
      )}

      {/* Building/error overlays float over the (possibly stale) frame so the
          last good preview stays visible while a rebuild is in flight. */}
      {building && (
        <Overlay>
          <Loader2 className="size-5 animate-spin text-sky-400" />
          <p className="font-medium text-foreground">Building wasm engine…</p>
          <LogTail log={status?.log ?? []} />
        </Overlay>
      )}
      {errored && (
        <Overlay>
          <AlertTriangle className="size-5 text-rose-400" />
          <p className="font-medium text-rose-300">Engine build failed</p>
          <LogTail log={status?.log ?? []} />
          <button type="button" onClick={rebuild}
            className="mt-1 inline-flex items-center gap-1 rounded border border-border bg-card px-2 py-1 text-[11px] text-foreground hover:bg-accent/50">
            <RefreshCw className="size-3" /> Retry build
          </button>
        </Overlay>
      )}

      {/* Idle "rebuild" affordance in the corner when a frame is shown. */}
      {showFrame && !building && status?.enabled && (
        <button type="button" onClick={rebuild} title="Rebuild the wasm engine now"
          className="absolute right-2 top-2 z-10 inline-flex items-center gap-1 rounded border border-border/60 bg-card/80 px-1.5 py-0.5 text-[10px] text-muted-foreground hover:text-foreground">
          <Hammer className="size-3" /> rebuild
        </button>
      )}
    </div>
  );
}

function Center({ children }: { children: React.ReactNode }) {
  return <div className="flex h-full items-center justify-center p-4 text-center text-[11px] text-muted-foreground">{children}</div>;
}

function Overlay({ children }: { children: React.ReactNode }) {
  return (
    <div className="absolute inset-0 z-20 flex flex-col items-center justify-center gap-2 bg-black/85 p-4 text-center text-[12px] text-muted-foreground">
      {children}
    </div>
  );
}

function LogTail({ log }: { log: string[] }) {
  if (!log.length) return null;
  return (
    <pre className="mt-1 max-h-40 w-full max-w-lg overflow-auto rounded bg-card/60 p-2 text-left font-mono text-[10px] leading-tight text-muted-foreground">
      {log.slice(-12).join("\n")}
    </pre>
  );
}

function DisabledHint() {
  return (
    <div className="flex flex-col items-center gap-2">
      <p className="font-medium text-foreground">Auto-build is off</p>
      <p>Set <code>BULANCI_WEB_AUTOBUILD=1</code> (default) to have the editor build the engine, or build once by hand:</p>
      <code className="rounded bg-card px-2 py-1 font-mono text-[11px] text-sky-300">open_bulanci/scripts/build-web.ps1</code>
    </div>
  );
}
