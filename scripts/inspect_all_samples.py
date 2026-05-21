import os
import wave

output_root = "unpacked/overlay/extracted_all_samples"

for bank_dir in sorted(os.listdir(output_root)):
    bank_path = os.path.join(output_root, bank_dir)
    if not os.path.isdir(bank_path):
        continue
        
    print(f"\n{bank_dir}:")
    for sample_file in sorted(os.listdir(bank_path)):
        if not sample_file.endswith(".wav"):
            continue
        sample_path = os.path.join(bank_path, sample_file)
        
        try:
            with wave.open(sample_path, "rb") as w:
                frames = w.getnframes()
                rate = w.getframerate()
                duration = frames / rate
                channels = w.getnchannels()
                width = w.getsampwidth()
                size_bytes = os.path.getsize(sample_path)
                print(f"  {sample_file}: duration={duration:.3f}s, rate={rate}Hz, channels={channels}, width={width} bytes, size={size_bytes} bytes")
        except Exception as e:
            print(f"  Error reading {sample_file}: {e}")
