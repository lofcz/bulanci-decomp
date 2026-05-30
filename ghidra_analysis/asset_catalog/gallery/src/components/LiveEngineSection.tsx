// "Live engine" panel in the asset detail sheet.
//
// The bridge auto-detects a running game subscribed to the studio's broker
// (web tab or native dev client) — there is no manual connect/disconnect,
// exactly like the data source in the assets tab. When a game is attached this
// lets you drop a
// replacement file straight into the engine and watch it hot-reload (no disk
// write, no restart); reverting drops the overlay so the shipped asset returns.
//
// The asset is addressed by its handle hash (resolved from `manifest.json`, or
// `handleHash` as a fallback); nothing about the asset's name leaves the editor.

import { useCallback, useMemo, useRef, useState } from "react";
import {
  Upload, RotateCcw, AlertTriangle, Loader2, Save, Layers, Check,
  RefreshCw, Globe, Radio,
} from "lucide-react";

import type { CatalogEntry } from "@/types";
import type { HandleManifest, PushTarget } from "@/data/handleManifest";
import { resolveTargets } from "@/data/handleManifest";
import { useEngineBridge } from "@/data/EngineBridge";
import { SectionHeading } from "./MetaSection";
import { Button } from "@/components/ui/button";
import {
  Popover, PopoverTrigger, PopoverContent,
} from "@/components/ui/popover";
import { cn } from "@/lib/utils";

interface Props {
  entry: CatalogEntry;
  manifest: HandleManifest | null;
}

export function LiveEngineSection({ entry, manifest }: Props) {
  const bridge = useEngineBridge();
  const targets = useMemo(() => resolveTargets(entry, manifest), [entry, manifest]);
  const online = bridge.status.kind === "online";

  return (
    <section>
      <div className="flex items-center justify-between">
        <SectionHeading>Live engine</SectionHeading>
        <StatusDot />
      </div>

      <div className="space-y-2 rounded-md bg-card/40 p-3">
        <StatusLine />
        {online && bridge.paks.length > 0 && <TargetPicker />}
        {online && (
          !entry.slug ? (
            <p className="text-[11px] italic text-muted-foreground">
              Name this asset (set a slug) to push it into the engine.
            </p>
          ) : targets.length === 0 ? (
            <p className="text-[11px] italic text-muted-foreground">
              No pushable stream for this asset.
            </p>
          ) : (
            <ul className="space-y-1.5">
              {targets.map(t => <TargetRow key={t.hashHex} target={t} />)}
            </ul>
          )
        )}
      </div>
    </section>
  );
}

// ---------------------------------------------------------------------
// per-target row
// ---------------------------------------------------------------------

function TargetRow({ target }: { target: PushTarget }) {
  const bridge = useEngineBridge();
  const inputRef = useRef<HTMLInputElement>(null);
  const [busy, setBusy] = useState<null | "push" | "clear">(null);
  const [error, setError] = useState<string | null>(null);

  const live = bridge.overlays.has(target.hashHex);
  const persisted = bridge.persisted.has(target.hashHex);

  const onPick = useCallback(async (ev: React.ChangeEvent<HTMLInputElement>) => {
    const file = ev.target.files?.[0];
    ev.target.value = "";                       // allow re-picking the same file
    if (!file) return;
    setBusy("push"); setError(null);
    try {
      const bytes = new Uint8Array(await file.arrayBuffer());
      const ext = /\.[^.]+$/.exec(file.name)?.[0] ?? target.ext;
      await bridge.push(target.hashHex, bytes, {
        ext,
        slug: target.slug,
        folder: target.folder,
        className: target.className,
      });
    } catch (err) {
      setError(String((err as Error).message ?? err));
    } finally {
      setBusy(null);
    }
  }, [bridge, target.hashHex]);

  const onRevert = useCallback(async () => {
    setBusy("clear"); setError(null);
    try {
      await bridge.clear(target.hashHex);
    } catch (err) {
      setError(String((err as Error).message ?? err));
    } finally {
      setBusy(null);
    }
  }, [bridge, target.hashHex]);

  return (
    <li className="rounded bg-background/40 px-2 py-1.5">
      <div className="flex items-center gap-2">
        <code className="truncate font-mono text-[11px] text-foreground" title={target.source}>
          {target.label}
        </code>
        {live && (
          <span className="inline-flex items-center gap-1 rounded bg-emerald-500/15 px-1.5 py-px text-[9px] font-bold uppercase tracking-[0.06em] text-emerald-300">
            <span className="size-1.5 animate-pulse rounded-full bg-emerald-400" /> live
          </span>
        )}
        {persisted && (
          <span
            className="inline-flex items-center gap-1 rounded bg-sky-500/15 px-1.5 py-px text-[9px] font-bold uppercase tracking-[0.06em] text-sky-300"
            title={`Saved to the ${bridge.target || "active"} pak — survives restarts`}
          >
            <Save className="size-2.5" /> saved
          </span>
        )}
        <span className="ml-auto flex items-center gap-1">
          {target.needsTranscode ? (
            <span
              className="text-[10px] italic text-muted-foreground"
              title="Ships as MsgPack inside the pack — pushing the raw JSON isn't supported yet."
            >
              msgpack only
            </span>
          ) : (
            <>
              <Button
                type="button"
                size="sm"
                variant="outline"
                className="h-6 gap-1 px-2 text-[10px]"
                disabled={busy !== null}
                onClick={() => inputRef.current?.click()}
              >
                {busy === "push"
                  ? <Loader2 className="size-3 animate-spin" />
                  : <Upload className="size-3" />}
                replace
              </Button>
              {live && (
                <Button
                  type="button"
                  size="sm"
                  variant="ghost"
                  className="h-6 gap-1 px-2 text-[10px] text-muted-foreground hover:text-foreground"
                  disabled={busy !== null}
                  onClick={onRevert}
                >
                  {busy === "clear"
                    ? <Loader2 className="size-3 animate-spin" />
                    : <RotateCcw className="size-3" />}
                  revert
                </Button>
              )}
            </>
          )}
        </span>
      </div>
      <div className="mt-0.5 truncate font-mono text-[9px] text-muted-foreground" title={target.hashHex}>
        {target.source} · {target.hashHex}
      </div>
      {error && (
        <div className="mt-1 flex items-start gap-1 text-[10px] text-rose-300">
          <AlertTriangle className="mt-px size-3 shrink-0" /> {error}
        </div>
      )}
      <input
        ref={inputRef}
        type="file"
        hidden
        accept={target.accept.join(",") || undefined}
        onChange={onPick}
      />
    </li>
  );
}

