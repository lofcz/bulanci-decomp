#include "CDSResourceSign.h"

// !PROLOGUE BEGIN
// _Globals helpers (FUN_0042d440 / FUN_0042e140 / FUN_0042e7b0) are reached
// through `this + offset` casts in the matched bodies below.
#include "_Globals.h"
// !PROLOGUE END

// !FUNC 0x0042e680 BEGIN
/* 42E680-42E693 00013 */
__declspec(noinline) uchar CDSResourceSign::CDSResourceSign_ReadDateField(int* param_1) {
    typedef uchar(__thiscall* PFN)(void*, void*, int);
    return reinterpret_cast<PFN*>(*param_1)[4](param_1, this, 4);
}
// !FUNC 0x0042e680 END

// !FUNC 0x0042e6a0 BEGIN
/* 42E6A0-42E6B3 00013 */
__declspec(noinline) uchar CDSResourceSign::CDSResourceSign_WriteDateField(int* param_1) {
    typedef uchar(__thiscall* PFN)(void*, void*, int);
    return reinterpret_cast<PFN*>(*param_1)[5](param_1, this, 4);
}
// !FUNC 0x0042e6a0 END

// !FUNC 0x00434310 BEGIN
/* 434310-434347 00037 */
uchar CDSResourceSign::CDSResourceSign_WriteToStream(int* param_1) {
    char* base = reinterpret_cast<char*>(this);
    reinterpret_cast<CDSResourceSign*>(base + 0x14)->CDSResourceSign_WriteDateField(param_1);
    reinterpret_cast<_Globals*>(base + 0x18)->FUN_0042d440(param_1);
    reinterpret_cast<_Globals*>(base + 0x1c)->FUN_0042d440(param_1);
    typedef uchar(__thiscall* PFN)(void*, void*, int);
    return reinterpret_cast<PFN*>(*param_1)[5](param_1, base + 0x20, 1);
}
// !FUNC 0x00434310 END

// !FUNC 0x004343a0 BEGIN
/* 4343A0-4343F5 00055 */
uchar CDSResourceSign::CDSResourceSign_ReadFromStream(int* param_1) {
    char* base = reinterpret_cast<char*>(this);
    reinterpret_cast<CDSResourceSign*>(base + 0x14)->CDSResourceSign_ReadDateField(param_1);
    reinterpret_cast<_Globals*>(base + 0x18)->FUN_0042e140(param_1);
    reinterpret_cast<_Globals*>(base + 0x1c)->FUN_0042e140(param_1);
    int threshold;
    reinterpret_cast<_Globals*>(&threshold)->FUN_0042e7b0(reinterpret_cast<uint*>(0x7d2), 3, 0xd);
    if (*reinterpret_cast<int*>(base + 0x14) >= threshold) {
        typedef uchar(__thiscall* PFN)(void*, void*, int);
        reinterpret_cast<PFN*>(*param_1)[4](param_1, base + 0x20, 1);
    }
    return 0;
}
// !FUNC 0x004343a0 END

// !FUNC 0x00434540 BEGIN
/* 434540-4345BE 0007E */
uint* CDSResourceSign::CDSResourceSign_ctor(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434540 END

// !FUNC 0x004345c0 BEGIN
/* 4345C0-4345C6 00006 */
uchar* CDSResourceSign::CDSResourceSign_GetClassData() {
    return reinterpret_cast<uchar*>(&DAT_004b7f48);
}
// !FUNC 0x004345c0 END

// !FUNC 0x004345d0 BEGIN
/* 4345D0-4345D8 00008 */
uchar CDSResourceSign::CDSResourceSign_scalar_deleting_dtor_thunk_n0x4(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004345d0 END

// !FUNC 0x004345e0 BEGIN
/* 4345E0-4345E8 00008 */
uchar CDSResourceSign::CDSResourceSign_scalar_deleting_dtor_thunk_n0x8(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004345e0 END

// !FUNC 0x004345f0 BEGIN
/* 4345F0-4345F8 00008 */
uchar CDSResourceSign::CDSResourceSign_scalar_deleting_dtor_thunk_n0x10(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004345f0 END

// !FUNC 0x00434610 BEGIN
/* 434610-434639 00029 */
uchar CDSResourceSign::CDSResourceSign_Release(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434610 END

// !FUNC 0x00434640 BEGIN
/* 434640-434648 00008 */
uchar CDSResourceSign::CDSResourceSign_Release_thunk_n0x4(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434640 END

// !FUNC 0x00434650 BEGIN
/* 434650-434658 00008 */
uchar CDSResourceSign::CDSResourceSign_Release_thunk_n0x8(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434650 END

// !FUNC 0x00434660 BEGIN
/* 434660-434668 00008 */
uchar CDSResourceSign::CDSResourceSign_Release_thunk_n0x10(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434660 END

// !FUNC 0x00434670 BEGIN
/* 434670-4346EC 0007C */
uchar CDSResourceSign::CDSResourceSign_dtor(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434670 END

// !FUNC 0x004349a0 BEGIN
/* 4349A0-4349BE 0001E */
uint* CDSResourceSign::CDSResourceSign_scalar_deleting_dtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004349a0 END

