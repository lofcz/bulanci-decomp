"""Shared helpers for the open_bulanci asset pipeline.

Submodules
----------
* `registry`   — load the hand-curated registry (single source of truth
                 for asset names).
* `catalog`    — load the auto-generated catalog (provides className).
* `extractors` — class-driven, generic export helpers from
                 `unpacked/overlay/` to `open_bulanci/assets/`.
"""
