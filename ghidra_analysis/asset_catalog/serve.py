"""Static-only host for the asset catalog gallery.

Usage:
    python ghidra_analysis/asset_catalog/serve.py [--port 8765]

Browse to http://127.0.0.1:8765/ in your browser.

The gallery is now a fully self-contained React app.  All catalog
merging, registry parsing, AudioBank sample slicing and BitmapJpegAnim
frame enumeration happens in the browser; this server only exposes
three flat URL prefixes:

    /                        -> gallery/dist/ (the built single-page app)
    /data/catalog/<file>     -> ghidra_analysis/asset_catalog/<file>
                                (catalog.json, registry.json, coverage.json,
                                 overlay-index.json — generated on demand)
    /data/overlay/<file>     -> unpacked/overlay/<file>

The `overlay-index.json` listing is built on first request and cached
against the overlay dir's mtime so subsequent loads are instant.

This script is now optional.  Equivalent setups also work:

    * `vp dev` inside gallery/ — Vite+ dev server with HMR.  The
      built-in plugin in vite.config.ts serves the same /data/* tree.
    * Any static host (GitHub Pages, S3, ...) — drop `gallery/dist/`
      plus a `data/` folder laid out the same way and the SPA finds
      everything itself.
    * Browser-only — open the SPA, click "Pick repository folder…"
      and grant the File System Access API permission.  No server
      needed at all.
"""

from __future__ import annotations

import argparse
import json
import mimetypes
import sys
import urllib.parse
from http import HTTPStatus
from http.server import ThreadingHTTPServer, BaseHTTPRequestHandler
from pathlib import Path

CATALOG_DIR      = Path(__file__).parent
REPO_ROOT        = CATALOG_DIR.parent.parent
UNPACKED_OVERLAY = REPO_ROOT / "unpacked" / "overlay"
PAKS_DIR         = REPO_ROOT / "open_bulanci" / "paks"
GALLERY_DIR      = CATALOG_DIR / "gallery"
GALLERY_DIST     = GALLERY_DIR / "dist"
# Codegen handle table, read by the gallery's live-engine bridge to map
# assets to push-target hashes.  Lives with the built assets.
ASSETS_MANIFEST  = REPO_ROOT / "open_bulanci" / "assets" / "manifest.json"


_overlay_index_cache: tuple[float, bytes] | None = None


def _overlay_index_json() -> bytes:
    """Flat list of every filename in unpacked/overlay/, JSON-encoded.

    Cached against the directory's mtime so refreshing the gallery
    doesn't re-walk thousands of files."""
    global _overlay_index_cache
    mtime = UNPACKED_OVERLAY.stat().st_mtime
    if _overlay_index_cache and _overlay_index_cache[0] == mtime:
        return _overlay_index_cache[1]
    names = sorted(p.name for p in UNPACKED_OVERLAY.iterdir() if p.is_file())
    payload = json.dumps(names).encode("utf-8")
    _overlay_index_cache = (mtime, payload)
    return payload


