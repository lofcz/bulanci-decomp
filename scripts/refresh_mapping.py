#! /usr/bin/env python3
"""Refresher: pull a fresh Ghidra snapshot into ``mapping.csv`` and
re-generate all derived artefacts on the **main** checkout.

This script is the **only** sanctioned writer of
``config/bulanci/mapping.csv``.  Worktrees consume mapping.csv read-only
by rebasing against ``main``; they never run this.

What it does, in order, while holding ``state/mapping.lock``:

  1. ``export_mapping_via_mcp.py`` -> ``config/bulanci/mapping.csv``
     (talks to the running Ghidra MCP at ``GHIDRA_MCP_URL``).
  2. ``configure.py --skip-ghidra``  to regenerate ``build/Src``
     headers / stubs that depend on the new mapping rows.
     (The ``--skip-ghidra`` flag avoids the slow headless
     ExportDelinker step; we only need the source-side regeneration.)
  3. ``rename_matched_bodies.py``  so matched body signatures track
     any new prototype / namespace updates.
  4. Optionally ``git commit`` if anything changed.

Why a single-process refresher: two concurrent ``export_mapping_via_mcp``
runs both fetch a coherent snapshot of Ghidra state and then race to be
the last writer of ``mapping.csv`` -- the loser's prototype edits get
rolled back the next time anyone runs ``configure.py``.  The
``state/mapping.lock`` we acquire here serialises that and the commit
keeps the canonical state on ``main``.

Refuses to run from a linked worktree (``.git`` is a file there).  If
you want to refresh mapping you switch to the main checkout first.
"""
from __future__ import annotations

import argparse
import os
import shlex
import subprocess
import sys
from pathlib import Path

# Hoist scripts/ onto sys.path so we can import the shared helpers.
SCRIPTS_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPTS_DIR))

from _file_lock import FileLock, FileLockError  # noqa: E402
from _state_root import (  # noqa: E402
    MAIN_REPO_ROOT,
    STATE_DIR,
    is_main_checkout,
)

MAPPING_CSV = MAIN_REPO_ROOT / "config" / "bulanci" / "mapping.csv"
UNITS_LISTING_CSV = MAIN_REPO_ROOT / "config" / "bulanci" / "units_listing.csv"
LOCK_PATH = STATE_DIR / "mapping.lock"

# Long enough to cover the slowest legitimate refresh (Ghidra export +
# configure + rename ~ 30-90s on this box).  TTL exists only for crash
# recovery; the refresher heartbeats while running.
DEFAULT_LOCK_TTL = 600.0
DEFAULT_LOCK_TIMEOUT = 900.0


def _run(cmd: list[str], *, cwd: Path) -> None:
    """Run a child process with the parent's stdio; bail loudly on failure."""
    pretty = " ".join(shlex.quote(c) for c in cmd)
    print(f"[refresh-mapping] $ {pretty}", flush=True)
    proc = subprocess.run(cmd, cwd=str(cwd))
    if proc.returncode != 0:
        raise SystemExit(
            f"refresh-mapping: step failed (exit {proc.returncode}): {pretty}"
        )


def _git_dirty(paths: list[Path]) -> bool:
    """Return True if ``git status --porcelain`` reports changes for the
    given paths.  Used to decide whether the commit step has anything
    to do; we avoid creating empty commits.
    """
    rel = [str(p.relative_to(MAIN_REPO_ROOT)) for p in paths]
    res = subprocess.run(
        ["git", "status", "--porcelain", "--", *rel],
        cwd=str(MAIN_REPO_ROOT),
        capture_output=True,
        text=True,
        check=True,
    )
    return bool(res.stdout.strip())


def _mapping_row_count() -> int:
    """Cheap row count for nicer commit messages."""
    try:
        with MAPPING_CSV.open("r", encoding="utf-8") as f:
            return sum(1 for _ in f)
    except FileNotFoundError:
        return 0


