# Logic analysis workspace

Pre-match reverse-engineering of `bulanci.exe` subsystems. Use this
folder when the work is **"understand how this thing works"** rather
than **"produce byte-exact COFF for symbol X"** (the latter belongs in
`src/bulanci/`, see `docs/DECOMP.md`).

`STATUS.md` is the canonical tracker — update it whenever you raise
or lower the confidence of a subsystem so the next agent can pick up
where you left off.

## File conventions

| Pattern | What it is | Committed? |
|---|---|---|
| `<component>.md` | Write-up: byte layouts, dispatch tables, callsite maps, semantic names. | Yes |
| `<component>.py` / `<component>_<step>.py` | Repeatable analysis script (parses an `.eap`, dumps a header, audits factory tables...). | Yes |
| `<component>.{txt,json,csv}` | Auto-generated data dump emitted by one of the `.py` files. Commit when the schema is stable. | Yes |
| `_scratch_*.{py,md,txt}` | Short-lived notebook / one-shot debug script. Promote to a real name once stable. | **No** (gitignored) |

Keep prose tight. Prefer:

* tables for byte/field layouts and dispatch maps,
* fenced asm/C snippets for the actual ground truth,
* one address per artefact (`FUN_00438c40`, `DAT_004b3b20`) so other
  agents can find it in Ghidra.

## Add a new analysis

1. Pick a `<component>` name (e.g. `net_protocol`, `eap_packing`,
   `audio_bank`, `save_format`).
2. Drop findings into `<component>.md`. Reference exact addresses
   from `bulanci.exe`.
3. Commit any repeatable extraction logic as `<component>.py`. Use the
   unpacker (`tools/bulanci_unpack/bulanci_unpack.py`) for resource
   payloads; use Ghidra MCP (`http://localhost:8089`) for code/data
   queries.
4. Add or update the row in `STATUS.md`.

## Handoff to the match track

A subsystem is ready to drive matches when its `STATUS.md` row reads
**Verified**. Then:

1. **Push types/sigs into Ghidra** — set accurate prototypes via MCP
   `set_function_prototype`, file struct layouts into a Ghidra GDT or
   manual structs.
2. **Re-export** — close Ghidra, `python scripts/configure.py`, so
   `config/bulanci/mapping.csv` reflects the new types.
3. **Match the functions** named in the write-up via the workflow in
   `docs/DECOMP.md`.

## Existing artefacts

See `STATUS.md`.
