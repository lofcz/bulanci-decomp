"""Read the hand-curated asset registry.

The registry (`ghidra_analysis/asset_catalog/registry.json`) is the
single source of truth for *named* master-pack assets.  Both the
gallery UI (via `serve.py`) and the runtime asset pipeline (via
`build_assets.py`) consume it.

The on-disk format is plain JSON with a sidecar JSON Schema (so a
VS Code-style editor offers autocompletion).  Edits happen through
the gallery's `registryEdits.ts` (programmatic — no regex) or by
direct file edits.  See `registry.schema.json` for the contract.

This module is intentionally thin: it just parses JSON and surfaces
typed `NamedAsset` records.  Mapping a record to an actual file (i.e.
choosing the right extractor based on className) lives in
`extractors.py` so the registry can stay class-agnostic.
"""
from __future__ import annotations

import json
from dataclasses import dataclass, field
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parents[3]
REGISTRY_PATH = _REPO_ROOT / "ghidra_analysis" / "asset_catalog" / "registry.json"


@dataclass(frozen=True)
class NamedAsset:
    """One `assets["0x..."]` row from registry.json."""

    resource_id: int
    id_hex:      str                                  # "0x000100ae"
    slug:        str
    notes:       str | None     = None
    ship:        bool           = False
    folder:      str | None     = None                # "menu/widgets/radio"
    samples:     dict[int, str] = field(default_factory=dict)


@dataclass(frozen=True)
class ManualAsset:
    """One row from the registry's top-level `manual_assets` array.

    These are synthetic — they have *no* master-pack catalog ID — so
    `build_typed_handles.py` can't look up their `className` or
    derive a source path automatically.  The registry author supplies
    both up front.
    """

    slug:   str
    cls:    str            # Rust marker class (BitmapSpecial / Font / ...)
    source: str            # path inside `open_bulanci/assets/`
    notes:  str | None     = None
    folder: str | None     = None


@dataclass(frozen=True)
class NamedScript:
    """One `scripts["<dec>"]` row from registry.json."""

    script_id: int
    slug:      str
    notes:     str | None = None


@dataclass(frozen=True)
class FolderInfo:
    """One `folders["path"]` row from registry.json (sparse — only
    folders with notes / tags appear)."""

    path:  str
    notes: str | None = None
    tags:  tuple[str, ...] = ()


def load_registry(path: Path | None = None) -> dict:
    """Raw JSON data — useful for callers that need extra fields."""
    p = path or REGISTRY_PATH
    with p.open("r", encoding="utf-8") as f:
        return json.load(f)


def load_named_assets(path: Path | None = None) -> list[NamedAsset]:
    """Parse the `assets` table into typed records."""
    reg = load_registry(path)
    out: list[NamedAsset] = []
    for key, blob in (reg.get("assets") or {}).items():
        slug = blob.get("slug")
        if not slug:
            continue
        rid = int(key, 16)
        # JSON object keys are always strings; coerce sample indexes
        # back to ints so callers can iterate them in numeric order.
        samples = {int(k): v for k, v in (blob.get("samples") or {}).items()}
        out.append(NamedAsset(
            resource_id=rid,
            id_hex=f"0x{rid:08x}",
            slug=slug,
            notes=blob.get("notes"),
            ship=bool(blob.get("ship", False)),
            folder=_normalise_folder(blob.get("folder")),
            samples=samples,
        ))
    return out


def load_manual_assets(path: Path | None = None) -> list[ManualAsset]:
    """Parse the top-level `manual_assets` array, if present.

    Returns an empty list when the registry has no manual entries.
    Validates `class` and `source` are non-empty strings — the codegen
    needs both to emit a usable handle.
    """
    reg = load_registry(path)
    out: list[ManualAsset] = []
    for blob in reg.get("manual_assets") or []:
        slug = blob.get("slug")
        cls  = blob.get("class")
        src  = blob.get("source")
        if not slug or not cls or not src:
            continue
        out.append(ManualAsset(
            slug=slug,
            cls=cls,
            source=src,
            notes=blob.get("notes"),
            folder=_normalise_folder(blob.get("folder")),
        ))
    return out


def load_folders(path: Path | None = None) -> dict[str, FolderInfo]:
    """Parse the sparse `folders` metadata table."""
    reg = load_registry(path)
    out: dict[str, FolderInfo] = {}
    for path_key, blob in (reg.get("folders") or {}).items():
        normalised = _normalise_folder(path_key)
        if not normalised:
            continue
        out[normalised] = FolderInfo(
            path=normalised,
            notes=blob.get("notes"),
            tags=tuple(blob.get("tags") or ()),
        )
    return out


def _normalise_folder(raw: str | None) -> str | None:
    """Strip leading/trailing/redundant slashes; reject empty paths."""
    if not isinstance(raw, str):
        return None
    cleaned = "/".join(p for p in (s.strip() for s in raw.split("/")) if p)
    return cleaned or None


def load_named_scripts(path: Path | None = None) -> list[NamedScript]:
    """Parse the `scripts` table into typed records."""
    reg = load_registry(path)
    out: list[NamedScript] = []
    for key, blob in (reg.get("scripts") or {}).items():
        slug = blob.get("slug")
        if not slug:
            continue
        out.append(NamedScript(
            script_id=int(key),
            slug=slug,
            notes=blob.get("notes"),
        ))
    return out
