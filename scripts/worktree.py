#! /usr/bin/env python3
"""Per-agent worktree manager.

Wraps the bits of ``git worktree`` we actually use so individual agent
sessions don't have to memorise the incantations -- and so we can layer
a few extras (``build/orig`` symlink, optional auto-claim, branch
naming convention) on top.

Subcommands
-----------

``create   --agent <id> [--unit <U>] [--base <ref>] [--no-symlink]``
    Create ``worktrees/<id>/`` on a fresh ``agent/<id>`` branch
    branched off ``<ref>`` (default: ``main``).  Symlinks ``build/orig``
    from the main checkout (the ~1.5min headless ExportDelinker step
    only needs to happen once per host).  Optionally claims ``<U>``.

``destroy  --agent <id> [--keep-branch] [--force]``
    Release any claims held by the agent, remove the worktree, and
    delete the ``agent/<id>`` branch (unless ``--keep-branch``).

``reset    --agent <id> [--onto <ref>]``
    From inside ``worktrees/<id>/``: ``git fetch && git rebase`` so the
    worktree picks up the latest committed ``mapping.csv``.

``list``
    Tabular overview of every worktree, its branch, ahead/behind main,
    and which units it currently holds claims on.

``merge    --agent <id> [--keep] [--no-push]``
    Fast-forward ``main`` to ``agent/<id>`` if linear, otherwise refuse
    and tell the user to open a PR.  Releases the agent's claims on
    success.
"""
from __future__ import annotations

import argparse
import os
import shlex
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Iterable

SCRIPTS_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPTS_DIR))

from _state_root import MAIN_REPO_ROOT, is_main_checkout  # noqa: E402

WORKTREES_DIR = MAIN_REPO_ROOT / "worktrees"
BRANCH_PREFIX = "agent/"


def _detect_default_base() -> str:
    """Prefer ``BULANCI_WORKTREE_BASE``; otherwise pick whichever of
    ``main`` / ``master`` actually exists locally.  Falls back to
    ``main`` if neither exists yet (fresh repo)."""
    explicit = os.environ.get("BULANCI_WORKTREE_BASE")
    if explicit:
        return explicit
    # Don't shell out at import time; use git's plumbing once.
    for candidate in ("main", "master"):
        res = subprocess.run(
            ["git", "show-ref", "--verify", f"refs/heads/{candidate}"],
            cwd=str(MAIN_REPO_ROOT),
            capture_output=True,
        )
        if res.returncode == 0:
            return candidate
    return "main"


DEFAULT_BASE = _detect_default_base()


# ---------------------------------------------------------------------------
# subprocess plumbing
# ---------------------------------------------------------------------------


def _run(
    cmd: list[str],
    *,
    cwd: Path | None = None,
    capture: bool = False,
    check: bool = True,
) -> subprocess.CompletedProcess:
    pretty = " ".join(shlex.quote(c) for c in cmd)
    if not capture:
        print(f"[worktree] $ {pretty}", flush=True)
    res = subprocess.run(
        cmd,
        cwd=str(cwd) if cwd else None,
        capture_output=capture,
        text=True,
    )
    if check and res.returncode != 0:
        if capture:
            sys.stderr.write(res.stdout or "")
            sys.stderr.write(res.stderr or "")
        raise SystemExit(
            f"worktree: command failed (exit {res.returncode}): {pretty}"
        )
    return res


def _git(args: list[str], *, cwd: Path | None = None, **kw) -> subprocess.CompletedProcess:
    return _run(["git", *args], cwd=cwd or MAIN_REPO_ROOT, **kw)


# ---------------------------------------------------------------------------
# helpers
# ---------------------------------------------------------------------------


def _branch_name(agent: str) -> str:
    return f"{BRANCH_PREFIX}{agent}"


def _worktree_path(agent: str) -> Path:
    return WORKTREES_DIR / agent


def _existing_worktrees() -> list[dict]:
    """Parse ``git worktree list --porcelain`` into a list of dicts.

    Each entry has keys: ``path``, ``head``, ``branch`` (optional),
    ``locked`` (bool).
    """
    res = _git(["worktree", "list", "--porcelain"], capture=True)
    entries: list[dict] = []
    cur: dict = {}
    for raw in (res.stdout or "").splitlines():
        if not raw.strip():
            if cur:
                entries.append(cur)
                cur = {}
            continue
        if " " in raw:
            key, _, value = raw.partition(" ")
        else:
            key, value = raw, ""
        if key == "worktree":
            cur["path"] = value
        elif key == "HEAD":
            cur["head"] = value
        elif key == "branch":
            # value is like ``refs/heads/agent/foo``
            cur["branch"] = value.split("refs/heads/", 1)[-1]
        elif key == "locked":
            cur["locked"] = True
        elif key == "detached":
            cur["detached"] = True
    if cur:
        entries.append(cur)
    return entries


