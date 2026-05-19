"""Class-level claim/release coordinator for the bulanci annotation swarm.

Lives entirely on the local filesystem under ``state/claims.json`` to
avoid pulling in a database / message-queue dependency for the early
phases. Atomic writes via ``os.replace`` + a sidecar lockfile so
multiple agent processes on one box can call ``claim`` / ``release``
without corrupting the JSON.

Concepts:

* A **claim** ties an ``agent_id`` to a ``class_name`` (one of the
  units in ``config/bulanci/units_listing.csv``) for a TTL window
  (default 30 minutes).
* Agents must ``heartbeat`` while working; claims older than their
  ``expires_at`` are automatically reclaimable.
* Releasing a claim records the list of functions the agent named /
  renamed, so a future scheduler can prioritise *un-touched* classes
  and re-route classes the agent could not finish.

Programmatic API (see ``__main__`` for the CLI):

    from scripts.agent_coord import ClaimStore
    store = ClaimStore()
    if store.claim("CBulanci", agent_id="agent-073"):
        ...
        store.release("CBulanci", attempted=["FUN_004032c0"])

CLI:

    python scripts/agent_coord.py claim CBulanci --agent agent-073
    python scripts/agent_coord.py heartbeat CBulanci --agent agent-073
    python scripts/agent_coord.py release CBulanci --agent agent-073
    python scripts/agent_coord.py list
    python scripts/agent_coord.py list --unclaimed
    python scripts/agent_coord.py cleanup
"""

from __future__ import annotations

import argparse
import csv
import json
import os
import sys
import time
import uuid
from contextlib import contextmanager
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Iterator

REPO_ROOT = Path(os.path.realpath(__file__)).resolve().parents[1]
STATE_DIR = REPO_ROOT / "state"
CLAIMS_FILE = STATE_DIR / "claims.json"
LOCK_FILE = STATE_DIR / "claims.json.lock"
UNITS_FILE = REPO_ROOT / "config" / "bulanci" / "units_listing.csv"

DEFAULT_TTL_SECONDS = 30 * 60
HEARTBEAT_EXTEND_SECONDS = 30 * 60


@dataclass
class Claim:
    class_name: str
    agent_id: str
    claimed_at: float
    expires_at: float
    attempted_functions: list[str] = field(default_factory=list)

    def is_expired(self, now: float | None = None) -> bool:
        now = time.time() if now is None else now
        return self.expires_at <= now