def refresh(
    *,
    workers: int,
    skip_export: bool,
    skip_configure: bool,
    skip_rename: bool,
    do_commit: bool,
    commit_message: str | None,
    lock_timeout: float,
    lock_ttl: float,
) -> int:
    if not is_main_checkout():
        # Hard fail.  The whole point of this script is the canonical
        # state living on main; running it inside a worktree would
        # commit mapping.csv onto an agent branch and create exactly
        # the merge conflicts we're trying to avoid.
        print(
            "refresh-mapping: refusing to run from a linked worktree.\n"
            "  cd to the main checkout (`scripts/_state_root.py` will show it) "
            "and re-run from there.",
            file=sys.stderr,
        )
        return 2

    lock = FileLock(
        LOCK_PATH,
        owner="refresh-mapping",
        ttl_seconds=lock_ttl,
        acquire_timeout_seconds=lock_timeout,
    )
    print(f"[refresh-mapping] acquiring {LOCK_PATH}")
    try:
        lock.acquire()
    except FileLockError as e:
        print(f"refresh-mapping: {e}", file=sys.stderr)
        return 3

    try:
        py = sys.executable

        # Step 1: export Ghidra state into mapping.csv.  We pass
        # ``--skip-lock`` because the caller (us) already owns the
        # canonical mapping.lock; otherwise the child would deadlock
        # itself.
        if not skip_export:
            _run(
                [
                    py,
                    str(SCRIPTS_DIR / "internal" / "export_mapping_via_mcp.py"),
                    "--workers",
                    str(workers),
                    "--output",
                    str(MAPPING_CSV),
                    "--skip-lock",
                ],
                cwd=MAIN_REPO_ROOT,
            )
            lock.heartbeat()
        else:
            print("[refresh-mapping] --skip-export: keeping mapping.csv as-is")

        # Step 2: regenerate derived sources / headers.
        # ``--skip-ghidra`` because we don't need the headless
        # ExportDelinker run; mapping.csv already reflects the latest
        # Ghidra state.
        if not skip_configure:
            _run(
                [py, str(SCRIPTS_DIR / "configure.py"), "--skip-ghidra"],
                cwd=MAIN_REPO_ROOT,
            )
            lock.heartbeat()
        else:
            print("[refresh-mapping] --skip-configure: not regenerating sources")

        # Step 3: align matched-body signatures with the new mapping.
        if not skip_rename:
            _run(
                [
                    py,
                    str(SCRIPTS_DIR / "internal" / "rename_matched_bodies.py"),
                ],
                cwd=MAIN_REPO_ROOT,
            )
            lock.heartbeat()
        else:
            print("[refresh-mapping] --skip-rename: leaving matched bodies untouched")

        # Step 4: commit.  We only stage files we actually own and only
        # commit if at least one of them changed -- no empty commits.
        if do_commit:
            paths = [MAPPING_CSV, UNITS_LISTING_CSV,
                     MAIN_REPO_ROOT / "src", MAIN_REPO_ROOT / "include"]
            if not _git_dirty(paths):
                print("[refresh-mapping] no changes to commit; done")
                return 0
            rows = _mapping_row_count()
            msg = commit_message or f"ghidra: refresh mapping ({rows} rows)"
            _run(
                ["git", "add", "--",
                 str(MAPPING_CSV.relative_to(MAIN_REPO_ROOT)),
                 str(UNITS_LISTING_CSV.relative_to(MAIN_REPO_ROOT)),
                 "src", "include"],
                cwd=MAIN_REPO_ROOT,
            )
            _run(["git", "commit", "-m", msg], cwd=MAIN_REPO_ROOT)
            # NOTE: deliberately no `git push`.  Whoever runs the
            # refresher decides whether to publish (CI vs. solo).
        else:
            print("[refresh-mapping] --no-commit: leaving changes uncommitted")

        return 0
    finally:
        lock.release()


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--workers",
        type=int,
        default=int(os.environ.get("BULANCI_EXPORT_WORKERS", "16")),
        help="parallel HTTP workers for the Ghidra export step",
    )
    parser.add_argument(
        "--skip-export",
        action="store_true",
        help="skip the Ghidra export step (re-run only configure + rename)",
    )
    parser.add_argument(
        "--skip-configure",
        action="store_true",
        help="skip the configure.py --skip-ghidra step",
    )
    parser.add_argument(
        "--skip-rename",
        action="store_true",
        help="skip the rename_matched_bodies.py step",
    )
    parser.add_argument(
        "--no-commit",
        dest="do_commit",
        action="store_false",
        default=True,
        help="produce changes but DO NOT git commit (useful for local previews)",
    )
    parser.add_argument(
        "--message",
        default=None,
        help="override the auto-generated commit message",
    )
    parser.add_argument(
        "--lock-timeout",
        type=float,
        default=DEFAULT_LOCK_TIMEOUT,
        help="seconds to wait for an existing refresher before giving up",
    )
    parser.add_argument(
        "--lock-ttl",
        type=float,
        default=DEFAULT_LOCK_TTL,
        help="stale-lock TTL; only matters if the refresher crashes",
    )
    args = parser.parse_args(argv)

    return refresh(
        workers=args.workers,
        skip_export=args.skip_export,
        skip_configure=args.skip_configure,
        skip_rename=args.skip_rename,
        do_commit=args.do_commit,
        commit_message=args.message,
        lock_timeout=args.lock_timeout,
        lock_ttl=args.lock_ttl,
    )


if __name__ == "__main__":
    sys.exit(main())
