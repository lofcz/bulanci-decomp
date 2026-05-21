use std::env;
use std::fs;
use std::path::Path;

fn main() {
    println!("cargo:rerun-if-changed=../assets.pack");

    let pack_path = Path::new("../assets.pack");
    if pack_path.exists() {
        let raw_data = fs::read(pack_path).expect("failed to read assets.pack");
        let compressed_data = zstd::encode_all(&raw_data[..], 9).expect("failed to compress assets.pack");
        
        let out_dir = env::var_os("OUT_DIR").unwrap();
        let dest_path = Path::new(&out_dir).join("assets.pack.zst");
        fs::write(&dest_path, compressed_data).expect("failed to write compressed assets.pack.zst");
    } else {
        println!("cargo:warning=assets.pack not found, skipping compression");
    }
}