def _scenes_json() -> bytes:
    """Flat list of every scene declared across the load-order profile's
    paks (read-only mirror of the Vite broker's `/data/scenes`)."""
    try:
        profile = json.loads((PAKS_DIR / "profile.json").read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return b'{"scenes":[]}'
    scenes: list[dict] = []
    for entry in profile.get("paks") or []:
        pid = entry.get("id")
        if not pid or entry.get("kind") == "master":
            continue
        folder = entry.get("path") or pid
        try:
            manifest = json.loads((PAKS_DIR / folder / "pak.json").read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            continue
        writable = entry.get("kind") == "mod"
        for name, scene in (manifest.get("scenes") or {}).items():
            scenes.append({
                "pak": pid,
                "name": name,
                "kind": "declarative" if scene.get("doc") else "module",
                "doc": scene.get("doc"),
                "scripts": scene.get("scripts") or [],
                "file": scene.get("file"),
                "writable": writable,
            })
    return json.dumps({"scenes": scenes}).encode("utf-8")


def _pak_folder(pak: str) -> str:
    """Map a pak id to its on-disk folder via the profile (defaults to id)."""
    try:
        profile = json.loads((PAKS_DIR / "profile.json").read_text(encoding="utf-8"))
        for entry in profile.get("paks") or []:
            if entry.get("id") == pak and entry.get("path"):
                return entry["path"]
    except (OSError, json.JSONDecodeError):
        pass
    return pak


def _pak_scripts_json(pak: str) -> bytes:
    """Every `.luau` under a pak folder, pak-relative + sorted (the Scripts
    panel's file browser; read-only mirror of the Vite broker route)."""
    if not pak.replace("_", "").replace("-", "").isalnum():
        return b'{"scripts":[]}'
    root = (PAKS_DIR / _pak_folder(pak)).resolve()
    try:
        root.relative_to(PAKS_DIR.resolve())
    except ValueError:
        return b'{"scripts":[]}'
    scripts: list[str] = []
    if root.is_dir():
        for p in sorted(root.rglob("*.luau")):
            if p.is_file():
                scripts.append(p.relative_to(root).as_posix())
    return json.dumps({"scripts": scripts}).encode("utf-8")


def _resolve_pak_file(rel: str) -> Path | None:
    """Resolve `<pakId>/<relpath>` under PAKS_DIR (profile maps id->folder),
    refusing `..` escapes and unknown paks."""
    if "/" not in rel:
        return None
    pak, _, relpath = rel.partition("/")
    if not relpath or not pak.replace("_", "").replace("-", "").isalnum():
        return None
    folder = pak
    try:
        profile = json.loads((PAKS_DIR / "profile.json").read_text(encoding="utf-8"))
        for entry in profile.get("paks") or []:
            if entry.get("id") == pak and entry.get("path"):
                folder = entry["path"]
                break
    except (OSError, json.JSONDecodeError):
        pass
    return _resolve_under(PAKS_DIR / folder, relpath)


def _send_bytes(handler: BaseHTTPRequestHandler, data: bytes,
                content_type: str, status: int = 200) -> None:
    handler.send_response(status)
    handler.send_header("Content-Type",   content_type)
    handler.send_header("Content-Length", str(len(data)))
    handler.send_header("Cache-Control",  "no-store")
    handler.end_headers()
    handler.wfile.write(data)


def _send_file(handler: BaseHTTPRequestHandler, path: Path) -> None:
    if not path.is_file():
        handler.send_error(HTTPStatus.NOT_FOUND, f"missing: {path.name}")
        return
    ctype = mimetypes.guess_type(path.name)[0] or "application/octet-stream"
    size  = path.stat().st_size
    handler.send_response(200)
    handler.send_header("Content-Type",   ctype)
    handler.send_header("Content-Length", str(size))
    handler.send_header("Cache-Control",  "no-store")
    handler.end_headers()
    with path.open("rb") as f:
        # Loop instead of read-all so big atlases/banks don't balloon RAM.
        while True:
            chunk = f.read(64 * 1024)
            if not chunk: break
            handler.wfile.write(chunk)


def _resolve_under(root: Path, relative: str) -> Path | None:
    """Resolve `relative` against `root`, refusing `..` escapes."""
    # `relative` is the URL-decoded path component, so `..` segments
    # would otherwise let a client read outside the served roots.
    candidate = (root / relative).resolve()
    try:
        candidate.relative_to(root.resolve())
    except ValueError:
        return None
    return candidate


class _Handler(BaseHTTPRequestHandler):
    # quieter access log
    def log_message(self, format: str, *args):  # noqa: A002 - matches stdlib
        sys.stderr.write(f"[serve] {self.address_string()} {format % args}\n")

    def do_GET(self):
        parsed = urllib.parse.urlsplit(self.path)
        path   = parsed.path

        # ---------------- data tree -----------------------------------
        if path == "/data/catalog/overlay-index.json":
            return _send_bytes(self, _overlay_index_json(), "application/json")

        # Scene discovery + read-only pak file access (the static shim never
        # writes; the Vite broker handles PUT for live editing).
        if path == "/data/scenes":
            return _send_bytes(self, _scenes_json(), "application/json")

        if path.startswith("/data/pak-scripts/"):
            pak = urllib.parse.unquote(path[len("/data/pak-scripts/"):])
            return _send_bytes(self, _pak_scripts_json(pak), "application/json")

        if path.startswith("/data/pak/"):
            rel  = urllib.parse.unquote(path[len("/data/pak/"):])
            full = _resolve_pak_file(rel)
            if full is None: return self.send_error(HTTPStatus.FORBIDDEN)
            return _send_file(self, full)

        if path == "/data/catalog/manifest.json":
            if not ASSETS_MANIFEST.is_file():
                return self.send_error(HTTPStatus.NOT_FOUND, "manifest.json not built")
            return _send_file(self, ASSETS_MANIFEST)

        if path.startswith("/data/catalog/"):
            rel  = urllib.parse.unquote(path[len("/data/catalog/"):])
            full = _resolve_under(CATALOG_DIR, rel)
            if full is None: return self.send_error(HTTPStatus.FORBIDDEN)
            return _send_file(self, full)

        if path.startswith("/data/overlay/"):
            rel  = urllib.parse.unquote(path[len("/data/overlay/"):])
            full = _resolve_under(UNPACKED_OVERLAY, rel)
            if full is None: return self.send_error(HTTPStatus.FORBIDDEN)
            return _send_file(self, full)

        # ---------------- gallery SPA --------------------------------
        if not GALLERY_DIST.exists():
            return self.send_error(
                HTTPStatus.NOT_FOUND,
                "gallery/dist not built. Run `vp build` inside gallery/",
            )

        rel  = path.lstrip("/") or "index.html"
        full = _resolve_under(GALLERY_DIST, urllib.parse.unquote(rel))
        if full is None: return self.send_error(HTTPStatus.FORBIDDEN)
        if not full.is_file():
            # SPA fallback for client routes.
            full = GALLERY_DIST / "index.html"
        return _send_file(self, full)


def main() -> int:
    parser = argparse.ArgumentParser(description="Bulanci asset catalog static host")
    parser.add_argument("--port", type=int, default=8765)
    parser.add_argument("--host", default="127.0.0.1")
    args = parser.parse_args()

    if not GALLERY_DIST.exists():
        sys.stderr.write(
            f"warning: {GALLERY_DIST.relative_to(REPO_ROOT)} does not exist.\n"
            "         Run `vp install && vp build` inside gallery/\n"
            "         or `vp dev` for HMR.\n\n"
        )

    server = ThreadingHTTPServer((args.host, args.port), _Handler)
    print(f"asset-catalog server running on http://{args.host}:{args.port}/")
    print(f"  spa:     {GALLERY_DIST}")
    print(f"  catalog: {CATALOG_DIR}")
    print(f"  overlay: {UNPACKED_OVERLAY}")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nbye")
        server.shutdown()
    return 0


if __name__ == "__main__":
    sys.exit(main())
