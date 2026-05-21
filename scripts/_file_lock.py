"""Cross-process file lock with TTL + heartbeat semantics.

Originally lived inline in ``agent_coord.py`` (the
``ClaimStore._locked`` context manager).  Lifted out here because the
mapping-csv refresher (``export_mapping_via_mcp.py`` /
``refresh_mapping.py``) needs the **same** lock primitives -- a tiny
exclusive sidecar file with cooperative TTL and heartbeat -- and we
don't want to drag a full ``ClaimStore`` instance just to serialise an
export.

Design notes:

* ``O_CREAT | O_EXCL | O_WRONLY`` gives us atomic ``test-and-create``
  on POSIX and Windows alike (Python's ``open`` flags map to the
  Win32 ``CREATE_NEW`` disposition).  No fcntl, no msvcrt.locking.
* The lock file holds a small JSON payload (``{"pid", "host",
  "owner", "claimed_at", "expires_at"}``) so a human or another
  agent can ``cat`` it to see who is holding it and for how long.
* A **stale** lock (``expires_at`` in the past AND the pid is gone)
  is reclaimable: the next acquirer overwrites the file.  This is
  the only reason we need a TTL at all -- if every agent released
  cleanly we'd just use the bare ``O_EXCL`` primitive.

Usage:

    from _file_lock import FileLock
    with FileLock("state/mapping.lock", owner="export-mapping",
                  ttl_seconds=300):
        ...  # critical section
"""
from __future__ import annotations

import contextlib
import json
import os
import socket
import time
from pathlib import Path
from typing import Iterator, Optional


class FileLockError(RuntimeError):
    """Raised when the lock cannot be acquired within the deadline."""


class FileLock:
    """Cooperative exclusive lock on a sidecar file.

    Parameters
    ----------
    path
        Lock file path.  Created when held, removed when released.
    owner
        Short human-readable label persisted into the file; surfaces
        in error messages so debugging stuck locks is painless.
    ttl_seconds
        How long until the lock is considered stale and reclaimable.
        Crash recovery only; cleanly-released locks are unlinked
        regardless of TTL.
    acquire_timeout_seconds
        Maximum time to block waiting for an existing holder before
        ``FileLockError`` is raised.  ``None`` means block forever.
    """

    def __init__(
        self,
        path: os.PathLike | str,
        owner: str = "unknown",
        ttl_seconds: float = 300.0,
        acquire_timeout_seconds: Optional[float] = 60.0,
        poll_interval_seconds: float = 0.1,
    ):
        self.path = Path(path)
        self.owner = owner
        self.ttl_seconds = ttl_seconds
        self.acquire_timeout_seconds = acquire_timeout_seconds
        self.poll_interval_seconds = poll_interval_seconds
        self._held = False
        self._payload: Optional[dict] = None

    # ----- introspection ---------------------------------------------------

    def read_payload(self) -> Optional[dict]:
        """Best-effort read of the current holder's payload, or None."""
        try:
            with self.path.open("r", encoding="utf-8") as f:
                return json.load(f)
        except (FileNotFoundError, json.JSONDecodeError):
            return None

    def _is_stale(self, now: float) -> bool:
        payload = self.read_payload()
        if payload is None:
            return False
        expires_at = float(payload.get("expires_at", 0.0))
        if expires_at > now:
            return False
        pid = int(payload.get("pid", 0))
        if pid and _pid_alive(pid):
            # The holder is still around -- respect the TTL even after
            # it expires.  Heartbeats are the holder's job; we refuse
            # to steal a lock from a live process.
            return False
        return True

    # ----- acquire / release ----------------------------------------------

    def acquire(self) -> None:
        if self._held:
            raise FileLockError(f"{self.path} already held by this instance")
        self.path.parent.mkdir(parents=True, exist_ok=True)
        deadline = (
            time.time() + self.acquire_timeout_seconds
            if self.acquire_timeout_seconds is not None
            else float("inf")
        )
        last_stale_warning = 0.0
        while True:
            now = time.time()
            try:
                fd = os.open(
                    str(self.path),
                    os.O_CREAT | os.O_EXCL | os.O_WRONLY,
                    0o644,
                )
            except FileExistsError:
                # Try to reclaim a stale lock; otherwise wait.
                if self._is_stale(now):
                    if now - last_stale_warning > 5.0:
                        # One nudge per ~5s; otherwise noisy in tight
                        # acquire loops.
                        last_stale_warning = now
                    try:
                        self.path.unlink()
                    except FileNotFoundError:
                        pass
                    continue
                if now >= deadline:
                    payload = self.read_payload() or {}
                    raise FileLockError(
                        f"timed out acquiring {self.path} after "
                        f"{self.acquire_timeout_seconds}s; held by "
                        f"{payload.get('owner', '?')} "
                        f"pid={payload.get('pid', '?')} "
                        f"(delete the file by hand if you're sure no "
                        f"process is using it)"
                    )
                time.sleep(self.poll_interval_seconds)
                continue
            # We won the race; serialise the payload.
            self._payload = {
                "pid": os.getpid(),
                "host": socket.gethostname(),
                "owner": self.owner,
                "claimed_at": now,
                "expires_at": now + self.ttl_seconds,
            }
            try:
                os.write(fd, json.dumps(self._payload).encode("utf-8"))
            finally:
                os.close(fd)
            self._held = True
            return

    def heartbeat(self, extend_seconds: Optional[float] = None) -> None:
        """Push ``expires_at`` forward.  Idempotent if not held."""
        if not self._held:
            return
        bump = extend_seconds if extend_seconds is not None else self.ttl_seconds
        if self._payload is None:
            return
        self._payload["expires_at"] = time.time() + bump
        # Best effort -- if the file vanished underneath us (another
        # agent assumed we crashed) we silently no-op rather than
        # raising; the caller will hit the *real* contention on the
        # next acquire.
        try:
            with self.path.open("w", encoding="utf-8") as f:
                json.dump(self._payload, f)
        except OSError:
            pass

    def release(self) -> None:
        if not self._held:
            return
        try:
            self.path.unlink()
        except FileNotFoundError:
            # Stolen by a stale-claim reaper; nothing we can do.
            pass
        self._held = False
        self._payload = None

    # ----- context-manager sugar ------------------------------------------

    def __enter__(self) -> "FileLock":
        self.acquire()
        return self

    def __exit__(self, exc_type, exc, tb) -> None:
        self.release()


def _pid_alive(pid: int) -> bool:
    """Return True if `pid` looks like it is still running.

    POSIX: ``os.kill(pid, 0)`` raises ``ProcessLookupError`` if not.
    Windows: ``os.kill`` simulates the same on Python 3.2+; we treat
    any ``OSError`` as "dead enough".
    """
    if pid <= 0:
        return False
    try:
        os.kill(pid, 0)
    except ProcessLookupError:
        return False
    except PermissionError:
        # The process exists but is owned by someone else.  Still
        # alive from a locking perspective.
        return True
    except OSError:
        return False
    return True


@contextlib.contextmanager
def file_lock(
    path: os.PathLike | str,
    owner: str = "unknown",
    ttl_seconds: float = 300.0,
    acquire_timeout_seconds: Optional[float] = 60.0,
) -> Iterator[FileLock]:
    """Convenience wrapper for one-shot ``with file_lock(...): ...``."""
    lock = FileLock(
        path,
        owner=owner,
        ttl_seconds=ttl_seconds,
        acquire_timeout_seconds=acquire_timeout_seconds,
    )
    lock.acquire()
    try:
        yield lock
    finally:
        lock.release()
