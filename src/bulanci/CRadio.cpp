#include "CRadio.h"

// !FUNC 0x00402fb0 BEGIN
/* 402FB0-403009 00059 */
void CRadio::CRadio_OnMouseDown(uint param_1, short param_2) { STUB_BODY(); }
// !FUNC 0x00402fb0 END

// !FUNC 0x00403010 BEGIN
/* 403010-403016 00006 */
uint CRadio::CRadio_GetDataSize() { return 1; }
// !FUNC 0x00403010 END

// !FUNC 0x00403020 BEGIN
/* 403020-403032 00012 */
uchar CRadio::CRadio_Invalidate(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00403020 END

// !FUNC 0x00403ac0 BEGIN
/* 403AC0-403CB6 001F6 */
void CRadio::CRadio_Render(void* param_1) { STUB_BODY(); }
// !FUNC 0x00403ac0 END

// !FUNC 0x00403cc0 BEGIN
/* 403CC0-403D04 00044 */
void CRadio::CRadio_SetSelected(uchar param_1) { STUB_BODY(); }
// !FUNC 0x00403cc0 END

// !FUNC 0x00403d10 BEGIN
/* 403D10-403D20 00010 */
uchar CRadio::CRadio_LoadData(uchar* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00403d10 END

// !FUNC 0x00405240 BEGIN
/* 405240-40527F 0003F */
uchar CRadio::CRadio_OnMouseUp(uint param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00405240 END

// !FUNC 0x00407300 BEGIN
/* 407300-40739F 0009F */
uchar CRadio::CRadio_ctor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00407300 END

// !FUNC 0x004073a0 BEGIN
/* 4073A0-4073A6 00006 */
uchar* CRadio::CRadio_GetTypeDescriptor() {
    return reinterpret_cast<uchar*>(&DAT_004b3334);
}
// !FUNC 0x004073a0 END

// !FUNC 0x004073b0 BEGIN
/* 4073B0-4073B8 00008 */
uchar CRadio::CRadio_AdjustorThunk04_Dtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004073b0 END

// !FUNC 0x004073c0 BEGIN
/* 4073C0-4073C8 00008 */
uchar CRadio::CRadio_AdjustorThunk10_Dtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004073c0 END

// !FUNC 0x004073d0 BEGIN
/* 4073D0-4073D8 00008 */
uchar CRadio::CRadio_AdjustorThunk18_Dtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004073d0 END

// !FUNC 0x00407500 BEGIN
/* 407500-4075EA 000EA */
void CRadio::CRadio_dtor(uchar param_1) { STUB_BODY(); }
// !FUNC 0x00407500 END

// !FUNC 0x00407f30 BEGIN
/* 407F30-407F4E 0001E */
void* CRadio::CRadio_vDtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00407f30 END

// !FUNC 0x00408540 BEGIN
/* 408540-4086D9 00199 */
void* CRadio::CRadio_BuildAt(int param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00408540 END

// !FUNC 0x0040aa00 BEGIN
/* 40AA00-40AA0C 0000C */
uchar CRadio::CRadio_SaveData(uchar* param_1) {
    uchar val = *(reinterpret_cast<uchar*>(this) + 0x68);
    *param_1 = val;
    return val;
}
// !FUNC 0x0040aa00 END

// !FUNC 0x004075f0 BEGIN
/* 4075F0-4076F7 00107 */
uchar CRadio::CRadio_AddOption(WCHAR* param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004075f0 END

