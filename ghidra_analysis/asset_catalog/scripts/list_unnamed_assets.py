#!/usr/bin/env python3
"""List master-pack assets that still need human names (Bulánci naming waves).

Loads catalog.json + registry.json, walks the gallery-canonical tile list
(catalog entries sorted by id, excluding AudioBankIndex), and flags assets
that lack a registry slug and/or still have a missing, empty, or generic
catalog ``name``.

The gallery "to-do" count (~149) matches assets with no ``registry.slug``.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

CATALOG_DIR = Path(__file__).resolve().parents[1]
CATALOG_PATH = CATALOG_DIR / "catalog.json"
REGISTRY_PATH = CATALOG_DIR / "registry.json"
DEFAULT_BATCHES_PATH = CATALOG_DIR / "naming" / "unnamed_batches.json"
BATCH_SIZE = 10


def canonical_stem(entry: dict[str, Any]) -> str:
    id10 = str(entry["id"]).zfill(10)
    return f"res_{id10}_{entry['classId']}_{entry['className']}"


def resource_key(entry: dict[str, Any]) -> str:
    return f"0x{entry['id']:08x}"


def is_generic_catalog_name(entry: dict[str, Any], name: Any) -> bool:
    if name is None:
        return True
    if not isinstance(name, str):
        return True
    stripped = name.strip()
    if not stripped:
        return True
    if stripped.startswith("res_"):
        return True
    if stripped == canonical_stem(entry):
        return True
    return False


def registry_slug(entry: dict[str, Any], assets: dict[str, Any]) -> str | None:
    reg = assets.get(resource_key(entry)) or {}
    slug = reg.get("slug")
    return slug if isinstance(slug, str) and slug.strip() else None


def asset_path(entry: dict[str, Any], assets: dict[str, Any]) -> str:
    key = resource_key(entry)
    folder = (assets.get(key) or {}).get("folder")
    if isinstance(folder, str) and folder.strip():
        return folder.strip()
    files = entry.get("files") or []
    if files:
        return str(files[0])
    return canonical_stem(entry)


def is_unnamed(
    entry: dict[str, Any],
    assets: dict[str, Any],
    *,
    gallery_only: bool,
) -> bool:
    """Return True if this tile still needs naming work."""
    has_slug = registry_slug(entry, assets) is not None
    generic_name = is_generic_catalog_name(entry, entry.get("name"))
    if gallery_only:
        return not has_slug
    return generic_name or not has_slug


def load_data() -> tuple[list[dict[str, Any]], dict[str, Any]]:
    catalog = json.loads(CATALOG_PATH.read_text(encoding="utf-8"))
    registry = json.loads(REGISTRY_PATH.read_text(encoding="utf-8"))
    entries = sorted(catalog.get("entries") or [], key=lambda e: e["id"])
    assets = registry.get("assets") or {}
    return entries, assets


def build_unnamed_records(
    entries: list[dict[str, Any]],
    assets: dict[str, Any],
    *,
    gallery_only: bool,
) -> list[dict[str, Any]]:
    # Global index map (full catalog, including AudioBankIndex).
    index_by_id = {e["id"]: i for i, e in enumerate(entries)}

    tiles = [e for e in entries if e.get("className") != "AudioBankIndex"]
    out: list[dict[str, Any]] = []
    for entry in tiles:
        if not is_unnamed(entry, assets, gallery_only=gallery_only):
            continue
        slug = registry_slug(entry, assets)
        name = entry.get("name")
        reasons: list[str] = []
        if is_generic_catalog_name(entry, name):
            reasons.append("catalog_name_missing_or_generic")
        if slug is None:
            reasons.append("registry_slug_missing")
        out.append(
            {
                "index": index_by_id[entry["id"]],
                "id": entry["id"],
                "id_hex": resource_key(entry),
                "path": asset_path(entry, assets),
                "class": entry.get("className"),
                "classId": entry.get("classId"),
                "catalog_name": name,
                "registry_slug": slug,
                "reasons": reasons,
                "files": entry.get("files") or [],
            }
        )
    return out


def print_table(records: list[dict[str, Any]]) -> None:
    print(f"Unnamed assets: {len(records)}")
    if not records:
        return
    header = f"{'idx':>4}  {'id':>6}  {'id_hex':>10}  {'class':<16}  path / file"
    print(header)
    print("-" * len(header))
    for r in records:
        print(
            f"{r['index']:4d}  {r['id']:6d}  {r['id_hex']:>10}  "
            f"{(r.get('class') or '?'):<16}  {r['path']}"
        )


def build_batches(records: list[dict[str, Any]]) -> list[list[int]]:
    ids = [r["id"] for r in records]
    batches: list[list[int]] = []
    for i in range(0, len(ids), BATCH_SIZE):
        batches.append(ids[i : i + BATCH_SIZE])
    return batches


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--json",
        metavar="PATH",
        help=f"Write batch JSON (default: {DEFAULT_BATCHES_PATH})",
        nargs="?",
        const=str(DEFAULT_BATCHES_PATH),
    )
    parser.add_argument(
        "--strict-catalog",
        action="store_true",
        help="Only catalog name checks (ignores gallery slug count; not the default).",
    )
    args = parser.parse_args()

    entries, assets = load_data()
    gallery_only = not args.strict_catalog
    records = build_unnamed_records(entries, assets, gallery_only=gallery_only)
    print_table(records)

    if args.json:
        out_path = Path(args.json)
        out_path.parent.mkdir(parents=True, exist_ok=True)
        payload = {
            "total": len(records),
            "batch_size": BATCH_SIZE,
            "batch_count": len(build_batches(records)),
            "definition": (
                "gallery_no_registry_slug"
                if gallery_only
                else "catalog_name_or_no_registry_slug"
            ),
            "assets": records,
            "batches": build_batches(records),
        }
        out_path.write_text(
            json.dumps(payload, indent=2, ensure_ascii=False) + "\n",
            encoding="utf-8",
        )
        print(f"\nWrote {out_path}", file=sys.stderr)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
