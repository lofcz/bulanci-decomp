// Parity check: recompute every handle hash in manifest.json with the
// browser-side blake2b and assert it matches the codegen's hash.  Run
// with `bun scripts/verify_handle_hash.mjs` (or node) from the gallery
// dir.  Exits non-zero on any mismatch so CI can gate on it.

import { readFileSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, resolve } from "node:path";
import { blake2b } from "blakejs";

const here = dirname(fileURLToPath(import.meta.url));
const manifestPath = resolve(here, "../../../../open_bulanci/assets/manifest.json");

const enc = new TextEncoder();
const handleHash = (folder, slug) => {
  const key = `${folder || "_root"}/${slug}`;
  const d = blake2b(enc.encode(key), undefined, 8);
  let hex = "";
  for (const b of d) hex += b.toString(16).padStart(2, "0");
  return hex;
};

const manifest = JSON.parse(readFileSync(manifestPath, "utf8"));
let checked = 0;
let bad = 0;
for (const [hashHex, meta] of Object.entries(manifest)) {
  const got = handleHash(meta.folder, meta.slug);
  checked++;
  if (got !== hashHex) {
    bad++;
    console.error(`MISMATCH ${meta.folder}/${meta.slug}: manifest=${hashHex} computed=${got}`);
  }
}

if (bad > 0) {
  console.error(`\nFAIL: ${bad}/${checked} hashes diverged`);
  process.exit(1);
}
console.log(`OK: ${checked} handle hashes match the codegen (blake2b/8, big-endian)`);
