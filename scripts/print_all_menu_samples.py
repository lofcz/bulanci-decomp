import json

json_path = "unpacked/overlay/res_0000065540_67_AudioBankIndex.bank.json"
with open(json_path, "r", encoding="utf-8") as f:
    data = json.load(f)

print(f"Total samples: {len(data['samples'])}")
for idx, s in enumerate(data['samples']):
    # Duration in ms (at 22050 Hz Mono 16-bit, so 2 bytes per sample, 44100 bytes per second)
    duration_ms = (s['byteLen'] / 44100) * 1000
    print(f"Index {idx} (0x{idx:02x}): offset={s['offsetInBank']}, len={s['byteLen']} ({duration_ms:.1f} ms)")
