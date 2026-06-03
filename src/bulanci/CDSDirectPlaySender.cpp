#include "CDSDirectPlaySender.h"

// !FUNC 0x0043ad20 BEGIN
/* 43AD20-43AD55 00035 */
void CDSDirectPlaySender::CDSDirectPlaySender_ThreadEntry(uchar param_1) { STUB_BODY(); }
// !FUNC 0x0043ad20 END

// !FUNC 0x0043b120 BEGIN
/* 43B120-43B126 00006 */
uchar* CDSDirectPlaySender::CDSDirectPlaySender_GetTypeID_thunk() {
    return reinterpret_cast<uchar*>(&DAT_004b8588);
}
// !FUNC 0x0043b120 END

// !FUNC 0x0043b130 BEGIN
/* 43B130-43B136 00006 */
uchar* CDSDirectPlaySender::CDSDirectPlaySender_GetTypeID() {
    return reinterpret_cast<uchar*>(&DAT_004b83dc);
}
// !FUNC 0x0043b130 END

// !FUNC 0x0043b140 BEGIN
/* 43B140-43B148 00008 */
uchar CDSDirectPlaySender::CDSDirectPlaySender_scalar_deleting_dtor_thunk(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043b140 END

// !FUNC 0x0043b150 BEGIN
/* 43B150-43B1FD 000AD */
void CDSDirectPlaySender::CDSDirectPlaySender_dtor_body(uchar param_1) { STUB_BODY(); }
// !FUNC 0x0043b150 END

// !FUNC 0x0043b530 BEGIN
/* 43B530-43B54E 0001E */
void* CDSDirectPlaySender::CDSDirectPlaySender_scalar_deleting_dtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043b530 END

