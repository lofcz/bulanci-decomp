// Editor → engine live-overlay bridge (client side).
//
// There is a single dev transport: the studio dev-server broker
// (`vite.config.ts :: bulanciModBroker`), which relays pushes to every
// connected game over a WebSocket. Both web *and* native dev clients
// subscribe to that broker (the native client via `client/src/asset/ws_dev.rs`,
// opt-in with `BULANCI_DEV_EDITOR`), so the editor no longer talks to any
// game directly — it just pushes to the broker, which fans out.
//
// The bridge **auto-detects** whether a game is attached — no manual
// connect/disconnect, the same way the data source auto-detects in the assets
// tab. The broker is always up under `vite dev`, so it only counts as live
// once at least one game has subscribed (its `/health` reports `clients > 0`).
//
// Pushing bytes installs an in-memory overlay in every attached game and
// triggers its hot-reload path — the same code the disk watcher drives, so a
// push and a `Ctrl+S` are indistinguishable downstream. Assets are addressed
// purely by their 64-bit handle hash; no slug or path crosses to the engine,
// matching the no-`.rodata`-leak invariant.

import {
  createContext, useCallback, useContext, useEffect, useMemo, useRef, useState,
} from "react";

import type { PatchOp } from "@/scene/diff";

// The studio's own dev server hosts the broker on this origin (relative path
// → resolves against wherever the editor is served), which fans pushes out to
// every attached game over a WebSocket. See the gallery's vite.config.ts
// (`bulanciModBroker`).
export const WEB_BRIDGE_URL = "/__engine";

/** Which live transport is currently attached. Only `web` (the studio broker)
 *  exists today — native dev clients subscribe to that same broker rather than
 *  being probed directly. The union is kept for forward-compat. */
export type Transport = "native" | "web";

/** Probe order. The broker is the only transport: it's always up under
 *  `vite dev` and counts as live once a game (web tab or native client) has
 *  subscribed. A static production build has no broker, so it has no live
 *  target at all. */
const CANDIDATES: { url: string; transport: Transport }[] = import.meta.env.DEV
  ? [{ url: WEB_BRIDGE_URL, transport: "web" }]
  : [];

const POLL_MS = 3000;
/** Per-candidate probe budget so a refused loopback (no native game) can't
 *  stall the poll. A refused connection rejects well within this anyway. */
const PROBE_TIMEOUT_MS = 1500;

export interface BridgeHealth {
  ok: boolean;
  /** Count of embedded (packed) handles. */
  packed: number;
  /** Count of live overlays in the engine. */
  overlays: number;
  /** Connected game clients (web broker only; the native bridge omits it,
   *  since it *is* the single game). */
  clients?: number;
  /** Active write-target pak id (web broker only). */
  target?: string;
  /** Count of overlays backed by a persisted pak file (web broker only). */
  persisted?: number;
}

/** One mod pak the broker knows about. */
export interface PakInfo { id: string; kind: string }

/** Optional per-asset metadata sent with a push so the broker can name
 *  the persisted file and record a meaningful `pak.json` entry. */
export interface PushMeta {
  ext?: string;
  slug?: string;
  folder?: string | null;
  className?: string;
  /** When true the push is live-only and not written to the active pak. */
  transient?: boolean;
  /** When true the broker refreshes its stored snapshot but does NOT broadcast
   *  to attached games — they keep their live-patched state (no rebuild). Used
   *  by the idle re-sync so reconnects stay current without disrupting the game
   *  you're editing against. Implies `transient`. */
  cacheOnly?: boolean;
}

export type BridgeStatus =
  // First probe in flight (startup only).
  | { kind: "connecting" }
  // A game is attached on `transport` and ready for live edits.
  | { kind: "online";  health: BridgeHealth; transport: Transport }
  // Bridge infrastructure is up (e.g. the studio broker) but no game is
  // attached yet — patches would go nowhere, so we surface it distinctly.
  | { kind: "waiting"; detail: string }
  // Nothing reachable (no native game, no broker).
  | { kind: "offline"; error: string };

