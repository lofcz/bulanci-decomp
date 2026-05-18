# Decompilation pipeline

This document describes how the bulanci-decomp pipeline is wired and how
to regenerate every artifact end-to-end. The setup mirrors
[regratka/mkp](https://decomp.dev/regratka/mkp), with two deliberate
differences explained at the bottom.

## Layout

```
orig/             # original binaries (committed, freeware) + hashes file
config/bulanci/   # source-of-truth CSV configuration for the bulanci unit
src/bulanci/      # stub C++ source emitted by scripts/generate_sources.py
include/bulanci/  # stub C++ headers emitted by the same script
include/globals.h # shared typedefs (uchar/ushort/uint, BOOL, NULL, ...)
build/            # generated target/base .obj files (gitignored)
scripts/          # Python build orchestration
scripts/ghidra/   # Ghidra Java headless scripts
scripts/internal/ # helpers that talk to the Ghidra MCP HTTP API
.github/workflows # CI that uploads report.json as `bulanci_report`
tools/            # local-only third-party tools (objdiff-cli, MSVC, ...)
objdiff.json      # objdiff configuration (generated)
report.json       # objdiff progress report (committed; CI uploads it)
decomp-goal.toml  # decomp-goal-harness adapter entry point
```

## Prerequisites

| Tool                  | How                                                                 |
|-----------------------|---------------------------------------------------------------------|
| `bulanci.exe`         | drop the original binary at `orig/bulanci.exe`                      |
| Python 3.10+          | `python --version`                                                  |
| Ghidra                | install + set `GHIDRA_HOME` so headless scripts can find it         |
| ghidra-mcp plugin     | run a Ghidra GUI with `bulanci.exe` open; the plugin exposes 8089   |
| `tools/objdiff-cli.exe` | download from <https://github.com/encounter/objdiff/releases>     |

Ninja and MSVC are only required once we start matching base objects — the
first decomp.dev report does not need them.

## Regenerate mapping.csv + units_listing.csv

`config/bulanci/mapping.csv` is the source-of-truth function table. It is
byte-identical to what `scripts/ghidra/GenerateMapping.java` would produce
under Ghidra headless, but we use the MCP plugin instead so the running
GUI session is the source of truth:

```bash
python scripts/internal/export_mapping_via_mcp.py
python scripts/internal/seed_units_listing.py
```

The seeder rewrites every file-scope (no `::`) function as
`_Globals::FUN_xxx` and emits a single `_Globals` translation unit, so
the rest of the pipeline has at least one non-empty namespace to bind to.
As Ghidra learns more real namespaces (`CGame`, `CBulanci`, ...), re-run
both scripts and the unit list will grow.

## Regenerate sources / objdiff / ninja

```bash
python scripts/configure.py --skip-ghidra
```

This writes:

* `src/bulanci/<unit>.cpp` and `include/bulanci/<unit>.h` (one per unit)
* `build.ninja` (compile rule for the stubs; uses `tools/msvc/Bin/cl.exe`
  by default — override with `BULANCI_CL=path/to/cl.exe`)
* `objdiff.json` with `build_base: false` and `base_path: null` until a
  base build is wired up

Drop `--skip-ghidra` once you have Ghidra installed and want the COFF
target objects exported via `scripts/ghidra/ExportDelinker.java`. The
exporter runs against a fresh temp project so it does not need you to
close your interactive Ghidra session.

## Regenerate report.json

```bash
progress_update.bat
```

This shells out to `tools/objdiff-cli.exe report generate`. The CI
workflow at `.github/workflows/progress.yml` does **not** rebuild
`report.json`; it only uploads the committed file as the
`bulanci_report` artifact that decomp.dev's GitHub app reads. Keep
`report.json` checked in.

## decomp-goal-harness loop

```bash
uv run decomp-goal run --repo C:\path\to\bulanci --unit _Globals
```

The harness reads `decomp-goal.toml`, which calls `scripts/oracle.py`:

* `--build` reruns `objdiff-cli` and refreshes `report.json`
* `--score --unit U` extracts the unit's measures into the harness's
  `{matched, score, exact_bytes, total_bytes, matching_prefix_bytes,
  matching_prefix_percent, first_mismatch_offset}` schema
* `--diff --unit U` shells out to `objdiff-cli diff` when available, or
  prints a small text summary otherwise

## Differences vs MKP

1. **No vendored MSVC.** MKP keeps a Visual C++ 6.6 toolchain in
   `tools/msvc66/`. Until we confirm which compiler bulanci.exe was
   built with (Rich Header parsing TBD), `objdiff.json` is generated
   with `build_base: false` and `base_path: null` so the first report is
   trivially valid at 0 % progress.
2. **MCP-driven mapping export.** MKP runs `GenerateMapping.java` under
   Ghidra headless, which requires a fresh import + analysis (~minutes).
   We additionally ship
   [`scripts/internal/export_mapping_via_mcp.py`](../scripts/internal/export_mapping_via_mcp.py),
   which talks to the already-running Ghidra MCP plugin and produces the
   same CSV in seconds. The headless `GenerateMapping.java` is kept as a
   no-MCP fallback.
