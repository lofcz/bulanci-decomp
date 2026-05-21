import os
import json
import struct

def make_wav_header(pcm_len, sample_rate=22050, num_channels=1, bits_per_sample=16):
    byte_rate = sample_rate * num_channels * bits_per_sample // 8
    block_align = num_channels * bits_per_sample // 8
    
    header = struct.pack(
        '<4sI4s4sIHHIIHH4sI',
        b'RIFF',
        36 + pcm_len,
        b'WAVE',
        b'fmt ',
        16,
        1, # PCM
        num_channels,
        sample_rate,
        byte_rate,
        block_align,
        bits_per_sample,
        b'data',
        pcm_len
    )
    return header

output_root = "unpacked/overlay/extracted_all_samples"
os.makedirs(output_root, exist_ok=True)

for index_id in range(65536, 65541):
    json_path = f"unpacked/overlay/res_00000{index_id}_67_AudioBankIndex.bank.json"
    if not os.path.exists(json_path):
        print(f"Skipping index {index_id}: JSON not found")
        continue
        
    with open(json_path, "r", encoding="utf-8") as f:
        meta = json.load(f)
        
    bank_id = meta["bankResourceID"]
    samples = meta["samples"]
    
    bank_path = f"unpacked/overlay/res_00000{bank_id}_43_AudioBank.wav"
    if not os.path.exists(bank_path):
        # Check parent folder too
        bank_path = f"unpacked/res_00000{bank_id}_43_AudioBank.wav"
        if not os.path.exists(bank_path):
            print(f"Skipping bank {bank_id}: WAV file not found")
            continue
            
    print(f"Extracting from bank {bank_id} (associated with index {index_id})...")
    bank_bytes = open(bank_path, "rb").read()
    
    bank_dir = os.path.join(output_root, f"bank_{bank_id}")
    os.makedirs(bank_dir, exist_ok=True)
    
    for i, s in enumerate(samples):
        offset = s["offsetInBank"]
        byte_len = s["byteLen"]
        
        # WAV file data chunk starts after the 44-byte header
        # Let's verify that
        pcm_start = 44 + offset
        pcm_end = pcm_start + byte_len
        
        if pcm_end > len(bank_bytes):
            print(f"  Warning: sample {i} goes out of bounds!")
            continue
            
        pcm_data = bank_bytes[pcm_start:pcm_end]
        wav_data = make_wav_header(len(pcm_data)) + pcm_data
        
        out_path = os.path.join(bank_dir, f"sample_{i:02d}.wav")
        with open(out_path, "wb") as out_f:
            out_f.write(wav_data)
            
    print(f"  Successfully extracted {len(samples)} samples to {bank_dir}")