export interface EngineBridge {
  /** The detected live endpoint URL, or "" when no game is attached. */
  baseUrl: string;
  /** Which transport is live, or null when offline/waiting. */
  transport: Transport | null;
  status:  BridgeStatus;
  /** Hashes currently overridden in the engine (16-hex). */
  overlays: Set<string>;
  /** Subset of `overlays` that is persisted to a pak file (web broker). */
  persisted: Set<string>;
  /** Mod paks the broker can write into (empty for the native bridge). */
  paks: PakInfo[];
  /** Active write-target pak id ("" when unknown / native bridge). */
  target: string;
  /** Number of games currently subscribed to the broker (0 when offline).
   *  The scene store watches this to re-push the full doc when a client
   *  attaches — so a freshly (re)opened game catches up to the live state. */
  clients: number;
  /** Pick the active write-target pak (web broker only). */
  setTarget(id: string): Promise<void>;
  /** Re-probe transports + overlay list now. */
  refresh(): void;
  /** Overwrite a handle's bytes in the running engine (and persist into
   *  the active pak unless `meta.transient`). */
  push(hashHex: string, bytes: Uint8Array, meta?: PushMeta): Promise<void>;
  /** Drop an overlay so the lower pak / embedded asset comes back. */
  clear(hashHex: string): Promise<void>;
  /** Stream a scene-HMR patch: the engine applies `ops` in place to the live
   *  scene named `scene` (preserving the Luau VM). No-op when `ops` is empty
   *  or the engine isn't showing `scene`. */
  pushScenePatch(scene: string, ops: PatchOp[], rev: number): Promise<void>;
}

// ---------------------------------------------------------------------
// raw HTTP calls
// ---------------------------------------------------------------------

/** Probe one candidate's `/health`. Returns its health on success, or `null`
 *  when it isn't reachable (refused / timed out / non-200) — auto-detection
 *  treats every transport as optional, so an absent one is not an error. */
async function probeHealth(baseUrl: string): Promise<BridgeHealth | null> {
  const ctrl = new AbortController();
  const timer = setTimeout(() => ctrl.abort(), PROBE_TIMEOUT_MS);
  try {
    const r = await fetch(`${baseUrl}/health`, { signal: ctrl.signal, cache: "no-store" });
    if (!r.ok) return null;
    return await r.json() as BridgeHealth;
  } catch {
    return null;
  } finally {
    clearTimeout(timer);
  }
}

/** A transport is *live* (a usable push target) when a game is actually
 *  attached: the web broker reports `clients`, so it needs `clients > 0`; the
 *  native bridge has no such field because it is itself the one game. */
function isLive(h: BridgeHealth): boolean {
  return typeof h.clients === "number" ? h.clients > 0 : true;
}

interface OverlayState {
  overlays: string[];
  persisted: string[];
  target: string;
  paks: PakInfo[];
}

async function fetchOverlays(baseUrl: string, signal: AbortSignal): Promise<OverlayState> {
  const r = await fetch(`${baseUrl}/overlays`, { signal, cache: "no-store" });
  if (!r.ok) throw new Error(`overlays: HTTP ${r.status}`);
  const body = await r.json() as Partial<OverlayState>;
  return {
    overlays: body.overlays ?? [],
    persisted: body.persisted ?? [],
    target: body.target ?? "",
    paks: body.paks ?? [],
  };
}

// ---------------------------------------------------------------------
// structural equality — keep state references stable across identical
// polls so the context value (and every consumer) doesn't re-render every
// `POLL_MS` just because `new Set(...)`/`{ health }` are fresh objects.
// ---------------------------------------------------------------------

function setEq(a: Set<string>, b: readonly string[]): boolean {
  if (a.size !== b.length) return false;
  for (const x of b) if (!a.has(x)) return false;
  return true;
}

function paksEq(a: readonly PakInfo[], b: readonly PakInfo[]): boolean {
  if (a.length !== b.length) return false;
  for (let i = 0; i < a.length; i++) {
    if (a[i].id !== b[i].id || a[i].kind !== b[i].kind) return false;
  }
  return true;
}

