#include "CDSUpdatedItem.h"

// !FUNC 0x0042ea70 BEGIN
/* 42EA70-42EA76 00006 */
uchar* CDSUpdatedItem::CDSUpdatedItem_GetTypeInfo() {
    return reinterpret_cast<uchar*>(&DAT_004b7c80);
}
// !FUNC 0x0042ea70 END

// !FUNC 0x0042f060 BEGIN
/* 42F060-42F13C 000DC */
void CDSUpdatedItem::CDSUpdatedItem_ctor(void* param_1) { STUB_BODY(); }
// !FUNC 0x0042f060 END


// !FUNC 0x0042eaa0 BEGIN
/* 42EAA0-42EABE 0001E */
bool CDSUpdatedItem::Scheduler_IsSlotLive(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042eaa0 END

// !FUNC 0x0042eac0 BEGIN
/* 42EAC0-42EAF2 00032 */
void CDSUpdatedItem::Scheduler_FreeSlotIfLive(uint param_1) { STUB_BODY(); }
// !FUNC 0x0042eac0 END

// !FUNC 0x0042ebb0 BEGIN
/* 42EBB0-42EBE6 00036 */
void CDSUpdatedItem::Scheduler_EnsureCapacity(uint param_1) { STUB_BODY(); }
// !FUNC 0x0042ebb0 END

// !FUNC 0x0042f1e0 BEGIN
/* 42F1E0-42F204 00024 */
void* CDSUpdatedItem::Scheduler_GetEventSlot(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f1e0 END

// !FUNC 0x0042f210 BEGIN
/* 42F210-42F285 00075 */
void CDSUpdatedItem::Scheduler_RegisterEventSlot(uint param_1, uint param_2, uint param_3) { STUB_BODY(); }
// !FUNC 0x0042f210 END

// !FUNC 0x0042f290 BEGIN
/* 42F290-42F2C1 00031 */
void CDSUpdatedItem::Scheduler_SetEventLastFireMs(uint param_1, int param_2) { STUB_BODY(); }
// !FUNC 0x0042f290 END

// !FUNC 0x0042f2d0 BEGIN
/* 42F2D0-42F2F6 00026 */
void CDSUpdatedItem::Scheduler_SetEventDelayMs(uint param_1, uint param_2) { STUB_BODY(); }
// !FUNC 0x0042f2d0 END

// !FUNC 0x0042f300 BEGIN
/* 42F300-42F32A 0002A */
uchar CDSUpdatedItem::Scheduler_ArmSlot(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f300 END

// !FUNC 0x0042f330 BEGIN
/* 42F330-42F388 00058 */
uchar CDSUpdatedItem::Scheduler_AckSlot(uint param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042f330 END

