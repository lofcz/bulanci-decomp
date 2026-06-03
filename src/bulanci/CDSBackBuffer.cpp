#include "CDSBackBuffer.h"

// !FUNC 0x0042ad20 BEGIN
/* 42AD20-42AD26 00006 */
uchar* CDSBackBuffer::CDSBackBuffer_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b3b34);
}
// !FUNC 0x0042ad20 END

// !FUNC 0x0042ad30 BEGIN
/* 42AD30-42AD38 00008 */
uchar CDSBackBuffer::CDSBackBuffer_OnEvent(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042ad30 END


// !FUNC 0x0042adb0 BEGIN
/* 42ADB0-42ADCE 0001E */
void* CDSBackBuffer::CDSBackBuffer_DtorScalar(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042adb0 END

// !FUNC 0x0042ad40 BEGIN
/* 42AD40-42ADAB 0006B */
void CDSBackBuffer::CDSBackBuffer_dtor(void* param_1) { STUB_BODY(); }
// !FUNC 0x0042ad40 END

