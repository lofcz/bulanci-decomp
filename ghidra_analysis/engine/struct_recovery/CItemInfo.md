# CItemInfo

## Status

**VERIFIED** — size `0x10` (16 bytes); minimal `IDSEventHandler` shell (parent meta `CDSObject` @ `0x4b7bfc`). Standalone instances are factory-only; **`CListBoxItem`** (class **2014**) embeds this layout as MFC parent (meta `0x4b33b8`). Row **`dwRowFlags`** at `+0x0c` is read by list `RenderItem` paths (`& 1` disabled tint). `+0x04` padding and `+0x08` reserved dword have no consumer xref.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CItemInfo) == 0x10` | `0x00404c50` | MFC `CreateObject` factory: `PUSH 0x10` → `OperatorNewWithBadAlloc` |
| Last dword at `+0x0c` | `0x00404c69` | Factory `MOV [EAX+0xc], ECX` (zero) after alloc |
| Class registration | `0x0047b1d0` | `PUSH 0x404c50`; `PUSH 0x4b7bfc`; `PUSH 0x7dd` (2013); `MOV ECX,0x4b33b8` → `HandleClassRegister@0x42e910` |
| `CListBoxItem` parent | `0x0047b200` | `PUSH 0x406690`; `PUSH 0x4b33b8` (this meta); `PUSH 0x7de` (2014); `MOV ECX,0x4b33cc` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void *` | `vftable` | `CItemInfo::CreateObject@0x00404c50` → `*puVar1 = 0x47faa4` |
| `+0x04` | 4 | `uint32` | `dwPad_04` | Not written in `CItemInfo::CreateObject@0x404c50` or `CListBoxItem_ctor@0x40b640` |
| `+0x08` | 4 | `uint32` | `dwReserved_08` | `CreateObject` → zero; `CListBoxItem_ctor` → `*(this+8)=0`; no read xref |
| `+0x0c` | 4 | `uint32` | `dwRowFlags` | Zero-init in ctors; `CListBox_RenderItem@0x405fc0`, `CLevelList_RenderItem@0x40d490` test bit 0 |

## RTTI / vtable

| Slot | Address | Symbol |
|------|---------|--------|
| Primary `IDSEventHandler` | `0x0047faa4` | `[0]=CItemInfo::GetTypeInfo@0x403500`; `[1]=CDSException_DtorScalar@0x434ae0`; `[2]=CDSObject_ReleaseViaVtable@0x4245c0`; `[3]=CDSObject_GetThis@0x434b10` |
| Type descriptor | `0x004b33b8` | `CItemInfo::GetTypeInfo@0x403500` returns `&DAT_004b33b8` |

## Ghidra apply

**Slice 12 (2026-05-30):** `modify_struct_field` renamed `dwPad_04`, `dwReserved_08`, `dwRowFlags`; `CItemInfo::CreateObject@0x404c50` return type `CItemInfo *`; `CItemInfo::GetTypeInfo@0x403500`.

```
Structure: CItemInfo  Size: 16
  vftable @ 0
  dwPad_04 @ 4
  dwReserved_08 @ 8
  dwRowFlags @ 12
```

Decompiled factory (`CItemInfo::CreateObject`):

```c
p = OperatorNewWithBadAlloc(0x10);
p->vftable = 0x47faa4;
p->dwReserved_08 = 0;
p->dwRowFlags = 0;
// dwPad_04 (+0x04) left uninitialized
```

## UNK

- `dwReserved_08` (`+0x08`): zero-init only; no read xref (may mirror `CListBoxItem.dwAssocOrNext` when subclassed).
- Standalone `CItemInfo` (class **2013** / `0x7dd`): no heap consumers besides static `HandleClassRegister`; instances are normally reached as `CListBoxItem` bases.
