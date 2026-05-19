# `CDSMpx` ↔ libmad 0.15.1b function map

Matching workbook for the libmad-derived bodies inside `src/bulanci/CDSMpx.cpp`.
Provenance and the cross-section identification are written up in
`mpx_audio_format.md` (§9.2) and `STATUS.md`; this file is the practical
"which libmad function goes where" reference for the ongoing byte-match.

Sources are upstream libmad 0.15.1b (`ref/libmad-0.15.1b/`). Each row lists
the bulanci.exe address, the matching upstream file/symbol, the recovered
size in bytes, and current matching status.

Status legend:

* **MATCHED** -- hand-ported into the `// !FUNC` block; objdiff byte-matches once
  the surrounding rename / pipeline pass has propagated.
* **PORT-READY** -- straight 1:1 with upstream; small, low-risk to drop in.
* **NEEDS-STRUCT** -- depends on `struct mad_stream` / `mad_frame` / `mad_synth`
  layout being declared first (offsets recovered, but not pushed to Ghidra
  yet).
* **WRAPPER** -- bulanci-specific glue around libmad calls (not libmad code).
* **DRIVER** -- top-level Layer I/II/III decode functions; large but
  mechanically faithful to upstream.

## bit.c (5 functions)

| addr | size | upstream | status | objdiff match |
|------|-----:|----------|--------|---------------|
| `0x00459650` | 23 B | `bit.c::mad_bit_init` | **PORTED** | **83.3%** -- `xor al, al` pre-ret synthesised by delinker on the right side but MSVC8 elided ours; cosmetic |
| `0x00459670` | 32 B | `bit.c::mad_bit_length` | NOT-IN-CSV (created in Ghidra this session; needs pipeline refresh) | -- |
| `0x00459690` | 17 B | `bit.c::mad_bit_nextbyte` | **PORTED** | **97.5%** -- only diff is `ecx` vs `eax` for the temp pointer register; MSVC8 reg-alloc choice |
| `0x004596b0` | 64 B | `bit.c::mad_bit_skip` | PORT-READY | -- |
| `0x004596f0` | 192 B | `bit.c::mad_bit_read` | PORT-READY | -- |
| `0x004597b0` | 335 B | `bit.c::mad_bit_crc` | PORT-READY (depends on `g_awMadCrcTable @ 0x0048b938`) | -- |

`bit.c` also exports the CRC lookup table `crc_table[256]` -- that's
`g_awMadCrcTable` at `0x0048b938` in the binary, byte-for-byte. It is
emitted by the .rdata side of the linker, not by the code TU.

### First-port observations

* **Build pipeline works end-to-end.** `ninja build\Src\bulanci\CDSMpx.obj` -> `objdiff-cli diff --unit bulanci/CDSMpx` gives JSON match scores per symbol.
* **Q3.28/SSO build-flag inferences hold.** Neither of the two ported functions touches fixed-point math, but the calling convention (`__cdecl`, args on stack at `[esp+4]`/`[esp+8]`) and the prologue-less leaf-function shape are both consistent with `/O2 /Gd` + no security-check + no frame pointer.
* **Two friction sources to budget for:**
  1. **Register choice mismatches** (the `ecx`-vs-`eax` thing in `mad_bit_nextbyte`). Possible mitigations: declare `param_1` as `__thiscall` (would force `ecx`) -- but that lies about the binary; OR insert a local that takes EAX so the compiler reaches for ECX next -- brittle. Practical answer: accept ~97% for tiny leaves where the function is shorter than the register-juggling preamble.
  2. **Delinker prototype-driven synthesis** (the trailing `xor al, al` for a `uchar`-returning function whose body has no `return` in the binary). If we change the Ghidra prototype to `void` for these no-op-return helpers, delinker stops emitting the synthetic xor and our `return 0`-less void bodies match exactly. Worth doing as a sweep.

## frame.c (top-level frame parsing)

| addr | size | upstream | status |
|------|-----:|----------|--------|
| `0x00456850` | 83 B | `frame.c::mad_stream_sync` (substring scan for 11-bit sync word) | PORT-READY |
| `0x00458f50` | 472 B | `frame.c::mad_header_decode` | PORT-READY (depends on `g_dwMpegBitrateTable @ 0x0048b7b4`, `g_dwMpegSampleRateTable @ 0x0048b91c`) |
| `0x00458e10` | 150 B | `frame.c::decode_frame` (per-layer dispatcher; calls `g_pfnMpegLayerDecoder`) | PORT-READY |
| `0x00458eb0` | 44 B  | `frame.c::mad_frame_init` (or `mad_header_init`; tiny init helper) | PORT-READY |
| `0x00458ee0` | 56 B  | `frame.c::mad_header_init` (zero-fill `mad_header`) | PORT-READY |

