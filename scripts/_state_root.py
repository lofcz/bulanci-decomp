"""Locate the canonical ``state/`` directory shared across worktrees.

Several scripts (``agent_coord.py``, the new ``mapping.lock`` in
``export_mapping_via_mcp.py``) keep their on-disk coordination under
``<repo>/state/``.  When the repo is checked out as a linked worktree
(``git worktree add``), every worktree has its **own** working tree, so
``Path(__file__).parents[1] / "state"`` would point inside the
worktree -- not the main checkout.  Multiple agents would then end up
talking to **different** ``claims.json`` files and never see each
other's locks.

This helper returns the main checkout root regardless of which
worktree the caller is running in.  The lookup is cheap (one stat plus
maybe one open) and has no dependency on the ``git`` binary.

How worktrees are detected:

* In the **main** checkout, ``<root>/.git`` is a real directory.
* In a **linked worktree**, ``<root>/.git`` is a small text file whose
  first line is ``gitdir: <abs path>/.git/worktrees/<name>``.  Walking
  the absolute path up three levels (``worktrees/<name>`` ->
  ``worktrees`` -> ``.git``) and then one more lands on the main
  checkout root.

Falls back to ``Path(__file__).parents[1]`` if neither shape is
detected so that out-of-tree clones (CI tarballs without ``.git``)
still work.
"""
from __future__ import annotations

import os
from pathlib import Path
from typing import Optional


def _resolve_from_gitfile(gitfile: Path) -> Optional[Path]:
    """Read a worktree ``.git`` pointer and return the main checkout root.

    Returns ``None`` if the file does not match the expected
    ``gitdir: ...`` shape; callers should fall back to a sensible
    default in that case.
    """
    try:
        text = gitfile.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return None
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line.startswith("gitdir:"):
            continue
        gitdir = Path(line.split(":", 1)[1].strip())
        if not gitdir.is_absolute():
            # Some Git versions write a path relative to the .git file.
            gitdir = (gitfile.parent / gitdir).resolve()
        # gitdir points at  <main_git>/.git/worktrees/<name>
        # Walk:  <name> -> worktrees -> .git -> main checkout root.
        try:
            main_root = gitdir.parents[2]
        except IndexError:
            return None
        return main_root
    return None


def find_main_repo_root(start: Optional[Path] = None) -> Path:
    """Return the path of the **main** Git working tree.

    `start` defaults to this module's directory; callers can pass any
    path inside the repo to anchor the search.  Walks upward looking
    for either a ``.git`` directory (main checkout) or a ``.git`` file
    (worktree, which encodes a pointer back to the main root).
    """
    if start is None:
        start = Path(__file__).resolve().parent
    here = start.resolve()
    for candidate in (here, *here.parents):
        dot_git = candidate / ".git"
        if dot_git.is_dir():
            return candidate
        if dot_git.is_file():
            resolved = _resolve_from_gitfile(dot_git)
            if resolved is not None:
                return resolved
            # Fall through: a malformed gitfile shouldn't tank the run.
    # No .git at all (tarball checkout, embedded build).  Assume the
    # script lives at <repo>/scripts/_state_root.py.
    return Path(__file__).resolve().parents[1]


def state_root() -> Path:
    """Return ``<main_repo>/state`` and create the directory on demand."""
    root = find_main_repo_root() / "state"
    root.mkdir(parents=True, exist_ok=True)
    return root


# Convenience constants for scripts that just want the resolved paths.
MAIN_REPO_ROOT = find_main_repo_root()
STATE_DIR = MAIN_REPO_ROOT / "state"


def is_main_checkout(path: Optional[Path] = None) -> bool:
    """Return True if `path` (default: cwd) is the main checkout.

    Useful for scripts that must refuse to run inside a worktree
    (``refresh_mapping.py`` is the obvious one).
    """
    here = (path or Path.cwd()).resolve()
    main = find_main_repo_root(start=here).resolve()
    return here == main


if __name__ == "__main__":
    # `python scripts/_state_root.py` prints the resolved main repo
    # root.  Handy for debugging worktree wiring.
    print(MAIN_REPO_ROOT)
    if not is_main_checkout():
        # Surface the distinction to humans, not just scripts.
        print("(running inside a linked worktree)")
