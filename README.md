# bulanci-decomp

[![Progress](https://decomp.dev/lofcz/bulanci-decomp?mode=shield&format=svg)](https://decomp.dev/lofcz/bulanci-decomp)

WIP matching decompilation of Bulánci — a Czech
freeware game originally distributed for Windows.

The repository contains tooling, configuration, and stub C++ source for
recreating the byte-identical executable from source.

| __File__       | __Progress__ | __Size__      | __SHA1 hash__                              |
|----------------|------------|----------------|--------------------------------------------|
| `bulanci.exe`  | [decomp.dev/lofcz/bulanci-decomp](https://decomp.dev/lofcz/bulanci-decomp) | 22,445,318 B | `cab343aabc006a7ac8aef60901becb8bfd70d875` |

## Quick start

```shell
:: One-time: extract the function table from a running Ghidra MCP session.
python scripts\internal\export_mapping_via_mcp.py
python scripts\internal\seed_units_listing.py

:: Regenerate stub sources, objdiff.json, build.ninja.
python scripts\configure.py --skip-ghidra

:: Regenerate report.json (committed; CI uploads it as bulanci_report).
progress_update.bat
```

See [`docs/DECOMP.md`](docs/DECOMP.md) for the full pipeline.

## Asset unpacker

`tools/bulanci_unpack/` is an independent tool that decompresses the
custom "GZIP"+`.eap` resource container used inside `bulanci.exe`'s
overlay and the `tutorial.eap`/`.eapres` level files. It is not part of
the matching decomp pipeline; see
[`tools/bulanci_unpack/README.md`](tools/bulanci_unpack/README.md).

## decomp-goal-harness

`decomp-goal.toml` wires the project into
[`decomp-goal-harness`](https://github.com/lofcz/decomp-goal-harness)
via `scripts/oracle.py`. Once the harness is installed:

```shell
uv run decomp-goal run --repo C:\path\to\bulanci --unit _Globals
```

## License

The repository's own tooling, scripts, and stub C++ source are
distributed under the MIT license (see `LICENSE`).
