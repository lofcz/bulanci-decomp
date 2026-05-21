import json

m = json.load(open('unpacked/overlay/res_0000065540_67_AudioBankIndex.bank.json', encoding='utf-8'))
samples = m['samples']

print(f"{'Index':5} | {'Offset':8} | {'ByteLen':8} | {'Secs (22kHz)':12} | {'Comment/Role':20}")
print("-" * 70)

# We know:
# Slot 24: "Historie"
# Slot 25: "Konec"
# Slot 26: "Konec hry"
# Slot 27: "Start hry"
# Slot 28: alternate exit
# Slot 29: CLevelScore ctor (lobby startup)
# Slot 30: Volume slider click

for i, s in enumerate(samples):
    offset = s['offsetInBank']
    byte_len = s['byteLen']
    secs = byte_len / (22050 * 2)
    role = ""
    if i == 24: role = "History click ('Historie')"
    elif i == 25: role = "Quit click ('Konec')"
    elif i == 26: role = "X hotkey ('Konec hry')"
    elif i == 27: role = "Start click ('Start hry')"
    elif i == 28: role = "Alt exit / click"
    elif i == 29: role = "CLevelScore::ctor (Lobby)"
    elif i == 30: role = "Volume slider click"
    print(f"{i:5d} | {offset:8d} | {byte_len:8d} | {secs:10.3f}s | {role}")
