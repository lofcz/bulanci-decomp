#include "CItemInfo.h"

// !FUNC 0x00403500 BEGIN
/* 403500-403506 00006 */
uchar* CItemInfo::CItemInfo_GetTypeInfo() {
    return reinterpret_cast<uchar*>(&DAT_004b33b8);
}
// !FUNC 0x00403500 END

// !FUNC 0x00434b10 BEGIN
/* 434B10-434B13 00003 */
uint CItemInfo::CDSObject_GetThis() {
    return reinterpret_cast<uint>(this);
}
// !FUNC 0x00434b10 END

