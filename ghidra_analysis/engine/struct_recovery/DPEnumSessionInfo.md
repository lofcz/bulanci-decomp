# DPEnumSessionInfo

## Status

**PARTIAL** — sparse struct for DirectPlay session-enumeration callback memory; only offsets read by `CSessionItem_Initialize@0x0040e8b0`. Not a full Windows `DPSESSIONDESC2`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Minimum span `0x34` | `0x0040e8b0` | Reads `pSessionNameA` at `+0x30` (4-byte pointer) |
| Ghidra allocated size | — | `get_struct_layout` → **52 bytes** (highest field `+0x30` + pointer) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x08` | 4 | `uint` | `dwHostIp` | `CSessionItem_Initialize@0x0040e8b0` → `CSessionItem.hostIp` |
| `0x0c` | 4 | `uint` | `dwPort` | → `CSessionItem.port` |
| `0x10` | 4 | `uint` | `dwPlayerCount` | → `CSessionItem.playerCount` |
| `0x14` | 4 | `uint` | `dwSessionFlags` | → `CSessionItem.sessionFlags` |
| `0x30` | 4 | `char *` | `pSessionNameA` | MBCS session name for list label |

## Ghidra apply

```
create_struct DPEnumSessionInfo (5 fields)
set_parameter_type CSessionItem_Initialize@0x0040e8b0 param_1 → DPEnumSessionInfo *
get_struct_layout → 52 bytes
```

## UNK

- All bytes outside the table above (including `+0x00..+0x07` before `dwHostIp`).
- Whether the callback buffer is literally `DPSESSIONDESC2` or a Bulanci wrapper struct.
