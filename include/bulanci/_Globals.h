#ifndef __GLOBALS
#define __GLOBALS

#include <globals.h>

/* Forward declarations of types referenced by parameter / return
 * positions but not (yet) defined in this project. Stubs only use
 * pointers so a struct forward-decl is enough. */
struct EHExceptionRecord;
struct EHRegistrationNode;
struct _CONTEXT;
struct _CRT_DOUBLE;
struct _CRT_FLOAT;
struct _LDBL12;
struct FILE;
struct localeinfo_struct;

#include "bulanci_fwd.h"

class _Globals {
public:
	// !DECL 0x00401000 BEGIN
	/* 401000 */ void AtlThrow(HRESULT param_1);
	// !DECL 0x00401000 END
	// !DECL 0x00401020 BEGIN
	/* 401020 */ void ThrowWin32ErrorAsHRESULT();
	// !DECL 0x00401020 END
	// !DECL 0x00401060 BEGIN
	/* 401060 */ static uchar CDsString_EnsureCapacityWide(int* param_1, size_t param_2, void* param_3, int param_4);
	// !DECL 0x00401060 END
	// !DECL 0x004010f0 BEGIN
	/* 4010F0 */ int CIntList_BinarySearch(int* param_1, uchar* param_2, int param_3);
	// !DECL 0x004010f0 END
	// !DECL 0x00401190 BEGIN
	/* 401190 */ void* CDsString_InitFromLiteral(short* param_1);
	// !DECL 0x00401190 END
	// !DECL 0x004011b0 BEGIN
	/* 4011B0 */ void* CDsString_InitFromHandle(uint* param_1);
	// !DECL 0x004011b0 END
	// !DECL 0x004011d0 BEGIN
	/* 4011D0 */ void eh_CDSStreamStorage_DeleteCriticalSection(void* param_1);
	// !DECL 0x004011d0 END
	// !DECL 0x00401290 BEGIN
	/* 401290 */ uchar MultiByteToWideChar_Wrapper(int param_1, int param_2);
	// !DECL 0x00401290 END
	// !DECL 0x00401340 BEGIN
	/* 401340 */ void CDsString_EhVectorElemDtor(int* param_1);
	// !DECL 0x00401340 END
	// !DECL 0x00401450 BEGIN
	/* 401450 */ uchar CBulanci_DirEnumCtx_dtor(int* param_1);
	// !DECL 0x00401450 END
	// !DECL 0x00401520 BEGIN
	/* 401520 */ void Runtime_HeapPtr_EH_dtor(uint* param_1);
	// !DECL 0x00401520 END
	// !DECL 0x00401910 BEGIN
	/* 401910 */ uchar CWindow_dtor(int param_1);
	// !DECL 0x00401910 END
	// !DECL 0x00401a70 BEGIN
	/* 401A70 */ uint CDSException_TopLevelFilter(CDSException* param_1);
	// !DECL 0x00401a70 END
	// !DECL 0x00401d30 BEGIN
	/* 401D30 */ uchar CIntList_EnsureCapacity(uint param_1);
	// !DECL 0x00401d30 END
	// !DECL 0x00401d70 BEGIN
	/* 401D70 */ void eh_CDSPtrSlotVec_ResizeZero(CDSPtrSlotVec* param_1);
	// !DECL 0x00401d70 END
	// !DECL 0x00401fc3 BEGIN
	/* 401FC3 */ uchar* Catch_00401fc3();
	// !DECL 0x00401fc3 END
	// !DECL 0x00402290 BEGIN
	/* 402290 */ uchar CDSApp_ShowSetupDialog(int param_1);
	// !DECL 0x00402290 END
	// !DECL 0x00402680 BEGIN
	/* 402680 */ int WinMain(HINSTANCE param_1, HINSTANCE param_2, int param_3, int param_4);
	// !DECL 0x00402680 END
	// !DECL 0x00402a90 BEGIN
	/* 402A90 */ uint CBulanci_CreateObject();
	// !DECL 0x00402a90 END
	// !DECL 0x00402e20 BEGIN
	/* 402E20 */ uchar CScroller_DtorCommon(int param_1, uint param_2);
	// !DECL 0x00402e20 END
	// !DECL 0x00402f40 BEGIN
	/* 402F40 */ static int WidgetStateFlags_ToTintColor(ushort param_1);
	// !DECL 0x00402f40 END
	// !DECL 0x004031b0 BEGIN
	/* 4031B0 */ static uchar CDsString_EnsureCapacityAnsi(int* param_1, size_t param_2, void* param_3, int param_4);
	// !DECL 0x004031b0 END
	// !DECL 0x00404100 BEGIN
	/* 404100 */ int CListViewer_GetVerticalScrollbarHeight(int param_1);
	// !DECL 0x00404100 END
	// !DECL 0x00404320 BEGIN
	/* 404320 */ uchar CListViewer_ScrollToItem(int param_1);
	// !DECL 0x00404320 END
	// !DECL 0x00404390 BEGIN
	/* 404390 */ uchar CListViewer_UpdateContentSize(int* param_1);
	// !DECL 0x00404390 END
	// !DECL 0x004045f0 BEGIN
	/* 4045F0 */ uchar CDsString_AssignFromWideCapped80(int param_1, int param_2);
	// !DECL 0x004045f0 END
	// !DECL 0x004046c0 BEGIN
	/* 4046C0 */ uchar CDynPtrArray_RemoveRange(int param_1, int param_2);
	// !DECL 0x004046c0 END
	// !DECL 0x00404700 BEGIN
	/* 404700 */ uchar CDSRect_Assign(uint* param_1);
	// !DECL 0x00404700 END
	// !DECL 0x00404730 BEGIN
	/* 404730 */ uint CDSRect_Overlaps(tagRECT* param_1);
	// !DECL 0x00404730 END
	// !DECL 0x00404890 BEGIN
	/* 404890 */ uchar CDSView_UnwindDtor_StringVec9(int param_1);
	// !DECL 0x00404890 END
	// !DECL 0x00404be0 BEGIN
	/* 404BE0 */ uint CreateObject();
	// !DECL 0x00404be0 END
	// !DECL 0x00405370 BEGIN
	/* 405370 */ uchar CStaticText_SetLabelFromStringHandle(int* param_1);
	// !DECL 0x00405370 END
	// !DECL 0x00405bc0 BEGIN
	/* 405BC0 */ uchar CListViewer_ReindexItemsFrom(int param_1);
	// !DECL 0x00405bc0 END
	// !DECL 0x00405bf0 BEGIN
	/* 405BF0 */ uchar CListViewer_ClearSelectedItems(void* param_1);
	// !DECL 0x00405bf0 END
	// !DECL 0x00405c40 BEGIN
	/* 405C40 */ uchar CScroller_ClearContent(CScroller* param_1);
	// !DECL 0x00405c40 END
	// !DECL 0x00406300 BEGIN
	/* 406300 */ uchar CDynPtrArray_RemovePointer(int param_1, uchar* param_2);
	// !DECL 0x00406300 END
	// !DECL 0x004063c0 BEGIN
	/* 4063C0 */ uint CreateObject_004063c0();
	// !DECL 0x004063c0 END
	// !DECL 0x00406450 BEGIN
	/* 406450 */ CStaticText* CreateObject_00406450();
	// !DECL 0x00406450 END
	// !DECL 0x00406570 BEGIN
	/* 406570 */ CIcon* CreateObject_00406570();
	// !DECL 0x00406570 END
	// !DECL 0x00406600 BEGIN
	/* 406600 */ CButton* CreateObject_00406600();
	// !DECL 0x00406600 END
	// !DECL 0x004066e0 BEGIN
	/* 4066E0 */ uint CreateObject_004066e0();
	// !DECL 0x004066e0 END
	// !DECL 0x004067c0 BEGIN
	/* 4067C0 */ uchar CScrollBar_NudgeOnArrowHold(char param_1);
	// !DECL 0x004067c0 END
	// !DECL 0x00406810 BEGIN
	/* 406810 */ uchar CScrollBar_SetDragMode(uint param_1, uint* param_2);
	// !DECL 0x00406810 END
	// !DECL 0x00406a20 BEGIN
	/* 406A20 */ uchar CRadio_OnMouseMove(int* param_1);
	// !DECL 0x00406a20 END
	// !DECL 0x00407020 BEGIN
	/* 407020 */ uchar CEdit_LayoutToCaretNoAnchor(CEdit* param_1);
	// !DECL 0x00407020 END
	// !DECL 0x004070c0 BEGIN
	/* 4070C0 */ uint CEdit_SubmitText(int param_1, char param_2);
	// !DECL 0x004070c0 END
	// !DECL 0x004072b0 BEGIN
	/* 4072B0 */ int CIntList_InsertAt(uint param_1, int param_2);
	// !DECL 0x004072b0 END
	// !DECL 0x00407710 BEGIN
	/* 407710 */ uchar CListViewer_InsertItemAt(uint param_1, int param_2);
	// !DECL 0x00407710 END
	// !DECL 0x00407e10 BEGIN
	/* 407E10 */ void CDSPtrSlotVec_EH_dtor(CDSPtrSlotVec* param_1);
	// !DECL 0x00407e10 END
	// !DECL 0x00407e20 BEGIN
	/* 407E20 */ int CIntListInsertSortedOrAppend(int param_1, uchar* param_2, int param_3);
	// !DECL 0x00407e20 END
	// !DECL 0x00407ec0 BEGIN
	/* 407EC0 */ uint CreateObject_00407ec0();
	// !DECL 0x00407ec0 END
	// !DECL 0x004080d0 BEGIN
	/* 4080D0 */ uchar CListViewer_SetItemSelected(int param_1, char param_2);
	// !DECL 0x004080d0 END
	// !DECL 0x004082c0 BEGIN
	/* 4082C0 */ uint* CreateObject_004082c0();
	// !DECL 0x004082c0 END
	// !DECL 0x00408ea0 BEGIN
	/* 408EA0 */ CLevelScript* CreateObject_00408ea0();
	// !DECL 0x00408ea0 END
	// !DECL 0x004090c0 BEGIN
	/* 4090C0 */ uchar CBulanci_ClearProfileKeyList(int param_1);
	// !DECL 0x004090c0 END
	// !DECL 0x00409110 BEGIN
	/* 409110 */ uchar CDsString_WriteHandleToStream(int param_1);
	// !DECL 0x00409110 END
	// !DECL 0x00409510 BEGIN
	/* 409510 */ void* CDSRegKey_ctor(int param_1, int param_2, uchar* param_3);
	// !DECL 0x00409510 END
	// !DECL 0x004095b0 BEGIN
	/* 4095B0 */ uchar CDSRegKeyQueryScope_dtor(int* param_1);
	// !DECL 0x004095b0 END
	// !DECL 0x00409f20 BEGIN
	/* 409F20 */ uchar CBulanci_ClearLevelScriptList(int param_1);
	// !DECL 0x00409f20 END
	// !DECL 0x0040a380 BEGIN
	/* 40A380 */ void CBulanci_DestroyConfigStore(CDSChain_full* param_1);
	// !DECL 0x0040a380 END
	// !DECL 0x0040a7d0 BEGIN
	/* 40A7D0 */ uchar CDSAnim_CleanupBody(int param_1);
	// !DECL 0x0040a7d0 END
	// !DECL 0x0040ad10 BEGIN
	/* 40AD10 */ uchar Unwind_CDSVideoPlayer_TM(int param_1);
	// !DECL 0x0040ad10 END
	// !DECL 0x0040bb90 BEGIN
	/* 40BB90 */ uchar CStartGame2_dtor(int param_1, uint param_2);
	// !DECL 0x0040bb90 END
	// !DECL 0x0040bce0 BEGIN
	/* 40BCE0 */ uchar CGameCounter_dtor(int param_1);
	// !DECL 0x0040bce0 END
	// !DECL 0x0040e810 BEGIN
	/* 40E810 */ uint CreateObject_0040e810();
	// !DECL 0x0040e810 END
	// !DECL 0x0040e980 BEGIN
	/* 40E980 */ CSessionList* CreateObject_0040e980();
	// !DECL 0x0040e980 END
	// !DECL 0x0040ed20 BEGIN
	/* 40ED20 */ uint CreateObject_0040ed20();
	// !DECL 0x0040ed20 END
	// !DECL 0x0040eed0 BEGIN
	/* 40EED0 */ CGameCounter* CreateObject_0040eed0();
	// !DECL 0x0040eed0 END
	// !DECL 0x0040ef60 BEGIN
	/* 40EF60 */ CWindow* CreateObject_0040ef60();
	// !DECL 0x0040ef60 END
	// !DECL 0x0040f1e0 BEGIN
	/* 40F1E0 */ CSetupDlg* CSetupDlg_CreateObject();
	// !DECL 0x0040f1e0 END
	// !DECL 0x0040f250 BEGIN
	/* 40F250 */ CAdvertising* CreateObject_0040f250();
	// !DECL 0x0040f250 END
	// !DECL 0x0040f2c0 BEGIN
	/* 40F2C0 */ static uint CMsgDialog_ShowModalFromStringHandle(int param_1);
	// !DECL 0x0040f2c0 END
	// !DECL 0x0040f380 BEGIN
	/* 40F380 */ static void CSessionList_AppendEnumSession(DPEnumSessionInfo* param_1, uchar param_2);
	// !DECL 0x0040f380 END
	// !DECL 0x0040fb90 BEGIN
	/* 40FB90 */ CStartGame1* CreateObject_0040fb90();
	// !DECL 0x0040fb90 END
	// !DECL 0x004102d0 BEGIN
	/* 4102D0 */ static void CStartGame2_BuildLobbyChatPanel(CStartGame2* param_1, uchar param_2, int param_3, int param_4);
	// !DECL 0x004102d0 END
	// !DECL 0x004120f0 BEGIN
	/* 4120F0 */ CExitDlg* CreateObject_004120f0();
	// !DECL 0x004120f0 END
	// !DECL 0x004123d0 BEGIN
	/* 4123D0 */ uint* CreateObject_004123d0();
	// !DECL 0x004123d0 END
	// !DECL 0x00412460 BEGIN
	/* 412460 */ uchar CGame_TeardownNetworkSession(int param_1);
	// !DECL 0x00412460 END
	// !DECL 0x00412490 BEGIN
	/* 412490 */ uchar GetMaxAmmoForKind(uchar param_1);
	// !DECL 0x00412490 END
	// !DECL 0x004124a0 BEGIN
	/* 4124A0 */ uchar CGame_NetSendKick_t0a(uint param_1);
	// !DECL 0x004124a0 END
	// !DECL 0x00412590 BEGIN
	/* 412590 */ uint CGameGetPlayerMidGameStatusByte(uchar param_1);
	// !DECL 0x00412590 END
	// !DECL 0x004125b0 BEGIN
	/* 4125B0 */ uint CGame_IsPlayerAliveForLastMan(uchar param_1);
	// !DECL 0x004125b0 END
	// !DECL 0x004125f0 BEGIN
	/* 4125F0 */ bool CGame_AllSlotsHaveState(char param_1);
	// !DECL 0x004125f0 END
	// !DECL 0x00412640 BEGIN
	/* 412640 */ uint CMenu_GetProfileBindIndex(int param_1);
	// !DECL 0x00412640 END
	// !DECL 0x00412660 BEGIN
	/* 412660 */ uchar CGameGetModeAndScoreLimit(int* param_1);
	// !DECL 0x00412660 END
	// !DECL 0x004128a0 BEGIN
	/* 4128A0 */ uchar CGame_SetTeamScore(uchar param_1, int param_2);
	// !DECL 0x004128a0 END
	// !DECL 0x004128f0 BEGIN
	/* 4128F0 */ uint CGame_IsLocalPlayerSlot(uchar param_1);
	// !DECL 0x004128f0 END
	// !DECL 0x00412910 BEGIN
	/* 412910 */ wchar_t** CGameGetPlayerNamePtr(uchar param_1);
	// !DECL 0x00412910 END
	// !DECL 0x00412930 BEGIN
	/* 412930 */ uchar CGameGetPlayerColorByte(uchar param_1);
	// !DECL 0x00412930 END
	// !DECL 0x00412950 BEGIN
	/* 412950 */ uchar CGame_NetSendWorldEvent_t13(ushort* param_1);
	// !DECL 0x00412950 END
	// !DECL 0x00412990 BEGIN
	/* 412990 */ uchar CGame_NetSendPlayerEvent2_t14(uchar param_1);
	// !DECL 0x00412990 END
	// !DECL 0x00412a00 BEGIN
	/* 412A00 */ uchar CGame_NetSendRoundTimer_t12(ushort param_1);
	// !DECL 0x00412a00 END
	// !DECL 0x00412a80 BEGIN
	/* 412A80 */ uchar CGame_NetSendTriByteEvent_t19(uchar param_1, uchar param_2, uchar param_3);
	// !DECL 0x00412a80 END
	// !DECL 0x00412ac0 BEGIN
	/* 412AC0 */ uchar CGame_NetSendPlaceObject_t18(uchar param_1, uchar param_2, ushort* param_3);
	// !DECL 0x00412ac0 END
	// !DECL 0x00412b60 BEGIN
	/* 412B60 */ uchar CGame_NetSendPlayerEvent1_t0e(uchar param_1);
	// !DECL 0x00412b60 END
	// !DECL 0x00412b90 BEGIN
	/* 412B90 */ uchar CGame_NetSendShotSpawn_t0f(uchar param_1, uchar param_2, uchar param_3, ushort* param_4);
	// !DECL 0x00412b90 END
	// !DECL 0x00412bf0 BEGIN
	/* 412BF0 */ uchar CGame_NetSendHit_t10(uchar param_1, uchar param_2, ushort* param_3);
	// !DECL 0x00412bf0 END
	// !DECL 0x00412df0 BEGIN
	/* 412DF0 */ uchar CGame_NetSendCustomScriptPayload_t15(int* param_1);
	// !DECL 0x00412df0 END
	// !DECL 0x00412f40 BEGIN
	/* 412F40 */ static uchar CDsStringHandleVector_MoveTail(void* param_1, void* param_2, int param_3);
	// !DECL 0x00412f40 END
	// !DECL 0x004130e0 BEGIN
	/* 4130E0 */ uchar CGame_ClearSchedulerChildren(int param_1);
	// !DECL 0x004130e0 END
	// !DECL 0x00413200 BEGIN
	/* 413200 */ static uchar CGame_CopyDefaultPaletteSeed(uint* param_1, int param_2);
	// !DECL 0x00413200 END
	// !DECL 0x00413220 BEGIN
	/* 413220 */ static uchar CGame_BuildPaletteLut(uint* param_1, uint param_2);
	// !DECL 0x00413220 END
	// !DECL 0x00413410 BEGIN
	/* 413410 */ static uchar CLevelNameList_ZeroNewElements(uint* param_1, int param_2);
	// !DECL 0x00413410 END
	// !DECL 0x00413440 BEGIN
	/* 413440 */ int CLevelNameList_FindIndex(uint* param_1, uchar* param_2, int param_3);
	// !DECL 0x00413440 END
	// !DECL 0x004134f0 BEGIN
	/* 4134F0 */ uchar Runtime_FreePointerFieldZero(uint* param_1);
	// !DECL 0x004134f0 END
	// !DECL 0x00413510 BEGIN
	/* 413510 */ uchar CGame_ResetForLobby(int param_1);
	// !DECL 0x00413510 END
	// !DECL 0x00413560 BEGIN
	/* 413560 */ int CGame_FindResourceByName(int param_1);
	// !DECL 0x00413560 END
	// !DECL 0x004137b0 BEGIN
	/* 4137B0 */ uchar CMenu_ShowConnectingDialog(uchar param_1);
	// !DECL 0x004137b0 END
	// !DECL 0x00413c50 BEGIN
	/* 413C50 */ static uchar CGame_BuildTeamPaletteLut(uint* param_1, int param_2);
	// !DECL 0x00413c50 END
	// !DECL 0x00413c70 BEGIN
	/* 413C70 */ int CLevelNameList_FindIndexWithKey(int param_1, uchar* param_2);
	// !DECL 0x00413c70 END
	// !DECL 0x004140fa BEGIN
	/* 4140FA */ uchar Catch_004140fa();
	// !DECL 0x004140fa END
	// !DECL 0x0041415e BEGIN
	/* 41415E */ uchar Catch_0041415e();
	// !DECL 0x0041415e END
	// !DECL 0x00414250 BEGIN
	/* 414250 */ uchar CGame_SetCommStrm(int* param_1);
	// !DECL 0x00414250 END
	// !DECL 0x004144c0 BEGIN
	/* 4144C0 */ static uchar CDsStringPtrArray_ReleaseRange(int param_1, int param_2);
	// !DECL 0x004144c0 END
	// !DECL 0x004146b0 BEGIN
	/* 4146B0 */ uchar CLevelNameList_Resize(int param_1);
	// !DECL 0x004146b0 END
	// !DECL 0x00414730 BEGIN
	/* 414730 */ uchar CGame_LevelList_EnsureCapacity(uint param_1);
	// !DECL 0x00414730 END
	// !DECL 0x00414770 BEGIN
	/* 414770 */ uchar CDSPtrSlotVec_ResetOnUnwind(int param_1);
	// !DECL 0x00414770 END
	// !DECL 0x00414870 BEGIN
	/* 414870 */ int CDsStringHandleVector_InsertAt(int param_1, int param_2);
	// !DECL 0x00414870 END
	// !DECL 0x00414910 BEGIN
	/* 414910 */ uchar CLevelNameList_Clear(void* param_1);
	// !DECL 0x00414910 END
	// !DECL 0x00414920 BEGIN
	/* 414920 */ int CGame_LevelList_AddByName(int param_1, uchar* param_2, uchar* param_3);
	// !DECL 0x00414920 END
	// !DECL 0x00415ef0 BEGIN
	/* 415EF0 */ CGame* CreateObject_00415ef0();
	// !DECL 0x00415ef0 END
	// !DECL 0x00416510 BEGIN
	/* 416510 */ uchar CDSView_DtorTeardownAnimInner(int param_1);
	// !DECL 0x00416510 END
	// !DECL 0x004165b0 BEGIN
	/* 4165B0 */ uchar CDSView_PostEntityDetachMessage_f9(int param_1);
	// !DECL 0x004165b0 END
	// !DECL 0x004165d0 BEGIN
	/* 4165D0 */ uchar CTeleportPoint_TryActivateFromOverlap(uint param_1);
	// !DECL 0x004165d0 END
	// !DECL 0x00416610 BEGIN
	/* 416610 */ uchar CBulanek_NetSendTeamScoreOnCollect(uint param_1);
	// !DECL 0x00416610 END
	// !DECL 0x00416640 BEGIN
	/* 416640 */ uchar CBulanek_GetAmmoForKind(uchar param_1);
	// !DECL 0x00416640 END
	// !DECL 0x00416670 BEGIN
	/* 416670 */ uint CBulanek_GetSpawnQuipSlot(int param_1);
	// !DECL 0x00416670 END
	// !DECL 0x00416720 BEGIN
	/* 416720 */ uint CBulanek_IsHumanPlayer(int param_1);
	// !DECL 0x00416720 END
	// !DECL 0x00416780 BEGIN
	/* 416780 */ uchar CWeapon_PlayFireAnim(int param_1);
	// !DECL 0x00416780 END
	// !DECL 0x004167e0 BEGIN
	/* 4167E0 */ uchar DefineTraceArea_PostTransitionEvent(int param_1, char param_2);
	// !DECL 0x004167e0 END
	// !DECL 0x004168d0 BEGIN
	/* 4168D0 */ uint CGaming_GetObjectAtSlotUnchecked(uchar param_1);
	// !DECL 0x004168d0 END
	// !DECL 0x00416970 BEGIN
	/* 416970 */ int CGaming_CountOccupiedPlayerSlots(int param_1);
	// !DECL 0x00416970 END
	// !DECL 0x00416ce0 BEGIN
	/* 416CE0 */ static uint CDSScript_ReadKindAndRect4(uint* param_1, CDSScript* param_2);
	// !DECL 0x00416ce0 END
	// !DECL 0x004171b0 BEGIN
	/* 4171B0 */ uchar CAnim_dtor(int param_1);
	// !DECL 0x004171b0 END
	// !DECL 0x00417260 BEGIN
	/* 417260 */ uchar CBulanek_ArmFireDelayScheduler(int param_1);
	// !DECL 0x00417260 END
	// !DECL 0x004173f0 BEGIN
	/* 4173F0 */ uchar CBulanek_HasAmmoForCurrentWeapon(void* param_1);
	// !DECL 0x004173f0 END
	// !DECL 0x004174a0 BEGIN
	/* 4174A0 */ uint CBulanek_CanDispatchPlayerAction(int param_1, char param_2);
	// !DECL 0x004174a0 END
	// !DECL 0x00417500 BEGIN
	/* 417500 */ uchar CBulanek_DecrementWeaponAmmo(void* param_1);
	// !DECL 0x00417500 END
	// !DECL 0x00417950 BEGIN
	/* 417950 */ static void TriggerSoundEffect(int param_1, uchar param_2, int* param_3);
	// !DECL 0x00417950 END
	// !DECL 0x004179c0 BEGIN
	/* 4179C0 */ static uchar CBulanek_PlayItemPickupSound(uchar param_1, int* param_2);
	// !DECL 0x004179c0 END
	// !DECL 0x00417a20 BEGIN
	/* 417A20 */ uchar CWeapon_FireMachineGunBurstStart(int param_1);
	// !DECL 0x00417a20 END
	// !DECL 0x00417ce0 BEGIN
	/* 417CE0 */ uchar CGaming_FreeDangerZoneNodes(int param_1);
	// !DECL 0x00417ce0 END
	// !DECL 0x00417d20 BEGIN
	/* 417D20 */ uchar CLevelScript_DispatchTraceAreaTransition(uint param_1, uint* param_2, char param_3);
	// !DECL 0x00417d20 END
	// !DECL 0x00417dd0 BEGIN
	/* 417DD0 */ uchar CGaming_DispatchTraceAreaTransitionIfMasked(uint param_1, char param_2, uint* param_3, char param_4);
	// !DECL 0x00417dd0 END
	// !DECL 0x00417e80 BEGIN
	/* 417E80 */ uchar CGaming_OnSlotPlacementEvent(uchar param_1, uchar param_2, uchar param_3, int* param_4, char param_5);
	// !DECL 0x00417e80 END
	// !DECL 0x00417fb0 BEGIN
	/* 417FB0 */ uchar CGaming_RegisterObjectAtSlot(int param_1, uchar param_2);
	// !DECL 0x00417fb0 END
	// !DECL 0x00418000 BEGIN
	/* 418000 */ uchar TriggerLevelScriptSound(uchar param_1, int param_2);
	// !DECL 0x00418000 END
	// !DECL 0x00418bbc BEGIN
	/* 418BBC */ uchar* Catch_00418bbc();
	// !DECL 0x00418bbc END
	// !DECL 0x00418d00 BEGIN
	/* 418D00 */ CGameView* CreateObject_00418d00();
	// !DECL 0x00418d00 END
	// !DECL 0x00418f70 BEGIN
	/* 418F70 */ uint CreateObject_00418f70();
	// !DECL 0x00418f70 END
	// !DECL 0x00418fe0 BEGIN
	/* 418FE0 */ uchar CGameEntity_SetEntityType(uint param_1);
	// !DECL 0x00418fe0 END
	// !DECL 0x00419530 BEGIN
	/* 419530 */ uint CBulanek_TestRectOverlapWithEntity(uint param_1);
	// !DECL 0x00419530 END
	// !DECL 0x004195f0 BEGIN
	/* 4195F0 */ uchar CBulanek_ClampMoveRectByCollision(uint param_1, int* param_2, int* param_3, int* param_4);
	// !DECL 0x004195f0 END
	// !DECL 0x00419d60 BEGIN
	/* 419D60 */ uchar CGaming_OnTimerCountdown(short param_1, uint param_2, int param_3);
	// !DECL 0x00419d60 END
	// !DECL 0x00419e90 BEGIN
	/* 419E90 */ uchar CGaming_CheckTraceAreasForEntity(uint* param_1, uint param_2, int* param_3, char param_4);
	// !DECL 0x00419e90 END
	// !DECL 0x0041a0f0 BEGIN
	/* 41A0F0 */ uint CGaming_TryGetPlayerCoords(uchar param_1, uint* param_2);
	// !DECL 0x0041a0f0 END
	// !DECL 0x0041a6a0 BEGIN
	/* 41A6A0 */ CGameView* CreateObject_0041a6a0();
	// !DECL 0x0041a6a0 END
	// !DECL 0x0041a710 BEGIN
	/* 41A710 */ uint* CreateObject_0041a710();
	// !DECL 0x0041a710 END
	// !DECL 0x0041a780 BEGIN
	/* 41A780 */ CAnim* CreateObject_0041a780();
	// !DECL 0x0041a780 END
	// !DECL 0x0041a7f0 BEGIN
	/* 41A7F0 */ uint CreateObject_0041a7f0();
	// !DECL 0x0041a7f0 END
	// !DECL 0x0041ad80 BEGIN
	/* 41AD80 */ static uchar Scheduler_EnqueueEvent(void* param_1, uint param_2, uint param_3);
	// !DECL 0x0041ad80 END
	// !DECL 0x0041af70 BEGIN
	/* 41AF70 */ uint CBulanek_StepMovementAndCollision(CBulanek* param_1);
	// !DECL 0x0041af70 END
	// !DECL 0x0041bc40 BEGIN
	/* 41BC40 */ uint CreateObject_0041bc40();
	// !DECL 0x0041bc40 END
	// !DECL 0x0041bcb0 BEGIN
	/* 41BCB0 */ uint* CreateObject_0041bcb0();
	// !DECL 0x0041bcb0 END
	// !DECL 0x0041bd20 BEGIN
	/* 41BD20 */ uint* CreateObject_0041bd20();
	// !DECL 0x0041bd20 END
	// !DECL 0x0041bd90 BEGIN
	/* 41BD90 */ uint CreateObject_0041bd90();
	// !DECL 0x0041bd90 END
	// !DECL 0x0041be00 BEGIN
	/* 41BE00 */ uint* CreateObject_0041be00();
	// !DECL 0x0041be00 END
	// !DECL 0x0041be70 BEGIN
	/* 41BE70 */ uint CGaming_CreateObject();
	// !DECL 0x0041be70 END
	// !DECL 0x0041d240 BEGIN
	/* 41D240 */ uint CGame_SpawnPickupObject(uint param_1, uint param_2);
	// !DECL 0x0041d240 END
	// !DECL 0x0041d3c6 BEGIN
	/* 41D3C6 */ uchar* Catch_0041d3c6();
	// !DECL 0x0041d3c6 END
	// !DECL 0x0041e2c0 BEGIN
	/* 41E2C0 */ uchar CGaming_SpawnPickupAndBroadcast(uint param_1);
	// !DECL 0x0041e2c0 END
	// !DECL 0x0041eba0 BEGIN
	/* 41EBA0 */ uchar CGame_ApplyPickup(uchar param_1, uchar param_2);
	// !DECL 0x0041eba0 END
	// !DECL 0x0041ed60 BEGIN
	/* 41ED60 */ uchar DetonatePlayerMines(int param_1);
	// !DECL 0x0041ed60 END
	// !DECL 0x0041f0c0 BEGIN
	/* 41F0C0 */ uchar CGaming_TickPlayerCollisions(int* param_1);
	// !DECL 0x0041f0c0 END
	// !DECL 0x0041f1d0 BEGIN
	/* 41F1D0 */ uchar CGaming_SpawnSpecialPickupIfAllowed(char param_1);
	// !DECL 0x0041f1d0 END
	// !DECL 0x0041f230 BEGIN
	/* 41F230 */ uchar CGaming_SpawnBulletAndPlaySound(int* param_1, uchar param_2, uchar param_3, uchar param_4, int* param_5, int param_6);
	// !DECL 0x0041f230 END
	// !DECL 0x0041f350 BEGIN
	/* 41F350 */ uchar CGaming_TickRoundStateAndScoring(int* param_1);
	// !DECL 0x0041f350 END
	// !DECL 0x0041f500 BEGIN
	/* 41F500 */ uchar CGaming_SpawnAndInitializePlayer(uchar param_1, void* param_2, int param_3, int param_4, int param_5);
	// !DECL 0x0041f500 END
	// !DECL 0x0041f5d0 BEGIN
	/* 41F5D0 */ uchar CGaming_SpawnPracticeDummy(void* param_1, int param_2, int param_3, int param_4);
	// !DECL 0x0041f5d0 END
	// !DECL 0x0041f770 BEGIN
	/* 41F770 */ uchar CGaming_RespawnPlayer(char param_1, uchar param_2, int* param_3);
	// !DECL 0x0041f770 END
	// !DECL 0x0041fdf0 BEGIN
	/* 41FDF0 */ uchar CWeapon_FireGrenade(CWeapon* param_1);
	// !DECL 0x0041fdf0 END
	// !DECL 0x00420530 BEGIN
	/* 420530 */ uchar CGaming_SpawnPlayerAtSlot(uchar param_1);
	// !DECL 0x00420530 END
	// !DECL 0x00420550 BEGIN
	/* 420550 */ uchar CGaming_SpawnCoopPartnerSlots(void* param_1);
	// !DECL 0x00420550 END
	// !DECL 0x004205a0 BEGIN
	/* 4205A0 */ uchar CBulanek_SpawnPlayerAndCampaignSlots(CBulanek* param_1);
	// !DECL 0x004205a0 END
	// !DECL 0x00420650 BEGIN
	/* 420650 */ uchar CBulanek_DispatchCurrentWeaponAction(CWeapon* param_1);
	// !DECL 0x00420650 END
	// !DECL 0x004208c0 BEGIN
	/* 4208C0 */ uchar CBulanek_TriggerPrimaryActionAndBroadcast(int param_1);
	// !DECL 0x004208c0 END
	// !DECL 0x00421830 BEGIN
	/* 421830 */ CHelpScript* CreateObject_00421830();
	// !DECL 0x00421830 END
	// !DECL 0x00421ba0 BEGIN
	/* 421BA0 */ CHelpView* CreateObject_00421ba0();
	// !DECL 0x00421ba0 END
	// !DECL 0x00422280 BEGIN
	/* 422280 */ CHelpDlg* CreateObject_00422280();
	// !DECL 0x00422280 END
	// !DECL 0x00422310 BEGIN
	/* 422310 */ static void* CDSAudioPlayer_Create(uchar* param_1, IDSAudioSource* param_2, int param_3, void* param_4, char param_5);
	// !DECL 0x00422310 END
	// !DECL 0x004223c0 BEGIN
	/* 4223C0 */ uchar CDSAudioPlayer_PlayAndRelease(int* param_1, uchar param_2);
	// !DECL 0x004223c0 END
	// !DECL 0x004223e0 BEGIN
	/* 4223E0 */ static void CDSAudio_SetPanPreview(int param_1);
	// !DECL 0x004223e0 END
	// !DECL 0x00422430 BEGIN
	/* 422430 */ static uchar TriggerBankSample(uchar* param_1, int param_2, int param_3, int param_4, void* param_5, char param_6);
	// !DECL 0x00422430 END
	// !DECL 0x00422500 BEGIN
	/* 422500 */ static int ComputeSpatialAttenuationDb100(int* param_1);
	// !DECL 0x00422500 END
	// !DECL 0x004229b0 BEGIN
	/* 4229B0 */ CHistoryScript* CHistoryScript_CreateObject();
	// !DECL 0x004229b0 END
	// !DECL 0x00422f00 BEGIN
	/* 422F00 */ CHistoryView* CreateObject_00422f00();
	// !DECL 0x00422f00 END
	// !DECL 0x00423610 BEGIN
	/* 423610 */ CHistoryDlg* CreateObject_00423610();
	// !DECL 0x00423610 END
	// !DECL 0x00423f20 BEGIN
	/* 423F20 */ uchar CSwitch_PlayHoverTrack(int param_1);
	// !DECL 0x00423f20 END
	// !DECL 0x00424010 BEGIN
	/* 424010 */ void CMenu_EnableBackgroundState(char param_1);
	// !DECL 0x00424010 END
	// !DECL 0x00424400 BEGIN
	/* 424400 */ static uchar CDSVec_ZeroNewIntPairs(uint* param_1, int param_2);
	// !DECL 0x00424400 END
	// !DECL 0x00424430 BEGIN
	/* 424430 */ CRuch* CreateObject_00424430();
	// !DECL 0x00424430 END
	// !DECL 0x00424d30 BEGIN
	/* 424D30 */ void Button_Click(CSwitch* param_1);
	// !DECL 0x00424d30 END
	// !DECL 0x00425060 BEGIN
	/* 425060 */ uchar CPoemScroller_ReleaseOwnedResources(int param_1);
	// !DECL 0x00425060 END
	// !DECL 0x004250c0 BEGIN
	/* 4250C0 */ uchar CDSVec_ResizeIntPair(int param_1);
	// !DECL 0x004250c0 END
	// !DECL 0x00425160 BEGIN
	/* 425160 */ uint CreateObject_00425160();
	// !DECL 0x00425160 END
	// !DECL 0x004251d0 BEGIN
	/* 4251D0 */ uchar CMenu_CreateObject();
	// !DECL 0x004251d0 END
	// !DECL 0x00425450 BEGIN
	/* 425450 */ uchar CDSVec_ResizeIntPair_ToZero(void* param_1);
	// !DECL 0x00425450 END
	// !DECL 0x00426500 BEGIN
	/* 426500 */ CGunMouse* CGunMouse_CreateObject();
	// !DECL 0x00426500 END
	// !DECL 0x00426570 BEGIN
	/* 426570 */ CPoemScroller* CPoemScroller_CreateObject();
	// !DECL 0x00426570 END
	// !DECL 0x004276c0 BEGIN
	/* 4276C0 */ uchar CGameCounter_UpdatePlayerScore(uchar param_1, char param_2);
	// !DECL 0x004276c0 END
	// !DECL 0x00427710 BEGIN
	/* 427710 */ uchar CGaming_SetRoundTimerLabelFromString(uint* param_1);
	// !DECL 0x00427710 END
	// !DECL 0x004279f0 BEGIN
	/* 4279F0 */ CShotCounter* CreateObject_004279f0();
	// !DECL 0x004279f0 END
	// !DECL 0x00427a80 BEGIN
	/* 427A80 */ uint CreateObject_00427a80();
	// !DECL 0x00427a80 END
	// !DECL 0x00427ba0 BEGIN
	/* 427BA0 */ CPanel* CreateObject_00427ba0();
	// !DECL 0x00427ba0 END
	// !DECL 0x00427c30 BEGIN
	/* 427C30 */ uchar CGamingHud_SetAmmoStripState(int* param_1, uchar param_2, uchar param_3);
	// !DECL 0x00427c30 END
	// !DECL 0x00427c90 BEGIN
	/* 427C90 */ uchar CGamingHud_UpdatePlayerAmmoStrip(void* param_1);
	// !DECL 0x00427c90 END
	// !DECL 0x00428c15 BEGIN
	/* 428C15 */ uchar Catch_00428c15();
	// !DECL 0x00428c15 END
	// !DECL 0x00429280 BEGIN
	/* 429280 */ uchar CDSJpegImage_JpegSetupScope_dtor(int* param_1);
	// !DECL 0x00429280 END
	// !DECL 0x004298b0 BEGIN
	/* 4298B0 */ bool CDSBackBuffer_RestoreSurface();
	// !DECL 0x004298b0 END
	// !DECL 0x004298d0 BEGIN
	/* 4298D0 */ bool CDSBackBuffer_BindImageFromSurface();
	// !DECL 0x004298d0 END
	// !DECL 0x00429930 BEGIN
	/* 429930 */ bool CDSBackBuffer_Flip(CDSBackBuffer* param_1);
	// !DECL 0x00429930 END
	// !DECL 0x00429960 BEGIN
	/* 429960 */ void CDSBackBuffer_FreeImageMember(CDSBackBuffer* param_1);
	// !DECL 0x00429960 END
	// !DECL 0x00429c00 BEGIN
	/* 429C00 */ int CDSApp_WndProc(HWND param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00429c00 END
	// !DECL 0x00429c8d BEGIN
	/* 429C8D */ uint Catch_00429c8d();
	// !DECL 0x00429c8d END
	// !DECL 0x00429cc0 BEGIN
	/* 429CC0 */ uchar CDSApp_KeybQueue(ushort param_1, uchar param_2, uchar param_3);
	// !DECL 0x00429cc0 END
	// !DECL 0x0042a040 BEGIN
	/* 42A040 */ static uchar TArray16_ZeroRange(uint* param_1, int param_2);
	// !DECL 0x0042a040 END
	// !DECL 0x0042a070 BEGIN
	/* 42A070 */ int CDSApp_DirtyRectList_FindIndex(int* param_1, void* param_2, int param_3);
	// !DECL 0x0042a070 END
	// !DECL 0x0042a130 BEGIN
	/* 42A130 */ static void CDSApp_DirtyRectList_SlideRecords(void* param_1, void* param_2, int param_3);
	// !DECL 0x0042a130 END
	// !DECL 0x0042a590 BEGIN
	/* 42A590 */ uchar CDSApp_TryBindBackBufferSurface();
	// !DECL 0x0042a590 END
	// !DECL 0x0042a910 BEGIN
	/* 42A910 */ uchar CDSApp_DirtyRectList_SetSize(int param_1);
	// !DECL 0x0042a910 END
	// !DECL 0x0042a980 BEGIN
	/* 42A980 */ uchar CDSApp_DirtyRectList_EnsureCapacity(uint param_1);
	// !DECL 0x0042a980 END
	// !DECL 0x0042a9c0 BEGIN
	/* 42A9C0 */ void Catch_0042ab28_WriteExceptionLog(void* param_1, int* param_2, wchar_t* param_3);
	// !DECL 0x0042a9c0 END
	// !DECL 0x0042aa60 BEGIN
	/* 42AA60 */ static uint CDSApp_AppMain(HINSTANCE* param_1, uint param_2, int param_3);
	// !DECL 0x0042aa60 END
	// !DECL 0x0042ab28 BEGIN
	/* 42AB28 */ uchar Catch_0042ab28();
	// !DECL 0x0042ab28 END
	// !DECL 0x0042abb9 BEGIN
	/* 42ABB9 */ uchar* Catch_0042abb9();
	// !DECL 0x0042abb9 END
	// !DECL 0x0042abcf BEGIN
	/* 42ABCF */ uint Catch_0042ab28_ShowMessageAndRelease();
	// !DECL 0x0042abcf END
	// !DECL 0x0042ac20 BEGIN
	/* 42AC20 */ int CDSApp_DirtyRectList_InsertAt(uint* param_1, int param_2);
	// !DECL 0x0042ac20 END
	// !DECL 0x0042add0 BEGIN
	/* 42ADD0 */ uchar CDSApp_SetPendingChildView(int param_1);
	// !DECL 0x0042add0 END
	// !DECL 0x0042ae40 BEGIN
	/* 42AE40 */ uchar CDSApp_DirtyRectList_Clear(void* param_1);
	// !DECL 0x0042ae40 END
	// !DECL 0x0042ae50 BEGIN
	/* 42AE50 */ int CDSApp_DirtyRectList_UpsertRect(int* param_1, uchar* param_2, int param_3);
	// !DECL 0x0042ae50 END
	// !DECL 0x0042b910 BEGIN
	/* 42B910 */ uint* CreateObject_0042b910();
	// !DECL 0x0042b910 END
	// !DECL 0x0042bbe0 BEGIN
	/* 42BBE0 */ uchar CDSApp_FlushDirtyRectsIfFlagged(uchar param_1);
	// !DECL 0x0042bbe0 END
	// !DECL 0x0042bc00 BEGIN
	/* 42BC00 */ uchar CDSApp_RenderFrame(uchar param_1);
	// !DECL 0x0042bc00 END
	// !DECL 0x0042bda0 BEGIN
	/* 42BDA0 */ uchar CDSApp_FrameBody(uchar param_1);
	// !DECL 0x0042bda0 END
	// !DECL 0x0042be60 BEGIN
	/* 42BE60 */ uchar CDSApp_PumpTick(uchar param_1);
	// !DECL 0x0042be60 END
	// !DECL 0x0042bed0 BEGIN
	/* 42BED0 */ uchar CDSView_SetModalEligible(uint param_1);
	// !DECL 0x0042bed0 END
	// !DECL 0x0042c140 BEGIN
	/* 42C140 */ void CDSView_InputChain_OnDetach();
	// !DECL 0x0042c140 END
	// !DECL 0x0042c230 BEGIN
	/* 42C230 */ uchar CDSView_ReleaseKeyboardFocus(int* param_1);
	// !DECL 0x0042c230 END
	// !DECL 0x0042c290 BEGIN
	/* 42C290 */ uchar CDSView_SetActive(uint param_1);
	// !DECL 0x0042c290 END
	// !DECL 0x0042c6c0 BEGIN
	/* 42C6C0 */ uchar CMenu_ClearAllChildren(uchar param_1);
	// !DECL 0x0042c6c0 END
	// !DECL 0x0042c700 BEGIN
	/* 42C700 */ uchar CDSApp_RefreshInputChainHitTest();
	// !DECL 0x0042c700 END
	// !DECL 0x0042c7f0 BEGIN
	/* 42C7F0 */ uchar CDSApp_SetInputChainHead(int* param_1);
	// !DECL 0x0042c7f0 END
	// !DECL 0x0042c860 BEGIN
	/* 42C860 */ uchar CControl_ClaimModalFocusOnPress(int* param_1);
	// !DECL 0x0042c860 END
	// !DECL 0x0042c880 BEGIN
	/* 42C880 */ uchar CDSView_UpdateInputChainOnMouseMove(int* param_1);
	// !DECL 0x0042c880 END
	// !DECL 0x0042c8e0 BEGIN
	/* 42C8E0 */ uchar CDSView_AcquireKeyboardFocus(int* param_1);
	// !DECL 0x0042c8e0 END
	// !DECL 0x0042c960 BEGIN
	/* 42C960 */ uchar CDSView_Show_UpdateFocusChain(int* param_1);
	// !DECL 0x0042c960 END
	// !DECL 0x0042c9f0 BEGIN
	/* 42C9F0 */ ushort CDSView_EnableWidget(int* param_1);
	// !DECL 0x0042c9f0 END
	// !DECL 0x0042cc30 BEGIN
	/* 42CC30 */ uchar CDSView_OffsetRectAndAdapt(int param_1, int param_2);
	// !DECL 0x0042cc30 END
	// !DECL 0x0042ce30 BEGIN
	/* 42CE30 */ uint* CreateObject_0042ce30();
	// !DECL 0x0042ce30 END
	// !DECL 0x0042d2d0 BEGIN
	/* 42D2D0 */ uchar CDsStringReleaseHeader(void* param_1);
	// !DECL 0x0042d2d0 END
	// !DECL 0x0042d350 BEGIN
	/* 42D350 */ static int* CDsString_AllocWide(int param_1, int param_2);
	// !DECL 0x0042d350 END
	// !DECL 0x0042d3a0 BEGIN
	/* 42D3A0 */ static int* CDsStringAllocAndCopyWide(void* param_1, int param_2, int param_3);
	// !DECL 0x0042d3a0 END
	// !DECL 0x0042d3e0 BEGIN
	/* 42D3E0 */ uchar CDsStringSetLengthAndTerminate(int param_1);
	// !DECL 0x0042d3e0 END
	// !DECL 0x0042d440 BEGIN
	/* 42D440 */ uchar CDsString_WriteWStringToStream(int* param_1);
	// !DECL 0x0042d440 END
	// !DECL 0x0042d490 BEGIN
	/* 42D490 */ void* CDsStringAssignFromHandle(uint* param_1);
	// !DECL 0x0042d490 END
	// !DECL 0x0042d5a0 BEGIN
	/* 42D5A0 */ void* CDsStringInsertWide(int* param_1, size_t param_2, wchar_t* param_3, size_t param_4);
	// !DECL 0x0042d5a0 END
	// !DECL 0x0042d710 BEGIN
	/* 42D710 */ void* CDsStringVsprintfToStackBuffer(wchar_t* param_1, uint* param_2);
	// !DECL 0x0042d710 END
	// !DECL 0x0042d770 BEGIN
	/* 42D770 */ static void* CDsStringFormatV(void* param_1, wchar_t* param_2);
	// !DECL 0x0042d770 END
	// !DECL 0x0042d850 BEGIN
	/* 42D850 */ uchar CDsString_EnsureUnique(int* param_1);
	// !DECL 0x0042d850 END
	// !DECL 0x0042d8a0 BEGIN
	/* 42D8A0 */ uchar CDsStringAssignFromLong(void* param_1);
	// !DECL 0x0042d8a0 END
	// !DECL 0x0042db60 BEGIN
	/* 42DB60 */ uchar CDsString_WideToMultiByteBuffer(int param_1, int param_2);
	// !DECL 0x0042db60 END
	// !DECL 0x0042dc50 BEGIN
	/* 42DC50 */ int* CDsString_ClearInPlace(int* param_1);
	// !DECL 0x0042dc50 END
	// !DECL 0x0042e140 BEGIN
	/* 42E140 */ int* CDsString_ReadWStringFromStream(int* param_1);
	// !DECL 0x0042e140 END
	// !DECL 0x0042e4e0 BEGIN
	/* 42E4E0 */ void CDsString_WriteTwoWideStringsToStream(int* param_1, wchar_t* param_2);
	// !DECL 0x0042e4e0 END
	// !DECL 0x0042e630 BEGIN
	/* 42E630 */ static uint IsLeapYear(uint param_1);
	// !DECL 0x0042e630 END
	// !DECL 0x0042e790 BEGIN
	/* 42E790 */ void CDSApp_UpdateClock();
	// !DECL 0x0042e790 END
	// !DECL 0x0042e7b0 BEGIN
	/* 42E7B0 */ uchar CDate_SetDate(uint* param_1, uint param_2, uint param_3);
	// !DECL 0x0042e7b0 END
	// !DECL 0x0042e8f0 BEGIN
	/* 42E8F0 */ void ClassRegEntry_PrependListHead(void* param_1, void* param_2);
	// !DECL 0x0042e8f0 END
	// !DECL 0x0042e910 BEGIN
	/* 42E910 */ void* HandleClassRegister(uint param_1, uint param_2, uint param_3);
	// !DECL 0x0042e910 END
	// !DECL 0x0042e960 BEGIN
	/* 42E960 */ uint ClassRegEntry_ListContains(void* param_1);
	// !DECL 0x0042e960 END
	// !DECL 0x0042e980 BEGIN
	/* 42E980 */ uchar HandleInterfaceRegister(int param_1, uint param_2, uint param_3);
	// !DECL 0x0042e980 END
	// !DECL 0x0042e9a0 BEGIN
	/* 42E9A0 */ uint HandleVirtualBaseCast(int param_1);
	// !DECL 0x0042e9a0 END
	// !DECL 0x0042e9f0 BEGIN
	/* 42E9F0 */ int CheckedVirtualBaseCast(int param_1);
	// !DECL 0x0042e9f0 END
	// !DECL 0x0042ea40 BEGIN
	/* 42EA40 */ static uchar CDSQueue_ZeroRecordArgsRange(int param_1, int param_2);
	// !DECL 0x0042ea40 END
	// !DECL 0x0042ea80 BEGIN
	/* 42EA80 */ void CDSObject_AddRef(int* param_1);
	// !DECL 0x0042ea80 END
	// !DECL 0x0042eb30 BEGIN
	/* 42EB30 */ void Scheduler_DispatchDueEvents(CDSUpdatedItem* param_1);
	// !DECL 0x0042eb30 END
	// !DECL 0x0042ebf0 BEGIN
	/* 42EBF0 */ uchar CDSApp_PulseTasks();
	// !DECL 0x0042ebf0 END
	// !DECL 0x0042ecc0 BEGIN
	/* 42ECC0 */ static uint CStartGame2_EnqueueEvent_NullSafe(void* param_1, ushort param_2, ushort param_3, uint param_4, uint param_5);
	// !DECL 0x0042ecc0 END
	// !DECL 0x0042ecf0 BEGIN
	/* 42ECF0 */ uint CDSQueue_Push(uint* param_1);
	// !DECL 0x0042ecf0 END
	// !DECL 0x0042ed70 BEGIN
	/* 42ED70 */ uint CDSQueue_PopDiscard(int param_1);
	// !DECL 0x0042ed70 END
	// !DECL 0x0042eda0 BEGIN
	/* 42EDA0 */ uint CDSQueue_PopCopy(uint* param_1);
	// !DECL 0x0042eda0 END
	// !DECL 0x0042ee20 BEGIN
	/* 42EE20 */ int CDSQueue_Peek(int param_1);
	// !DECL 0x0042ee20 END
	// !DECL 0x0042ee40 BEGIN
	/* 42EE40 */ uchar CDSQueue_SetCapacity(int param_1);
	// !DECL 0x0042ee40 END
	// !DECL 0x0042ef00 BEGIN
	/* 42EF00 */ static uint InitializeByClassId(int param_1, uint param_2, int param_3);
	// !DECL 0x0042ef00 END
	// !DECL 0x0042f410 BEGIN
	/* 42F410 */ uint CDSApp_PollEventQueue();
	// !DECL 0x0042f410 END
	// !DECL 0x0042f460 BEGIN
	/* 42F460 */ uchar CDSApp_DispatchOneEvent();
	// !DECL 0x0042f460 END
	// !DECL 0x0042f5c0 BEGIN
	/* 42F5C0 */ uint CDSQueue_Init(int param_1);
	// !DECL 0x0042f5c0 END
	// !DECL 0x0042f5f0 BEGIN
	/* 42F5F0 */ uchar Scheduler_PopHook(int param_1);
	// !DECL 0x0042f5f0 END
	// !DECL 0x0042f620 BEGIN
	/* 42F620 */ uchar Scheduler_PushHook(int param_1);
	// !DECL 0x0042f620 END
	// !DECL 0x0042f640 BEGIN
	/* 42F640 */ int CDSEventHandler_ctor(int param_1);
	// !DECL 0x0042f640 END
	// !DECL 0x0042f690 BEGIN
	/* 42F690 */ uchar CDSEventQueue_ClearForTarget(int param_1);
	// !DECL 0x0042f690 END
	// !DECL 0x0042f6f0 BEGIN
	/* 42F6F0 */ void* Runtime_MallocOrThrow(uchar* param_1);
	// !DECL 0x0042f6f0 END
	// !DECL 0x0042f720 BEGIN
	/* 42F720 */ void Runtime_Free(void* param_1);
	// !DECL 0x0042f720 END
	// !DECL 0x0042f730 BEGIN
	/* 42F730 */ void* Runtime_ReallocOrThrow(void* param_1, uchar* param_2);
	// !DECL 0x0042f730 END
	// !DECL 0x0042f780 BEGIN
	/* 42F780 */ uchar CDSChained_LinkIntrusiveNode(uint* param_1, uint* param_2);
	// !DECL 0x0042f780 END
	// !DECL 0x0042f7a0 BEGIN
	/* 42F7A0 */ uchar CDSChained_UnlinkIntrusiveNode(int param_1);
	// !DECL 0x0042f7a0 END
	// !DECL 0x0042f7c0 BEGIN
	/* 42F7C0 */ void* CDSChained_GetFirstChildView(CDSChain* param_1);
	// !DECL 0x0042f7c0 END
	// !DECL 0x0042f7d0 BEGIN
	/* 42F7D0 */ uint CDSIntrusiveNode_GetLinkNext();
	// !DECL 0x0042f7d0 END
	// !DECL 0x0042f7e0 BEGIN
	/* 42F7E0 */ uint CDSIntrusiveNode_GetNextLinkSkipSentinel(int param_1);
	// !DECL 0x0042f7e0 END
	// !DECL 0x0042f850 BEGIN
	/* 42F850 */ uchar CDSChained_ResetHeadOrSpliceBefore(int param_1);
	// !DECL 0x0042f850 END
	// !DECL 0x0042f920 BEGIN
	/* 42F920 */ void* CDSChained_GetNextSiblingView(void* param_1);
	// !DECL 0x0042f920 END
	// !DECL 0x0042f9b0 BEGIN
	/* 42F9B0 */ uchar CDSChained_AppendChild(void* param_1);
	// !DECL 0x0042f9b0 END
	// !DECL 0x0042f9d0 BEGIN
	/* 42F9D0 */ void CDSChained_InsertChildAtAnchor(void* param_1, void* param_2, void* param_3);
	// !DECL 0x0042f9d0 END
	// !DECL 0x0042fc05 BEGIN
	/* 42FC05 */ uchar Catch_0042fc05();
	// !DECL 0x0042fc05 END
	// !DECL 0x00430270 BEGIN
	/* 430270 */ static void RaiseStreamException(uint param_1, int* param_2);
	// !DECL 0x00430270 END
	// !DECL 0x004302e0 BEGIN
	/* 4302E0 */ static void ThrowStreamErrorNoReturn(uint param_1, int* param_2, uint param_3);
	// !DECL 0x004302e0 END
	// !DECL 0x00430a70 BEGIN
	/* 430A70 */ uchar CDSEasyMemStream_GuardReadable(int param_1);
	// !DECL 0x00430a70 END
	// !DECL 0x00430e70 BEGIN
	/* 430E70 */ CDSEasyMemStream* CDSEasyMemStream_CreateFromStreamSlice(void* param_1, uint param_2, void* param_3);
	// !DECL 0x00430e70 END
	// !DECL 0x00431070 BEGIN
	/* 431070 */ uchar CDSCollection_EnsureCapacity(int param_1);
	// !DECL 0x00431070 END
	// !DECL 0x004310b0 BEGIN
	/* 4310B0 */ int CDSCollection_Insert(uint param_1, int param_2);
	// !DECL 0x004310b0 END
	// !DECL 0x00431100 BEGIN
	/* 431100 */ uchar CDSCollection_Remove(int param_1, int param_2, char param_3);
	// !DECL 0x00431100 END
	// !DECL 0x004313f5 BEGIN
	/* 4313F5 */ uchar Catch_004313f5();
	// !DECL 0x004313f5 END
	// !DECL 0x00431590 BEGIN
	/* 431590 */ static uchar CDSJpegImage_InitJpegErrorMgr(uint* param_1);
	// !DECL 0x00431590 END
	// !DECL 0x004315d0 BEGIN
	/* 4315D0 */ static void CDSJpegMemPool_free_pool(void* param_1, int param_2);
	// !DECL 0x004315d0 END
	// !DECL 0x00431670 BEGIN
	/* 431670 */ static uint fill_input_buffer(int param_1);
	// !DECL 0x00431670 END
	// !DECL 0x004316c0 BEGIN
	/* 4316C0 */ static uchar jpeg_skip_input_data(int param_1, int param_2);
	// !DECL 0x004316c0 END
	// !DECL 0x00431700 BEGIN
	/* 431700 */ static uchar jpeg_CDSStreamStorage_src(int param_1, int* param_2, uint param_3);
	// !DECL 0x00431700 END
	// !DECL 0x00431790 BEGIN
	/* 431790 */ static int CDSJpegMemPool_BumpAlloc(int param_1, int param_2, int param_3);
	// !DECL 0x00431790 END
	// !DECL 0x00431819 BEGIN
	/* 431819 */ uchar* Catch_00431819();
	// !DECL 0x00431819 END
	// !DECL 0x00431851 BEGIN
	/* 431851 */ int CDSJpegMemPool_LinkNewChunk();
	// !DECL 0x00431851 END
	// !DECL 0x00431890 BEGIN
	/* 431890 */ static int CDSJpegMemPool_alloc_sarray(int param_1, int param_2, int param_3, uint param_4);
	// !DECL 0x00431890 END
	// !DECL 0x004318f0 BEGIN
	/* 4318F0 */ static int CDSJpegMemPool_alloc_barray(int param_1, int param_2, int param_3, uint param_4);
	// !DECL 0x004318f0 END
	// !DECL 0x004319d0 BEGIN
	/* 4319D0 */ static uchar jpeg_realize_virt_arrays(int param_1);
	// !DECL 0x004319d0 END
	// !DECL 0x00431a70 BEGIN
	/* 431A70 */ static uchar jinit_memory_mgr(int param_1);
	// !DECL 0x00431a70 END
	// !DECL 0x00431ab4 BEGIN
	/* 431AB4 */ uchar* Catch_00431ab4();
	// !DECL 0x00431ab4 END
	// !DECL 0x00431ae8 BEGIN
	/* 431AE8 */ uchar CDSJpegMemPool_InitMethodPointers();
	// !DECL 0x00431ae8 END
	// !DECL 0x00431ca9 BEGIN
	/* 431CA9 */ uchar Catch_00431ca9();
	// !DECL 0x00431ca9 END
	// !DECL 0x00431f98 BEGIN
	/* 431F98 */ uchar Catch_00431f98();
	// !DECL 0x00431f98 END
	// !DECL 0x004331e0 BEGIN
	/* 4331E0 */ uchar CBulanci_CloseFindHandle(int param_1);
	// !DECL 0x004331e0 END
	// !DECL 0x004339e0 BEGIN
	/* 4339E0 */ uchar Eh_LeaveCriticalSection(uint* param_1);
	// !DECL 0x004339e0 END
	// !DECL 0x00434250 BEGIN
	/* 434250 */ uchar IDSChainedTail_ClearSubObjStash(int param_1);
	// !DECL 0x00434250 END
	// !DECL 0x004344c0 BEGIN
	/* 4344C0 */ void eh_dtor_CDSObject_ptr(int** param_1);
	// !DECL 0x004344c0 END
	// !DECL 0x004344d0 BEGIN
	/* 4344D0 */ CDSResourceException* CDSResourceException_CreateObject();
	// !DECL 0x004344d0 END
	// !DECL 0x00434930 BEGIN
	/* 434930 */ CDSResourceSign* InitializeAndAllocate();
	// !DECL 0x00434930 END
	// !DECL 0x004349e0 BEGIN
	/* 4349E0 */ void Runtime_ThrowBadAlloc(uchar* param_1);
	// !DECL 0x004349e0 END
	// !DECL 0x00434bd0 BEGIN
	/* 434BD0 */ static uchar CDsStringFromWin32ErrorCode(uchar param_1, DWORD param_2);
	// !DECL 0x00434bd0 END
	// !DECL 0x00434e30 BEGIN
	/* 434E30 */ static uint CDSGZipStream_Decompress_static(uint param_1, int param_2, uint* param_3, uint param_4);
	// !DECL 0x00434e30 END
	// !DECL 0x00434ee0 BEGIN
	/* 434EE0 */ static uint CDSGZipStream_Compress_static(uint param_1, int param_2, uint* param_3, uint param_4);
	// !DECL 0x00434ee0 END
	// !DECL 0x00435050 BEGIN
	/* 435050 */ uchar CDSGZipStream_ReadBufferAt(uint param_1, uint param_2);
	// !DECL 0x00435050 END
	// !DECL 0x00435140 BEGIN
	/* 435140 */ uchar CDSGZipStream_WriteBuffer(int param_1);
	// !DECL 0x00435140 END
	// !DECL 0x004354a0 BEGIN
	/* 4354A0 */ uchar CDSGZipStream_CloseInt(int param_1);
	// !DECL 0x004354a0 END
	// !DECL 0x00435670 BEGIN
	/* 435670 */ uchar CDSGZipStream_AllocateBuffers(int param_1);
	// !DECL 0x00435670 END
	// !DECL 0x00435c70 BEGIN
	/* 435C70 */ static uint MapBitCountToFormat(uint param_1);
	// !DECL 0x00435c70 END
	// !DECL 0x00435d00 BEGIN
	/* 435D00 */ static uchar SampleSourcePixel_Indexed1(uint param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00435d00 END
	// !DECL 0x00435d30 BEGIN
	/* 435D30 */ static uchar SampleSourcePixel_Indexed2(uint param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00435d30 END
	// !DECL 0x00435d70 BEGIN
	/* 435D70 */ static uchar SampleSourcePixel_Indexed4(uint param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00435d70 END
	// !DECL 0x00435db0 BEGIN
	/* 435DB0 */ static uchar SampleSourcePixel_Indexed8(int param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00435db0 END
	// !DECL 0x00435dd0 BEGIN
	/* 435DD0 */ static ushort Blit_ReadDstPixel_16bpp(int param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00435dd0 END
	// !DECL 0x00435df0 BEGIN
	/* 435DF0 */ static uint CDSImage_GetPixel24(int param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00435df0 END
	// !DECL 0x00435e20 BEGIN
	/* 435E20 */ static uint Blit_ReadDstPixel_32bpp(int param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00435e20 END
	// !DECL 0x00435f30 BEGIN
	/* 435F30 */ static uchar Blit_WriteDstPixel_8bpp(int param_1, int param_2, uchar param_3, int param_4, int param_5);
	// !DECL 0x00435f30 END
	// !DECL 0x00435f50 BEGIN
	/* 435F50 */ static uchar Blit_WriteDstPixel_16bpp(int param_1, int param_2, ushort param_3, int param_4, int param_5);
	// !DECL 0x00435f50 END
	// !DECL 0x00435f80 BEGIN
	/* 435F80 */ static uchar CDSImage_PutPixel24(int param_1, int param_2, uint param_3, int param_4, int param_5);
	// !DECL 0x00435f80 END
	// !DECL 0x00435fb0 BEGIN
	/* 435FB0 */ static uchar Blit_WriteDstPixel_32bpp(int param_1, int param_2, uint param_3, int param_4, int param_5);
	// !DECL 0x00435fb0 END
	// !DECL 0x00436750 BEGIN
	/* 436750 */ void CDSBackBuffer_ClearPreFlipFields(CDSImage_BackBufferEmbed* param_1);
	// !DECL 0x00436750 END
	// !DECL 0x00436d90 BEGIN
	/* 436D90 */ uchar CDSImage_BindFromSurfaceDesc(int param_1);
	// !DECL 0x00436d90 END
	// !DECL 0x00437030 BEGIN
	/* 437030 */ uchar CDSView_CopyColorPlane(void* param_1);
	// !DECL 0x00437030 END
	// !DECL 0x00437080 BEGIN
	/* 437080 */ uchar NotifyDirtyAll(void* param_1);
	// !DECL 0x00437080 END
	// !DECL 0x00437440 BEGIN
	/* 437440 */ uchar WideCharToMultiByte_Wrapper(int param_1, int param_2);
	// !DECL 0x00437440 END
	// !DECL 0x004375e0 BEGIN
	/* 4375E0 */ uchar TextShaper_LayOutAndRender(int param_1, tagRECT* param_2, CPoemScroller* param_3, uint* param_4);
	// !DECL 0x004375e0 END
	// !DECL 0x00437b00 BEGIN
	/* 437B00 */ uchar CDSRegKey_CloseKey(uint* param_1);
	// !DECL 0x00437b00 END
	// !DECL 0x00437cd0 BEGIN
	/* 437CD0 */ uchar CDSRegKeyException_ThrowFromWin32(uint param_1);
	// !DECL 0x00437cd0 END
	// !DECL 0x00437d60 BEGIN
	/* 437D60 */ uchar InitializeRegistryKey(int param_1, int param_2, int param_3);
	// !DECL 0x00437d60 END
	// !DECL 0x00437ed0 BEGIN
	/* 437ED0 */ uchar ThrowRegKeyException(uint param_1, uint param_2);
	// !DECL 0x00437ed0 END
	// !DECL 0x004382e0 BEGIN
	/* 4382E0 */ uint CDSScript_GetGlobalVar(int param_1);
	// !DECL 0x004382e0 END
	// !DECL 0x004382f0 BEGIN
	/* 4382F0 */ uchar CDSScript_SetGlobalVar(int param_1, uint param_2);
	// !DECL 0x004382f0 END
	// !DECL 0x004384a0 BEGIN
	/* 4384A0 */ static uchar CDSScript_GetGlobalVar_004384a0(CDSScript* param_1);
	// !DECL 0x004384a0 END
	// !DECL 0x004388a0 BEGIN
	/* 4388A0 */ static ushort StrCharAt(CDSScript* param_1);
	// !DECL 0x004388a0 END
	// !DECL 0x00438900 BEGIN
	/* 438900 */ static uint CDSScript_And(CDSScript* param_1);
	// !DECL 0x00438900 END
	// !DECL 0x00438920 BEGIN
	/* 438920 */ static uint CDSScript_Or(CDSScript* param_1);
	// !DECL 0x00438920 END
	// !DECL 0x00438960 BEGIN
	/* 438960 */ static uchar** CDSScript_StrmCreateMem(CDSScript* param_1);
	// !DECL 0x00438960 END
	// !DECL 0x00438b30 BEGIN
	/* 438B30 */ uint CDSScript_Run(uint param_1, int param_2, void* param_3);
	// !DECL 0x00438b30 END
	// !DECL 0x00438bef BEGIN
	/* 438BEF */ uchar Catch_00438bef();
	// !DECL 0x00438bef END
	// !DECL 0x00438cb0 BEGIN
	/* 438CB0 */ static int CDSScript_Rand(CDSScript* param_1);
	// !DECL 0x00438cb0 END
	// !DECL 0x00438cf0 BEGIN
	/* 438CF0 */ static int CDSScript_Add(CDSScript* param_1);
	// !DECL 0x00438cf0 END
	// !DECL 0x00438d30 BEGIN
	/* 438D30 */ static int CDSScript_Mul(CDSScript* param_1);
	// !DECL 0x00438d30 END
	// !DECL 0x00438d50 BEGIN
	/* 438D50 */ static int CDSScript_Div(CDSScript* param_1);
	// !DECL 0x00438d50 END
	// !DECL 0x00438f90 BEGIN
	/* 438F90 */ uchar TM_BindSequence(int* param_1);
	// !DECL 0x00438f90 END
	// !DECL 0x00438fb0 BEGIN
	/* 438FB0 */ uchar TM_RewindCurrent(int param_1);
	// !DECL 0x00438fb0 END
	// !DECL 0x00438fd0 BEGIN
	/* 438FD0 */ void TM_SetAnimFrameFromAnimSub(int param_1);
	// !DECL 0x00438fd0 END
	// !DECL 0x00438fe0 BEGIN
	/* 438FE0 */ uchar TM_RenderFrame(char param_1);
	// !DECL 0x00438fe0 END
	// !DECL 0x00439080 BEGIN
	/* 439080 */ uchar TM_TickBlit(int param_1);
	// !DECL 0x00439080 END
	// !DECL 0x004391e0 BEGIN
	/* 4391E0 */ uchar TM_SetTrack(int param_1, int param_2, uint* param_3);
	// !DECL 0x004391e0 END
	// !DECL 0x00439270 BEGIN
	/* 439270 */ uchar AnimInner_Init(void* param_1);
	// !DECL 0x00439270 END
	// !DECL 0x004392e0 BEGIN
	/* 4392E0 */ uchar AnimInner_Teardown(ODSImage* param_1);
	// !DECL 0x004392e0 END
	// !DECL 0x00439350 BEGIN
	/* 439350 */ void* AnimInner_InitParam(int* param_1, int param_2);
	// !DECL 0x00439350 END
	// !DECL 0x00439610 BEGIN
	/* 439610 */ uint* CreateObject_00439610();
	// !DECL 0x00439610 END
	// !DECL 0x00439680 BEGIN
	/* 439680 */ CDSAnim* CreateObject_00439680();
	// !DECL 0x00439680 END
	// !DECL 0x004397e0 BEGIN
	/* 4397E0 */ static void TM_ShiftTrackEntries(uint* param_1, uint* param_2, int param_3);
	// !DECL 0x004397e0 END
	// !DECL 0x00439840 BEGIN
	/* 439840 */ void TM_PauseAndStampClock(CDSVideoPlayer* param_1);
	// !DECL 0x00439840 END
	// !DECL 0x004398b0 BEGIN
	/* 4398B0 */ uchar TM_ClearTracks(int param_1);
	// !DECL 0x004398b0 END
	// !DECL 0x00439900 BEGIN
	/* 439900 */ uchar CDSVideoPlayer_EnsureCapacity(uint param_1);
	// !DECL 0x00439900 END
	// !DECL 0x004399b0 BEGIN
	/* 4399B0 */ void TM_AdvanceFrame(CDSVideoPlayer* param_1);
	// !DECL 0x004399b0 END
	// !DECL 0x00439a30 BEGIN
	/* 439A30 */ uchar TM_SeekToFrame(int param_1);
	// !DECL 0x00439a30 END
	// !DECL 0x00439b40 BEGIN
	/* 439B40 */ void BeginCurrentTrackPlayback(CDSVideoPlayer* param_1);
	// !DECL 0x00439b40 END
	// !DECL 0x00439bc0 BEGIN
	/* 439BC0 */ void TM_ResetTrackListHead(CDSVideoPlayer* param_1);
	// !DECL 0x00439bc0 END
	// !DECL 0x00439e00 BEGIN
	/* 439E00 */ uchar AddTrackSource(int* param_1);
	// !DECL 0x00439e00 END
	// !DECL 0x00439e79 BEGIN
	/* 439E79 */ uchar Catch_00439e79();
	// !DECL 0x00439e79 END
	// !DECL 0x00439eb0 BEGIN
	/* 439EB0 */ void SetCurrentTrack(int param_1, char param_2);
	// !DECL 0x00439eb0 END
	// !DECL 0x00439fe0 BEGIN
	/* 439FE0 */ uchar CDSIDSReferencedSub_ReleaseHeldRef(int param_1);
	// !DECL 0x00439fe0 END
	// !DECL 0x0043a000 BEGIN
	/* 43A000 */ int ComputeDurationMs(int param_1);
	// !DECL 0x0043a000 END
	// !DECL 0x0043a030 BEGIN
	/* 43A030 */ void CDSAudioPlayer_SetDirectSoundBufferPosition(uint param_1);
	// !DECL 0x0043a030 END
	// !DECL 0x0043a0d0 BEGIN
	/* 43A0D0 */ void CDSAudioPlayer_SetVolumePercent(uint param_1);
	// !DECL 0x0043a0d0 END
	// !DECL 0x0043a590 BEGIN
	/* 43A590 */ uchar CDSAudioPlayer_StopAll();
	// !DECL 0x0043a590 END
	// !DECL 0x0043a5e0 BEGIN
	/* 43A5E0 */ static uchar CDSAudio_ApplyPanTable(void* param_1, int param_2);
	// !DECL 0x0043a5e0 END
	// !DECL 0x0043aae0 BEGIN
	/* 43AAE0 */ uchar CDSDirectPlay_ResetMembers(int param_1);
	// !DECL 0x0043aae0 END
	// !DECL 0x0043abc0 BEGIN
	/* 43ABC0 */ uchar CDSDirectPlaySender_Unbind(int param_1);
	// !DECL 0x0043abc0 END
	// !DECL 0x0043ace0 BEGIN
	/* 43ACE0 */ uchar CDSDirectPlay_CloseSession(int param_1);
	// !DECL 0x0043ace0 END
	// !DECL 0x0043ade5 BEGIN
	/* 43ADE5 */ uchar Catch_0043ade5();
	// !DECL 0x0043ade5 END
	// !DECL 0x0043ae20 BEGIN
	/* 43AE20 */ uchar CDSDirectPlay_Shutdown(int param_1);
	// !DECL 0x0043ae20 END
	// !DECL 0x0043b200 BEGIN
	/* 43B200 */ uint CreateObject_0043b200();
	// !DECL 0x0043b200 END
	// !DECL 0x0043b550 BEGIN
	/* 43B550 */ uchar CDSDirectPlaySender_EnsureScratchBuffer(uchar* param_1);
	// !DECL 0x0043b550 END
	// !DECL 0x0043b590 BEGIN
	/* 43B590 */ uint CreateObject_0043b590();
	// !DECL 0x0043b590 END
	// !DECL 0x0043b680 BEGIN
	/* 43B680 */ uchar CDSDirectPlaySender_EnqueueSend(void* param_1, uchar* param_2);
	// !DECL 0x0043b680 END
	// !DECL 0x0043b6d0 BEGIN
	/* 43B6D0 */ uchar CDSDirectPlay_Send(uint param_1, void* param_2, uchar* param_3);
	// !DECL 0x0043b6d0 END
	// !DECL 0x0043bd60 BEGIN
	/* 43BD60 */ CDSAudioVideoPlayer* CreateObject_0043bd60();
	// !DECL 0x0043bd60 END
	// !DECL 0x0043be90 BEGIN
	/* 43BE90 */ uchar CDSQueueStream_DetachQueue(int param_1);
	// !DECL 0x0043be90 END
	// !DECL 0x0043c1d0 BEGIN
	/* 43C1D0 */ uchar CDSMemQueue_Init(uint param_1);
	// !DECL 0x0043c1d0 END
	// !DECL 0x0043c1f0 BEGIN
	/* 43C1F0 */ uchar CDSMemQueue_Lock(int param_1);
	// !DECL 0x0043c1f0 END
	// !DECL 0x0043c200 BEGIN
	/* 43C200 */ uchar CDSMemQueue_Unlock(int param_1);
	// !DECL 0x0043c200 END
	// !DECL 0x0043c210 BEGIN
	/* 43C210 */ uchar CDSMemQueue_Read(void* param_1, void* param_2);
	// !DECL 0x0043c210 END
	// !DECL 0x0043c2ab BEGIN
	/* 43C2AB */ uchar Catch_0043c2ab();
	// !DECL 0x0043c2ab END
	// !DECL 0x0043c980 BEGIN
	/* 43C980 */ uint CDSDirectSound_DuplicateSoundBuffer(uint param_1);
	// !DECL 0x0043c980 END
	// !DECL 0x0043ca90 BEGIN
	/* 43CA90 */ uint* CreateObject_0043ca90();
	// !DECL 0x0043ca90 END
	// !DECL 0x0043ce50 BEGIN
	/* 43CE50 */ static uchar BlitOpaque_Indexed1_to_Indexed1(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043ce50 END
	// !DECL 0x0043cf10 BEGIN
	/* 43CF10 */ static uchar BlitOpaque_Indexed1_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043cf10 END
	// !DECL 0x0043d000 BEGIN
	/* 43D000 */ static uchar BlitOpaque_Indexed1_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d000 END
	// !DECL 0x0043d0d0 BEGIN
	/* 43D0D0 */ static uchar BlitOpaque_Indexed1_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d0d0 END
	// !DECL 0x0043d1a0 BEGIN
	/* 43D1A0 */ static uchar BlitOpaque_Indexed2_to_Indexed8(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043d1a0 END
	// !DECL 0x0043d260 BEGIN
	/* 43D260 */ static uchar BlitOpaque_Indexed2_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d260 END
	// !DECL 0x0043d350 BEGIN
	/* 43D350 */ static uchar BlitOpaque_Indexed2_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d350 END
	// !DECL 0x0043d420 BEGIN
	/* 43D420 */ static uchar Blit_Opaque_2bpp_32bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d420 END
	// !DECL 0x0043d4f0 BEGIN
	/* 43D4F0 */ static uchar BlitOpaque_Indexed4_to_Indexed8(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043d4f0 END
	// !DECL 0x0043d5b0 BEGIN
	/* 43D5B0 */ static uchar BlitOpaque_Indexed4_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d5b0 END
	// !DECL 0x0043d6a0 BEGIN
	/* 43D6A0 */ static uchar Blit_Opaque_4bpp_24bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d6a0 END
	// !DECL 0x0043d770 BEGIN
	/* 43D770 */ static uchar BlitOpaque_Indexed4_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d770 END
	// !DECL 0x0043d840 BEGIN
	/* 43D840 */ static uchar Blit_Opaque_Indexed8_to_Indexed8(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043d840 END
	// !DECL 0x0043d8d0 BEGIN
	/* 43D8D0 */ static uchar BlitOpaque_Palette8_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d8d0 END
	// !DECL 0x0043d990 BEGIN
	/* 43D990 */ static uchar BlitOpaque_Indexed8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043d990 END
	// !DECL 0x0043da40 BEGIN
	/* 43DA40 */ static uchar BlitOpaque_Indexed8_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0043da40 END
	// !DECL 0x0043dae0 BEGIN
	/* 43DAE0 */ static uchar BlitOpaque_RGB565_to_Indexed8(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8);
	// !DECL 0x0043dae0 END
	// !DECL 0x0043dbc0 BEGIN
	/* 43DBC0 */ static uchar BlitOpaque_RGB565_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043dbc0 END
	// !DECL 0x0043dc60 BEGIN
	/* 43DC60 */ static uchar Blit_Opaque_16bpp_24bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043dc60 END
	// !DECL 0x0043dd30 BEGIN
	/* 43DD30 */ static uchar BlitOpaque_RGB565_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043dd30 END
	// !DECL 0x0043de00 BEGIN
	/* 43DE00 */ static uchar BlitOpaque_BGR24_to_Palette8(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8);
	// !DECL 0x0043de00 END
	// !DECL 0x0043dec0 BEGIN
	/* 43DEC0 */ static uchar Blit_Opaque_24bpp_16bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043dec0 END
	// !DECL 0x0043e040 BEGIN
	/* 43E040 */ static uchar BlitOpaque_BGR24_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043e040 END
	// !DECL 0x0043e0f0 BEGIN
	/* 43E0F0 */ static uchar BlitOpaque_BGRA32_to_Palette8(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8);
	// !DECL 0x0043e0f0 END
	// !DECL 0x0043e1a0 BEGIN
	/* 43E1A0 */ static uchar BlitOpaque_BGRA32_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043e1a0 END
	// !DECL 0x0043e260 BEGIN
	/* 43E260 */ static uchar BlitOpaque_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043e260 END
	// !DECL 0x0043e310 BEGIN
	/* 43E310 */ static uchar BlitOpaque_BGRA32_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043e310 END
	// !DECL 0x0043e3b0 BEGIN
	/* 43E3B0 */ static uchar BlitDestKey_Indexed1_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043e3b0 END
	// !DECL 0x0043e4a0 BEGIN
	/* 43E4A0 */ static uchar BlitDestKey_Indexed1_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043e4a0 END
	// !DECL 0x0043e580 BEGIN
	/* 43E580 */ static uchar Blit_DestKey_1bpp_32bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043e580 END
	// !DECL 0x0043e650 BEGIN
	/* 43E650 */ static uchar BlitChromaKey_Indexed2_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043e650 END
	// !DECL 0x0043e740 BEGIN
	/* 43E740 */ static uchar BlitDestKey_Indexed2_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043e740 END
	// !DECL 0x0043e820 BEGIN
	/* 43E820 */ static uchar Blit_DestKey_2bpp_32bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043e820 END
	// !DECL 0x0043e8f0 BEGIN
	/* 43E8F0 */ static uchar BlitDestKey_Indexed4_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043e8f0 END
	// !DECL 0x0043e9e0 BEGIN
	/* 43E9E0 */ static uchar BlitChromaKey_Indexed4_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043e9e0 END
	// !DECL 0x0043eac0 BEGIN
	/* 43EAC0 */ static uchar Blit_ChromaKey_4bpp_32bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043eac0 END
	// !DECL 0x0043eb90 BEGIN
	/* 43EB90 */ static uchar BlitDestKey_Indexed8_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043eb90 END
	// !DECL 0x0043ec60 BEGIN
	/* 43EC60 */ static uchar BlitDestKey_Idx8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043ec60 END
	// !DECL 0x0043ed10 BEGIN
	/* 43ED10 */ static uchar Blit_DestKey_8bpp_32bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8);
	// !DECL 0x0043ed10 END
	// !DECL 0x0043edc0 BEGIN
	/* 43EDC0 */ static uchar BlitDestKey_RGB565_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8);
	// !DECL 0x0043edc0 END
	// !DECL 0x0043ee60 BEGIN
	/* 43EE60 */ static uchar BlitChromaKey_RGB565_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8);
	// !DECL 0x0043ee60 END
	// !DECL 0x0043ef30 BEGIN
	/* 43EF30 */ static uchar Blit_ChromaKey_16bpp_32bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8);
	// !DECL 0x0043ef30 END
	// !DECL 0x0043f000 BEGIN
	/* 43F000 */ static uchar Blit_DestKey_24bpp_16bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8);
	// !DECL 0x0043f000 END
	// !DECL 0x0043f0d0 BEGIN
	/* 43F0D0 */ static uchar BlitDestKey_24to24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8);
	// !DECL 0x0043f0d0 END
	// !DECL 0x0043f190 BEGIN
	/* 43F190 */ static uchar BlitDestKey_BGR24_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8);
	// !DECL 0x0043f190 END
	// !DECL 0x0043f240 BEGIN
	/* 43F240 */ static uchar BlitDestKey_BGRA32_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8);
	// !DECL 0x0043f240 END
	// !DECL 0x0043f310 BEGIN
	/* 43F310 */ static uchar BlitDestKey_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8);
	// !DECL 0x0043f310 END
	// !DECL 0x0043f3c0 BEGIN
	/* 43F3C0 */ static uchar Blit_DestKey_32bpp_32bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8);
	// !DECL 0x0043f3c0 END
	// !DECL 0x0043f470 BEGIN
	/* 43F470 */ static uchar BlitChromaKey(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x0043f470 END
	// !DECL 0x0043f630 BEGIN
	/* 43F630 */ static uchar BlitChromaKey_Indexed1_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x0043f630 END
	// !DECL 0x0043f7c0 BEGIN
	/* 43F7C0 */ static uchar BlitChromaKey_PAL1_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x0043f7c0 END
	// !DECL 0x0043f940 BEGIN
	/* 43F940 */ static uchar BlitChromaKey_2bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x0043f940 END
	// !DECL 0x0043fb00 BEGIN
	/* 43FB00 */ static uchar BlitMasked_Idx2_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x0043fb00 END
	// !DECL 0x0043fc90 BEGIN
	/* 43FC90 */ static uchar BlitChromaKey_PAL2_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x0043fc90 END
	// !DECL 0x0043fe10 BEGIN
	/* 43FE10 */ static uchar BlitChromaKey_4bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x0043fe10 END
	// !DECL 0x0043ffd0 BEGIN
	/* 43FFD0 */ static uchar BlitChromaKey_Indexed4_to_BGR24_0043ffd0(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x0043ffd0 END
	// !DECL 0x00440160 BEGIN
	/* 440160 */ static uchar BlitChromaKey_PAL4_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x00440160 END
	// !DECL 0x004402e0 BEGIN
	/* 4402E0 */ static uchar BlitChromaKey_Indexed8_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x004402e0 END
	// !DECL 0x00440480 BEGIN
	/* 440480 */ static uchar BlitChromaKey_PAL8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x00440480 END
	// !DECL 0x004405f0 BEGIN
	/* 4405F0 */ static uchar BlitChromaKey_Idx8_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9);
	// !DECL 0x004405f0 END
	// !DECL 0x00440740 BEGIN
	/* 440740 */ static uchar BlitChromaKey_RGB565_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9);
	// !DECL 0x00440740 END
	// !DECL 0x00440900 BEGIN
	/* 440900 */ static uchar BlitChromaKey_RGB565_to_BGR24_00440900(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9);
	// !DECL 0x00440900 END
	// !DECL 0x00440a90 BEGIN
	/* 440A90 */ static uchar BlitChromaKey_RGB565_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9);
	// !DECL 0x00440a90 END
	// !DECL 0x00440c10 BEGIN
	/* 440C10 */ static uchar BlitAlphaBlend_BGR24_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9);
	// !DECL 0x00440c10 END
	// !DECL 0x00440db0 BEGIN
	/* 440DB0 */ static uchar BlitChromaKey_BGR24_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9);
	// !DECL 0x00440db0 END
	// !DECL 0x00440f20 BEGIN
	/* 440F20 */ static uchar BlitChromaKey_BGR24_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9);
	// !DECL 0x00440f20 END
	// !DECL 0x00441080 BEGIN
	/* 441080 */ static uchar BlitChromaKey_BGRA32_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9);
	// !DECL 0x00441080 END
	// !DECL 0x00441220 BEGIN
	/* 441220 */ static uchar BlitChromaKey_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9);
	// !DECL 0x00441220 END
	// !DECL 0x00441390 BEGIN
	/* 441390 */ static uchar BlitChromaKey_BGRA32_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9);
	// !DECL 0x00441390 END
	// !DECL 0x004414e0 BEGIN
	/* 4414E0 */ static uchar BlitMasked(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x004414e0 END
	// !DECL 0x004416a0 BEGIN
	/* 4416A0 */ static uchar BlitMasked_Indexed1_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x004416a0 END
	// !DECL 0x00441830 BEGIN
	/* 441830 */ static uchar BlitMasked_PAL1_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00441830 END
	// !DECL 0x004419b0 BEGIN
	/* 4419B0 */ static uchar BlitKeyAndMask_2bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x004419b0 END
	// !DECL 0x00441b70 BEGIN
	/* 441B70 */ static uchar BlitMasked_Indexed2_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00441b70 END
	// !DECL 0x00441d00 BEGIN
	/* 441D00 */ static uchar BlitMasked_PAL2_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00441d00 END
	// !DECL 0x00441e80 BEGIN
	/* 441E80 */ static uchar BlitMasked_PAL4_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00441e80 END
	// !DECL 0x00442040 BEGIN
	/* 442040 */ static uchar BlitMasked_Indexed4_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00442040 END
	// !DECL 0x004421d0 BEGIN
	/* 4421D0 */ static uchar BlitMasked_PAL4_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x004421d0 END
	// !DECL 0x00442350 BEGIN
	/* 442350 */ static uchar BlitMasked_Idx8_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00442350 END
	// !DECL 0x004424f0 BEGIN
	/* 4424F0 */ static uchar BlitMasked_Idx8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x004424f0 END
	// !DECL 0x00442650 BEGIN
	/* 442650 */ static uchar BlitMasked_Idx8_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00442650 END
	// !DECL 0x004427a0 BEGIN
	/* 4427A0 */ static uchar BlitMasked_RGB565_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x004427a0 END
	// !DECL 0x00442960 BEGIN
	/* 442960 */ static uchar BlitMasked_RGB565_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00442960 END
	// !DECL 0x00442af0 BEGIN
	/* 442AF0 */ static uchar BlitMasked_RGB565_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00442af0 END
	// !DECL 0x00442c60 BEGIN
	/* 442C60 */ static uchar BlitMasked_BGR24_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00442c60 END
	// !DECL 0x00442e00 BEGIN
	/* 442E00 */ static uchar BlitMasked_BGR24_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00442e00 END
	// !DECL 0x00442f70 BEGIN
	/* 442F70 */ static uchar BlitMasked_BGR24_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00442f70 END
	// !DECL 0x004430d0 BEGIN
	/* 4430D0 */ static uchar BlitMasked_BGRA32_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x004430d0 END
	// !DECL 0x00443270 BEGIN
	/* 443270 */ static uchar BlitMasked_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x00443270 END
	// !DECL 0x004433d0 BEGIN
	/* 4433D0 */ static uchar BlitMasked_BGRA32_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10);
	// !DECL 0x004433d0 END
	// !DECL 0x00443520 BEGIN
	/* 443520 */ static uchar BlitKeyAndMask1bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00443520 END
	// !DECL 0x00443700 BEGIN
	/* 443700 */ static uchar BlitKeyAndMask_1bpp_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00443700 END
	// !DECL 0x004438a0 BEGIN
	/* 4438A0 */ static uchar BlitKeyAndMask_1bpp_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x004438a0 END
	// !DECL 0x00443a30 BEGIN
	/* 443A30 */ static uchar BlitKeyAndMask2bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00443a30 END
	// !DECL 0x00443c10 BEGIN
	/* 443C10 */ static uchar BlitAlphaBlend_2bitIndex_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00443c10 END
	// !DECL 0x00443db0 BEGIN
	/* 443DB0 */ static uchar BlitAlphaBlend_2bitIndex_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00443db0 END
	// !DECL 0x00443f40 BEGIN
	/* 443F40 */ static uchar BlitKeyAndMask4bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00443f40 END
	// !DECL 0x00444120 BEGIN
	/* 444120 */ static uchar BlitKeyAndMask_4bpp_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00444120 END
	// !DECL 0x004442c0 BEGIN
	/* 4442C0 */ static uchar BlitKeyAndMask_4bpp_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x004442c0 END
	// !DECL 0x00444450 BEGIN
	/* 444450 */ static uchar BlitAlphaBlend_ByteIndex_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00444450 END
	// !DECL 0x00444600 BEGIN
	/* 444600 */ static uchar BlitKeyAndMask_PAL8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00444600 END
	// !DECL 0x00444780 BEGIN
	/* 444780 */ static uchar BlitKeyAndMask_PAL8_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00444780 END
	// !DECL 0x004448f0 BEGIN
	/* 4448F0 */ static uchar BlitKeyAndMask16bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x004448f0 END
	// !DECL 0x00444ac0 BEGIN
	/* 444AC0 */ static uchar BlitKeyAndMask_RGB565_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00444ac0 END
	// !DECL 0x00444c60 BEGIN
	/* 444C60 */ static uchar BlitKeyAndMask_RGB565_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00444c60 END
	// !DECL 0x00444df0 BEGIN
	/* 444DF0 */ static uchar BlitKeyAndMask_BGR24_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00444df0 END
	// !DECL 0x00444fb0 BEGIN
	/* 444FB0 */ static uchar BlitMasked_BGR24_to_BGR24_00444fb0(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00444fb0 END
	// !DECL 0x00445130 BEGIN
	/* 445130 */ static uchar BlitKeyAndMask_BGR24_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00445130 END
	// !DECL 0x004452a0 BEGIN
	/* 4452A0 */ static uchar BlitAlphaBlend_DwordIndex_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x004452a0 END
	// !DECL 0x00445450 BEGIN
	/* 445450 */ static uchar BlitKeyAndMask_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x00445450 END
	// !DECL 0x004455d0 BEGIN
	/* 4455D0 */ static uchar BlitKeyAndMask_BGRA32_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11);
	// !DECL 0x004455d0 END
	// !DECL 0x00446620 BEGIN
	/* 446620 */ uchar CDSMpx_FinishMadFrame(int param_1);
	// !DECL 0x00446620 END
	// !DECL 0x00446c30 BEGIN
	/* 446C30 */ void CDSSafeStream_ClearThreadSlices(CDSSafeStream* param_1);
	// !DECL 0x00446c30 END
	// !DECL 0x004470c0 BEGIN
	/* 4470C0 */ bool CDSWorkingThread_PollExited(DWORD param_1);
	// !DECL 0x004470c0 END
	// !DECL 0x004470f0 BEGIN
	/* 4470F0 */ uchar CDSWorkingThread_ThreadProc(int* param_1);
	// !DECL 0x004470f0 END
	// !DECL 0x0044712e BEGIN
	/* 44712E */ uchar* Catch_0044712e();
	// !DECL 0x0044712e END
	// !DECL 0x0044714a BEGIN
	/* 44714A */ uint CDSWorkingThread_ThreadProcEpilogue();
	// !DECL 0x0044714a END
	// !DECL 0x00447190 BEGIN
	/* 447190 */ uchar CDSWorkingThread_StopAndJoin(DWORD param_1);
	// !DECL 0x00447190 END
	// !DECL 0x004473e8 BEGIN
	/* 4473E8 */ static void _unlock_4_free_epilog();
	// !DECL 0x004473e8 END
	// !DECL 0x0044751c BEGIN
	/* 44751C */ uchar _unlock_4_calloc_impl_epilog();
	// !DECL 0x0044751c END
	// !DECL 0x004476c1 BEGIN
	/* 4476C1 */ static void _unlock_4_realloc_epilog();
	// !DECL 0x004476c1 END
	// !DECL 0x00447897 BEGIN
	/* 447897 */ static uint FID_conflict_CxxFrameHandler3(EHExceptionRecord* param_1, EHRegistrationNode* param_2, void* param_3, void* param_4);
	// !DECL 0x00447897 END
	// !DECL 0x00447c42 BEGIN
	/* 447C42 */ static uchar OperatorNewWithBadAlloc(size_t param_1);
	// !DECL 0x00447c42 END
	// !DECL 0x00447e6c BEGIN
	/* 447E6C */ uchar _onexit_unlock8_epilog();
	// !DECL 0x00447e6c END
	// !DECL 0x004486a4 BEGIN
	/* 4486A4 */ void eh_vector_constructor_iterator_unwind_epilog();
	// !DECL 0x004486a4 END
	// !DECL 0x00448765 BEGIN
	/* 448765 */ void eh_vector_destructor_iterator_unwind_epilog();
	// !DECL 0x00448765 END
	// !DECL 0x004489d1 BEGIN
	/* 4489D1 */ static errno_t FID_conflict_wcslwr_s_l(wchar_t* param_1, size_t param_2, int param_3);
	// !DECL 0x004489d1 END
	// !DECL 0x00448a97 BEGIN
	/* 448A97 */ static void _crt_default_thread_hook_nop();
	// !DECL 0x00448a97 END
	// !DECL 0x00448bfb BEGIN
	/* 448BFB */ static uchar fast_error_exit(int param_1);
	// !DECL 0x00448bfb END
	// !DECL 0x00448c1f BEGIN
	/* 448C1F */ static uint check_managed_app();
	// !DECL 0x00448c1f END
	// !DECL 0x00449386 BEGIN
	/* 449386 */ static void _unlock_4_V6_HeapAlloc_epilog();
	// !DECL 0x00449386 END
	// !DECL 0x004494ec BEGIN
	/* 4494EC */ void _unlock_4_msize_epilog();
	// !DECL 0x004494ec END
	// !DECL 0x00449b1b BEGIN
	/* 449B1B */ static void _unlock(int param_1);
	// !DECL 0x00449b1b END
	// !DECL 0x00449bea BEGIN
	/* 449BEA */ void _unlock_10_mtinit_epilog();
	// !DECL 0x00449bea END
	// !DECL 0x00449c90 BEGIN
	/* 449C90 */ uint* _sbh_find_block(void* param_1);
	// !DECL 0x00449c90 END
	// !DECL 0x0044a916 BEGIN
	/* 44A916 */ static void _initp_encoded_null_callnewh(uint param_1);
	// !DECL 0x0044a916 END
	// !DECL 0x0044a942 BEGIN
	/* 44A942 */ static void _initp_encoded_null_invalid_parameter(uint param_1);
	// !DECL 0x0044a942 END
	// !DECL 0x0044ab44 BEGIN
	/* 44AB44 */ void* _fls_getvalue(uint param_1);
	// !DECL 0x0044ab44 END
	// !DECL 0x0044ab59 BEGIN
	/* 44AB59 */ int _get_flsindex();
	// !DECL 0x0044ab59 END
	// !DECL 0x0044ac8a BEGIN
	/* 44AC8A */ uchar _unlock_12_initptd_epilog();
	// !DECL 0x0044ac8a END
	// !DECL 0x0044ae3a BEGIN
	/* 44AE3A */ uchar _unlock_13_freefls_epilog();
	// !DECL 0x0044ae3a END
	// !DECL 0x0044ae46 BEGIN
	/* 44AE46 */ void _unlock_12_freefls_epilog();
	// !DECL 0x0044ae46 END
	// !DECL 0x0044b1cf BEGIN
	/* 44B1CF */ uchar _FrameUnwindToState_processing_throw_epilog();
	// !DECL 0x0044b1cf END
	// !DECL 0x0044b352 BEGIN
	/* 44B352 */ uchar Catch_All_0044b352();
	// !DECL 0x0044b352 END
	// !DECL 0x0044b490 BEGIN
	/* 44B490 */ uchar CallCatchBlock_epilog();
	// !DECL 0x0044b490 END
	// !DECL 0x0044bfad BEGIN
	/* 44BFAD */ static uchar fastzero_I(void* param_1, uint param_2);
	// !DECL 0x0044bfad END
	// !DECL 0x0044c19e BEGIN
	/* 44C19E */ static void _crtCorExitProcess(int param_1);
	// !DECL 0x0044c19e END
	// !DECL 0x0044c1d9 BEGIN
	/* 44C1D9 */ static void _lock_8_onexit_prolog();
	// !DECL 0x0044c1d9 END
	// !DECL 0x0044c1e2 BEGIN
	/* 44C1E2 */ void _unlock_8_onexit_stub();
	// !DECL 0x0044c1e2 END
	// !DECL 0x0044c328 BEGIN
	/* 44C328 */ static uchar doexit(int param_1, int param_2, int param_3);
	// !DECL 0x0044c328 END
	// !DECL 0x0044c3e1 BEGIN
	/* 44C3E1 */ void _unlock_8_doexit_epilog();
	// !DECL 0x0044c3e1 END
	// !DECL 0x0044c482 BEGIN
	/* 44C482 */ static void _VEC_memcpy_movdqa_tail(uint* param_1, uint* param_2, uint param_3);
	// !DECL 0x0044c482 END
	// !DECL 0x0044c895 BEGIN
	/* 44C895 */ uchar _unlock_13_updatetmbcinfo_epilog();
	// !DECL 0x0044c895 END
	// !DECL 0x0044cc24 BEGIN
	/* 44CC24 */ void _unlock_13_setmbcp_epilog();
	// !DECL 0x0044cc24 END
	// !DECL 0x0044d594 BEGIN
	/* 44D594 */ static void _initp_encoded_null_purecall(uint param_1);
	// !DECL 0x0044d594 END
	// !DECL 0x0044e3a0 BEGIN
	/* 44E3A0 */ static uchar parse_cmdline(uint* param_1, uchar* param_2, int* param_3);
	// !DECL 0x0044e3a0 END
	// !DECL 0x0044ef20 BEGIN
	/* 44EF20 */ static uchar write_char(wchar_t param_1);
	// !DECL 0x0044ef20 END
	// !DECL 0x0044ef45 BEGIN
	/* 44EF45 */ static uchar write_multi_char(wchar_t param_1, int param_2);
	// !DECL 0x0044ef45 END
	// !DECL 0x0044ef6a BEGIN
	/* 44EF6A */ static uchar write_string(int param_1);
	// !DECL 0x0044ef6a END
	// !DECL 0x0044faab BEGIN
	/* 44FAAB */ static void _initp_encoded_null_critsec_spin(uint param_1);
	// !DECL 0x0044faab END
	// !DECL 0x0044fcdf BEGIN
	/* 44FCDF */ void _clear_fatal_exit_flag_hook();
	// !DECL 0x0044fcdf END
	// !DECL 0x0044fe54 BEGIN
	/* 44FE54 */ void _NLG_unwind_dispatch_stub();
	// !DECL 0x0044fe54 END
	// !DECL 0x0044fe57 BEGIN
	/* 44FE57 */ uint _get_sse2_info_probe();
	// !DECL 0x0044fe57 END
	// !DECL 0x0044ff2d BEGIN
	/* 44FF2D */ static uint siglookup(uint param_1);
	// !DECL 0x0044ff2d END
	// !DECL 0x004500e2 BEGIN
	/* 4500E2 */ uchar _unlock_0_raise_epilog();
	// !DECL 0x004500e2 END
	// !DECL 0x0045011e BEGIN
	/* 45011E */ static void _initp_encoded_null_slot5(uint param_1);
	// !DECL 0x0045011e END
	// !DECL 0x004510db BEGIN
	/* 4510DB */ static int FID_conflict_atoflt_l(_CRT_FLOAT* param_1, char* param_2, int param_3);
	// !DECL 0x004510db END
	// !DECL 0x00451181 BEGIN
	/* 451181 */ static int FID_conflict_atoflt_l_00451181(_CRT_FLOAT* param_1, char* param_2, int param_3);
	// !DECL 0x00451181 END
	// !DECL 0x00451825 BEGIN
	/* 451825 */ static int FID_conflict_ld12tod(_LDBL12* param_1, _CRT_DOUBLE* param_2);
	// !DECL 0x00451825 END
	// !DECL 0x00451d67 BEGIN
	/* 451D67 */ static int FID_conflict_ld12tod_00451d67(_LDBL12* param_1, _CRT_DOUBLE* param_2);
	// !DECL 0x00451d67 END
	// !DECL 0x00452869 BEGIN
	/* 452869 */ void _lseeki64_unlock_fhandle_epilog();
	// !DECL 0x00452869 END
	// !DECL 0x00452f0b BEGIN
	/* 452F0B */ void _write_unlock_fhandle_epilog();
	// !DECL 0x00452f0b END
	// !DECL 0x00452fb7 BEGIN
	/* 452FB7 */ static FILE* _iob_func();
	// !DECL 0x00452fb7 END
	// !DECL 0x00453c4c BEGIN
	/* 453C4C */ static uint I10_OUTPUT(int param_1, uint param_2, ushort param_3, int param_4, uchar param_5, short* param_6);
	// !DECL 0x00453c4c END
	// !DECL 0x00454ace BEGIN
	/* 454ACE */ void _unlock_10();
	// !DECL 0x00454ace END
	// !DECL 0x00454ad7 BEGIN
	/* 454AD7 */ static uchar MSVCRT_unlock_fhandle(uint param_1);
	// !DECL 0x00454ad7 END
	// !DECL 0x00456800 BEGIN
	/* 456800 */ static uchar CDSMpx_FreeMadAuxAlloc(int param_1);
	// !DECL 0x00456800 END
	// !DECL 0x00458f20 BEGIN
	/* 458F20 */ static void mad_frame_finish(int* param_1);
	// !DECL 0x00458f20 END
	// !DECL 0x00459900 BEGIN
	/* 459900 */ static int III_sideinfo(uint param_1, int param_2, uint* param_3, int* param_4);
	// !DECL 0x00459900 END
	// !DECL 0x00459b80 BEGIN
	/* 459B80 */ static uchar decode_mcu(int* param_1, int param_2, int param_3, uchar* param_4);
	// !DECL 0x00459b80 END
	// !DECL 0x00459e50 BEGIN
	/* 459E50 */ static uchar III_scalefactors(int param_1, uchar param_2);
	// !DECL 0x00459e50 END
	// !DECL 0x0045a060 BEGIN
	/* 45A060 */ static uchar III_exponents(uchar* param_1, int* param_2);
	// !DECL 0x0045a060 END
	// !DECL 0x0045a220 BEGIN
	/* 45A220 */ uint III_huffdec_fixed_mul(int param_1);
	// !DECL 0x0045a220 END
	// !DECL 0x0045a2c0 BEGIN
	/* 45A2C0 */ static uint III_huffdecode(uint* param_1, ushort* param_2, uchar* param_3, int param_4);
	// !DECL 0x0045a2c0 END
	// !DECL 0x0045a9d0 BEGIN
	/* 45A9D0 */ uchar III_reorder(int param_1, uchar* param_2);
	// !DECL 0x0045a9d0 END
	// !DECL 0x0045ab00 BEGIN
	/* 45AB00 */ uint III_stereo(int param_1, int param_2, int param_3, uchar* param_4);
	// !DECL 0x0045ab00 END
	// !DECL 0x0045afb0 BEGIN
	/* 45AFB0 */ uchar III_aliasreduce(int param_1);
	// !DECL 0x0045afb0 END
	// !DECL 0x0045b080 BEGIN
	/* 45B080 */ uchar fastsdct(int* param_1);
	// !DECL 0x0045b080 END
	// !DECL 0x0045b250 BEGIN
	/* 45B250 */ static uchar imdct36(int param_1, int* param_2);
	// !DECL 0x0045b250 END
	// !DECL 0x0045b370 BEGIN
	/* 45B370 */ static uchar dctIV(int* param_1);
	// !DECL 0x0045b370 END
	// !DECL 0x0045b480 BEGIN
	/* 45B480 */ static void imdct36_post_dctIV_scatter(int* param_1);
	// !DECL 0x0045b480 END
	// !DECL 0x0045b530 BEGIN
	/* 45B530 */ void III_imdct_l(uint param_1);
	// !DECL 0x0045b530 END
	// !DECL 0x0045b7a0 BEGIN
	/* 45B7A0 */ static uchar III_imdct_s(int param_1);
	// !DECL 0x0045b7a0 END
	// !DECL 0x0045ba90 BEGIN
	/* 45BA90 */ uchar III_overlap(int param_1, int param_2, int param_3);
	// !DECL 0x0045ba90 END
	// !DECL 0x0045bb40 BEGIN
	/* 45BB40 */ uchar III_freqinver();
	// !DECL 0x0045bb40 END
	// !DECL 0x0045bbb0 BEGIN
	/* 45BBB0 */ static int III_decode(int* param_1, int param_2, int param_3, uint param_4);
	// !DECL 0x0045bbb0 END
	// !DECL 0x0045d160 BEGIN
	/* 45D160 */ static uchar jpeg_abort(int param_1);
	// !DECL 0x0045d160 END
	// !DECL 0x0045d1a0 BEGIN
	/* 45D1A0 */ static uchar jpeg_destroy(int param_1);
	// !DECL 0x0045d1a0 END
	// !DECL 0x0045d1d0 BEGIN
	/* 45D1D0 */ static void* jpeg_alloc_quant_table(void* param_1);
	// !DECL 0x0045d1d0 END
	// !DECL 0x0045d1f0 BEGIN
	/* 45D1F0 */ static uchar alloc_small(int param_1);
	// !DECL 0x0045d1f0 END
	// !DECL 0x0045d210 BEGIN
	/* 45D210 */ uchar get_soi();
	// !DECL 0x0045d210 END
	// !DECL 0x0045d2a0 BEGIN
	/* 45D2A0 */ uint get_sof(uchar param_1);
	// !DECL 0x0045d2a0 END
	// !DECL 0x0045d560 BEGIN
	/* 45D560 */ uint get_sos();
	// !DECL 0x0045d560 END
	// !DECL 0x0045d7d0 BEGIN
	/* 45D7D0 */ static uchar get_dht(int* param_1);
	// !DECL 0x0045d7d0 END
	// !DECL 0x0045da50 BEGIN
	/* 45DA50 */ static uchar get_dqt(int* param_1);
	// !DECL 0x0045da50 END
	// !DECL 0x0045dc40 BEGIN
	/* 45DC40 */ static uchar get_dri(int* param_1);
	// !DECL 0x0045dc40 END
	// !DECL 0x0045dd20 BEGIN
	/* 45DD20 */ void examine_app0(uint param_1);
	// !DECL 0x0045dd20 END
	// !DECL 0x0045dda0 BEGIN
	/* 45DDA0 */ uchar process_APP14(uint param_1);
	// !DECL 0x0045dda0 END
	// !DECL 0x0045dde0 BEGIN
	/* 45DDE0 */ static uchar save_marker(int* param_1);
	// !DECL 0x0045dde0 END
	// !DECL 0x0045df60 BEGIN
	/* 45DF60 */ static uchar skip_variable(int param_1);
	// !DECL 0x0045df60 END
	// !DECL 0x0045dfe0 BEGIN
	/* 45DFE0 */ uint next_marker();
	// !DECL 0x0045dfe0 END
	// !DECL 0x0045e0c0 BEGIN
	/* 45E0C0 */ static uint first_marker(int* param_1);
	// !DECL 0x0045e0c0 END
	// !DECL 0x0045e170 BEGIN
	/* 45E170 */ static uint read_markers(int* param_1);
	// !DECL 0x0045e170 END
	// !DECL 0x0045e4d0 BEGIN
	/* 45E4D0 */ static uint read_restart_marker(int param_1);
	// !DECL 0x0045e4d0 END
	// !DECL 0x0045e540 BEGIN
	/* 45E540 */ static uint jpeg_resync_to_restart(int param_1, int param_2);
	// !DECL 0x0045e540 END
	// !DECL 0x0045e5f0 BEGIN
	/* 45E5F0 */ static uchar reset_marker_reader(int param_1);
	// !DECL 0x0045e5f0 END
	// !DECL 0x0045e620 BEGIN
	/* 45E620 */ static uchar jinit_marker_reader(int param_1);
	// !DECL 0x0045e620 END
	// !DECL 0x0045e6a0 BEGIN
	/* 45E6A0 */ static uchar jpeg_CreateDecompress(int* param_1, int param_2, int param_3);
	// !DECL 0x0045e6a0 END
	// !DECL 0x0045e7a0 BEGIN
	/* 45E7A0 */ uchar default_decompress_parms();
	// !DECL 0x0045e7a0 END
	// !DECL 0x0045e8d0 BEGIN
	/* 45E8D0 */ static int jpeg_consume_input(int* param_1);
	// !DECL 0x0045e8d0 END
	// !DECL 0x0045e9a0 BEGIN
	/* 45E9A0 */ static uint jpeg_finish_decompress(int* param_1);
	// !DECL 0x0045e9a0 END
	// !DECL 0x0045ea70 BEGIN
	/* 45EA70 */ static int jpeg_read_header(int* param_1, char param_2);
	// !DECL 0x0045ea70 END
	// !DECL 0x0045eaf0 BEGIN
	/* 45EAF0 */ uint output_pass_setup();
	// !DECL 0x0045eaf0 END
	// !DECL 0x0045eba0 BEGIN
	/* 45EBA0 */ static int jpeg_read_scanlines(int* param_1, uint param_2, uint param_3);
	// !DECL 0x0045eba0 END
	// !DECL 0x0045ec10 BEGIN
	/* 45EC10 */ static uint jpeg_start_decompress(int* param_1);
	// !DECL 0x0045ec10 END
	// !DECL 0x0045f7e0 BEGIN
	/* 45F7E0 */ static int jdiv_round_up(int param_1, int param_2);
	// !DECL 0x0045f7e0 END
	// !DECL 0x0045f7f0 BEGIN
	/* 45F7F0 */ static int jround_up(int param_1, int param_2);
	// !DECL 0x0045f7f0 END
	// !DECL 0x0045f810 BEGIN
	/* 45F810 */ static uchar jcopy_sample_rows(int param_1, int param_2, int param_3, int param_4, int param_5, size_t param_6);
	// !DECL 0x0045f810 END
	// !DECL 0x0045f860 BEGIN
	/* 45F860 */ static void jcopy_block_row(void* param_1, void* param_2, int param_3);
	// !DECL 0x0045f860 END
	// !DECL 0x0045f880 BEGIN
	/* 45F880 */ static void IJG_jzero_far(void* param_1, size_t param_2);
	// !DECL 0x0045f880 END
	// !DECL 0x0045f8a0 BEGIN
	/* 45F8A0 */ uchar initial_setup();
	// !DECL 0x0045f8a0 END
	// !DECL 0x0045faa0 BEGIN
	/* 45FAA0 */ uint per_scan_setup();
	// !DECL 0x0045faa0 END
	// !DECL 0x0045fc50 BEGIN
	/* 45FC50 */ uchar select_scan_quant_tables();
	// !DECL 0x0045fc50 END
	// !DECL 0x0045fd00 BEGIN
	/* 45FD00 */ static uchar jpeg_consume_markers(int param_1);
	// !DECL 0x0045fd00 END
	// !DECL 0x0045fd40 BEGIN
	/* 45FD40 */ static int jpeg_reset_input_controller(int* param_1);
	// !DECL 0x0045fd40 END
	// !DECL 0x0045fe00 BEGIN
	/* 45FE00 */ static uchar jpeg_start_input_pass(int* param_1);
	// !DECL 0x0045fe00 END
	// !DECL 0x0045fe60 BEGIN
	/* 45FE60 */ static uchar jinit_input_controller(int param_1);
	// !DECL 0x0045fe60 END
	// !DECL 0x0045feb0 BEGIN
	/* 45FEB0 */ uint use_merged_upsample(uint param_1, int param_2);
	// !DECL 0x0045feb0 END
	// !DECL 0x0045ff30 BEGIN
	/* 45FF30 */ static uchar jinit_d_main_controller(int* param_1);
	// !DECL 0x0045ff30 END
	// !DECL 0x00460160 BEGIN
	/* 460160 */ void prepare_range_limit_table(void* param_1);
	// !DECL 0x00460160 END
	// !DECL 0x00460200 BEGIN
	/* 460200 */ uchar master_selection();
	// !DECL 0x00460200 END
	// !DECL 0x00460380 BEGIN
	/* 460380 */ static uchar post_process_1pass(int* param_1);
	// !DECL 0x00460380 END
	// !DECL 0x004604d0 BEGIN
	/* 4604D0 */ static uchar jinit_d_post_controller(int param_1);
	// !DECL 0x004604d0 END
	// !DECL 0x00460510 BEGIN
	/* 460510 */ static uchar emit_byte(uchar param_1);
	// !DECL 0x00460510 END
	// !DECL 0x00460550 BEGIN
	/* 460550 */ static uchar emit_marker(uchar param_1);
	// !DECL 0x00460550 END
	// !DECL 0x00460570 BEGIN
	/* 460570 */ uchar emit_2bytes();
	// !DECL 0x00460570 END
	// !DECL 0x004605a0 BEGIN
	/* 4605A0 */ char encode_one_block(int* param_1);
	// !DECL 0x004605a0 END
	// !DECL 0x004606a0 BEGIN
	/* 4606A0 */ uchar emit_dht(int param_1, char param_2);
	// !DECL 0x004606a0 END
	// !DECL 0x00460790 BEGIN
	/* 460790 */ uchar emit_dri();
	// !DECL 0x00460790 END
	// !DECL 0x004607c0 BEGIN
	/* 4607C0 */ uchar emit_sof(int* param_1);
	// !DECL 0x004607c0 END
	// !DECL 0x00460870 BEGIN
	/* 460870 */ uchar emit_sos();
	// !DECL 0x00460870 END
	// !DECL 0x00460940 BEGIN
	/* 460940 */ uchar emit_jfif_app0();
	// !DECL 0x00460940 END
	// !DECL 0x004609e0 BEGIN
	/* 4609E0 */ uchar emit_adobe_app14();
	// !DECL 0x004609e0 END
	// !DECL 0x00460a70 BEGIN
	/* 460A70 */ static uchar write_marker_header(int* param_1, uchar param_2, uint param_3);
	// !DECL 0x00460a70 END
	// !DECL 0x00460ad0 BEGIN
	/* 460AD0 */ static uchar write_file_header(int param_1);
	// !DECL 0x00460ad0 END
	// !DECL 0x00460bf0 BEGIN
	/* 460BF0 */ static uchar write_scan_header(void* param_1);
	// !DECL 0x00460bf0 END
	// !DECL 0x00460cd0 BEGIN
	/* 460CD0 */ static uchar write_tables_only(int* param_1);
	// !DECL 0x00460cd0 END
	// !DECL 0x00460e80 BEGIN
	/* 460E80 */ void create_context_buffer();
	// !DECL 0x00460e80 END
	// !DECL 0x00460f30 BEGIN
	/* 460F30 */ static void create_context_buffer_edge_replicate(int param_1);
	// !DECL 0x00460f30 END
	// !DECL 0x00461080 BEGIN
	/* 461080 */ static void pre_process_context_row_replicate(int param_1);
	// !DECL 0x00461080 END
	// !DECL 0x00461160 BEGIN
	/* 461160 */ static void pre_process_context_row_shift(int* param_1);
	// !DECL 0x00461160 END
	// !DECL 0x00461460 BEGIN
	/* 461460 */ static void jinit_d_prep_controller_sample_bufs(int* param_1, char param_2);
	// !DECL 0x00461460 END
	// !DECL 0x00461540 BEGIN
	/* 461540 */ void start_iMCU_row();
	// !DECL 0x00461540 END
	// !DECL 0x004615b0 BEGIN
	/* 4615B0 */ static uint jpeg_decompress_data(int param_1, int param_2);
	// !DECL 0x004615b0 END
	// !DECL 0x004619e0 BEGIN
	/* 4619E0 */ static int decompress_data(int param_1, int param_2);
	// !DECL 0x004619e0 END
	// !DECL 0x00461b70 BEGIN
	/* 461B70 */ static int smoothing_ok(int* param_1);
	// !DECL 0x00461b70 END
	// !DECL 0x00462370 BEGIN
	/* 462370 */ static uchar jinit_d_coef_controller(uint* param_1, char param_2);
	// !DECL 0x00462370 END
	// !DECL 0x004624c0 BEGIN
	/* 4624C0 */ static uchar jpeg_make_d_derived_tbl(int* param_1, char param_2, int param_3, int* param_4);
	// !DECL 0x004624c0 END
	// !DECL 0x004627b0 BEGIN
	/* 4627B0 */ static uint jpeg_fill_bit_buffer(uint* param_1, uint param_2, int param_3, int param_4);
	// !DECL 0x004627b0 END
	// !DECL 0x004628b0 BEGIN
	/* 4628B0 */ static uint jpeg_huff_decode(uint* param_1, uint param_2, int param_3, int param_4, int param_5);
	// !DECL 0x004628b0 END
	// !DECL 0x00462980 BEGIN
	/* 462980 */ uint process_restart_00462980();
	// !DECL 0x00462980 END
	// !DECL 0x00462a00 BEGIN
	/* 462A00 */ static uint decode_mcu_00462a00(int param_1, int param_2);
	// !DECL 0x00462a00 END
	// !DECL 0x00462f30 BEGIN
	/* 462F30 */ static uchar jinit_huff_decoder(int param_1);
	// !DECL 0x00462f30 END
	// !DECL 0x00462f80 BEGIN
	/* 462F80 */ uint process_restart();
	// !DECL 0x00462f80 END
	// !DECL 0x00463010 BEGIN
	/* 463010 */ static uint jpeg_decode_mcu_DC_first(uint param_1, int param_2);
	// !DECL 0x00463010 END
	// !DECL 0x00463250 BEGIN
	/* 463250 */ static uint jpeg_decode_mcu_AC_first(int param_1, int* param_2);
	// !DECL 0x00463250 END
	// !DECL 0x004634a0 BEGIN
	/* 4634A0 */ static uint jpeg_decode_mcu_AC_refine(int param_1, int param_2);
	// !DECL 0x004634a0 END
	// !DECL 0x00463960 BEGIN
	/* 463960 */ static uchar start_pass_huff_decoder(int* param_1);
	// !DECL 0x00463960 END
	// !DECL 0x00463b50 BEGIN
	/* 463B50 */ static uchar jinit_huff_decoder_00463b50(int param_1);
	// !DECL 0x00463b50 END
	// !DECL 0x00463bc0 BEGIN
	/* 463BC0 */ static uchar start_pass_dcolor(int* param_1);
	// !DECL 0x00463bc0 END
	// !DECL 0x00463ee0 BEGIN
	/* 463EE0 */ static void jinit_color_deconverter_00463ee0(int* param_1);
	// !DECL 0x00463ee0 END
	// !DECL 0x00463fe0 BEGIN
	/* 463FE0 */ static uchar process_data_simple_main(int param_1, uint param_2, uint param_3, uint param_4, uint param_5, int* param_6);
	// !DECL 0x00463fe0 END
	// !DECL 0x00464090 BEGIN
	/* 464090 */ static uchar process_data_context_main(int param_1);
	// !DECL 0x00464090 END
	// !DECL 0x00464230 BEGIN
	/* 464230 */ static uchar jinit_d_main_controller_00464230(int param_1, char param_2);
	// !DECL 0x00464230 END
	// !DECL 0x004642f0 BEGIN
	/* 4642F0 */ static uchar sep_upsample(int param_1, int param_2, int* param_3, uint param_4, int param_5, int* param_6, int param_7);
	// !DECL 0x004642f0 END
	// !DECL 0x004643e0 BEGIN
	/* 4643E0 */ static uchar int_upsample(int param_1, uint* param_2, uint* param_3, int* param_4);
	// !DECL 0x004643e0 END
	// !DECL 0x00464660 BEGIN
	/* 464660 */ static uchar h2v2_smooth_downsample(int param_1, int param_2, uint* param_3, int* param_4);
	// !DECL 0x00464660 END
	// !DECL 0x004647a0 BEGIN
	/* 4647A0 */ static uchar jinit_upsampler(int* param_1);
	// !DECL 0x004647a0 END
	// !DECL 0x00464990 BEGIN
	/* 464990 */ uchar build_ycc_rgb_table();
	// !DECL 0x00464990 END
	// !DECL 0x00464a60 BEGIN
	/* 464A60 */ static uchar ycc_rgb_convert(int param_1, int* param_2, int param_3, uint* param_4, int param_5);
	// !DECL 0x00464a60 END
	// !DECL 0x00464ca0 BEGIN
	/* 464CA0 */ static uchar ycck_cmyk_convert(int param_1, int* param_2, int param_3, uint* param_4, int param_5);
	// !DECL 0x00464ca0 END
	// !DECL 0x00464e00 BEGIN
	/* 464E00 */ static uchar jinit_color_deconverter(int* param_1);
	// !DECL 0x00464e00 END
	// !DECL 0x00464f90 BEGIN
	/* 464F90 */ uchar build_ycc_rgb_table_00464f90();
	// !DECL 0x00464f90 END
	// !DECL 0x00465180 BEGIN
	/* 465180 */ static void h2v1_merged_upsample(int param_1, int* param_2, int param_3, uint* param_4);
	// !DECL 0x00465180 END
	// !DECL 0x004654f0 BEGIN
	/* 4654F0 */ static uchar jinit_merged_upsampler(int param_1);
	// !DECL 0x004654f0 END
	// !DECL 0x004655f0 BEGIN
	/* 4655F0 */ int* find_biggest_color_pop(int param_1, int param_2);
	// !DECL 0x004655f0 END
	// !DECL 0x00465620 BEGIN
	/* 465620 */ int* find_biggest_volume(int param_1, int param_2);
	// !DECL 0x00465620 END
	// !DECL 0x00465650 BEGIN
	/* 465650 */ uchar update_box(int* param_1);
	// !DECL 0x00465650 END
	// !DECL 0x00465a60 BEGIN
	/* 465A60 */ static int median_cut(void* param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00465a60 END
	// !DECL 0x00465bb0 BEGIN
	/* 465BB0 */ static uchar fill_inverse_cmap(int param_1, int param_2);
	// !DECL 0x00465bb0 END
	// !DECL 0x00465d30 BEGIN
	/* 465D30 */ static uchar select_colors(int param_1);
	// !DECL 0x00465d30 END
	// !DECL 0x00465da0 BEGIN
	/* 465DA0 */ uchar init_error_limit(int param_1, int param_2, int param_3);
	// !DECL 0x00465da0 END
	// !DECL 0x00465f70 BEGIN
	/* 465F70 */ static uchar prescan_quantize(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, uchar* param_7);
	// !DECL 0x00465f70 END
	// !DECL 0x004660f0 BEGIN
	/* 4660F0 */ uchar start_pass_1_quant(int param_1, int param_2, int param_3);
	// !DECL 0x004660f0 END
	// !DECL 0x00466230 BEGIN
	/* 466230 */ static uchar color_quantize3(int param_1, int param_2, int* param_3, int param_4);
	// !DECL 0x00466230 END
	// !DECL 0x00466300 BEGIN
	/* 466300 */ static uchar h2v2_merged_upsample(int param_1, int param_2, int* param_3, int param_4);
	// !DECL 0x00466300 END
	// !DECL 0x004665c0 BEGIN
	/* 4665C0 */ uchar init_inverse_cmap();
	// !DECL 0x004665c0 END
	// !DECL 0x004666a0 BEGIN
	/* 4666A0 */ static uchar start_pass_2(int* param_1, char param_2);
	// !DECL 0x004666a0 END
	// !DECL 0x004667d0 BEGIN
	/* 4667D0 */ static uchar jinit_2pass_quantizer(int* param_1);
	// !DECL 0x004667d0 END
	// !DECL 0x00466920 BEGIN
	/* 466920 */ static int compute_sample_table_size(int* param_1, int* param_2);
	// !DECL 0x00466920 END
	// !DECL 0x00466a10 BEGIN
	/* 466A10 */ int scale_dither_index_to_byte(int param_1);
	// !DECL 0x00466a10 END
	// !DECL 0x00466a30 BEGIN
	/* 466A30 */ int gen_codes_rounded_idiv_stub(int param_1);
	// !DECL 0x00466a30 END
	// !DECL 0x00466a50 BEGIN
	/* 466A50 */ static uchar build_colormap_sample_tables(int* param_1);
	// !DECL 0x00466a50 END
	// !DECL 0x00466cc0 BEGIN
	/* 466CC0 */ int* alloc_scaled_quant_table_from_rdata(int param_1);
	// !DECL 0x00466cc0 END
	// !DECL 0x00466d40 BEGIN
	/* 466D40 */ uchar init_scaled_quant_tables_dedup();
	// !DECL 0x00466d40 END
	// !DECL 0x00466e40 BEGIN
	/* 466E40 */ static uchar color_quantize(int param_1, int param_2, int* param_3, int param_4);
	// !DECL 0x00466e40 END
	// !DECL 0x00466f00 BEGIN
	/* 466F00 */ static void merged_upsample_row_non_rgb(int* param_1, int param_2, uint* param_3, int param_4);
	// !DECL 0x00466f00 END
	// !DECL 0x00467020 BEGIN
	/* 467020 */ static void h2v1_merged_upsample_rgb_row(int* param_1, int param_2, int* param_3, int param_4);
	// !DECL 0x00467020 END
	// !DECL 0x00467150 BEGIN
	/* 467150 */ static void h2v2_fancy_upsample_row(int* param_1, int param_2, uint** param_3, int param_4);
	// !DECL 0x00467150 END
	// !DECL 0x00467300 BEGIN
	/* 467300 */ void alloc_fancy_upsample_row_bufs(int* param_1);
	// !DECL 0x00467300 END
	// !DECL 0x00467340 BEGIN
	/* 467340 */ static void start_pass_merged_upsampler(int* param_1);
	// !DECL 0x00467340 END
	// !DECL 0x00467460 BEGIN
	/* 467460 */ static uchar jinit_merged_upsampler_00467460(int* param_1);
	// !DECL 0x00467460 END
	// !DECL 0x00467690 BEGIN
	/* 467690 */ void start_iMCU_row_00467690();
	// !DECL 0x00467690 END
	// !DECL 0x004676e0 BEGIN
	/* 4676E0 */ static uint jpeg_compress_data(int param_1, int param_2);
	// !DECL 0x004676e0 END
	// !DECL 0x00467930 BEGIN
	/* 467930 */ static bool compress_output(int param_1);
	// !DECL 0x00467930 END
	// !DECL 0x00467af0 BEGIN
	/* 467AF0 */ static uchar compress_first_pass(int param_1, uint* param_2);
	// !DECL 0x00467af0 END
	// !DECL 0x00467d10 BEGIN
	/* 467D10 */ static void start_pass_coef(int* param_1, int param_2);
	// !DECL 0x00467d10 END
	// !DECL 0x00467ed0 BEGIN
	/* 467ED0 */ static uchar jpeg_make_c_derived_tbl(int* param_1, char param_2, int param_3, int* param_4);
	// !DECL 0x00467ed0 END
	// !DECL 0x004680f0 BEGIN
	/* 4680F0 */ uchar emit_byte_s();
	// !DECL 0x004680f0 END
	// !DECL 0x00468120 BEGIN
	/* 468120 */ uint emit_bits_s(uint param_1);
	// !DECL 0x00468120 END
	// !DECL 0x004681d0 BEGIN
	/* 4681D0 */ uint flush_bits_s();
	// !DECL 0x004681d0 END
	// !DECL 0x00468200 BEGIN
	/* 468200 */ static uint encode_one_block_00468200(void* param_1, short* param_2, uint param_3, uint* param_4);
	// !DECL 0x00468200 END
	// !DECL 0x004683a0 BEGIN
	/* 4683A0 */ static uint emit_restart(void* param_1, char param_2);
	// !DECL 0x004683a0 END
	// !DECL 0x00468420 BEGIN
	/* 468420 */ static uint encode_mcu(int* param_1, int param_2);
	// !DECL 0x00468420 END
	// !DECL 0x00468590 BEGIN
	/* 468590 */ static void finish_pass_huff(int* param_1);
	// !DECL 0x00468590 END
	// !DECL 0x00468650 BEGIN
	/* 468650 */ static int* htest_one_block(int* param_1, short* param_2, int param_3);
	// !DECL 0x00468650 END
	// !DECL 0x00468810 BEGIN
	/* 468810 */ static uchar jpeg_gen_optimal_table(int* param_1, uint* param_2, int param_3);
	// !DECL 0x00468810 END
	// !DECL 0x00468af0 BEGIN
	/* 468AF0 */ static uchar finish_pass_gather(int* param_1);
	// !DECL 0x00468af0 END
	// !DECL 0x00468be0 BEGIN
	/* 468BE0 */ static void start_pass_huff(void* param_1, char param_2);
	// !DECL 0x00468be0 END
	// !DECL 0x00468dd0 BEGIN
	/* 468DD0 */ uchar emit_byte_e();
	// !DECL 0x00468dd0 END
	// !DECL 0x00468e10 BEGIN
	/* 468E10 */ uchar emit_bits_e(uint param_1);
	// !DECL 0x00468e10 END
	// !DECL 0x00468ed0 BEGIN
	/* 468ED0 */ uchar flush_bits_e();
	// !DECL 0x00468ed0 END
	// !DECL 0x00468ef0 BEGIN
	/* 468EF0 */ int emit_ac_symbol(void* param_1);
	// !DECL 0x00468ef0 END
	// !DECL 0x00468f20 BEGIN
	/* 468F20 */ uchar emit_buffered_bits(char* param_1);
	// !DECL 0x00468f20 END
	// !DECL 0x00468f50 BEGIN
	/* 468F50 */ uchar emit_eobrun();
	// !DECL 0x00468f50 END
	// !DECL 0x00468fd0 BEGIN
	/* 468FD0 */ static uchar emit_restart_00468fd0(char param_1);
	// !DECL 0x00468fd0 END
	// !DECL 0x00469060 BEGIN
	/* 469060 */ static bool encode_mcu_DC_first(int* param_1, int param_2);
	// !DECL 0x00469060 END
	// !DECL 0x004691b0 BEGIN
	/* 4691B0 */ static uint encode_one_block_004691b0(int* param_1, int* param_2);
	// !DECL 0x004691b0 END
	// !DECL 0x004696a0 BEGIN
	/* 4696A0 */ static void finish_pass_gather_004696a0(int* param_1);
	// !DECL 0x004696a0 END
	// !DECL 0x00469770 BEGIN
	/* 469770 */ static uchar start_pass_huff_decoder_00469770(int* param_1, char param_2);
	// !DECL 0x00469770 END
	// !DECL 0x0046a840 BEGIN
	/* 46A840 */ static void expand_bottom_edge(size_t param_1);
	// !DECL 0x0046a840 END
	// !DECL 0x0046a870 BEGIN
	/* 46A870 */ static void pre_process_data(int* param_1, int param_2, uint* param_3, uint param_4, int param_5, uint* param_6, uint param_7);
	// !DECL 0x0046a870 END
	// !DECL 0x0046aa00 BEGIN
	/* 46AA00 */ static uchar compress_output_0046aa00(int param_1, int param_2, uint* param_3, uint param_4, uint param_5, uint* param_6, uint param_7);
	// !DECL 0x0046aa00 END
	// !DECL 0x0046ada0 BEGIN
	/* 46ADA0 */ uchar expand_right_edge(int param_1, int param_2);
	// !DECL 0x0046ada0 END
	// !DECL 0x0046adf0 BEGIN
	/* 46ADF0 */ static void sep_downsample(int param_1, int* param_2, int param_3, int* param_4, int param_5);
	// !DECL 0x0046adf0 END
	// !DECL 0x0046ae80 BEGIN
	/* 46AE80 */ static void int_downsample(int param_1, int param_2, int* param_3, int param_4);
	// !DECL 0x0046ae80 END
	// !DECL 0x0046b000 BEGIN
	/* 46B000 */ static uchar h2v1_downsample(void* param_1, int param_2, uint* param_3, int param_4);
	// !DECL 0x0046b000 END
	// !DECL 0x0046b0b0 BEGIN
	/* 46B0B0 */ static uchar h2v2_downsample(void* param_1, int param_2, uint* param_3, int param_4);
	// !DECL 0x0046b0b0 END
	// !DECL 0x0046b180 BEGIN
	/* 46B180 */ static uchar h2v2_fancy_upsample(int param_1, int param_2, int param_3, int param_4);
	// !DECL 0x0046b180 END
	// !DECL 0x0046b400 BEGIN
	/* 46B400 */ static void fullsize_smooth_downsample(int param_1, int param_2, uint* param_3, int param_4);
	// !DECL 0x0046b400 END
	// !DECL 0x0046b6e0 BEGIN
	/* 46B6E0 */ static uchar rgb_ycc_start(int param_1);
	// !DECL 0x0046b6e0 END
	// !DECL 0x0046b7c0 BEGIN
	/* 46B7C0 */ static uchar rgb_ycc_convert(int param_1, uint* param_2, int* param_3, int param_4, int param_5);
	// !DECL 0x0046b7c0 END
	// !DECL 0x0046b8e0 BEGIN
	/* 46B8E0 */ static uchar rgb_gray_convert(int param_1, uint* param_2, int* param_3, int param_4, int param_5);
	// !DECL 0x0046b8e0 END
	// !DECL 0x0046b990 BEGIN
	/* 46B990 */ static uchar cmyk_ycck_convert(int param_1, uint* param_2, int* param_3, int param_4, int param_5);
	// !DECL 0x0046b990 END
	// !DECL 0x0046c3d0 BEGIN
	/* 46C3D0 */ static void select_scan_parameters(int param_1);
	// !DECL 0x0046c3d0 END
	// !DECL 0x0046c4c0 BEGIN
	/* 46C4C0 */ uint per_scan_setup_0046c4c0();
	// !DECL 0x0046c4c0 END
	// !DECL 0x0046c690 BEGIN
	/* 46C690 */ static void prepare_for_pass(int* param_1);
	// !DECL 0x0046c690 END
	// !DECL 0x0046c850 BEGIN
	/* 46C850 */ static void pass_startup(int* param_1);
	// !DECL 0x0046c850 END
	// !DECL 0x0046db50 BEGIN
	/* 46DB50 */ static void h2v1_merged_upsample_ycbcr_colormap(int param_1, int param_2, int param_3, int* param_4, int param_5);
	// !DECL 0x0046db50 END
	// !DECL 0x0046e000 BEGIN
	/* 46E000 */ static uchar jpeg_idct_islow(int param_1);
	// !DECL 0x0046e000 END
	// !DECL 0x0046e320 BEGIN
	/* 46E320 */ static uchar jpeg_idct_ifast(int param_1);
	// !DECL 0x0046e320 END
	// !DECL 0x0046f770 BEGIN
	/* 46F770 */ uchar lm_init();
	// !DECL 0x0046f770 END
	// !DECL 0x0046f970 BEGIN
	/* 46F970 */ uchar fill_window();
	// !DECL 0x0046f970 END
	// !DECL 0x0046fba0 BEGIN
	/* 46FBA0 */ static uchar deflate_fast(int* param_1, int param_2);
	// !DECL 0x0046fba0 END
	// !DECL 0x00470250 BEGIN
	/* 470250 */ static uint deflateResetKeep(int param_1);
	// !DECL 0x00470250 END
	// !DECL 0x00471300 BEGIN
	/* 471300 */ static uchar pqdownheap(int param_1);
	// !DECL 0x00471300 END
	// !DECL 0x004713d0 BEGIN
	/* 4713D0 */ uchar gen_bitlen_tail(int* param_1);
	// !DECL 0x004713d0 END
	// !DECL 0x004715f0 BEGIN
	/* 4715F0 */ uchar zlib_scan_tree(int param_1);
	// !DECL 0x004715f0 END
	// !DECL 0x004716e0 BEGIN
	/* 4716E0 */ uchar send_tree(int param_1, int param_2);
	// !DECL 0x004716e0 END
	// !DECL 0x00472270 BEGIN
	/* 472270 */ uchar detect_data_type(int param_1);
	// !DECL 0x00472270 END
	// !DECL 0x00472300 BEGIN
	/* 472300 */ uint bi_reverse(uint param_1, int param_2);
	// !DECL 0x00472300 END
	// !DECL 0x00472320 BEGIN
	/* 472320 */ uchar bi_flush();
	// !DECL 0x00472320 END
	// !DECL 0x004723a0 BEGIN
	/* 4723A0 */ uchar bi_windup();
	// !DECL 0x004723a0 END
	// !DECL 0x00472420 BEGIN
	/* 472420 */ void copy_block(uint param_1, uchar* param_2, int param_3);
	// !DECL 0x00472420 END
	// !DECL 0x004724b0 BEGIN
	/* 4724B0 */ static uchar zlib_tr_init(int param_1);
	// !DECL 0x004724b0 END
	// !DECL 0x00472520 BEGIN
	/* 472520 */ uchar gen_codes(uint param_1, int param_2);
	// !DECL 0x00472520 END
	// !DECL 0x004725a0 BEGIN
	/* 4725A0 */ static uchar build_tree(int* param_1);
	// !DECL 0x004725a0 END
	// !DECL 0x004727a0 BEGIN
	/* 4727A0 */ uchar build_bl_tree();
	// !DECL 0x004727a0 END
	// !DECL 0x00472870 BEGIN
	/* 472870 */ static uchar send_bits(int param_1, uchar* param_2, int param_3, int param_4);
	// !DECL 0x00472870 END
	// !DECL 0x00472910 BEGIN
	/* 472910 */ static uchar _tr_align(int param_1);
	// !DECL 0x00472910 END
	// !DECL 0x00472b00 BEGIN
	/* 472B00 */ static uchar _tr_flush_block(int param_1, uchar* param_2, int param_3, int param_4);
	// !DECL 0x00472b00 END
	// !DECL 0x00472cf0 BEGIN
	/* 472CF0 */ static uchar inflate_codes_new(uchar param_1, uchar param_2, uint param_3, uint param_4, int param_5);
	// !DECL 0x00472cf0 END
	// !DECL 0x00472d30 BEGIN
	/* 472D30 */ static uchar inflate_codes(uint param_1, int* param_2, int param_3);
	// !DECL 0x00472d30 END
	// !DECL 0x00473ac0 BEGIN
	/* 473AC0 */ static uint zlib_inflate_fixed(uint* param_1, uint* param_2, uint* param_3, uint* param_4);
	// !DECL 0x00473ac0 END
	// !DECL 0x00473af0 BEGIN
	/* 473AF0 */ static int zlib_inflate_flush(int param_1, void* param_2, int param_3);
	// !DECL 0x00473af0 END
	// !DECL 0x00473f40 BEGIN
	/* 473F40 */ uchar Unwind_00473f40();
	// !DECL 0x00473f40 END
	// !DECL 0x00473f80 BEGIN
	/* 473F80 */ uchar Unwind_00473f80();
	// !DECL 0x00473f80 END
	// !DECL 0x00473f88 BEGIN
	/* 473F88 */ uchar Unwind_00473f88();
	// !DECL 0x00473f88 END
	// !DECL 0x00473fb0 BEGIN
	/* 473FB0 */ uchar Unwind_00473fb0();
	// !DECL 0x00473fb0 END
	// !DECL 0x00473fe0 BEGIN
	/* 473FE0 */ uchar Unwind_00473fe0();
	// !DECL 0x00473fe0 END
	// !DECL 0x00474020 BEGIN
	/* 474020 */ uchar Unwind_00474020();
	// !DECL 0x00474020 END
	// !DECL 0x00474028 BEGIN
	/* 474028 */ uchar Unwind_00474028();
	// !DECL 0x00474028 END
	// !DECL 0x00474030 BEGIN
	/* 474030 */ uchar Unwind_00474030();
	// !DECL 0x00474030 END
	// !DECL 0x00474060 BEGIN
	/* 474060 */ uchar Unwind_00474060();
	// !DECL 0x00474060 END
	// !DECL 0x00474068 BEGIN
	/* 474068 */ uchar Unwind_00474068();
	// !DECL 0x00474068 END
	// !DECL 0x00474090 BEGIN
	/* 474090 */ uchar Unwind_00474090();
	// !DECL 0x00474090 END
	// !DECL 0x00474098 BEGIN
	/* 474098 */ uchar Unwind_00474098();
	// !DECL 0x00474098 END
	// !DECL 0x004740a3 BEGIN
	/* 4740A3 */ uchar Unwind_004740a3();
	// !DECL 0x004740a3 END
	// !DECL 0x004740ae BEGIN
	/* 4740AE */ uchar Unwind_004740ae();
	// !DECL 0x004740ae END
	// !DECL 0x004740b9 BEGIN
	/* 4740B9 */ uchar Unwind_004740b9();
	// !DECL 0x004740b9 END
	// !DECL 0x004740e0 BEGIN
	/* 4740E0 */ uchar Unwind_004740e0();
	// !DECL 0x004740e0 END
	// !DECL 0x00474110 BEGIN
	/* 474110 */ uchar Unwind_00474110();
	// !DECL 0x00474110 END
	// !DECL 0x00474140 BEGIN
	/* 474140 */ uchar Unwind_00474140();
	// !DECL 0x00474140 END
	// !DECL 0x00474190 BEGIN
	/* 474190 */ uchar Unwind_00474190();
	// !DECL 0x00474190 END
	// !DECL 0x004741c0 BEGIN
	/* 4741C0 */ uchar Unwind_004741c0();
	// !DECL 0x004741c0 END
	// !DECL 0x004741c8 BEGIN
	/* 4741C8 */ uchar Unwind_004741c8();
	// !DECL 0x004741c8 END
	// !DECL 0x004741d0 BEGIN
	/* 4741D0 */ uchar Unwind_004741d0();
	// !DECL 0x004741d0 END
	// !DECL 0x004741d8 BEGIN
	/* 4741D8 */ uchar Unwind_004741d8();
	// !DECL 0x004741d8 END
	// !DECL 0x004741e3 BEGIN
	/* 4741E3 */ uchar Unwind_004741e3();
	// !DECL 0x004741e3 END
	// !DECL 0x004741eb BEGIN
	/* 4741EB */ uchar Unwind_004741eb();
	// !DECL 0x004741eb END
	// !DECL 0x004741f3 BEGIN
	/* 4741F3 */ uchar Unwind_004741f3();
	// !DECL 0x004741f3 END
	// !DECL 0x004741fb BEGIN
	/* 4741FB */ uchar Unwind_004741fb();
	// !DECL 0x004741fb END
	// !DECL 0x00474203 BEGIN
	/* 474203 */ uchar Unwind_00474203();
	// !DECL 0x00474203 END
	// !DECL 0x0047420e BEGIN
	/* 47420E */ uchar Unwind_0047420e();
	// !DECL 0x0047420e END
	// !DECL 0x00474240 BEGIN
	/* 474240 */ uchar Unwind_00474240();
	// !DECL 0x00474240 END
	// !DECL 0x00474280 BEGIN
	/* 474280 */ uchar Unwind_00474280();
	// !DECL 0x00474280 END
	// !DECL 0x004742b0 BEGIN
	/* 4742B0 */ uchar Unwind_004742b0();
	// !DECL 0x004742b0 END
	// !DECL 0x004742be BEGIN
	/* 4742BE */ uchar Unwind_004742be();
	// !DECL 0x004742be END
	// !DECL 0x004742f0 BEGIN
	/* 4742F0 */ uchar Unwind_004742f0();
	// !DECL 0x004742f0 END
	// !DECL 0x004742f8 BEGIN
	/* 4742F8 */ uchar Unwind_004742f8();
	// !DECL 0x004742f8 END
	// !DECL 0x00474306 BEGIN
	/* 474306 */ uchar Unwind_00474306();
	// !DECL 0x00474306 END
	// !DECL 0x00474314 BEGIN
	/* 474314 */ uchar Unwind_00474314();
	// !DECL 0x00474314 END
	// !DECL 0x00474322 BEGIN
	/* 474322 */ uchar Unwind_00474322();
	// !DECL 0x00474322 END
	// !DECL 0x00474350 BEGIN
	/* 474350 */ uchar Unwind_00474350();
	// !DECL 0x00474350 END
	// !DECL 0x00474358 BEGIN
	/* 474358 */ uchar Unwind_00474358();
	// !DECL 0x00474358 END
	// !DECL 0x00474366 BEGIN
	/* 474366 */ uchar Unwind_00474366();
	// !DECL 0x00474366 END
	// !DECL 0x00474374 BEGIN
	/* 474374 */ uchar Unwind_00474374();
	// !DECL 0x00474374 END
	// !DECL 0x004743a0 BEGIN
	/* 4743A0 */ uchar Unwind_004743a0();
	// !DECL 0x004743a0 END
	// !DECL 0x004743d0 BEGIN
	/* 4743D0 */ uchar Unwind_004743d0();
	// !DECL 0x004743d0 END
	// !DECL 0x00474400 BEGIN
	/* 474400 */ uchar Unwind_00474400();
	// !DECL 0x00474400 END
	// !DECL 0x0047440b BEGIN
	/* 47440B */ uchar Unwind_0047440b();
	// !DECL 0x0047440b END
	// !DECL 0x00474416 BEGIN
	/* 474416 */ uchar Unwind_00474416();
	// !DECL 0x00474416 END
	// !DECL 0x00474450 BEGIN
	/* 474450 */ uchar Unwind_00474450();
	// !DECL 0x00474450 END
	// !DECL 0x00474480 BEGIN
	/* 474480 */ uchar Unwind_00474480();
	// !DECL 0x00474480 END
	// !DECL 0x00474488 BEGIN
	/* 474488 */ uchar Unwind_00474488();
	// !DECL 0x00474488 END
	// !DECL 0x00474493 BEGIN
	/* 474493 */ uchar Unwind_00474493();
	// !DECL 0x00474493 END
	// !DECL 0x004744c0 BEGIN
	/* 4744C0 */ uchar Unwind_004744c0();
	// !DECL 0x004744c0 END
	// !DECL 0x004744f0 BEGIN
	/* 4744F0 */ uchar Unwind_004744f0();
	// !DECL 0x004744f0 END
	// !DECL 0x004744f8 BEGIN
	/* 4744F8 */ uchar Unwind_004744f8();
	// !DECL 0x004744f8 END
	// !DECL 0x00474520 BEGIN
	/* 474520 */ uchar Unwind_00474520();
	// !DECL 0x00474520 END
	// !DECL 0x00474528 BEGIN
	/* 474528 */ uchar Unwind_00474528();
	// !DECL 0x00474528 END
	// !DECL 0x00474570 BEGIN
	/* 474570 */ uchar Unwind_00474570();
	// !DECL 0x00474570 END
	// !DECL 0x004745a0 BEGIN
	/* 4745A0 */ uchar Unwind_004745a0();
	// !DECL 0x004745a0 END
	// !DECL 0x004745d0 BEGIN
	/* 4745D0 */ uchar Unwind_004745d0();
	// !DECL 0x004745d0 END
	// !DECL 0x00474600 BEGIN
	/* 474600 */ uchar Unwind_00474600();
	// !DECL 0x00474600 END
	// !DECL 0x00474630 BEGIN
	/* 474630 */ uchar Unwind_00474630();
	// !DECL 0x00474630 END
	// !DECL 0x00474660 BEGIN
	/* 474660 */ uchar Unwind_00474660();
	// !DECL 0x00474660 END
	// !DECL 0x00474690 BEGIN
	/* 474690 */ uchar Unwind_00474690();
	// !DECL 0x00474690 END
	// !DECL 0x004746c0 BEGIN
	/* 4746C0 */ uchar Unwind_004746c0();
	// !DECL 0x004746c0 END
	// !DECL 0x004746c8 BEGIN
	/* 4746C8 */ uchar Unwind_004746c8();
	// !DECL 0x004746c8 END
	// !DECL 0x004746f0 BEGIN
	/* 4746F0 */ uchar Unwind_004746f0();
	// !DECL 0x004746f0 END
	// !DECL 0x00474720 BEGIN
	/* 474720 */ uchar Unwind_00474720();
	// !DECL 0x00474720 END
	// !DECL 0x00474728 BEGIN
	/* 474728 */ uchar Unwind_00474728();
	// !DECL 0x00474728 END
	// !DECL 0x0047473e BEGIN
	/* 47473E */ uchar Unwind_0047473e();
	// !DECL 0x0047473e END
	// !DECL 0x00474770 BEGIN
	/* 474770 */ uchar Unwind_00474770();
	// !DECL 0x00474770 END
	// !DECL 0x004747a0 BEGIN
	/* 4747A0 */ uchar Unwind_004747a0();
	// !DECL 0x004747a0 END
	// !DECL 0x004747a8 BEGIN
	/* 4747A8 */ uchar Unwind_004747a8();
	// !DECL 0x004747a8 END
	// !DECL 0x004747cf BEGIN
	/* 4747CF */ uchar Unwind_004747cf();
	// !DECL 0x004747cf END
	// !DECL 0x00474800 BEGIN
	/* 474800 */ uchar Unwind_00474800();
	// !DECL 0x00474800 END
	// !DECL 0x00474830 BEGIN
	/* 474830 */ uchar Unwind_00474830();
	// !DECL 0x00474830 END
	// !DECL 0x00474860 BEGIN
	/* 474860 */ uchar Unwind_00474860();
	// !DECL 0x00474860 END
	// !DECL 0x00474890 BEGIN
	/* 474890 */ uchar Unwind_00474890();
	// !DECL 0x00474890 END
	// !DECL 0x004748c0 BEGIN
	/* 4748C0 */ uchar Unwind_004748c0();
	// !DECL 0x004748c0 END
	// !DECL 0x004748f0 BEGIN
	/* 4748F0 */ uchar Unwind_004748f0();
	// !DECL 0x004748f0 END
	// !DECL 0x00474920 BEGIN
	/* 474920 */ uchar Unwind_00474920();
	// !DECL 0x00474920 END
	// !DECL 0x00474950 BEGIN
	/* 474950 */ uchar Unwind_00474950();
	// !DECL 0x00474950 END
	// !DECL 0x00474958 BEGIN
	/* 474958 */ uchar Unwind_00474958();
	// !DECL 0x00474958 END
	// !DECL 0x00474960 BEGIN
	/* 474960 */ uchar Unwind_00474960();
	// !DECL 0x00474960 END
	// !DECL 0x0047496b BEGIN
	/* 47496B */ uchar Unwind_0047496b();
	// !DECL 0x0047496b END
	// !DECL 0x004749a0 BEGIN
	/* 4749A0 */ uchar Unwind_004749a0();
	// !DECL 0x004749a0 END
	// !DECL 0x004749a8 BEGIN
	/* 4749A8 */ uchar Unwind_004749a8();
	// !DECL 0x004749a8 END
	// !DECL 0x004749b0 BEGIN
	/* 4749B0 */ uchar Unwind_004749b0();
	// !DECL 0x004749b0 END
	// !DECL 0x004749bb BEGIN
	/* 4749BB */ uchar Unwind_004749bb();
	// !DECL 0x004749bb END
	// !DECL 0x004749f0 BEGIN
	/* 4749F0 */ uchar Unwind_004749f0();
	// !DECL 0x004749f0 END
	// !DECL 0x00474a30 BEGIN
	/* 474A30 */ uchar Unwind_00474a30();
	// !DECL 0x00474a30 END
	// !DECL 0x00474a60 BEGIN
	/* 474A60 */ uchar Unwind_00474a60();
	// !DECL 0x00474a60 END
	// !DECL 0x00474a90 BEGIN
	/* 474A90 */ uchar Unwind_00474a90();
	// !DECL 0x00474a90 END
	// !DECL 0x00474a98 BEGIN
	/* 474A98 */ uchar Unwind_00474a98();
	// !DECL 0x00474a98 END
	// !DECL 0x00474aa3 BEGIN
	/* 474AA3 */ uchar Unwind_00474aa3();
	// !DECL 0x00474aa3 END
	// !DECL 0x00474ad0 BEGIN
	/* 474AD0 */ uchar Unwind_00474ad0();
	// !DECL 0x00474ad0 END
	// !DECL 0x00474b00 BEGIN
	/* 474B00 */ uchar Unwind_00474b00();
	// !DECL 0x00474b00 END
	// !DECL 0x00474b08 BEGIN
	/* 474B08 */ uchar Unwind_00474b08();
	// !DECL 0x00474b08 END
	// !DECL 0x00474b13 BEGIN
	/* 474B13 */ uchar Unwind_00474b13();
	// !DECL 0x00474b13 END
	// !DECL 0x00474b1e BEGIN
	/* 474B1E */ uchar Unwind_00474b1e();
	// !DECL 0x00474b1e END
	// !DECL 0x00474b60 BEGIN
	/* 474B60 */ uchar Unwind_00474b60();
	// !DECL 0x00474b60 END
	// !DECL 0x00474b90 BEGIN
	/* 474B90 */ uchar Unwind_00474b90();
	// !DECL 0x00474b90 END
	// !DECL 0x00474b98 BEGIN
	/* 474B98 */ uchar Unwind_00474b98();
	// !DECL 0x00474b98 END
	// !DECL 0x00474ba3 BEGIN
	/* 474BA3 */ uchar Unwind_00474ba3();
	// !DECL 0x00474ba3 END
	// !DECL 0x00474bb1 BEGIN
	/* 474BB1 */ uchar Unwind_00474bb1();
	// !DECL 0x00474bb1 END
	// !DECL 0x00474be0 BEGIN
	/* 474BE0 */ uchar Unwind_00474be0();
	// !DECL 0x00474be0 END
	// !DECL 0x00474be8 BEGIN
	/* 474BE8 */ uchar Unwind_00474be8();
	// !DECL 0x00474be8 END
	// !DECL 0x00474c10 BEGIN
	/* 474C10 */ uchar Unwind_00474c10();
	// !DECL 0x00474c10 END
	// !DECL 0x00474c18 BEGIN
	/* 474C18 */ uchar Unwind_00474c18();
	// !DECL 0x00474c18 END
	// !DECL 0x00474c40 BEGIN
	/* 474C40 */ uchar Unwind_00474c40();
	// !DECL 0x00474c40 END
	// !DECL 0x00474c70 BEGIN
	/* 474C70 */ uchar Unwind_00474c70();
	// !DECL 0x00474c70 END
	// !DECL 0x00474c78 BEGIN
	/* 474C78 */ uchar Unwind_00474c78();
	// !DECL 0x00474c78 END
	// !DECL 0x00474c86 BEGIN
	/* 474C86 */ uchar Unwind_00474c86();
	// !DECL 0x00474c86 END
	// !DECL 0x00474cb0 BEGIN
	/* 474CB0 */ uchar Unwind_00474cb0();
	// !DECL 0x00474cb0 END
	// !DECL 0x00474ce0 BEGIN
	/* 474CE0 */ uchar Unwind_00474ce0();
	// !DECL 0x00474ce0 END
	// !DECL 0x00474d10 BEGIN
	/* 474D10 */ uchar Unwind_00474d10();
	// !DECL 0x00474d10 END
	// !DECL 0x00474d18 BEGIN
	/* 474D18 */ uchar Unwind_00474d18();
	// !DECL 0x00474d18 END
	// !DECL 0x00474d23 BEGIN
	/* 474D23 */ uchar Unwind_00474d23();
	// !DECL 0x00474d23 END
	// !DECL 0x00474d60 BEGIN
	/* 474D60 */ uchar Unwind_00474d60();
	// !DECL 0x00474d60 END
	// !DECL 0x00474d68 BEGIN
	/* 474D68 */ uchar Unwind_00474d68();
	// !DECL 0x00474d68 END
	// !DECL 0x00474d73 BEGIN
	/* 474D73 */ uchar Unwind_00474d73();
	// !DECL 0x00474d73 END
	// !DECL 0x00474d7e BEGIN
	/* 474D7E */ uchar Unwind_00474d7e();
	// !DECL 0x00474d7e END
	// !DECL 0x00474dc0 BEGIN
	/* 474DC0 */ uchar Unwind_00474dc0();
	// !DECL 0x00474dc0 END
	// !DECL 0x00474dc8 BEGIN
	/* 474DC8 */ uchar Unwind_00474dc8();
	// !DECL 0x00474dc8 END
	// !DECL 0x00474e00 BEGIN
	/* 474E00 */ uchar Unwind_00474e00();
	// !DECL 0x00474e00 END
	// !DECL 0x00474e08 BEGIN
	/* 474E08 */ uchar Unwind_00474e08();
	// !DECL 0x00474e08 END
	// !DECL 0x00474e10 BEGIN
	/* 474E10 */ uchar Unwind_00474e10();
	// !DECL 0x00474e10 END
	// !DECL 0x00474e26 BEGIN
	/* 474E26 */ uchar Unwind_00474e26();
	// !DECL 0x00474e26 END
	// !DECL 0x00474e31 BEGIN
	/* 474E31 */ uchar Unwind_00474e31();
	// !DECL 0x00474e31 END
	// !DECL 0x00474e60 BEGIN
	/* 474E60 */ uchar Unwind_00474e60();
	// !DECL 0x00474e60 END
	// !DECL 0x00474e68 BEGIN
	/* 474E68 */ uchar Unwind_00474e68();
	// !DECL 0x00474e68 END
	// !DECL 0x00474e73 BEGIN
	/* 474E73 */ uchar Unwind_00474e73();
	// !DECL 0x00474e73 END
	// !DECL 0x00474e7e BEGIN
	/* 474E7E */ uchar Unwind_00474e7e();
	// !DECL 0x00474e7e END
	// !DECL 0x00474eb0 BEGIN
	/* 474EB0 */ uchar Unwind_00474eb0();
	// !DECL 0x00474eb0 END
	// !DECL 0x00474eb8 BEGIN
	/* 474EB8 */ uchar Unwind_00474eb8();
	// !DECL 0x00474eb8 END
	// !DECL 0x00474ef0 BEGIN
	/* 474EF0 */ uchar Unwind_00474ef0();
	// !DECL 0x00474ef0 END
	// !DECL 0x00474f20 BEGIN
	/* 474F20 */ uchar Unwind_00474f20();
	// !DECL 0x00474f20 END
	// !DECL 0x00474f50 BEGIN
	/* 474F50 */ uchar Unwind_00474f50();
	// !DECL 0x00474f50 END
	// !DECL 0x00474f80 BEGIN
	/* 474F80 */ uchar Unwind_00474f80();
	// !DECL 0x00474f80 END
	// !DECL 0x00474f88 BEGIN
	/* 474F88 */ uchar Unwind_00474f88();
	// !DECL 0x00474f88 END
	// !DECL 0x00474fb0 BEGIN
	/* 474FB0 */ uchar Unwind_00474fb0();
	// !DECL 0x00474fb0 END
	// !DECL 0x00474fb8 BEGIN
	/* 474FB8 */ uchar Unwind_00474fb8();
	// !DECL 0x00474fb8 END
	// !DECL 0x00474fe0 BEGIN
	/* 474FE0 */ uchar Unwind_00474fe0();
	// !DECL 0x00474fe0 END
	// !DECL 0x00474fe8 BEGIN
	/* 474FE8 */ uchar Unwind_00474fe8();
	// !DECL 0x00474fe8 END
	// !DECL 0x00475010 BEGIN
	/* 475010 */ uchar Unwind_00475010();
	// !DECL 0x00475010 END
	// !DECL 0x00475040 BEGIN
	/* 475040 */ uchar Unwind_00475040();
	// !DECL 0x00475040 END
	// !DECL 0x00475048 BEGIN
	/* 475048 */ uchar Unwind_00475048();
	// !DECL 0x00475048 END
	// !DECL 0x00475090 BEGIN
	/* 475090 */ uchar Unwind_00475090();
	// !DECL 0x00475090 END
	// !DECL 0x00475098 BEGIN
	/* 475098 */ uchar Unwind_00475098();
	// !DECL 0x00475098 END
	// !DECL 0x004750c0 BEGIN
	/* 4750C0 */ uchar Unwind_004750c0();
	// !DECL 0x004750c0 END
	// !DECL 0x004750f0 BEGIN
	/* 4750F0 */ uchar Unwind_004750f0();
	// !DECL 0x004750f0 END
	// !DECL 0x00475120 BEGIN
	/* 475120 */ uchar Unwind_00475120();
	// !DECL 0x00475120 END
	// !DECL 0x0047512b BEGIN
	/* 47512B */ uchar Unwind_0047512b();
	// !DECL 0x0047512b END
	// !DECL 0x00475160 BEGIN
	/* 475160 */ uchar Unwind_00475160();
	// !DECL 0x00475160 END
	// !DECL 0x00475168 BEGIN
	/* 475168 */ uchar Unwind_00475168();
	// !DECL 0x00475168 END
	// !DECL 0x00475173 BEGIN
	/* 475173 */ uchar Unwind_00475173();
	// !DECL 0x00475173 END
	// !DECL 0x0047517e BEGIN
	/* 47517E */ uchar Unwind_0047517e();
	// !DECL 0x0047517e END
	// !DECL 0x00475189 BEGIN
	/* 475189 */ uchar Unwind_00475189();
	// !DECL 0x00475189 END
	// !DECL 0x004751b0 BEGIN
	/* 4751B0 */ uchar Unwind_004751b0();
	// !DECL 0x004751b0 END
	// !DECL 0x004751e0 BEGIN
	/* 4751E0 */ uchar Unwind_004751e0();
	// !DECL 0x004751e0 END
	// !DECL 0x004751e8 BEGIN
	/* 4751E8 */ uchar Unwind_004751e8();
	// !DECL 0x004751e8 END
	// !DECL 0x004751f0 BEGIN
	/* 4751F0 */ uchar Unwind_004751f0();
	// !DECL 0x004751f0 END
	// !DECL 0x004751f8 BEGIN
	/* 4751F8 */ uchar Unwind_004751f8();
	// !DECL 0x004751f8 END
	// !DECL 0x00475200 BEGIN
	/* 475200 */ uchar Unwind_00475200();
	// !DECL 0x00475200 END
	// !DECL 0x00475208 BEGIN
	/* 475208 */ uchar Unwind_00475208();
	// !DECL 0x00475208 END
	// !DECL 0x00475230 BEGIN
	/* 475230 */ uchar Unwind_00475230();
	// !DECL 0x00475230 END
	// !DECL 0x00475260 BEGIN
	/* 475260 */ uchar Unwind_00475260();
	// !DECL 0x00475260 END
	// !DECL 0x0047526b BEGIN
	/* 47526B */ uchar Unwind_0047526b();
	// !DECL 0x0047526b END
	// !DECL 0x00475276 BEGIN
	/* 475276 */ uchar Unwind_00475276();
	// !DECL 0x00475276 END
	// !DECL 0x004752b0 BEGIN
	/* 4752B0 */ uchar Unwind_004752b0();
	// !DECL 0x004752b0 END
	// !DECL 0x004752c6 BEGIN
	/* 4752C6 */ uchar Unwind_004752c6();
	// !DECL 0x004752c6 END
	// !DECL 0x004752d1 BEGIN
	/* 4752D1 */ uchar Unwind_004752d1();
	// !DECL 0x004752d1 END
	// !DECL 0x004752dc BEGIN
	/* 4752DC */ uchar Unwind_004752dc();
	// !DECL 0x004752dc END
	// !DECL 0x004752e7 BEGIN
	/* 4752E7 */ uchar Unwind_004752e7();
	// !DECL 0x004752e7 END
	// !DECL 0x00475310 BEGIN
	/* 475310 */ uchar Unwind_00475310();
	// !DECL 0x00475310 END
	// !DECL 0x00475318 BEGIN
	/* 475318 */ uchar Unwind_00475318();
	// !DECL 0x00475318 END
	// !DECL 0x00475320 BEGIN
	/* 475320 */ uchar Unwind_00475320();
	// !DECL 0x00475320 END
	// !DECL 0x00475328 BEGIN
	/* 475328 */ uchar Unwind_00475328();
	// !DECL 0x00475328 END
	// !DECL 0x00475350 BEGIN
	/* 475350 */ uchar Unwind_00475350();
	// !DECL 0x00475350 END
	// !DECL 0x00475366 BEGIN
	/* 475366 */ uchar Unwind_00475366();
	// !DECL 0x00475366 END
	// !DECL 0x00475371 BEGIN
	/* 475371 */ uchar Unwind_00475371();
	// !DECL 0x00475371 END
	// !DECL 0x0047537c BEGIN
	/* 47537C */ uchar Unwind_0047537c();
	// !DECL 0x0047537c END
	// !DECL 0x00475387 BEGIN
	/* 475387 */ uchar Unwind_00475387();
	// !DECL 0x00475387 END
	// !DECL 0x004753b0 BEGIN
	/* 4753B0 */ uchar Unwind_004753b0();
	// !DECL 0x004753b0 END
	// !DECL 0x004753e0 BEGIN
	/* 4753E0 */ uchar Unwind_004753e0();
	// !DECL 0x004753e0 END
	// !DECL 0x00475410 BEGIN
	/* 475410 */ uchar Unwind_00475410();
	// !DECL 0x00475410 END
	// !DECL 0x00475418 BEGIN
	/* 475418 */ uchar Unwind_00475418();
	// !DECL 0x00475418 END
	// !DECL 0x00475423 BEGIN
	/* 475423 */ uchar Unwind_00475423();
	// !DECL 0x00475423 END
	// !DECL 0x00475450 BEGIN
	/* 475450 */ uchar Unwind_00475450();
	// !DECL 0x00475450 END
	// !DECL 0x00475458 BEGIN
	/* 475458 */ uchar Unwind_00475458();
	// !DECL 0x00475458 END
	// !DECL 0x00475480 BEGIN
	/* 475480 */ uchar Unwind_00475480();
	// !DECL 0x00475480 END
	// !DECL 0x00475488 BEGIN
	/* 475488 */ uchar Unwind_00475488();
	// !DECL 0x00475488 END
	// !DECL 0x004754b0 BEGIN
	/* 4754B0 */ uchar Unwind_004754b0();
	// !DECL 0x004754b0 END
	// !DECL 0x004754e0 BEGIN
	/* 4754E0 */ uchar Unwind_004754e0();
	// !DECL 0x004754e0 END
	// !DECL 0x00475510 BEGIN
	/* 475510 */ uchar Unwind_00475510();
	// !DECL 0x00475510 END
	// !DECL 0x00475540 BEGIN
	/* 475540 */ uchar Unwind_00475540();
	// !DECL 0x00475540 END
	// !DECL 0x00475570 BEGIN
	/* 475570 */ uchar Unwind_00475570();
	// !DECL 0x00475570 END
	// !DECL 0x004755a0 BEGIN
	/* 4755A0 */ uchar Unwind_004755a0();
	// !DECL 0x004755a0 END
	// !DECL 0x004755d0 BEGIN
	/* 4755D0 */ uchar Unwind_004755d0();
	// !DECL 0x004755d0 END
	// !DECL 0x00475600 BEGIN
	/* 475600 */ uchar Unwind_00475600();
	// !DECL 0x00475600 END
	// !DECL 0x00475630 BEGIN
	/* 475630 */ uchar Unwind_00475630();
	// !DECL 0x00475630 END
	// !DECL 0x00475660 BEGIN
	/* 475660 */ uchar Unwind_00475660();
	// !DECL 0x00475660 END
	// !DECL 0x00475668 BEGIN
	/* 475668 */ uchar Unwind_00475668();
	// !DECL 0x00475668 END
	// !DECL 0x00475670 BEGIN
	/* 475670 */ uchar Unwind_00475670();
	// !DECL 0x00475670 END
	// !DECL 0x0047567b BEGIN
	/* 47567B */ uchar Unwind_0047567b();
	// !DECL 0x0047567b END
	// !DECL 0x004756b0 BEGIN
	/* 4756B0 */ uchar Unwind_004756b0();
	// !DECL 0x004756b0 END
	// !DECL 0x004756b8 BEGIN
	/* 4756B8 */ uchar Unwind_004756b8();
	// !DECL 0x004756b8 END
	// !DECL 0x004756c3 BEGIN
	/* 4756C3 */ uchar Unwind_004756c3();
	// !DECL 0x004756c3 END
	// !DECL 0x004756ce BEGIN
	/* 4756CE */ uchar Unwind_004756ce();
	// !DECL 0x004756ce END
	// !DECL 0x004756d9 BEGIN
	/* 4756D9 */ uchar Unwind_004756d9();
	// !DECL 0x004756d9 END
	// !DECL 0x00475700 BEGIN
	/* 475700 */ uchar Unwind_00475700();
	// !DECL 0x00475700 END
	// !DECL 0x00475708 BEGIN
	/* 475708 */ uchar Unwind_00475708();
	// !DECL 0x00475708 END
	// !DECL 0x00475713 BEGIN
	/* 475713 */ uchar Unwind_00475713();
	// !DECL 0x00475713 END
	// !DECL 0x0047571e BEGIN
	/* 47571E */ uchar Unwind_0047571e();
	// !DECL 0x0047571e END
	// !DECL 0x00475729 BEGIN
	/* 475729 */ uchar Unwind_00475729();
	// !DECL 0x00475729 END
	// !DECL 0x00475750 BEGIN
	/* 475750 */ uchar Unwind_00475750();
	// !DECL 0x00475750 END
	// !DECL 0x00475780 BEGIN
	/* 475780 */ uchar Unwind_00475780();
	// !DECL 0x00475780 END
	// !DECL 0x0047578b BEGIN
	/* 47578B */ uchar Unwind_0047578b();
	// !DECL 0x0047578b END
	// !DECL 0x00475796 BEGIN
	/* 475796 */ uchar Unwind_00475796();
	// !DECL 0x00475796 END
	// !DECL 0x004757a1 BEGIN
	/* 4757A1 */ uchar Unwind_004757a1();
	// !DECL 0x004757a1 END
	// !DECL 0x004757ac BEGIN
	/* 4757AC */ uchar Unwind_004757ac();
	// !DECL 0x004757ac END
	// !DECL 0x004757b7 BEGIN
	/* 4757B7 */ uchar Unwind_004757b7();
	// !DECL 0x004757b7 END
	// !DECL 0x004757c2 BEGIN
	/* 4757C2 */ uchar Unwind_004757c2();
	// !DECL 0x004757c2 END
	// !DECL 0x004757cd BEGIN
	/* 4757CD */ uchar Unwind_004757cd();
	// !DECL 0x004757cd END
	// !DECL 0x004757d8 BEGIN
	/* 4757D8 */ uchar Unwind_004757d8();
	// !DECL 0x004757d8 END
	// !DECL 0x00475800 BEGIN
	/* 475800 */ uchar Unwind_00475800();
	// !DECL 0x00475800 END
	// !DECL 0x00475830 BEGIN
	/* 475830 */ uchar Unwind_00475830();
	// !DECL 0x00475830 END
	// !DECL 0x00475838 BEGIN
	/* 475838 */ uchar Unwind_00475838();
	// !DECL 0x00475838 END
	// !DECL 0x00475843 BEGIN
	/* 475843 */ uchar Unwind_00475843();
	// !DECL 0x00475843 END
	// !DECL 0x0047584e BEGIN
	/* 47584E */ uchar Unwind_0047584e();
	// !DECL 0x0047584e END
	// !DECL 0x00475859 BEGIN
	/* 475859 */ uchar Unwind_00475859();
	// !DECL 0x00475859 END
	// !DECL 0x00475864 BEGIN
	/* 475864 */ uchar Unwind_00475864();
	// !DECL 0x00475864 END
	// !DECL 0x0047586f BEGIN
	/* 47586F */ uchar Unwind_0047586f();
	// !DECL 0x0047586f END
	// !DECL 0x004758a0 BEGIN
	/* 4758A0 */ uchar Unwind_004758a0();
	// !DECL 0x004758a0 END
	// !DECL 0x004758d0 BEGIN
	/* 4758D0 */ uchar Unwind_004758d0();
	// !DECL 0x004758d0 END
	// !DECL 0x00475900 BEGIN
	/* 475900 */ uchar Unwind_00475900();
	// !DECL 0x00475900 END
	// !DECL 0x00475908 BEGIN
	/* 475908 */ uchar Unwind_00475908();
	// !DECL 0x00475908 END
	// !DECL 0x00475930 BEGIN
	/* 475930 */ uchar Unwind_00475930();
	// !DECL 0x00475930 END
	// !DECL 0x00475938 BEGIN
	/* 475938 */ uchar Unwind_00475938();
	// !DECL 0x00475938 END
	// !DECL 0x00475943 BEGIN
	/* 475943 */ uchar Unwind_00475943();
	// !DECL 0x00475943 END
	// !DECL 0x0047594e BEGIN
	/* 47594E */ uchar Unwind_0047594e();
	// !DECL 0x0047594e END
	// !DECL 0x00475959 BEGIN
	/* 475959 */ uchar Unwind_00475959();
	// !DECL 0x00475959 END
	// !DECL 0x00475964 BEGIN
	/* 475964 */ uchar Unwind_00475964();
	// !DECL 0x00475964 END
	// !DECL 0x0047596f BEGIN
	/* 47596F */ uchar Unwind_0047596f();
	// !DECL 0x0047596f END
	// !DECL 0x0047597a BEGIN
	/* 47597A */ uchar Unwind_0047597a();
	// !DECL 0x0047597a END
	// !DECL 0x00475985 BEGIN
	/* 475985 */ uchar Unwind_00475985();
	// !DECL 0x00475985 END
	// !DECL 0x00475990 BEGIN
	/* 475990 */ uchar Unwind_00475990();
	// !DECL 0x00475990 END
	// !DECL 0x0047599b BEGIN
	/* 47599B */ uchar Unwind_0047599b();
	// !DECL 0x0047599b END
	// !DECL 0x004759a6 BEGIN
	/* 4759A6 */ uchar Unwind_004759a6();
	// !DECL 0x004759a6 END
	// !DECL 0x004759b1 BEGIN
	/* 4759B1 */ uchar Unwind_004759b1();
	// !DECL 0x004759b1 END
	// !DECL 0x004759e0 BEGIN
	/* 4759E0 */ uchar Unwind_004759e0();
	// !DECL 0x004759e0 END
	// !DECL 0x004759e8 BEGIN
	/* 4759E8 */ uchar Unwind_004759e8();
	// !DECL 0x004759e8 END
	// !DECL 0x004759f3 BEGIN
	/* 4759F3 */ uchar Unwind_004759f3();
	// !DECL 0x004759f3 END
	// !DECL 0x00475a20 BEGIN
	/* 475A20 */ uchar Unwind_00475a20();
	// !DECL 0x00475a20 END
	// !DECL 0x00475a28 BEGIN
	/* 475A28 */ uchar Unwind_00475a28();
	// !DECL 0x00475a28 END
	// !DECL 0x00475a50 BEGIN
	/* 475A50 */ uchar Unwind_00475a50();
	// !DECL 0x00475a50 END
	// !DECL 0x00475a58 BEGIN
	/* 475A58 */ uchar Unwind_00475a58();
	// !DECL 0x00475a58 END
	// !DECL 0x00475a63 BEGIN
	/* 475A63 */ uchar Unwind_00475a63();
	// !DECL 0x00475a63 END
	// !DECL 0x00475a6e BEGIN
	/* 475A6E */ uchar Unwind_00475a6e();
	// !DECL 0x00475a6e END
	// !DECL 0x00475a79 BEGIN
	/* 475A79 */ uchar Unwind_00475a79();
	// !DECL 0x00475a79 END
	// !DECL 0x00475aa0 BEGIN
	/* 475AA0 */ uchar Unwind_00475aa0();
	// !DECL 0x00475aa0 END
	// !DECL 0x00475ad0 BEGIN
	/* 475AD0 */ uchar Unwind_00475ad0();
	// !DECL 0x00475ad0 END
	// !DECL 0x00475ad8 BEGIN
	/* 475AD8 */ uchar Unwind_00475ad8();
	// !DECL 0x00475ad8 END
	// !DECL 0x00475ae3 BEGIN
	/* 475AE3 */ uchar Unwind_00475ae3();
	// !DECL 0x00475ae3 END
	// !DECL 0x00475b10 BEGIN
	/* 475B10 */ uchar Unwind_00475b10();
	// !DECL 0x00475b10 END
	// !DECL 0x00475b40 BEGIN
	/* 475B40 */ uchar Unwind_00475b40();
	// !DECL 0x00475b40 END
	// !DECL 0x00475b70 BEGIN
	/* 475B70 */ uchar Unwind_00475b70();
	// !DECL 0x00475b70 END
	// !DECL 0x00475ba0 BEGIN
	/* 475BA0 */ uchar Unwind_00475ba0();
	// !DECL 0x00475ba0 END
	// !DECL 0x00475bd0 BEGIN
	/* 475BD0 */ uchar Unwind_00475bd0();
	// !DECL 0x00475bd0 END
	// !DECL 0x00475c00 BEGIN
	/* 475C00 */ uchar Unwind_00475c00();
	// !DECL 0x00475c00 END
	// !DECL 0x00475c30 BEGIN
	/* 475C30 */ uchar Unwind_00475c30();
	// !DECL 0x00475c30 END
	// !DECL 0x00475c60 BEGIN
	/* 475C60 */ uchar Unwind_00475c60();
	// !DECL 0x00475c60 END
	// !DECL 0x00475c90 BEGIN
	/* 475C90 */ uchar Unwind_00475c90();
	// !DECL 0x00475c90 END
	// !DECL 0x00475cc0 BEGIN
	/* 475CC0 */ uchar Unwind_00475cc0();
	// !DECL 0x00475cc0 END
	// !DECL 0x00475cf0 BEGIN
	/* 475CF0 */ uchar Unwind_00475cf0();
	// !DECL 0x00475cf0 END
	// !DECL 0x00475d20 BEGIN
	/* 475D20 */ uchar Unwind_00475d20();
	// !DECL 0x00475d20 END
	// !DECL 0x00475d50 BEGIN
	/* 475D50 */ uchar Unwind_00475d50();
	// !DECL 0x00475d50 END
	// !DECL 0x00475d80 BEGIN
	/* 475D80 */ uchar Unwind_00475d80();
	// !DECL 0x00475d80 END
	// !DECL 0x00475db0 BEGIN
	/* 475DB0 */ uchar Unwind_00475db0();
	// !DECL 0x00475db0 END
	// !DECL 0x00475de0 BEGIN
	/* 475DE0 */ uchar Unwind_00475de0();
	// !DECL 0x00475de0 END
	// !DECL 0x00475e10 BEGIN
	/* 475E10 */ uchar Unwind_00475e10();
	// !DECL 0x00475e10 END
	// !DECL 0x00475e40 BEGIN
	/* 475E40 */ uchar Unwind_00475e40();
	// !DECL 0x00475e40 END
	// !DECL 0x00475e70 BEGIN
	/* 475E70 */ uchar Unwind_00475e70();
	// !DECL 0x00475e70 END
	// !DECL 0x00475ea0 BEGIN
	/* 475EA0 */ uchar Unwind_00475ea0();
	// !DECL 0x00475ea0 END
	// !DECL 0x00475ea8 BEGIN
	/* 475EA8 */ uchar Unwind_00475ea8();
	// !DECL 0x00475ea8 END
	// !DECL 0x00475ed0 BEGIN
	/* 475ED0 */ uchar Unwind_00475ed0();
	// !DECL 0x00475ed0 END
	// !DECL 0x00475f00 BEGIN
	/* 475F00 */ uchar Unwind_00475f00();
	// !DECL 0x00475f00 END
	// !DECL 0x00475f08 BEGIN
	/* 475F08 */ uchar Unwind_00475f08();
	// !DECL 0x00475f08 END
	// !DECL 0x00475f30 BEGIN
	/* 475F30 */ uchar Unwind_00475f30();
	// !DECL 0x00475f30 END
	// !DECL 0x00475f60 BEGIN
	/* 475F60 */ uchar Unwind_00475f60();
	// !DECL 0x00475f60 END
	// !DECL 0x00475f68 BEGIN
	/* 475F68 */ uchar Unwind_00475f68();
	// !DECL 0x00475f68 END
	// !DECL 0x00475f90 BEGIN
	/* 475F90 */ uchar Unwind_00475f90();
	// !DECL 0x00475f90 END
	// !DECL 0x00475f98 BEGIN
	/* 475F98 */ uchar Unwind_00475f98();
	// !DECL 0x00475f98 END
	// !DECL 0x00475fa3 BEGIN
	/* 475FA3 */ uchar Unwind_00475fa3();
	// !DECL 0x00475fa3 END
	// !DECL 0x00475fab BEGIN
	/* 475FAB */ uchar Unwind_00475fab();
	// !DECL 0x00475fab END
	// !DECL 0x00475fb3 BEGIN
	/* 475FB3 */ uchar Unwind_00475fb3();
	// !DECL 0x00475fb3 END
	// !DECL 0x00475fbe BEGIN
	/* 475FBE */ uchar Unwind_00475fbe();
	// !DECL 0x00475fbe END
	// !DECL 0x00475ff0 BEGIN
	/* 475FF0 */ uchar Unwind_00475ff0();
	// !DECL 0x00475ff0 END
	// !DECL 0x00475ff8 BEGIN
	/* 475FF8 */ uchar Unwind_00475ff8();
	// !DECL 0x00475ff8 END
	// !DECL 0x00476030 BEGIN
	/* 476030 */ uchar Unwind_00476030();
	// !DECL 0x00476030 END
	// !DECL 0x00476060 BEGIN
	/* 476060 */ uchar Unwind_00476060();
	// !DECL 0x00476060 END
	// !DECL 0x00476068 BEGIN
	/* 476068 */ uchar Unwind_00476068();
	// !DECL 0x00476068 END
	// !DECL 0x004760a0 BEGIN
	/* 4760A0 */ uchar Unwind_004760a0();
	// !DECL 0x004760a0 END
	// !DECL 0x004760a8 BEGIN
	/* 4760A8 */ uchar Unwind_004760a8();
	// !DECL 0x004760a8 END
	// !DECL 0x004760d0 BEGIN
	/* 4760D0 */ uchar Unwind_004760d0();
	// !DECL 0x004760d0 END
	// !DECL 0x00476100 BEGIN
	/* 476100 */ uchar Unwind_00476100();
	// !DECL 0x00476100 END
	// !DECL 0x00476108 BEGIN
	/* 476108 */ uchar Unwind_00476108();
	// !DECL 0x00476108 END
	// !DECL 0x00476130 BEGIN
	/* 476130 */ uchar Unwind_00476130();
	// !DECL 0x00476130 END
	// !DECL 0x00476138 BEGIN
	/* 476138 */ uchar Unwind_00476138();
	// !DECL 0x00476138 END
	// !DECL 0x00476160 BEGIN
	/* 476160 */ uchar Unwind_00476160();
	// !DECL 0x00476160 END
	// !DECL 0x00476168 BEGIN
	/* 476168 */ uchar Unwind_00476168();
	// !DECL 0x00476168 END
	// !DECL 0x00476190 BEGIN
	/* 476190 */ uchar Unwind_00476190();
	// !DECL 0x00476190 END
	// !DECL 0x0047619b BEGIN
	/* 47619B */ uchar Unwind_0047619b();
	// !DECL 0x0047619b END
	// !DECL 0x004761a6 BEGIN
	/* 4761A6 */ uchar Unwind_004761a6();
	// !DECL 0x004761a6 END
	// !DECL 0x004761b1 BEGIN
	/* 4761B1 */ uchar Unwind_004761b1();
	// !DECL 0x004761b1 END
	// !DECL 0x004761e0 BEGIN
	/* 4761E0 */ uchar Unwind_004761e0();
	// !DECL 0x004761e0 END
	// !DECL 0x004761eb BEGIN
	/* 4761EB */ uchar Unwind_004761eb();
	// !DECL 0x004761eb END
	// !DECL 0x004761f9 BEGIN
	/* 4761F9 */ uchar Unwind_004761f9();
	// !DECL 0x004761f9 END
	// !DECL 0x00476207 BEGIN
	/* 476207 */ uchar Unwind_00476207();
	// !DECL 0x00476207 END
	// !DECL 0x00476212 BEGIN
	/* 476212 */ uchar Unwind_00476212();
	// !DECL 0x00476212 END
	// !DECL 0x00476220 BEGIN
	/* 476220 */ uchar Unwind_00476220();
	// !DECL 0x00476220 END
	// !DECL 0x0047622e BEGIN
	/* 47622E */ uchar Unwind_0047622e();
	// !DECL 0x0047622e END
	// !DECL 0x00476239 BEGIN
	/* 476239 */ uchar Unwind_00476239();
	// !DECL 0x00476239 END
	// !DECL 0x00476247 BEGIN
	/* 476247 */ uchar Unwind_00476247();
	// !DECL 0x00476247 END
	// !DECL 0x00476255 BEGIN
	/* 476255 */ uchar Unwind_00476255();
	// !DECL 0x00476255 END
	// !DECL 0x00476263 BEGIN
	/* 476263 */ uchar Unwind_00476263();
	// !DECL 0x00476263 END
	// !DECL 0x00476271 BEGIN
	/* 476271 */ uchar Unwind_00476271();
	// !DECL 0x00476271 END
	// !DECL 0x0047627f BEGIN
	/* 47627F */ uchar Unwind_0047627f();
	// !DECL 0x0047627f END
	// !DECL 0x0047628d BEGIN
	/* 47628D */ uchar Unwind_0047628d();
	// !DECL 0x0047628d END
	// !DECL 0x0047629b BEGIN
	/* 47629B */ uchar Unwind_0047629b();
	// !DECL 0x0047629b END
	// !DECL 0x004762a9 BEGIN
	/* 4762A9 */ uchar Unwind_004762a9();
	// !DECL 0x004762a9 END
	// !DECL 0x004762b7 BEGIN
	/* 4762B7 */ uchar Unwind_004762b7();
	// !DECL 0x004762b7 END
	// !DECL 0x004762f0 BEGIN
	/* 4762F0 */ uchar Unwind_004762f0();
	// !DECL 0x004762f0 END
	// !DECL 0x004762f8 BEGIN
	/* 4762F8 */ uchar Unwind_004762f8();
	// !DECL 0x004762f8 END
	// !DECL 0x0047630e BEGIN
	/* 47630E */ uchar Unwind_0047630e();
	// !DECL 0x0047630e END
	// !DECL 0x00476316 BEGIN
	/* 476316 */ uchar Unwind_00476316();
	// !DECL 0x00476316 END
	// !DECL 0x00476321 BEGIN
	/* 476321 */ uchar Unwind_00476321();
	// !DECL 0x00476321 END
	// !DECL 0x00476350 BEGIN
	/* 476350 */ uchar Unwind_00476350();
	// !DECL 0x00476350 END
	// !DECL 0x0047635b BEGIN
	/* 47635B */ uchar Unwind_0047635b();
	// !DECL 0x0047635b END
	// !DECL 0x00476369 BEGIN
	/* 476369 */ uchar Unwind_00476369();
	// !DECL 0x00476369 END
	// !DECL 0x00476374 BEGIN
	/* 476374 */ uchar Unwind_00476374();
	// !DECL 0x00476374 END
	// !DECL 0x00476382 BEGIN
	/* 476382 */ uchar Unwind_00476382();
	// !DECL 0x00476382 END
	// !DECL 0x00476390 BEGIN
	/* 476390 */ uchar Unwind_00476390();
	// !DECL 0x00476390 END
	// !DECL 0x0047639e BEGIN
	/* 47639E */ uchar Unwind_0047639e();
	// !DECL 0x0047639e END
	// !DECL 0x004763a9 BEGIN
	/* 4763A9 */ uchar Unwind_004763a9();
	// !DECL 0x004763a9 END
	// !DECL 0x004763b7 BEGIN
	/* 4763B7 */ uchar Unwind_004763b7();
	// !DECL 0x004763b7 END
	// !DECL 0x004763c2 BEGIN
	/* 4763C2 */ uchar Unwind_004763c2();
	// !DECL 0x004763c2 END
	// !DECL 0x004763d0 BEGIN
	/* 4763D0 */ uchar Unwind_004763d0();
	// !DECL 0x004763d0 END
	// !DECL 0x004763de BEGIN
	/* 4763DE */ uchar Unwind_004763de();
	// !DECL 0x004763de END
	// !DECL 0x004763e9 BEGIN
	/* 4763E9 */ uchar Unwind_004763e9();
	// !DECL 0x004763e9 END
	// !DECL 0x004763f7 BEGIN
	/* 4763F7 */ uchar Unwind_004763f7();
	// !DECL 0x004763f7 END
	// !DECL 0x00476405 BEGIN
	/* 476405 */ uchar Unwind_00476405();
	// !DECL 0x00476405 END
	// !DECL 0x00476410 BEGIN
	/* 476410 */ uchar Unwind_00476410();
	// !DECL 0x00476410 END
	// !DECL 0x0047641e BEGIN
	/* 47641E */ uchar Unwind_0047641e();
	// !DECL 0x0047641e END
	// !DECL 0x0047642c BEGIN
	/* 47642C */ uchar Unwind_0047642c();
	// !DECL 0x0047642c END
	// !DECL 0x00476437 BEGIN
	/* 476437 */ uchar Unwind_00476437();
	// !DECL 0x00476437 END
	// !DECL 0x00476470 BEGIN
	/* 476470 */ uchar Unwind_00476470();
	// !DECL 0x00476470 END
	// !DECL 0x00476478 BEGIN
	/* 476478 */ uchar Unwind_00476478();
	// !DECL 0x00476478 END
	// !DECL 0x00476480 BEGIN
	/* 476480 */ uchar Unwind_00476480();
	// !DECL 0x00476480 END
	// !DECL 0x0047648b BEGIN
	/* 47648B */ uchar Unwind_0047648b();
	// !DECL 0x0047648b END
	// !DECL 0x00476496 BEGIN
	/* 476496 */ uchar Unwind_00476496();
	// !DECL 0x00476496 END
	// !DECL 0x004764a1 BEGIN
	/* 4764A1 */ uchar Unwind_004764a1();
	// !DECL 0x004764a1 END
	// !DECL 0x004764d0 BEGIN
	/* 4764D0 */ uchar Unwind_004764d0();
	// !DECL 0x004764d0 END
	// !DECL 0x004764d8 BEGIN
	/* 4764D8 */ uchar Unwind_004764d8();
	// !DECL 0x004764d8 END
	// !DECL 0x004764e3 BEGIN
	/* 4764E3 */ uchar Unwind_004764e3();
	// !DECL 0x004764e3 END
	// !DECL 0x004764ee BEGIN
	/* 4764EE */ uchar Unwind_004764ee();
	// !DECL 0x004764ee END
	// !DECL 0x004764f9 BEGIN
	/* 4764F9 */ uchar Unwind_004764f9();
	// !DECL 0x004764f9 END
	// !DECL 0x00476504 BEGIN
	/* 476504 */ uchar Unwind_00476504();
	// !DECL 0x00476504 END
	// !DECL 0x00476530 BEGIN
	/* 476530 */ uchar Unwind_00476530();
	// !DECL 0x00476530 END
	// !DECL 0x00476560 BEGIN
	/* 476560 */ uchar Unwind_00476560();
	// !DECL 0x00476560 END
	// !DECL 0x00476590 BEGIN
	/* 476590 */ uchar Unwind_00476590();
	// !DECL 0x00476590 END
	// !DECL 0x004765c0 BEGIN
	/* 4765C0 */ uchar Unwind_004765c0();
	// !DECL 0x004765c0 END
	// !DECL 0x004765f0 BEGIN
	/* 4765F0 */ uchar Unwind_004765f0();
	// !DECL 0x004765f0 END
	// !DECL 0x00476620 BEGIN
	/* 476620 */ uchar Unwind_00476620();
	// !DECL 0x00476620 END
	// !DECL 0x00476650 BEGIN
	/* 476650 */ uchar Unwind_00476650();
	// !DECL 0x00476650 END
	// !DECL 0x00476680 BEGIN
	/* 476680 */ uchar Unwind_00476680();
	// !DECL 0x00476680 END
	// !DECL 0x004766b0 BEGIN
	/* 4766B0 */ uchar Unwind_004766b0();
	// !DECL 0x004766b0 END
	// !DECL 0x004766bb BEGIN
	/* 4766BB */ uchar Unwind_004766bb();
	// !DECL 0x004766bb END
	// !DECL 0x004766c6 BEGIN
	/* 4766C6 */ uchar Unwind_004766c6();
	// !DECL 0x004766c6 END
	// !DECL 0x004766d1 BEGIN
	/* 4766D1 */ uchar Unwind_004766d1();
	// !DECL 0x004766d1 END
	// !DECL 0x00476700 BEGIN
	/* 476700 */ uchar Unwind_00476700();
	// !DECL 0x00476700 END
	// !DECL 0x00476730 BEGIN
	/* 476730 */ uchar Unwind_00476730();
	// !DECL 0x00476730 END
	// !DECL 0x00476760 BEGIN
	/* 476760 */ uchar Unwind_00476760();
	// !DECL 0x00476760 END
	// !DECL 0x00476790 BEGIN
	/* 476790 */ uchar Unwind_00476790();
	// !DECL 0x00476790 END
	// !DECL 0x004767c0 BEGIN
	/* 4767C0 */ uchar Unwind_004767c0();
	// !DECL 0x004767c0 END
	// !DECL 0x004767f0 BEGIN
	/* 4767F0 */ uchar Unwind_004767f0();
	// !DECL 0x004767f0 END
	// !DECL 0x004767f8 BEGIN
	/* 4767F8 */ uchar Unwind_004767f8();
	// !DECL 0x004767f8 END
	// !DECL 0x00476803 BEGIN
	/* 476803 */ uchar Unwind_00476803();
	// !DECL 0x00476803 END
	// !DECL 0x0047680e BEGIN
	/* 47680E */ uchar Unwind_0047680e();
	// !DECL 0x0047680e END
	// !DECL 0x00476819 BEGIN
	/* 476819 */ uchar Unwind_00476819();
	// !DECL 0x00476819 END
	// !DECL 0x00476827 BEGIN
	/* 476827 */ uchar Unwind_00476827();
	// !DECL 0x00476827 END
	// !DECL 0x00476835 BEGIN
	/* 476835 */ uchar Unwind_00476835();
	// !DECL 0x00476835 END
	// !DECL 0x0047684d BEGIN
	/* 47684D */ uchar Unwind_0047684d();
	// !DECL 0x0047684d END
	// !DECL 0x0047685b BEGIN
	/* 47685B */ uchar Unwind_0047685b();
	// !DECL 0x0047685b END
	// !DECL 0x00476869 BEGIN
	/* 476869 */ uchar Unwind_00476869();
	// !DECL 0x00476869 END
	// !DECL 0x00476877 BEGIN
	/* 476877 */ uchar Unwind_00476877();
	// !DECL 0x00476877 END
	// !DECL 0x004768a0 BEGIN
	/* 4768A0 */ uchar Unwind_004768a0();
	// !DECL 0x004768a0 END
	// !DECL 0x004768a8 BEGIN
	/* 4768A8 */ uchar Unwind_004768a8();
	// !DECL 0x004768a8 END
	// !DECL 0x004768cf BEGIN
	/* 4768CF */ uchar Unwind_004768cf();
	// !DECL 0x004768cf END
	// !DECL 0x004768f6 BEGIN
	/* 4768F6 */ uchar Unwind_004768f6();
	// !DECL 0x004768f6 END
	// !DECL 0x00476901 BEGIN
	/* 476901 */ uchar Unwind_00476901();
	// !DECL 0x00476901 END
	// !DECL 0x0047690f BEGIN
	/* 47690F */ uchar Unwind_0047690f();
	// !DECL 0x0047690f END
	// !DECL 0x0047691d BEGIN
	/* 47691D */ uchar Unwind_0047691d();
	// !DECL 0x0047691d END
	// !DECL 0x00476935 BEGIN
	/* 476935 */ uchar Unwind_00476935();
	// !DECL 0x00476935 END
	// !DECL 0x00476943 BEGIN
	/* 476943 */ uchar Unwind_00476943();
	// !DECL 0x00476943 END
	// !DECL 0x00476951 BEGIN
	/* 476951 */ uchar Unwind_00476951();
	// !DECL 0x00476951 END
	// !DECL 0x0047695f BEGIN
	/* 47695F */ uchar Unwind_0047695f();
	// !DECL 0x0047695f END
	// !DECL 0x00476990 BEGIN
	/* 476990 */ uchar Unwind_00476990();
	// !DECL 0x00476990 END
	// !DECL 0x0047699b BEGIN
	/* 47699B */ uchar Unwind_0047699b();
	// !DECL 0x0047699b END
	// !DECL 0x004769a6 BEGIN
	/* 4769A6 */ uchar Unwind_004769a6();
	// !DECL 0x004769a6 END
	// !DECL 0x004769ae BEGIN
	/* 4769AE */ uchar Unwind_004769ae();
	// !DECL 0x004769ae END
	// !DECL 0x004769f0 BEGIN
	/* 4769F0 */ uchar Unwind_004769f0();
	// !DECL 0x004769f0 END
	// !DECL 0x00476a09 BEGIN
	/* 476A09 */ uchar Unwind_00476a09();
	// !DECL 0x00476a09 END
	// !DECL 0x00476a14 BEGIN
	/* 476A14 */ uchar Unwind_00476a14();
	// !DECL 0x00476a14 END
	// !DECL 0x00476a1f BEGIN
	/* 476A1F */ uchar Unwind_00476a1f();
	// !DECL 0x00476a1f END
	// !DECL 0x00476a38 BEGIN
	/* 476A38 */ uchar Unwind_00476a38();
	// !DECL 0x00476a38 END
	// !DECL 0x00476a43 BEGIN
	/* 476A43 */ uchar Unwind_00476a43();
	// !DECL 0x00476a43 END
	// !DECL 0x00476a4e BEGIN
	/* 476A4E */ uchar Unwind_00476a4e();
	// !DECL 0x00476a4e END
	// !DECL 0x00476a90 BEGIN
	/* 476A90 */ uchar Unwind_00476a90();
	// !DECL 0x00476a90 END
	// !DECL 0x00476af0 BEGIN
	/* 476AF0 */ uchar Unwind_00476af0();
	// !DECL 0x00476af0 END
	// !DECL 0x00476b20 BEGIN
	/* 476B20 */ uchar Unwind_00476b20();
	// !DECL 0x00476b20 END
	// !DECL 0x00476b50 BEGIN
	/* 476B50 */ uchar Unwind_00476b50();
	// !DECL 0x00476b50 END
	// !DECL 0x00476b80 BEGIN
	/* 476B80 */ uchar Unwind_00476b80();
	// !DECL 0x00476b80 END
	// !DECL 0x00476bb0 BEGIN
	/* 476BB0 */ uchar Unwind_00476bb0();
	// !DECL 0x00476bb0 END
	// !DECL 0x00476be0 BEGIN
	/* 476BE0 */ uchar Unwind_00476be0();
	// !DECL 0x00476be0 END
	// !DECL 0x00476be8 BEGIN
	/* 476BE8 */ uchar Unwind_00476be8();
	// !DECL 0x00476be8 END
	// !DECL 0x00476bf6 BEGIN
	/* 476BF6 */ uchar Unwind_00476bf6();
	// !DECL 0x00476bf6 END
	// !DECL 0x00476c20 BEGIN
	/* 476C20 */ uchar Unwind_00476c20();
	// !DECL 0x00476c20 END
	// !DECL 0x00476c28 BEGIN
	/* 476C28 */ uchar Unwind_00476c28();
	// !DECL 0x00476c28 END
	// !DECL 0x00476c70 BEGIN
	/* 476C70 */ uchar Unwind_00476c70();
	// !DECL 0x00476c70 END
	// !DECL 0x00476cc0 BEGIN
	/* 476CC0 */ uchar Unwind_00476cc0();
	// !DECL 0x00476cc0 END
	// !DECL 0x00476cf0 BEGIN
	/* 476CF0 */ uchar Unwind_00476cf0();
	// !DECL 0x00476cf0 END
	// !DECL 0x00476d20 BEGIN
	/* 476D20 */ uchar Unwind_00476d20();
	// !DECL 0x00476d20 END
	// !DECL 0x00476d50 BEGIN
	/* 476D50 */ uchar Unwind_00476d50();
	// !DECL 0x00476d50 END
	// !DECL 0x00476d58 BEGIN
	/* 476D58 */ uchar Unwind_00476d58();
	// !DECL 0x00476d58 END
	// !DECL 0x00476d90 BEGIN
	/* 476D90 */ uchar Unwind_00476d90();
	// !DECL 0x00476d90 END
	// !DECL 0x00476dc0 BEGIN
	/* 476DC0 */ uchar Unwind_00476dc0();
	// !DECL 0x00476dc0 END
	// !DECL 0x00476df0 BEGIN
	/* 476DF0 */ uchar Unwind_00476df0();
	// !DECL 0x00476df0 END
	// !DECL 0x00476df8 BEGIN
	/* 476DF8 */ uchar Unwind_00476df8();
	// !DECL 0x00476df8 END
	// !DECL 0x00476e30 BEGIN
	/* 476E30 */ uchar Unwind_00476e30();
	// !DECL 0x00476e30 END
	// !DECL 0x00476e38 BEGIN
	/* 476E38 */ uchar Unwind_00476e38();
	// !DECL 0x00476e38 END
	// !DECL 0x00476e70 BEGIN
	/* 476E70 */ uchar Unwind_00476e70();
	// !DECL 0x00476e70 END
	// !DECL 0x00476e78 BEGIN
	/* 476E78 */ uchar Unwind_00476e78();
	// !DECL 0x00476e78 END
	// !DECL 0x00476eb0 BEGIN
	/* 476EB0 */ uchar Unwind_00476eb0();
	// !DECL 0x00476eb0 END
	// !DECL 0x00476eb8 BEGIN
	/* 476EB8 */ uchar Unwind_00476eb8();
	// !DECL 0x00476eb8 END
	// !DECL 0x00476ef0 BEGIN
	/* 476EF0 */ uchar Unwind_00476ef0();
	// !DECL 0x00476ef0 END
	// !DECL 0x00476f20 BEGIN
	/* 476F20 */ uchar Unwind_00476f20();
	// !DECL 0x00476f20 END
	// !DECL 0x00476f28 BEGIN
	/* 476F28 */ uchar Unwind_00476f28();
	// !DECL 0x00476f28 END
	// !DECL 0x00476f70 BEGIN
	/* 476F70 */ uchar Unwind_00476f70();
	// !DECL 0x00476f70 END
	// !DECL 0x00476fa0 BEGIN
	/* 476FA0 */ uchar Unwind_00476fa0();
	// !DECL 0x00476fa0 END
	// !DECL 0x00476fd0 BEGIN
	/* 476FD0 */ uchar Unwind_00476fd0();
	// !DECL 0x00476fd0 END
	// !DECL 0x00477000 BEGIN
	/* 477000 */ uchar Unwind_00477000();
	// !DECL 0x00477000 END
	// !DECL 0x00477030 BEGIN
	/* 477030 */ uchar Unwind_00477030();
	// !DECL 0x00477030 END
	// !DECL 0x00477060 BEGIN
	/* 477060 */ uchar Unwind_00477060();
	// !DECL 0x00477060 END
	// !DECL 0x00477090 BEGIN
	/* 477090 */ uchar Unwind_00477090();
	// !DECL 0x00477090 END
	// !DECL 0x00477098 BEGIN
	/* 477098 */ uchar Unwind_00477098();
	// !DECL 0x00477098 END
	// !DECL 0x004770a3 BEGIN
	/* 4770A3 */ uchar Unwind_004770a3();
	// !DECL 0x004770a3 END
	// !DECL 0x004770d0 BEGIN
	/* 4770D0 */ uchar Unwind_004770d0();
	// !DECL 0x004770d0 END
	// !DECL 0x004770d8 BEGIN
	/* 4770D8 */ uchar Unwind_004770d8();
	// !DECL 0x004770d8 END
	// !DECL 0x00477101 BEGIN
	/* 477101 */ uchar Unwind_00477101();
	// !DECL 0x00477101 END
	// !DECL 0x0047710f BEGIN
	/* 47710F */ uchar Unwind_0047710f();
	// !DECL 0x0047710f END
	// !DECL 0x00477140 BEGIN
	/* 477140 */ uchar Unwind_00477140();
	// !DECL 0x00477140 END
	// !DECL 0x00477148 BEGIN
	/* 477148 */ uchar Unwind_00477148();
	// !DECL 0x00477148 END
	// !DECL 0x00477150 BEGIN
	/* 477150 */ uchar Unwind_00477150();
	// !DECL 0x00477150 END
	// !DECL 0x00477180 BEGIN
	/* 477180 */ uchar Unwind_00477180();
	// !DECL 0x00477180 END
	// !DECL 0x00477188 BEGIN
	/* 477188 */ uchar Unwind_00477188();
	// !DECL 0x00477188 END
	// !DECL 0x004771af BEGIN
	/* 4771AF */ uchar Unwind_004771af();
	// !DECL 0x004771af END
	// !DECL 0x004771bd BEGIN
	/* 4771BD */ uchar Unwind_004771bd();
	// !DECL 0x004771bd END
	// !DECL 0x004771d5 BEGIN
	/* 4771D5 */ uchar Unwind_004771d5();
	// !DECL 0x004771d5 END
	// !DECL 0x004771e3 BEGIN
	/* 4771E3 */ uchar Unwind_004771e3();
	// !DECL 0x004771e3 END
	// !DECL 0x004771f1 BEGIN
	/* 4771F1 */ uchar Unwind_004771f1();
	// !DECL 0x004771f1 END
	// !DECL 0x004771ff BEGIN
	/* 4771FF */ uchar Unwind_004771ff();
	// !DECL 0x004771ff END
	// !DECL 0x0047720d BEGIN
	/* 47720D */ uchar Unwind_0047720d();
	// !DECL 0x0047720d END
	// !DECL 0x0047721b BEGIN
	/* 47721B */ uchar Unwind_0047721b();
	// !DECL 0x0047721b END
	// !DECL 0x00477229 BEGIN
	/* 477229 */ uchar Unwind_00477229();
	// !DECL 0x00477229 END
	// !DECL 0x00477237 BEGIN
	/* 477237 */ uchar Unwind_00477237();
	// !DECL 0x00477237 END
	// !DECL 0x00477245 BEGIN
	/* 477245 */ uchar Unwind_00477245();
	// !DECL 0x00477245 END
	// !DECL 0x00477253 BEGIN
	/* 477253 */ uchar Unwind_00477253();
	// !DECL 0x00477253 END
	// !DECL 0x00477280 BEGIN
	/* 477280 */ uchar Unwind_00477280();
	// !DECL 0x00477280 END
	// !DECL 0x004772b0 BEGIN
	/* 4772B0 */ uchar Unwind_004772b0();
	// !DECL 0x004772b0 END
	// !DECL 0x004772e0 BEGIN
	/* 4772E0 */ uchar Unwind_004772e0();
	// !DECL 0x004772e0 END
	// !DECL 0x00477310 BEGIN
	/* 477310 */ uchar Unwind_00477310();
	// !DECL 0x00477310 END
	// !DECL 0x00477340 BEGIN
	/* 477340 */ uchar Unwind_00477340();
	// !DECL 0x00477340 END
	// !DECL 0x00477370 BEGIN
	/* 477370 */ uchar Unwind_00477370();
	// !DECL 0x00477370 END
	// !DECL 0x004773a0 BEGIN
	/* 4773A0 */ uchar Unwind_004773a0();
	// !DECL 0x004773a0 END
	// !DECL 0x004773a8 BEGIN
	/* 4773A8 */ uchar Unwind_004773a8();
	// !DECL 0x004773a8 END
	// !DECL 0x004773f0 BEGIN
	/* 4773F0 */ uchar Unwind_004773f0();
	// !DECL 0x004773f0 END
	// !DECL 0x00477430 BEGIN
	/* 477430 */ uchar Unwind_00477430();
	// !DECL 0x00477430 END
	// !DECL 0x00477438 BEGIN
	/* 477438 */ uchar Unwind_00477438();
	// !DECL 0x00477438 END
	// !DECL 0x00477443 BEGIN
	/* 477443 */ uchar Unwind_00477443();
	// !DECL 0x00477443 END
	// !DECL 0x0047744e BEGIN
	/* 47744E */ uchar Unwind_0047744e();
	// !DECL 0x0047744e END
	// !DECL 0x00477459 BEGIN
	/* 477459 */ uchar Unwind_00477459();
	// !DECL 0x00477459 END
	// !DECL 0x00477480 BEGIN
	/* 477480 */ uchar Unwind_00477480();
	// !DECL 0x00477480 END
	// !DECL 0x004774b0 BEGIN
	/* 4774B0 */ uchar Unwind_004774b0();
	// !DECL 0x004774b0 END
	// !DECL 0x004774e0 BEGIN
	/* 4774E0 */ uchar Unwind_004774e0();
	// !DECL 0x004774e0 END
	// !DECL 0x004774e8 BEGIN
	/* 4774E8 */ uchar Unwind_004774e8();
	// !DECL 0x004774e8 END
	// !DECL 0x00477510 BEGIN
	/* 477510 */ uchar Unwind_00477510();
	// !DECL 0x00477510 END
	// !DECL 0x00477518 BEGIN
	/* 477518 */ uchar Unwind_00477518();
	// !DECL 0x00477518 END
	// !DECL 0x00477526 BEGIN
	/* 477526 */ uchar Unwind_00477526();
	// !DECL 0x00477526 END
	// !DECL 0x00477550 BEGIN
	/* 477550 */ uchar Unwind_00477550();
	// !DECL 0x00477550 END
	// !DECL 0x00477558 BEGIN
	/* 477558 */ uchar Unwind_00477558();
	// !DECL 0x00477558 END
	// !DECL 0x00477566 BEGIN
	/* 477566 */ uchar Unwind_00477566();
	// !DECL 0x00477566 END
	// !DECL 0x00477590 BEGIN
	/* 477590 */ uchar Unwind_00477590();
	// !DECL 0x00477590 END
	// !DECL 0x00477598 BEGIN
	/* 477598 */ uchar Unwind_00477598();
	// !DECL 0x00477598 END
	// !DECL 0x004775c0 BEGIN
	/* 4775C0 */ uchar Unwind_004775c0();
	// !DECL 0x004775c0 END
	// !DECL 0x004775c8 BEGIN
	/* 4775C8 */ uchar Unwind_004775c8();
	// !DECL 0x004775c8 END
	// !DECL 0x004775f0 BEGIN
	/* 4775F0 */ uchar Unwind_004775f0();
	// !DECL 0x004775f0 END
	// !DECL 0x004775fb BEGIN
	/* 4775FB */ uchar Unwind_004775fb();
	// !DECL 0x004775fb END
	// !DECL 0x00477630 BEGIN
	/* 477630 */ uchar Unwind_00477630();
	// !DECL 0x00477630 END
	// !DECL 0x00477638 BEGIN
	/* 477638 */ uchar Unwind_00477638();
	// !DECL 0x00477638 END
	// !DECL 0x00477640 BEGIN
	/* 477640 */ uchar Unwind_00477640();
	// !DECL 0x00477640 END
	// !DECL 0x0047764b BEGIN
	/* 47764B */ uchar Unwind_0047764b();
	// !DECL 0x0047764b END
	// !DECL 0x00477653 BEGIN
	/* 477653 */ uchar Unwind_00477653();
	// !DECL 0x00477653 END
	// !DECL 0x00477690 BEGIN
	/* 477690 */ uchar Unwind_00477690();
	// !DECL 0x00477690 END
	// !DECL 0x0047769b BEGIN
	/* 47769B */ uchar Unwind_0047769b();
	// !DECL 0x0047769b END
	// !DECL 0x004776c0 BEGIN
	/* 4776C0 */ uchar Unwind_004776c0();
	// !DECL 0x004776c0 END
	// !DECL 0x004776c8 BEGIN
	/* 4776C8 */ uchar Unwind_004776c8();
	// !DECL 0x004776c8 END
	// !DECL 0x004776f0 BEGIN
	/* 4776F0 */ uchar Unwind_004776f0();
	// !DECL 0x004776f0 END
	// !DECL 0x00477720 BEGIN
	/* 477720 */ uchar Unwind_00477720();
	// !DECL 0x00477720 END
	// !DECL 0x00477750 BEGIN
	/* 477750 */ uchar Unwind_00477750();
	// !DECL 0x00477750 END
	// !DECL 0x00477780 BEGIN
	/* 477780 */ uchar Unwind_00477780();
	// !DECL 0x00477780 END
	// !DECL 0x00477788 BEGIN
	/* 477788 */ uchar Unwind_00477788();
	// !DECL 0x00477788 END
	// !DECL 0x00477793 BEGIN
	/* 477793 */ uchar Unwind_00477793();
	// !DECL 0x00477793 END
	// !DECL 0x0047779e BEGIN
	/* 47779E */ uchar Unwind_0047779e();
	// !DECL 0x0047779e END
	// !DECL 0x004777a9 BEGIN
	/* 4777A9 */ uchar Unwind_004777a9();
	// !DECL 0x004777a9 END
	// !DECL 0x004777b4 BEGIN
	/* 4777B4 */ uchar Unwind_004777b4();
	// !DECL 0x004777b4 END
	// !DECL 0x004777e0 BEGIN
	/* 4777E0 */ uchar Unwind_004777e0();
	// !DECL 0x004777e0 END
	// !DECL 0x00477810 BEGIN
	/* 477810 */ uchar Unwind_00477810();
	// !DECL 0x00477810 END
	// !DECL 0x00477840 BEGIN
	/* 477840 */ uchar Unwind_00477840();
	// !DECL 0x00477840 END
	// !DECL 0x00477848 BEGIN
	/* 477848 */ uchar Unwind_00477848();
	// !DECL 0x00477848 END
	// !DECL 0x00477856 BEGIN
	/* 477856 */ uchar Unwind_00477856();
	// !DECL 0x00477856 END
	// !DECL 0x00477864 BEGIN
	/* 477864 */ uchar Unwind_00477864();
	// !DECL 0x00477864 END
	// !DECL 0x00477872 BEGIN
	/* 477872 */ uchar Unwind_00477872();
	// !DECL 0x00477872 END
	// !DECL 0x0047787a BEGIN
	/* 47787A */ uchar Unwind_0047787a();
	// !DECL 0x0047787a END
	// !DECL 0x004778a0 BEGIN
	/* 4778A0 */ uchar Unwind_004778a0();
	// !DECL 0x004778a0 END
	// !DECL 0x004778d0 BEGIN
	/* 4778D0 */ uchar Unwind_004778d0();
	// !DECL 0x004778d0 END
	// !DECL 0x004778d8 BEGIN
	/* 4778D8 */ uchar Unwind_004778d8();
	// !DECL 0x004778d8 END
	// !DECL 0x00477910 BEGIN
	/* 477910 */ uchar Unwind_00477910();
	// !DECL 0x00477910 END
	// !DECL 0x00477940 BEGIN
	/* 477940 */ uchar Unwind_00477940();
	// !DECL 0x00477940 END
	// !DECL 0x00477970 BEGIN
	/* 477970 */ uchar Unwind_00477970();
	// !DECL 0x00477970 END
	// !DECL 0x004779a0 BEGIN
	/* 4779A0 */ uchar Unwind_004779a0();
	// !DECL 0x004779a0 END
	// !DECL 0x004779ab BEGIN
	/* 4779AB */ uchar Unwind_004779ab();
	// !DECL 0x004779ab END
	// !DECL 0x004779e0 BEGIN
	/* 4779E0 */ uchar Unwind_004779e0();
	// !DECL 0x004779e0 END
	// !DECL 0x00477a10 BEGIN
	/* 477A10 */ uchar Unwind_00477a10();
	// !DECL 0x00477a10 END
	// !DECL 0x00477a40 BEGIN
	/* 477A40 */ uchar Unwind_00477a40();
	// !DECL 0x00477a40 END
	// !DECL 0x00477a48 BEGIN
	/* 477A48 */ uchar Unwind_00477a48();
	// !DECL 0x00477a48 END
	// !DECL 0x00477a53 BEGIN
	/* 477A53 */ uchar Unwind_00477a53();
	// !DECL 0x00477a53 END
	// !DECL 0x00477a61 BEGIN
	/* 477A61 */ uchar Unwind_00477a61();
	// !DECL 0x00477a61 END
	// !DECL 0x00477a79 BEGIN
	/* 477A79 */ uchar Unwind_00477a79();
	// !DECL 0x00477a79 END
	// !DECL 0x00477a87 BEGIN
	/* 477A87 */ uchar Unwind_00477a87();
	// !DECL 0x00477a87 END
	// !DECL 0x00477a95 BEGIN
	/* 477A95 */ uchar Unwind_00477a95();
	// !DECL 0x00477a95 END
	// !DECL 0x00477aa3 BEGIN
	/* 477AA3 */ uchar Unwind_00477aa3();
	// !DECL 0x00477aa3 END
	// !DECL 0x00477ab1 BEGIN
	/* 477AB1 */ uchar Unwind_00477ab1();
	// !DECL 0x00477ab1 END
	// !DECL 0x00477abf BEGIN
	/* 477ABF */ uchar Unwind_00477abf();
	// !DECL 0x00477abf END
	// !DECL 0x00477acd BEGIN
	/* 477ACD */ uchar Unwind_00477acd();
	// !DECL 0x00477acd END
	// !DECL 0x00477adb BEGIN
	/* 477ADB */ uchar Unwind_00477adb();
	// !DECL 0x00477adb END
	// !DECL 0x00477ae9 BEGIN
	/* 477AE9 */ uchar Unwind_00477ae9();
	// !DECL 0x00477ae9 END
	// !DECL 0x00477af7 BEGIN
	/* 477AF7 */ uchar Unwind_00477af7();
	// !DECL 0x00477af7 END
	// !DECL 0x00477b05 BEGIN
	/* 477B05 */ uchar Unwind_00477b05();
	// !DECL 0x00477b05 END
	// !DECL 0x00477b10 BEGIN
	/* 477B10 */ uchar Unwind_00477b10();
	// !DECL 0x00477b10 END
	// !DECL 0x00477b1b BEGIN
	/* 477B1B */ uchar Unwind_00477b1b();
	// !DECL 0x00477b1b END
	// !DECL 0x00477b50 BEGIN
	/* 477B50 */ uchar Unwind_00477b50();
	// !DECL 0x00477b50 END
	// !DECL 0x00477b5b BEGIN
	/* 477B5B */ uchar Unwind_00477b5b();
	// !DECL 0x00477b5b END
	// !DECL 0x00477b66 BEGIN
	/* 477B66 */ uchar Unwind_00477b66();
	// !DECL 0x00477b66 END
	// !DECL 0x00477b90 BEGIN
	/* 477B90 */ uchar Unwind_00477b90();
	// !DECL 0x00477b90 END
	// !DECL 0x00477b98 BEGIN
	/* 477B98 */ uchar Unwind_00477b98();
	// !DECL 0x00477b98 END
	// !DECL 0x00477bd0 BEGIN
	/* 477BD0 */ uchar Unwind_00477bd0();
	// !DECL 0x00477bd0 END
	// !DECL 0x00477c00 BEGIN
	/* 477C00 */ uchar Unwind_00477c00();
	// !DECL 0x00477c00 END
	// !DECL 0x00477c30 BEGIN
	/* 477C30 */ uchar Unwind_00477c30();
	// !DECL 0x00477c30 END
	// !DECL 0x00477c60 BEGIN
	/* 477C60 */ uchar Unwind_00477c60();
	// !DECL 0x00477c60 END
	// !DECL 0x00477c90 BEGIN
	/* 477C90 */ uchar Unwind_00477c90();
	// !DECL 0x00477c90 END
	// !DECL 0x00477cc0 BEGIN
	/* 477CC0 */ uchar Unwind_00477cc0();
	// !DECL 0x00477cc0 END
	// !DECL 0x00477cf0 BEGIN
	/* 477CF0 */ uchar Unwind_00477cf0();
	// !DECL 0x00477cf0 END
	// !DECL 0x00477d20 BEGIN
	/* 477D20 */ uchar Unwind_00477d20();
	// !DECL 0x00477d20 END
	// !DECL 0x00477d28 BEGIN
	/* 477D28 */ uchar Unwind_00477d28();
	// !DECL 0x00477d28 END
	// !DECL 0x00477d33 BEGIN
	/* 477D33 */ uchar Unwind_00477d33();
	// !DECL 0x00477d33 END
	// !DECL 0x00477d41 BEGIN
	/* 477D41 */ uchar Unwind_00477d41();
	// !DECL 0x00477d41 END
	// !DECL 0x00477d4c BEGIN
	/* 477D4C */ uchar Unwind_00477d4c();
	// !DECL 0x00477d4c END
	// !DECL 0x00477d57 BEGIN
	/* 477D57 */ uchar Unwind_00477d57();
	// !DECL 0x00477d57 END
	// !DECL 0x00477d62 BEGIN
	/* 477D62 */ uchar Unwind_00477d62();
	// !DECL 0x00477d62 END
	// !DECL 0x00477d90 BEGIN
	/* 477D90 */ uchar Unwind_00477d90();
	// !DECL 0x00477d90 END
	// !DECL 0x00477d98 BEGIN
	/* 477D98 */ uchar Unwind_00477d98();
	// !DECL 0x00477d98 END
	// !DECL 0x00477da3 BEGIN
	/* 477DA3 */ uchar Unwind_00477da3();
	// !DECL 0x00477da3 END
	// !DECL 0x00477dd0 BEGIN
	/* 477DD0 */ uchar Unwind_00477dd0();
	// !DECL 0x00477dd0 END
	// !DECL 0x00477dd8 BEGIN
	/* 477DD8 */ uchar Unwind_00477dd8();
	// !DECL 0x00477dd8 END
	// !DECL 0x00477e00 BEGIN
	/* 477E00 */ uchar Unwind_00477e00();
	// !DECL 0x00477e00 END
	// !DECL 0x00477e30 BEGIN
	/* 477E30 */ uchar Unwind_00477e30();
	// !DECL 0x00477e30 END
	// !DECL 0x00477e60 BEGIN
	/* 477E60 */ uchar Unwind_00477e60();
	// !DECL 0x00477e60 END
	// !DECL 0x00477e90 BEGIN
	/* 477E90 */ uchar Unwind_00477e90();
	// !DECL 0x00477e90 END
	// !DECL 0x00477e98 BEGIN
	/* 477E98 */ uchar Unwind_00477e98();
	// !DECL 0x00477e98 END
	// !DECL 0x00477ed0 BEGIN
	/* 477ED0 */ uchar Unwind_00477ed0();
	// !DECL 0x00477ed0 END
	// !DECL 0x00477f00 BEGIN
	/* 477F00 */ uchar Unwind_00477f00();
	// !DECL 0x00477f00 END
	// !DECL 0x00477f08 BEGIN
	/* 477F08 */ uchar Unwind_00477f08();
	// !DECL 0x00477f08 END
	// !DECL 0x00477f30 BEGIN
	/* 477F30 */ uchar Unwind_00477f30();
	// !DECL 0x00477f30 END
	// !DECL 0x00477f60 BEGIN
	/* 477F60 */ uchar Unwind_00477f60();
	// !DECL 0x00477f60 END
	// !DECL 0x00477f90 BEGIN
	/* 477F90 */ uchar Unwind_00477f90();
	// !DECL 0x00477f90 END
	// !DECL 0x00477fc0 BEGIN
	/* 477FC0 */ uchar Unwind_00477fc0();
	// !DECL 0x00477fc0 END
	// !DECL 0x00477fc8 BEGIN
	/* 477FC8 */ uchar Unwind_00477fc8();
	// !DECL 0x00477fc8 END
	// !DECL 0x00477ff0 BEGIN
	/* 477FF0 */ uchar Unwind_00477ff0();
	// !DECL 0x00477ff0 END
	// !DECL 0x00478020 BEGIN
	/* 478020 */ uchar Unwind_00478020();
	// !DECL 0x00478020 END
	// !DECL 0x00478050 BEGIN
	/* 478050 */ uchar Unwind_00478050();
	// !DECL 0x00478050 END
	// !DECL 0x00478080 BEGIN
	/* 478080 */ uchar Unwind_00478080();
	// !DECL 0x00478080 END
	// !DECL 0x004780b0 BEGIN
	/* 4780B0 */ uchar Unwind_004780b0();
	// !DECL 0x004780b0 END
	// !DECL 0x004780b8 BEGIN
	/* 4780B8 */ uchar Unwind_004780b8();
	// !DECL 0x004780b8 END
	// !DECL 0x004780c3 BEGIN
	/* 4780C3 */ uchar Unwind_004780c3();
	// !DECL 0x004780c3 END
	// !DECL 0x004780d1 BEGIN
	/* 4780D1 */ uchar Unwind_004780d1();
	// !DECL 0x004780d1 END
	// !DECL 0x004780dc BEGIN
	/* 4780DC */ uchar Unwind_004780dc();
	// !DECL 0x004780dc END
	// !DECL 0x004780e7 BEGIN
	/* 4780E7 */ uchar Unwind_004780e7();
	// !DECL 0x004780e7 END
	// !DECL 0x004780f2 BEGIN
	/* 4780F2 */ uchar Unwind_004780f2();
	// !DECL 0x004780f2 END
	// !DECL 0x00478120 BEGIN
	/* 478120 */ uchar Unwind_00478120();
	// !DECL 0x00478120 END
	// !DECL 0x00478128 BEGIN
	/* 478128 */ uchar Unwind_00478128();
	// !DECL 0x00478128 END
	// !DECL 0x00478133 BEGIN
	/* 478133 */ uchar Unwind_00478133();
	// !DECL 0x00478133 END
	// !DECL 0x00478160 BEGIN
	/* 478160 */ uchar Unwind_00478160();
	// !DECL 0x00478160 END
	// !DECL 0x00478168 BEGIN
	/* 478168 */ uchar Unwind_00478168();
	// !DECL 0x00478168 END
	// !DECL 0x00478190 BEGIN
	/* 478190 */ uchar Unwind_00478190();
	// !DECL 0x00478190 END
	// !DECL 0x004781c0 BEGIN
	/* 4781C0 */ uchar Unwind_004781c0();
	// !DECL 0x004781c0 END
	// !DECL 0x004781c8 BEGIN
	/* 4781C8 */ uchar Unwind_004781c8();
	// !DECL 0x004781c8 END
	// !DECL 0x004781d3 BEGIN
	/* 4781D3 */ uchar Unwind_004781d3();
	// !DECL 0x004781d3 END
	// !DECL 0x00478200 BEGIN
	/* 478200 */ uchar Unwind_00478200();
	// !DECL 0x00478200 END
	// !DECL 0x00478230 BEGIN
	/* 478230 */ uchar Unwind_00478230();
	// !DECL 0x00478230 END
	// !DECL 0x00478238 BEGIN
	/* 478238 */ uchar Unwind_00478238();
	// !DECL 0x00478238 END
	// !DECL 0x00478260 BEGIN
	/* 478260 */ uchar Unwind_00478260();
	// !DECL 0x00478260 END
	// !DECL 0x00478268 BEGIN
	/* 478268 */ uchar Unwind_00478268();
	// !DECL 0x00478268 END
	// !DECL 0x00478290 BEGIN
	/* 478290 */ uchar Unwind_00478290();
	// !DECL 0x00478290 END
	// !DECL 0x004782c0 BEGIN
	/* 4782C0 */ uchar Unwind_004782c0();
	// !DECL 0x004782c0 END
	// !DECL 0x004782f0 BEGIN
	/* 4782F0 */ uchar Unwind_004782f0();
	// !DECL 0x004782f0 END
	// !DECL 0x00478320 BEGIN
	/* 478320 */ uchar Unwind_00478320();
	// !DECL 0x00478320 END
	// !DECL 0x00478350 BEGIN
	/* 478350 */ uchar Unwind_00478350();
	// !DECL 0x00478350 END
	// !DECL 0x00478358 BEGIN
	/* 478358 */ uchar Unwind_00478358();
	// !DECL 0x00478358 END
	// !DECL 0x00478363 BEGIN
	/* 478363 */ uchar Unwind_00478363();
	// !DECL 0x00478363 END
	// !DECL 0x00478390 BEGIN
	/* 478390 */ uchar Unwind_00478390();
	// !DECL 0x00478390 END
	// !DECL 0x00478398 BEGIN
	/* 478398 */ uchar Unwind_00478398();
	// !DECL 0x00478398 END
	// !DECL 0x004783bf BEGIN
	/* 4783BF */ uchar Unwind_004783bf();
	// !DECL 0x004783bf END
	// !DECL 0x004783cd BEGIN
	/* 4783CD */ uchar Unwind_004783cd();
	// !DECL 0x004783cd END
	// !DECL 0x00478400 BEGIN
	/* 478400 */ uchar Unwind_00478400();
	// !DECL 0x00478400 END
	// !DECL 0x00478430 BEGIN
	/* 478430 */ uchar Unwind_00478430();
	// !DECL 0x00478430 END
	// !DECL 0x00478460 BEGIN
	/* 478460 */ uchar Unwind_00478460();
	// !DECL 0x00478460 END
	// !DECL 0x0047846b BEGIN
	/* 47846B */ uchar Unwind_0047846b();
	// !DECL 0x0047846b END
	// !DECL 0x00478476 BEGIN
	/* 478476 */ uchar Unwind_00478476();
	// !DECL 0x00478476 END
	// !DECL 0x004784a0 BEGIN
	/* 4784A0 */ uchar Unwind_004784a0();
	// !DECL 0x004784a0 END
	// !DECL 0x004784ab BEGIN
	/* 4784AB */ uchar Unwind_004784ab();
	// !DECL 0x004784ab END
	// !DECL 0x004784b6 BEGIN
	/* 4784B6 */ uchar Unwind_004784b6();
	// !DECL 0x004784b6 END
	// !DECL 0x004784e0 BEGIN
	/* 4784E0 */ uchar Unwind_004784e0();
	// !DECL 0x004784e0 END
	// !DECL 0x00478510 BEGIN
	/* 478510 */ uchar Unwind_00478510();
	// !DECL 0x00478510 END
	// !DECL 0x00478518 BEGIN
	/* 478518 */ uchar Unwind_00478518();
	// !DECL 0x00478518 END
	// !DECL 0x00478560 BEGIN
	/* 478560 */ uchar Unwind_00478560();
	// !DECL 0x00478560 END
	// !DECL 0x00478568 BEGIN
	/* 478568 */ uchar Unwind_00478568();
	// !DECL 0x00478568 END
	// !DECL 0x00478573 BEGIN
	/* 478573 */ uchar Unwind_00478573();
	// !DECL 0x00478573 END
	// !DECL 0x0047857e BEGIN
	/* 47857E */ uchar Unwind_0047857e();
	// !DECL 0x0047857e END
	// !DECL 0x0047858c BEGIN
	/* 47858C */ uchar Unwind_0047858c();
	// !DECL 0x0047858c END
	// !DECL 0x0047859a BEGIN
	/* 47859A */ uchar Unwind_0047859a();
	// !DECL 0x0047859a END
	// !DECL 0x004785a8 BEGIN
	/* 4785A8 */ uchar Unwind_004785a8();
	// !DECL 0x004785a8 END
	// !DECL 0x004785b6 BEGIN
	/* 4785B6 */ uchar Unwind_004785b6();
	// !DECL 0x004785b6 END
	// !DECL 0x004785c4 BEGIN
	/* 4785C4 */ uchar Unwind_004785c4();
	// !DECL 0x004785c4 END
	// !DECL 0x004785f0 BEGIN
	/* 4785F0 */ uchar Unwind_004785f0();
	// !DECL 0x004785f0 END
	// !DECL 0x004785f8 BEGIN
	/* 4785F8 */ uchar Unwind_004785f8();
	// !DECL 0x004785f8 END
	// !DECL 0x00478620 BEGIN
	/* 478620 */ uchar Unwind_00478620();
	// !DECL 0x00478620 END
	// !DECL 0x0047862b BEGIN
	/* 47862B */ uchar Unwind_0047862b();
	// !DECL 0x0047862b END
	// !DECL 0x00478636 BEGIN
	/* 478636 */ uchar Unwind_00478636();
	// !DECL 0x00478636 END
	// !DECL 0x00478660 BEGIN
	/* 478660 */ uchar Unwind_00478660();
	// !DECL 0x00478660 END
	// !DECL 0x00478668 BEGIN
	/* 478668 */ uchar Unwind_00478668();
	// !DECL 0x00478668 END
	// !DECL 0x0047868f BEGIN
	/* 47868F */ uchar Unwind_0047868f();
	// !DECL 0x0047868f END
	// !DECL 0x0047869d BEGIN
	/* 47869D */ uchar Unwind_0047869d();
	// !DECL 0x0047869d END
	// !DECL 0x004786ab BEGIN
	/* 4786AB */ uchar Unwind_004786ab();
	// !DECL 0x004786ab END
	// !DECL 0x004786b9 BEGIN
	/* 4786B9 */ uchar Unwind_004786b9();
	// !DECL 0x004786b9 END
	// !DECL 0x004786c7 BEGIN
	/* 4786C7 */ uchar Unwind_004786c7();
	// !DECL 0x004786c7 END
	// !DECL 0x00478700 BEGIN
	/* 478700 */ uchar Unwind_00478700();
	// !DECL 0x00478700 END
	// !DECL 0x00478730 BEGIN
	/* 478730 */ uchar Unwind_00478730();
	// !DECL 0x00478730 END
	// !DECL 0x00478738 BEGIN
	/* 478738 */ uchar Unwind_00478738();
	// !DECL 0x00478738 END
	// !DECL 0x00478743 BEGIN
	/* 478743 */ uchar Unwind_00478743();
	// !DECL 0x00478743 END
	// !DECL 0x0047874e BEGIN
	/* 47874E */ uchar Unwind_0047874e();
	// !DECL 0x0047874e END
	// !DECL 0x0047875c BEGIN
	/* 47875C */ uchar Unwind_0047875c();
	// !DECL 0x0047875c END
	// !DECL 0x0047876a BEGIN
	/* 47876A */ uchar Unwind_0047876a();
	// !DECL 0x0047876a END
	// !DECL 0x00478778 BEGIN
	/* 478778 */ uchar Unwind_00478778();
	// !DECL 0x00478778 END
	// !DECL 0x00478786 BEGIN
	/* 478786 */ uchar Unwind_00478786();
	// !DECL 0x00478786 END
	// !DECL 0x00478794 BEGIN
	/* 478794 */ uchar Unwind_00478794();
	// !DECL 0x00478794 END
	// !DECL 0x004787a2 BEGIN
	/* 4787A2 */ uchar Unwind_004787a2();
	// !DECL 0x004787a2 END
	// !DECL 0x004787d0 BEGIN
	/* 4787D0 */ uchar Unwind_004787d0();
	// !DECL 0x004787d0 END
	// !DECL 0x004787d8 BEGIN
	/* 4787D8 */ uchar Unwind_004787d8();
	// !DECL 0x004787d8 END
	// !DECL 0x004787e3 BEGIN
	/* 4787E3 */ uchar Unwind_004787e3();
	// !DECL 0x004787e3 END
	// !DECL 0x004787f1 BEGIN
	/* 4787F1 */ uchar Unwind_004787f1();
	// !DECL 0x004787f1 END
	// !DECL 0x004787ff BEGIN
	/* 4787FF */ uchar Unwind_004787ff();
	// !DECL 0x004787ff END
	// !DECL 0x0047880d BEGIN
	/* 47880D */ uchar Unwind_0047880d();
	// !DECL 0x0047880d END
	// !DECL 0x0047881b BEGIN
	/* 47881B */ uchar Unwind_0047881b();
	// !DECL 0x0047881b END
	// !DECL 0x00478850 BEGIN
	/* 478850 */ uchar Unwind_00478850();
	// !DECL 0x00478850 END
	// !DECL 0x00478880 BEGIN
	/* 478880 */ uchar Unwind_00478880();
	// !DECL 0x00478880 END
	// !DECL 0x004788b0 BEGIN
	/* 4788B0 */ uchar Unwind_004788b0();
	// !DECL 0x004788b0 END
	// !DECL 0x004788b8 BEGIN
	/* 4788B8 */ uchar Unwind_004788b8();
	// !DECL 0x004788b8 END
	// !DECL 0x004788c3 BEGIN
	/* 4788C3 */ uchar Unwind_004788c3();
	// !DECL 0x004788c3 END
	// !DECL 0x004788d1 BEGIN
	/* 4788D1 */ uchar Unwind_004788d1();
	// !DECL 0x004788d1 END
	// !DECL 0x004788df BEGIN
	/* 4788DF */ uchar Unwind_004788df();
	// !DECL 0x004788df END
	// !DECL 0x004788e7 BEGIN
	/* 4788E7 */ uchar Unwind_004788e7();
	// !DECL 0x004788e7 END
	// !DECL 0x004788f2 BEGIN
	/* 4788F2 */ uchar Unwind_004788f2();
	// !DECL 0x004788f2 END
	// !DECL 0x004788fd BEGIN
	/* 4788FD */ uchar Unwind_004788fd();
	// !DECL 0x004788fd END
	// !DECL 0x00478908 BEGIN
	/* 478908 */ uchar Unwind_00478908();
	// !DECL 0x00478908 END
	// !DECL 0x00478913 BEGIN
	/* 478913 */ uchar Unwind_00478913();
	// !DECL 0x00478913 END
	// !DECL 0x0047891b BEGIN
	/* 47891B */ uchar Unwind_0047891b();
	// !DECL 0x0047891b END
	// !DECL 0x00478926 BEGIN
	/* 478926 */ uchar Unwind_00478926();
	// !DECL 0x00478926 END
	// !DECL 0x00478931 BEGIN
	/* 478931 */ uchar Unwind_00478931();
	// !DECL 0x00478931 END
	// !DECL 0x0047893c BEGIN
	/* 47893C */ uchar Unwind_0047893c();
	// !DECL 0x0047893c END
	// !DECL 0x00478947 BEGIN
	/* 478947 */ uchar Unwind_00478947();
	// !DECL 0x00478947 END
	// !DECL 0x00478952 BEGIN
	/* 478952 */ uchar Unwind_00478952();
	// !DECL 0x00478952 END
	// !DECL 0x0047895d BEGIN
	/* 47895D */ uchar Unwind_0047895d();
	// !DECL 0x0047895d END
	// !DECL 0x00478968 BEGIN
	/* 478968 */ uchar Unwind_00478968();
	// !DECL 0x00478968 END
	// !DECL 0x00478973 BEGIN
	/* 478973 */ uchar Unwind_00478973();
	// !DECL 0x00478973 END
	// !DECL 0x0047897e BEGIN
	/* 47897E */ uchar Unwind_0047897e();
	// !DECL 0x0047897e END
	// !DECL 0x004789b0 BEGIN
	/* 4789B0 */ uchar Unwind_004789b0();
	// !DECL 0x004789b0 END
	// !DECL 0x004789e0 BEGIN
	/* 4789E0 */ uchar Unwind_004789e0();
	// !DECL 0x004789e0 END
	// !DECL 0x00478a10 BEGIN
	/* 478A10 */ uchar Unwind_00478a10();
	// !DECL 0x00478a10 END
	// !DECL 0x00478a18 BEGIN
	/* 478A18 */ uchar Unwind_00478a18();
	// !DECL 0x00478a18 END
	// !DECL 0x00478a40 BEGIN
	/* 478A40 */ uchar Unwind_00478a40();
	// !DECL 0x00478a40 END
	// !DECL 0x00478a48 BEGIN
	/* 478A48 */ uchar Unwind_00478a48();
	// !DECL 0x00478a48 END
	// !DECL 0x00478a90 BEGIN
	/* 478A90 */ uchar Unwind_00478a90();
	// !DECL 0x00478a90 END
	// !DECL 0x00478ac0 BEGIN
	/* 478AC0 */ uchar Unwind_00478ac0();
	// !DECL 0x00478ac0 END
	// !DECL 0x00478af0 BEGIN
	/* 478AF0 */ uchar Unwind_00478af0();
	// !DECL 0x00478af0 END
	// !DECL 0x00478b20 BEGIN
	/* 478B20 */ uchar Unwind_00478b20();
	// !DECL 0x00478b20 END
	// !DECL 0x00478b50 BEGIN
	/* 478B50 */ uchar Unwind_00478b50();
	// !DECL 0x00478b50 END
	// !DECL 0x00478b80 BEGIN
	/* 478B80 */ uchar Unwind_00478b80();
	// !DECL 0x00478b80 END
	// !DECL 0x00478bb0 BEGIN
	/* 478BB0 */ uchar Unwind_00478bb0();
	// !DECL 0x00478bb0 END
	// !DECL 0x00478bb8 BEGIN
	/* 478BB8 */ uchar Unwind_00478bb8();
	// !DECL 0x00478bb8 END
	// !DECL 0x00478be0 BEGIN
	/* 478BE0 */ uchar Unwind_00478be0();
	// !DECL 0x00478be0 END
	// !DECL 0x00478be8 BEGIN
	/* 478BE8 */ uchar Unwind_00478be8();
	// !DECL 0x00478be8 END
	// !DECL 0x00478c20 BEGIN
	/* 478C20 */ uchar Unwind_00478c20();
	// !DECL 0x00478c20 END
	// !DECL 0x00478c28 BEGIN
	/* 478C28 */ uchar Unwind_00478c28();
	// !DECL 0x00478c28 END
	// !DECL 0x00478c33 BEGIN
	/* 478C33 */ uchar Unwind_00478c33();
	// !DECL 0x00478c33 END
	// !DECL 0x00478c70 BEGIN
	/* 478C70 */ uchar Unwind_00478c70();
	// !DECL 0x00478c70 END
	// !DECL 0x00478c78 BEGIN
	/* 478C78 */ uchar Unwind_00478c78();
	// !DECL 0x00478c78 END
	// !DECL 0x00478c8e BEGIN
	/* 478C8E */ uchar Unwind_00478c8e();
	// !DECL 0x00478c8e END
	// !DECL 0x00478c96 BEGIN
	/* 478C96 */ uchar Unwind_00478c96();
	// !DECL 0x00478c96 END
	// !DECL 0x00478ca1 BEGIN
	/* 478CA1 */ uchar Unwind_00478ca1();
	// !DECL 0x00478ca1 END
	// !DECL 0x00478ca9 BEGIN
	/* 478CA9 */ uchar Unwind_00478ca9();
	// !DECL 0x00478ca9 END
	// !DECL 0x00478cb1 BEGIN
	/* 478CB1 */ uchar Unwind_00478cb1();
	// !DECL 0x00478cb1 END
	// !DECL 0x00478cbc BEGIN
	/* 478CBC */ uchar Unwind_00478cbc();
	// !DECL 0x00478cbc END
	// !DECL 0x00478cc7 BEGIN
	/* 478CC7 */ uchar Unwind_00478cc7();
	// !DECL 0x00478cc7 END
	// !DECL 0x00478cd2 BEGIN
	/* 478CD2 */ uchar Unwind_00478cd2();
	// !DECL 0x00478cd2 END
	// !DECL 0x00478cdd BEGIN
	/* 478CDD */ uchar Unwind_00478cdd();
	// !DECL 0x00478cdd END
	// !DECL 0x00478ce8 BEGIN
	/* 478CE8 */ uchar Unwind_00478ce8();
	// !DECL 0x00478ce8 END
	// !DECL 0x00478cf3 BEGIN
	/* 478CF3 */ uchar Unwind_00478cf3();
	// !DECL 0x00478cf3 END
	// !DECL 0x00478cfe BEGIN
	/* 478CFE */ uchar Unwind_00478cfe();
	// !DECL 0x00478cfe END
	// !DECL 0x00478d09 BEGIN
	/* 478D09 */ uchar Unwind_00478d09();
	// !DECL 0x00478d09 END
	// !DECL 0x00478d14 BEGIN
	/* 478D14 */ uchar Unwind_00478d14();
	// !DECL 0x00478d14 END
	// !DECL 0x00478d1f BEGIN
	/* 478D1F */ uchar Unwind_00478d1f();
	// !DECL 0x00478d1f END
	// !DECL 0x00478d27 BEGIN
	/* 478D27 */ uchar Unwind_00478d27();
	// !DECL 0x00478d27 END
	// !DECL 0x00478d50 BEGIN
	/* 478D50 */ uchar Unwind_00478d50();
	// !DECL 0x00478d50 END
	// !DECL 0x00478d80 BEGIN
	/* 478D80 */ uchar Unwind_00478d80();
	// !DECL 0x00478d80 END
	// !DECL 0x00478dd0 BEGIN
	/* 478DD0 */ uchar Unwind_00478dd0();
	// !DECL 0x00478dd0 END
	// !DECL 0x00478e00 BEGIN
	/* 478E00 */ uchar Unwind_00478e00();
	// !DECL 0x00478e00 END
	// !DECL 0x00478e08 BEGIN
	/* 478E08 */ uchar Unwind_00478e08();
	// !DECL 0x00478e08 END
	// !DECL 0x00478e2f BEGIN
	/* 478E2F */ uchar Unwind_00478e2f();
	// !DECL 0x00478e2f END
	// !DECL 0x00478e56 BEGIN
	/* 478E56 */ uchar Unwind_00478e56();
	// !DECL 0x00478e56 END
	// !DECL 0x00478e80 BEGIN
	/* 478E80 */ uchar Unwind_00478e80();
	// !DECL 0x00478e80 END
	// !DECL 0x00478eb0 BEGIN
	/* 478EB0 */ uchar Unwind_00478eb0();
	// !DECL 0x00478eb0 END
	// !DECL 0x00478ee0 BEGIN
	/* 478EE0 */ uchar Unwind_00478ee0();
	// !DECL 0x00478ee0 END
	// !DECL 0x00478ee8 BEGIN
	/* 478EE8 */ uchar Unwind_00478ee8();
	// !DECL 0x00478ee8 END
	// !DECL 0x00478f30 BEGIN
	/* 478F30 */ uchar Unwind_00478f30();
	// !DECL 0x00478f30 END
	// !DECL 0x00478f38 BEGIN
	/* 478F38 */ uchar Unwind_00478f38();
	// !DECL 0x00478f38 END
	// !DECL 0x00478f5f BEGIN
	/* 478F5F */ uchar Unwind_00478f5f();
	// !DECL 0x00478f5f END
	// !DECL 0x00478f90 BEGIN
	/* 478F90 */ uchar Unwind_00478f90();
	// !DECL 0x00478f90 END
	// !DECL 0x00478f98 BEGIN
	/* 478F98 */ uchar Unwind_00478f98();
	// !DECL 0x00478f98 END
	// !DECL 0x00478fa3 BEGIN
	/* 478FA3 */ uchar Unwind_00478fa3();
	// !DECL 0x00478fa3 END
	// !DECL 0x00478fd0 BEGIN
	/* 478FD0 */ uchar Unwind_00478fd0();
	// !DECL 0x00478fd0 END
	// !DECL 0x00478fd8 BEGIN
	/* 478FD8 */ uchar Unwind_00478fd8();
	// !DECL 0x00478fd8 END
	// !DECL 0x00478fe0 BEGIN
	/* 478FE0 */ uchar Unwind_00478fe0();
	// !DECL 0x00478fe0 END
	// !DECL 0x00478fe8 BEGIN
	/* 478FE8 */ uchar Unwind_00478fe8();
	// !DECL 0x00478fe8 END
	// !DECL 0x00478ff0 BEGIN
	/* 478FF0 */ uchar Unwind_00478ff0();
	// !DECL 0x00478ff0 END
	// !DECL 0x00478ffb BEGIN
	/* 478FFB */ uchar Unwind_00478ffb();
	// !DECL 0x00478ffb END
	// !DECL 0x00479040 BEGIN
	/* 479040 */ uchar Unwind_00479040();
	// !DECL 0x00479040 END
	// !DECL 0x00479070 BEGIN
	/* 479070 */ uchar Unwind_00479070();
	// !DECL 0x00479070 END
	// !DECL 0x00479078 BEGIN
	/* 479078 */ uchar Unwind_00479078();
	// !DECL 0x00479078 END
	// !DECL 0x00479080 BEGIN
	/* 479080 */ uchar Unwind_00479080();
	// !DECL 0x00479080 END
	// !DECL 0x004790b0 BEGIN
	/* 4790B0 */ uchar Unwind_004790b0();
	// !DECL 0x004790b0 END
	// !DECL 0x004790b8 BEGIN
	/* 4790B8 */ uchar Unwind_004790b8();
	// !DECL 0x004790b8 END
	// !DECL 0x004790e0 BEGIN
	/* 4790E0 */ uchar Unwind_004790e0();
	// !DECL 0x004790e0 END
	// !DECL 0x004790e8 BEGIN
	/* 4790E8 */ uchar Unwind_004790e8();
	// !DECL 0x004790e8 END
	// !DECL 0x00479130 BEGIN
	/* 479130 */ uchar Unwind_00479130();
	// !DECL 0x00479130 END
	// !DECL 0x00479138 BEGIN
	/* 479138 */ uchar Unwind_00479138();
	// !DECL 0x00479138 END
	// !DECL 0x00479143 BEGIN
	/* 479143 */ uchar Unwind_00479143();
	// !DECL 0x00479143 END
	// !DECL 0x0047914e BEGIN
	/* 47914E */ uchar Unwind_0047914e();
	// !DECL 0x0047914e END
	// !DECL 0x00479159 BEGIN
	/* 479159 */ uchar Unwind_00479159();
	// !DECL 0x00479159 END
	// !DECL 0x00479164 BEGIN
	/* 479164 */ uchar Unwind_00479164();
	// !DECL 0x00479164 END
	// !DECL 0x00479172 BEGIN
	/* 479172 */ uchar Unwind_00479172();
	// !DECL 0x00479172 END
	// !DECL 0x004791a0 BEGIN
	/* 4791A0 */ uchar Unwind_004791a0();
	// !DECL 0x004791a0 END
	// !DECL 0x004791a8 BEGIN
	/* 4791A8 */ uchar Unwind_004791a8();
	// !DECL 0x004791a8 END
	// !DECL 0x004791b3 BEGIN
	/* 4791B3 */ uchar Unwind_004791b3();
	// !DECL 0x004791b3 END
	// !DECL 0x004791be BEGIN
	/* 4791BE */ uchar Unwind_004791be();
	// !DECL 0x004791be END
	// !DECL 0x004791c9 BEGIN
	/* 4791C9 */ uchar Unwind_004791c9();
	// !DECL 0x004791c9 END
	// !DECL 0x004791d4 BEGIN
	/* 4791D4 */ uchar Unwind_004791d4();
	// !DECL 0x004791d4 END
	// !DECL 0x004791e2 BEGIN
	/* 4791E2 */ uchar Unwind_004791e2();
	// !DECL 0x004791e2 END
	// !DECL 0x004791f0 BEGIN
	/* 4791F0 */ uchar Unwind_004791f0();
	// !DECL 0x004791f0 END
	// !DECL 0x004791fe BEGIN
	/* 4791FE */ uchar Unwind_004791fe();
	// !DECL 0x004791fe END
	// !DECL 0x0047920c BEGIN
	/* 47920C */ uchar Unwind_0047920c();
	// !DECL 0x0047920c END
	// !DECL 0x0047921a BEGIN
	/* 47921A */ uchar Unwind_0047921a();
	// !DECL 0x0047921a END
	// !DECL 0x00479240 BEGIN
	/* 479240 */ uchar Unwind_00479240();
	// !DECL 0x00479240 END
	// !DECL 0x00479248 BEGIN
	/* 479248 */ uchar Unwind_00479248();
	// !DECL 0x00479248 END
	// !DECL 0x00479253 BEGIN
	/* 479253 */ uchar Unwind_00479253();
	// !DECL 0x00479253 END
	// !DECL 0x0047925e BEGIN
	/* 47925E */ uchar Unwind_0047925e();
	// !DECL 0x0047925e END
	// !DECL 0x00479269 BEGIN
	/* 479269 */ uchar Unwind_00479269();
	// !DECL 0x00479269 END
	// !DECL 0x00479274 BEGIN
	/* 479274 */ uchar Unwind_00479274();
	// !DECL 0x00479274 END
	// !DECL 0x00479282 BEGIN
	/* 479282 */ uchar Unwind_00479282();
	// !DECL 0x00479282 END
	// !DECL 0x00479290 BEGIN
	/* 479290 */ uchar Unwind_00479290();
	// !DECL 0x00479290 END
	// !DECL 0x0047929e BEGIN
	/* 47929E */ uchar Unwind_0047929e();
	// !DECL 0x0047929e END
	// !DECL 0x004792ac BEGIN
	/* 4792AC */ uchar Unwind_004792ac();
	// !DECL 0x004792ac END
	// !DECL 0x004792ba BEGIN
	/* 4792BA */ uchar Unwind_004792ba();
	// !DECL 0x004792ba END
	// !DECL 0x004792e0 BEGIN
	/* 4792E0 */ uchar Unwind_004792e0();
	// !DECL 0x004792e0 END
	// !DECL 0x00479310 BEGIN
	/* 479310 */ uchar Unwind_00479310();
	// !DECL 0x00479310 END
	// !DECL 0x00479340 BEGIN
	/* 479340 */ uchar Unwind_00479340();
	// !DECL 0x00479340 END
	// !DECL 0x00479348 BEGIN
	/* 479348 */ uchar Unwind_00479348();
	// !DECL 0x00479348 END
	// !DECL 0x0047936f BEGIN
	/* 47936F */ uchar Unwind_0047936f();
	// !DECL 0x0047936f END
	// !DECL 0x004793a0 BEGIN
	/* 4793A0 */ uchar Unwind_004793a0();
	// !DECL 0x004793a0 END
	// !DECL 0x004793d0 BEGIN
	/* 4793D0 */ uchar Unwind_004793d0();
	// !DECL 0x004793d0 END
	// !DECL 0x00479400 BEGIN
	/* 479400 */ uchar Unwind_00479400();
	// !DECL 0x00479400 END
	// !DECL 0x00479450 BEGIN
	/* 479450 */ uchar Unwind_00479450();
	// !DECL 0x00479450 END
	// !DECL 0x00479458 BEGIN
	/* 479458 */ uchar Unwind_00479458();
	// !DECL 0x00479458 END
	// !DECL 0x00479460 BEGIN
	/* 479460 */ uchar Unwind_00479460();
	// !DECL 0x00479460 END
	// !DECL 0x00479468 BEGIN
	/* 479468 */ uchar Unwind_00479468();
	// !DECL 0x00479468 END
	// !DECL 0x00479470 BEGIN
	/* 479470 */ uchar Unwind_00479470();
	// !DECL 0x00479470 END
	// !DECL 0x00479478 BEGIN
	/* 479478 */ uchar Unwind_00479478();
	// !DECL 0x00479478 END
	// !DECL 0x004794a0 BEGIN
	/* 4794A0 */ uchar Unwind_004794a0();
	// !DECL 0x004794a0 END
	// !DECL 0x004794a8 BEGIN
	/* 4794A8 */ uchar Unwind_004794a8();
	// !DECL 0x004794a8 END
	// !DECL 0x004794d0 BEGIN
	/* 4794D0 */ uchar Unwind_004794d0();
	// !DECL 0x004794d0 END
	// !DECL 0x004794d8 BEGIN
	/* 4794D8 */ uchar Unwind_004794d8();
	// !DECL 0x004794d8 END
	// !DECL 0x00479500 BEGIN
	/* 479500 */ uchar Unwind_00479500();
	// !DECL 0x00479500 END
	// !DECL 0x00479540 BEGIN
	/* 479540 */ uchar Unwind_00479540();
	// !DECL 0x00479540 END
	// !DECL 0x00479570 BEGIN
	/* 479570 */ uchar Unwind_00479570();
	// !DECL 0x00479570 END
	// !DECL 0x00479578 BEGIN
	/* 479578 */ uchar Unwind_00479578();
	// !DECL 0x00479578 END
	// !DECL 0x004795a0 BEGIN
	/* 4795A0 */ uchar Unwind_004795a0();
	// !DECL 0x004795a0 END
	// !DECL 0x004795ab BEGIN
	/* 4795AB */ uchar Unwind_004795ab();
	// !DECL 0x004795ab END
	// !DECL 0x004795e0 BEGIN
	/* 4795E0 */ uchar Unwind_004795e0();
	// !DECL 0x004795e0 END
	// !DECL 0x00479610 BEGIN
	/* 479610 */ uchar Unwind_00479610();
	// !DECL 0x00479610 END
	// !DECL 0x0047961b BEGIN
	/* 47961B */ uchar Unwind_0047961b();
	// !DECL 0x0047961b END
	// !DECL 0x00479650 BEGIN
	/* 479650 */ uchar Unwind_00479650();
	// !DECL 0x00479650 END
	// !DECL 0x00479680 BEGIN
	/* 479680 */ uchar Unwind_00479680();
	// !DECL 0x00479680 END
	// !DECL 0x0047968b BEGIN
	/* 47968B */ uchar Unwind_0047968b();
	// !DECL 0x0047968b END
	// !DECL 0x004796c0 BEGIN
	/* 4796C0 */ uchar Unwind_004796c0();
	// !DECL 0x004796c0 END
	// !DECL 0x004796cb BEGIN
	/* 4796CB */ uchar Unwind_004796cb();
	// !DECL 0x004796cb END
	// !DECL 0x00479720 BEGIN
	/* 479720 */ uchar Unwind_00479720();
	// !DECL 0x00479720 END
	// !DECL 0x00479770 BEGIN
	/* 479770 */ uchar Unwind_00479770();
	// !DECL 0x00479770 END
	// !DECL 0x00479778 BEGIN
	/* 479778 */ uchar Unwind_00479778();
	// !DECL 0x00479778 END
	// !DECL 0x00479783 BEGIN
	/* 479783 */ uchar Unwind_00479783();
	// !DECL 0x00479783 END
	// !DECL 0x004797b0 BEGIN
	/* 4797B0 */ uchar Unwind_004797b0();
	// !DECL 0x004797b0 END
	// !DECL 0x004797b8 BEGIN
	/* 4797B8 */ uchar Unwind_004797b8();
	// !DECL 0x004797b8 END
	// !DECL 0x004797c3 BEGIN
	/* 4797C3 */ uchar Unwind_004797c3();
	// !DECL 0x004797c3 END
	// !DECL 0x004797ce BEGIN
	/* 4797CE */ uchar Unwind_004797ce();
	// !DECL 0x004797ce END
	// !DECL 0x004797d9 BEGIN
	/* 4797D9 */ uchar Unwind_004797d9();
	// !DECL 0x004797d9 END
	// !DECL 0x00479800 BEGIN
	/* 479800 */ uchar Unwind_00479800();
	// !DECL 0x00479800 END
	// !DECL 0x00479808 BEGIN
	/* 479808 */ uchar Unwind_00479808();
	// !DECL 0x00479808 END
	// !DECL 0x00479813 BEGIN
	/* 479813 */ uchar Unwind_00479813();
	// !DECL 0x00479813 END
	// !DECL 0x0047981e BEGIN
	/* 47981E */ uchar Unwind_0047981e();
	// !DECL 0x0047981e END
	// !DECL 0x00479829 BEGIN
	/* 479829 */ uchar Unwind_00479829();
	// !DECL 0x00479829 END
	// !DECL 0x00479850 BEGIN
	/* 479850 */ uchar Unwind_00479850();
	// !DECL 0x00479850 END
	// !DECL 0x00479880 BEGIN
	/* 479880 */ uchar Unwind_00479880();
	// !DECL 0x00479880 END
	// !DECL 0x004798b0 BEGIN
	/* 4798B0 */ uchar Unwind_004798b0();
	// !DECL 0x004798b0 END
	// !DECL 0x004798e0 BEGIN
	/* 4798E0 */ uchar Unwind_004798e0();
	// !DECL 0x004798e0 END
	// !DECL 0x00479910 BEGIN
	/* 479910 */ uchar Unwind_00479910();
	// !DECL 0x00479910 END
	// !DECL 0x00479940 BEGIN
	/* 479940 */ uchar Unwind_00479940();
	// !DECL 0x00479940 END
	// !DECL 0x00479948 BEGIN
	/* 479948 */ uchar Unwind_00479948();
	// !DECL 0x00479948 END
	// !DECL 0x00479970 BEGIN
	/* 479970 */ uchar Unwind_00479970();
	// !DECL 0x00479970 END
	// !DECL 0x00479978 BEGIN
	/* 479978 */ uchar Unwind_00479978();
	// !DECL 0x00479978 END
	// !DECL 0x004799a0 BEGIN
	/* 4799A0 */ uchar Unwind_004799a0();
	// !DECL 0x004799a0 END
	// !DECL 0x004799ab BEGIN
	/* 4799AB */ uchar Unwind_004799ab();
	// !DECL 0x004799ab END
	// !DECL 0x004799d0 BEGIN
	/* 4799D0 */ uchar Unwind_004799d0();
	// !DECL 0x004799d0 END
	// !DECL 0x004799d8 BEGIN
	/* 4799D8 */ uchar Unwind_004799d8();
	// !DECL 0x004799d8 END
	// !DECL 0x00479a00 BEGIN
	/* 479A00 */ uchar Unwind_00479a00();
	// !DECL 0x00479a00 END
	// !DECL 0x00479ac0 BEGIN
	/* 479AC0 */ uchar Unwind_00479ac0();
	// !DECL 0x00479ac0 END
	// !DECL 0x00479ac8 BEGIN
	/* 479AC8 */ uchar Unwind_00479ac8();
	// !DECL 0x00479ac8 END
	// !DECL 0x00479b10 BEGIN
	/* 479B10 */ uchar Unwind_00479b10();
	// !DECL 0x00479b10 END
	// !DECL 0x00479b50 BEGIN
	/* 479B50 */ uchar Unwind_00479b50();
	// !DECL 0x00479b50 END
	// !DECL 0x00479b80 BEGIN
	/* 479B80 */ uchar Unwind_00479b80();
	// !DECL 0x00479b80 END
	// !DECL 0x00479b88 BEGIN
	/* 479B88 */ uchar Unwind_00479b88();
	// !DECL 0x00479b88 END
	// !DECL 0x00479bd0 BEGIN
	/* 479BD0 */ uchar Unwind_00479bd0();
	// !DECL 0x00479bd0 END
	// !DECL 0x00479bd8 BEGIN
	/* 479BD8 */ uchar Unwind_00479bd8();
	// !DECL 0x00479bd8 END
	// !DECL 0x00479be3 BEGIN
	/* 479BE3 */ uchar Unwind_00479be3();
	// !DECL 0x00479be3 END
	// !DECL 0x00479c10 BEGIN
	/* 479C10 */ uchar Unwind_00479c10();
	// !DECL 0x00479c10 END
	// !DECL 0x00479c18 BEGIN
	/* 479C18 */ uchar Unwind_00479c18();
	// !DECL 0x00479c18 END
	// !DECL 0x00479c3f BEGIN
	/* 479C3F */ uchar Unwind_00479c3f();
	// !DECL 0x00479c3f END
	// !DECL 0x00479c70 BEGIN
	/* 479C70 */ uchar Unwind_00479c70();
	// !DECL 0x00479c70 END
	// !DECL 0x00479ca0 BEGIN
	/* 479CA0 */ uchar Unwind_00479ca0();
	// !DECL 0x00479ca0 END
	// !DECL 0x00479ca8 BEGIN
	/* 479CA8 */ uchar Unwind_00479ca8();
	// !DECL 0x00479ca8 END
	// !DECL 0x00479cf0 BEGIN
	/* 479CF0 */ uchar Unwind_00479cf0();
	// !DECL 0x00479cf0 END
	// !DECL 0x00479d20 BEGIN
	/* 479D20 */ uchar Unwind_00479d20();
	// !DECL 0x00479d20 END
	// !DECL 0x00479d28 BEGIN
	/* 479D28 */ uchar Unwind_00479d28();
	// !DECL 0x00479d28 END
	// !DECL 0x00479d50 BEGIN
	/* 479D50 */ uchar Unwind_00479d50();
	// !DECL 0x00479d50 END
	// !DECL 0x00479d80 BEGIN
	/* 479D80 */ uchar Unwind_00479d80();
	// !DECL 0x00479d80 END
	// !DECL 0x00479db0 BEGIN
	/* 479DB0 */ uchar Unwind_00479db0();
	// !DECL 0x00479db0 END
	// !DECL 0x00479de0 BEGIN
	/* 479DE0 */ uchar Unwind_00479de0();
	// !DECL 0x00479de0 END
	// !DECL 0x00479de8 BEGIN
	/* 479DE8 */ uchar Unwind_00479de8();
	// !DECL 0x00479de8 END
	// !DECL 0x00479df3 BEGIN
	/* 479DF3 */ uchar Unwind_00479df3();
	// !DECL 0x00479df3 END
	// !DECL 0x00479dfe BEGIN
	/* 479DFE */ uchar Unwind_00479dfe();
	// !DECL 0x00479dfe END
	// !DECL 0x00479e30 BEGIN
	/* 479E30 */ uchar Unwind_00479e30();
	// !DECL 0x00479e30 END
	// !DECL 0x00479e38 BEGIN
	/* 479E38 */ uchar Unwind_00479e38();
	// !DECL 0x00479e38 END
	// !DECL 0x00479e43 BEGIN
	/* 479E43 */ uchar Unwind_00479e43();
	// !DECL 0x00479e43 END
	// !DECL 0x00479e4e BEGIN
	/* 479E4E */ uchar Unwind_00479e4e();
	// !DECL 0x00479e4e END
	// !DECL 0x00479e80 BEGIN
	/* 479E80 */ uchar Unwind_00479e80();
	// !DECL 0x00479e80 END
	// !DECL 0x00479eb0 BEGIN
	/* 479EB0 */ uchar Unwind_00479eb0();
	// !DECL 0x00479eb0 END
	// !DECL 0x00479eb8 BEGIN
	/* 479EB8 */ uchar Unwind_00479eb8();
	// !DECL 0x00479eb8 END
	// !DECL 0x00479ec3 BEGIN
	/* 479EC3 */ uchar Unwind_00479ec3();
	// !DECL 0x00479ec3 END
	// !DECL 0x00479ece BEGIN
	/* 479ECE */ uchar Unwind_00479ece();
	// !DECL 0x00479ece END
	// !DECL 0x00479ed9 BEGIN
	/* 479ED9 */ uchar Unwind_00479ed9();
	// !DECL 0x00479ed9 END
	// !DECL 0x00479f00 BEGIN
	/* 479F00 */ uchar Unwind_00479f00();
	// !DECL 0x00479f00 END
	// !DECL 0x00479f30 BEGIN
	/* 479F30 */ uchar Unwind_00479f30();
	// !DECL 0x00479f30 END
	// !DECL 0x00479f60 BEGIN
	/* 479F60 */ uchar Unwind_00479f60();
	// !DECL 0x00479f60 END
	// !DECL 0x00479f90 BEGIN
	/* 479F90 */ uchar Unwind_00479f90();
	// !DECL 0x00479f90 END
	// !DECL 0x00479fc0 BEGIN
	/* 479FC0 */ uchar Unwind_00479fc0();
	// !DECL 0x00479fc0 END
	// !DECL 0x00479fc8 BEGIN
	/* 479FC8 */ uchar Unwind_00479fc8();
	// !DECL 0x00479fc8 END
	// !DECL 0x00479ff0 BEGIN
	/* 479FF0 */ uchar Unwind_00479ff0();
	// !DECL 0x00479ff0 END
	// !DECL 0x00479ff8 BEGIN
	/* 479FF8 */ uchar Unwind_00479ff8();
	// !DECL 0x00479ff8 END
	// !DECL 0x0047a01f BEGIN
	/* 47A01F */ uchar Unwind_0047a01f();
	// !DECL 0x0047a01f END
	// !DECL 0x0047a050 BEGIN
	/* 47A050 */ uchar Unwind_0047a050();
	// !DECL 0x0047a050 END
	// !DECL 0x0047a080 BEGIN
	/* 47A080 */ uchar Unwind_0047a080();
	// !DECL 0x0047a080 END
	// !DECL 0x0047a08b BEGIN
	/* 47A08B */ uchar Unwind_0047a08b();
	// !DECL 0x0047a08b END
	// !DECL 0x0047a093 BEGIN
	/* 47A093 */ uchar Unwind_0047a093();
	// !DECL 0x0047a093 END
	// !DECL 0x0047a09e BEGIN
	/* 47A09E */ uchar Unwind_0047a09e();
	// !DECL 0x0047a09e END
	// !DECL 0x0047a0d0 BEGIN
	/* 47A0D0 */ uchar Unwind_0047a0d0();
	// !DECL 0x0047a0d0 END
	// !DECL 0x0047a100 BEGIN
	/* 47A100 */ uchar Unwind_0047a100();
	// !DECL 0x0047a100 END
	// !DECL 0x0047a130 BEGIN
	/* 47A130 */ uchar Unwind_0047a130();
	// !DECL 0x0047a130 END
	// !DECL 0x0047a160 BEGIN
	/* 47A160 */ uchar Unwind_0047a160();
	// !DECL 0x0047a160 END
	// !DECL 0x0047a190 BEGIN
	/* 47A190 */ uchar Unwind_0047a190();
	// !DECL 0x0047a190 END
	// !DECL 0x0047a1c0 BEGIN
	/* 47A1C0 */ uchar Unwind_0047a1c0();
	// !DECL 0x0047a1c0 END
	// !DECL 0x0047a1f0 BEGIN
	/* 47A1F0 */ uchar Unwind_0047a1f0();
	// !DECL 0x0047a1f0 END
	// !DECL 0x0047a1f8 BEGIN
	/* 47A1F8 */ uchar Unwind_0047a1f8();
	// !DECL 0x0047a1f8 END
	// !DECL 0x0047a203 BEGIN
	/* 47A203 */ uchar Unwind_0047a203();
	// !DECL 0x0047a203 END
	// !DECL 0x0047a20e BEGIN
	/* 47A20E */ uchar Unwind_0047a20e();
	// !DECL 0x0047a20e END
	// !DECL 0x0047a219 BEGIN
	/* 47A219 */ uchar Unwind_0047a219();
	// !DECL 0x0047a219 END
	// !DECL 0x0047a240 BEGIN
	/* 47A240 */ uchar Unwind_0047a240();
	// !DECL 0x0047a240 END
	// !DECL 0x0047a248 BEGIN
	/* 47A248 */ uchar Unwind_0047a248();
	// !DECL 0x0047a248 END
	// !DECL 0x0047a253 BEGIN
	/* 47A253 */ uchar Unwind_0047a253();
	// !DECL 0x0047a253 END
	// !DECL 0x0047a25e BEGIN
	/* 47A25E */ uchar Unwind_0047a25e();
	// !DECL 0x0047a25e END
	// !DECL 0x0047a269 BEGIN
	/* 47A269 */ uchar Unwind_0047a269();
	// !DECL 0x0047a269 END
	// !DECL 0x0047a290 BEGIN
	/* 47A290 */ uchar Unwind_0047a290();
	// !DECL 0x0047a290 END
	// !DECL 0x0047a2c0 BEGIN
	/* 47A2C0 */ uchar Unwind_0047a2c0();
	// !DECL 0x0047a2c0 END
	// !DECL 0x0047a2f0 BEGIN
	/* 47A2F0 */ uchar Unwind_0047a2f0();
	// !DECL 0x0047a2f0 END
	// !DECL 0x0047a330 BEGIN
	/* 47A330 */ uchar Unwind_0047a330();
	// !DECL 0x0047a330 END
	// !DECL 0x0047a338 BEGIN
	/* 47A338 */ uchar Unwind_0047a338();
	// !DECL 0x0047a338 END
	// !DECL 0x0047a360 BEGIN
	/* 47A360 */ uchar Unwind_0047a360();
	// !DECL 0x0047a360 END
	// !DECL 0x0047a368 BEGIN
	/* 47A368 */ uchar Unwind_0047a368();
	// !DECL 0x0047a368 END
	// !DECL 0x0047a370 BEGIN
	/* 47A370 */ uchar Unwind_0047a370();
	// !DECL 0x0047a370 END
	// !DECL 0x0047a37b BEGIN
	/* 47A37B */ uchar Unwind_0047a37b();
	// !DECL 0x0047a37b END
	// !DECL 0x0047a3b0 BEGIN
	/* 47A3B0 */ uchar Unwind_0047a3b0();
	// !DECL 0x0047a3b0 END
	// !DECL 0x0047a3e0 BEGIN
	/* 47A3E0 */ uchar Unwind_0047a3e0();
	// !DECL 0x0047a3e0 END
	// !DECL 0x0047a3e8 BEGIN
	/* 47A3E8 */ uchar Unwind_0047a3e8();
	// !DECL 0x0047a3e8 END
	// !DECL 0x0047a410 BEGIN
	/* 47A410 */ uchar Unwind_0047a410();
	// !DECL 0x0047a410 END
	// !DECL 0x0047a440 BEGIN
	/* 47A440 */ uchar Unwind_0047a440();
	// !DECL 0x0047a440 END
	// !DECL 0x0047a470 BEGIN
	/* 47A470 */ uchar Unwind_0047a470();
	// !DECL 0x0047a470 END
	// !DECL 0x0047a4a0 BEGIN
	/* 47A4A0 */ uchar Unwind_0047a4a0();
	// !DECL 0x0047a4a0 END
	// !DECL 0x0047a4d0 BEGIN
	/* 47A4D0 */ uchar Unwind_0047a4d0();
	// !DECL 0x0047a4d0 END
	// !DECL 0x0047a500 BEGIN
	/* 47A500 */ uchar Unwind_0047a500();
	// !DECL 0x0047a500 END
	// !DECL 0x0047a540 BEGIN
	/* 47A540 */ uchar Unwind_0047a540();
	// !DECL 0x0047a540 END
	// !DECL 0x0047a548 BEGIN
	/* 47A548 */ uchar Unwind_0047a548();
	// !DECL 0x0047a548 END
	// !DECL 0x0047a570 BEGIN
	/* 47A570 */ uchar Unwind_0047a570();
	// !DECL 0x0047a570 END
	// !DECL 0x0047a5d0 BEGIN
	/* 47A5D0 */ uchar Unwind_0047a5d0();
	// !DECL 0x0047a5d0 END
	// !DECL 0x0047a600 BEGIN
	/* 47A600 */ uchar Unwind_0047a600();
	// !DECL 0x0047a600 END
	// !DECL 0x0047a630 BEGIN
	/* 47A630 */ uchar Unwind_0047a630();
	// !DECL 0x0047a630 END
	// !DECL 0x0047a638 BEGIN
	/* 47A638 */ uchar Unwind_0047a638();
	// !DECL 0x0047a638 END
	// !DECL 0x0047a660 BEGIN
	/* 47A660 */ uchar Unwind_0047a660();
	// !DECL 0x0047a660 END
	// !DECL 0x0047a668 BEGIN
	/* 47A668 */ uchar Unwind_0047a668();
	// !DECL 0x0047a668 END
	// !DECL 0x0047a690 BEGIN
	/* 47A690 */ uchar Unwind_0047a690();
	// !DECL 0x0047a690 END
	// !DECL 0x0047a698 BEGIN
	/* 47A698 */ uchar Unwind_0047a698();
	// !DECL 0x0047a698 END
	// !DECL 0x0047a6c0 BEGIN
	/* 47A6C0 */ uchar Unwind_0047a6c0();
	// !DECL 0x0047a6c0 END
	// !DECL 0x0047a6f0 BEGIN
	/* 47A6F0 */ uchar Unwind_0047a6f0();
	// !DECL 0x0047a6f0 END
	// !DECL 0x0047a720 BEGIN
	/* 47A720 */ uchar Unwind_0047a720();
	// !DECL 0x0047a720 END
	// !DECL 0x0047a750 BEGIN
	/* 47A750 */ uchar Unwind_0047a750();
	// !DECL 0x0047a750 END
	// !DECL 0x0047a758 BEGIN
	/* 47A758 */ uchar Unwind_0047a758();
	// !DECL 0x0047a758 END
	// !DECL 0x0047a763 BEGIN
	/* 47A763 */ uchar Unwind_0047a763();
	// !DECL 0x0047a763 END
	// !DECL 0x0047a76e BEGIN
	/* 47A76E */ uchar Unwind_0047a76e();
	// !DECL 0x0047a76e END
	// !DECL 0x0047a7a0 BEGIN
	/* 47A7A0 */ uchar Unwind_0047a7a0();
	// !DECL 0x0047a7a0 END
	// !DECL 0x0047a7a8 BEGIN
	/* 47A7A8 */ uchar Unwind_0047a7a8();
	// !DECL 0x0047a7a8 END
	// !DECL 0x0047a7cf BEGIN
	/* 47A7CF */ uchar Unwind_0047a7cf();
	// !DECL 0x0047a7cf END
	// !DECL 0x0047a7da BEGIN
	/* 47A7DA */ uchar Unwind_0047a7da();
	// !DECL 0x0047a7da END
	// !DECL 0x0047a820 BEGIN
	/* 47A820 */ uchar Unwind_0047a820();
	// !DECL 0x0047a820 END
	// !DECL 0x0047a870 BEGIN
	/* 47A870 */ uchar Unwind_0047a870();
	// !DECL 0x0047a870 END
	// !DECL 0x0047a878 BEGIN
	/* 47A878 */ uchar Unwind_0047a878();
	// !DECL 0x0047a878 END
	// !DECL 0x0047a883 BEGIN
	/* 47A883 */ uchar Unwind_0047a883();
	// !DECL 0x0047a883 END
	// !DECL 0x0047a8b0 BEGIN
	/* 47A8B0 */ uchar Unwind_0047a8b0();
	// !DECL 0x0047a8b0 END
	// !DECL 0x0047a900 BEGIN
	/* 47A900 */ uchar Unwind_0047a900();
	// !DECL 0x0047a900 END
	// !DECL 0x0047a908 BEGIN
	/* 47A908 */ uchar Unwind_0047a908();
	// !DECL 0x0047a908 END
	// !DECL 0x0047a913 BEGIN
	/* 47A913 */ uchar Unwind_0047a913();
	// !DECL 0x0047a913 END
	// !DECL 0x0047a91e BEGIN
	/* 47A91E */ uchar Unwind_0047a91e();
	// !DECL 0x0047a91e END
	// !DECL 0x0047a950 BEGIN
	/* 47A950 */ uchar Unwind_0047a950();
	// !DECL 0x0047a950 END
	// !DECL 0x0047a958 BEGIN
	/* 47A958 */ uchar Unwind_0047a958();
	// !DECL 0x0047a958 END
	// !DECL 0x0047a97f BEGIN
	/* 47A97F */ uchar Unwind_0047a97f();
	// !DECL 0x0047a97f END
	// !DECL 0x0047a98a BEGIN
	/* 47A98A */ uchar Unwind_0047a98a();
	// !DECL 0x0047a98a END
	// !DECL 0x0047a9b0 BEGIN
	/* 47A9B0 */ uchar Unwind_0047a9b0();
	// !DECL 0x0047a9b0 END
	// !DECL 0x0047a9e0 BEGIN
	/* 47A9E0 */ uchar Unwind_0047a9e0();
	// !DECL 0x0047a9e0 END
	// !DECL 0x0047a9e8 BEGIN
	/* 47A9E8 */ uchar Unwind_0047a9e8();
	// !DECL 0x0047a9e8 END
	// !DECL 0x0047aa10 BEGIN
	/* 47AA10 */ uchar Unwind_0047aa10();
	// !DECL 0x0047aa10 END
	// !DECL 0x0047aa18 BEGIN
	/* 47AA18 */ uchar Unwind_0047aa18();
	// !DECL 0x0047aa18 END
	// !DECL 0x0047aa23 BEGIN
	/* 47AA23 */ uchar Unwind_0047aa23();
	// !DECL 0x0047aa23 END
	// !DECL 0x0047aa50 BEGIN
	/* 47AA50 */ uchar Unwind_0047aa50();
	// !DECL 0x0047aa50 END
	// !DECL 0x0047aa80 BEGIN
	/* 47AA80 */ uchar Unwind_0047aa80();
	// !DECL 0x0047aa80 END
	// !DECL 0x0047aab0 BEGIN
	/* 47AAB0 */ uchar Unwind_0047aab0();
	// !DECL 0x0047aab0 END
	// !DECL 0x0047aae0 BEGIN
	/* 47AAE0 */ uchar Unwind_0047aae0();
	// !DECL 0x0047aae0 END
	// !DECL 0x0047ab10 BEGIN
	/* 47AB10 */ uchar Unwind_0047ab10();
	// !DECL 0x0047ab10 END
	// !DECL 0x0047ab18 BEGIN
	/* 47AB18 */ uchar Unwind_0047ab18();
	// !DECL 0x0047ab18 END
	// !DECL 0x0047ab23 BEGIN
	/* 47AB23 */ uchar Unwind_0047ab23();
	// !DECL 0x0047ab23 END
	// !DECL 0x0047ab50 BEGIN
	/* 47AB50 */ uchar Unwind_0047ab50();
	// !DECL 0x0047ab50 END
	// !DECL 0x0047ab58 BEGIN
	/* 47AB58 */ uchar Unwind_0047ab58();
	// !DECL 0x0047ab58 END
	// !DECL 0x0047ab63 BEGIN
	/* 47AB63 */ uchar Unwind_0047ab63();
	// !DECL 0x0047ab63 END
	// !DECL 0x0047ab6e BEGIN
	/* 47AB6E */ uchar Unwind_0047ab6e();
	// !DECL 0x0047ab6e END
	// !DECL 0x0047aba0 BEGIN
	/* 47ABA0 */ uchar Unwind_0047aba0();
	// !DECL 0x0047aba0 END
	// !DECL 0x0047ac30 BEGIN
	/* 47AC30 */ uchar Unwind_0047ac30();
	// !DECL 0x0047ac30 END
	// !DECL 0x0047ac38 BEGIN
	/* 47AC38 */ uchar Unwind_0047ac38();
	// !DECL 0x0047ac38 END
	// !DECL 0x0047ac60 BEGIN
	/* 47AC60 */ uchar Unwind_0047ac60();
	// !DECL 0x0047ac60 END
	// !DECL 0x0047acd0 BEGIN
	/* 47ACD0 */ uchar Unwind_0047acd0();
	// !DECL 0x0047acd0 END
	// !DECL 0x0047acd8 BEGIN
	/* 47ACD8 */ uchar Unwind_0047acd8();
	// !DECL 0x0047acd8 END
	// !DECL 0x0047ace3 BEGIN
	/* 47ACE3 */ uchar Unwind_0047ace3();
	// !DECL 0x0047ace3 END
	// !DECL 0x0047acee BEGIN
	/* 47ACEE */ uchar Unwind_0047acee();
	// !DECL 0x0047acee END
	// !DECL 0x0047ad20 BEGIN
	/* 47AD20 */ uchar Unwind_0047ad20();
	// !DECL 0x0047ad20 END
	// !DECL 0x0047ad28 BEGIN
	/* 47AD28 */ uchar Unwind_0047ad28();
	// !DECL 0x0047ad28 END
	// !DECL 0x0047ad4f BEGIN
	/* 47AD4F */ uchar Unwind_0047ad4f();
	// !DECL 0x0047ad4f END
	// !DECL 0x0047ad76 BEGIN
	/* 47AD76 */ uchar Unwind_0047ad76();
	// !DECL 0x0047ad76 END
	// !DECL 0x0047adc0 BEGIN
	/* 47ADC0 */ uchar Unwind_0047adc0();
	// !DECL 0x0047adc0 END
	// !DECL 0x0047adf0 BEGIN
	/* 47ADF0 */ uchar Unwind_0047adf0();
	// !DECL 0x0047adf0 END
	// !DECL 0x0047ae20 BEGIN
	/* 47AE20 */ uchar Unwind_0047ae20();
	// !DECL 0x0047ae20 END
	// !DECL 0x0047ae28 BEGIN
	/* 47AE28 */ uchar Unwind_0047ae28();
	// !DECL 0x0047ae28 END
	// !DECL 0x0047ae60 BEGIN
	/* 47AE60 */ uchar Unwind_0047ae60();
	// !DECL 0x0047ae60 END
	// !DECL 0x0047ae68 BEGIN
	/* 47AE68 */ uchar Unwind_0047ae68();
	// !DECL 0x0047ae68 END
	// !DECL 0x0047ae76 BEGIN
	/* 47AE76 */ uchar Unwind_0047ae76();
	// !DECL 0x0047ae76 END
	// !DECL 0x0047aea0 BEGIN
	/* 47AEA0 */ uchar Unwind_0047aea0();
	// !DECL 0x0047aea0 END
	// !DECL 0x0047aed0 BEGIN
	/* 47AED0 */ uchar Unwind_0047aed0();
	// !DECL 0x0047aed0 END
	// !DECL 0x0047af00 BEGIN
	/* 47AF00 */ uchar Unwind_0047af00();
	// !DECL 0x0047af00 END
	// !DECL 0x0047af30 BEGIN
	/* 47AF30 */ uchar Unwind_0047af30();
	// !DECL 0x0047af30 END
	// !DECL 0x0047af60 BEGIN
	/* 47AF60 */ uchar Unwind_0047af60();
	// !DECL 0x0047af60 END
	// !DECL 0x0047af68 BEGIN
	/* 47AF68 */ uchar Unwind_0047af68();
	// !DECL 0x0047af68 END
	// !DECL 0x0047afb0 BEGIN
	/* 47AFB0 */ uchar Unwind_0047afb0();
	// !DECL 0x0047afb0 END
	// !DECL 0x0047aff0 BEGIN
	/* 47AFF0 */ uchar CBulanci_RegisterAppDescriptor();
	// !DECL 0x0047aff0 END

	// !DECL 0x0042f590 BEGIN
	/* 42F590 */ static uint CDSView_PostMessage_NullSafe(void* param_1, ushort param_2, ushort param_3, uint param_4, uint param_5);
	// !DECL 0x0042f590 END
	// !DECL 0x0043df90 BEGIN
	/* 43DF90 */ static uchar BlitOpaque_BGR24_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0043df90 END

	// !DECL 0x0042f390 BEGIN
	/* 42F390 */ uint CDSView_PostMessage(ushort param_1, ushort param_2, uint param_3, uint param_4);
	// !DECL 0x0042f390 END
};

#endif