class ClaimStore:
    """File-backed claims store. Safe for multiple processes on one host."""

    def __init__(self, claims_file: Path = CLAIMS_FILE):
        self.claims_file = claims_file
        STATE_DIR.mkdir(parents=True, exist_ok=True)

    @contextmanager
    def _locked(self) -> Iterator[None]:
        """Cooperative exclusive lock via O_EXCL sidecar file.

        Linear retry with a 50 ms back-off. We don't expect heavy
        contention (one host, dozens of agents tops). If we cannot
        acquire after ~5 s, raise - that signals an orphaned lock the
        caller can investigate.
        """
        deadline = time.time() + 5.0
        while True:
            try:
                fd = os.open(str(LOCK_FILE), os.O_CREAT | os.O_EXCL | os.O_WRONLY)
                os.write(fd, str(os.getpid()).encode("ascii"))
                os.close(fd)
                break
            except FileExistsError:
                if time.time() >= deadline:
                    raise RuntimeError(
                        f"could not acquire {LOCK_FILE} after 5s. "
                        "If no other agent is running, delete the file "
                        "manually and retry."
                    )
                time.sleep(0.05)
        try:
            yield
        finally:
            try:
                os.unlink(LOCK_FILE)
            except FileNotFoundError:
                pass

    def _read(self) -> dict[str, Claim]:
        if not self.claims_file.exists():
            return {}
        try:
            raw = json.loads(self.claims_file.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            return {}
        return {k: Claim(**v) for k, v in raw.items()}

    def _write(self, claims: dict[str, Claim]) -> None:
        tmp = self.claims_file.with_suffix(".json.tmp")
        payload = {k: asdict(v) for k, v in claims.items()}
        tmp.write_text(json.dumps(payload, indent=2, sort_keys=True), encoding="utf-8")
        os.replace(tmp, self.claims_file)

    def claim(
        self,
        class_name: str,
        agent_id: str,
        ttl_seconds: int = DEFAULT_TTL_SECONDS,
    ) -> bool:
        """Try to claim a class. Returns True on success."""
        now = time.time()
        with self._locked():
            claims = self._read()
            cur = claims.get(class_name)
            if cur and not cur.is_expired(now) and cur.agent_id != agent_id:
                return False
            preserved_attempts = cur.attempted_functions if cur else []
            claims[class_name] = Claim(
                class_name=class_name,
                agent_id=agent_id,
                claimed_at=now,
                expires_at=now + ttl_seconds,
                attempted_functions=preserved_attempts,
            )
            self._write(claims)
            return True

    def heartbeat(
        self,
        class_name: str,
        agent_id: str,
        extend_seconds: int = HEARTBEAT_EXTEND_SECONDS,
    ) -> bool:
        with self._locked():
            claims = self._read()
            cur = claims.get(class_name)
            if cur is None or cur.agent_id != agent_id:
                return False
            cur.expires_at = max(cur.expires_at, time.time() + extend_seconds)
            claims[class_name] = cur
            self._write(claims)
            return True

    def release(
        self,
        class_name: str,
        agent_id: str,
        attempted: list[str] | None = None,
    ) -> bool:
        with self._locked():
            claims = self._read()
            cur = claims.get(class_name)
            if cur is None or cur.agent_id != agent_id:
                return False
            if attempted:
                cur.attempted_functions = sorted(set(cur.attempted_functions) | set(attempted))
            # Releasing keeps the row (with attempted_functions populated)
            # but expires it so the next claim wins immediately.
            cur.expires_at = 0.0
            claims[class_name] = cur
            self._write(claims)
            return True

    def cleanup_stale(self) -> int:
        """Forget claims whose TTL passed AND that have no recorded work."""
        now = time.time()
        with self._locked():
            claims = self._read()
            removed = 0
            for k, v in list(claims.items()):
                if v.is_expired(now) and not v.attempted_functions:
                    del claims[k]
                    removed += 1
            self._write(claims)
            return removed

    def snapshot(self) -> dict[str, Claim]:
        return self._read()


def _load_units() -> list[str]:
    """All unit (class) names known to the build, alphabetical."""
    if not UNITS_FILE.exists():
        return []
    seen: set[str] = set()
    with UNITS_FILE.open(newline="", encoding="utf-8") as f:
        for row in csv.reader(f):
            if not row:
                continue
            seen.add(row[0])
    return sorted(seen)


def _format_claim(c: Claim, now: float) -> str:
    remaining = max(0, int(c.expires_at - now))
    age = int(now - c.claimed_at)
    status = "EXPIRED" if c.is_expired(now) else f"{remaining}s remaining"
    return (
        f"  {c.class_name:<25s} agent={c.agent_id:<18s} "
        f"age={age:>5d}s  {status:>14s}  "
        f"attempted={len(c.attempted_functions)}"
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="Bulanci agent coordinator.")
    sub = parser.add_subparsers(dest="cmd", required=True)

    p_claim = sub.add_parser("claim", help="Acquire a class claim.")
    p_claim.add_argument("class_name")
    p_claim.add_argument("--agent", required=True)
    p_claim.add_argument("--ttl", type=int, default=DEFAULT_TTL_SECONDS)

    p_hb = sub.add_parser("heartbeat", help="Extend an existing claim.")
    p_hb.add_argument("class_name")
    p_hb.add_argument("--agent", required=True)

    p_rel = sub.add_parser("release", help="Mark a claim done.")
    p_rel.add_argument("class_name")
    p_rel.add_argument("--agent", required=True)
    p_rel.add_argument("--attempted", nargs="*", default=[])

    p_list = sub.add_parser("list", help="Show claims.")
    p_list.add_argument(
        "--unclaimed", action="store_true",
        help="Show classes that have NO active claim (good for pickers).",
    )
    p_list.add_argument(
        "--csv", action="store_true",
        help="Emit machine-readable CSV instead of human text.",
    )

    sub.add_parser("cleanup", help="Drop expired claims with no recorded work.")

    p_pick = sub.add_parser(
        "pick",
        help="Atomically claim the next unclaimed class for an agent.",
    )
    p_pick.add_argument("--agent", required=True)
    p_pick.add_argument("--ttl", type=int, default=DEFAULT_TTL_SECONDS)
    p_pick.add_argument(
        "--prefer", default="CBulanci,CGame,CGaming,CBulanek",
        help="Comma-separated preference order. Falls back to alphabetical.",
    )

    args = parser.parse_args()
    store = ClaimStore()
    now = time.time()

    if args.cmd == "claim":
        ok = store.claim(args.class_name, args.agent, ttl_seconds=args.ttl)
        print("OK" if ok else "BUSY")
        return 0 if ok else 1

    if args.cmd == "heartbeat":
        ok = store.heartbeat(args.class_name, args.agent)
        print("OK" if ok else "NOT_HELD")
        return 0 if ok else 1

    if args.cmd == "release":
        ok = store.release(args.class_name, args.agent, attempted=args.attempted)
        print("OK" if ok else "NOT_HELD")
        return 0 if ok else 1

    if args.cmd == "list":
        claims = store.snapshot()
        units = _load_units()
        active = {k: v for k, v in claims.items() if not v.is_expired(now)}
        if args.unclaimed:
            free = [u for u in units if u not in active]
            for u in free:
                print(u)
            return 0
        if args.csv:
            w = csv.writer(sys.stdout)
            w.writerow(["class", "agent_id", "claimed_at", "expires_at", "attempted_count"])
            for c in claims.values():
                w.writerow([
                    c.class_name, c.agent_id, c.claimed_at, c.expires_at,
                    len(c.attempted_functions),
                ])
            return 0
        if not claims:
            print("(no claims recorded)")
            return 0
        print(f"{len(active)}/{len(claims)} claims active out of {len(units)} units.")
        for c in sorted(claims.values(), key=lambda c: c.class_name):
            print(_format_claim(c, now))
        return 0

    if args.cmd == "cleanup":
        removed = store.cleanup_stale()
        print(f"removed {removed} stale claim(s).")
        return 0

    if args.cmd == "pick":
        prefs = [p.strip() for p in args.prefer.split(",") if p.strip()]
        units = _load_units()
        claims = store.snapshot()
        free = [
            u for u in units
            if u not in claims or claims[u].is_expired(now)
        ]
        if not free:
            print("(no unclaimed classes)")
            return 1
        ordered = [p for p in prefs if p in free] + [u for u in free if u not in prefs]
        for cls in ordered:
            if store.claim(cls, args.agent, ttl_seconds=args.ttl):
                print(cls)
                return 0
        print("(every candidate busy)")
        return 1

    return 2


if __name__ == "__main__":
    sys.exit(main())
