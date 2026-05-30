# CTcpIpConfig

## Status

**PARTIAL** — object size `0x70` (`CWindow` dialog base) verified; ctor builds four heap children via `CDSView__AddChild`. No tail fields past `CWindow`. Window **client** size in ctor is `312×160` (`0x138×0xa0`), not object size.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CTcpIpConfig) == 0x70` | `_Globals::CreateObject@0x00472464` | `OperatorNewWithBadAlloc(0x70)` → `CTcpIpConfig::CTcpIpConfig` |
| Stack instance `0x70` | `CMenu_OpenNetworkSession@0x00414dd0` | `undefined1 local_4174[112]`; placement `CTcpIpConfig(local_4174)`; `CWindow_dtor(local_4174)` |
| Base layout | `CTcpIpConfig@0x0040c060` | `CWindow::CWindow_BuildAt(this,0,0,0x138,0xa0,1)` then four vtable writes |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00–0x6B | 0x6C | `CWindow` | *(base through chain band)* | Same field map as `CWindow` — `get_struct_layout CWindow` / `CTcpIpConfig` (112 B) |
| 0x40 | 4 | `dword` | `dwChainHead_40` | `CDSChained_ResetChainCounters`; dialog chain band |
| 0x44 | 2 | `ushort` | `wViewStateFlags` | same |
| 0x46 | 2 | `ushort` | `wWidgetFlags` | ctor `\|= 8` after bbox shift |
| 0x48 | 2 | `ushort` | `wChainCounter_48` | `ResetChainCounters` |
| 0x4A | 2 | `ushort` | `wChainCounter_4a` | same |
| 0x4C | 4 | `void *` | `pParent` | chain band |
| 0x50 | 4 | `dword` | `dwChainField_50` | same |
| 0x54–0x63 | 0x10 | *(embedded `CDSChain` MI + pads)* | — | `pVftable_CDSChain_*`, `dwField_5c`…`64` |
| 0x68 | 1 | `byte` | `bModalFlag` | `CWindow` modal tail (not CTcpIp-specific) |
| 0x6C | 4 | `void *` | `pDefaultFocusChild` | `CWindow` tail |
| — | — | — | *(no extension past 0x70)* | Ctor only `AddChild` for heap controls; no stores past `sizeof(CWindow)` |

### Ctor side effects (not stored in `this`)

| Child | Alloc | Build | Evidence |
|-------|-------|-------|----------|
| Prompt `CStaticText` | `OperatorNew(0x98)` | `(20,20,0x124,0x3c)` font `0x100ae`, pool string @ `g_apCDSStaticTextsSingleton[2]+0x7c` | `CTcpIpConfig@0x0040c060` |
| Host `CEdit` | `OperatorNew(0xb8)` | `(20,70,0x124,0x3c)` maxlen `0x12`, font `0x100af` | same |
| OK `CButton` | `OperatorNew(0x98)` | pool `+0x74` (index 29) | same |
| Cancel `CButton` | `OperatorNew(0x98)` | pool `+0x78` (index 30) | same |

Post-build window shift (screen placement, not extra fields):

```
*(this+0x2c) -= *(this+0x24);
*(this+0x28) = (*(this+0x28) - *(this+0x20)) + 300;
*(this+0x20) = 300; *(this+0x24) = 0;
*(ushort*)(this+0x46) |= 8;
```

(`main_menu.md` §5.4, `lobby_ui.md` §2.A)

## Vtable cluster

| Face | Address | Notes |
|------|---------|-------|
| primary | `0x004812a4` | `GetClassMeta` slot 0; `CWindow_vDtor` slot 1 (`master_vtable_catalog.csv`) |
| `IDSChained` ×2 | `0x0048126c`, `0x00481284` | standard window chain faces |

## Lifecycle

| Path | Evidence |
|------|----------|
| Join / TCP/IP | `CMenu_OpenNetworkSession` join + `field_0x65==0` → modal `CTcpIpConfig` on stack → `CDSView_SaveData` → `CDSDirectPlay_ConnectTCP` |
| CDS factory | `_Globals::CreateObject@0x00472451` — heap `0x70` for class registry (no in-game `Build` xrefs beyond ctor) |

## Ghidra apply

```
delete_data_type CTcpIpConfig
create_struct CTcpIpConfig — mirror CWindow fields (size 0x70)
set_function_prototype 0x0040c060 → void CTcpIpConfig(CTcpIpConfig *this)
get_struct_layout CTcpIpConfig → size 112 (0x70); mirrors CWindow
set_function_prototype CTcpIpConfig@0x0040c060 → void CTcpIpConfig(CTcpIpConfig *this)
save_program bulanci.exe
```

Applied 2026-05-30 slice 20.

**Agent todo 21 (2026-05-30):** Filled chain-band gap @ `+0x40..+0x4a` (`dwChainHead_40`, `wViewStateFlags`, `wWidgetFlags`, `wChainCounter_48`/`4a`); renamed `dwParent`→`pParent`, `pPad_50`→`dwChainField_50`. `set_decompiler_comment@0x0040c060`. `save_program`.

## UNK

- Whether Ghidra should typedef `CTcpIpConfig` as `extends CWindow` vs duplicated field list (today: duplicate `CWindow` layout for decompiler `this` typing).
- Which heap child owns the saved IP string consumed by `CDSView_SaveData` after modal (walk child chain — not a `CTcpIpConfig` member offset).
- ClassId / `ClassRegEntry` blob address (factory `CreateObject` only — not traced this slice).
