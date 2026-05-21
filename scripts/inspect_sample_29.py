import json
m = json.load(open('unpacked/overlay/res_0000065540_67_AudioBankIndex.bank.json', encoding='utf-8'))
sample = m['samples'][29]
print("Sample 29 info:", sample)
print(f"Start in WAV: {44 + sample['offsetInBank']}")
print(f"Length: {sample['byteLen']}")
# Let's print adjacent samples too to see context
for i in (27, 28, 29, 30, 31):
    s = m['samples'][i]
    print(f"Sample {i}: offset={s['offsetInBank']}, len={s['byteLen']} ({s['byteLen'] / 44100 / 2:.3f}s at 44.1k or {s['byteLen'] / 22050 / 2:.3f}s at 22.05k)")
