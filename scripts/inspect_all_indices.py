import json

for i in range(65536, 65541):
    fp = f"unpacked/overlay/res_00000{i}_67_AudioBankIndex.bank.json"
    with open(fp, "r", encoding="utf-8") as f:
        data = json.load(f)
    print(f"Index {i} (0x{i:05x}):")
    print(f"  Bank ID: {data.get('bankResourceID')} (0x{data.get('bankResourceID'):05x})")
    print(f"  Samples count: {len(data.get('samples'))}")
    # Print first few samples info
    for idx, s in enumerate(data.get('samples')[:3]):
        secs = s['byteLen'] / (22050 * 2)
        print(f"    Sample {idx}: offset={s['offsetInBank']}, len={s['byteLen']} ({secs:.3f}s)")
    if len(data.get('samples')) > 3:
        print(f"    ... and {len(data.get('samples')) - 3} more samples")
