import glob
import os

for rid in [77825, 77827, 77829, 77831]:
    pattern = f"unpacked/**/res_00000{rid}_*"
    matches = glob.glob(pattern, recursive=True)
    print(f"\nResource ID {rid}:")
    for m in matches:
        print(f"  {m} ({os.path.getsize(m)} bytes)")
