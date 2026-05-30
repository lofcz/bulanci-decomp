# Struct recovery agent protocol (evidence-only)

## Per-struct deliverable: `ghidra_analysis/engine/struct_recovery/{StructName}.md`

### Required sections

1. **Status**: `VERIFIED` | `PARTIAL` | `UNVERIFIED` | `SKIP` (already sized / CRT stub)
2. **Size proof table**

| Claim | Address | Evidence |
|-------|---------|----------|

3. **Layout table** (only proven offsets)

| Offset | Size | Type | Name | Evidence (func@addr) |

4. **Ghidra apply** (if VERIFIED or PARTIAL with size only): `get_struct_layout` after `create_struct`
5. **UNK** — anything not proven

## Rules

- NO field without ≥1 xref or ctor write + consumer.
- `get_struct_layout` must show size > 1 before apply.
- CRT/EH stubs (`type_info`, `exception`, …): document as SKIP unless game code allocates.
- One `save_program bulanci.exe` per batch at end.
