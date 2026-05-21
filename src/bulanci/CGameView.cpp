#include "CGameView.h"

// !FUNC 0x00416f30 BEGIN
/* 416F30-416F36 00006 */
uchar* CGameView::FUN_00416f30() {
    return reinterpret_cast<uchar*>(&DAT_004b3804);
}
// !FUNC 0x00416f30 END

// !FUNC 0x00416f50 BEGIN
/* 416F50-416F57 00007 */
int CGameView::FUN_00416f50() {
    return *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x6c)
         + *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x2c);
}
// !FUNC 0x00416f50 END

// !FUNC 0x00417030 BEGIN
/* 417030-4170D7 000A7 */
uint* CGameView::FUN_00417030(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417030 END

// !FUNC 0x00417210 BEGIN
/* 417210-417256 00046 */
uchar CGameView::FUN_00417210(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417210 END

// !FUNC 0x00417f40 BEGIN
/* 417F40-417F74 00034 */
uint CGameView::FUN_00417f40(uchar param_1, int param_2, uchar param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00417f40 END

// !FUNC 0x00418290 BEGIN
/* 418290-4182C9 00039 */
uchar CGameView::FUN_00418290(uchar param_1, uchar param_2, uchar param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00418290 END

// !FUNC 0x004182d0 BEGIN
/* 4182D0-4182FD 0002D */
uchar CGameView::FUN_004182d0(uchar param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004182d0 END

// !FUNC 0x00418c60 BEGIN
/* 418C60-418D00 000A0 */
uint* CGameView::FUN_00418c60() { STUB_BODY(); return 0; }
// !FUNC 0x00418c60 END

// !FUNC 0x00418d70 BEGIN
/* 418D70-418E8B 0011B */
uint* CGameView::FUN_00418d70(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00418d70 END

// !FUNC 0x00419010 BEGIN
/* 419010-419066 00056 */
uchar CGameView::FUN_00419010(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00419010 END

// !FUNC 0x004191a0 BEGIN
/* 4191A0-419273 000D3 */
uint* CGameView::FUN_004191a0(uint param_1, uint param_2, int* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004191a0 END

// !FUNC 0x0041acf0 BEGIN
/* 41ACF0-41AD7D 0008D */
uchar CGameView::FUN_0041acf0(short param_1, uint param_2, uint* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0041acf0 END

// !FUNC 0x0041e4b0 BEGIN
/* 41E4B0-41EA87 005D7 */
uint* CGameView::FUN_0041e4b0(uchar* param_1, uchar param_2, void* param_3, int param_4, uint* param_5, int param_6, int param_7, int param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0041e4b0 END

// !FUNC 0x00439720 BEGIN
/* 439720-43972A 0000A */
void CGameView::TM_SetFrameDelayOverrideMs(int param_1) {
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x44) = param_1;
}
// !FUNC 0x00439720 END

