"""Inspect the byte stream that follows the 36-byte DSM header."""
import struct


def main() -> None:
    f = r"unpacked\overlay\res_0000077825_76_BitmapJpegAnim.bin"
    data = open(f, "rb").read()
    end = struct.unpack_from("<I", data, 0)[0]
    w, h, fmt, dur_ms, frames = struct.unpack_from("<5I", data, 4)
    audio_bytes, packed, rate = struct.unpack_from("<3I", data, 24)
    print(f"file: {f}")
    print(f"size: {len(data)}, payloadEnd: {end}")
    print(f"video: {w}x{h} fmt={fmt} N={frames} dur={dur_ms}ms")
    print(f"audio: bytes={audio_bytes} packed=0x{packed:08x} rate={rate}")

    print()
    print("first 64 bytes (after 36-byte header):")
    print(" ".join(f"{b:02x}" for b in data[36:36 + 64]))
    print()
    print("If chunked {u32 len, bytes[len]}, first chunk len:")
    if 36 + 4 <= len(data):
        first_len = struct.unpack_from("<I", data, 36)[0]
        print(f"  len = {first_len} (0x{first_len:08x})")
        print(f"  body first 32 bytes: {' '.join(f'{b:02x}' for b in data[40:40 + 32])}")
        if 40 + first_len <= len(data):
            print(f"  next chunk at file offset 0x{40 + first_len:x}:")
            print(f"  next len = {struct.unpack_from('<I', data, 40 + first_len)[0]}")
        # Walk chunks
        off = 36
        chunks = []
        while off + 4 <= len(data):
            n = struct.unpack_from("<I", data, off)[0]
            if n == 0 or n > len(data) - off - 4:
                break
            body_off = off + 4
            body_end = body_off + n
            body0 = data[body_off:body_off + 4]
            is_jpeg = body0[:3] == b"\xff\xd8\xff"
            chunks.append((off, n, is_jpeg, body0.hex()))
            off = body_end
        print(f"\nWalked {len(chunks)} chunks; last walked offset = 0x{off:x} of 0x{len(data):x}")
        print("first 15 chunks:")
        for c in chunks[:15]:
            print(f"  @0x{c[0]:08x}  len={c[1]:>10d}  jpeg={c[2]}  body0={c[3]}")
        print("last 5 chunks:")
        for c in chunks[-5:]:
            print(f"  @0x{c[0]:08x}  len={c[1]:>10d}  jpeg={c[2]}  body0={c[3]}")
        if chunks:
            jpeg_chunks = [c for c in chunks if c[2]]
            audio_chunks = [c for c in chunks if not c[2]]
            print(f"\njpeg chunks: {len(jpeg_chunks)} (expected ~{frames})")
            print(f"non-jpeg chunks: {len(audio_chunks)}")
            if audio_chunks:
                total_audio = sum(c[1] for c in audio_chunks)
                print(f"total non-jpeg bytes: {total_audio} (expected audio ~{audio_bytes})")
            if jpeg_chunks:
                total_jpeg = sum(c[1] for c in jpeg_chunks)
                print(f"total jpeg bytes: {total_jpeg}")


if __name__ == "__main__":
    main()
