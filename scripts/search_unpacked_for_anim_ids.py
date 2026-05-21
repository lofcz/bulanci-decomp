import os

target_ids = [77825, 77827, 77829, 77831]
target_strs = [str(tid) for tid in target_ids]
target_hexes = [f"{tid:08x}" for tid in target_ids]
target_hexes_short = [f"{tid:x}" for tid in target_ids]

print("Searching for resource ID references in unpacked files...")

for root, dirs, files in os.walk("unpacked"):
    for f in files:
        if f.endswith(".wav") or f.endswith(".mp3") or f.endswith(".jpg") or f.endswith(".png"):
            continue
        fp = os.path.join(root, f)
        try:
            with open(fp, "rb") as file_obj:
                content = file_obj.read()
            
            # Check for decimal string
            for s in target_strs:
                if s.encode('utf-8') in content:
                    print(f"Decimal match: ID {s} in file {fp}")
                    
            # Check for little-endian dword
            for tid in target_ids:
                le_bytes = tid.to_bytes(4, byteorder='little')
                if le_bytes in content:
                    print(f"Little-endian match: ID {tid} (0x{tid:x}) in file {fp}")
                    
        except Exception as e:
            pass
