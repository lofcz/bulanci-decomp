# CDPEnumSessionInfo

## Status

**PARTIAL** — not a game class; typed view of the DirectPlay enumeration buffer passed to `CSessionItem_Initialize`. Only offsets touched by that function are defined. **Not** a full `DPSESSIONDESC2` layout (no `guidInstance` field split at `+0x08` in consumer code).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Minimum span through name pointer | `0x0040e8b0` | `CSessionItem_Initialize` reads dwords at `+0x08..+0x14` and `*(char **)(param_1 + 0x30)` |
| Ghidra struct size | — | `get_struct_layout` → **52 bytes** (padded to `lpszSessionNameA` @ `0x30`) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 8 | — | `pad_00` | No reads in `CSessionItem_Initialize` |
| `0x08` | 4 | `uint` | `hostIp` | `CSessionItem_Initialize@0x0040e8b0` → `this->hostIp` |
| `0x0C` | 4 | `uint` | `port` | same → `this->port` |
| `0x10` | 4 | `uint` | `playerCount` | same → `this->playerCount` |
| `0x14` | 4 | `uint` | `sessionFlags` | same → `this->sessionFlags` |
| `0x18` | 16 | — | `pad_18` | No reads in initializer |
| `0x30` | 4 | `char *` | `lpszSessionNameA` | MBCS session name → list label (`MultiByteToWideChar` path) |

## Ghidra apply

```
create_struct CDPEnumSessionInfo (batch 18 follow-up) — 52 B with pad_00 / pad_18
```

**Note:** Decompiler and `CSessionItem_Initialize` parameter type use the sibling sparse struct **`DPEnumSessionInfo`** (`dwHostIp`, `pSessionNameA` @ `0x30`) — see [DPEnumSessionInfo.md](./DPEnumSessionInfo.md). Both layouts share proven offsets `+0x08..+0x14` and `+0x30`.

## UNK

- Bytes `0x00..0x07` and `0x18..0x2F` (likely `DPSESSIONDESC2` header fields; not consumed by Bulanci row builder).
- Whether `+0x08` is host IPv4 vs part of a 16-byte instance key (row compare uses copied dwords at `CSessionItem+0x14`, not a separate GUID field).
