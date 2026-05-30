#include "CBulAnim.h"

// !FUNC 0x0040ad20 BEGIN
/* 40AD20-40AD7D 0005D */
void CBulAnim::CleanupBody(void* param_1) { STUB_BODY(); }
// !FUNC 0x0040ad20 END

// !FUNC 0x0040afd0 BEGIN
/* 40AFD0-40B015 00045 */
uchar CBulAnim::ApplyTeamPalette(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040afd0 END

// !FUNC 0x0040b020 BEGIN
/* 40B020-40B04B 0002B */
uchar CBulAnim::OnTeamPaletteEvt(short param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0040b020 END

// !FUNC 0x0040b820 BEGIN
/* 40B820-40B826 00006 */
uchar* CBulAnim::GetTypeInfo_Sub68() {
    return reinterpret_cast<uchar*>(&DAT_004b82fc);
}
// !FUNC 0x0040b820 END

// !FUNC 0x0040b830 BEGIN
/* 40B830-40B836 00006 */
uchar* CBulAnim::GetTypeInfo_Sub78() {
    return reinterpret_cast<uchar*>(&DAT_004b8304);
}
// !FUNC 0x0040b830 END

// !FUNC 0x0040b840 BEGIN
/* 40B840-40B848 00008 */
uchar CBulAnim::PrimaryTick_thunk(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040b840 END

// !FUNC 0x0040b850 BEGIN
/* 40B850-40B86C 0001C */
uchar CBulAnim::PrimaryRender_thunk(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040b850 END

// !FUNC 0x0040b870 BEGIN
/* 40B870-40B876 00006 */
uchar* CBulAnim::PrimaryDtor_thunk() {
    return reinterpret_cast<uchar*>(&DAT_004b35a0);
}
// !FUNC 0x0040b870 END

// !FUNC 0x0040b880 BEGIN
/* 40B880-40B888 00008 */
uchar CBulAnim::Sub78_CDSObjectAdjust_thunk(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040b880 END

// !FUNC 0x0040b890 BEGIN
/* 40B890-40B898 00008 */
uchar CBulAnim::Sub78Dtor_thunk(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040b890 END

// !FUNC 0x0040b8a0 BEGIN
/* 40B8A0-40B8A8 00008 */
uchar CBulAnim::Sub68Dtor_thunk(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040b8a0 END

// !FUNC 0x0040eaa0 BEGIN
/* 40EAA0-40EB29 00089 */
uchar CBulAnim::CreateCBulAnim() { STUB_BODY(); return 0; }
// !FUNC 0x0040eaa0 END

// !FUNC 0x00438f20 BEGIN
/* 438F20-438F5D 0003D */
uchar CBulAnim::IDSAnim_NotifyEvents(uint param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00438f20 END

// !FUNC 0x00438f60 BEGIN
/* 438F60-438F80 00020 */
uchar CBulAnim::IDSAnim_BindUserData(uint param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00438f60 END

// !FUNC 0x00439010 BEGIN
/* 439010-439044 00034 */
void CBulAnim::SetPalette(uint* param_1) { STUB_BODY(); }
// !FUNC 0x00439010 END

// !FUNC 0x004391d0 BEGIN
/* 4391D0-4391E0 00010 */
uchar CBulAnim::IDSAnim_SetSequence_thunk(uint param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004391d0 END

// !FUNC 0x004392a0 BEGIN
/* 4392A0-4392DD 0003D */
uchar CBulAnim::IDSAnim_SelectRandomTrack(uint param_1, short param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004392a0 END


// !FUNC 0x00439530 BEGIN
/* 439530-439538 00008 */
uchar CBulAnim::CBulAnim_Sub10Dtor_thunk(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439530 END

// !FUNC 0x00439540 BEGIN
/* 439540-439548 00008 */
uchar CBulAnim::Sub18Dtor_thunk(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439540 END

// !FUNC 0x00439550 BEGIN
/* 439550-439558 00008 */
uchar CBulAnim::Sub78ChainOp_thunk(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439550 END

// !FUNC 0x004396f0 BEGIN
/* 4396F0-43970E 0001E */
void* CBulAnim::ScalarDeletingDtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004396f0 END

