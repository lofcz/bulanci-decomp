# Round 3 spawn manifest

- **total_tasks_found:** 330 (from 330 raw extractions, deduplicated)
- **assigned_count:** 50
- **remaining_count:** 280

## Assigned tasks (round 3)

| id | priority | struct | one_liner |
|----|----------|--------|-----------|
| 0 | high | CDSJpegImage | `CDSJpegImage`: registry factory `0x0043c160` not disassembled; dual meaning of `+0x60` (`0x4b` class id vs JPEG quality |
| 1 | high | CGame | - Deferred: Ghidra `CGame` is still 872 B vs embedded span `0x248` (`CBulanci.md`). Tail field remains `void *` in Ghidr |
| 2 | high | CDSAudioBank | CDSAudioBank: Exact relationship `CDSAudioBank` vs `CDSWavStream` C++ types on class-43 instances (shared `0x40` alloc,  |
| 3 | high | CDSJpegImage | `CDSJpegImage`: registry slot @ `0x0043c160` is `0x24`-byte factory (not this type); `+0x60` dual role (class id constan |
| 4 | high | CDSFlxFile | CDSFlxFile: `field_2c` (zeroed in factory; read in `DecodeFrame` on `+0x04` face). |
| 5 | high | CMsgDialog | `CMsgDialog` class factory id at `CMsgDialog_GetClassTable@0x0040b760`. |
| 6 | high | CMsgDialog | RTTI class ids (`0x808` pause, `CMsgDialog` factory table). |
| 7 | high | CPauseDlg_Allocate | - Separate factory path: `CPauseDlg_Allocate@0x0040f090` (DATA xref class registration only). |
| 8 | high | CDSAudioBank | CDSAudioBank: Semantics of `dwField_20` / `dwField_38` (factory zero/init only; no post-deserialize consumer in batch sc |
| 9 | high | CDSAudioVideoPlayer | CDSAudioVideoPlayer: Standalone heap `CDSVideoPlayer` factory path vs this embedded `0x48`-byte subobject (same ctor/dto |
| 10 | high | CDSException | CDSException: Replacing the 1-byte `CDSException` placeholder with an inheritable Ghidra struct (MCP `create_struct` rep |
| 11 | high | CDSFont | CDSFont: Full `CDSImage` field map for `+0x08..+0x4b` once merged with `CDSImage.md`. |
| 12 | high | CDSImageMouse | CDSImageMouse: Relationship to `CGunMouse` (`0x218`): menu cursor is a separate class; `CGunMouse` does not use this fac |
| 13 | high | CDSJpegImage | CDSJpegImage: Registry factory @ `0x0043c160` — not yet a Ghidra function; may differ from `CDSJpegImage_CreateObject` @ |
| 14 | high | CDSMouse | CDSMouse: Whether heap `CDSMouse` instances are ever used at runtime vs factory serving as abstract base registration on |
| 15 | high | CDSStrmResInfo | CDSStrmResInfo: `loaderAux` consumer (factory clears; not read in `Serialize`). |
| 16 | high | CMsgDialog | CMsgDialog: Class factory table `CMsgDialog_GetClassTable@0x0040b760` → `&DAT_004b3524` (class id not decoded). |
| 17 | high | CPauseDlg | CPauseDlg: RTTI class id `0x808`; factory table `&DAT_004b367c` (`CPauseDlg_GetClassTable@0x0040be10`). |
| 18 | high | CDSQueueStream | CDSQueueStream: Purpose of standalone-only dword at `+0x20` when `sizeof` is `0x24` (no Read/Seek/Set xref). |
| 19 | high | CListBoxItem | Ghidra `/CListBoxItem` placeholder vs `CListBoxItemRow` naming (manual rename/merge when Java scripts compile). |
| 20 | high | CBulanci | CBulanci: Interior of embedded `CGame` (`0x248` bytes) — Ghidra’s existing `CGame` type is 872 bytes and must not be use |
| 21 | high | CPauseDlg | CPauseDlg: `pGame` left as `void *` until `CGame` struct recovery batch (872 B Ghidra placeholder must not be applied). |
| 22 | med | CDSMpxDecoder | CDSMpxDecoder: MI vtables (proven on `CDSMpx`, not separate alloc): `CDSMpx_dtor@0x00432f40` restores `param_1[1]=0x4872 |
| 23 | high | CGame | `pGame` Ghidra type until `CGame` batch recovers `0x248` layout. |
| 24 | med | CDSObject | `CDSObject`: image-variant `scheduler`/`CDSImage` overlay, `+0x34` play flags, `+0x48..+0x4f` vtable patch naming |
| 25 | med | CDSUpdatedItem | `CDSUpdatedItem+0x00` on hosts without facet vtable write (e.g. `CAdvertising+0x70`). |
| 26 | med | CBulanci | `ODSImage::pOwner` still generic `pointer` (owner is `CBulanci*` in `SetOwner` decompile — typing deferred). |
| 27 | med | CDSApiException | CDSApiException: Deferred: formal `CDSException` Ghidra struct to deduplicate with siblings — prefix `0x00..0x3b` matche |
| 28 | med | (misc) | `pSubObjStash@+0x3c` type; exact MI placement of `-0x18` / `-0x38` bases beyond vtable xrefs. |
| 29 | med | CAnim | CAnim: `+0x90..+0x97` — between `IDSAnim` vtable and `anim_sub` vtable. |
| 30 | med | CBitmap | CBitmap: `+0x8c` — `ODSImage::vf_odsimage` / `IDSAnim` vtable (`CGameView_ctor` patches `*(this+0x8c)`); `CBitmap_FireOn |
| 31 | med | CBulAnim | CBulAnim: `+0x08..+0x0F`, `+0x1C..+0x67` — CDSView / ODSImage / chain subobjects; only scattered vtable writes proven at |
| 32 | med | CBulAnim | CBulAnim: `+0x70..+0x77` — eight bytes between `CDSUpdatedItem` and `AnimInner` vtables. |
| 33 | med | CBulPicture | CBulPicture: `+0x08..+0x0F`, `+0x1C..+0x1F` — CDSChained/CDSView header between known vtables and `originX`. |
| 34 | med | CDSApiException | CDSApiException: Full `CDSException` hierarchy vtable map at `+0x00` beyond stored pointer value `0x487564` not expanded |
| 35 | med | CDSBackBuffer | CDSBackBuffer: Tail `CDSImage` MI vtables (`+0x4c..+0x5b` in full 96 B type) absent in 80 B `CDSBackBuffer` embed. |
| 36 | med | CDSChain | CDSChain: Full `CDSChain` object (`CDSChain_ctor@0x0040a680`, `mapping.csv` span `0xa4`) — vector at `this+0xd`, registr |
| 37 | med | CDSCollection | CDSCollection: None at collection payload (+0x08..+0x17). Base vtables (+0/+4) follow engine `IDSReferenced` / `IDSChain |
| 38 | med | CDSImageMouse | CDSImageMouse: `pCursorSprite` type (vtable slot uses `*(this+0xc)` with width at `+4`, height at `+8` — resource handle |
| 39 | med | CDSMpx | CDSMpx: MI vtable slots at `+0x04` / `+0x18` share storage with `stream` while decoding; only restored in `CDSMpx_dtor`, |
| 40 | med | CDSMpxStream | CDSMpxStream: Exact byte offset of the `IDSChained` / `face_8slots` subobjects beyond ctor vtable writes (`+0x38`, `+0x3 |
| 41 | med | CDSObject | CDSObject: `+0x48..+0x4f` on image instances: vtable patches in `CDSObject_CtorWithImage` (`pPad_48+4`, `pPad_54`); not  |
| 42 | med | CDSScript | CDSScript: Five vtable pointers (+0x00..+0x28): only two DWORD vtables written in base ctor; remaining slots filled by s |
| 43 | med | CDSStreamStorage | CDSStreamStorage: `GetStreamEntry@0x004339d0` — asm `MOV EAX,[ECX+0x20]` then index (`0x004339d0`); uses `CDSCollection: |
| 44 | med | CDSStreamStorage | CDSStreamStorage: `CDSCollection` @ `this+0x1c`: `CDSStreamStorage_ctor@0x00401790` writes collection vtables at `+0x1c` |
| 45 | med | CDSUpdatedItem | CDSUpdatedItem: `CAdvertising+0x70` and other hosts: `+0x00` at embed base not written by `CDSUpdatedItem_ctor`; may rem |
| 46 | med | CHistoryScript | CHistoryScript: Semantic type of `+0x438` sub-object beyond vtable / `FUN_00434250` cleanup. |
| 47 | med | CMina | CMina: Full `CAnim` field map inside `0x00–0xEF` (vtable slots at `+0x88/+0x98` overwritten in ctor but not named here). |
| 48 | med | CTeleportPoint | CTeleportPoint: Full CAnim decomposition for bytes `+0x6a..+0x73`, `+0x85..+0xa3`, `+0xa8..+0xd3` (vtable/anim core only |
| 49 | med | CLevelList | CLevelList: `CListBox.p_base` byte-level layout (204 B) — deferred to `CListBox` / `CListViewer` struct recovery; see cr |

## Remaining tasks (deferred)

| one_liner |
|-----------|
| `CDSBmpImage` / `CDSJpegImage` Ghidra struct size 100 vs heap `OperatorNew(0x60)` — embedded `CDSImage` overlaps MI vtables (same issue as batch 32 note) |
| `IDSReferenced` / `IDSEventHandler` subobjects on `CDSEasyMemStream` (vtable catalog only). |
| `CDSImage_BackBufferEmbed` omits tail MI vtables (`vf_IDSChained` … `m_chain`) — parent is only 80 B; not heap `CDSImage` (96 B). |
| Dedup sibling structs (`CHelpView`, `CMsgDialog`, …) to `CDSChained` in Ghidra — layout doc ready, per-class vtables still separate. |
| `FUN_0047ea50` atexit vtable overwrite on static singleton at exit. |
| `CAnim` / `CBitmap`: `+0x28..+0x2f`, `+0x40..+0x43`, `+0x48..+0x53`, `+0x58..+0x67` |
| `CAnim`: `+0x90..+0x97`; `track_manager` inner fields `+0xac..+0xef` |
| `CAnim` bytes at `+0x68..+0x6f` vs death net dwords — semantic overlap unresolved. |
| Full `CAnim` `+0x28..+0x67` (see `CAnim.md`). |
| `CAnim_recovered` gaps `+0x28..+0x67`, `+0xac..+0xef` (batch 3 scope; not expanded here). |
| `CBitmap`: `+0x14` uint16 widget flags (CBulAnim path only, not re-verified on `CBitmap`) |
| `CBulAnim` / `CBulPicture` `+0x08..+0x67` (and picture `+0x40..+0x67` tail): full `CDSView` layout — blocked on a dedicated `CDSView` recovery batch. |
| CBulanci: full `CDSApp` interior below `+0x284`; embedded `CGame` blob (`pPad_48` / `byte[568]`); `field_280`, `dwNetSessionField0/1`, purpose of `field_4b4` / `field_4c4` / `byte_4c8`. |
| `CDPEnumSessionInfo` / `DPSESSIONDESC2` header fields before `+0x08` and between `+0x18` and `+0x30` |
| CDSAnim drawable band `+0x28..+0x43` and `field_70`; CDSObject fields inside `+0x88`. |
| `CDSBackBuffer::embeddedImage::nField_44` (`CDSApp_ctor` writes `8` @ `CDSApp+0xc4`) — same offset as batch-24 `defaultBpp`; consumer beyond ctor not re-proven this round. |
| `CDSChained` dword/ushort semantics at `+0x08..+0x1c`, `+0x30..+0x3c`, `+0x5c..+0x64` (only zero-init proven). |
| `CDSDirectSound_SignalEventIfVoicesActive` passes `this+0x4c` to `SetEvent` in disassembly while the event handle lives at `+0x50` (documented in [CDSDirectSound.md](./CDSDirectSound.md)). |
| `CDSException_GetMessageW` → `__swprintf` into `+0x14` without explicit buffer size in decompilation. |
| CDSFont: full `CDSImage` sub-layout `+0x08..+0x4b`; `fontFooter` @ `+0x50c` not a separate Ghidra field (overlaps glyph/payload tail); `justifyWidth` @ `+0x215` inside blob |
| `CDSGZipStreamData` `+0x0c..+0x0f` padding (carried from batch 31) |
| `CDSImageMouse`: cursor sprite type; full inner `CDSImage` field use beyond Draw/Erase blit at `+0x1c`. |
| `CDSJpegImage` Ghidra struct 100 bytes vs two heap footprints (`0x64` wrapper vs `0x60` embedded image) — MI layout overlap at `m_image+0x5c` / object `+0x60` |
| `CDSMemoryException` bytes `+0x14..+0x3B` (`unkBaseTail`). |
| CDSMemoryException: Secondary vtables on `IDSChained` facet (`master_vtable_catalog.csv` lists 5 slots @ `0x00487534`). |
| CDSMouse: Relationship to `CGunMouse` (`0x218`) and `CDSImageMouse` (`0x88`) — derived classes use vtables `0x00483794` / `0x00486f68`, not `0x00483548` / `0x0048355c` (see `CGunMouse.md`, `CDSImageMo |
| CDSMpxDecoder: Whether MSVC names this a separate type or typedef of the `CDSMpx` primary base; only one RTTI vtable (`0x00489524`, 3 entries). |
| `CDSMpxStream` `mpxFormatTail@+0x08` vs live `mad_stream` overlay at runtime. |
| CDSObject: exact MI layout inside `scheduler` (`+0x04..+0x1b`); `+0x48..+0x4f` on image instances; whether `0x48`-byte TM objects use same bytes as `0x60` image tail beyond `+0x44`. |
| `CDSQueueStream` standalone dword at `+0x20` purpose. |
| CDSQueueStream: `InitializeQueueStreamView@0x004289a0` / `InitializeSourceRegion@0x0043be50` init paths on `CDSDsmFile` slices (vtable install, not a standalone `OperatorNew` for `CDSQueueStream`). |
| CDSResourceException: Secondary vtable slots on `0x4874b0` not expanded (same deferral as other `CDSException` subclasses). |
| - `FUN_0042f9b0@0x0042f9e1` — `*(this+0x10) += 1` with `this = parent+0x18` → `CDSSafeStream+0x28` |
| - `CDSSafeStream_RegisterThreadSlice@0x00446f60`: `MOV dword ptr [EDI+0x8], 1` after first thread node is chained |
| `CDSScript` pad dwords `+0x0c` / `+0x14` / `+0x1c` (base class; see `CDSScript.md`). |
| CDSSimpleException: IDSChained @ `0x0048754c`: Five slots are the primary vptr at `+0` (`vftable_methods.csv`); no embedded secondary vtable subobject or `AdjustThisOffset` on this type. Catalog label |
| CDSSimpleException: Vtable slots (proven): `[0]` `CDSSimpleException_GetClassTable@0x00434ac0` → `&DAT_004b81fc`; `[1]` `CDSException_DtorScalar@0x00434ae0`; `[2]` `CDSException_ReleaseViaFlag@0x0042f |
| CDSStreamException: `CDSStreamException_ctor_win32` @ `0x004301b0`: Same field order and vtable `0x4870cc`; sets `dwWin32Error = param_3` instead of `GetLastError()`; otherwise matches default ctor. |
| Ghidra `CDSStrmResInfo.pEmbeddedResource` label at `+0x14` (misleading vs `streamExtent`). |
| `CDSUpdatedItem+0x00` naming in Ghidra (`dwField_0x00`) for non–`CDSVideoPlayer` embeds |
| `SetupTrack` byte at parent `+0x3d` aligns with `CDSVideoPlayer::bPaused` (`+0x35` in embed); “trackReady” naming vs play-state flags. |
| `CDeath2` `+0x30..+0x67` interior (ODSImage/drawable header). |
| CDirectKeyb: `CDirectKeyb_GetKeyEdge@0x004121a0`: assembly uses `CDirectKeyb+4` / `+0x104` with scan code index; Ghidra MCP cannot retype `__thiscall` ECX `this` (decompile may still show `CGame*`). P |
| `CExplosion`: `+0x28..+0x67` header; `track_manager` sub-fields; blast helper signatures (`CGaming*` first arg). |
| `CGame` typed member at `CPauseDlg+0x70`. |
| `CGunMouse`: `CDSImage` blobs still `byte[96]`; `trackManager` interior (`+0x1b0..+0x1f7`). |
| `CHelpDlg` / both dialogs: `base_CWindow_MI` (`0x00..0x6f`) not field-split; `field_0x7c` at `+0x7c` unread. |
| CHelpDlg: `base_CWindow_MI` (`0x00..0x6f`): four vtables + `CWindow` bounds/children; mirror `CHistoryDlg_ctor@004231d0` (same geometry `0xe7,0x24,0x2fa,0x239`). |
| `CHelpScript+0x43c` on help/history scripts (zero-init only). |
| `CHelpScript` / `CHistoryDlg` `CWindow` prefix field names (`+0x00..+0x6f`). |
| `CHistoryScript` `+0x438` sub-object / `FUN_00434250` cleanup semantics. |
| `CHistoryScript+0x438` sub-object semantics; `nNestedRefCount` role on history path. |
| `CItemInfo+0x08`: zero-init only; likely CDS chain/refcount slot per `widgets.md` §13 `CListBoxItem` row layout — no direct read xref in follow-up scope |
| CListBoxItem: `+0x04`: no ctor write; likely CDS chain / sibling link (see `CItemInfo+0x04` note in batch 12). |
| Ghidra `CPauseDlg.pParent` component offset 78 vs runtime 0x4c (76) — 2-byte layout drift in DB; runtime xrefs use `+0x4c`. |
| - Resolved: `CPauseDlg_Build@0x00411df0` sole code xref = `CGaming_ctor@0x00420423` → `OperatorNew(0x7c)` → `Build` → stored `CGaming+0x334`, `CDSView__AddChild`. |
| +0x00 dual use on `CSET_SPECIAL` (`pExcept == 0x123`): `param_2` is `void ` for `ExceptionContinuation`, not a normal `pNext` link. |
| Reconcile `CScoreItem.m_link_prev` vs `CDsString` stream use at `+0xc`. |
| `CSessionItem+0x04` padding; `field_08` / `field_0c` semantics on `CListBoxItem` base |
| `CSessionList` `+0x22..+0x23` implicit padding vs explicit `wPad_16` on `CMsgDialog`. |
| `CSessionList` implicit padding `+0x48..+0x45`, `+0x47..+0x67`, `+0x69..+0x6B` and unmapped `CDSChained` dwords (`dwField_08`, `pParent`, etc.) — no `CSessionList`-specific consumers beyond shared dia |
| bad_exception: *(follow-up round 2 closed)* Throw frequency: `g_pBadException_vftable` @ `0x00489638` has exactly two DATA xrefs — `std::bad_exception_ScalarDeletingDtor` and `FindHandler` (vtable ins |
| `m_reserved_40` (+0x40): init-only zero, no readers. |
| Inline `wchar_t[256]` capacity at `+0x40` (size-derived only). |
| CAdvertising: +0x08..+0x0f, +0x1c..+0x6f: `CWindow::CWindow_BuildAt` on `this` at `+0` — treat as `CWindow`/`CDSView` interior; not individually named here. |
| CAdvertising: `updatedItem` (+0x70): follow-up applied verified `CDSUpdatedItem` (24 B); `+0x88..+0x8b` is explicit padding before `m_bBlockDismiss`. |
| CAnim: `+0x08..+0x0f` — not written in `CAnim_ctor` / `FUN_00419070`. |
| CAnim: `+0x28..+0x2f` — 8 B gap between `origin_y` and `bounds` (no ctor write in batch). |
| CAnim: `+0x40..+0x43` — 4 B between `bounds` tail and `view_flags` (no direct xref). |
| CAnim: `+0x48..+0x53` — 12 B before `child_chain` (no field-named consumer). |
| CAnim: `+0x58..+0x67` — 16 B between `child_chain` and `view_state_68` (CDSView chrome; compare `CBlackView` in `movie_cinema_views.md`). |
| CAnim: `+0x14` `uint16 flags` (`\|=0x200` on `CBulAnim` in `CMenu::FUN_004104f0`) not re-verified on `CAnim`. |
| CAnim: `+0xac..+0xef` — `track_manager` blob (`ConstructTrackManager@0x00439c70` on `this+0xa8`); ctor writes at `+0xa8..+0xdf` not field-named (see `CDSObject::ConstructTrackManager` decompile for in |
| CBitmap: `+0x08..+0x0f` — not written in `CGameView_ctor` / `FUN_00419070`. |
| CBitmap: `+0x28..+0x2f`, `+0x40..+0x43`, `+0x48..+0x53`, `+0x58..+0x67` — same CDSView gaps as `CAnim.md`. |
| CBulAnim: `trackManager` interior — field map in `CDSObject.md` through `+0x44`; Ghidra embed is opaque `byte[72]` at `+0x88` (active TM is `0x48` bytes; remainder is padding to `+0xD0`). |
| CBulPicture: `+0x40..+0x67` — CDSView/widget tail between render rect and `pBitmap`; no CBulPicture-only field xrefs. |
| CBulPicture: `+0x46E..+0x46F` — tail padding to `0x470` (two bytes after chroma index). |
| CBulanci: Full `CDSApp` field map below `+0x284` (documented in `ghidra_analysis/engine/app_shell.md` but not re-derived here). |
| CDPEnumSessionInfo: Whether `+0x08` is host IPv4 vs part of a 16-byte instance key (row compare uses copied dwords at `CSessionItem+0x14`, not a separate GUID field). |
| CDSAnim: `+0x08..0x0f`, `+0x28..0x43`, `+0x45..0x67` — CDSView / ODSImage drawable prefix not field-named (see `anim_runtime.md` / `CDSChained.md`). |
| CDSAnim: `+0x70` — no direct ctor write located (subscriber-key slot per `anim_runtime.md`). |
| CDSAnim: `+0xbc..+0xcf` — tail after `cached_sequence` (CBulAnim `team_index` at `+0xd0` is sibling class only). |
| CDSAudioBank: Gap `+0x24..+0x2f` purpose. |
| CDSAudioBankSample: Full C++ base-class map (primary / IDSReferenced @+4 / IDSStream @+0x18); size includes all three faces within 0x24. |
| CDSAudioBankSample: Dedicated `IDSAudioSource` struct type (decoder object layout at `pDecoder` in deserialize — only +8/+0xC copies proven here). |
| CDSAudioVideoPlayer: `SetupTrack` byte at parent `+0x3d` vs `CDSVideoPlayer::bPlayFlags` / `bPaused` naming (same offset as embed `+0x35`). |
| CDSAudioVideoPlayer: `SetupTrack` stores `*(CDSAudioPlayer+0x10) = &videoTrackManager` (back-link to embed base). |
| CDSBackBuffer: `pDirectDrawSurface` at image `+0x48` vs standalone `CDSImage::nField_48` constant `8` in other contexts. |
| CDSBitmap: Full `CDSChained` / `CDSView` field map inside `+0x08..+0x4b`. |
| CDSBitmap: `wViewFlags` at `+0x14` (`\|= 0x278` in `CMovieView::InitTrackSequence`) — offset proven on embedded base, not named in Ghidra struct yet. |
| CDSBitmap: Whether standalone heap `CDSBitmap` and `CMovieView` embed share identical tail layout above `+0x68` (CMovieView adds resource pointers at `+0x78`). |
| CDSChained: Semantic names for `dwField_08` / `dwField_0c` / `dwField_1c` and render-rect copies at `+0x30..+0x3c` (likely `CDSView` layer; larger widgets extend past `0x68`). |
| CDSDirectSound: `CDSDirectSound_SignalEventIfVoicesActive` @ 0x0043ccb0 loads `SetEvent` arg from `this+0x4c` (asm), while ctor/dtor and the worker thread store the handle at `+0x50` — possible off-by |
| CDSDirectSound: `CDSDirectSound_InitPrimary` is typed as `CDSApp *` in Ghidra but is always called with the embedded `CDSDirectSound *` (`CDSApp+0x200` @ 0x0042afd0). |
| CDSDirectSound: `m_reserved_40` (+0x40): only zeroed in `InitPrimary`; no other consumers found. |
| CDSFilterStream: `field_10` (`+0x10`) — ctor `0x20` only; no read xref in follow-up scope. |
| CDSFont: `fontFooter` @ `+0x50c` (8-byte read) overlaps the tail of `glyphTable` / `embeddedFontPayload` — not modeled as a separate Ghidra field. |
| CDSGZipStreamData: `+0x0c..+0x0f` not touched in the `Open` allocation path (may be padding or filled elsewhere). |
| CDSImage: Full MI thunk graph between `+0x54` / `+0x58` bases and `CDSBmpImage` / `CDSJpegImage` tails (`bmp_decoder.md`). |
| CDSImage: Load/Save entry points receive `this` at the `+0x54` stream-host subobject; assembly uses `ECX-0x50` to reach pixel `CDSImage` (`CDSBmpImage_Load@0x004320c0`, `CDSJpegImage_Load@0x00431cc0`) |
| CDSJpegImage: Whether dword @ `+0x60` is only engine class id `0x4b` or also reused as JPEG quality 75 in `CDSJpegImage_Save` (IDSImage `this` at `+0x54` path) — same numeric value, consumer not re-ve |
| CDSJpegImage: Pixel buffer / stride / palette live in embedded `CDSImage` at `+0x04` (see `CDSJpegImage__DecompressToImage@0x00431b70` consumer `param_3+0x10` stride). |
| CDSMemoryException: Bytes `+0x14..+0x3B` (40 bytes): no ctor/dtor/consumer field splits; modeled as `unkBaseTail` (same gap as `CDSResourceException`). |
| CDSMemoryException: `wchar_t[256]` capacity at `+0x40`: derived from allocation size only; no bounds-checked consumer besides `__swprintf`. |
| CDSMpx: `mad_stream` dwords at `+0x08`, `+0x10`, `+0x2c`..`+0x38` (named `unk_*` in Ghidra). |
| CDSMpx: `mad_frame` interior (`body`, `param_1[0x90c]` at `+0x2430`). |
| CDSMpx: Semantic name for dword at `frame+0x14` copied to `sampleFormatPacked` (may be header field, not a distinct sample-format slot). |
| CDSMpx: Gap `+0x5884..+0x5887` and `+0x58b0..+0x58bb` (no independent xrefs). |
| CDSMpx: `ResolveResource@0x00446b90` reads `+0x1c`/`+0x24`/`+0x28` — likely base-class resource path, not remapped here. |
| CDSMpxStream: Whether `+0x08` `mpxFormatTail` overlaps live `mad_stream` bytes at runtime vs persistence-only overlay. |
| CDSMpxStream: `pSubObjStash` type at `+0x3c`. |
| CDSObject: Image variant: `scheduler` bytes are constructed by `CDSImage_ctor` (not `CDSUpdatedItem_ctor`); nested `CDSImage` type is 96 bytes heap-wide but only `+0x50`/`+0x5c` alias `CDSImage::refco |
| CDSObject: `+0x34` (`pad_34`): no ctor write in `ConstructTrackManager` (cf. `CDSVideoPlayer::bPlayFlags` at same offset). |
| CDSObject: Track-manager `0x48`-byte alloc ends at `+0x44`; image `0x60` alloc adds tail through `+0x5c` only. |
| CDSResourceSign: `+0x18` not explicitly zeroed in ctor (only `param_1[7]`/`[8]`); relies on `malloc` zero-init — not separately proven. |
| CDSScript: +0x0c `reserved_0c`, +0x14 `reserved_14`: ctor-zero only; decompile/xref pass found no consumers on `CDSScript*` (note: pack enumerator compares `+0xc` on resource descriptors, not script i |
| CDSScript: +0x18 `scriptData`: freed in `CDSScript_dtor@0x00438400`; no typed store besides ctor `=0` in `bulanci.exe` (resource bind may write via raw offsets — not proven). |
| CDSScript: +0xe0..+0x42b (844 bytes): no field-level xrefs in base-class methods; `CLevelScript`/`CHistoryScript` extend opcode table into part of this span via second `InstallOpcodeTable` splice at ` |
| CDSSimpleException: `__swprintf` length: `CDSException_GetMessageW@0x00434b80` calls `__swprintf` into `+0x14` without an explicit count; safe use depends on format strings from `g_pCDSApp_vftable[10] |
| CDSSimpleException: `inlineMessage` span: `0x14 + 0x28 = 0x3C` matches `OperatorNew(0x3c)` in `CDSSimpleException_Throw@0x00434c20`; no fields past `+0x13` on this subclass. |
| CDSStreamException: `+0x3C` vs `inlineMessage`: `wchar_t[20]` at `+0x14` occupies `0x14..0x3B` (`0x28` bytes); `pFormatMsg` at `+0x3C` is adjacent, not overlapping. Ctor/dtor use `this->pFormatMsg` /  |
| CDSStreamStorage: `GetStreamCount@0x004339c0` — asm `MOV EAX,[ECX+0x24]` → returns `CDSCollection::m_items` pointer as `uint`, not `m_count` at `+0x28`. Misleading name; sole direct caller `IDSAnim_Se |
| CDSStreamStorage: `FUN_00433f00`, `FUN_00433f70`, `FUN_00434760` stream-loader helpers: per-thread nodes at `this+0x34` and temporary `CDSStrmResInfo` keys on stack in `CloseStreamByKey`. |
| CDeath2: `+0x30..+0x67` between `draw_pos_y` and `m_netStateWord0` (no `CDeath2`-specific consumer; may be `ODSImage` / drawable header — see `CAnim.md` `+0x28..+0x67`). |
| CDeath2: `+0x68` as `CAnim` view bytes vs dword net state — death path uses dword copy only (`UpdateStateFromParams`). |
| CDeath2: Full `track_manager` (`+0xa8..`) field map — defer to `CAnim` / `anim_runtime.md`. |
| CExitDlg: No non-zero writer for `pLinkedAnim` / `reserved_74` / `reserved_78` in any of the three `CExitDlg_*` symbols (`ctor`, `RouteSyntheticCloseEvent`, `GetClassTable`); program-wide `MOV [reg+0x |
| CExitDlg: `header_unk` between `+0x30` and `+0x46` not individually xrefs’d on `CExitDlg`. |
| CExitDlg: Decor `CAnim`×2, `CStaticText`, `CButton` are separate `OperatorNew` objects, not embedded at `+0x70`. |
| CExplosion: `+0x28..+0x67` drawable header gap (see `CAnim.md` `header_unk_28`). |
| CExplosion: `track_manager` inner layout (`+0xAC..+0xEF` on standalone `CAnim`) not field-resolved. |
| CExplosion: `CollectEntitiesInBlastRect` / `CollectLandminesInBlastRect` first parameter is `CGaming*` (`explosion+0x84`), not `CExplosion*`. |
| CGunMouse: Exact `CDSImage` / `CDSObject` sub-layout sizes (assumed `0x60` per image from ctor spacing `+0x1c` dwords). |
| CGunMouse: `trackManager` internal fields (`+0x1b0..+0x1f7`) except destructor boundary. |
| CGunMouse: Scalars at `+0x190..+0x18f` between last `CDSImage` and `m_mouseX` (ctor zeroes `+0x190..+0x1ab` region via indices `0x64..0x6b`). |
| CHelpScript: `+0x0c`, `+0x14`, `+0x1c`: zeroed in `CDSScript::ctor` only (see `CDSScript.md`). |
| CHelpScript: `+0x43c`: ctor zero only on help/history scripts (`CLevelScript` reuses offset for MI adjust — not proven here). |
| CHistoryDlg: Full `CWindow`/`CDSView` sub-object field names below `+0x70` (mirror `CHelpDlg.base_CWindow_MI`; see `widgets.md` §16). |
| CHistoryScript: Whether `+0x434` matches nested-refcount semantics on level/help script paths (no direct `CHistoryScript` consumer). |
| CItemInfo: `+0x08`: zero-init only; chain/refcount role per `widgets.md` §13 `CListBoxItem` row layout — no direct read xref found. |
| CMina: `InitMine` vs `CMina_Ctor` field order swap at `+0x108`/`+0x110` (deploy path vs weapon deploy). |
| CPauseDlg: Extra pause buttons in `CPauseDlg_Build` are heap children via `CDSView__AddChild` (not tail fields past `+0x78`). |
| CScoreItem: `m_link_prev` vs `CDsString_*` at `+0xc`: list splice (`FUN_0042f820`) and `Serialize@0x00408f50` (`LEA ECX,[EDI+0xc]` before `CDsString_WriteWStringToStream`) share the same offset — reco |
| CScoreItem: `m_deaths` (`+0x18`) not written by `Serialize@0x00408f50` (only `+0x10` and `+0x14` dword payloads after WString); compare/dtor still use `+0x18`. |
| CSessionItem: `+0x04` (`dwField_04`): list-row chain slot (see `CListBoxItem.md`). |
| CSessionItem: `CDPEnumSessionInfo` bytes `+0x00..+0x07` and `+0x18..+0x2f` (no reads in `CSessionItem_Initialize`). |
| CSessionItem: `CSessionList_SelectBySessionGuid@0x0040c570` compares 16 bytes at `item+0x14` (four dwords: IP, port, count, flags) — not a separate GUID field. |
| CSessionList: `CDSChained` dwords in `+0x08..+0x64` gap (inherited from dialog pattern; no `CSessionList`-specific xrefs in `BuildDialog`). |
| CSessionList: Whether `CSessionList_SetStatusFromStringHandle`’s `this` is always a `CSessionList*` (named under `CGame::` in export dump; uses `this+0x74`). |
| CTeleportPoint: Writer for `overlap_entity` @ `+0x64` — no `MOV [reg+0x64]` store in `0x00400000..0x0047ffff` (same offset as `CDSChained::dwField_64`); consumer-only in `OnEvent`. |
| CTeleportPoint: `OnEvent` still uses `this[-1].pPartner_node` for partner visibility probe — partner at `+0xf0` needs denser `+0xd8..+0xef` layout or `CAnim` nested type to eliminate adjacency hack. |
| DPEnumSessionInfo: All bytes outside the table above (including `+0x00..+0x07` before `dwHostIp`). |
| EHExceptionRecord: `ExceptionRecord` (`+0x08`), `ExceptionAddress` (`+0x0C`): no direct field xref in bulanci EH paths (only whole-struct `RtlUnwind` cast). |
| EHExceptionRecord: `ExceptionInformation[3..14]` (`+0x20`..`+0x4C`): no consumer xref; C++ EH path always uses three parameters. |
| EHRegistrationNode: `ENABLE_EHTRACE` tail field: Not present in this VS2005 build (no xref past `+0x08` on plain `EHRegistrationNode*`). |
| `CBulAnim` `trackManager`: typed sub-fields in Ghidra (embed is opaque `byte[72]`; active TM `0x48`). |
| `GetStreamEntry` / `GetStreamCount` naming and `IDSAnim_SelectRandomTrack` cast of `CBulAnim::pPad_1c` (see [CDSStreamStorage.md](./CDSStreamStorage.md)). |
| `CBulPicture` `pBitmap` exact type (`CDSBitmap*` vs `void *`) until `CDSBitmap` / cast id `DAT_004b826c` is sized in struct recovery. |
| `CBulanci`: `CGame` blob, tail field semantics, Ghidra size reconciliation |
| Full standalone `CBulanci` `CDSChain` object (ctor @ `0x0040a680`) — not the embedded `0x14` slice. |
| `m_pAudioSequence` concrete type (`CDSAnimSequence *` vs `void *`) pending RTTI target struct. |
| `CDSAudioBankSample`: `dwReadCursor` semantics; full MI map; standalone `IDSAudioSource` struct (decoder-only layout not fully recovered). |
| `CDSAudioPlayer`: who reads `pEventTarget` after `Init` (only clear-on-stop proven); `OnPlaybackTick` completion uses player pointer in `PostMessage`, not `pEventTarget`. |
| `CDSBitmap` opaque regions `viewHeader_28`, `pad_50`; `CDSChained` type still absent in Ghidra. |
| `CDSChain.m_pSentinel` heap vs. circular self-pointer (`FUN_0042f800`). |
| Full `CDSChain` object (`CDSChain_ctor` / size `0xa4` on `CBulanci`) — not the embedded list-head |
| `CDSChained` dword/ushort semantic names for `dwField_08..64` and chain-flag bit meanings. |
| `CDSChained` dialog/view base (`0x68`+) — separate batch in `batches_50.json`. |
| Standalone `CDSChained` prefix (dialogs/views) — separate manifest batch, not 30–36 file indices. |
| - Decompiled `CDSChained_ResetChainCounters@0x0042beb0` (called from `CDSChained_ctor@0x004032d0` / `CWindow_BuildAt`). |
| `CDSCollection_InsertKeyed`: decompiler `this` type not fully retyped to `CDSCollection*`. |
| CDSDirectXException: Formal standalone `CDSException` struct type in Ghidra (batch 29 manifest) — layout here matches siblings at `0x48` alloc. |
| `CDSDirectXException_What` vs `GetMessageW` / inline buffer usage on thrown instances. |
| Standalone `CDSException` type in Ghidra. |
| Formal `CDSException` base type (0x3C prefix) — batch 29. |
| `dwM_streamFlags` / `CDSFilterStream::field_10` — write-only or ctor-only in follow-up scope. |
| CDSFlxFile: semantic meaning of `hdr_dword0`..`hdr_dword4` / `hdr_tailDword`; `field_2c` |
| `CDSImage` slot vector / `field_40` semantics unchanged from batch 31. |
| `CDSImage` `field_44` / `field_48` constant `8`; full MI thunk graph |
| `CDSImageMouse`: `pCursorSprite` resource type; inner `CDSImage` MI/slot details (`CDSImage.md`) |
| Whether bare `CDSMouse` heap instances are used vs abstract registration-only base. |
| `CDSObject` / `CDSImage` inner layouts unchanged this batch. |
| `CDSPtrSlotVec_Resize` callee namespace (`CDSAudioBank::` prefix in decompiler display only) |
| `dwM_streamFlags` `0x20` vs `1` on `CDSSafeStream`. |
| `CDSSafeStream` upgrade to VERIFIED blocked on read-side proof for `dwM_streamFlags` and filter-vs-safe field reconciliation beyond RTTI. |
| Decompiler still types `RegisterThreadSlice` as `CDSSafeStreamInfo *this` (ECX retype API limitation); true object is `CDSSafeStream *` |
| `CDSStreamStorage_GetStreamEntry` / `GetStreamCount` interface `this` adjustment vs `CloseStreamByKey` collection base |
| `CDSStrmResInfo.loaderAux`, `pad_tail` |
| `CDSView` (128 B) vs `CWindow` (112 B) relationship for non-dialog views. |
| `CDeath` dedicated struct recovery (`CDeath.md`) — assigned to later batch in `batches_50.json`. |
| `CDeath` `field_100` / `field_104` consumers. |
| `CDeath2` interior pads and `pM_pPalette` / `pM_pParent` semantics. |
| `CDirectKeyb_GetKeyEdge` `this` typing (batch 06 note, out of scope for batch 05). |
| `CDirectKeyb_GetKeyEdge` decompiler `this` type (manual retype in Ghidra UI if desired). |
| `CExitDlg`: `pLinkedAnim` / `reserved_74` / `reserved_78` purpose (layout slot unused in this build); `0x40–0x4B` chain padding. |
| Dedicated `CGameView.md` not created (alias documented in `CBitmap.md`) |
| Full-object ctors (`CGunMouse`, `CWeapon`, etc.) — out of `0x10` mixin scope. |
| Full decomposition of `CListBox.p_base` (`undefined1[204]`) — owned by `CListBox` / `CListViewer` recovery, not `CLevelList` |
| `CListBoxItem` `field_08` / `field_0c` semantics. |
| … and 80 more in `round3_tasks_remaining.json` |