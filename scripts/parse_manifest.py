import json

manifest_path = "unpacked/_manifest.json"
with open(manifest_path, "r", encoding="utf-8") as f:
    manifest = json.load(f)

# The manifest is likely a dict or list. Let's check its type and find resources of interest.
print("Manifest type:", type(manifest))

interesting_ids = set(range(65860, 65876)) | {77825, 77827, 77829, 77831}

def scan_item(item):
    if isinstance(item, dict):
        if "id" in item and item["id"] in interesting_ids:
            print(f"\nResource ID {item['id']}:")
            print(json.dumps(item, indent=2))
        for k, v in item.items():
            scan_item(v)
    elif isinstance(item, list):
        for x in item:
            scan_item(x)

scan_item(manifest)
