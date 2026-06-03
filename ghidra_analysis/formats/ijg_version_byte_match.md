# IJG v6 / v6a / v6b byte-match experiment (MSVC8)

**Date:** 2026-06-03  
**PE:** `orig/bulanci.exe`  
**Toolchain:** `tools/msvc8/Bin/cl.exe` 14.00.50727.42 (SleepTeam-era VC8 RTM)  
**Sources:** `C:\Users\mstagl-dev\Documents\GitHub\libjpeg` (kornelski/libjpeg), commits `0baf670` / `16c9714` / `41f55c0`  
**Script:** `ghidra_analysis/scripts/verify_ijg_byte_match.py`

## Method

1. `git archive` each IJG commit into `ghidra_analysis/build/ijg_verify/{v6,v6a,v6b}/`.
2. Use **same** `jconfig.vc` → `jconfig.h` (6b Win32 config) for all three so diffs are **source-only**.
3. Compile probe `.c` files with `cl /nologo /c /O2 /I.` (matches decomp stub flags in `generate_ninja.py`).
4. Extract PE `.text` at known VAs; compare to COFF `.text` symbols via `scripts/internal/near_miss.py`.
5. Port bodies into `src/bulanci/_Globals.cpp` and score with `tools/objdiff-cli.exe --unit bulanci/_Globals` (see `scripts/_tmp_inspect_jpeg_create.py`). Target COFF symbols follow Ghidra export names until `configure.py` re-runs ExportDelinker after renames.

## Results (honest)

### RoData — all three versions

| Artifact | v6 | v6a | v6b | PE |
|----------|----|----|-----|-----|
| `jpeg_natural_order[80]` @ `0x0049db50` | MATCH | MATCH | MATCH | reference |

Cannot discriminate 6/6a/6b from this table alone (identical since IJG v6).

### API entry points — version immediate in machine code

| Function | PE bytes | v6a `_jpeg_Create*` | v6b `_jpeg_Create*` |
|----------|----------|---------------------|---------------------|
| `jpeg_CreateDecompress` @ `0x0045e6a0` | 231 | 228 B, **`CMP EAX, 0x3d` (61)** @ +0xF | 242 B, **`CMP EAX, 0x3e` (62)** @ +0xF |
| `jpeg_CreateCompress` @ `0x0045ecc0` | 190 | 187 B, **`CMP EAX, 0x3d` (61)** | 201 B, **`CMP EAX, 0x3e` (62)** |

PE disasm @ `0x0045e6ad` / `0x0045ecc0`: **`83 F8 3E`** → requires **62 / 6b**, not 61 / 6a.

v6 has **`_jpeg_create_decompress`** only (no `jpeg_Create*`) → wrong API era.

**No EXACT byte match** for either 6a or 6b Create* under `/O2` (stack frame / prologue differs: e.g. 6b adds `PUSH EDI` early). Closest **semantic** fit is **6b** because of the **0x3e** guard.

### Outlined locals (`pre_process_data`, `sep_downsample`, …)

MSVC `/O2` compiles each `.c` to **one public symbol** (`_jinit_downsampler`, `_jinit_c_prep_controller`, …) and **inlines** `METHODDEF` bodies. PE keeps many of these as **separate outlined functions** (`FUN_0046a870`, `FUN_0046adf0`, …).

Subobject search: PE bytes for `sep_downsample` / `h2v2_downsample` / `pre_process_data` **do not appear** inside compiled `jcsample.obj` / `jcprepct.obj` `.text` at all.

So **per-function byte match against single-file `/O2` objects cannot work** for those probes without:

- compiling a **whole static library** and matching linked `.lib`, or
- porting bodies into `src/bulanci/_Globals.cpp` stubs and using the project's **objdiff** pipeline (`scripts/_tmp_inspect_jpeg_create.py`; re-export `build/orig` after Ghidra renames).

### `jinit_phuff_encoder` @ `0x00469900`

| Version | Compiled size | vs PE 63 B |
|---------|---------------|-----------|
| v6 / v6a / v6b | 63 B each | ~30–43% prefix match only (not EXACT) |

## Verdict (skeptical)

| Claim | Supported? |
|-------|------------|
| Not plain **v6** | **Yes** — `jpeg_Create*` + version 62 |
| Not **v6a** error/API numbering | **Yes** — PE uses JERR slots **0xC / 0x15** and **0x3E** version check (= 6b table, not 6a **0xA / 0x13 / 0x3D**) |
| **6b** best release fit | **Yes** for guards + rodata; **not** proven as byte-identical tarball build |
| Every labeled `FUN_*` = stock 6b symbol | **No** — outlined splits + no COFF match in this experiment |

**Do not treat “6b” as “every function matched.”** Treat it as: **binary was built from 6b-era IJG headers and mostly 6b source**, with SleepTeam/MSVC customization (outlining, stripped strings, custom error manager).

## Re-run

```powershell
C:\Python314\python.exe ghidra_analysis\scripts\verify_ijg_byte_match.py
```

Requires `orig\bulanci.exe`, `tools\msvc8\Bin\cl.exe`, and libjpeg git repo.