def _branch_exists(branch: str) -> bool:
    res = _git(
        ["show-ref", "--verify", f"refs/heads/{branch}"],
        capture=True,
        check=False,
    )
    return res.returncode == 0


def _ahead_behind(branch: str, base: str) -> tuple[int, int]:
    """Return ``(ahead, behind)`` of `branch` relative to `base`.

    Returns ``(-1, -1)`` if either ref is missing -- useful for
    ``list``'s display column rather than a hard error.
    """
    res = _git(
        ["rev-list", "--left-right", "--count", f"{base}...{branch}"],
        capture=True,
        check=False,
    )
    if res.returncode != 0:
        return (-1, -1)
    parts = (res.stdout or "").split()
    if len(parts) != 2:
        return (-1, -1)
    behind, ahead = parts
    return int(ahead), int(behind)


def _symlink_or_copy(src: Path, dst: Path) -> str:
    """Create ``dst`` mirroring ``src``.

    Prefer a symlink (fast; works for ``build/orig`` re-use across
    worktrees).  On Windows without developer-mode privileges the
    symlink call may fail; fall back to a directory junction via
    ``mklink /J`` and finally to a plain copy.  Returns the strategy
    used so the caller can log it.
    """
    if dst.exists() or dst.is_symlink():
        return "exists"
    if not src.exists():
        return "skipped-src-missing"
    try:
        dst.symlink_to(src, target_is_directory=True)
        return "symlink"
    except (OSError, NotImplementedError):
        pass
    if os.name == "nt":
        # ``mklink /J`` produces a directory junction; doesn't need
        # admin and ``ninja`` follows it just like a symlink.
        proc = subprocess.run(
            ["cmd", "/c", "mklink", "/J", str(dst), str(src)],
            capture_output=True,
            text=True,
        )
        if proc.returncode == 0:
            return "junction"
    # Last resort: full copy.  Slow but always works.
    shutil.copytree(src, dst)
    return "copy"


def _claim_store():
    """Lazy import; agent_coord pulls in CSV / state stuff we don't
    always need (e.g. ``list`` is fine without it)."""
    from agent_coord import ClaimStore

    return ClaimStore()


def _agent_claims(agent: str) -> list[str]:
    """Return the class names currently claimed by ``agent``."""
    try:
        store = _claim_store()
    except Exception:
        return []
    out = []
    # ``snapshot()`` is the public read of the claims map; values are
    # ``Claim`` dataclasses with ``agent_id`` + ``is_expired()``.
    for class_name, claim in store.snapshot().items():
        if claim.agent_id == agent and not claim.is_expired():
            out.append(class_name)
    return sorted(out)


# ---------------------------------------------------------------------------
# subcommands
# ---------------------------------------------------------------------------