The bitrate / sample-rate tables are the .rdata constants at
`g_dwMpegBitrateTable[60]` and `g_dwMpegSampleRateTable[3]` -- both
already typed in Ghidra.

## layer12.c (Layer I/II)

| addr | size | upstream | status |
|------|-----:|----------|--------|
| `0x0045c480` | 96 B  | `layer12.c::I_sample` | PORT-READY (uses `g_anMadLinearTable @ 0x0049c310`) |
| `0x0045c4e0` | 783 B | `layer12.c::mad_layer_I` | DRIVER |
| `0x0045c800` | 192 B | `layer12.c::II_samples` | PORT-READY (uses `g_anMadSfTable`) |
| `0x0045c8c0` | 1642 B | `layer12.c::mad_layer_II` | DRIVER (uses `sbquant_table[5]`, `bitalloc_table[8]`, `offset_table[6][15]`, `qc_table[17]`) |

The Layer II in-binary tables (sbquant / bitalloc / offset / qc) should
appear adjacent to the bitrate/sample-rate tables in .rdata. They have
not been individually located yet -- `mad_layer_II` references them
indirectly so once `mad_layer_II` is ported, the references will fall
out automatically as `&DAT_*` patches.

## layer3.c (Layer III)

| addr | size | upstream | status |
|------|-----:|----------|--------|
| `0x0045c100` | 895 B | `layer3.c::mad_layer_III` | DRIVER (depends on huffman.c tables, `imdct_l_arm.S`/imdct_l C, sf_band table) |
| `0x0045cf80` | 21 B  | `layer3.c::III_sideinfo` helper (tiny scalar) | PORT-READY |
| `0x0045cfa0` | 32 B  | `layer3.c::III_scalefactors_lsf` (LSF scalefactor stub) | PORT-READY |
| `0x0045cfc0` | 106 B | `layer3.c::III_requantize` (`pow(2, ...) * (sample)^(4/3)` chain) | PORT-READY (uses `rq_table.dat` -- the 485 KB requantization table at one of the larger .rdata blobs in bulanci.exe; locate during port) |
| `0x0045d030` | 310 B | `layer3.c::III_decode` (Huffman + dequant + reorder + alias-reduction) | DRIVER |

`layer3.c` is by far the heaviest of the upstream files (70 KB source).
Most of its volume is huffman decoders that index `huffman.c` tables --
once the tables are declared and the dispatch shape is matched, the
bodies should byte-match without case-by-case analysis.

## synth.c (polyphase synthesis)

| addr | size | upstream | status |
|------|-----:|----------|--------|
| `0x00456910` | 4481 B | `synth.c::dct32` (fully-unrolled 32-point) | DRIVER (uses `g_anMadDct32Costab @ 0x0048af70`) |
| `0x00457aa0` | 2472 B | `synth.c::synth_full` (stereo windowing+downsample, calls dct32) | DRIVER (uses `D[17][32]` -- the windowing constants table) |
| `0x00458450` | 2492 B | `synth.c::synth_half` (mono / half-rate, calls dct32) | DRIVER |
| `0x00446670` | 41 B  | `synth.c::audio_linear_round` (`mad_fixed_t` -> `s16` clamp; bulanci-specific shift `>> 13` is the SSO output stage) | PORT-READY |

`D[17][32]` is the 16-entry windowing-coefficient table from
`D.dat` (26 KB upstream); it lives in bulanci's .rdata adjacent to the
cosine matrix. Not located by address yet -- a `set_global` pass after
the synth.c port will pin it.

## stream.c (input bitstream wrapper)

