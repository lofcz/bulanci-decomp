use std::collections::HashMap;
use anyhow::{Result, Context, anyhow};

// Embed the compressed assets pack compiled by build.rs
const COMPRESSED_PACK: &[u8] = include_bytes!(concat!(env!("OUT_DIR"), "/assets.pack.zst"));

pub struct AssetFileSystem {
    files: HashMap<String, Vec<u8>>,
}

impl AssetFileSystem {
    /// Load and decompress the embedded asset pack at runtime.
    pub fn load() -> Result<Self> {
        let decompressed = zstd::decode_all(COMPRESSED_PACK)
            .context("failed to decompress embedded assets pack")?;
        
        let mut files = HashMap::new();
        let mut cursor = 0;
        
        if decompressed.len() < 4 {
            return Err(anyhow!("asset pack truncated (no file count)"));
        }
        
        let file_count = u32::from_le_bytes(decompressed[0..4].try_into().unwrap()) as usize;
        cursor += 4;
        
        for _ in 0..file_count {
            if cursor + 4 > decompressed.len() {
                return Err(anyhow!("asset pack truncated inside header"));
            }
            let path_len = u32::from_le_bytes(decompressed[cursor..cursor+4].try_into().unwrap()) as usize;
            cursor += 4;
            
            if cursor + path_len > decompressed.len() {
                return Err(anyhow!("asset pack truncated inside path bytes"));
            }
            let path_bytes = &decompressed[cursor..cursor+path_len];
            let path_str = std::str::from_utf8(path_bytes)
                .context("invalid utf-8 path in asset pack")?;
            cursor += path_len;
            
            if cursor + 4 > decompressed.len() {
                return Err(anyhow!("asset pack truncated inside data header"));
            }
            let data_len = u32::from_le_bytes(decompressed[cursor..cursor+4].try_into().unwrap()) as usize;
            cursor += 4;
            
            if cursor + data_len > decompressed.len() {
                return Err(anyhow!("asset pack truncated inside file data"));
            }
            let data_bytes = decompressed[cursor..cursor+data_len].to_vec();
            cursor += data_len;
            
            files.insert(path_str.to_string(), data_bytes);
        }
        
        Ok(AssetFileSystem { files })
    }

    /// Read file content by path.
    pub fn read(&self, path: &str) -> Option<&[u8]> {
        self.files.get(path).map(|v| v.as_slice())
    }

    /// Read file content as UTF-8 string.
    pub fn read_to_string(&self, path: &str) -> Option<Result<String>> {
        self.read(path).map(|bytes| {
            std::str::from_utf8(bytes)
                .map(|s| s.to_string())
                .map_err(|e| anyhow!("invalid utf-8 file data: {}", e))
        })
    }

    /// Get all file paths in the virtual file system.
    pub fn list_files(&self) -> impl Iterator<Item = &String> {
        self.files.keys()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_embedded_assets_load() {
        let vfs = AssetFileSystem::load().expect("VFS should load and decompress successfully");
        assert!(!vfs.files.is_empty(), "VFS should contain transpiled Lua level scripts");
        
        // Assert that we can read one of our transpiled Lua files
        let test_script = vfs.read_to_string("levels/res_0000065855_2026_Script.lua");
        assert!(test_script.is_some(), "levels/res_0000065855_2026_Script.lua should exist in VFS");
        let script_code = test_script.unwrap().unwrap();
        assert!(script_code.contains("OnInit"), "Lua level script should contain 'OnInit'");
    }
}
