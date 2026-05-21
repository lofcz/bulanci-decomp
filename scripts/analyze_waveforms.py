import os
import wave
import math

output_root = "unpacked/overlay/extracted_all_samples/bank_65874"

print(f"{'Sample':12} | {'Duration':8} | {'Max Amp':8} | {'RMS':8} | {'Crest Factor':12} | {'Zero Cross':10}")
print("-" * 75)

for i in range(41):
    sample_file = f"sample_{i:02d}.wav"
    sample_path = os.path.join(output_root, sample_file)
    if not os.path.exists(sample_path):
        continue
        
    try:
        with wave.open(sample_path, "rb") as w:
            n_frames = w.getnframes()
            frames_bytes = w.readframes(n_frames)
            rate = w.getframerate()
            duration = n_frames / rate
            
            # Read 16-bit mono PCM
            data = []
            for j in range(0, len(frames_bytes), 2):
                if j + 1 < len(frames_bytes):
                    val = int.from_bytes(frames_bytes[j:j+2], byteorder='little', signed=True)
                    data.append(val)
            
            if not data:
                continue
                
            max_amp = max(abs(val) for val in data)
            sum_sq = sum(val**2 for val in data)
            rms = math.sqrt(sum_sq / len(data))
            crest_factor = max_amp / rms if rms > 0 else 0
            
            # Zero crossings
            zero_crossings = 0
            last_sign = 1 if data[0] >= 0 else -1
            for val in data[1:]:
                sign = 1 if val >= 0 else -1
                if sign != last_sign:
                    zero_crossings += 1
                    last_sign = sign
            
            print(f"{sample_file:12} | {duration:7.3f}s | {max_amp:7.0f} | {rms:7.1f} | {crest_factor:12.2f} | {zero_crossings:10d}")
    except Exception as e:
        print(f"{sample_file:12} | Error: {e}")
