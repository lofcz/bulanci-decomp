#include "CDSApiException.h"

// !FUNC 0x00434b00 BEGIN
/* 434B00-434B06 00006 */
uchar* CDSApiException::CDSApiException_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b81e8);
}
// !FUNC 0x00434b00 END

// !FUNC 0x00434b20 BEGIN
/* 434B20-434B76 00056 */
uchar CDSApiException::CDSApiException_dtor(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434b20 END

// !FUNC 0x00434c70 BEGIN
/* 434C70-434CF4 00084 */
wchar_t* CDSApiException::CDSApiException_What(wchar_t* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434c70 END

// !FUNC 0x00434d00 BEGIN
/* 434D00-434D4C 0004C */
void CDSApiException::CDSApiException_ThrowFromGetLastError() { STUB_BODY(); }
// !FUNC 0x00434d00 END

// !FUNC 0x00434d50 BEGIN
/* 434D50-434D9A 0004A */
uchar CDSApiException::CDSApiException_ThrowFromWin32(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434d50 END

// !FUNC 0x00434e10 BEGIN
/* 434E10-434E2E 0001E */
void* CDSApiException::CDSApiException_DtorScalar(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434e10 END

