#include "CDSSimpleException.h"

// !FUNC 0x00434ac0 BEGIN
/* 434AC0-434AC6 00006 */
uchar* CDSSimpleException::CDSSimpleException_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b81fc);
}
// !FUNC 0x00434ac0 END

// !FUNC 0x00434ad0 BEGIN
/* 434AD0-434AD7 00007 */
uint CDSSimpleException::CDSSimpleException_What(uint param_1) {
    return param_1;
}
// !FUNC 0x00434ad0 END

// !FUNC 0x00434c20 BEGIN
/* 434C20-434C62 00042 */
uchar CDSSimpleException::CDSSimpleException_Throw(uint param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00434c20 END

