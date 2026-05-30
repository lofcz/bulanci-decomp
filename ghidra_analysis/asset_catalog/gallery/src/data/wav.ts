// In-browser port of `serve.py::_slice_wav_sample`.
//
// Each AudioBank ships as one big PCM-WAV; the bank's index gives
// `(offsetInBank, byteLen)` per voice.  To play an individual sample
// the gallery used to ask the Python server to slice the file; now we
// fetch the bank bytes (cached at the DataSource level), parse the
// RIFF chunks once, then build a fresh standalone WAV for each slice.

interface RiffInfo {
  fmtChunk:   Uint8Array;     // raw "fmt " chunk including header (8 + size)
  dataStart:  number;         // offset of the first PCM byte in the source
}

/** Parse the source bank's RIFF header.  Returns the verbatim `fmt `
 *  chunk (we copy it untouched into the output so sample rate / channels
 *  / bit-depth all line up) plus the absolute offset of the first PCM
 *  byte in the source buffer. */
function parseRiff(buf: Uint8Array): RiffInfo {
  const dv = new DataView(buf.buffer, buf.byteOffset, buf.byteLength);
  // 0..4 = "RIFF",  4..8 = file size,  8..12 = "WAVE"
  if (buf.byteLength < 12
      || readAscii(buf, 0, 4)  !== "RIFF"
      || readAscii(buf, 8, 4)  !== "WAVE") {
    throw new Error("wav: not a RIFF/WAVE file");
  }
  let fmtChunk: Uint8Array | null = null;
  let dataStart = -1;
  let pos = 12;
  while (pos + 8 <= buf.byteLength) {
    const tag      = readAscii(buf, pos, 4);
    const chunkLen = dv.getUint32(pos + 4, true);
    const payload  = pos + 8;
    if (tag === "fmt ") {
      fmtChunk = buf.subarray(pos, payload + chunkLen);
    } else if (tag === "data") {
      dataStart = payload;
      break;
    }
    pos = payload + chunkLen + (chunkLen & 1);    // 2-byte alignment per RIFF spec
  }
  if (!fmtChunk || dataStart < 0) {
    throw new Error("wav: missing fmt or data chunk");
  }
  return { fmtChunk, dataStart };
}

function readAscii(buf: Uint8Array, offset: number, len: number): string {
  return String.fromCharCode(...buf.subarray(offset, offset + len));
}

/** Build a standalone WAV containing `bankBytes[dataStart+offset .. +length]`.
 *  Returns a new `Uint8Array` you can wrap with `new Blob([…], { type: "audio/wav" })`. */
export function sliceWavSample(bankBytes: Uint8Array, offsetInData: number, length: number): Uint8Array {
  const { fmtChunk, dataStart } = parseRiff(bankBytes);
  const pcmStart = dataStart + offsetInData;
  const pcmEnd   = pcmStart + length;
  if (pcmEnd > bankBytes.byteLength) {
    throw new Error(`wav: slice runs past end of bank (wanted ${length} bytes at +${offsetInData}, have ${bankBytes.byteLength - dataStart})`);
  }
  const pcm = bankBytes.subarray(pcmStart, pcmEnd);

  // header sizes:
  //   12 (RIFF/size/WAVE)  +  fmtChunk.length  +  8 ("data"/size)  +  pcm.length
  const total = 12 + fmtChunk.length + 8 + pcm.length;
  const out   = new Uint8Array(total);
  const dv    = new DataView(out.buffer);

  // RIFF header
  writeAscii(out,  0, "RIFF");
  dv.setUint32(    4, total - 8, true);
  writeAscii(out,  8, "WAVE");

  // fmt chunk (copied verbatim from the bank)
  out.set(fmtChunk, 12);

  // data chunk
  const dataHdr = 12 + fmtChunk.length;
  writeAscii(out, dataHdr,     "data");
  dv.setUint32(   dataHdr + 4, pcm.length, true);
  out.set(pcm,    dataHdr + 8);

  return out;
}

function writeAscii(buf: Uint8Array, offset: number, str: string): void {
  for (let i = 0; i < str.length; i++) buf[offset + i] = str.charCodeAt(i);
}
