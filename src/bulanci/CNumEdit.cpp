#include "CNumEdit.h"

// !FUNC 0x004031a0 BEGIN
/* 4031A0-4031AF 0000F */
uchar CNumEdit::CNumEdit_GetValue(uint* param_1) {
    uint val = *reinterpret_cast<uint*>(reinterpret_cast<char*>(this) + 0xc0);
    *param_1 = val;
    return static_cast<uchar>(val);
}
// !FUNC 0x004031a0 END

// !FUNC 0x00404550 BEGIN
/* 404550-404587 00037 */
uint CNumEdit::CNumEdit_ValidateRange(short param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00404550 END

// !FUNC 0x00406290 BEGIN
/* 406290-4062F7 00067 */
uint CNumEdit::CNumEdit_OnTextChanged(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00406290 END

// !FUNC 0x00406750 BEGIN
/* 406750-406756 00006 */
uchar* CNumEdit::CNumEdit_GetTypeDescriptor() {
    return reinterpret_cast<uchar*>(&DAT_004b3414);
}
// !FUNC 0x00406750 END

// !FUNC 0x00407220 BEGIN
/* 407220-4072AD 0008D */
uchar CNumEdit::CNumEdit_OnFocusOut(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00407220 END

// !FUNC 0x004073e0 BEGIN
/* 4073E0-40745B 0007B */
uint* CNumEdit::CNumEdit_Allocate() { STUB_BODY(); return 0; }
// !FUNC 0x004073e0 END

// !FUNC 0x00407de0 BEGIN
/* 407DE0-407DFC 0001C */
uchar CNumEdit::CNumEdit_AdjustorThunk04_Dtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00407de0 END

// !FUNC 0x00407e00 BEGIN
/* 407E00-407E0F 0000F */
uchar CNumEdit::CNumEdit_vDtor(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00407e00 END

// !FUNC 0x0042d2f0 BEGIN
/* 42D2F0-42D303 00013 */
uint CNumEdit::FUN_0042d2f0(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042d2f0 END

// !FUNC 0x00448ed6 BEGIN
/* 448ED6-448EE7 00011 */
uchar CNumEdit::FUN_00448ed6(wchar_t* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00448ed6 END

// !FUNC 0x0044eb1e BEGIN
/* 44EB1E-44ED15 001F7 */
ulong CNumEdit::wcstoxl(localeinfo_struct* param_1, wchar_t* param_2, wchar_t** param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x0044eb1e END

// !FUNC 0x0044ed15 BEGIN
/* 44ED15-44ED3E 00029 */
long CNumEdit::_wcstol(wchar_t* param_1, wchar_t** param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0044ed15 END

// !FUNC 0x0044ed3e BEGIN
/* 44ED3E-44EDC0 00082 */
int CNumEdit::_iswctype_l(int param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0044ed3e END

// !FUNC 0x00451606 BEGIN
/* 451606-451788 00182 */
int CNumEdit::_wchartodigit(ushort param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00451606 END

// !FUNC 0x004522a9 BEGIN
/* 4522A9-4524CD 00224 */
int CNumEdit::_crtGetStringTypeW_stat(localeinfo_struct* param_1, ulong param_2, wchar_t* param_3, int param_4, ushort* param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x004522a9 END

// !FUNC 0x004524cd BEGIN
/* 4524CD-45250B 0003E */
uchar CNumEdit::_crtGetStringTypeW(localeinfo_struct* param_1, ulong param_2, wchar_t* param_3, int param_4, ushort* param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x004524cd END

