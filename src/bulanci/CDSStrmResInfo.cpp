#include "CDSStrmResInfo.h"

// !FUNC 0x00433940 BEGIN
/* 433940-433971 00031 */
uchar CDSStrmResInfo::CDSStrmResInfo_Serialize(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00433940 END

// !FUNC 0x00433980 BEGIN
/* 433980-4339B1 00031 */
void CDSStrmResInfo::CDSStrmResInfo_Deserialize(CDSStreamStorage* param_1) { STUB_BODY(); }
// !FUNC 0x00433980 END

// !FUNC 0x00433a90 BEGIN
/* 433A90-433A96 00006 */
uchar* CDSStrmResInfo::CDSStrmResInfo_GetTypeInfo() {
    return reinterpret_cast<uchar*>(&DAT_004b7ef0);
}
// !FUNC 0x00433a90 END