def cmd_create(args: argparse.Namespace) -> int:
    if not is_main_checkout():
        print(
            "worktree create: must be run from the main checkout "
            f"(currently inside a worktree at {Path.cwd()}).",
            file=sys.stderr,
        )
        return 2

    agent = args.agent
    branch = _branch_name(agent)
    wt = _worktree_path(agent)

    if wt.exists():
        print(f"worktree create: {wt} already exists.", file=sys.stderr)
        return 3
    WORKTREES_DIR.mkdir(parents=True, exist_ok=True)

    # ``git worktree add`` accepts an existing branch (checkout it out
    # detached-from-main) or creates a new one with ``-b``.  We always
    # use ``-b`` to enforce the ``agent/<id>`` naming convention.
    if _branch_exists(branch):
        # If the branch already exists (re-creating after a clean
        # destroy) attach without ``-b``.
        _git(["worktree", "add", str(wt), branch])
    else:
        _git(["worktree", "add", str(wt), "-b", branch, args.base])

    # Symlink the expensive ``build/orig`` tree if it exists.  ``build/``
    # itself stays per-worktree so each agent gets its own ninja graph
    # and ``Src/`` generation cache.
    if not args.no_symlink:
        orig_src = MAIN_REPO_ROOT / "build" / "orig"
        if orig_src.exists():
            (wt / "build").mkdir(parents=True, exist_ok=True)
            strategy = _symlink_or_copy(orig_src, wt / "build" / "orig")
            print(f"[worktree] build/orig -> {strategy}")
        else:
            # No baseline objs yet on main; first build will create
            # them.  Surfacing the gap saves head-scratching later.
            print(
                "[worktree] note: build/orig does not exist on main; "
                "the worktree's first `configure.py` run will create it",
                file=sys.stderr,
            )

    # Optional unit claim -- worktree owner usually wants exactly one
    # class in flight at a time.
    if args.unit:
        try:
            store = _claim_store()
            ok = store.claim(args.unit, agent)
            if ok:
                print(f"[worktree] claimed {args.unit} for {agent}")
            else:
                # ``claim`` returns False if another agent holds the
                # class; we surface that but don't undo the worktree
                # creation -- the user can still e.g. abandon and try
                # another unit.
                snap = store.snapshot().get(args.unit)
                holder = snap.agent_id if snap else "(unknown)"
                print(
                    f"[worktree] WARNING: {args.unit} is currently held by {holder}; "
                    f"the worktree exists but you don't have the claim",
                    file=sys.stderr,
                )
        except Exception as e:
            print(
                f"[worktree] WARNING: claim of {args.unit} failed: {e}\n"
                f"  worktree was created; claim manually with "
                f"`scripts/agent_coord.py claim {args.unit} --agent {agent}`",
                file=sys.stderr,
            )

    # Drop a ``.envrc`` so direnv-using shells auto-export the agent id
    # (Phase-5 claim guards key off this).  Plain shells can ``source``
    # it manually -- we use the simplest possible syntax to avoid
    # depending on direnv specifically.
    envrc = wt / ".envrc"
    envrc.write_text(
        f"# Auto-generated by scripts/worktree.py for agent {agent}.\n"
        f"# Source this from your shell (or use direnv) so the\n"
        f"# Bulanci scripts know which agent's claims to enforce.\n"
        f"export BULANCI_AGENT={shlex.quote(agent)}\n",
        encoding="utf-8",
    )
    print(f"[worktree] wrote {envrc.relative_to(MAIN_REPO_ROOT)}")

    print()
    print(f"  cd {wt.relative_to(MAIN_REPO_ROOT)}")
    print(f"  export BULANCI_AGENT={shlex.quote(agent)}   # or `source .envrc`")
    print()
    return 0


def cmd_destroy(args: argparse.Namespace) -> int:
    agent = args.agent
    branch = _branch_name(agent)
    wt = _worktree_path(agent)

    # Release any outstanding claims first; otherwise the next ``list``
    # will misreport "agent X owns Y" long after agent X is gone.
    held = _agent_claims(agent)
    if held:
        store = _claim_store()
        for class_name in held:
            try:
                store.release(class_name, agent)
                print(f"[worktree] released claim on {class_name}")
            except Exception as e:
                print(f"[worktree] WARN: release {class_name}: {e}", file=sys.stderr)

    # ``git worktree remove`` refuses with uncommitted changes unless
    # ``--force``; we pass it through so the user has to opt in.
    if wt.exists():
        cmd = ["worktree", "remove", str(wt)]
        if args.force:
            cmd.append("--force")
        _git(cmd)
    else:
        print(f"[worktree] {wt} already gone")

    if not args.keep_branch and _branch_exists(branch):
        # ``-D`` instead of ``-d`` because the branch may not have been
        # merged back into main; that's the agent's decision.
        _git(["branch", "-D", branch])
    return 0


def cmd_reset(args: argparse.Namespace) -> int:
    agent = args.agent
    wt = _worktree_path(agent)
    if not wt.exists():
        print(f"worktree reset: {wt} does not exist.", file=sys.stderr)
        return 2

    onto = args.onto or DEFAULT_BASE
    # Fetch first so we pull fresh ``mapping.csv`` commits from
    # ``refresh_mapping.py`` runs on main.
    _git(["fetch", "origin"] if args.remote else ["fetch"], cwd=wt, check=False)
    # Allow either ``main`` or ``origin/main`` -- locally-only repos
    # don't have a remote yet, so fall back gracefully.
    rebase_target = f"origin/{onto}" if args.remote else onto
    _git(["rebase", rebase_target], cwd=wt)
    return 0


def cmd_list(args: argparse.Namespace) -> int:
    entries = _existing_worktrees()
    # Filter to ``agent/<id>`` branches; the user's main checkout shows
    # up here too and isn't interesting in this view.
    rows: list[tuple[str, str, str, str]] = []
    for e in entries:
        branch = e.get("branch", "")
        if not branch.startswith(BRANCH_PREFIX) and not args.all:
            continue
        agent = branch[len(BRANCH_PREFIX):] if branch.startswith(BRANCH_PREFIX) else "(main)"
        base = DEFAULT_BASE
        ahead, behind = _ahead_behind(branch or "HEAD", base) if branch else (0, 0)
        ab = f"+{ahead}/-{behind}" if ahead >= 0 else "?"
        claims = ",".join(_agent_claims(agent)) if agent != "(main)" else ""
        rows.append((agent, e.get("path", ""), branch or "(detached)", ab + (" claims=" + claims if claims else "")))

    if not rows:
        print("(no worktrees)")
        return 0

    # Crude column formatting; good enough for ~10 entries.
    widths = [max(len(r[i]) for r in rows) for i in range(4)]
    fmt = "  ".join(f"{{:<{w}}}" for w in widths)
    header = ("AGENT", "PATH", "BRANCH", "AHEAD/BEHIND")
    print(fmt.format(*[h.ljust(w) for h, w in zip(header, widths)]))
    for row in rows:
        print(fmt.format(*row))
    return 0


