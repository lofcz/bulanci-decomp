#include "CTeleportPoint.h"

// !FUNC 0x0041a920 BEGIN
/* 41A920-41A965 00045 */
uchar CTeleportPoint::CTeleportPoint_InitVtables(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041a920 END

// !FUNC 0x0041a970 BEGIN
/* 41A970-41A976 00006 */
uchar* CTeleportPoint::CTeleportPoint_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b3838);
}
// !FUNC 0x0041a970 END

// !FUNC 0x0041c9a0 BEGIN
/* 41C9A0-41CB62 001C2 */
void* CTeleportPoint::CTeleportPoint_Ctor(int* param_1, uint param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0041c9a0 END

// !FUNC 0x0041fca0 BEGIN
/* 41FCA0-41FCD1 00031 */
void CTeleportPoint::TriggerTeleportFX(void* param_1) { STUB_BODY(); }
// !FUNC 0x0041fca0 END

// !FUNC 0x0041fed0 BEGIN
/* 41FED0-41FF8E 000BE */
uchar CTeleportPoint::OnEvent(uint param_1, short param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0041fed0 END

