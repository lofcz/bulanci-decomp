import os
import struct
import json
from pathlib import Path
import msgpack

def pack_assets():
    root_dir = Path(__file__).parent.parent
    assets_dir = root_dir / "open_bulanci" / "assets"
    output_pack = root_dir / "open_bulanci" / "assets.pack"
    
    print(f"Packing assets in {assets_dir} into {output_pack}...")
    
    # Collect all files under assets/
    all_files = []
    for root, dirs, files in os.walk(assets_dir):
        for f in files:
            fp = Path(root) / f
            # Calculate path relative to assets_dir
            rel_path = fp.relative_to(assets_dir)
            rel_path_str = str(rel_path).replace("\\", "/") # Normalize to forward slashes
            
            # Check if this is an atlas or cursor composition JSON
            if (rel_path_str.startswith("atlases/") or rel_path_str.startswith("cursor/")) and rel_path_str.endswith(".json"):
                # Transcode from JSON to MsgPack (.bin) for production efficiency
                bin_path_str = rel_path_str[:-5] + ".bin"
                print(f"  Transcoding: {rel_path_str} -> {bin_path_str}")
                
                try:
                    json_data = json.loads(fp.read_text(encoding="utf-8"))
                    bin_data = msgpack.packb(json_data, use_bin_type=True)
                    all_files.append((bin_path_str, bin_data))
                except Exception as e:
                    print(f"Error transcoding {fp}: {e}")
                    raise e
            else:
                # Pack other files normally
                data_bytes = fp.read_bytes()
                all_files.append((rel_path_str, data_bytes))
            
    # Binary pack format:
    #   u32 file_count
    #   repeat file_count times:
    #     u32 rel_path_len
    #     bytes rel_path (utf-8)
    #     u32 file_data_len
    #     bytes file_data
    
    packed_bytes = bytearray()
    # Placeholder for file_count
    packed_bytes.extend(struct.pack("<I", len(all_files)))
    
    for rel_path_str, data_bytes in all_files:
        path_bytes = rel_path_str.encode("utf-8")
        
        packed_bytes.extend(struct.pack("<I", len(path_bytes)))
        packed_bytes.extend(path_bytes)
        packed_bytes.extend(struct.pack("<I", len(data_bytes)))
        packed_bytes.extend(data_bytes)
        
    output_pack.write_bytes(packed_bytes)
    print(f"Packed {len(all_files)} files successfully ({len(packed_bytes)} bytes).")

if __name__ == "__main__":
    pack_assets()