| addr | size | upstream | status |
|------|-----:|----------|--------|
| `0x004465e0` | 55 B | `stream.c::mad_stream_init` (zero/init the input state) | PORT-READY |
| `0x00446640` | 37 B | `stream.c::mad_stream_finish` (free `main_data` + `next_frame`) | PORT-READY |
| `0x00446a00` | 223 B | `stream.c::mad_stream_buffer` (= bulanci's `AttachBitstream`) | PORT-READY |
| `0x00459500` | 155 B | `stream.c::mad_stream_decode_frame` (one frame end-to-end) | DRIVER |
| `0x004595a0` | 106 B | `stream.c::mad_stream_skip` or `mad_stream_sync` helper | PORT-READY |
| `0x00459620` | 33 B | `stream.c` getter (small) | PORT-READY |

## decoder.c (top-level decoder driver) -- BULANCI-SPECIFIC WRAPPERS

The `decoder.c`-equivalents in bulanci are written by the original
authors as bulanci-specific glue around the libmad API; they wrap
`mad_decoder_run` / `mad_synth_frame` behind `CDSMpx`'s
`IDSAudioSource` and ring-buffer interface.

| addr | size | upstream-equivalent | status |
|------|-----:|---------------------|--------|
| `0x00446770` | 303 B | wraps `mad_decoder_run` inner loop | WRAPPER |
| `0x00446940` | 85 B  | bulanci `CDSMpx::ReadPCM` (`IDSAudioSource` API)  | WRAPPER |
| `0x004469a0` | 24 B  | `ReadPCM` thunk through instance pointer | WRAPPER |
| `0x00446b00` | 136 B | factory: `mad_decoder_init` + wiring   | WRAPPER |
| `0x00446b90` | 105 B | `CDSMpx::ResolveResource` (DirectSound gate) | WRAPPER (not libmad) |
| `0x004466c0` | 176 B | bulanci-specific input refill (calls `mad_stream_buffer` style) | WRAPPER |
| `0x004466a0` | 26 B  | tiny accessor                          | WRAPPER |

## frame structure I/O -- BULANCI-SPECIFIC `.mpx` WRAPPER

These live in `CDSMpxStream.cpp` and `CDSMpx.cpp`; they save/load the
16-byte WAVEFORMAT-style header around the raw MPEG audio bitstream.
**Not libmad code** -- they are bulanci's own container format. See
`mpx_audio_format.md` §4.

| addr | size | role |
|------|-----:|------|
| `0x00432eb0` | (CDSMpxStream) `SaveMpxFile` |
| `0x00433180` | (CDSMpxStream) `LoadMpxFile` |
| `0x0043a000` | `ComputeDurationMs` (player-side WAV duration) |

## Constant tables (.rdata, all bit-identical to libmad)

| addr | size | upstream | status |
|------|-----:|----------|--------|
| `0x0048af70` | 124 B | `synth.c::costab1..costab31` (Q3.28) | typed in Ghidra: `g_anMadDct32Costab` |
| `0x0048b938` | 512 B | `bit.c::crc_table[256]` (ushort) | typed: `g_awMadCrcTable` |
| `0x0049c210` | 256 B | `sf_table.dat` (`sf_table[64]`, Q3.28) | typed: `g_anMadSfTable` |
| `0x0049c310` | 56 B  | `layer12.c::linear_table[14]` (Q3.28) | typed: `g_anMadLinearTable` |
| `0x0048b7b4` | 240 B | bitrate table | typed: `g_dwMpegBitrateTable` |
| `0x0048b91c` | 12 B  | sample-rate table | typed: `g_dwMpegSampleRateTable` |
| ... | ... | `qc_table.dat` (17 × 12 B), `D[17][32]`, `rq_table.dat` (485 KB), `imdct_s.dat` -- still to locate |

## Build flags (best guess; verify with first match)

`FPM_DEFAULT|FPM_64BIT + OPT_SSO + !OPT_DCTO`. Locked down by:

* the `(hi << 4) | (lo >> 28)` mad_f_mul retrieve idiom -> 64-bit path
* the `>> 13` audio-linear-round shift -> OPT_SSO active
* cosine constants in `g_anMadDct32Costab` match the non-OPT_DCTO
  `costab1..31` -> OPT_DCTO inactive

Compiled with MSVC 8 / `cl 14.00.50727` `/O2` (project default).

## Workflow for the next byte-match

1. Pick a row above with status **PORT-READY** (start with the smallest,
   e.g. `0x004596b0 = mad_bit_skip` = 64 bytes).
2. Open `ref/libmad-0.15.1b/<file>.c` and grab the upstream function.
3. Open `src/bulanci/CDSMpx.cpp`, find the `// !FUNC <addr> BEGIN/END`
   block, replace `STUB_BODY()` with the upstream body. Keep the
   existing `FUN_*` symbol name (the project's pipeline owns renaming);
   leave a comment naming the upstream function. See the two already-
   matched bodies (`0x00459650`, `0x00459690`) for the pattern.
4. `tools\ninja\ninja.exe` -> `tools\objdiff-cli.exe diff --project . --unit bulanci/CDSMpx ...`
5. If not byte-matching, diff the recovered prototype against libmad's --
   most non-matches at this point will be (a) struct field offsets the
   compiler can't infer from `int*`/`uint*`, or (b) OPT_SSO / OPT_DCTO
   flag mismatches in the synthesis path.

The smallest-bodies-first ordering (`mad_bit_nextbyte` -> `mad_bit_init`
-> `mad_bit_skip` -> ...) is deliberate: each successful port confirms
one more aspect of the build configuration (struct layout, calling
convention, register allocation) without risking a cascade of
mismatches.
