#include "CDSEasyMemStream.h"

// !FUNC 0x00409170 BEGIN
/* 409170-4091EE 0007E */
void* CDSEasyMemStream::CDSEasyMemStream_ctor(uchar* param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00409170 END

// !FUNC 0x004091f0 BEGIN
/* 4091F0-4091F6 00006 */
uchar* CDSEasyMemStream::CDSEasyMemStream_GetTypeInfo() {
    return reinterpret_cast<uchar*>(&DAT_004b7d34);
}
// !FUNC 0x004091f0 END

// !FUNC 0x00409200 BEGIN
/* 409200-409214 00014 */
uchar CDSEasyMemStream::CloseStream(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409200 END

// !FUNC 0x00409220 BEGIN
/* 409220-409225 00005 */
longlong CDSEasyMemStream::GetSize() {
    return *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0xc);
}
// !FUNC 0x00409220 END

// !FUNC 0x00409230 BEGIN
/* 409230-409235 00005 */
longlong CDSEasyMemStream::TellPosition() {
    return *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x8);
}
// !FUNC 0x00409230 END

// !FUNC 0x00409240 BEGIN
/* 409240-409248 00008 */
uchar CDSEasyMemStream::CDSEasyMemStream_ScalarDeletingDtor_thunk_Sub4(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409240 END

// !FUNC 0x00409250 BEGIN
/* 409250-409258 00008 */
uchar CDSEasyMemStream::CDSEasyMemStream_ScalarDeletingDtor_thunk(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409250 END

// !FUNC 0x00409270 BEGIN
/* 409270-4092E7 00077 */
uchar CDSEasyMemStream::CDSEasyMemStream_dtor(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409270 END

// !FUNC 0x004092f0 BEGIN
/* 4092F0-40930E 0001E */
void* CDSEasyMemStream::CDSEasyMemStream_ScalarDeletingDtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004092f0 END

// !FUNC 0x004306c0 BEGIN
/* 4306C0-4306DE 0001E */
uchar CDSEasyMemStream::ReleaseBackingBuffer(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004306c0 END

// !FUNC 0x004306e0 BEGIN
/* 4306E0-4307ED 0010D */
uchar CDSEasyMemStream::FUN_004306e0(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004306e0 END

// !FUNC 0x004307f0 BEGIN
/* 4307F0-4308BD 000CD */
uchar CDSEasyMemStream::ReadBytes(void* param_1, size_t param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004307f0 END

// !FUNC 0x004308c0 BEGIN
/* 4308C0-43097F 000BF */
uchar CDSEasyMemStream::WriteBytes(void* param_1, size_t param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004308c0 END

// !FUNC 0x00430980 BEGIN
/* 430980-4309EA 0006A */
uchar CDSEasyMemStream::SeekPosition(int param_1, uint param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00430980 END

// !FUNC 0x004309f0 BEGIN
/* 4309F0-430A3F 0004F */
uchar CDSEasyMemStream::SetStreamSize(uint param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004309f0 END

// !FUNC 0x00430a40 BEGIN
/* 430A40-430A67 00027 */
uchar CDSEasyMemStream::GetStreamName(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00430a40 END

// !FUNC 0x00430d60 BEGIN
/* 430D60-430DAE 0004E */
uchar CDSEasyMemStream::FUN_00430d60(uchar* param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00430d60 END

// !FUNC 0x00430db0 BEGIN
/* 430DB0-430DB7 00007 */
uchar CDSEasyMemStream::LockRegion(CDSFilterStream* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00430db0 END

// !FUNC 0x00430dc0 BEGIN
/* 430DC0-430DC7 00007 */
uchar CDSEasyMemStream::UnlockRegion(CDSFilterStream* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00430dc0 END