def cmd_merge(args: argparse.Namespace) -> int:
    if not is_main_checkout():
        print("worktree merge: run this from the main checkout.", file=sys.stderr)
        return 2
    agent = args.agent
    branch = _branch_name(agent)
    if not _branch_exists(branch):
        print(f"worktree merge: branch {branch} does not exist.", file=sys.stderr)
        return 3
    base = args.base
    # Insist on a clean fast-forward: anything else (true merges,
    # rebases, conflict resolution) needs human review and should go
    # through a PR.  Refusing here is the conservative behaviour the
    # user explicitly asked for.
    res = _git(
        ["merge-base", "--is-ancestor", base, branch],
        capture=True,
        check=False,
    )
    if res.returncode != 0:
        print(
            f"worktree merge: {branch} is not a fast-forward over {base}.\n"
            f"  rebase first ({sys.argv[0]} reset --agent {agent}) or open a PR.",
            file=sys.stderr,
        )
        return 4
    _git(["checkout", base])
    _git(["merge", "--ff-only", branch])
    # Release the agent's claims now that the work is upstream.
    held = _agent_claims(agent)
    if held:
        store = _claim_store()
        for class_name in held:
            try:
                store.release(class_name, agent)
                print(f"[worktree] released claim on {class_name}")
            except Exception as e:
                print(f"[worktree] WARN: release {class_name}: {e}", file=sys.stderr)
    # Don't ``git push`` automatically -- user can opt in once their
    # local main looks right.
    if not args.keep and args.cleanup:
        cmd_destroy(argparse.Namespace(agent=agent, keep_branch=False, force=False))
    return 0


# ---------------------------------------------------------------------------
# CLI wiring
# ---------------------------------------------------------------------------


def _build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    subs = p.add_subparsers(dest="command", required=True)

    pc = subs.add_parser("create", help="create a worktree for an agent")
    pc.add_argument("--agent", required=True, help="agent id, e.g. `agent-A`")
    pc.add_argument("--unit", default=None, help="claim this class immediately")
    pc.add_argument(
        "--base",
        default=DEFAULT_BASE,
        help="base ref to branch from (default: main)",
    )
    pc.add_argument(
        "--no-symlink",
        action="store_true",
        help="don't symlink build/orig (force a fresh ExportDelinker per worktree)",
    )
    pc.set_defaults(func=cmd_create)

    pd = subs.add_parser("destroy", help="remove an agent's worktree")
    pd.add_argument("--agent", required=True)
    pd.add_argument("--keep-branch", action="store_true")
    pd.add_argument(
        "--force",
        action="store_true",
        help="pass --force to git worktree remove (drops uncommitted changes)",
    )
    pd.set_defaults(func=cmd_destroy)

    pr = subs.add_parser(
        "reset",
        help="rebase an agent's branch onto main to pick up fresh mapping.csv",
    )
    pr.add_argument("--agent", required=True)
    pr.add_argument(
        "--onto",
        default=None,
        help="base to rebase onto (default: main)",
    )
    pr.add_argument(
        "--remote",
        action="store_true",
        help="fetch + rebase against `origin/<base>` instead of local <base>",
    )
    pr.set_defaults(func=cmd_reset)

    pl = subs.add_parser("list", help="list active worktrees + claims")
    pl.add_argument(
        "--all",
        action="store_true",
        help="include non-agent worktrees too (main checkout etc.)",
    )
    pl.set_defaults(func=cmd_list)

    pm = subs.add_parser(
        "merge",
        help="fast-forward main to an agent branch (refuses non-FF)",
    )
    pm.add_argument("--agent", required=True)
    pm.add_argument("--base", default=DEFAULT_BASE)
    pm.add_argument(
        "--keep",
        action="store_true",
        help="leave the worktree + branch around after a successful merge",
    )
    pm.add_argument(
        "--cleanup",
        action="store_true",
        default=False,
        help="destroy the worktree after a successful merge",
    )
    pm.set_defaults(func=cmd_merge)

    return p


def main(argv: Iterable[str] | None = None) -> int:
    parser = _build_parser()
    args = parser.parse_args(list(argv) if argv is not None else None)
    return args.func(args) or 0


if __name__ == "__main__":
    sys.exit(main())
