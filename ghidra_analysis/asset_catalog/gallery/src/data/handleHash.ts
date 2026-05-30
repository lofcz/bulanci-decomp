// Client-side twin of the codegen's asset-handle hash.
//
// The engine addresses every asset by a 64-bit handle — a blake2b
// digest of its canonical `"<folder>/<slug>"` path, *never* the slug
// itself (that's the whole point: no asset names in `.rodata`).  The
// authoritative implementation lives in
// `open_bulanci/asset_pipeline/build_typed_handles.py::hash_path`:
//
//     key = f"{folder or '_root'}/{slug}".encode("utf-8")
//     int.from_bytes(blake2b(key, digest_size=8).digest(), "big")
//
// and the Rust runtime mirrors it in `core/src/assets.rs::hash_path`.
// This module is the third mirror, so the editor can compute the same
// hash to push live overlays through the engine bridge without having
// to ship a lookup table.  It's covered by a parity test
// (`handleHash.test.ts`) that checks a handful of values against
// `manifest.json` so the three implementations can't silently drift.

import { blake2b } from "blakejs";

const enc = new TextEncoder();

/** Canonical key the codegen hashes: `"<folder>/<slug>"`, with a
 *  `_root` sentinel for folder-less assets. */
export function handleKey(folder: string | null | undefined, slug: string): string {
  return `${folder || "_root"}/${slug}`;
}

/** 16-hex-digit handle hash for a `(folder, slug)` pair — the form the
 *  engine bridge's `/overlay/hash/<hex>` route expects.  `digest_size=8`
 *  blake2b, big-endian, matching Python/Rust byte-for-byte. */
export function handleHash(folder: string | null | undefined, slug: string): string {
  // blakejs returns the digest as a Uint8Array in big-endian order, so
  // hex-encoding the bytes in sequence reproduces Python's
  // `int.from_bytes(..., "big")` + `:016x` formatting exactly.
  const digest = blake2b(enc.encode(handleKey(folder, slug)), undefined, 8);
  let hex = "";
  for (const b of digest) hex += b.toString(16).padStart(2, "0");
  return hex;
}
