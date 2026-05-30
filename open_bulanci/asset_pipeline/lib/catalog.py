"""Load the auto-generated asset catalog.

The catalog (`ghidra_analysis/asset_catalog/catalog.json`) lists every
master-pack resource with its className, file list, xrefs etc.  The
pipeline only needs className + files; both are derived directly.
"""
from __future__ import annotations

import json
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parents[3]
CATALOG_PATH = _REPO_ROOT / "ghidra_analysis" / "asset_catalog" / "catalog.json"


def load_catalog(path: Path | None = None) -> dict:
    p = path or CATALOG_PATH
    return json.loads(p.read_text(encoding="utf-8"))


def index_by_id(catalog: dict) -> dict[int, dict]:
    """Map resource-id -> catalog entry for O(1) lookup."""
    return {e["id"]: e for e in catalog.get("entries") or []}