// ---------------------------------------------------------------------
// status + connect chrome
// ---------------------------------------------------------------------

// Active write-target pak picker (web broker only). Pushes get persisted
// into the chosen mod pak; switching changes where future edits are saved.
function TargetPicker() {
  const bridge = useEngineBridge();
  const mods = bridge.paks.filter(p => p.kind === "mod");
  if (mods.length === 0) return null;
  return (
    <Popover>
      <PopoverTrigger asChild>
        <Button
          type="button"
          size="sm"
          variant="outline"
          className="h-6 w-full justify-start gap-1.5 px-2 text-[10px]"
          title="Where live edits are saved (the active mod pak)"
        >
          <Layers className="size-3 text-sky-400/80" />
          <span className="text-muted-foreground">save to</span>
          <code className="font-mono text-foreground">{bridge.target || mods[mods.length - 1]?.id}</code>
        </Button>
      </PopoverTrigger>
      <PopoverContent className="w-56 p-1" align="start">
        <div className="px-2 py-1 text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground">
          write-target pak
        </div>
        {mods.map(p => (
          <button
            key={p.id}
            type="button"
            className={cn(
              "flex w-full items-center gap-2 rounded px-2 py-1 text-left text-[11px] hover:bg-accent",
              p.id === bridge.target && "text-sky-300",
            )}
            onClick={() => void bridge.setTarget(p.id)}
          >
            <Check className={cn("size-3", p.id === bridge.target ? "opacity-100" : "opacity-0")} />
            <code className="font-mono">{p.id}</code>
          </button>
        ))}
      </PopoverContent>
    </Popover>
  );
}

function StatusDot() {
  const { status } = useEngineBridge();
  const tone =
    status.kind === "online"     ? "bg-emerald-400" :
    status.kind === "connecting" ? "bg-amber-400 animate-pulse" :
    status.kind === "waiting"    ? "bg-amber-400 animate-pulse" :
                                   "bg-muted-foreground/40";  // offline
  const label =
    status.kind === "online"     ? "live" :
    status.kind === "connecting" ? "detecting…" :
    status.kind === "waiting"    ? "no game" :
                                   "offline";
  return (
    <span className="inline-flex items-center gap-1 text-[10px] text-muted-foreground">
      <span className={cn("size-2 rounded-full", tone)} /> {label}
    </span>
  );
}

// Auto-managed status: the bridge probes both transports on its own, so this is
// a read-out (+ an optional manual re-scan), never a connect/disconnect switch.
function StatusLine() {
  const bridge = useEngineBridge();
  const { status } = bridge;

  const TransportIcon = status.kind === "online" ? Globe : Radio;

  return (
    <div className="flex items-center justify-between gap-2 text-[11px]">
      <span className="flex min-w-0 items-center gap-1.5 text-muted-foreground">
        <TransportIcon className="size-3 shrink-0" />
        {status.kind === "online" ? (
          <span className="truncate font-mono" title={bridge.baseUrl}>
            {bridge.baseUrl.replace(/^https?:\/\//, "") || "studio broker"}
          </span>
        ) : status.kind === "waiting" ? (
          <span className="truncate" title={status.detail}>{status.detail}</span>
        ) : status.kind === "connecting" ? (
          <span>looking for a running game…</span>
        ) : (
          <span className="truncate text-rose-300/90" title={status.error}>{status.error}</span>
        )}
      </span>
      <span className="flex shrink-0 items-center gap-2">
        {status.kind === "online" && (
          <span className="text-muted-foreground">
            {status.health.overlays} live · {status.health.packed} packed
            {typeof status.health.clients === "number" ? ` · ${status.health.clients} tab${status.health.clients === 1 ? "" : "s"}` : ""}
          </span>
        )}
        <Button
          type="button"
          size="sm"
          variant="ghost"
          className="h-5 gap-1 px-1.5 text-[10px] text-muted-foreground hover:text-foreground"
          title="Re-scan for a running game now"
          onClick={bridge.refresh}
        >
          <RefreshCw className="size-3" /> rescan
        </Button>
      </span>
    </div>
  );
}
