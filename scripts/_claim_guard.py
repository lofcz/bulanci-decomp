"""Shared ``--agent`` guard for write-side scripts.

``sync_units.py``, ``generate_sources.py`` and
``internal/rename_matched_bodies.py`` can all rewrite arbitrary unit
files.  When the user runs them with ``--agent <id>`` (or via the
``BULANCI_AGENT`` env var that ``scripts/worktree.py`` exports), each
write is gated on an *active* claim in ``state/claims.json``.

Why a helper instead of inlining: keeping the policy in one place
makes it trivial to flip behaviour later (warn-then-allow, strict,
allow-with-stale-claim, etc.).

Two entry points:

``resolve_agent(cli_agent: str|None) -> str|None``
    The agent id we should enforce against, or ``None`` (= permissive
    legacy mode).

``guard_units(agent: str|None, units: Iterable[str], *, action: str)``
    Raises ``SystemExit`` if any of the requested unit names are not
    currently held by ``agent``.  ``action`` is included in the error
    so the user knows which step refused.

There is also a thin ``guard_class(...)`` for callers that already
have a single class name in hand.
"""
from __future__ import annotations

import os
import sys
from pathlib import Path
from typing import Iterable, Optional

# Allow this file to live alongside the other shared scripts and still
# be importable from ``scripts/internal/`` callers.
sys.path.insert(0, str(Path(__file__).resolve().parent))


def resolve_agent(cli_agent: Optional[str]) -> Optional[str]:
    """Pick the agent id to enforce against.

    Priority: explicit ``--agent`` > ``BULANCI_AGENT`` env > nothing.
    Returning ``None`` is the documented "permissive" mode (the
    refresher running on main needs this so a single committer can
    rewrite any file).
    """
    if cli_agent:
        return cli_agent
    env = os.environ.get("BULANCI_AGENT")
    if env:
        return env
    return None


def _store():
    """Lazy import so ``--help`` doesn't drag the CSV machinery in."""
    from agent_coord import ClaimStore

    return ClaimStore()


def guard_class(agent: Optional[str], class_name: str, *, action: str) -> None:
    """Refuse to ``action`` ``class_name`` unless ``agent`` holds it.

    No-op when ``agent`` is ``None`` (permissive mode -- see
    ``resolve_agent``).  Exits 5 on a failed guard, matching the rest
    of the pipeline's "policy violation" convention.
    """
    if not agent:
        return
    store = _store()
    if store.is_held(class_name, agent):
        return
    snap = store.snapshot().get(class_name)
    holder = snap.agent_id if (snap and not snap.is_expired()) else "(unclaimed)"
    sys.stderr.write(
        f"refusing to {action} {class_name}: not claimed by {agent} "
        f"(holder: {holder}).  Run `scripts/agent_coord.py claim "
        f"{class_name} --agent {agent}` first.\n"
    )
    raise SystemExit(5)


def guard_units(
    agent: Optional[str],
    units: Iterable[str],
    *,
    action: str,
    allow_unclaimed: bool = False,
) -> list[str]:
    """Filter `units` to those `agent` is allowed to touch.

    In permissive mode (``agent is None``) every unit passes through
    untouched.  In enforcing mode:
      * units claimed by ``agent`` pass through.
      * units claimed by *somebody else* are dropped with a stderr
        warning.
      * unclaimed units pass through iff ``allow_unclaimed`` is True;
        otherwise they're also dropped with a warning.

    Returns the filtered list so the caller can keep doing useful work
    on the subset it actually owns.
    """
    if not agent:
        return list(units)
    store = _store()
    snap = store.snapshot()
    keep: list[str] = []
    for unit in units:
        cur = snap.get(unit)
        if cur and not cur.is_expired() and cur.agent_id == agent:
            keep.append(unit)
            continue
        if cur and not cur.is_expired():
            sys.stderr.write(
                f"[{action}] skipping {unit}: claimed by {cur.agent_id}, "
                f"not {agent}\n"
            )
            continue
        if allow_unclaimed:
            keep.append(unit)
        else:
            sys.stderr.write(
                f"[{action}] skipping {unit}: no live claim "
                f"(use --no-enforce or claim it for {agent})\n"
            )
    return keep
