import json

with open("unpacked/_manifest.json", "r", encoding="utf-8") as f:
    manifest = json.load(f)

for item in manifest.get("resources", []):
    if item.get("classId") in [43, 67]:
        print(f"ID: {item.get('id')}, Class: {item.get('className')}, Size: {item.get('size')}, File: {item.get('rawFile')}")
