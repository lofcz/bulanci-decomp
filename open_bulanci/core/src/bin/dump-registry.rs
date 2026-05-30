//! `dump-registry` — export the declarative scene control type registry as
//! JSON for the editor (its control toolbox + property grid are generated
//! from this, so they never drift from the engine's `derive(Reflect)`
//! control structs; see `bulanci_core::scene::registry`).
//!
//! Usage:
//!   cargo run -p bulanci_core --bin dump-registry [OUTPUT]
//!
//! With no argument it writes to the asset-studio catalog dir
//! (`ghidra_analysis/asset_catalog/control-registry.json`, served by the
//! gallery at `/data/catalog/control-registry.json`). Pass a path to write
//! elsewhere, or `-` to print to stdout.

use std::path::PathBuf;

fn default_out() -> PathBuf {
    // core lives at <repo>/open_bulanci/core; the editor catalog is at
    // <repo>/ghidra_analysis/asset_catalog. Resolve relative to the crate so
    // the command works from any cwd.
    PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("..")
        .join("..")
        .join("ghidra_analysis")
        .join("asset_catalog")
        .join("control-registry.json")
}

fn main() {
    let json = bulanci_core::scene::registry::registry_json_string();

    match std::env::args().nth(1).as_deref() {
        Some("-") => {
            println!("{json}");
        }
        Some(p) => write_to(PathBuf::from(p), &json),
        None => write_to(default_out(), &json),
    }
}

fn write_to(out: PathBuf, json: &str) {
    if let Some(parent) = out.parent() {
        if let Err(e) = std::fs::create_dir_all(parent) {
            eprintln!("dump-registry: cannot create {}: {e}", parent.display());
            std::process::exit(1);
        }
    }
    match std::fs::write(&out, json.as_bytes()) {
        Ok(()) => eprintln!("dump-registry: wrote {} ({} bytes)", out.display(), json.len()),
        Err(e) => {
            eprintln!("dump-registry: failed to write {}: {e}", out.display());
            std::process::exit(1);
        }
    }
}