function healthEq(a: BridgeHealth, b: BridgeHealth): boolean {
  return a.ok === b.ok && a.packed === b.packed && a.overlays === b.overlays
    && a.clients === b.clients && a.target === b.target && a.persisted === b.persisted;
}

// ---------------------------------------------------------------------
// store hook
// ---------------------------------------------------------------------

export function useEngineBridgeStore(): EngineBridge {
  const [status, setStatus] = useState<BridgeStatus>({ kind: "connecting" });
  const [transport, setTransport] = useState<Transport | null>(null);
  // The detected live endpoint. Mirrored into a ref so push/clear/patch read
  // the freshest target without rebuilding their callbacks each detection.
  const [baseUrl, setBaseUrl] = useState<string>("");
  const activeUrl = useRef<string>("");
  const [overlays, setOverlays] = useState<Set<string>>(new Set());
  const [persisted, setPersisted] = useState<Set<string>>(new Set());
  const [paks, setPaks] = useState<PakInfo[]>([]);
  const [target, setTargetState] = useState<string>("");

  // Bump to force a re-probe outside the timer (after a push/clear or refresh).
  const [pollNonce, setPollNonce] = useState(0);
  const refresh = useCallback(() => setPollNonce(n => n + 1), []);

  // Auto-detect: probe every candidate transport, attach to the first that has
  // a live game, and reflect that in `status` — no manual enable.
  useEffect(() => {
    let cancelled = false;
    const ctrl = new AbortController();

    const goNotOnline = (next: BridgeStatus) => {
      activeUrl.current = "";
      setBaseUrl((u) => (u === "" ? u : ""));
      setTransport((t) => (t === null ? t : null));
      setStatus((prev) => {
        if (prev.kind === next.kind) {
          if (next.kind === "waiting" && prev.kind === "waiting" && prev.detail === next.detail) return prev;
          if (next.kind === "offline" && prev.kind === "offline" && prev.error === next.error) return prev;
          if (next.kind === "connecting") return prev;
        }
        return next;
      });
      setOverlays((p) => (p.size ? new Set() : p));
      setPersisted((p) => (p.size ? new Set() : p));
      setPaks((p) => (p.length ? [] : p));
    };

    const tick = async () => {
      // Probe all candidates in parallel (so a refused native loopback doesn't
      // serialize the broker probe), then pick by priority order.
      const probed = await Promise.all(
        CANDIDATES.map(async (c) => ({ c, health: await probeHealth(c.url) })),
      );
      if (cancelled) return;

      let live: { c: typeof CANDIDATES[number]; health: BridgeHealth } | null = null;
      let reachable: { c: typeof CANDIDATES[number]; health: BridgeHealth } | null = null;
      for (const { c, health } of probed) {     // already in priority order
        if (!health) continue;
        reachable ??= { c, health };
        if (isLive(health)) { live = { c, health }; break; }
      }

      if (!live) {
        if (reachable) {
          // Broker is up but no game has subscribed yet.
          goNotOnline({
            kind: "waiting",
            detail: "studio broker up — no game connected. Open the web game tab, or launch the native client with BULANCI_DEV_EDITOR=1.",
          });
        } else {
          goNotOnline({
            kind: "offline",
            error: import.meta.env.DEV
              ? "studio broker unreachable"
              : "live editing needs the studio dev server (vite dev)",
          });
        }
        return;
      }

      // A live game is attached — record the endpoint + pull its overlay list.
      activeUrl.current = live.c.url;
      const ov = await fetchOverlays(live.c.url, ctrl.signal).catch(() => ({
        overlays: [], persisted: [], target: "", paks: [],
      } as OverlayState));
      if (cancelled) return;

      const winner = live;   // narrow for the closures below
      setBaseUrl((u) => (u === winner.c.url ? u : winner.c.url));
      setTransport((t) => (t === winner.c.transport ? t : winner.c.transport));
      // Reuse the previous reference whenever the polled value is structurally
      // unchanged — React then bails out instead of re-rendering every consumer.
      setStatus((prev) => (prev.kind === "online" && prev.transport === winner.c.transport && healthEq(prev.health, winner.health))
        ? prev : { kind: "online", health: winner.health, transport: winner.c.transport });
      setOverlays((prev) => setEq(prev, ov.overlays) ? prev : new Set(ov.overlays));
      setPersisted((prev) => setEq(prev, ov.persisted) ? prev : new Set(ov.persisted));
      setPaks((prev) => paksEq(prev, ov.paks) ? prev : ov.paks);
      setTargetState(ov.target || winner.health.target || "");
    };

    void tick();
    const id = window.setInterval(tick, POLL_MS);
    return () => { cancelled = true; ctrl.abort(); window.clearInterval(id); };
  }, [pollNonce]);

  const setTarget = useCallback(async (id: string) => {
    const url = activeUrl.current;
    if (!url) return;
    setTargetState(id);   // optimistic
    try {
      await fetch(`${url}/target`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ id }),
      });
    } catch { /* native bridge has no target endpoint */ }
    refresh();
  }, [refresh]);

  const push = useCallback(async (hashHex: string, bytes: Uint8Array, meta?: PushMeta) => {
    const url = activeUrl.current;
    if (!url) return;   // no game attached — nothing to push to
    const headers: Record<string, string> = { "Content-Type": "application/octet-stream" };
    if (meta?.ext) headers["X-Asset-Ext"] = meta.ext;
    if (meta?.slug) headers["X-Asset-Slug"] = meta.slug;
    if (meta?.folder) headers["X-Asset-Folder"] = meta.folder;
    if (meta?.className) headers["X-Asset-Class"] = meta.className;
    if (meta?.transient) headers["X-Asset-Transient"] = "1";
    if (meta?.cacheOnly) headers["X-Asset-Cache-Only"] = "1";
    const r = await fetch(`${url}/overlay/hash/${hashHex}`, {
      method:  "PUT",
      headers,
      body:    new Blob([bytes as BlobPart]),
    });
    if (!r.ok) throw new Error(`push ${hashHex}: HTTP ${r.status}`);
    setOverlays(prev => new Set(prev).add(hashHex));   // optimistic
    refresh();
  }, [refresh]);

  const clear = useCallback(async (hashHex: string) => {
    const url = activeUrl.current;
    if (!url) return;
    const r = await fetch(`${url}/overlay/hash/${hashHex}`, { method: "DELETE" });
    if (!r.ok) throw new Error(`clear ${hashHex}: HTTP ${r.status}`);
    setOverlays(prev => {
      const next = new Set(prev);
      next.delete(hashHex);
      return next;
    });
    refresh();
  }, [refresh]);

  const pushScenePatch = useCallback(async (scene: string, ops: PatchOp[], rev: number) => {
    if (!ops.length) return;
    const url = activeUrl.current;
    if (!url) return;   // no game attached — the edit just isn't streamed
    const r = await fetch(`${url}/scene/patch`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      // `surface` is the surface router's routing key (a scene name today);
      // the engine also accepts the legacy `scene` alias.
      body: JSON.stringify({ surface: scene, rev, ops }),
    });
    if (!r.ok) throw new Error(`scene patch: HTTP ${r.status}`);
  }, []);

  // Surfaced so consumers (the scene store) can react to a game attaching /
  // detaching without reaching into the `status` union.
  const clients = status.kind === "online" && typeof status.health.clients === "number"
    ? status.health.clients : 0;

  return useMemo<EngineBridge>(() => ({
    baseUrl, transport, status, overlays, persisted, paks, target, clients,
    setTarget, refresh, push, clear, pushScenePatch,
  }), [baseUrl, transport, status, overlays, persisted, paks, target, clients,
       setTarget, refresh, push, clear, pushScenePatch]);
}

// ---------------------------------------------------------------------
// context glue
// ---------------------------------------------------------------------

const Ctx = createContext<EngineBridge | null>(null);
Ctx.displayName = "EngineBridge";

export function EngineBridgeProvider({ children }: { children: React.ReactNode }) {
  const value = useEngineBridgeStore();
  return <Ctx.Provider value={value}>{children}</Ctx.Provider>;
}

export function useEngineBridge(): EngineBridge {
  const ctx = useContext(Ctx);
  if (!ctx) throw new Error("useEngineBridge called outside provider");
  return ctx;
}
