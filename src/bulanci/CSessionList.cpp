#include "CSessionList.h"

// !FUNC 0x0040adc0 BEGIN
/* 40ADC0-40ADF2 00032 */
uchar CSessionList::CSessionList_OnEvent(short param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0040adc0 END

// !FUNC 0x0040c2d0 BEGIN
/* 40C2D0-40C52E 0025E */
void* CSessionList::CSessionList_BuildDialog(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040c2d0 END

// !FUNC 0x0040c530 BEGIN
/* 40C530-40C536 00006 */
uchar* CSessionList::CSessionList_GetTypeDescriptor() {
    return reinterpret_cast<uchar*>(&DAT_004b34fc);
}
// !FUNC 0x0040c530 END

// !FUNC 0x0040c550 BEGIN
/* 40C550-40C568 00018 */
CSessionItem* CSessionList::CSessionList_GetPick(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040c550 END

// !FUNC 0x0040c570 BEGIN
/* 40C570-40C5C6 00056 */
void CSessionList::CSessionList_SelectBySessionGuid(uchar* param_1) { STUB_BODY(); }
// !FUNC 0x0040c570 END

// !FUNC 0x0040c5d0 BEGIN
/* 40C5D0-40C646 00076 */
void CSessionList::CSessionList_SetStatusFromStringHandle(int param_1) { STUB_BODY(); }
// !FUNC 0x0040c5d0 END

