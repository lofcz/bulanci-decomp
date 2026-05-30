# Asset folders & typed Rust handles

This document covers the *project-level* asset grouping system and how
it lands as strongly-typed Rust code at compile time.

## Project layout (mutable, human-friendly)

In `ghidra_analysis/asset_catalog/registry.json` each named asset
carries a `folder` path plus a `slug`:

```json
"assets": {
  "0x00010152": {
    "slug":   "menu_sfx_bank",
    "folder": "menu/sfx",
    "ship":   true
  }
}
```

The folder path is:

* **forward-slash separated** — every segment becomes a Rust module.
* **arbitrary and cross-class** — `menu/widgets/radio` can hold a mix
  of `BitmapSpecial`, `BitmapSprite`, `AudioBank` … the grouping is
  about how *we* think about the asset, not its file type.
* **purely organisational** — moving an asset between folders changes
  Rust call sites but not the asset itself.

Folder-level notes can be attached via a sparse `folders` table — only
folders that actually need a description appear there:

```json
"folders": {
  "menu/widgets/radio": {
    "notes": "Five-state CRadio skin: idle/hover × selected/unselected + flash."
  }
}
```

Folder notes show up in the gallery's detail panel; they're not
embedded in the shipped binary.

The full schema is in
`ghidra_analysis/asset_catalog/registry.schema.json` — VS Code picks
it up via the `$schema` pointer for autocomplete and validation.

## Compile-time codegen

`open_bulanci/asset_pipeline/build_typed_handles.py` reads
`registry.json` and emits two artefacts:

### `open_bulanci/client/src/generated/assets.rs`

A nested `pub mod` tree where each leaf is a `pub const` typed handle:

```rust
pub mod menu {
    pub mod widgets {
        pub mod radio {
            /// Radio button: selected + mouse hover.
            pub const RADIO_SELECTED_HOVER: AssetHandle<BitmapSpecial>
                = AssetHandle::new(0x6e58921e8d66d5a3u64);
            // ...
        }
    }
}
```

The constant's type carries the asset class as a phantom parameter
(`AssetHandle<BitmapSpecial>`, `AssetHandle<AudioBank>`, …) so the
asset manager's `load` method can return the right concrete type
without a runtime cast:

```rust
let bank: AssetHandle<AudioBank> = assets::menu::sfx::MENU_SFX_BANK;
let pcm  = asset_manager.load(bank)?;   // returns AudioBank, not &[u8]
```

The `AssetHandle<T>` type and the per-class marker structs live in
hand-written Rust (`crate::asset::handle` and `crate::asset::types`).
The generator only *references* them.

### `open_bulanci/assets/manifest.json`

A simple table mapping each handle's 64-bit hash to where the asset
manager can load it from:

```json
{
  "63bb5b6d1e7692bc": {
    "class":  "AudioBank",
    "folder": "menu/sfx",
    "slug":   "menu_sfx_bank",
    "source": "audio/menu_sfx_bank/"
  }
}
```

For local development the asset manager reads files straight from
`open_bulanci/assets/` using `source`.  For the eventual packed VFS
the manifest's `source` is replaced with an offset/length inside a
single pack file — and `folder`/`slug` are dropped — so the shipped
binary only carries the opaque 16-hex-digit keys.

## The hash

```python
hash = blake2b(f"{folder}/{slug}".encode("utf-8"), digest_size=8)
```

* **blake2b** — fast, cryptographic-strength, no salt.
* **8 bytes (u64)** — 2⁶⁴ keyspace, ~0% collision chance for our
  asset count; the generator still validates.
* **Stable across runs** — the same `folder/slug` always produces the
  same hash, so renaming an asset is "edit registry.json → re-run the
  generator → fix the Rust call sites that referenced the old
  constant".  No migrations needed for the on-disk artefacts.

## What does *not* ship

The shipped binary contains only the u64 hashes.  In particular it
does **not** contain:

* the folder path (`menu/sfx`),
* the slug (`menu_sfx_bank`),
* the catalog notes,
* the original Bulánci resource ID (`0x10152`),
* the unpacker's canonical filename (`res_0000065874_43_AudioBank.wav`).

Anyone reverse-engineering the binary sees the same opaque keys we
embedded — the project structure stays internal.

## Workflow

1. Edit `registry.json` (or use the gallery's editor UI) — name new
   assets, group with `folder`.
2. Refresh the gallery → see the layout change live (no restart).
3. `python open_bulanci/asset_pipeline/build_typed_handles.py`
   → regenerates `assets.rs` + `manifest.json`.
4. Update Rust call sites to use the new typed constants.
5. `python open_bulanci/asset_pipeline/build_assets.py`
   → re-exports the actual asset files (still keyed by `slug`).

The generator's `--check` flag validates without writing — good for
CI:

```sh
python open_bulanci/asset_pipeline/build_typed_handles.py --check
```
