import os

def list_sound_files(startpath):
    for root, dirs, files in os.walk(startpath):
        for f in files:
            ext = os.path.splitext(f)[1].lower()
            if ext in ['.wav', '.mp3', '.ogg', '.raw']:
                fp = os.path.join(root, f)
                size = os.path.getsize(fp)
                print(f"{fp} ({size} bytes)")

list_sound_files('unpacked')
