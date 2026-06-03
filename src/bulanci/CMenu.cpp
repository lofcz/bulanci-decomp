#include "CMenu.h"

// !FUNC 0x00401050 BEGIN
/* 401050-401057 00007 */
int CMenu::CMenu_GetAppSubstruct() {
    return reinterpret_cast<int>(this) + 0x2e6;
}
// !FUNC 0x00401050 END

// !FUNC 0x00401af0 BEGIN
/* 401AF0-401B70 00080 */
void CMenu::CMenu_OnSubScreenBack(uchar param_1) { STUB_BODY(); }
// !FUNC 0x00401af0 END

// !FUNC 0x00401b70 BEGIN
/* 401B70-401BB1 00041 */
uchar* CMenu::Catch_00401b70() { STUB_BODY(); return 0; }
// !FUNC 0x00401b70 END

// !FUNC 0x00401bb4 BEGIN
/* 401BB4-401BD3 0001F */
uchar CMenu::CMenu_HideAllButtons() { STUB_BODY(); return 0; }
// !FUNC 0x00401bb4 END





// !FUNC 0x00412720 BEGIN
/* 412720-41275A 0003A */
uchar CMenu::CMenu_InitPlayerSlotFromDefaults(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412720 END

// !FUNC 0x00412760 BEGIN
/* 412760-412771 00011 */
uint CMenu::CMenu_GetDigitFromVirtualKey(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412760 END

// !FUNC 0x00412780 BEGIN
/* 412780-4127BF 0003F */
uchar CMenu::CMenu_CleanupSlots(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412780 END

// !FUNC 0x004127d0 BEGIN
/* 4127D0-4127F3 00023 */
uchar CMenu::CMenu_ThrowModalDone(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004127d0 END


// !FUNC 0x00413620 BEGIN
/* 413620-41379F 0017F */
uchar CMenu::CGame_CopyPlayerBindingsFromProfile(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00413620 END

// !FUNC 0x00413b20 BEGIN
/* 413B20-413B8F 0006F */
uchar CMenu::CMenuGetResourceById(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00413b20 END


// !FUNC 0x00414232 BEGIN
/* 414232-414248 00016 */
uchar CMenu::Catch_00414232() { STUB_BODY(); return 0; }
// !FUNC 0x00414232 END

// !FUNC 0x00414640 BEGIN
/* 414640-4146AC 0006C */
uchar CMenu::CMenu_NetSendLobbySyncAll(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00414640 END


// !FUNC 0x00414847 BEGIN
/* 414847-414868 00021 */
uchar CMenu::Catch_00414847() { STUB_BODY(); return 0; }
// !FUNC 0x00414847 END

// !FUNC 0x00414dd0 BEGIN
/* 414DD0-41520D 0043D */
void CMenu::CMenu_OpenNetworkSession(CGame* param_1) { STUB_BODY(); }
// !FUNC 0x00414dd0 END

// !FUNC 0x0041520d BEGIN
/* 41520D-41521E 00011 */
uchar CMenu::Catch_0041520d() { STUB_BODY(); return 0; }
// !FUNC 0x0041520d END

// !FUNC 0x00423a00 BEGIN
/* 423A00-423AB9 000B9 */
uchar CMenu::CMenu_CloseCurrentSubScreen(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00423a00 END

// !FUNC 0x00423fa0 BEGIN
/* 423FA0-424005 00065 */
uchar CMenu::CMenu_SetButtonsHidden(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00423fa0 END

// !FUNC 0x00424080 BEGIN
/* 424080-4240F0 00070 */
void CMenu::CMenu_OnMusicFadeTick(uchar param_1) { STUB_BODY(); }
// !FUNC 0x00424080 END

// !FUNC 0x00424520 BEGIN
/* 424520-4245A9 00089 */
uchar CMenu::CMenu_ctor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00424520 END

// !FUNC 0x004245b0 BEGIN
/* 4245B0-4245B6 00006 */
uchar* CMenu::CMenu_GetSingleton() {
    return reinterpret_cast<uchar*>(&DAT_004b3984);
}
// !FUNC 0x004245b0 END

// !FUNC 0x004245d0 BEGIN
/* 4245D0-4245D8 00008 */
uchar CMenu::CMenu_VectorDtorWrapper_thunk_Sub4(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004245d0 END

// !FUNC 0x004245e0 BEGIN
/* 4245E0-4245E8 00008 */
uchar CMenu::CMenu_VectorDtorWrapper_thunk_Sub68(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004245e0 END

// !FUNC 0x004245f0 BEGIN
/* 4245F0-4245F8 00008 */
uchar CMenu::CMenu_VectorDtorWrapper_thunk_Sub10(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004245f0 END

// !FUNC 0x00424600 BEGIN
/* 424600-424608 00008 */
uchar CMenu::CMenu_VectorDtorWrapper_thunk_Sub18(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00424600 END

// !FUNC 0x00424e70 BEGIN
/* 424E70-424F4B 000DB */
uchar CMenu::CMenu_dtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00424e70 END

// !FUNC 0x00424f50 BEGIN
/* 424F50-424F9F 0004F */
uchar CMenu::CMenu_OnEvent(ushort param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00424f50 END

// !FUNC 0x00424fa0 BEGIN
/* 424FA0-425032 00092 */
char CMenu::CMenu_OnKeyDown(uchar param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00424fa0 END

// !FUNC 0x00425240 BEGIN
/* 425240-42525E 0001E */
uchar CMenu::CMenu_VectorDtorWrapper(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00425240 END

// !FUNC 0x00425340 BEGIN
/* 425340-4253CB 0008B */
void CMenu::CMenu_DispatchHotkey(ushort param_1, int param_2) { STUB_BODY(); }
// !FUNC 0x00425340 END

// !FUNC 0x00425870 BEGIN
/* 425870-42596B 000FB */
uchar CMenu::CMenu_LoadBackgroundResource(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00425870 END

// !FUNC 0x00425970 BEGIN
/* 425970-425C97 00327 */
uchar CMenu::CMenu_CmdDispatch(ushort param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00425970 END

// !FUNC 0x0042c000 BEGIN
/* 42C000-42C03A 0003A */
void CMenu::CMenu_DetachChild(uchar param_1) { STUB_BODY(); }
// !FUNC 0x0042c000 END

// !FUNC 0x0042d160 BEGIN
/* 42D160-42D19E 0003E */
uchar CMenu::CMenu_DetachChildWithVisibility(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042d160 END

// !FUNC 0x0043aba0 BEGIN
/* 43ABA0-43ABB1 00011 */
uchar CMenu::CDSDirectPlaySender_Bind(uint param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0043aba0 END

// !FUNC 0x0043abd0 BEGIN
/* 43ABD0-43ABFC 0002C */
uchar CMenu::CDSDirectPlay_InitializeConnection(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043abd0 END

// !FUNC 0x0043aca0 BEGIN
/* 43ACA0-43ACDE 0003E */
void CMenu::CDSDirectPlay_CreateLocalPlayer(uchar param_1) { STUB_BODY(); }
// !FUNC 0x0043aca0 END

// !FUNC 0x0043ae70 BEGIN
/* 43AE70-43AF00 00090 */
uchar CMenu::CDSDirectPlay_SetGuids(uint* param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0043ae70 END

// !FUNC 0x0043af00 BEGIN
/* 43AF00-43AF9B 0009B */
uchar CMenu::CDSDirectPlay_HostSession(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043af00 END

// !FUNC 0x0043afa0 BEGIN
/* 43AFA0-43B052 000B2 */
void CMenu::CDSDirectPlay_JoinSession(uint* param_1) { STUB_BODY(); }
// !FUNC 0x0043afa0 END

// !FUNC 0x0043b360 BEGIN
/* 43B360-43B4B1 00151 */
uchar CMenu::CDSDirectPlay_ConnectTCP(uchar param_1, uint* param_2, uint* param_3, char* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0043b360 END

// !FUNC 0x0043b620 BEGIN
/* 43B620-43B67D 0005D */
uchar CMenu::CDSDirectPlay_ConnectLobby(uint* param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0043b620 END

// !FUNC 0x004265e0 BEGIN
/* 4265E0-426CD9 006F9 */
uchar CMenu::CMenu_ctor_with_ui(uint param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004265e0 END

