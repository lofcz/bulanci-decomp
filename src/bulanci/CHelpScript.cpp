#include "CHelpScript.h"

// !FUNC 0x004215e0 BEGIN
/* 4215E0-421672 00092 */
void* CHelpScript::CHelpScript_ctor(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004215e0 END

// !FUNC 0x00421680 BEGIN
/* 421680-421686 00006 */
uchar* CHelpScript::CHelpScript_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b3894);
}
// !FUNC 0x00421680 END

// !FUNC 0x00421690 BEGIN
/* 421690-42169B 0000B */
uchar CHelpScript::CHelpScript_ScalarDeletingDtor_thunk(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00421690 END

// !FUNC 0x004216a0 BEGIN
/* 4216A0-4216AB 0000B */
uchar CHelpScript::CHelpScript_ScalarDeletingDtor_thunk_Sub438(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004216a0 END

// !FUNC 0x004216b0 BEGIN
/* 4216B0-4216B8 00008 */
uchar CHelpScript::CHelpScript_AdjustOffsetDtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004216b0 END

// !FUNC 0x004216f0 BEGIN
/* 4216F0-42174D 0005D */
void CHelpScript::CHelpScript_dtor(uchar param_1) { STUB_BODY(); }
// !FUNC 0x004216f0 END

// !FUNC 0x00421920 BEGIN
/* 421920-42193E 0001E */
void* CHelpScript::CHelpScript_ScalarDeletingDtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00421920 END

// !FUNC 0x00421940 BEGIN
/* 421940-4219FD 000BD */
uchar CHelpScript::HelpBuildStaticTextAuto(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00421940 END

// !FUNC 0x00421a00 BEGIN
/* 421A00-421AEF 000EF */
uchar CHelpScript::HelpBuildStaticTextSized(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00421a00 END

// !FUNC 0x00421af0 BEGIN
/* 421AF0-421BA0 000B0 */
uchar CHelpScript::HelpBuildHelpButtonWidget(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00421af0 END

// !FUNC 0x004221a0 BEGIN
/* 4221A0-422278 000D8 */
CDSBitmap* CHelpScript::HelpBuildHelpBitmapWidget(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004221a0 END

// !FUNC 0x004217e0 BEGIN
/* 4217E0-421802 00022 */
void* CHelpScript::HhAddChildToParentView(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004217e0 END

// !FUNC 0x00422620 BEGIN
/* 422620-42262D 0000D */
uchar CHelpScript::CDSScript_SetBoundParentView(uint param_1) {
    *reinterpret_cast<uint*>(reinterpret_cast<char*>(this) + 0x440) = param_1;
    return static_cast<uchar>(param_1);
}
// !FUNC 0x00422620 END

