#include "_Globals.h"

// !PROLOGUE BEGIN
// Cross-unit types used in mapping.csv signatures (pointers only in stubs).
#include "bulanci_fwd.h"
// zlib::inflate*/deflate* are reached from the matched bodies at 0x434e30
// and 0x434ee0 (CDSGZipStream::Decompress / Compress).
#include "zlib.h"
// !PROLOGUE END

#include "Runtime.h"

// !FUNC 0x00401000 BEGIN
/* 401000-401017 00017 */
void _Globals::AtlThrow(HRESULT param_1) { STUB_BODY(); }
// !FUNC 0x00401000 END

// !FUNC 0x00401020 BEGIN
/* 401020-40103A 0001A */
void _Globals::ThrowWin32ErrorAsHRESULT() { STUB_BODY(); }
// !FUNC 0x00401020 END

// !FUNC 0x00401060 BEGIN
/* 401060-4010F0 00090 */
uchar _Globals::CDsString_EnsureCapacityWide(int* param_1, size_t param_2, void* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00401060 END

// !FUNC 0x004010f0 BEGIN
/* 4010F0-40118B 0009B */
int _Globals::CIntList_BinarySearch(int* param_1, uchar* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004010f0 END

// !FUNC 0x00401190 BEGIN
/* 401190-4011A9 00019 */
void* _Globals::CDsString_InitFromLiteral(short* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401190 END

// !FUNC 0x004011b0 BEGIN
/* 4011B0-4011C9 00019 */
void* _Globals::CDsString_InitFromHandle(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004011b0 END

// !FUNC 0x004011d0 BEGIN
/* 4011D0-4011D8 00008 */
void _Globals::eh_CDSStreamStorage_DeleteCriticalSection(void* param_1) { STUB_BODY(); }
// !FUNC 0x004011d0 END

// !FUNC 0x00401290 BEGIN
/* 401290-40133F 000AF */
uchar _Globals::MultiByteToWideChar_Wrapper(CHAR* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00401290 END

// !FUNC 0x00401340 BEGIN
/* 401340-40134F 0000F */
void _Globals::CDsString_EhVectorElemDtor(int* param_1) { STUB_BODY(); }
// !FUNC 0x00401340 END

// !FUNC 0x00401450 BEGIN
/* 401450-4014AC 0005C */
uchar _Globals::CBulanci_DirEnumCtx_dtor(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401450 END

// !FUNC 0x00401520 BEGIN
/* 401520-401532 00012 */
void _Globals::Runtime_HeapPtr_EH_dtor(uint* param_1) { STUB_BODY(); }
// !FUNC 0x00401520 END

// !FUNC 0x00401910 BEGIN
/* 401910-40196E 0005E */
uchar _Globals::CWindow_dtor(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401910 END


// !FUNC 0x00401a70 BEGIN
/* 401A70-401AEF 0007F */
uint _Globals::CDSException_TopLevelFilter(CDSException* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401a70 END

// !FUNC 0x00401d30 BEGIN
/* 401D30-401D6B 0003B */
uchar _Globals::CIntList_EnsureCapacity(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401d30 END

// !FUNC 0x00401d70 BEGIN
/* 401D70-401D78 00008 */
void _Globals::eh_CDSPtrSlotVec_ResizeZero(CDSPtrSlotVec* param_1) { STUB_BODY(); }
// !FUNC 0x00401d70 END

// !FUNC 0x00401fc3 BEGIN
/* 401FC3-401FEA 00027 */
uchar* _Globals::Catch_00401fc3() { STUB_BODY(); return 0; }
// !FUNC 0x00401fc3 END

// !FUNC 0x00402290 BEGIN
/* 402290-4023B6 00126 */
uchar _Globals::CDSApp_ShowSetupDialog(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00402290 END

// !FUNC 0x00402680 BEGIN
/* 402680-4026E4 00064 */
int _Globals::WinMain(HINSTANCE__* param_1, HINSTANCE__* param_2, CHAR* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00402680 END

// !FUNC 0x00402a90 BEGIN
/* 402A90-402AF7 00067 */
uint _Globals::CBulanci_CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x00402a90 END

// !FUNC 0x00402e20 BEGIN
/* 402E20-402E7A 0005A */
uchar _Globals::CScroller_DtorCommon(int param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00402e20 END

// !FUNC 0x00402f40 BEGIN
/* 402F40-402F60 00020 */
int _Globals::WidgetStateFlags_ToTintColor(ushort param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00402f40 END

// !FUNC 0x004031b0 BEGIN
/* 4031B0-403240 00090 */
uchar _Globals::CDsString_EnsureCapacityAnsi(int* param_1, size_t param_2, void* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x004031b0 END


// !FUNC 0x00404100 BEGIN
/* 404100-404110 00010 */
int _Globals::CListViewer_GetVerticalScrollbarHeight(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00404100 END

// !FUNC 0x00404320 BEGIN
/* 404320-40438A 0006A */
uchar _Globals::CListViewer_ScrollToItem(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00404320 END

// !FUNC 0x00404390 BEGIN
/* 404390-4044E3 00153 */
uchar _Globals::CListViewer_UpdateContentSize(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00404390 END


// !FUNC 0x004045f0 BEGIN
/* 4045F0-4046B2 000C2 */
uchar _Globals::CDsString_AssignFromWideCapped80(WCHAR* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004045f0 END

// !FUNC 0x004046c0 BEGIN
/* 4046C0-4046F5 00035 */
uchar _Globals::CDynPtrArray_RemoveRange(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004046c0 END

// !FUNC 0x00404700 BEGIN
/* 404700-40472C 0002C */
uchar _Globals::CDSRect_Assign(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00404700 END

// !FUNC 0x00404730 BEGIN
/* 404730-40478F 0005F */
uint _Globals::CDSRect_Overlaps(tagRECT* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00404730 END

// !FUNC 0x00404890 BEGIN
/* 404890-404909 00079 */
uchar _Globals::CDSView_UnwindDtor_StringVec9(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00404890 END

// !FUNC 0x00404be0 BEGIN
/* 404BE0-404C47 00067 */
uint _Globals::CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x00404be0 END



// !FUNC 0x00405280 BEGIN
/* 405280-4052D3 00053 */
int _Globals::FUN_00405280(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00405280 END

// !FUNC 0x00405370 BEGIN
/* 405370-40543A 000CA */
uchar _Globals::CStaticText_SetLabelFromStringHandle(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00405370 END

// !FUNC 0x00405bc0 BEGIN
/* 405BC0-405BE7 00027 */
uchar _Globals::CListViewer_ReindexItemsFrom(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00405bc0 END

// !FUNC 0x00405bf0 BEGIN
/* 405BF0-405C33 00043 */
uchar _Globals::CListViewer_ClearSelectedItems(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00405bf0 END

// !FUNC 0x00405c40 BEGIN
/* 405C40-405C97 00057 */
uchar _Globals::CScroller_ClearContent(CScroller* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00405c40 END

// !FUNC 0x00406300 BEGIN
/* 406300-406332 00032 */
uchar _Globals::CDynPtrArray_RemovePointer(int param_1, uchar* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00406300 END

// !FUNC 0x004063c0 BEGIN
/* 4063C0-406427 00067 */
uint _Globals::CreateObject_004063c0() { STUB_BODY(); return 0; }
// !FUNC 0x004063c0 END

// !FUNC 0x00406450 BEGIN
/* 406450-4064B7 00067 */
CStaticText* _Globals::CreateObject_00406450() { STUB_BODY(); return 0; }
// !FUNC 0x00406450 END

// !FUNC 0x00406570 BEGIN
/* 406570-4065D4 00064 */
CIcon* _Globals::CreateObject_00406570() { STUB_BODY(); return 0; }
// !FUNC 0x00406570 END

// !FUNC 0x00406600 BEGIN
/* 406600-406667 00067 */
CButton* _Globals::CreateObject_00406600() { STUB_BODY(); return 0; }
// !FUNC 0x00406600 END

// !FUNC 0x004066e0 BEGIN
/* 4066E0-406747 00067 */
uint _Globals::CreateObject_004066e0() { STUB_BODY(); return 0; }
// !FUNC 0x004066e0 END

// !FUNC 0x004067c0 BEGIN
/* 4067C0-406802 00042 */
uchar _Globals::CScrollBar_NudgeOnArrowHold(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004067c0 END

// !FUNC 0x00406810 BEGIN
/* 406810-406993 00183 */
uchar _Globals::CScrollBar_SetDragMode(uint param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00406810 END

// !FUNC 0x00406a20 BEGIN
/* 406A20-406A46 00026 */
uchar _Globals::CRadio_OnMouseMove(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00406a20 END

// !FUNC 0x00407020 BEGIN
/* 407020-407037 00017 */
uchar _Globals::CEdit_LayoutToCaretNoAnchor(CEdit* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00407020 END

// !FUNC 0x004070c0 BEGIN
/* 4070C0-407218 00158 */
uint _Globals::CEdit_SubmitText(WCHAR* param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004070c0 END

// !FUNC 0x004072b0 BEGIN
/* 4072B0-4072F2 00042 */
int _Globals::CIntList_InsertAt(uint param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004072b0 END


// !FUNC 0x00407710 BEGIN
/* 407710-40775F 0004F */
uchar _Globals::CListViewer_InsertItemAt(uint param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00407710 END

// !FUNC 0x00407e10 BEGIN
/* 407E10-407E1F 0000F */
void _Globals::CDSPtrSlotVec_EH_dtor(CDSPtrSlotVec* param_1) { STUB_BODY(); }
// !FUNC 0x00407e10 END

// !FUNC 0x00407e20 BEGIN
/* 407E20-407EBE 0009E */
int _Globals::CIntListInsertSortedOrAppend(int param_1, uchar* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00407e20 END

// !FUNC 0x00407ec0 BEGIN
/* 407EC0-407F27 00067 */
uint _Globals::CreateObject_00407ec0() { STUB_BODY(); return 0; }
// !FUNC 0x00407ec0 END

// !FUNC 0x004080d0 BEGIN
/* 4080D0-408198 000C8 */
uchar _Globals::CListViewer_SetItemSelected(int param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004080d0 END

// !FUNC 0x004082c0 BEGIN
/* 4082C0-408327 00067 */
uint* _Globals::CreateObject_004082c0() { STUB_BODY(); return 0; }
// !FUNC 0x004082c0 END

// !FUNC 0x00408ea0 BEGIN
/* 408EA0-408F07 00067 */
CLevelScript* _Globals::CreateObject_00408ea0() { STUB_BODY(); return 0; }
// !FUNC 0x00408ea0 END

// !FUNC 0x004090c0 BEGIN
/* 4090C0-4090FE 0003E */
uchar _Globals::CBulanci_ClearProfileKeyList(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004090c0 END

// !FUNC 0x00409110 BEGIN
/* 409110-40916C 0005C */
uchar _Globals::CDsString_WriteHandleToStream(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409110 END

// !FUNC 0x00409510 BEGIN
/* 409510-4095A7 00097 */
void* _Globals::CDSRegKey_ctor(HKEY__* param_1, int param_2, uchar* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00409510 END

// !FUNC 0x004095b0 BEGIN
/* 4095B0-409612 00062 */
uchar _Globals::CDSRegKeyQueryScope_dtor(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004095b0 END


// !FUNC 0x00409f20 BEGIN
/* 409F20-409F5D 0003D */
uchar _Globals::CBulanci_ClearLevelScriptList(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409f20 END

// !FUNC 0x0040a380 BEGIN
/* 40A380-40A43B 000BB */
void _Globals::CBulanci_DestroyConfigStore(CDSChain_full* param_1) { STUB_BODY(); }
// !FUNC 0x0040a380 END

// !FUNC 0x0040a7d0 BEGIN
/* 40A7D0-40A82A 0005A */
uchar _Globals::CDSAnim_CleanupBody(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040a7d0 END


// !FUNC 0x0040ad10 BEGIN
/* 40AD10-40AD18 00008 */
uchar _Globals::Unwind_CDSVideoPlayer_TM(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040ad10 END



// !FUNC 0x0040bb90 BEGIN
/* 40BB90-40BBEA 0005A */
uchar _Globals::CStartGame2_dtor(int param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0040bb90 END

// !FUNC 0x0040bce0 BEGIN
/* 40BCE0-40BD42 00062 */
uchar _Globals::CGameCounter_dtor(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040bce0 END



// !FUNC 0x0040e810 BEGIN
/* 40E810-40E874 00064 */
uint _Globals::CreateObject_0040e810() { STUB_BODY(); return 0; }
// !FUNC 0x0040e810 END

// !FUNC 0x0040e980 BEGIN
/* 40E980-40E9E4 00064 */
CSessionList* _Globals::CreateObject_0040e980() { STUB_BODY(); return 0; }
// !FUNC 0x0040e980 END

// !FUNC 0x0040ed20 BEGIN
/* 40ED20-40ED87 00067 */
uint _Globals::CreateObject_0040ed20() { STUB_BODY(); return 0; }
// !FUNC 0x0040ed20 END

// !FUNC 0x0040eed0 BEGIN
/* 40EED0-40EF37 00067 */
CGameCounter* _Globals::CreateObject_0040eed0() { STUB_BODY(); return 0; }
// !FUNC 0x0040eed0 END

// !FUNC 0x0040ef60 BEGIN
/* 40EF60-40EFC4 00064 */
CWindow* _Globals::CreateObject_0040ef60() { STUB_BODY(); return 0; }
// !FUNC 0x0040ef60 END

// !FUNC 0x0040f1e0 BEGIN
/* 40F1E0-40F244 00064 */
CSetupDlg* _Globals::CSetupDlg_CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x0040f1e0 END

// !FUNC 0x0040f250 BEGIN
/* 40F250-40F2B7 00067 */
CAdvertising* _Globals::CreateObject_0040f250() { STUB_BODY(); return 0; }
// !FUNC 0x0040f250 END

// !FUNC 0x0040f2c0 BEGIN
/* 40F2C0-40F375 000B5 */
uint _Globals::CMsgDialog_ShowModalFromStringHandle(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040f2c0 END

// !FUNC 0x0040f380 BEGIN
/* 40F380-40F3F2 00072 */
void _Globals::CSessionList_AppendEnumSession(DPEnumSessionInfo* param_1, uchar param_2) { STUB_BODY(); }
// !FUNC 0x0040f380 END

// !FUNC 0x0040fb90 BEGIN
/* 40FB90-40FBF7 00067 */
CStartGame1* _Globals::CreateObject_0040fb90() { STUB_BODY(); return 0; }
// !FUNC 0x0040fb90 END

// !FUNC 0x004102d0 BEGIN
/* 4102D0-4104E3 00213 */
void _Globals::CStartGame2_BuildLobbyChatPanel(CStartGame2* param_1, uchar param_2, int param_3, int param_4) { STUB_BODY(); }
// !FUNC 0x004102d0 END

// !FUNC 0x004120f0 BEGIN
/* 4120F0-412154 00064 */
CExitDlg* _Globals::CreateObject_004120f0() { STUB_BODY(); return 0; }
// !FUNC 0x004120f0 END

// !FUNC 0x004123d0 BEGIN
/* 4123D0-412437 00067 */
uint* _Globals::CreateObject_004123d0() { STUB_BODY(); return 0; }
// !FUNC 0x004123d0 END

// !FUNC 0x00412460 BEGIN
/* 412460-412490 00030 */
uchar _Globals::CGame_TeardownNetworkSession(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412460 END

// !FUNC 0x00412490 BEGIN
/* 412490-41249E 0000E */
uchar _Globals::GetMaxAmmoForKind(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412490 END

// !FUNC 0x004124a0 BEGIN
/* 4124A0-412511 00071 */
uchar _Globals::CGame_NetSendKick_t0a(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004124a0 END

// !FUNC 0x00412590 BEGIN
/* 412590-4125A2 00012 */
uint _Globals::CGameGetPlayerMidGameStatusByte(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412590 END

// !FUNC 0x004125b0 BEGIN
/* 4125B0-4125E1 00031 */
uint _Globals::CGame_IsPlayerAliveForLastMan(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004125b0 END

// !FUNC 0x004125f0 BEGIN
/* 4125F0-412631 00041 */
bool _Globals::CGame_AllSlotsHaveState(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004125f0 END

// !FUNC 0x00412640 BEGIN
/* 412640-412657 00017 */
uint _Globals::CMenu_GetProfileBindIndex(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412640 END

// !FUNC 0x00412660 BEGIN
/* 412660-41267D 0001D */
uchar _Globals::CGameGetModeAndScoreLimit(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412660 END

// !FUNC 0x004128a0 BEGIN
/* 4128A0-4128E1 00041 */
uchar _Globals::CGame_SetTeamScore(uchar param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004128a0 END

// !FUNC 0x004128f0 BEGIN
/* 4128F0-41290E 0001E */
uint _Globals::CGame_IsLocalPlayerSlot(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004128f0 END

// !FUNC 0x00412910 BEGIN
/* 412910-412922 00012 */
wchar_t** _Globals::CGameGetPlayerNamePtr(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412910 END

// !FUNC 0x00412930 BEGIN
/* 412930-412942 00012 */
uchar _Globals::CGameGetPlayerColorByte(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412930 END

// !FUNC 0x00412950 BEGIN
/* 412950-41298D 0003D */
uchar _Globals::CGame_NetSendWorldEvent_t13(ushort* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412950 END

// !FUNC 0x00412990 BEGIN
/* 412990-4129BC 0002C */
uchar _Globals::CGame_NetSendPlayerEvent2_t14(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412990 END

// !FUNC 0x00412a00 BEGIN
/* 412A00-412A33 00033 */
uchar _Globals::CGame_NetSendRoundTimer_t12(ushort param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412a00 END

// !FUNC 0x00412a80 BEGIN
/* 412A80-412ABC 0003C */
uchar _Globals::CGame_NetSendTriByteEvent_t19(uchar param_1, uchar param_2, uchar param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00412a80 END

// !FUNC 0x00412ac0 BEGIN
/* 412AC0-412B0D 0004D */
uchar _Globals::CGame_NetSendPlaceObject_t18(uchar param_1, uchar param_2, ushort* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00412ac0 END

// !FUNC 0x00412b60 BEGIN
/* 412B60-412B8C 0002C */
uchar _Globals::CGame_NetSendPlayerEvent1_t0e(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412b60 END

// !FUNC 0x00412b90 BEGIN
/* 412B90-412BE5 00055 */
uchar _Globals::CGame_NetSendShotSpawn_t0f(uchar param_1, uchar param_2, uchar param_3, ushort* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00412b90 END

// !FUNC 0x00412bf0 BEGIN
/* 412BF0-412C3D 0004D */
uchar _Globals::CGame_NetSendHit_t10(uchar param_1, uchar param_2, ushort* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00412bf0 END

// !FUNC 0x00412df0 BEGIN
/* 412DF0-412E88 00098 */
uchar _Globals::CGame_NetSendCustomScriptPayload_t15(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412df0 END

// !FUNC 0x00412f40 BEGIN
/* 412F40-412FA0 00060 */
uchar _Globals::CDsStringHandleVector_MoveTail(void* param_1, void* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00412f40 END

// !FUNC 0x004130e0 BEGIN
/* 4130E0-41311C 0003C */
uchar _Globals::CGame_ClearSchedulerChildren(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004130e0 END

// !FUNC 0x00413200 BEGIN
/* 413200-413212 00012 */
uchar _Globals::CGame_CopyDefaultPaletteSeed(uint* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00413200 END

// !FUNC 0x00413220 BEGIN
/* 413220-413389 00169 */
uchar _Globals::CGame_BuildPaletteLut(uint* param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00413220 END

// !FUNC 0x00413410 BEGIN
/* 413410-413435 00025 */
uchar _Globals::CLevelNameList_ZeroNewElements(uint* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00413410 END

// !FUNC 0x00413440 BEGIN
/* 413440-4134EE 000AE */
int _Globals::CLevelNameList_FindIndex(uint* param_1, uchar* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00413440 END

// !FUNC 0x004134f0 BEGIN
/* 4134F0-41350C 0001C */
uchar _Globals::Runtime_FreePointerFieldZero(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004134f0 END

// !FUNC 0x00413510 BEGIN
/* 413510-41355D 0004D */
uchar _Globals::CGame_ResetForLobby(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00413510 END

// !FUNC 0x00413560 BEGIN
/* 413560-413611 000B1 */
int _Globals::CGame_FindResourceByName(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00413560 END

// !FUNC 0x004137b0 BEGIN
/* 4137B0-413840 00090 */
uchar _Globals::CMenu_ShowConnectingDialog(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004137b0 END

// !FUNC 0x00413c50 BEGIN
/* 413C50-413C6F 0001F */
uchar _Globals::CGame_BuildTeamPaletteLut(uint* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00413c50 END

// !FUNC 0x00413c70 BEGIN
/* 413C70-413CDB 0006B */
int _Globals::CLevelNameList_FindIndexWithKey(int param_1, uchar* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00413c70 END

// !FUNC 0x004140fa BEGIN
/* 4140FA-41415E 00064 */
uchar _Globals::Catch_004140fa() { STUB_BODY(); return 0; }
// !FUNC 0x004140fa END

// !FUNC 0x0041415e BEGIN
/* 41415E-41416F 00011 */
uchar _Globals::Catch_0041415e() { STUB_BODY(); return 0; }
// !FUNC 0x0041415e END

// !FUNC 0x00414250 BEGIN
/* 414250-41427B 0002B */
uchar _Globals::CGame_SetCommStrm(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00414250 END

// !FUNC 0x004144c0 BEGIN
/* 4144C0-4144EB 0002B */
uchar _Globals::CDsStringPtrArray_ReleaseRange(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004144c0 END

// !FUNC 0x004146b0 BEGIN
/* 4146B0-414725 00075 */
uchar _Globals::CLevelNameList_Resize(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004146b0 END

// !FUNC 0x00414730 BEGIN
/* 414730-41476B 0003B */
uchar _Globals::CGame_LevelList_EnsureCapacity(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00414730 END

// !FUNC 0x00414770 BEGIN
/* 414770-414782 00012 */
uchar _Globals::CDSPtrSlotVec_ResetOnUnwind(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00414770 END

// !FUNC 0x00414870 BEGIN
/* 414870-414907 00097 */
int _Globals::CDsStringHandleVector_InsertAt(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00414870 END

// !FUNC 0x00414910 BEGIN
/* 414910-41491F 0000F */
uchar _Globals::CLevelNameList_Clear(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00414910 END

// !FUNC 0x00414920 BEGIN
/* 414920-414A7D 0015D */
int _Globals::CGame_LevelList_AddByName(int param_1, uchar* param_2, uchar* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00414920 END

// !FUNC 0x00415ef0 BEGIN
/* 415EF0-415F57 00067 */
CGame* _Globals::CreateObject_00415ef0() { STUB_BODY(); return 0; }
// !FUNC 0x00415ef0 END

// !FUNC 0x00416510 BEGIN
/* 416510-41656D 0005D */
uchar _Globals::CDSView_DtorTeardownAnimInner(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416510 END


// !FUNC 0x004165b0 BEGIN
/* 4165B0-4165C7 00017 */
uchar _Globals::CDSView_PostEntityDetachMessage_f9(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004165b0 END

// !FUNC 0x004165d0 BEGIN
/* 4165D0-416604 00034 */
uchar _Globals::CTeleportPoint_TryActivateFromOverlap(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004165d0 END

// !FUNC 0x00416610 BEGIN
/* 416610-41662A 0001A */
uchar _Globals::CBulanek_NetSendTeamScoreOnCollect(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416610 END

// !FUNC 0x00416640 BEGIN
/* 416640-41664F 0000F */
uchar _Globals::CBulanek_GetAmmoForKind(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416640 END

// !FUNC 0x00416670 BEGIN
/* 416670-416690 00020 */
uint _Globals::CBulanek_GetSpawnQuipSlot(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416670 END

// !FUNC 0x00416720 BEGIN
/* 416720-416746 00026 */
uint _Globals::CBulanek_IsHumanPlayer(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416720 END

// !FUNC 0x00416780 BEGIN
/* 416780-41678B 0000B */
uchar _Globals::CWeapon_PlayFireAnim(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416780 END

// !FUNC 0x004167e0 BEGIN
/* 4167E0-41680D 0002D */
uchar _Globals::DefineTraceArea_PostTransitionEvent(int param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004167e0 END

// !FUNC 0x004168d0 BEGIN
/* 4168D0-4168DF 0000F */
uint _Globals::CGaming_GetObjectAtSlotUnchecked(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004168d0 END

// !FUNC 0x00416970 BEGIN
/* 416970-416991 00021 */
int _Globals::CGaming_CountOccupiedPlayerSlots(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416970 END





// !FUNC 0x00416ce0 BEGIN
/* 416CE0-416D21 00041 */
uint _Globals::CDSScript_ReadKindAndRect4(uint* param_1, CDSScript* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00416ce0 END



// !FUNC 0x004171b0 BEGIN
/* 4171B0-417210 00060 */
uchar _Globals::CAnim_dtor(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004171b0 END

// !FUNC 0x00417260 BEGIN
/* 417260-4172CC 0006C */
uchar _Globals::CBulanek_ArmFireDelayScheduler(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417260 END

// !FUNC 0x004173f0 BEGIN
/* 4173F0-417401 00011 */
uchar _Globals::CBulanek_HasAmmoForCurrentWeapon(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004173f0 END

// !FUNC 0x004174a0 BEGIN
/* 4174A0-4174FA 0005A */
uint _Globals::CBulanek_CanDispatchPlayerAction(int param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004174a0 END

// !FUNC 0x00417500 BEGIN
/* 417500-417562 00062 */
uchar _Globals::CBulanek_DecrementWeaponAmmo(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417500 END

// !FUNC 0x00417950 BEGIN
/* 417950-417995 00045 */
void _Globals::TriggerSoundEffect(int param_1, uchar param_2, int* param_3) { STUB_BODY(); }
// !FUNC 0x00417950 END

// !FUNC 0x004179c0 BEGIN
/* 4179C0-4179DE 0001E */
uchar _Globals::CBulanek_PlayItemPickupSound(uchar param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004179c0 END

// !FUNC 0x00417a20 BEGIN
/* 417A20-417A37 00017 */
uchar _Globals::CWeapon_FireMachineGunBurstStart(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417a20 END

// !FUNC 0x00417ce0 BEGIN
/* 417CE0-417D17 00037 */
uchar _Globals::CGaming_FreeDangerZoneNodes(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417ce0 END

// !FUNC 0x00417d20 BEGIN
/* 417D20-417DCF 000AF */
uchar _Globals::CLevelScript_DispatchTraceAreaTransition(uint param_1, uint* param_2, char param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00417d20 END

// !FUNC 0x00417dd0 BEGIN
/* 417DD0-417E39 00069 */
uchar _Globals::CGaming_DispatchTraceAreaTransitionIfMasked(uint param_1, char param_2, uint* param_3, char param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00417dd0 END

// !FUNC 0x00417e80 BEGIN
/* 417E80-417F35 000B5 */
uchar _Globals::CGaming_OnSlotPlacementEvent(uchar param_1, uchar param_2, uchar param_3, int* param_4, char param_5) { STUB_BODY(); return 0; }
// !FUNC 0x00417e80 END

// !FUNC 0x00417fb0 BEGIN
/* 417FB0-417FF5 00045 */
uchar _Globals::CGaming_RegisterObjectAtSlot(int param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00417fb0 END

// !FUNC 0x00418000 BEGIN
/* 418000-4180B3 000B3 */
uchar _Globals::TriggerLevelScriptSound(uchar param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00418000 END












// !FUNC 0x00418bbc BEGIN
/* 418BBC-418BC2 00006 */
uchar* _Globals::Catch_00418bbc() { STUB_BODY(); return 0; }
// !FUNC 0x00418bbc END

// !FUNC 0x00418d00 BEGIN
/* 418D00-418D67 00067 */
CGameView* _Globals::CreateObject_00418d00() { STUB_BODY(); return 0; }
// !FUNC 0x00418d00 END

// !FUNC 0x00418f70 BEGIN
/* 418F70-418FD7 00067 */
uint _Globals::CreateObject_00418f70() { STUB_BODY(); return 0; }
// !FUNC 0x00418f70 END

// !FUNC 0x00418fe0 BEGIN
/* 418FE0-419009 00029 */
uchar _Globals::CGameEntity_SetEntityType(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00418fe0 END

// !FUNC 0x00419530 BEGIN
/* 419530-419552 00022 */
uint _Globals::CBulanek_TestRectOverlapWithEntity(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00419530 END

// !FUNC 0x004195f0 BEGIN
/* 4195F0-41979A 001AA */
uchar _Globals::CBulanek_ClampMoveRectByCollision(uint param_1, int* param_2, int* param_3, int* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x004195f0 END

// !FUNC 0x00419d60 BEGIN
/* 419D60-419E3A 000DA */
uchar _Globals::CGaming_OnTimerCountdown(short param_1, uint param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00419d60 END

// !FUNC 0x00419e90 BEGIN
/* 419E90-419F6B 000DB */
uchar _Globals::CGaming_CheckTraceAreasForEntity(uint* param_1, uint param_2, int* param_3, char param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00419e90 END


// !FUNC 0x0041a0f0 BEGIN
/* 41A0F0-41A13B 0004B */
uint _Globals::CGaming_TryGetPlayerCoords(uchar param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0041a0f0 END


// !FUNC 0x0041a6a0 BEGIN
/* 41A6A0-41A707 00067 */
CGameView* _Globals::CreateObject_0041a6a0() { STUB_BODY(); return 0; }
// !FUNC 0x0041a6a0 END

// !FUNC 0x0041a710 BEGIN
/* 41A710-41A774 00064 */
uint* _Globals::CreateObject_0041a710() { STUB_BODY(); return 0; }
// !FUNC 0x0041a710 END

// !FUNC 0x0041a780 BEGIN
/* 41A780-41A7E7 00067 */
CAnim* _Globals::CreateObject_0041a780() { STUB_BODY(); return 0; }
// !FUNC 0x0041a780 END

// !FUNC 0x0041a7f0 BEGIN
/* 41A7F0-41A857 00067 */
uint _Globals::CreateObject_0041a7f0() { STUB_BODY(); return 0; }
// !FUNC 0x0041a7f0 END

// !FUNC 0x0041ad80 BEGIN
/* 41AD80-41ADA8 00028 */
uchar _Globals::Scheduler_EnqueueEvent(void* param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0041ad80 END

// !FUNC 0x0041af70 BEGIN
/* 41AF70-41B173 00203 */
uint _Globals::CBulanek_StepMovementAndCollision(CBulanek* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041af70 END






// !FUNC 0x0041bc40 BEGIN
/* 41BC40-41BCA7 00067 */
uint _Globals::CreateObject_0041bc40() { STUB_BODY(); return 0; }
// !FUNC 0x0041bc40 END

// !FUNC 0x0041bcb0 BEGIN
/* 41BCB0-41BD17 00067 */
uint* _Globals::CreateObject_0041bcb0() { STUB_BODY(); return 0; }
// !FUNC 0x0041bcb0 END

// !FUNC 0x0041bd20 BEGIN
/* 41BD20-41BD87 00067 */
uint* _Globals::CreateObject_0041bd20() { STUB_BODY(); return 0; }
// !FUNC 0x0041bd20 END

// !FUNC 0x0041bd90 BEGIN
/* 41BD90-41BDF7 00067 */
uint _Globals::CreateObject_0041bd90() { STUB_BODY(); return 0; }
// !FUNC 0x0041bd90 END

// !FUNC 0x0041be00 BEGIN
/* 41BE00-41BE67 00067 */
uint* _Globals::CreateObject_0041be00() { STUB_BODY(); return 0; }
// !FUNC 0x0041be00 END

// !FUNC 0x0041be70 BEGIN
/* 41BE70-41BED7 00067 */
uint _Globals::CGaming_CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x0041be70 END


// !FUNC 0x0041d240 BEGIN
/* 41D240-41D27C 0003C */
uint _Globals::CGame_SpawnPickupObject(uint param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0041d240 END

// !FUNC 0x0041d3c6 BEGIN
/* 41D3C6-41D3D6 00010 */
uchar* _Globals::Catch_0041d3c6() { STUB_BODY(); return 0; }
// !FUNC 0x0041d3c6 END








// !FUNC 0x0041e2c0 BEGIN
/* 41E2C0-41E346 00086 */
uchar _Globals::CGaming_SpawnPickupAndBroadcast(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041e2c0 END


// !FUNC 0x0041eba0 BEGIN
/* 41EBA0-41ECA0 00100 */
uchar _Globals::CGame_ApplyPickup(uchar param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0041eba0 END

// !FUNC 0x0041ed60 BEGIN
/* 41ED60-41EDE7 00087 */
uchar _Globals::DetonatePlayerMines(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041ed60 END

// !FUNC 0x0041f0c0 BEGIN
/* 41F0C0-41F1C8 00108 */
uchar _Globals::CGaming_TickPlayerCollisions(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041f0c0 END

// !FUNC 0x0041f1d0 BEGIN
/* 41F1D0-41F20B 0003B */
uchar _Globals::CGaming_SpawnSpecialPickupIfAllowed(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041f1d0 END

// !FUNC 0x0041f230 BEGIN
/* 41F230-41F347 00117 */
uchar _Globals::CGaming_SpawnBulletAndPlaySound(int* param_1, uchar param_2, uchar param_3, uchar param_4, int* param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0041f230 END

// !FUNC 0x0041f350 BEGIN
/* 41F350-41F4F6 001A6 */
uchar _Globals::CGaming_TickRoundStateAndScoring(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041f350 END

// !FUNC 0x0041f500 BEGIN
/* 41F500-41F5CB 000CB */
uchar _Globals::CGaming_SpawnAndInitializePlayer(uchar param_1, void* param_2, int param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x0041f500 END

// !FUNC 0x0041f5d0 BEGIN
/* 41F5D0-41F60B 0003B */
uchar _Globals::CGaming_SpawnPracticeDummy(void* param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0041f5d0 END




// !FUNC 0x0041f770 BEGIN
/* 41F770-41F8F2 00182 */
uchar _Globals::CGaming_RespawnPlayer(char param_1, uchar param_2, int* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0041f770 END


// !FUNC 0x0041fdf0 BEGIN
/* 41FDF0-41FEC9 000D9 */
uchar _Globals::CWeapon_FireGrenade(CWeapon* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041fdf0 END

// !FUNC 0x00420530 BEGIN
/* 420530-420545 00015 */
uchar _Globals::CGaming_SpawnPlayerAtSlot(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00420530 END

// !FUNC 0x00420550 BEGIN
/* 420550-420598 00048 */
uchar _Globals::CGaming_SpawnCoopPartnerSlots(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00420550 END

// !FUNC 0x004205a0 BEGIN
/* 4205A0-42060E 0006E */
uchar _Globals::CBulanek_SpawnPlayerAndCampaignSlots(CBulanek* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004205a0 END

// !FUNC 0x00420650 BEGIN
/* 420650-42067F 0002F */
uchar _Globals::CBulanek_DispatchCurrentWeaponAction(CWeapon* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00420650 END

// !FUNC 0x004208c0 BEGIN
/* 4208C0-420907 00047 */
uchar _Globals::CBulanek_TriggerPrimaryActionAndBroadcast(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004208c0 END

// !FUNC 0x00421830 BEGIN
/* 421830-421897 00067 */
CHelpScript* _Globals::CreateObject_00421830() { STUB_BODY(); return 0; }
// !FUNC 0x00421830 END

// !FUNC 0x00421ba0 BEGIN
/* 421BA0-421C04 00064 */
CHelpView* _Globals::CreateObject_00421ba0() { STUB_BODY(); return 0; }
// !FUNC 0x00421ba0 END

// !FUNC 0x00422280 BEGIN
/* 422280-4222E7 00067 */
CHelpDlg* _Globals::CreateObject_00422280() { STUB_BODY(); return 0; }
// !FUNC 0x00422280 END

// !FUNC 0x00422310 BEGIN
/* 422310-4223B4 000A4 */
void* _Globals::CDSAudioPlayer_Create(uchar* param_1, IDSAudioSource* param_2, int param_3, void* param_4, char param_5) { STUB_BODY(); return 0; }
// !FUNC 0x00422310 END

// !FUNC 0x004223c0 BEGIN
/* 4223C0-4223DB 0001B */
uchar _Globals::CDSAudioPlayer_PlayAndRelease(int* param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004223c0 END

// !FUNC 0x004223e0 BEGIN
/* 4223E0-422427 00047 */
void _Globals::CDSAudio_SetPanPreview(int param_1) { STUB_BODY(); }
// !FUNC 0x004223e0 END

// !FUNC 0x00422430 BEGIN
/* 422430-42246B 0003B */
uchar _Globals::TriggerBankSample(uchar* param_1, int param_2, int param_3, int param_4, void* param_5, char param_6) { STUB_BODY(); return 0; }
// !FUNC 0x00422430 END

// !FUNC 0x00422500 BEGIN
/* 422500-422541 00041 */
int _Globals::ComputeSpatialAttenuationDb100(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00422500 END


// !FUNC 0x004229b0 BEGIN
/* 4229B0-422A17 00067 */
CHistoryScript* _Globals::CHistoryScript_CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x004229b0 END

// !FUNC 0x00422f00 BEGIN
/* 422F00-422F64 00064 */
CHistoryView* _Globals::CreateObject_00422f00() { STUB_BODY(); return 0; }
// !FUNC 0x00422f00 END

// !FUNC 0x00423610 BEGIN
/* 423610-423677 00067 */
CHistoryDlg* _Globals::CreateObject_00423610() { STUB_BODY(); return 0; }
// !FUNC 0x00423610 END

// !FUNC 0x00423f20 BEGIN
/* 423F20-423F6E 0004E */
uchar _Globals::CSwitch_PlayHoverTrack(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00423f20 END

// !FUNC 0x00424010 BEGIN
/* 424010-424078 00068 */
void _Globals::CMenu_EnableBackgroundState(char param_1) { STUB_BODY(); }
// !FUNC 0x00424010 END

// !FUNC 0x00424400 BEGIN
/* 424400-424424 00024 */
uchar _Globals::CDSVec_ZeroNewIntPairs(uint* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00424400 END

// !FUNC 0x00424430 BEGIN
/* 424430-424497 00067 */
CRuch* _Globals::CreateObject_00424430() { STUB_BODY(); return 0; }
// !FUNC 0x00424430 END

// !FUNC 0x00424d30 BEGIN
/* 424D30-424DBA 0008A */
void _Globals::Button_Click(CSwitch* param_1) { STUB_BODY(); }
// !FUNC 0x00424d30 END

// !FUNC 0x00425060 BEGIN
/* 425060-4250B8 00058 */
uchar _Globals::CPoemScroller_ReleaseOwnedResources(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00425060 END

// !FUNC 0x004250c0 BEGIN
/* 4250C0-425123 00063 */
uchar _Globals::CDSVec_ResizeIntPair(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004250c0 END

// !FUNC 0x00425160 BEGIN
/* 425160-4251C7 00067 */
uint _Globals::CreateObject_00425160() { STUB_BODY(); return 0; }
// !FUNC 0x00425160 END

// !FUNC 0x004251d0 BEGIN
/* 4251D0-425237 00067 */
uchar _Globals::CMenu_CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x004251d0 END

// !FUNC 0x00425450 BEGIN
/* 425450-425458 00008 */
uchar _Globals::CDSVec_ResizeIntPair_ToZero(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00425450 END



// !FUNC 0x00426500 BEGIN
/* 426500-426567 00067 */
CGunMouse* _Globals::CGunMouse_CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x00426500 END

// !FUNC 0x00426570 BEGIN
/* 426570-4265D7 00067 */
CPoemScroller* _Globals::CPoemScroller_CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x00426570 END


// !FUNC 0x004276c0 BEGIN
/* 4276C0-42770F 0004F */
uchar _Globals::CGameCounter_UpdatePlayerScore(uchar param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004276c0 END

// !FUNC 0x00427710 BEGIN
/* 427710-42773B 0002B */
uchar _Globals::CGaming_SetRoundTimerLabelFromString(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00427710 END

// !FUNC 0x004279f0 BEGIN
/* 4279F0-427A54 00064 */
CShotCounter* _Globals::CreateObject_004279f0() { STUB_BODY(); return 0; }
// !FUNC 0x004279f0 END

// !FUNC 0x00427a80 BEGIN
/* 427A80-427AE7 00067 */
uint _Globals::CreateObject_00427a80() { STUB_BODY(); return 0; }
// !FUNC 0x00427a80 END

// !FUNC 0x00427ba0 BEGIN
/* 427BA0-427C07 00067 */
CPanel* _Globals::CreateObject_00427ba0() { STUB_BODY(); return 0; }
// !FUNC 0x00427ba0 END

// !FUNC 0x00427c30 BEGIN
/* 427C30-427C8D 0005D */
uchar _Globals::CGamingHud_SetAmmoStripState(int* param_1, uchar param_2, uchar param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00427c30 END

// !FUNC 0x00427c90 BEGIN
/* 427C90-427D31 000A1 */
uchar _Globals::CGamingHud_UpdatePlayerAmmoStrip(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00427c90 END

// !FUNC 0x00428c15 BEGIN
/* 428C15-428C29 00014 */
uchar _Globals::Catch_00428c15() { STUB_BODY(); return 0; }
// !FUNC 0x00428c15 END

// !FUNC 0x00429280 BEGIN
/* 429280-4292E3 00063 */
uchar _Globals::CDSJpegImage_JpegSetupScope_dtor(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00429280 END


// !FUNC 0x004298b0 BEGIN
/* 4298B0-4298C1 00011 */
bool _Globals::CDSBackBuffer_RestoreSurface() { STUB_BODY(); return 0; }
// !FUNC 0x004298b0 END

// !FUNC 0x004298d0 BEGIN
/* 4298D0-429924 00054 */
bool _Globals::CDSBackBuffer_BindImageFromSurface() { STUB_BODY(); return 0; }
// !FUNC 0x004298d0 END

// !FUNC 0x00429930 BEGIN
/* 429930-429952 00022 */
bool _Globals::CDSBackBuffer_Flip(CDSBackBuffer* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00429930 END

// !FUNC 0x00429960 BEGIN
/* 429960-42998D 0002D */
void _Globals::CDSBackBuffer_FreeImageMember(CDSBackBuffer* param_1) { STUB_BODY(); }
// !FUNC 0x00429960 END


// !FUNC 0x00429c00 BEGIN
/* 429C00-429CA6 000A6 */
int _Globals::CDSApp_WndProc(HWND__* param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00429c00 END

// !FUNC 0x00429c8d BEGIN
/* 429C8D-429C9D 00010 */
uint _Globals::Catch_00429c8d() { STUB_BODY(); return 0; }
// !FUNC 0x00429c8d END

// !FUNC 0x00429cc0 BEGIN
/* 429CC0-429CF9 00039 */
uchar _Globals::CDSApp_KeybQueue(ushort param_1, uchar param_2, uchar param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00429cc0 END

// !FUNC 0x0042a040 BEGIN
/* 42A040-42A06A 0002A */
uchar _Globals::TArray16_ZeroRange(uint* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042a040 END

// !FUNC 0x0042a070 BEGIN
/* 42A070-42A12D 000BD */
int _Globals::CDSApp_DirtyRectList_FindIndex(int* param_1, void* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0042a070 END

// !FUNC 0x0042a130 BEGIN
/* 42A130-42A1B0 00080 */
void _Globals::CDSApp_DirtyRectList_SlideRecords(void* param_1, void* param_2, int param_3) { STUB_BODY(); }
// !FUNC 0x0042a130 END


// !FUNC 0x0042a590 BEGIN
/* 42A590-42A5B9 00029 */
uchar _Globals::CDSApp_TryBindBackBufferSurface() { STUB_BODY(); return 0; }
// !FUNC 0x0042a590 END

// !FUNC 0x0042a910 BEGIN
/* 42A910-42A971 00061 */
uchar _Globals::CDSApp_DirtyRectList_SetSize(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042a910 END

// !FUNC 0x0042a980 BEGIN
/* 42A980-42A9BB 0003B */
uchar _Globals::CDSApp_DirtyRectList_EnsureCapacity(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042a980 END

// !FUNC 0x0042a9c0 BEGIN
/* 42A9C0-42AA21 00061 */
void _Globals::Catch_0042ab28_WriteExceptionLog(void* param_1, int* param_2, wchar_t* param_3) { STUB_BODY(); }
// !FUNC 0x0042a9c0 END

// !FUNC 0x0042aa60 BEGIN
/* 42AA60-42AB28 000C8 */
uint _Globals::CDSApp_AppMain(HINSTANCE__** param_1, uint param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0042aa60 END

// !FUNC 0x0042ab28 BEGIN
/* 42AB28-42ABB9 00091 */
uchar _Globals::Catch_0042ab28() { STUB_BODY(); return 0; }
// !FUNC 0x0042ab28 END

// !FUNC 0x0042abb9 BEGIN
/* 42ABB9-42ABC9 00010 */
uchar* _Globals::Catch_0042abb9() { STUB_BODY(); return 0; }
// !FUNC 0x0042abb9 END

// !FUNC 0x0042abcf BEGIN
/* 42ABCF-42AC14 00045 */
uint _Globals::Catch_0042ab28_ShowMessageAndRelease() { STUB_BODY(); return 0; }
// !FUNC 0x0042abcf END

// !FUNC 0x0042ac20 BEGIN
/* 42AC20-42AC7E 0005E */
int _Globals::CDSApp_DirtyRectList_InsertAt(uint* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042ac20 END

// !FUNC 0x0042add0 BEGIN
/* 42ADD0-42AE34 00064 */
uchar _Globals::CDSApp_SetPendingChildView(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042add0 END

// !FUNC 0x0042ae40 BEGIN
/* 42AE40-42AE4F 0000F */
uchar _Globals::CDSApp_DirtyRectList_Clear(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042ae40 END

// !FUNC 0x0042ae50 BEGIN
/* 42AE50-42AEFA 000AA */
int _Globals::CDSApp_DirtyRectList_UpsertRect(int* param_1, uchar* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0042ae50 END



// !FUNC 0x0042b910 BEGIN
/* 42B910-42B977 00067 */
uint* _Globals::CreateObject_0042b910() { STUB_BODY(); return 0; }
// !FUNC 0x0042b910 END

// !FUNC 0x0042bbe0 BEGIN
/* 42BBE0-42BBF5 00015 */
uchar _Globals::CDSApp_FlushDirtyRectsIfFlagged(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042bbe0 END

// !FUNC 0x0042bc00 BEGIN
/* 42BC00-42BD63 00163 */
uchar _Globals::CDSApp_RenderFrame(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042bc00 END

// !FUNC 0x0042bda0 BEGIN
/* 42BDA0-42BE51 000B1 */
uchar _Globals::CDSApp_FrameBody(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042bda0 END

// !FUNC 0x0042be60 BEGIN
/* 42BE60-42BE86 00026 */
uchar _Globals::CDSApp_PumpTick(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042be60 END

// !FUNC 0x0042bed0 BEGIN
/* 42BED0-42BF39 00069 */
uchar _Globals::CDSView_SetModalEligible(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042bed0 END



// !FUNC 0x0042c140 BEGIN
/* 42C140-42C156 00016 */
void _Globals::CDSView_InputChain_OnDetach() { STUB_BODY(); }
// !FUNC 0x0042c140 END


// !FUNC 0x0042c230 BEGIN
/* 42C230-42C288 00058 */
uchar _Globals::CDSView_ReleaseKeyboardFocus(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c230 END

// !FUNC 0x0042c290 BEGIN
/* 42C290-42C2DF 0004F */
uchar _Globals::CDSView_SetActive(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c290 END


// !FUNC 0x0042c6c0 BEGIN
/* 42C6C0-42C6F7 00037 */
uchar _Globals::CMenu_ClearAllChildren(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c6c0 END

// !FUNC 0x0042c700 BEGIN
/* 42C700-42C768 00068 */
uchar _Globals::CDSApp_RefreshInputChainHitTest() { STUB_BODY(); return 0; }
// !FUNC 0x0042c700 END

// !FUNC 0x0042c7f0 BEGIN
/* 42C7F0-42C854 00064 */
uchar _Globals::CDSApp_SetInputChainHead(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c7f0 END

// !FUNC 0x0042c860 BEGIN
/* 42C860-42C879 00019 */
uchar _Globals::CControl_ClaimModalFocusOnPress(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c860 END

// !FUNC 0x0042c880 BEGIN
/* 42C880-42C8DB 0005B */
uchar _Globals::CDSView_UpdateInputChainOnMouseMove(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c880 END

// !FUNC 0x0042c8e0 BEGIN
/* 42C8E0-42C957 00077 */
uchar _Globals::CDSView_AcquireKeyboardFocus(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c8e0 END

// !FUNC 0x0042c960 BEGIN
/* 42C960-42C990 00030 */
uchar _Globals::CDSView_Show_UpdateFocusChain(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c960 END


// !FUNC 0x0042c9f0 BEGIN
/* 42C9F0-42CA22 00032 */
ushort _Globals::CDSView_EnableWidget(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c9f0 END

// !FUNC 0x0042cc30 BEGIN
/* 42CC30-42CC77 00047 */
uchar _Globals::CDSView_OffsetRectAndAdapt(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042cc30 END

// !FUNC 0x0042ce30 BEGIN
/* 42CE30-42CE94 00064 */
uint* _Globals::CreateObject_0042ce30() { STUB_BODY(); return 0; }
// !FUNC 0x0042ce30 END








// !FUNC 0x0042d2d0 BEGIN
/* 42D2D0-42D2E2 00012 */
uchar _Globals::CDsStringReleaseHeader(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042d2d0 END

// !FUNC 0x0042d350 BEGIN
/* 42D350-42D387 00037 */
int* _Globals::CDsString_AllocWide(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042d350 END

// !FUNC 0x0042d3a0 BEGIN
/* 42D3A0-42D3D2 00032 */
int* _Globals::CDsStringAllocAndCopyWide(void* param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0042d3a0 END

// !FUNC 0x0042d3e0 BEGIN
/* 42D3E0-42D3F0 00010 */
uchar _Globals::CDsStringSetLengthAndTerminate(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042d3e0 END

// !FUNC 0x0042d440 BEGIN
/* 42D440-42D487 00047 */
uchar _Globals::CDsString_WriteWStringToStream(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042d440 END

// !FUNC 0x0042d490 BEGIN
/* 42D490-42D501 00071 */
void* _Globals::CDsStringAssignFromHandle(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042d490 END

// !FUNC 0x0042d5a0 BEGIN
/* 42D5A0-42D702 00162 */
void* _Globals::CDsStringInsertWide(int* param_1, size_t param_2, wchar_t* param_3, size_t param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0042d5a0 END

// !FUNC 0x0042d710 BEGIN
/* 42D710-42D770 00060 */
void* _Globals::CDsStringVsprintfToStackBuffer(wchar_t* param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042d710 END

// !FUNC 0x0042d770 BEGIN
/* 42D770-42D794 00024 */
void* _Globals::CDsStringFormatV(void* param_1, wchar_t* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042d770 END

// !FUNC 0x0042d850 BEGIN
/* 42D850-42D898 00048 */
uchar _Globals::CDsString_EnsureUnique(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042d850 END

// !FUNC 0x0042d8a0 BEGIN
/* 42D8A0-42D8B6 00016 */
uchar _Globals::CDsStringAssignFromLong(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042d8a0 END

// !FUNC 0x0042db60 BEGIN
/* 42DB60-42DC22 000C2 */
uchar _Globals::CDsString_WideToMultiByteBuffer(WCHAR* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042db60 END

// !FUNC 0x0042dc50 BEGIN
/* 42DC50-42DC6D 0001D */
int* _Globals::CDsString_ClearInPlace(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042dc50 END

// !FUNC 0x0042e140 BEGIN
/* 42E140-42E22E 000EE */
int* _Globals::CDsString_ReadWStringFromStream(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042e140 END

// !FUNC 0x0042e4e0 BEGIN
/* 42E4E0-42E62B 0014B */
void _Globals::CDsString_WriteTwoWideStringsToStream(int* param_1, wchar_t* param_2) { STUB_BODY(); }
// !FUNC 0x0042e4e0 END

// !FUNC 0x0042e630 BEGIN
/* 42E630-42E677 00047 */
uint _Globals::IsLeapYear(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042e630 END

// !FUNC 0x0042e790 BEGIN
/* 42E790-42E7A2 00012 */
void _Globals::CDSApp_UpdateClock() { STUB_BODY(); }
// !FUNC 0x0042e790 END

// !FUNC 0x0042e7b0 BEGIN
/* 42E7B0-42E841 00091 */
uchar _Globals::CDate_SetDate(uint* param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0042e7b0 END

// !FUNC 0x0042e8f0 BEGIN
/* 42E8F0-42E909 00019 */
void _Globals::ClassRegEntry_PrependListHead(void* param_1, void* param_2) { STUB_BODY(); }
// !FUNC 0x0042e8f0 END

// !FUNC 0x0042e910 BEGIN
/* 42E910-42E95C 0004C */
void* _Globals::HandleClassRegister(uint param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0042e910 END

// !FUNC 0x0042e960 BEGIN
/* 42E960-42E97D 0001D */
uint _Globals::ClassRegEntry_ListContains(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042e960 END

// !FUNC 0x0042e980 BEGIN
/* 42E980-42E99F 0001F */
uchar _Globals::HandleInterfaceRegister(int param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0042e980 END

// !FUNC 0x0042e9a0 BEGIN
/* 42E9A0-42E9EA 0004A */
uint _Globals::HandleVirtualBaseCast(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042e9a0 END

// !FUNC 0x0042e9f0 BEGIN
/* 42E9F0-42EA13 00023 */
int _Globals::CheckedVirtualBaseCast(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042e9f0 END

// !FUNC 0x0042ea40 BEGIN
/* 42EA40-42EA65 00025 */
uchar _Globals::CDSQueue_ZeroRecordArgsRange(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042ea40 END

// !FUNC 0x0042ea80 BEGIN
/* 42EA80-42EA95 00015 */
void _Globals::CDSObject_AddRef(int* param_1) { STUB_BODY(); }
// !FUNC 0x0042ea80 END


// !FUNC 0x0042eb30 BEGIN
/* 42EB30-42EBA1 00071 */
void _Globals::Scheduler_DispatchDueEvents(CDSUpdatedItem* param_1) { STUB_BODY(); }
// !FUNC 0x0042eb30 END


// !FUNC 0x0042ebf0 BEGIN
/* 42EBF0-42EC35 00045 */
uchar _Globals::CDSApp_PulseTasks() { STUB_BODY(); return 0; }
// !FUNC 0x0042ebf0 END


// !FUNC 0x0042ecc0 BEGIN
/* 42ECC0-42ECE5 00025 */
uint _Globals::CStartGame2_EnqueueEvent_NullSafe(void* param_1, ushort param_2, ushort param_3, uint param_4, uint param_5) { STUB_BODY(); return 0; }
// !FUNC 0x0042ecc0 END

// !FUNC 0x0042ecf0 BEGIN
/* 42ECF0-42ED68 00078 */
uint _Globals::CDSQueue_Push(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042ecf0 END

// !FUNC 0x0042ed70 BEGIN
/* 42ED70-42ED96 00026 */
uint _Globals::CDSQueue_PopDiscard(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042ed70 END

// !FUNC 0x0042eda0 BEGIN
/* 42EDA0-42EE20 00080 */
uint _Globals::CDSQueue_PopCopy(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042eda0 END

// !FUNC 0x0042ee20 BEGIN
/* 42EE20-42EE3B 0001B */
int _Globals::CDSQueue_Peek(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042ee20 END

// !FUNC 0x0042ee40 BEGIN
/* 42EE40-42EEA6 00066 */
uchar _Globals::CDSQueue_SetCapacity(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042ee40 END

// !FUNC 0x0042ef00 BEGIN
/* 42EF00-42EFE2 000E2 */
uint _Globals::InitializeByClassId(WCHAR* param_1, uint param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0042ef00 END








// !FUNC 0x0042f410 BEGIN
/* 42F410-42F453 00043 */
uint _Globals::CDSApp_PollEventQueue() { STUB_BODY(); return 0; }
// !FUNC 0x0042f410 END

// !FUNC 0x0042f460 BEGIN
/* 42F460-42F4A2 00042 */
uchar _Globals::CDSApp_DispatchOneEvent() { STUB_BODY(); return 0; }
// !FUNC 0x0042f460 END


// !FUNC 0x0042f5c0 BEGIN
/* 42F5C0-42F5E6 00026 */
uint _Globals::CDSQueue_Init(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f5c0 END

// !FUNC 0x0042f5f0 BEGIN
/* 42F5F0-42F620 00030 */
uchar _Globals::Scheduler_PopHook(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f5f0 END

// !FUNC 0x0042f620 BEGIN
/* 42F620-42F634 00014 */
uchar _Globals::Scheduler_PushHook(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f620 END

// !FUNC 0x0042f640 BEGIN
/* 42F640-42F690 00050 */
int _Globals::CDSEventHandler_ctor(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f640 END

// !FUNC 0x0042f690 BEGIN
/* 42F690-42F6F0 00060 */
uchar _Globals::CDSEventQueue_ClearForTarget(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f690 END

// !FUNC 0x0042f6f0 BEGIN
/* 42F6F0-42F715 00025 */
void* _Globals::Runtime_MallocOrThrow(uchar* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f6f0 END

// !FUNC 0x0042f720 BEGIN
/* 42F720-42F72E 0000E */
void _Globals::Runtime_Free(void* param_1) { STUB_BODY(); }
// !FUNC 0x0042f720 END

// !FUNC 0x0042f730 BEGIN
/* 42F730-42F77C 0004C */
void* _Globals::Runtime_ReallocOrThrow(void* param_1, uchar* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042f730 END

// !FUNC 0x0042f780 BEGIN
/* 42F780-42F79B 0001B */
uchar _Globals::CDSChained_LinkIntrusiveNode(uint* param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042f780 END

// !FUNC 0x0042f7a0 BEGIN
/* 42F7A0-42F7BB 0001B */
uchar _Globals::CDSChained_UnlinkIntrusiveNode(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f7a0 END

// !FUNC 0x0042f7c0 BEGIN
/* 42F7C0-42F7CC 0000C */
void* _Globals::CDSChained_GetFirstChildView(CDSChain* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f7c0 END

// !FUNC 0x0042f7d0 BEGIN
/* 42F7D0-42F7D4 00004 */
uint _Globals::CDSIntrusiveNode_GetLinkNext() {
    return *reinterpret_cast<uint*>(reinterpret_cast<char*>(this) + 8);
}
// !FUNC 0x0042f7d0 END

// !FUNC 0x0042f7e0 BEGIN
/* 42F7E0-42F7F5 00015 */
uint _Globals::CDSIntrusiveNode_GetNextLinkSkipSentinel(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f7e0 END



// !FUNC 0x0042f850 BEGIN
/* 42F850-42F873 00023 */
uchar _Globals::CDSChained_ResetHeadOrSpliceBefore(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f850 END

// !FUNC 0x0042f920 BEGIN
/* 42F920-42F937 00017 */
void* _Globals::CDSChained_GetNextSiblingView(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f920 END

// !FUNC 0x0042f9b0 BEGIN
/* 42F9B0-42F9D0 00020 */
uchar _Globals::CDSChained_AppendChild(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f9b0 END

// !FUNC 0x0042f9d0 BEGIN
/* 42F9D0-42FA1D 0004D */
void _Globals::CDSChained_InsertChildAtAnchor(void* param_1, void* param_2, void* param_3) { STUB_BODY(); }
// !FUNC 0x0042f9d0 END


// !FUNC 0x0042fc05 BEGIN
/* 42FC05-42FC1B 00016 */
uchar _Globals::Catch_0042fc05() { STUB_BODY(); return 0; }
// !FUNC 0x0042fc05 END


// !FUNC 0x00430270 BEGIN
/* 430270-4302DD 0006D */
void _Globals::RaiseStreamException(uint param_1, int* param_2) { STUB_BODY(); }
// !FUNC 0x00430270 END

// !FUNC 0x004302e0 BEGIN
/* 4302E0-430352 00072 */
void _Globals::ThrowStreamErrorNoReturn(uint param_1, int* param_2, uint param_3) { STUB_BODY(); }
// !FUNC 0x004302e0 END

// !FUNC 0x00430a70 BEGIN
/* 430A70-430A84 00014 */
uchar _Globals::CDSEasyMemStream_GuardReadable(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00430a70 END

// !FUNC 0x00430e70 BEGIN
/* 430E70-430F3B 000CB */
CDSEasyMemStream* _Globals::CDSEasyMemStream_CreateFromStreamSlice(void* param_1, uint param_2, void* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00430e70 END

// !FUNC 0x00431070 BEGIN
/* 431070-4310AC 0003C */
uchar _Globals::CDSCollection_EnsureCapacity(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431070 END

// !FUNC 0x004310b0 BEGIN
/* 4310B0-4310FA 0004A */
int _Globals::CDSCollection_Insert(uint param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004310b0 END

// !FUNC 0x00431100 BEGIN
/* 431100-43116A 0006A */
uchar _Globals::CDSCollection_Remove(int param_1, int param_2, char param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00431100 END

// !FUNC 0x004313f5 BEGIN
/* 4313F5-43140D 00018 */
uchar _Globals::Catch_004313f5() { STUB_BODY(); return 0; }
// !FUNC 0x004313f5 END

// !FUNC 0x00431590 BEGIN
/* 431590-4315B3 00023 */
uchar _Globals::CDSJpegImage_InitJpegErrorMgr(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431590 END

// !FUNC 0x004315d0 BEGIN
/* 4315D0-431624 00054 */
void _Globals::CDSJpegMemPool_free_pool(void* param_1, int param_2) { STUB_BODY(); }
// !FUNC 0x004315d0 END

// !FUNC 0x00431670 BEGIN
/* 431670-4316B2 00042 */
uint _Globals::fill_input_buffer(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431670 END

// !FUNC 0x004316c0 BEGIN
/* 4316C0-4316F1 00031 */
uchar _Globals::jpeg_skip_input_data(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004316c0 END

// !FUNC 0x00431700 BEGIN
/* 431700-431783 00083 */
uchar _Globals::jpeg_CDSStreamStorage_src(int param_1, int* param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00431700 END

// !FUNC 0x00431790 BEGIN
/* 431790-431819 00089 */
int _Globals::CDSJpegMemPool_BumpAlloc(int param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00431790 END

// !FUNC 0x00431819 BEGIN
/* 431819-431848 0002F */
uchar* _Globals::Catch_00431819() { STUB_BODY(); return 0; }
// !FUNC 0x00431819 END

// !FUNC 0x00431851 BEGIN
/* 431851-43187E 0002D */
int _Globals::CDSJpegMemPool_LinkNewChunk() { STUB_BODY(); return 0; }
// !FUNC 0x00431851 END

// !FUNC 0x00431890 BEGIN
/* 431890-4318E3 00053 */
int _Globals::CDSJpegMemPool_alloc_sarray(int param_1, int param_2, int param_3, uint param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00431890 END

// !FUNC 0x004318f0 BEGIN
/* 4318F0-431946 00056 */
int _Globals::CDSJpegMemPool_alloc_barray(int param_1, int param_2, int param_3, uint param_4) { STUB_BODY(); return 0; }
// !FUNC 0x004318f0 END

// !FUNC 0x004319d0 BEGIN
/* 4319D0-431A67 00097 */
uchar _Globals::jpeg_realize_virt_arrays(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004319d0 END

// !FUNC 0x00431a70 BEGIN
/* 431A70-431AB4 00044 */
uchar _Globals::jinit_memory_mgr(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431a70 END

// !FUNC 0x00431ab4 BEGIN
/* 431AB4-431AE3 0002F */
uchar* _Globals::Catch_00431ab4() { STUB_BODY(); return 0; }
// !FUNC 0x00431ab4 END

// !FUNC 0x00431ae8 BEGIN
/* 431AE8-431B61 00079 */
uchar _Globals::CDSJpegMemPool_InitMethodPointers() { STUB_BODY(); return 0; }
// !FUNC 0x00431ae8 END


// !FUNC 0x00431ca9 BEGIN
/* 431CA9-431CBE 00015 */
uchar _Globals::Catch_00431ca9() { STUB_BODY(); return 0; }
// !FUNC 0x00431ca9 END

// !FUNC 0x00431f98 BEGIN
/* 431F98-431FAD 00015 */
uchar _Globals::Catch_00431f98() { STUB_BODY(); return 0; }
// !FUNC 0x00431f98 END

// !FUNC 0x004331e0 BEGIN
/* 4331E0-4331F3 00013 */
uchar _Globals::CBulanci_CloseFindHandle(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004331e0 END


// !FUNC 0x004339e0 BEGIN
/* 4339E0-4339EA 0000A */
uchar _Globals::Eh_LeaveCriticalSection(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004339e0 END

// !FUNC 0x00434250 BEGIN
/* 434250-434263 00013 */
uchar _Globals::IDSChainedTail_ClearSubObjStash(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00434250 END

// !FUNC 0x004344c0 BEGIN
/* 4344C0-4344CE 0000E */
void _Globals::eh_dtor_CDSObject_ptr(int** param_1) { STUB_BODY(); }
// !FUNC 0x004344c0 END

// !FUNC 0x004344d0 BEGIN
/* 4344D0-434534 00064 */
CDSResourceException* _Globals::CDSResourceException_CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x004344d0 END

// !FUNC 0x00434930 BEGIN
/* 434930-434994 00064 */
CDSResourceSign* _Globals::InitializeAndAllocate() { STUB_BODY(); return 0; }
// !FUNC 0x00434930 END

// !FUNC 0x004349e0 BEGIN
/* 4349E0-434A00 00020 */
void _Globals::Runtime_ThrowBadAlloc(uchar* param_1) { STUB_BODY(); }
// !FUNC 0x004349e0 END

// !FUNC 0x00434bd0 BEGIN
/* 434BD0-434C20 00050 */
uchar _Globals::CDsStringFromWin32ErrorCode(uchar param_1, DWORD param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00434bd0 END

// !FUNC 0x00434e30 BEGIN
/* 434E30-434EDA 000AA */
// CDSGZipStream::Decompress(byte* dest, ref int destLen, byte* source, int sourceLen)
// -- 1:1 with zlib 1.1.3 `uncompress()` (with the >64K guards stripped and
// `opaque` zeroed explicitly, like compress2 does).  The editor's C# mirror
// in GZipStream.cs is line-for-line equivalent.
//
// Branch order matters: the original source checks the *failure* path first
// (err != Z_STREAM_END), so the compiler lays out success at the end and
// uses ESI to preserve `err` across the inflateEnd call.  Inverting the
// `if`s makes /O2 use ECX (caller-saved) and emit a `mov ecx, eax` after
// every call, adding ~9 bytes the original never spends.
uint _Globals::CDSGZipStream_Decompress_static(uint param_1, int param_2, uint* param_3, uint param_4) {
    struct z_stream_s {
        void *next_in;       /* 0x00 */
        uint avail_in;       /* 0x04 */
        ulong total_in;      /* 0x08 */
        void *next_out;      /* 0x0c */
        uint avail_out;      /* 0x10 */
        ulong total_out;     /* 0x14 */
        char *msg;           /* 0x18 */
        void *state;         /* 0x1c */
        void *zalloc;        /* 0x20 */
        void *zfree;         /* 0x24 */
        void *opaque;        /* 0x28 */
        int data_type;       /* 0x2c */
        ulong adler;         /* 0x30 */
        ulong reserved;      /* 0x34 */
    } strm;
    uint err;
    strm.next_in = (void*)param_2;
    strm.avail_in = param_4;
    strm.next_out = (void*)param_1;
    strm.avail_out = *param_3;
    strm.zalloc = 0;
    strm.zfree = 0;
    strm.opaque = 0;
    err = zlib::inflateInit((int)&strm, (char*)"1.1.3", 0x38);
    if (err != 0) return err;
    err = zlib::inflate((int*)&strm, 4);
    if (err != 1) {
        zlib::inflateEnd((int)&strm);
        if (err == 0) return 0xfffffffb;
        return err;
    }
    *param_3 = (uint)strm.total_out;
    return zlib::inflateEnd((int)&strm);
}
// !FUNC 0x00434e30 END

// !FUNC 0x00434ee0 BEGIN
/* 434EE0-434F8C 000AC */
// CDSGZipStream::Compress(byte* dest, ref int destLen, byte* source, int sourceLen)
// -- zlib 1.1.3 `compress2()` with level hardcoded to Z_BEST_COMPRESSION (9).
// 1:1 with the editor's GZipStream.cs Compress() helper.  See Decompress
// (CDSGZipStream_Decompress_static) for why the failure branch comes first.
uint _Globals::CDSGZipStream_Compress_static(uint param_1, int param_2, uint* param_3, uint param_4) {
    struct z_stream_s {
        void *next_in;       /* 0x00 */
        uint avail_in;       /* 0x04 */
        ulong total_in;      /* 0x08 */
        void *next_out;      /* 0x0c */
        uint avail_out;      /* 0x10 */
        ulong total_out;     /* 0x14 */
        char *msg;           /* 0x18 */
        void *state;         /* 0x1c */
        void *zalloc;        /* 0x20 */
        void *zfree;         /* 0x24 */
        void *opaque;        /* 0x28 */
        int data_type;       /* 0x2c */
        ulong adler;         /* 0x30 */
        ulong reserved;      /* 0x34 */
    } strm;
    uint err;
    strm.next_in = (void*)param_2;
    strm.avail_in = param_4;
    strm.next_out = (void*)param_1;
    strm.avail_out = *param_3;
    strm.zalloc = 0;
    strm.zfree = 0;
    strm.opaque = 0;
    err = zlib::deflateInit((int)&strm, 9, (char*)"1.1.3", 0x38);
    if (err != 0) return err;
    err = zlib::deflate((int*)&strm, 4);
    if (err != 1) {
        zlib::deflateEnd((int)&strm);
        if (err == 0) return 0xfffffffb;
        return err;
    }
    *param_3 = (uint)strm.total_out;
    return zlib::deflateEnd((int)&strm);
}
// !FUNC 0x00434ee0 END

// !FUNC 0x00435050 BEGIN
/* 435050-435137 000E7 */
uchar _Globals::CDSGZipStream_ReadBufferAt(uint param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00435050 END

// !FUNC 0x00435140 BEGIN
/* 435140-435215 000D5 */
uchar _Globals::CDSGZipStream_WriteBuffer(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00435140 END

// !FUNC 0x004354a0 BEGIN
/* 4354A0-43561A 0017A */
uchar _Globals::CDSGZipStream_CloseInt(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004354a0 END

// !FUNC 0x00435670 BEGIN
/* 435670-4356DA 0006A */
uchar _Globals::CDSGZipStream_AllocateBuffers(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00435670 END

// !FUNC 0x00435c70 BEGIN
/* 435C70-435CB7 00047 */
uint _Globals::MapBitCountToFormat(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00435c70 END

// !FUNC 0x00435d00 BEGIN
/* 435D00-435D30 00030 */
uchar _Globals::SampleSourcePixel_Indexed1(uint param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00435d00 END

// !FUNC 0x00435d30 BEGIN
/* 435D30-435D62 00032 */
uchar _Globals::SampleSourcePixel_Indexed2(uint param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00435d30 END

// !FUNC 0x00435d70 BEGIN
/* 435D70-435DA2 00032 */
uchar _Globals::SampleSourcePixel_Indexed4(uint param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00435d70 END

// !FUNC 0x00435db0 BEGIN
/* 435DB0-435DCA 0001A */
uchar _Globals::SampleSourcePixel_Indexed8(int param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00435db0 END

// !FUNC 0x00435dd0 BEGIN
/* 435DD0-435DEF 0001F */
ushort _Globals::Blit_ReadDstPixel_16bpp(int param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00435dd0 END

// !FUNC 0x00435df0 BEGIN
/* 435DF0-435E19 00029 */
uint _Globals::CDSImage_GetPixel24(int param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00435df0 END

// !FUNC 0x00435e20 BEGIN
/* 435E20-435E3D 0001D */
uint _Globals::Blit_ReadDstPixel_32bpp(int param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00435e20 END

// !FUNC 0x00435f30 BEGIN
/* 435F30-435F4C 0001C */
uchar _Globals::Blit_WriteDstPixel_8bpp(int param_1, int param_2, uchar param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x00435f30 END

// !FUNC 0x00435f50 BEGIN
/* 435F50-435F71 00021 */
uchar _Globals::Blit_WriteDstPixel_16bpp(int param_1, int param_2, ushort param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x00435f50 END

// !FUNC 0x00435f80 BEGIN
/* 435F80-435FAB 0002B */
uchar _Globals::CDSImage_PutPixel24(int param_1, int param_2, uint param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x00435f80 END

// !FUNC 0x00435fb0 BEGIN
/* 435FB0-435FD1 00021 */
uchar _Globals::Blit_WriteDstPixel_32bpp(int param_1, int param_2, uint param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x00435fb0 END



// !FUNC 0x00436750 BEGIN
/* 436750-436759 00009 */
void _Globals::CDSBackBuffer_ClearPreFlipFields(CDSImage_BackBufferEmbed* param_1) { STUB_BODY(); }
// !FUNC 0x00436750 END



// !FUNC 0x00436d90 BEGIN
/* 436D90-436E0A 0007A */
uchar _Globals::CDSImage_BindFromSurfaceDesc(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00436d90 END





// !FUNC 0x00437030 BEGIN
/* 437030-437079 00049 */
uchar _Globals::CDSView_CopyColorPlane(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00437030 END

// !FUNC 0x00437080 BEGIN
/* 437080-4370AA 0002A */
uchar _Globals::NotifyDirtyAll(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00437080 END


// !FUNC 0x00437440 BEGIN
/* 437440-437502 000C2 */
uchar _Globals::WideCharToMultiByte_Wrapper(WCHAR* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00437440 END

// !FUNC 0x004375e0 BEGIN
/* 4375E0-437AB7 004D7 */
uchar _Globals::TextShaper_LayOutAndRender(WCHAR* param_1, tagRECT* param_2, CPoemScroller* param_3, uint* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x004375e0 END

// !FUNC 0x00437b00 BEGIN
/* 437B00-437B14 00014 */
uchar _Globals::CDSRegKey_CloseKey(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00437b00 END

// !FUNC 0x00437cd0 BEGIN
/* 437CD0-437D54 00084 */
uchar _Globals::CDSRegKeyException_ThrowFromWin32(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00437cd0 END

// !FUNC 0x00437d60 BEGIN
/* 437D60-437E1A 000BA */
uchar _Globals::InitializeRegistryKey(HKEY__* param_1, WCHAR* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00437d60 END

// !FUNC 0x00437ed0 BEGIN
/* 437ED0-437F3C 0006C */
uchar _Globals::ThrowRegKeyException(uint param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00437ed0 END


// !FUNC 0x004382e0 BEGIN
/* 4382E0-4382ED 0000D */
uint _Globals::CDSScript_GetGlobalVar(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004382e0 END

// !FUNC 0x004382f0 BEGIN
/* 4382F0-438301 00011 */
uchar _Globals::CDSScript_SetGlobalVar(int param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004382f0 END

// !FUNC 0x004384a0 BEGIN
/* 4384A0-4384B9 00019 */
uchar _Globals::CDSScript_GetGlobalVar_004384a0(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004384a0 END

// !FUNC 0x004388a0 BEGIN
/* 4388A0-4388BD 0001D */
ushort _Globals::StrCharAt(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004388a0 END

// !FUNC 0x00438900 BEGIN
/* 438900-43891B 0001B */
uint _Globals::CDSScript_And(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438900 END

// !FUNC 0x00438920 BEGIN
/* 438920-43893B 0001B */
uint _Globals::CDSScript_Or(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438920 END

// !FUNC 0x00438960 BEGIN
/* 438960-4389EC 0008C */
uchar** _Globals::CDSScript_StrmCreateMem(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438960 END

// !FUNC 0x00438b30 BEGIN
/* 438B30-438C23 000F3 */
uint _Globals::CDSScript_Run(uint param_1, int param_2, void* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00438b30 END

// !FUNC 0x00438bef BEGIN
/* 438BEF-438C04 00015 */
uchar _Globals::Catch_00438bef() { STUB_BODY(); return 0; }
// !FUNC 0x00438bef END

// !FUNC 0x00438cb0 BEGIN
/* 438CB0-438CE6 00036 */
int _Globals::CDSScript_Rand(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438cb0 END

// !FUNC 0x00438cf0 BEGIN
/* 438CF0-438D0B 0001B */
int _Globals::CDSScript_Add(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438cf0 END

// !FUNC 0x00438d30 BEGIN
/* 438D30-438D4C 0001C */
int _Globals::CDSScript_Mul(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438d30 END

// !FUNC 0x00438d50 BEGIN
/* 438D50-438D70 00020 */
int _Globals::CDSScript_Div(CDSScript* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438d50 END

// !FUNC 0x00438f90 BEGIN
/* 438F90-438FAF 0001F */
uchar _Globals::TM_BindSequence(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438f90 END

// !FUNC 0x00438fb0 BEGIN
/* 438FB0-438FCE 0001E */
uchar _Globals::TM_RewindCurrent(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438fb0 END

// !FUNC 0x00438fd0 BEGIN
/* 438FD0-438FD8 00008 */
void _Globals::TM_SetAnimFrameFromAnimSub(int param_1) { STUB_BODY(); }
// !FUNC 0x00438fd0 END

// !FUNC 0x00438fe0 BEGIN
/* 438FE0-439009 00029 */
uchar _Globals::TM_RenderFrame(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00438fe0 END

// !FUNC 0x00439080 BEGIN
/* 439080-4390C3 00043 */
uchar _Globals::TM_TickBlit(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439080 END


// !FUNC 0x004391e0 BEGIN
/* 4391E0-43926D 0008D */
uchar _Globals::TM_SetTrack(int param_1, int param_2, uint* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004391e0 END

// !FUNC 0x00439270 BEGIN
/* 439270-43929B 0002B */
uchar _Globals::AnimInner_Init(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439270 END

// !FUNC 0x004392e0 BEGIN
/* 4392E0-439345 00065 */
uchar _Globals::AnimInner_Teardown(ODSImage* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004392e0 END

// !FUNC 0x00439350 BEGIN
/* 439350-4393CB 0007B */
void* _Globals::AnimInner_InitParam(int* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00439350 END

// !FUNC 0x00439610 BEGIN
/* 439610-439674 00064 */
uint* _Globals::CreateObject_00439610() { STUB_BODY(); return 0; }
// !FUNC 0x00439610 END

// !FUNC 0x00439680 BEGIN
/* 439680-4396E7 00067 */
CDSAnim* _Globals::CreateObject_00439680() { STUB_BODY(); return 0; }
// !FUNC 0x00439680 END



// !FUNC 0x004397e0 BEGIN
/* 4397E0-43983E 0005E */
void _Globals::TM_ShiftTrackEntries(uint* param_1, uint* param_2, int param_3) { STUB_BODY(); }
// !FUNC 0x004397e0 END

// !FUNC 0x00439840 BEGIN
/* 439840-43987D 0003D */
void _Globals::TM_PauseAndStampClock(CDSVideoPlayer* param_1) { STUB_BODY(); }
// !FUNC 0x00439840 END

// !FUNC 0x004398b0 BEGIN
/* 4398B0-4398FF 0004F */
uchar _Globals::TM_ClearTracks(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004398b0 END

// !FUNC 0x00439900 BEGIN
/* 439900-43993B 0003B */
uchar _Globals::CDSVideoPlayer_EnsureCapacity(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439900 END



// !FUNC 0x004399b0 BEGIN
/* 4399B0-439A30 00080 */
void _Globals::TM_AdvanceFrame(CDSVideoPlayer* param_1) { STUB_BODY(); }
// !FUNC 0x004399b0 END

// !FUNC 0x00439a30 BEGIN
/* 439A30-439A62 00032 */
uchar _Globals::TM_SeekToFrame(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439a30 END


// !FUNC 0x00439b40 BEGIN
/* 439B40-439B81 00041 */
void _Globals::BeginCurrentTrackPlayback(CDSVideoPlayer* param_1) { STUB_BODY(); }
// !FUNC 0x00439b40 END

// !FUNC 0x00439bc0 BEGIN
/* 439BC0-439BCF 0000F */
void _Globals::TM_ResetTrackListHead(CDSVideoPlayer* param_1) { STUB_BODY(); }
// !FUNC 0x00439bc0 END


// !FUNC 0x00439e00 BEGIN
/* 439E00-439E79 00079 */
uchar _Globals::AddTrackSource(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439e00 END

// !FUNC 0x00439e79 BEGIN
/* 439E79-439EA1 00028 */
uchar _Globals::Catch_00439e79() { STUB_BODY(); return 0; }
// !FUNC 0x00439e79 END

// !FUNC 0x00439eb0 BEGIN
/* 439EB0-439F46 00096 */
void _Globals::SetCurrentTrack(int param_1, char param_2) { STUB_BODY(); }
// !FUNC 0x00439eb0 END


// !FUNC 0x00439fe0 BEGIN
/* 439FE0-439FF2 00012 */
uchar _Globals::CDSIDSReferencedSub_ReleaseHeldRef(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439fe0 END

// !FUNC 0x0043a000 BEGIN
/* 43A000-43A029 00029 */
int _Globals::ComputeDurationMs(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043a000 END

// !FUNC 0x0043a030 BEGIN
/* 43A030-43A058 00028 */
void _Globals::CDSAudioPlayer_SetDirectSoundBufferPosition(uint param_1) { STUB_BODY(); }
// !FUNC 0x0043a030 END


// !FUNC 0x0043a0d0 BEGIN
/* 43A0D0-43A0E4 00014 */
void _Globals::CDSAudioPlayer_SetVolumePercent(uint param_1) { STUB_BODY(); }
// !FUNC 0x0043a0d0 END




// !FUNC 0x0043a590 BEGIN
/* 43A590-43A5D3 00043 */
uchar _Globals::CDSAudioPlayer_StopAll() { STUB_BODY(); return 0; }
// !FUNC 0x0043a590 END

// !FUNC 0x0043a5e0 BEGIN
/* 43A5E0-43A629 00049 */
uchar _Globals::CDSAudio_ApplyPanTable(void* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0043a5e0 END



// !FUNC 0x0043aae0 BEGIN
/* 43AAE0-43AB3D 0005D */
uchar _Globals::CDSDirectPlay_ResetMembers(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043aae0 END

// !FUNC 0x0043abc0 BEGIN
/* 43ABC0-43ABC8 00008 */
uchar _Globals::CDSDirectPlaySender_Unbind(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043abc0 END

// !FUNC 0x0043ace0 BEGIN
/* 43ACE0-43AD1E 0003E */
uchar _Globals::CDSDirectPlay_CloseSession(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043ace0 END


// !FUNC 0x0043ade5 BEGIN
/* 43ADE5-43ADFE 00019 */
uchar _Globals::Catch_0043ade5() { STUB_BODY(); return 0; }
// !FUNC 0x0043ade5 END

// !FUNC 0x0043ae20 BEGIN
/* 43AE20-43AE6B 0004B */
uchar _Globals::CDSDirectPlay_Shutdown(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043ae20 END

// !FUNC 0x0043b200 BEGIN
/* 43B200-43B264 00064 */
uint _Globals::CreateObject_0043b200() { STUB_BODY(); return 0; }
// !FUNC 0x0043b200 END

// !FUNC 0x0043b550 BEGIN
/* 43B550-43B58C 0003C */
uchar _Globals::CDSDirectPlaySender_EnsureScratchBuffer(uchar* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043b550 END

// !FUNC 0x0043b590 BEGIN
/* 43B590-43B5F7 00067 */
uint _Globals::CreateObject_0043b590() { STUB_BODY(); return 0; }
// !FUNC 0x0043b590 END

// !FUNC 0x0043b680 BEGIN
/* 43B680-43B6CF 0004F */
uchar _Globals::CDSDirectPlaySender_EnqueueSend(void* param_1, uchar* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0043b680 END

// !FUNC 0x0043b6d0 BEGIN
/* 43B6D0-43B6E5 00015 */
uchar _Globals::CDSDirectPlay_Send(uint param_1, void* param_2, uchar* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0043b6d0 END


// !FUNC 0x0043bd60 BEGIN
/* 43BD60-43BDC4 00064 */
CDSAudioVideoPlayer* _Globals::CreateObject_0043bd60() { STUB_BODY(); return 0; }
// !FUNC 0x0043bd60 END

// !FUNC 0x0043be90 BEGIN
/* 43BE90-43BEA8 00018 */
uchar _Globals::CDSQueueStream_DetachQueue(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043be90 END

// !FUNC 0x0043c1d0 BEGIN
/* 43C1D0-43C1EB 0001B */
uchar _Globals::CDSMemQueue_Init(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043c1d0 END

// !FUNC 0x0043c1f0 BEGIN
/* 43C1F0-43C1FF 0000F */
uchar _Globals::CDSMemQueue_Lock(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043c1f0 END

// !FUNC 0x0043c200 BEGIN
/* 43C200-43C20F 0000F */
uchar _Globals::CDSMemQueue_Unlock(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043c200 END

// !FUNC 0x0043c210 BEGIN
/* 43C210-43C2C6 000B6 */
uchar _Globals::CDSMemQueue_Read(void* param_1, void* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0043c210 END

// !FUNC 0x0043c2ab BEGIN
/* 43C2AB-43C2BC 00011 */
uchar _Globals::Catch_0043c2ab() { STUB_BODY(); return 0; }
// !FUNC 0x0043c2ab END

// !FUNC 0x0043c980 BEGIN
/* 43C980-43C9AB 0002B */
uint _Globals::CDSDirectSound_DuplicateSoundBuffer(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043c980 END

// !FUNC 0x0043ca90 BEGIN
/* 43CA90-43CAF4 00064 */
uint* _Globals::CreateObject_0043ca90() { STUB_BODY(); return 0; }
// !FUNC 0x0043ca90 END

// !FUNC 0x0043ce50 BEGIN
/* 43CE50-43CF01 000B1 */
uchar _Globals::BlitOpaque_Indexed1_to_Indexed1(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043ce50 END

// !FUNC 0x0043cf10 BEGIN
/* 43CF10-43CFF3 000E3 */
uchar _Globals::BlitOpaque_Indexed1_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043cf10 END

// !FUNC 0x0043d000 BEGIN
/* 43D000-43D0CE 000CE */
uchar _Globals::BlitOpaque_Indexed1_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d000 END

// !FUNC 0x0043d0d0 BEGIN
/* 43D0D0-43D194 000C4 */
uchar _Globals::BlitOpaque_Indexed1_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d0d0 END

// !FUNC 0x0043d1a0 BEGIN
/* 43D1A0-43D253 000B3 */
uchar _Globals::BlitOpaque_Indexed2_to_Indexed8(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043d1a0 END

// !FUNC 0x0043d260 BEGIN
/* 43D260-43D345 000E5 */
uchar _Globals::BlitOpaque_Indexed2_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d260 END

// !FUNC 0x0043d350 BEGIN
/* 43D350-43D420 000D0 */
uchar _Globals::BlitOpaque_Indexed2_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d350 END

// !FUNC 0x0043d420 BEGIN
/* 43D420-43D4E6 000C6 */
uchar _Globals::Blit_Opaque_2bpp_32bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d420 END

// !FUNC 0x0043d4f0 BEGIN
/* 43D4F0-43D5A1 000B1 */
uchar _Globals::BlitOpaque_Indexed4_to_Indexed8(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043d4f0 END

// !FUNC 0x0043d5b0 BEGIN
/* 43D5B0-43D693 000E3 */
uchar _Globals::FUN_0043d5b0(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d5b0 END

// !FUNC 0x0043d6a0 BEGIN
/* 43D6A0-43D767 000C7 */
uchar _Globals::Blit_Opaque_4bpp_24bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d6a0 END

// !FUNC 0x0043d770 BEGIN
/* 43D770-43D834 000C4 */
uchar _Globals::BlitOpaque_Indexed4_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d770 END

// !FUNC 0x0043d840 BEGIN
/* 43D840-43D8CC 0008C */
uchar _Globals::Blit_Opaque_Indexed8_to_Indexed8(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043d840 END

// !FUNC 0x0043d8d0 BEGIN
/* 43D8D0-43D98D 000BD */
uchar _Globals::BlitOpaque_Palette8_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d8d0 END

// !FUNC 0x0043d990 BEGIN
/* 43D990-43DA38 000A8 */
uchar _Globals::BlitOpaque_Indexed8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043d990 END

// !FUNC 0x0043da40 BEGIN
/* 43DA40-43DADE 0009E */
uchar _Globals::BlitOpaque_Indexed8_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0043da40 END

// !FUNC 0x0043dae0 BEGIN
/* 43DAE0-43DBB2 000D2 */
uchar _Globals::FUN_0043dae0(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043dae0 END

// !FUNC 0x0043dbc0 BEGIN
/* 43DBC0-43DC56 00096 */
uchar _Globals::BlitOpaque_RGB565_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043dbc0 END

// !FUNC 0x0043dc60 BEGIN
/* 43DC60-43DD2B 000CB */
uchar _Globals::Blit_Opaque_16bpp_24bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043dc60 END

// !FUNC 0x0043dd30 BEGIN
/* 43DD30-43DDF1 000C1 */
uchar _Globals::BlitOpaque_RGB565_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043dd30 END

// !FUNC 0x0043de00 BEGIN
/* 43DE00-43DEB8 000B8 */
uchar _Globals::BlitOpaque_BGR24_to_Palette8(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043de00 END

// !FUNC 0x0043dec0 BEGIN
/* 43DEC0-43DF86 000C6 */
uchar _Globals::Blit_Opaque_24bpp_16bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043dec0 END


// !FUNC 0x0043e040 BEGIN
/* 43E040-43E0E7 000A7 */
uchar _Globals::FUN_0043e040(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043e040 END

// !FUNC 0x0043e0f0 BEGIN
/* 43E0F0-43E19F 000AF */
uchar _Globals::BlitOpaque_BGRA32_to_Palette8(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043e0f0 END

// !FUNC 0x0043e1a0 BEGIN
/* 43E1A0-43E25D 000BD */
uchar _Globals::BlitOpaque_BGRA32_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043e1a0 END

// !FUNC 0x0043e260 BEGIN
/* 43E260-43E308 000A8 */
uchar _Globals::BlitOpaque_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043e260 END

// !FUNC 0x0043e310 BEGIN
/* 43E310-43E3AF 0009F */
uchar _Globals::BlitOpaque_BGRA32_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043e310 END

// !FUNC 0x0043e3b0 BEGIN
/* 43E3B0-43E498 000E8 */
uchar _Globals::BlitDestKey_Indexed1_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043e3b0 END

// !FUNC 0x0043e4a0 BEGIN
/* 43E4A0-43E571 000D1 */
uchar _Globals::BlitDestKey_Indexed1_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043e4a0 END

// !FUNC 0x0043e580 BEGIN
/* 43E580-43E647 000C7 */
uchar _Globals::Blit_DestKey_1bpp_32bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043e580 END

// !FUNC 0x0043e650 BEGIN
/* 43E650-43E73A 000EA */
uchar _Globals::BlitChromaKey_Indexed2_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043e650 END

// !FUNC 0x0043e740 BEGIN
/* 43E740-43E813 000D3 */
uchar _Globals::FUN_0043e740(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043e740 END

// !FUNC 0x0043e820 BEGIN
/* 43E820-43E8E9 000C9 */
uchar _Globals::Blit_DestKey_2bpp_32bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043e820 END

// !FUNC 0x0043e8f0 BEGIN
/* 43E8F0-43E9D8 000E8 */
uchar _Globals::BlitDestKey_Indexed4_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043e8f0 END

// !FUNC 0x0043e9e0 BEGIN
/* 43E9E0-43EAB1 000D1 */
uchar _Globals::BlitChromaKey_Indexed4_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043e9e0 END

// !FUNC 0x0043eac0 BEGIN
/* 43EAC0-43EB87 000C7 */
uchar _Globals::Blit_ChromaKey_4bpp_32bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043eac0 END

// !FUNC 0x0043eb90 BEGIN
/* 43EB90-43EC52 000C2 */
uchar _Globals::BlitDestKey_Indexed8_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043eb90 END

// !FUNC 0x0043ec60 BEGIN
/* 43EC60-43ED0B 000AB */
uchar _Globals::BlitDestKey_Idx8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043ec60 END

// !FUNC 0x0043ed10 BEGIN
/* 43ED10-43EDB3 000A3 */
uchar _Globals::Blit_DestKey_8bpp_32bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043ed10 END

// !FUNC 0x0043edc0 BEGIN
/* 43EDC0-43EE5E 0009E */
uchar _Globals::BlitDestKey_RGB565_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043edc0 END

// !FUNC 0x0043ee60 BEGIN
/* 43EE60-43EF30 000D0 */
uchar _Globals::BlitChromaKey_RGB565_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043ee60 END

// !FUNC 0x0043ef30 BEGIN
/* 43EF30-43EFF6 000C6 */
uchar _Globals::Blit_ChromaKey_16bpp_32bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043ef30 END

// !FUNC 0x0043f000 BEGIN
/* 43F000-43F0CB 000CB */
uchar _Globals::Blit_DestKey_24bpp_16bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043f000 END

// !FUNC 0x0043f0d0 BEGIN
/* 43F0D0-43F186 000B6 */
uchar _Globals::BlitDestKey_24to24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043f0d0 END

// !FUNC 0x0043f190 BEGIN
/* 43F190-43F23C 000AC */
uchar _Globals::BlitDestKey_BGR24_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043f190 END

// !FUNC 0x0043f240 BEGIN
/* 43F240-43F302 000C2 */
uchar _Globals::BlitDestKey_BGRA32_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043f240 END

// !FUNC 0x0043f310 BEGIN
/* 43F310-43F3BD 000AD */
uchar _Globals::BlitDestKey_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043f310 END

// !FUNC 0x0043f3c0 BEGIN
/* 43F3C0-43F463 000A3 */
uchar _Globals::Blit_DestKey_32bpp_32bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, int param_8) { STUB_BODY(); return 0; }
// !FUNC 0x0043f3c0 END

// !FUNC 0x0043f470 BEGIN
/* 43F470-43F62D 001BD */
uchar _Globals::BlitChromaKey(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x0043f470 END

// !FUNC 0x0043f630 BEGIN
/* 43F630-43F7B8 00188 */
uchar _Globals::BlitChromaKey_Indexed1_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x0043f630 END

// !FUNC 0x0043f7c0 BEGIN
/* 43F7C0-43F935 00175 */
uchar _Globals::BlitChromaKey_PAL1_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x0043f7c0 END

// !FUNC 0x0043f940 BEGIN
/* 43F940-43FAFF 001BF */
uchar _Globals::BlitChromaKey_2bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x0043f940 END

// !FUNC 0x0043fb00 BEGIN
/* 43FB00-43FC8A 0018A */
uchar _Globals::BlitMasked_Idx2_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x0043fb00 END

// !FUNC 0x0043fc90 BEGIN
/* 43FC90-43FE07 00177 */
uchar _Globals::BlitChromaKey_PAL2_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x0043fc90 END

// !FUNC 0x0043fe10 BEGIN
/* 43FE10-43FFCD 001BD */
uchar _Globals::BlitChromaKey_4bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x0043fe10 END

// !FUNC 0x0043ffd0 BEGIN
/* 43FFD0-440158 00188 */
uchar _Globals::BlitChromaKey_Indexed4_to_BGR24_0043ffd0(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x0043ffd0 END

// !FUNC 0x00440160 BEGIN
/* 440160-4402D5 00175 */
uchar _Globals::BlitChromaKey_PAL4_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00440160 END

// !FUNC 0x004402e0 BEGIN
/* 4402E0-440477 00197 */
uchar _Globals::BlitChromaKey_Indexed8_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x004402e0 END

// !FUNC 0x00440480 BEGIN
/* 440480-4405E2 00162 */
uchar _Globals::BlitChromaKey_PAL8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00440480 END

// !FUNC 0x004405f0 BEGIN
/* 4405F0-44073F 0014F */
uchar _Globals::BlitChromaKey_Idx8_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x004405f0 END

// !FUNC 0x00440740 BEGIN
/* 440740-4408FA 001BA */
uchar _Globals::BlitChromaKey_RGB565_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00440740 END

// !FUNC 0x00440900 BEGIN
/* 440900-440A85 00185 */
uchar _Globals::BlitChromaKey_RGB565_to_BGR24_00440900(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00440900 END

// !FUNC 0x00440a90 BEGIN
/* 440A90-440C02 00172 */
uchar _Globals::BlitChromaKey_RGB565_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00440a90 END

// !FUNC 0x00440c10 BEGIN
/* 440C10-440DB0 001A0 */
uchar _Globals::BlitAlphaBlend_BGR24_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00440c10 END

// !FUNC 0x00440db0 BEGIN
/* 440DB0-440F1B 0016B */
uchar _Globals::BlitChromaKey_BGR24_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00440db0 END

// !FUNC 0x00440f20 BEGIN
/* 440F20-441078 00158 */
uchar _Globals::BlitChromaKey_BGR24_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00440f20 END

// !FUNC 0x00441080 BEGIN
/* 441080-441217 00197 */
uchar _Globals::BlitChromaKey_BGRA32_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00441080 END

// !FUNC 0x00441220 BEGIN
/* 441220-441382 00162 */
uchar _Globals::BlitChromaKey_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00441220 END

// !FUNC 0x00441390 BEGIN
/* 441390-4414DF 0014F */
uchar _Globals::BlitChromaKey_BGRA32_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, uint param_9) { STUB_BODY(); return 0; }
// !FUNC 0x00441390 END

// !FUNC 0x004414e0 BEGIN
/* 4414E0-44169B 001BB */
uchar _Globals::BlitMasked(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x004414e0 END

// !FUNC 0x004416a0 BEGIN
/* 4416A0-441826 00186 */
uchar _Globals::BlitMasked_Indexed1_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x004416a0 END

// !FUNC 0x00441830 BEGIN
/* 441830-4419A3 00173 */
uchar _Globals::BlitMasked_PAL1_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00441830 END

// !FUNC 0x004419b0 BEGIN
/* 4419B0-441B6D 001BD */
uchar _Globals::BlitKeyAndMask_2bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x004419b0 END

// !FUNC 0x00441b70 BEGIN
/* 441B70-441CF8 00188 */
uchar _Globals::BlitMasked_Indexed2_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00441b70 END

// !FUNC 0x00441d00 BEGIN
/* 441D00-441E75 00175 */
uchar _Globals::BlitMasked_PAL2_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00441d00 END

// !FUNC 0x00441e80 BEGIN
/* 441E80-44203B 001BB */
uchar _Globals::BlitMasked_PAL4_to_RGB565(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00441e80 END

// !FUNC 0x00442040 BEGIN
/* 442040-4421C6 00186 */
uchar _Globals::BlitMasked_Indexed4_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00442040 END

// !FUNC 0x004421d0 BEGIN
/* 4421D0-442343 00173 */
uchar _Globals::BlitMasked_PAL4_to_BGRA32(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x004421d0 END

// !FUNC 0x00442350 BEGIN
/* 442350-4424E5 00195 */
uchar _Globals::BlitMasked_Idx8_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00442350 END

// !FUNC 0x004424f0 BEGIN
/* 4424F0-442650 00160 */
uchar _Globals::BlitMasked_Idx8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x004424f0 END

// !FUNC 0x00442650 BEGIN
/* 442650-44279D 0014D */
uchar _Globals::BlitMasked_Idx8_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00442650 END

// !FUNC 0x004427a0 BEGIN
/* 4427A0-442958 001B8 */
uchar _Globals::BlitMasked_RGB565_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x004427a0 END

// !FUNC 0x00442960 BEGIN
/* 442960-442AE3 00183 */
uchar _Globals::BlitMasked_RGB565_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00442960 END

// !FUNC 0x00442af0 BEGIN
/* 442AF0-442C60 00170 */
uchar _Globals::BlitMasked_RGB565_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00442af0 END

// !FUNC 0x00442c60 BEGIN
/* 442C60-442DFE 0019E */
uchar _Globals::BlitMasked_BGR24_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00442c60 END

// !FUNC 0x00442e00 BEGIN
/* 442E00-442F69 00169 */
uchar _Globals::BlitMasked_BGR24_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00442e00 END

// !FUNC 0x00442f70 BEGIN
/* 442F70-4430C2 00152 */
uchar _Globals::BlitMasked_BGR24_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00442f70 END

// !FUNC 0x004430d0 BEGIN
/* 4430D0-443265 00195 */
uchar _Globals::BlitMasked_BGRA32_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x004430d0 END

// !FUNC 0x00443270 BEGIN
/* 443270-4433D0 00160 */
uchar _Globals::BlitMasked_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x00443270 END

// !FUNC 0x004433d0 BEGIN
/* 4433D0-443519 00149 */
uchar _Globals::BlitMasked_BGRA32_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10) { STUB_BODY(); return 0; }
// !FUNC 0x004433d0 END

// !FUNC 0x00443520 BEGIN
/* 443520-4436F2 001D2 */
uchar _Globals::BlitKeyAndMask1bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00443520 END

// !FUNC 0x00443700 BEGIN
/* 443700-44389D 0019D */
uchar _Globals::BlitKeyAndMask_1bpp_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00443700 END

// !FUNC 0x004438a0 BEGIN
/* 4438A0-443A2A 0018A */
uchar _Globals::FUN_004438a0(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x004438a0 END

// !FUNC 0x00443a30 BEGIN
/* 443A30-443C04 001D4 */
uchar _Globals::BlitKeyAndMask2bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00443a30 END

// !FUNC 0x00443c10 BEGIN
/* 443C10-443DAF 0019F */
uchar _Globals::BlitAlphaBlend_2bitIndex_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00443c10 END

// !FUNC 0x00443db0 BEGIN
/* 443DB0-443F3C 0018C */
uchar _Globals::FUN_00443db0(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00443db0 END

// !FUNC 0x00443f40 BEGIN
/* 443F40-444112 001D2 */
uchar _Globals::BlitKeyAndMask4bpp(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00443f40 END

// !FUNC 0x00444120 BEGIN
/* 444120-4442BD 0019D */
uchar _Globals::BlitKeyAndMask_4bpp_to_BGR24(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00444120 END

// !FUNC 0x004442c0 BEGIN
/* 4442C0-44444A 0018A */
uchar _Globals::FUN_004442c0(int* param_1, uint* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x004442c0 END

// !FUNC 0x00444450 BEGIN
/* 444450-4445FC 001AC */
uchar _Globals::BlitAlphaBlend_ByteIndex_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00444450 END

// !FUNC 0x00444600 BEGIN
/* 444600-444777 00177 */
uchar _Globals::BlitKeyAndMask_PAL8_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00444600 END

// !FUNC 0x00444780 BEGIN
/* 444780-4448E4 00164 */
uchar _Globals::BlitKeyAndMask_PAL8_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, int param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00444780 END

// !FUNC 0x004448f0 BEGIN
/* 4448F0-444ABF 001CF */
uchar _Globals::BlitKeyAndMask16bpp(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x004448f0 END

// !FUNC 0x00444ac0 BEGIN
/* 444AC0-444C5A 0019A */
uchar _Globals::BlitKeyAndMask_RGB565_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00444ac0 END

// !FUNC 0x00444c60 BEGIN
/* 444C60-444DE7 00187 */
uchar _Globals::BlitKeyAndMask_RGB565_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00444c60 END

// !FUNC 0x00444df0 BEGIN
/* 444DF0-444FA5 001B5 */
uchar _Globals::BlitKeyAndMask_BGR24_to_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00444df0 END

// !FUNC 0x00444fb0 BEGIN
/* 444FB0-445130 00180 */
uchar _Globals::BlitMasked_BGR24_to_BGR24_00444fb0(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00444fb0 END

// !FUNC 0x00445130 BEGIN
/* 445130-44529D 0016D */
uchar _Globals::BlitKeyAndMask_BGR24_to_BGR32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00445130 END

// !FUNC 0x004452a0 BEGIN
/* 4452A0-44544C 001AC */
uchar _Globals::BlitAlphaBlend_DwordIndex_RGB565(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x004452a0 END

// !FUNC 0x00445450 BEGIN
/* 445450-4455C7 00177 */
uchar _Globals::BlitKeyAndMask_BGRA32_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x00445450 END

// !FUNC 0x004455d0 BEGIN
/* 4455D0-445734 00164 */
uchar _Globals::BlitKeyAndMask_BGRA32_to_BGRA32(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6, uint param_7, uint param_8, int param_9, int param_10, uint param_11) { STUB_BODY(); return 0; }
// !FUNC 0x004455d0 END

// !FUNC 0x00446620 BEGIN
/* 446620-44663A 0001A */
uchar _Globals::CDSMpx_FinishMadFrame(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00446620 END

// !FUNC 0x00446c30 BEGIN
/* 446C30-446C96 00066 */
void _Globals::CDSSafeStream_ClearThreadSlices(CDSSafeStream* param_1) { STUB_BODY(); }
// !FUNC 0x00446c30 END

// !FUNC 0x004470c0 BEGIN
/* 4470C0-4470E6 00026 */
bool _Globals::CDSWorkingThread_PollExited(DWORD param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004470c0 END

// !FUNC 0x004470f0 BEGIN
/* 4470F0-44712E 0003E */
uchar _Globals::CDSWorkingThread_ThreadProc(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004470f0 END

// !FUNC 0x0044712e BEGIN
/* 44712E-447144 00016 */
uchar* _Globals::Catch_0044712e() { STUB_BODY(); return 0; }
// !FUNC 0x0044712e END

// !FUNC 0x0044714a BEGIN
/* 44714A-44716D 00023 */
uint _Globals::FUN_0044714a() { STUB_BODY(); return 0; }
// !FUNC 0x0044714a END

// !FUNC 0x00447190 BEGIN
/* 447190-4471C8 00038 */
uchar _Globals::CDSWorkingThread_StopAndJoin(DWORD param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00447190 END

// !FUNC 0x004473e8 BEGIN
/* 4473E8-4473F1 00009 */
uchar _Globals::FUN_004473e8() { STUB_BODY(); return 0; }
// !FUNC 0x004473e8 END

// !FUNC 0x0044751c BEGIN
/* 44751C-447525 00009 */
uchar _Globals::FUN_0044751c() { STUB_BODY(); return 0; }
// !FUNC 0x0044751c END

// !FUNC 0x004476c1 BEGIN
/* 4476C1-4476CA 00009 */
uchar _Globals::FUN_004476c1() { STUB_BODY(); return 0; }
// !FUNC 0x004476c1 END

// !FUNC 0x00447897 BEGIN
/* 447897-4478CD 00036 */
uint _Globals::FID_conflict_CxxFrameHandler3(EHExceptionRecord* param_1, EHRegistrationNode* param_2, void* param_3, void* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00447897 END

// !FUNC 0x00447c42 BEGIN
/* 447C42-447CAB 00069 */
uchar _Globals::OperatorNewWithBadAlloc(size_t param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00447c42 END

// !FUNC 0x00447e6c BEGIN
/* 447E6C-447E72 00006 */
uchar _Globals::FUN_00447e6c() { STUB_BODY(); return 0; }
// !FUNC 0x00447e6c END

// !FUNC 0x004486a4 BEGIN
/* 4486A4-4486BC 00018 */
uchar _Globals::FUN_004486a4() { STUB_BODY(); return 0; }
// !FUNC 0x004486a4 END

// !FUNC 0x00448765 BEGIN
/* 448765-44877D 00018 */
uchar _Globals::FUN_00448765() { STUB_BODY(); return 0; }
// !FUNC 0x00448765 END

// !FUNC 0x004489d1 BEGIN
/* 4489D1-448A04 00033 */
errno_t _Globals::FID_conflict_wcslwr_s_l(wchar_t* param_1, size_t param_2, localeinfo_struct* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004489d1 END

// !FUNC 0x00448a97 BEGIN
/* 448A97-448A98 00001 */
void _Globals::FUN_00448a97() { STUB_BODY(); }
// !FUNC 0x00448a97 END

// !FUNC 0x00448bfb BEGIN
/* 448BFB-448C1C 00021 */
uchar _Globals::fast_error_exit(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00448bfb END

// !FUNC 0x00448c1f BEGIN
/* 448C1F-448C60 00041 */
uint _Globals::check_managed_app() { STUB_BODY(); return 0; }
// !FUNC 0x00448c1f END

// !FUNC 0x00449386 BEGIN
/* 449386-44938F 00009 */
uchar _Globals::FUN_00449386() { STUB_BODY(); return 0; }
// !FUNC 0x00449386 END

// !FUNC 0x004494ec BEGIN
/* 4494EC-4494F5 00009 */
uchar _Globals::FUN_004494ec() { STUB_BODY(); return 0; }
// !FUNC 0x004494ec END

// !FUNC 0x00449b1b BEGIN
/* 449B1B-449B30 00015 */
void _Globals::_unlock(int param_1) { STUB_BODY(); }
// !FUNC 0x00449b1b END

// !FUNC 0x00449bea BEGIN
/* 449BEA-449BF3 00009 */
uchar _Globals::FUN_00449bea() { STUB_BODY(); return 0; }
// !FUNC 0x00449bea END

// !FUNC 0x00449c90 BEGIN
/* 449C90-449CA9 00019 */
uint* _Globals::_sbh_find_block(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00449c90 END

// !FUNC 0x0044a916 BEGIN
/* 44A916-44A920 0000A */
uchar _Globals::FUN_0044a916(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0044a916 END

// !FUNC 0x0044a942 BEGIN
/* 44A942-44A94C 0000A */
uchar _Globals::FUN_0044a942(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0044a942 END

// !FUNC 0x0044ab44 BEGIN
/* 44AB44-44AB59 00015 */
uchar _Globals::FUN_0044ab44(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0044ab44 END

// !FUNC 0x0044ab59 BEGIN
/* 44AB59-44AB5F 00006 */
uint _Globals::_get_flsindex() {
    return *reinterpret_cast<uint*>(&DAT_004b1158);
}
// !FUNC 0x0044ab59 END

// !FUNC 0x0044ac8a BEGIN
/* 44AC8A-44AC93 00009 */
uchar _Globals::FUN_0044ac8a() { STUB_BODY(); return 0; }
// !FUNC 0x0044ac8a END

// !FUNC 0x0044ae3a BEGIN
/* 44AE3A-44AE43 00009 */
uchar _Globals::FUN_0044ae3a() { STUB_BODY(); return 0; }
// !FUNC 0x0044ae3a END

// !FUNC 0x0044ae46 BEGIN
/* 44AE46-44AE4F 00009 */
uchar _Globals::FUN_0044ae46() { STUB_BODY(); return 0; }
// !FUNC 0x0044ae46 END

// !FUNC 0x0044b1cf BEGIN
/* 44B1CF-44B1EA 0001B */
uchar _Globals::FUN_0044b1cf() { STUB_BODY(); return 0; }
// !FUNC 0x0044b1cf END

// !FUNC 0x0044b352 BEGIN
/* 44B352-44B369 00017 */
uchar _Globals::Catch_All_0044b352() { STUB_BODY(); return 0; }
// !FUNC 0x0044b352 END

// !FUNC 0x0044b490 BEGIN
/* 44B490-44B506 00076 */
uchar _Globals::FUN_0044b490() { STUB_BODY(); return 0; }
// !FUNC 0x0044b490 END

// !FUNC 0x0044bfad BEGIN
/* 44BFAD-44BFFC 0004F */
uchar _Globals::fastzero_I(void* param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0044bfad END

// !FUNC 0x0044c19e BEGIN
/* 44C19E-44C1C4 00026 */
void _Globals::_crtCorExitProcess(int param_1) { STUB_BODY(); }
// !FUNC 0x0044c19e END

// !FUNC 0x0044c1d9 BEGIN
/* 44C1D9-44C1E2 00009 */
uchar _Globals::FUN_0044c1d9() { STUB_BODY(); return 0; }
// !FUNC 0x0044c1d9 END

// !FUNC 0x0044c1e2 BEGIN
/* 44C1E2-44C1EB 00009 */
uchar _Globals::FUN_0044c1e2() { STUB_BODY(); return 0; }
// !FUNC 0x0044c1e2 END

// !FUNC 0x0044c328 BEGIN
/* 44C328-44C3E4 000BC */
uchar _Globals::doexit(int param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0044c328 END

// !FUNC 0x0044c3e1 BEGIN
/* 44C3E1-44C3F0 0000F */
uchar _Globals::FUN_0044c3e1() { STUB_BODY(); return 0; }
// !FUNC 0x0044c3e1 END

// !FUNC 0x0044c482 BEGIN
/* 44C482-44C503 00081 */
uchar _Globals::FUN_0044c482(uint* param_1, uint* param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0044c482 END

// !FUNC 0x0044c895 BEGIN
/* 44C895-44C89E 00009 */
uchar _Globals::FUN_0044c895() { STUB_BODY(); return 0; }
// !FUNC 0x0044c895 END

// !FUNC 0x0044cc24 BEGIN
/* 44CC24-44CC2D 00009 */
uchar _Globals::FUN_0044cc24() { STUB_BODY(); return 0; }
// !FUNC 0x0044cc24 END

// !FUNC 0x0044d594 BEGIN
/* 44D594-44D59E 0000A */
uchar _Globals::FUN_0044d594(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0044d594 END

// !FUNC 0x0044e3a0 BEGIN
/* 44E3A0-44E538 00198 */
uchar _Globals::parse_cmdline(uint* param_1, uchar* param_2, int* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0044e3a0 END

// !FUNC 0x0044ef20 BEGIN
/* 44EF20-44EF45 00025 */
uchar _Globals::write_char(wchar_t param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0044ef20 END

// !FUNC 0x0044ef45 BEGIN
/* 44EF45-44EF6A 00025 */
uchar _Globals::write_multi_char(wchar_t param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0044ef45 END

// !FUNC 0x0044ef6a BEGIN
/* 44EF6A-44EFB9 0004F */
uchar _Globals::write_string(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0044ef6a END

// !FUNC 0x0044faab BEGIN
/* 44FAAB-44FAB5 0000A */
uchar _Globals::FUN_0044faab(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0044faab END

// !FUNC 0x0044fcdf BEGIN
/* 44FCDF-44FCE7 00008 */
void _Globals::FUN_0044fcdf() { STUB_BODY(); }
// !FUNC 0x0044fcdf END

// !FUNC 0x0044fe54 BEGIN
/* 44FE54-44FE57 00003 */
uchar _Globals::FUN_0044fe54() { STUB_BODY(); return 0; }
// !FUNC 0x0044fe54 END

// !FUNC 0x0044fe57 BEGIN
/* 44FE57-44FE84 0002D */
uint _Globals::FUN_0044fe57() { STUB_BODY(); return 0; }
// !FUNC 0x0044fe57 END

// !FUNC 0x0044ff2d BEGIN
/* 44FF2D-44FF61 00034 */
uint _Globals::siglookup(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0044ff2d END

// !FUNC 0x004500e2 BEGIN
/* 4500E2-4500F1 0000F */
uchar _Globals::FUN_004500e2() { STUB_BODY(); return 0; }
// !FUNC 0x004500e2 END

// !FUNC 0x0045011e BEGIN
/* 45011E-450128 0000A */
void _Globals::FUN_0045011e(uint param_1) { STUB_BODY(); }
// !FUNC 0x0045011e END

// !FUNC 0x004510db BEGIN
/* 4510DB-451181 000A6 */
int _Globals::FID_conflict_atoflt_l(_CRT_FLOAT* param_1, char* param_2, localeinfo_struct* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004510db END

// !FUNC 0x00451181 BEGIN
/* 451181-451227 000A6 */
int _Globals::FID_conflict_atoflt_l_00451181(_CRT_FLOAT* param_1, char* param_2, localeinfo_struct* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00451181 END

// !FUNC 0x00451825 BEGIN
/* 451825-451D67 00542 */
int _Globals::FID_conflict_ld12tod(_LDBL12* param_1, _CRT_DOUBLE* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00451825 END

// !FUNC 0x00451d67 BEGIN
/* 451D67-4522A9 00542 */
int _Globals::FID_conflict_ld12tod_00451d67(_LDBL12* param_1, _CRT_DOUBLE* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00451d67 END

// !FUNC 0x00452869 BEGIN
/* 452869-452873 0000A */
void _Globals::FUN_00452869() { STUB_BODY(); }
// !FUNC 0x00452869 END

// !FUNC 0x00452f0b BEGIN
/* 452F0B-452F15 0000A */
uchar _Globals::FUN_00452f0b() { STUB_BODY(); return 0; }
// !FUNC 0x00452f0b END

// !FUNC 0x00452fb7 BEGIN
/* 452FB7-452FBD 00006 */
uchar** _Globals::_iob_func() {
    return reinterpret_cast<uchar**>(&PTR_DAT_004b1d60);
}
// !FUNC 0x00452fb7 END

// !FUNC 0x00453c4c BEGIN
/* 453C4C-45450E 008C2 */
uint _Globals::I10_OUTPUT(int param_1, uint param_2, ushort param_3, int param_4, uchar param_5, short* param_6) { STUB_BODY(); return 0; }
// !FUNC 0x00453c4c END

// !FUNC 0x00454ace BEGIN
/* 454ACE-454AD7 00009 */
void _Globals::_unlock_10() { STUB_BODY(); }
// !FUNC 0x00454ace END

// !FUNC 0x00454ad7 BEGIN
/* 454AD7-454AF9 00022 */
uchar _Globals::MSVCRT_unlock_fhandle(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00454ad7 END

// !FUNC 0x00456800 BEGIN
/* 456800-45681E 0001E */
uchar _Globals::CDSMpx_FreeMadAuxAlloc(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00456800 END

// !FUNC 0x00458f20 BEGIN
/* 458F20-458F44 00024 */
void _Globals::mad_frame_finish(int* param_1) { STUB_BODY(); }
// !FUNC 0x00458f20 END

// !FUNC 0x00459900 BEGIN
/* 459900-459B71 00271 */
int _Globals::III_sideinfo(uint param_1, int param_2, uint* param_3, int* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00459900 END

// !FUNC 0x00459b80 BEGIN
/* 459B80-459E44 002C4 */
uchar _Globals::decode_mcu(int* param_1, int param_2, int param_3, uchar* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00459b80 END

// !FUNC 0x00459e50 BEGIN
/* 459E50-45A049 001F9 */
uchar _Globals::III_scalefactors(int param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00459e50 END

// !FUNC 0x0045a060 BEGIN
/* 45A060-45A20C 001AC */
uchar _Globals::III_exponents(uchar* param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045a060 END

// !FUNC 0x0045a220 BEGIN
/* 45A220-45A2B9 00099 */
uint _Globals::III_huffdec_fixed_mul(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045a220 END

// !FUNC 0x0045a2c0 BEGIN
/* 45A2C0-45A9C5 00705 */
uint _Globals::III_huffdecode(uint* param_1, ushort* param_2, uchar* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0045a2c0 END

// !FUNC 0x0045a9d0 BEGIN
/* 45A9D0-45AAF2 00122 */
uchar _Globals::III_reorder(int param_1, uchar* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045a9d0 END

// !FUNC 0x0045ab00 BEGIN
/* 45AB00-45AF96 00496 */
uint _Globals::III_stereo(int param_1, int param_2, int param_3, uchar* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0045ab00 END

// !FUNC 0x0045afb0 BEGIN
/* 45AFB0-45B074 000C4 */
uchar _Globals::III_aliasreduce(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045afb0 END

// !FUNC 0x0045b080 BEGIN
/* 45B080-45B246 001C6 */
uchar _Globals::fastsdct(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045b080 END

// !FUNC 0x0045b250 BEGIN
/* 45B250-45B364 00114 */
uchar _Globals::imdct36(int param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045b250 END

// !FUNC 0x0045b370 BEGIN
/* 45B370-45B477 00107 */
uchar _Globals::dctIV(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045b370 END

// !FUNC 0x0045b480 BEGIN
/* 45B480-45B52B 000AB */
void _Globals::FUN_0045b480(int* param_1) { STUB_BODY(); }
// !FUNC 0x0045b480 END

// !FUNC 0x0045b530 BEGIN
/* 45B530-45B792 00262 */
void _Globals::III_imdct_l(uint param_1) { STUB_BODY(); }
// !FUNC 0x0045b530 END

// !FUNC 0x0045b7a0 BEGIN
/* 45B7A0-45BA80 002E0 */
uchar _Globals::III_imdct_s(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045b7a0 END

// !FUNC 0x0045ba90 BEGIN
/* 45BA90-45BB31 000A1 */
uchar _Globals::III_overlap(int param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0045ba90 END

// !FUNC 0x0045bb40 BEGIN
/* 45BB40-45BBA3 00063 */
uchar _Globals::III_freqinver() { STUB_BODY(); return 0; }
// !FUNC 0x0045bb40 END

// !FUNC 0x0045bbb0 BEGIN
/* 45BBB0-45C0DC 0052C */
int _Globals::III_decode(int* param_1, int param_2, int param_3, uint param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0045bbb0 END

// !FUNC 0x0045d160 BEGIN
/* 45D160-45D199 00039 */
uchar _Globals::jpeg_abort(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045d160 END

// !FUNC 0x0045d1a0 BEGIN
/* 45D1A0-45D1C1 00021 */
uchar _Globals::jpeg_destroy(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045d1a0 END

// !FUNC 0x0045d1d0 BEGIN
/* 45D1D0-45D1EE 0001E */
void* _Globals::jpeg_alloc_quant_table(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045d1d0 END

// !FUNC 0x0045d1f0 BEGIN
/* 45D1F0-45D20E 0001E */
uchar _Globals::alloc_small(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045d1f0 END

// !FUNC 0x0045d210 BEGIN
/* 45D210-45D29C 0008C */
uchar _Globals::get_soi() { STUB_BODY(); return 0; }
// !FUNC 0x0045d210 END

// !FUNC 0x0045d2a0 BEGIN
/* 45D2A0-45D55D 002BD */
uint _Globals::get_sof(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045d2a0 END

// !FUNC 0x0045d560 BEGIN
/* 45D560-45D7CA 0026A */
uint _Globals::get_sos() { STUB_BODY(); return 0; }
// !FUNC 0x0045d560 END

// !FUNC 0x0045d7d0 BEGIN
/* 45D7D0-45DA44 00274 */
uchar _Globals::get_dht(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045d7d0 END

// !FUNC 0x0045da50 BEGIN
/* 45DA50-45DC3E 001EE */
uchar _Globals::get_dqt(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045da50 END

// !FUNC 0x0045dc40 BEGIN
/* 45DC40-45DD19 000D9 */
uchar _Globals::get_dri(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045dc40 END

// !FUNC 0x0045dd20 BEGIN
/* 45DD20-45DD9A 0007A */
void _Globals::examine_app0(uint param_1) { STUB_BODY(); }
// !FUNC 0x0045dd20 END

// !FUNC 0x0045dda0 BEGIN
/* 45DDA0-45DDD6 00036 */
uchar _Globals::process_APP14(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045dda0 END

// !FUNC 0x0045dde0 BEGIN
/* 45DDE0-45DF5D 0017D */
uchar _Globals::save_marker(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045dde0 END

// !FUNC 0x0045df60 BEGIN
/* 45DF60-45DFD7 00077 */
uchar _Globals::skip_variable(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045df60 END

// !FUNC 0x0045dfe0 BEGIN
/* 45DFE0-45E0B9 000D9 */
uint _Globals::next_marker() { STUB_BODY(); return 0; }
// !FUNC 0x0045dfe0 END

// !FUNC 0x0045e0c0 BEGIN
/* 45E0C0-45E167 000A7 */
uint _Globals::first_marker(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045e0c0 END

// !FUNC 0x0045e170 BEGIN
/* 45E170-45E387 00217 */
uint _Globals::read_markers(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045e170 END

// !FUNC 0x0045e4d0 BEGIN
/* 45E4D0-45E535 00065 */
uint _Globals::read_restart_marker(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045e4d0 END

// !FUNC 0x0045e540 BEGIN
/* 45E540-45E5E7 000A7 */
uint _Globals::jpeg_resync_to_restart(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045e540 END

// !FUNC 0x0045e5f0 BEGIN
/* 45E5F0-45E61B 0002B */
uchar _Globals::reset_marker_reader(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045e5f0 END

// !FUNC 0x0045e620 BEGIN
/* 45E620-45E694 00074 */
uchar _Globals::jinit_marker_reader(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045e620 END

// !FUNC 0x0045e6a0 BEGIN
/* 45E6A0-45E787 000E7 */
// libjpeg-6b: jpeg_CreateDecompress (jdapimin.c) -- version/struct guards + init.
uchar _Globals::jpeg_CreateDecompress(int* param_1, int param_2, int param_3) {
    int iVar1;
    int* err;

    param_1[1] = 0;
    if (param_2 != 0x3e) {
        err = reinterpret_cast<int*>(*param_1);
        err[2] = 0xc;
        err[3] = 0x3e;
        err[4] = param_2;
        reinterpret_cast<void(__cdecl*)(int*)>(*err)(param_1);
    }
    if (param_3 != 0x1b0) {
        err = reinterpret_cast<int*>(*param_1);
        err[2] = 0x15;
        err[3] = 0x1b0;
        err[4] = param_3;
        reinterpret_cast<void(__cdecl*)(int*)>(*err)(param_1);
    }
    iVar1 = *param_1;
    Runtime::MSVCRT::_memset(param_1, 0, 0x1b0);
    *param_1 = iVar1;
    *reinterpret_cast<uchar*>(reinterpret_cast<char*>(param_1) + 0x10) = 1;
    _Globals::jinit_memory_mgr((int)param_1);
    param_1[6] = 0;
    param_1[0x24] = 0;
    param_1[0x25] = 0;
    param_1[0x26] = 0;
    param_1[0x27] = 0;
    param_1[0x28] = 0;
    param_1[0x2c] = 0;
    param_1[0x29] = 0;
    param_1[0x2d] = 0;
    param_1[0x2a] = 0;
    param_1[0x2e] = 0;
    param_1[0x2b] = 0;
    param_1[0x2f] = 0;
    param_1[0x43] = 0;
    _Globals::jinit_marker_reader((int)param_1);
    _Globals::jinit_input_controller((int)param_1);
    param_1[5] = 200;
    return 0;
}
// !FUNC 0x0045e6a0 END

// !FUNC 0x0045e7a0 BEGIN
/* 45E7A0-45E8C5 00125 */
uchar _Globals::default_decompress_parms() { STUB_BODY(); return 0; }
// !FUNC 0x0045e7a0 END

// !FUNC 0x0045e8d0 BEGIN
/* 45E8D0-45E973 000A3 */
int _Globals::jpeg_consume_input(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045e8d0 END

// !FUNC 0x0045e9a0 BEGIN
/* 45E9A0-45EA68 000C8 */
uint _Globals::jpeg_finish_decompress(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045e9a0 END

// !FUNC 0x0045ea70 BEGIN
/* 45EA70-45EAE7 00077 */
int _Globals::jpeg_read_header(int* param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045ea70 END

// !FUNC 0x0045eaf0 BEGIN
/* 45EAF0-45EB9A 000AA */
uint _Globals::output_pass_setup() { STUB_BODY(); return 0; }
// !FUNC 0x0045eaf0 END

// !FUNC 0x0045eba0 BEGIN
/* 45EBA0-45EC04 00064 */
int _Globals::jpeg_read_scanlines(int* param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0045eba0 END

// !FUNC 0x0045ec10 BEGIN
/* 45EC10-45ECB5 000A5 */
uint _Globals::jpeg_start_decompress(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045ec10 END

// !FUNC 0x0045f7e0 BEGIN
/* 45F7E0-45F7F0 00010 */
// libjpeg-6b: jdiv_round_up (jutils.c) -- ceil(a/b) for a>=0, b>0.
int _Globals::jdiv_round_up(int param_1, int param_2) {
    return (param_1 + param_2 - 1) / param_2;
}
// !FUNC 0x0045f7e0 END

// !FUNC 0x0045f7f0 BEGIN
/* 45F7F0-45F808 00018 */
int _Globals::jround_up(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045f7f0 END

// !FUNC 0x0045f810 BEGIN
/* 45F810-45F857 00047 */
uchar _Globals::jcopy_sample_rows(int param_1, int param_2, int param_3, int param_4, int param_5, size_t param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0045f810 END

// !FUNC 0x0045f860 BEGIN
/* 45F860-45F87B 0001B */
void _Globals::jcopy_block_row(void* param_1, void* param_2, int param_3) { STUB_BODY(); }
// !FUNC 0x0045f860 END

// !FUNC 0x0045f880 BEGIN
/* 45F880-45F895 00015 */
void _Globals::IJG_jzero_far(void* param_1, size_t param_2) { STUB_BODY(); }
// !FUNC 0x0045f880 END

// !FUNC 0x0045f8a0 BEGIN
/* 45F8A0-45FA94 001F4 */
uchar _Globals::initial_setup() { STUB_BODY(); return 0; }
// !FUNC 0x0045f8a0 END

// !FUNC 0x0045faa0 BEGIN
/* 45FAA0-45FC3F 0019F */
uint _Globals::per_scan_setup() { STUB_BODY(); return 0; }
// !FUNC 0x0045faa0 END

// !FUNC 0x0045fc50 BEGIN
/* 45FC50-45FCF1 000A1 */
uchar _Globals::select_scan_quant_tables() { STUB_BODY(); return 0; }
// !FUNC 0x0045fc50 END

// !FUNC 0x0045fd00 BEGIN
/* 45FD00-45FD3F 0003F */
uchar _Globals::jpeg_consume_markers(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045fd00 END

// !FUNC 0x0045fd40 BEGIN
/* 45FD40-45FDF9 000B9 */
int _Globals::jpeg_reset_input_controller(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045fd40 END

// !FUNC 0x0045fe00 BEGIN
/* 45FE00-45FE3F 0003F */
uchar _Globals::jpeg_start_input_pass(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045fe00 END

// !FUNC 0x0045fe60 BEGIN
/* 45FE60-45FEA3 00043 */
uchar _Globals::jinit_input_controller(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045fe60 END

// !FUNC 0x0045feb0 BEGIN
/* 45FEB0-45FF25 00075 */
uint _Globals::use_merged_upsample(uint param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045feb0 END

// !FUNC 0x0045ff30 BEGIN
/* 45FF30-460143 00213 */
uchar _Globals::jinit_d_main_controller(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045ff30 END

// !FUNC 0x00460160 BEGIN
/* 460160-4601EE 0008E */
void _Globals::prepare_range_limit_table(void* param_1) { STUB_BODY(); }
// !FUNC 0x00460160 END

// !FUNC 0x00460200 BEGIN
/* 460200-460378 00178 */
uchar _Globals::master_selection() { STUB_BODY(); return 0; }
// !FUNC 0x00460200 END

// !FUNC 0x00460380 BEGIN
/* 460380-460492 00112 */
uchar _Globals::post_process_1pass(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00460380 END

// !FUNC 0x004604d0 BEGIN
/* 4604D0-460502 00032 */
uchar _Globals::jinit_d_post_controller(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004604d0 END

// !FUNC 0x00460510 BEGIN
/* 460510-460545 00035 */
uchar _Globals::emit_byte(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00460510 END

// !FUNC 0x00460550 BEGIN
/* 460550-46056C 0001C */
uchar _Globals::emit_marker(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00460550 END

// !FUNC 0x00460570 BEGIN
/* 460570-460591 00021 */
uchar _Globals::emit_2bytes() { STUB_BODY(); return 0; }
// !FUNC 0x00460570 END

// !FUNC 0x004605a0 BEGIN
/* 4605A0-460699 000F9 */
char _Globals::encode_one_block(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004605a0 END

// !FUNC 0x004606a0 BEGIN
/* 4606A0-46077E 000DE */
uchar _Globals::emit_dht(int param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004606a0 END

// !FUNC 0x00460790 BEGIN
/* 460790-4607B8 00028 */
uchar _Globals::emit_dri() { STUB_BODY(); return 0; }
// !FUNC 0x00460790 END

// !FUNC 0x004607c0 BEGIN
/* 4607C0-46086A 000AA */
uchar _Globals::emit_sof(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004607c0 END

// !FUNC 0x00460870 BEGIN
/* 460870-46093B 000CB */
uchar _Globals::emit_sos() { STUB_BODY(); return 0; }
// !FUNC 0x00460870 END

// !FUNC 0x00460940 BEGIN
/* 460940-4609D2 00092 */
uchar _Globals::emit_jfif_app0() { STUB_BODY(); return 0; }
// !FUNC 0x00460940 END

// !FUNC 0x004609e0 BEGIN
/* 4609E0-460A6E 0008E */
uchar _Globals::emit_adobe_app14() { STUB_BODY(); return 0; }
// !FUNC 0x004609e0 END

// !FUNC 0x00460a70 BEGIN
/* 460A70-460AB0 00040 */
uchar _Globals::write_marker_header(int* param_1, uchar param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00460a70 END

// !FUNC 0x00460ad0 BEGIN
/* 460AD0-460B16 00046 */
uchar _Globals::write_file_header(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00460ad0 END

// !FUNC 0x00460bf0 BEGIN
/* 460BF0-460CB4 000C4 */
uchar _Globals::write_scan_header(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00460bf0 END

// !FUNC 0x00460cd0 BEGIN
/* 460CD0-460D57 00087 */
uchar _Globals::write_tables_only(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00460cd0 END

// !FUNC 0x00460e80 BEGIN
/* 460E80-460F2A 000AA */
void _Globals::create_context_buffer() { STUB_BODY(); }
// !FUNC 0x00460e80 END

// !FUNC 0x00460f30 BEGIN
/* 460F30-461072 00142 */
void _Globals::FUN_00460f30(int param_1) { STUB_BODY(); }
// !FUNC 0x00460f30 END

// !FUNC 0x00461080 BEGIN
/* 461080-461154 000D4 */
void _Globals::FUN_00461080(int param_1) { STUB_BODY(); }
// !FUNC 0x00461080 END

// !FUNC 0x00461160 BEGIN
/* 461160-4611FA 0009A */
void _Globals::FUN_00461160(int* param_1) { STUB_BODY(); }
// !FUNC 0x00461160 END

// !FUNC 0x00461460 BEGIN
/* 461460-461533 000D3 */
void _Globals::FUN_00461460(int* param_1, char param_2) { STUB_BODY(); }
// !FUNC 0x00461460 END

// !FUNC 0x00461540 BEGIN
/* 461540-461589 00049 */
void _Globals::start_iMCU_row() { STUB_BODY(); }
// !FUNC 0x00461540 END

// !FUNC 0x004615b0 BEGIN
/* 4615B0-4617EC 0023C */
uint _Globals::jpeg_decompress_data(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004615b0 END

// !FUNC 0x004619e0 BEGIN
/* 4619E0-461B6F 0018F */
int _Globals::decompress_data(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004619e0 END

// !FUNC 0x00461b70 BEGIN
/* 461B70-461CD0 00160 */
int _Globals::smoothing_ok(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00461b70 END

// !FUNC 0x00462370 BEGIN
/* 462370-4624B9 00149 */
uchar _Globals::jinit_d_coef_controller(uint* param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00462370 END

// !FUNC 0x004624c0 BEGIN
/* 4624C0-462797 002D7 */
uchar _Globals::jpeg_make_d_derived_tbl(int* param_1, char param_2, int param_3, int* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x004624c0 END

// !FUNC 0x004627b0 BEGIN
/* 4627B0-4628A8 000F8 */
uint _Globals::jpeg_fill_bit_buffer(uint* param_1, uint param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x004627b0 END

// !FUNC 0x004628b0 BEGIN
/* 4628B0-462976 000C6 */
uint _Globals::jpeg_huff_decode(uint* param_1, uint param_2, int param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x004628b0 END

// !FUNC 0x00462980 BEGIN
/* 462980-4629F5 00075 */
uint _Globals::process_restart_00462980() { STUB_BODY(); return 0; }
// !FUNC 0x00462980 END

// !FUNC 0x00462a00 BEGIN
/* 462A00-462E16 00416 */
uint _Globals::decode_mcu_00462a00(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00462a00 END

// !FUNC 0x00462f30 BEGIN
/* 462F30-462F76 00046 */
uchar _Globals::jinit_huff_decoder(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00462f30 END

// !FUNC 0x00462f80 BEGIN
/* 462F80-462FFC 0007C */
uint _Globals::process_restart() { STUB_BODY(); return 0; }
// !FUNC 0x00462f80 END

// !FUNC 0x00463010 BEGIN
/* 463010-463247 00237 */
uint _Globals::jpeg_decode_mcu_DC_first(uint param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00463010 END

// !FUNC 0x00463250 BEGIN
/* 463250-463495 00245 */
uint _Globals::jpeg_decode_mcu_AC_first(int param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00463250 END

// !FUNC 0x004634a0 BEGIN
/* 4634A0-463585 000E5 */
uint _Globals::jpeg_decode_mcu_AC_refine(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004634a0 END

// !FUNC 0x00463960 BEGIN
/* 463960-463B40 001E0 */
uchar _Globals::start_pass_huff_decoder(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00463960 END

// !FUNC 0x00463b50 BEGIN
/* 463B50-463BBE 0006E */
uchar _Globals::jinit_huff_decoder_00463b50(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00463b50 END

// !FUNC 0x00463bc0 BEGIN
/* 463BC0-463EBA 002FA */
uchar _Globals::start_pass_dcolor(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00463bc0 END

// !FUNC 0x00463ee0 BEGIN
/* 463EE0-463F51 00071 */
void _Globals::jinit_color_deconverter_00463ee0(int* param_1) { STUB_BODY(); }
// !FUNC 0x00463ee0 END

// !FUNC 0x00463fe0 BEGIN
/* 463FE0-464081 000A1 */
uchar _Globals::process_data_simple_main(int param_1, uint param_2, uint param_3, uint param_4, uint param_5, int* param_6) { STUB_BODY(); return 0; }
// !FUNC 0x00463fe0 END

// !FUNC 0x00464090 BEGIN
/* 464090-464125 00095 */
uchar _Globals::process_data_context_main(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00464090 END

// !FUNC 0x00464230 BEGIN
/* 464230-4642C3 00093 */
uchar _Globals::jinit_d_main_controller_00464230(int param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00464230 END

// !FUNC 0x004642f0 BEGIN
/* 4642F0-4643BC 000CC */
uchar _Globals::sep_upsample(int param_1, int param_2, int* param_3, uint param_4, int param_5, int* param_6, int param_7) { STUB_BODY(); return 0; }
// !FUNC 0x004642f0 END

// !FUNC 0x004643e0 BEGIN
/* 4643E0-4644BE 000DE */
uchar _Globals::int_upsample(int param_1, uint* param_2, uint* param_3, int* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x004643e0 END

// !FUNC 0x00464660 BEGIN
/* 464660-46479C 0013C */
uchar _Globals::h2v2_smooth_downsample(int param_1, int param_2, uint* param_3, int* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00464660 END

// !FUNC 0x004647a0 BEGIN
/* 4647A0-46498E 001EE */
uchar _Globals::jinit_upsampler(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004647a0 END

// !FUNC 0x00464990 BEGIN
/* 464990-464A54 000C4 */
uchar _Globals::build_ycc_rgb_table() { STUB_BODY(); return 0; }
// !FUNC 0x00464990 END

// !FUNC 0x00464a60 BEGIN
/* 464A60-464B7F 0011F */
uchar _Globals::ycc_rgb_convert(int param_1, int* param_2, int param_3, uint* param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x00464a60 END

// !FUNC 0x00464ca0 BEGIN
/* 464CA0-464DE8 00148 */
uchar _Globals::ycck_cmyk_convert(int param_1, int* param_2, int param_3, uint* param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x00464ca0 END

// !FUNC 0x00464e00 BEGIN
/* 464E00-464F6E 0016E */
uchar _Globals::jinit_color_deconverter(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00464e00 END

// !FUNC 0x00464f90 BEGIN
/* 464F90-465054 000C4 */
uchar _Globals::build_ycc_rgb_table_00464f90() { STUB_BODY(); return 0; }
// !FUNC 0x00464f90 END

// !FUNC 0x00465180 BEGIN
/* 465180-4652D0 00150 */
void _Globals::h2v1_merged_upsample(int param_1, int* param_2, int param_3, uint* param_4) { STUB_BODY(); }
// !FUNC 0x00465180 END

// !FUNC 0x004654f0 BEGIN
/* 4654F0-465570 00080 */
uchar _Globals::jinit_merged_upsampler(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004654f0 END

// !FUNC 0x004655f0 BEGIN
/* 4655F0-46561C 0002C */
int* _Globals::find_biggest_color_pop(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004655f0 END

// !FUNC 0x00465620 BEGIN
/* 465620-465646 00026 */
int* _Globals::find_biggest_volume(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00465620 END

// !FUNC 0x00465650 BEGIN
/* 465650-465A42 003F2 */
uchar _Globals::update_box(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00465650 END

// !FUNC 0x00465a60 BEGIN
/* 465A60-465BA5 00145 */
int _Globals::median_cut(void* param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00465a60 END

// !FUNC 0x00465bb0 BEGIN
/* 465BB0-465D23 00173 */
uchar _Globals::fill_inverse_cmap(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00465bb0 END

// !FUNC 0x00465d30 BEGIN
/* 465D30-465D9D 0006D */
uchar _Globals::select_colors(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00465d30 END

// !FUNC 0x00465da0 BEGIN
/* 465DA0-465F67 001C7 */
uchar _Globals::init_error_limit(int param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00465da0 END

// !FUNC 0x00465f70 BEGIN
/* 465F70-4660E2 00172 */
uchar _Globals::prescan_quantize(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, uchar* param_7) { STUB_BODY(); return 0; }
// !FUNC 0x00465f70 END

// !FUNC 0x004660f0 BEGIN
/* 4660F0-46622E 0013E */
uchar _Globals::start_pass_1_quant(int param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004660f0 END

// !FUNC 0x00466230 BEGIN
/* 466230-4662F8 000C8 */
uchar _Globals::color_quantize3(int param_1, int param_2, int* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00466230 END

// !FUNC 0x00466300 BEGIN
/* 466300-4665BB 002BB */
uchar _Globals::h2v2_merged_upsample(int param_1, int param_2, int* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00466300 END

// !FUNC 0x004665c0 BEGIN
/* 4665C0-466666 000A6 */
uchar _Globals::init_inverse_cmap() { STUB_BODY(); return 0; }
// !FUNC 0x004665c0 END

// !FUNC 0x004666a0 BEGIN
/* 4666A0-4667B8 00118 */
uchar _Globals::start_pass_2(int* param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004666a0 END

// !FUNC 0x004667d0 BEGIN
/* 4667D0-466912 00142 */
uchar _Globals::jinit_2pass_quantizer(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004667d0 END

// !FUNC 0x00466920 BEGIN
/* 466920-466A02 000E2 */
int _Globals::FUN_00466920(int* param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00466920 END

// !FUNC 0x00466a10 BEGIN
/* 466A10-466A25 00015 */
int _Globals::FUN_00466a10(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00466a10 END

// !FUNC 0x00466a30 BEGIN
/* 466A30-466A43 00013 */
int _Globals::FUN_00466a30(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00466a30 END

// !FUNC 0x00466a50 BEGIN
/* 466A50-466B64 00114 */
uchar _Globals::FUN_00466a50(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00466a50 END


// !FUNC 0x00466cc0 BEGIN
/* 466CC0-466D28 00068 */
int* _Globals::FUN_00466cc0(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00466cc0 END

// !FUNC 0x00466d40 BEGIN
/* 466D40-466D90 00050 */
uchar _Globals::FUN_00466d40() { STUB_BODY(); return 0; }
// !FUNC 0x00466d40 END

// !FUNC 0x00466e40 BEGIN
/* 466E40-466EF8 000B8 */
uchar _Globals::color_quantize(int param_1, int param_2, int* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00466e40 END

// !FUNC 0x00466f00 BEGIN
/* 466F00-467011 00111 */
void _Globals::FUN_00466f00(int* param_1, int param_2, uint* param_3, int param_4) { STUB_BODY(); }
// !FUNC 0x00466f00 END

// !FUNC 0x00467020 BEGIN
/* 467020-467144 00124 */
void _Globals::FUN_00467020(int* param_1, int param_2, int* param_3, int param_4) { STUB_BODY(); }
// !FUNC 0x00467020 END

// !FUNC 0x00467150 BEGIN
/* 467150-4672E9 00199 */
uchar _Globals::FUN_00467150(int* param_1, int param_2, uint** param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00467150 END

// !FUNC 0x00467300 BEGIN
/* 467300-467340 00040 */
void _Globals::FUN_00467300(int* param_1) { STUB_BODY(); }
// !FUNC 0x00467300 END

// !FUNC 0x00467340 BEGIN
/* 467340-467423 000E3 */
void _Globals::FUN_00467340(int* param_1) { STUB_BODY(); }
// !FUNC 0x00467340 END

// !FUNC 0x00467460 BEGIN
/* 467460-4674F6 00096 */
uchar _Globals::jinit_merged_upsampler_00467460(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00467460 END

// !FUNC 0x00467690 BEGIN
/* 467690-4676D6 00046 */
void _Globals::start_iMCU_row_00467690() { STUB_BODY(); }
// !FUNC 0x00467690 END

// !FUNC 0x004676e0 BEGIN
/* 4676E0-46791E 0023E */
uint _Globals::jpeg_compress_data(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004676e0 END

// !FUNC 0x00467930 BEGIN
/* 467930-467AE2 001B2 */
bool _Globals::compress_output(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00467930 END

// !FUNC 0x00467af0 BEGIN
/* 467AF0-467CF8 00208 */
uchar _Globals::compress_first_pass(int param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00467af0 END

// !FUNC 0x00467d10 BEGIN
/* 467D10-467DBC 000AC */
void _Globals::start_pass_coef(int* param_1, int param_2) { STUB_BODY(); }
// !FUNC 0x00467d10 END

// !FUNC 0x00467ed0 BEGIN
/* 467ED0-4680DE 0020E */
uchar _Globals::jpeg_make_c_derived_tbl(int* param_1, char param_2, int param_3, int* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00467ed0 END

// !FUNC 0x004680f0 BEGIN
/* 4680F0-468114 00024 */
uchar _Globals::emit_byte_s() { STUB_BODY(); return 0; }
// !FUNC 0x004680f0 END

// !FUNC 0x00468120 BEGIN
/* 468120-4681CC 000AC */
uint _Globals::emit_bits_s(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00468120 END

// !FUNC 0x004681d0 BEGIN
/* 4681D0-4681F1 00021 */
uint _Globals::flush_bits_s() { STUB_BODY(); return 0; }
// !FUNC 0x004681d0 END

// !FUNC 0x00468200 BEGIN
/* 468200-468396 00196 */
uint _Globals::encode_one_block_00468200(void* param_1, short* param_2, uint param_3, uint* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00468200 END

// !FUNC 0x004683a0 BEGIN
/* 4683A0-46841A 0007A */
uint _Globals::emit_restart(void* param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004683a0 END

// !FUNC 0x00468420 BEGIN
/* 468420-468582 00162 */
uint _Globals::encode_mcu(int* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00468420 END

// !FUNC 0x00468590 BEGIN
/* 468590-468642 000B2 */
void _Globals::finish_pass_huff(int* param_1) { STUB_BODY(); }
// !FUNC 0x00468590 END

// !FUNC 0x00468650 BEGIN
/* 468650-468733 000E3 */
int* _Globals::htest_one_block(int* param_1, short* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00468650 END

// !FUNC 0x00468810 BEGIN
/* 468810-468AD5 002C5 */
uchar _Globals::jpeg_gen_optimal_table(int* param_1, uint* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00468810 END

// !FUNC 0x00468af0 BEGIN
/* 468AF0-468BD2 000E2 */
uchar _Globals::finish_pass_gather(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00468af0 END

// !FUNC 0x00468be0 BEGIN
/* 468BE0-468D65 00185 */
void _Globals::start_pass_huff(void* param_1, char param_2) { STUB_BODY(); }
// !FUNC 0x00468be0 END

// !FUNC 0x00468dd0 BEGIN
/* 468DD0-468E0A 0003A */
uchar _Globals::emit_byte_e() { STUB_BODY(); return 0; }
// !FUNC 0x00468dd0 END

// !FUNC 0x00468e10 BEGIN
/* 468E10-468ECD 000BD */
uchar _Globals::emit_bits_e(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00468e10 END

// !FUNC 0x00468ed0 BEGIN
/* 468ED0-468EEA 0001A */
uchar _Globals::flush_bits_e() { STUB_BODY(); return 0; }
// !FUNC 0x00468ed0 END

// !FUNC 0x00468ef0 BEGIN
/* 468EF0-468F19 00029 */
int _Globals::emit_ac_symbol(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00468ef0 END

// !FUNC 0x00468f20 BEGIN
/* 468F20-468F4E 0002E */
uchar _Globals::emit_buffered_bits(char* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00468f20 END

// !FUNC 0x00468f50 BEGIN
/* 468F50-468FC7 00077 */
uchar _Globals::emit_eobrun() { STUB_BODY(); return 0; }
// !FUNC 0x00468f50 END

// !FUNC 0x00468fd0 BEGIN
/* 468FD0-469051 00081 */
uchar _Globals::emit_restart_00468fd0(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00468fd0 END

// !FUNC 0x00469060 BEGIN
/* 469060-46919C 0013C */
bool _Globals::encode_mcu_DC_first(int* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00469060 END

// !FUNC 0x004691b0 BEGIN
/* 4691B0-46936F 001BF */
uint _Globals::encode_one_block_004691b0(int* param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004691b0 END

// !FUNC 0x004696a0 BEGIN
/* 4696A0-469765 000C5 */
void _Globals::finish_pass_gather_004696a0(int* param_1) { STUB_BODY(); }
// !FUNC 0x004696a0 END

// !FUNC 0x00469770 BEGIN
/* 469770-4698EB 0017B */
uchar _Globals::start_pass_huff_decoder_00469770(int* param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00469770 END

// !FUNC 0x0046a840 BEGIN
/* 46A840-46A86A 0002A */
void _Globals::FUN_0046a840(size_t param_1) { STUB_BODY(); }
// !FUNC 0x0046a840 END

// !FUNC 0x0046a870 BEGIN
/* 46A870-46A9F8 00188 */
void _Globals::pre_process_data(int* param_1, int param_2, uint* param_3, uint param_4, int param_5, uint* param_6, uint param_7) { STUB_BODY(); }
// !FUNC 0x0046a870 END

// !FUNC 0x0046aa00 BEGIN
/* 46AA00-46ABC6 001C6 */
uchar _Globals::compress_output_0046aa00(int param_1, int param_2, uint* param_3, uint param_4, uint param_5, uint* param_6, uint param_7) { STUB_BODY(); return 0; }
// !FUNC 0x0046aa00 END

// !FUNC 0x0046ada0 BEGIN
/* 46ADA0-46ADE4 00044 */
uchar _Globals::expand_right_edge(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0046ada0 END

// !FUNC 0x0046adf0 BEGIN
/* 46ADF0-46AE79 00089 */
void _Globals::sep_downsample(int param_1, int* param_2, int param_3, int* param_4, int param_5) { STUB_BODY(); }
// !FUNC 0x0046adf0 END

// !FUNC 0x0046ae80 BEGIN
/* 46AE80-46AFA2 00122 */
void _Globals::int_downsample(int param_1, int param_2, int* param_3, int param_4) { STUB_BODY(); }
// !FUNC 0x0046ae80 END

// !FUNC 0x0046b000 BEGIN
/* 46B000-46B0A4 000A4 */
uchar _Globals::h2v1_downsample(void* param_1, int param_2, uint* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0046b000 END

// !FUNC 0x0046b0b0 BEGIN
/* 46B0B0-46B17D 000CD */
uchar _Globals::h2v2_downsample(void* param_1, int param_2, uint* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0046b0b0 END

// !FUNC 0x0046b180 BEGIN
/* 46B180-46B3FE 0027E */
uchar _Globals::h2v2_fancy_upsample(int param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0046b180 END

// !FUNC 0x0046b400 BEGIN
/* 46B400-46B58D 0018D */
uchar _Globals::FUN_0046b400(int param_1, int param_2, uint* param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0046b400 END

// !FUNC 0x0046b6e0 BEGIN
/* 46B6E0-46B7AB 000CB */
uchar _Globals::rgb_ycc_start(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0046b6e0 END

// !FUNC 0x0046b7c0 BEGIN
/* 46B7C0-46B8D2 00112 */
uchar _Globals::rgb_ycc_convert(int param_1, uint* param_2, int* param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x0046b7c0 END

// !FUNC 0x0046b8e0 BEGIN
/* 46B8E0-46B980 000A0 */
uchar _Globals::rgb_gray_convert(int param_1, uint* param_2, int* param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x0046b8e0 END

// !FUNC 0x0046b990 BEGIN
/* 46B990-46BAD8 00148 */
uchar _Globals::cmyk_ycck_convert(int param_1, uint* param_2, int* param_3, int param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x0046b990 END

// !FUNC 0x0046c3d0 BEGIN
/* 46C3D0-46C4B7 000E7 */
void _Globals::select_scan_parameters(int param_1) { STUB_BODY(); }
// !FUNC 0x0046c3d0 END

// !FUNC 0x0046c4c0 BEGIN
/* 46C4C0-46C687 001C7 */
uint _Globals::per_scan_setup_0046c4c0() { STUB_BODY(); return 0; }
// !FUNC 0x0046c4c0 END

// !FUNC 0x0046c690 BEGIN
/* 46C690-46C849 001B9 */
uchar _Globals::FUN_0046c690(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0046c690 END

// !FUNC 0x0046c850 BEGIN
/* 46C850-46C87C 0002C */
uchar _Globals::FUN_0046c850(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0046c850 END

// !FUNC 0x0046db50 BEGIN
/* 46DB50-46DFC0 00470 */
uchar _Globals::FUN_0046db50(int param_1, int param_2, int param_3, int* param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x0046db50 END

// !FUNC 0x0046e000 BEGIN
/* 46E000-46E312 00312 */
uchar _Globals::jpeg_idct_islow(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0046e000 END

// !FUNC 0x0046e320 BEGIN
/* 46E320-46E9E9 006C9 */
uchar _Globals::jpeg_idct_ifast(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0046e320 END




// !FUNC 0x0046f770 BEGIN
/* 46F770-46F7F0 00080 */
uchar _Globals::lm_init() { STUB_BODY(); return 0; }
// !FUNC 0x0046f770 END


// !FUNC 0x0046f970 BEGIN
/* 46F970-46FA6B 000FB */
uchar _Globals::fill_window() { STUB_BODY(); return 0; }
// !FUNC 0x0046f970 END


// !FUNC 0x0046fba0 BEGIN
/* 46FBA0-46FE7E 002DE */
uchar _Globals::deflate_fast(int* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0046fba0 END


// !FUNC 0x00470250 BEGIN
/* 470250-4702C0 00070 */
uint _Globals::deflateResetKeep(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00470250 END


// !FUNC 0x00471300 BEGIN
/* 471300-4713C6 000C6 */
uchar _Globals::pqdownheap(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00471300 END

// !FUNC 0x004713d0 BEGIN
/* 4713D0-4715E1 00211 */
uchar _Globals::FUN_004713d0(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004713d0 END

// !FUNC 0x004715f0 BEGIN
/* 4715F0-4716DD 000ED */
uchar _Globals::zlib_scan_tree(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004715f0 END

// !FUNC 0x004716e0 BEGIN
/* 4716E0-471BF1 00511 */
uchar _Globals::send_tree(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004716e0 END



// !FUNC 0x00472270 BEGIN
/* 472270-4722F4 00084 */
uchar _Globals::detect_data_type(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00472270 END

// !FUNC 0x00472300 BEGIN
/* 472300-472319 00019 */
uint _Globals::bi_reverse(uint param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00472300 END

// !FUNC 0x00472320 BEGIN
/* 472320-472395 00075 */
uchar _Globals::bi_flush() { STUB_BODY(); return 0; }
// !FUNC 0x00472320 END

// !FUNC 0x004723a0 BEGIN
/* 4723A0-472413 00073 */
uchar _Globals::bi_windup() { STUB_BODY(); return 0; }
// !FUNC 0x004723a0 END

// !FUNC 0x00472420 BEGIN
/* 472420-4724A2 00082 */
void _Globals::FUN_00472420(uint param_1, uchar* param_2, int param_3) { STUB_BODY(); }
// !FUNC 0x00472420 END

// !FUNC 0x004724b0 BEGIN
/* 4724B0-472514 00064 */
uchar _Globals::zlib_tr_init(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004724b0 END

// !FUNC 0x00472520 BEGIN
/* 472520-47259D 0007D */
uchar _Globals::gen_codes(uint param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00472520 END

// !FUNC 0x004725a0 BEGIN
/* 4725A0-472791 001F1 */
uchar _Globals::build_tree(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004725a0 END

// !FUNC 0x004727a0 BEGIN
/* 4727A0-472866 000C6 */
uchar _Globals::build_bl_tree() { STUB_BODY(); return 0; }
// !FUNC 0x004727a0 END

// !FUNC 0x00472870 BEGIN
/* 472870-47290F 0009F */
uchar _Globals::send_bits(int param_1, uchar* param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00472870 END

// !FUNC 0x00472910 BEGIN
/* 472910-472AFD 001ED */
uchar _Globals::_tr_align(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00472910 END

// !FUNC 0x00472b00 BEGIN
/* 472B00-472CEE 001EE */
uchar _Globals::_tr_flush_block(int param_1, uchar* param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00472b00 END

// !FUNC 0x00472cf0 BEGIN
/* 472CF0-472D2B 0003B */
uchar _Globals::inflate_codes_new(uchar param_1, uchar param_2, uint param_3, uint param_4, int param_5) { STUB_BODY(); return 0; }
// !FUNC 0x00472cf0 END

// !FUNC 0x00472d30 BEGIN
/* 472D30-4733A2 00672 */
uchar _Globals::inflate_codes(uint param_1, int* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00472d30 END




// !FUNC 0x00473ac0 BEGIN
/* 473AC0-473AEE 0002E */
uint _Globals::zlib_inflate_fixed(uint* param_1, uint* param_2, uint* param_3, uint* param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00473ac0 END

// !FUNC 0x00473af0 BEGIN
/* 473AF0-473BDC 000EC */
int _Globals::zlib_inflate_flush(int param_1, void* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00473af0 END


// !FUNC 0x00473f40 BEGIN
/* 473F40-473F59 00019 */
uchar _Globals::Unwind_00473f40() { STUB_BODY(); return 0; }
// !FUNC 0x00473f40 END

// !FUNC 0x00473f80 BEGIN
/* 473F80-473F88 00008 */
uchar _Globals::Unwind_00473f80() { STUB_BODY(); return 0; }
// !FUNC 0x00473f80 END

// !FUNC 0x00473f88 BEGIN
/* 473F88-473F90 00008 */
uchar _Globals::Unwind_00473f88() { STUB_BODY(); return 0; }
// !FUNC 0x00473f88 END

// !FUNC 0x00473fb0 BEGIN
/* 473FB0-473FB8 00008 */
uchar _Globals::Unwind_00473fb0() { STUB_BODY(); return 0; }
// !FUNC 0x00473fb0 END

// !FUNC 0x00473fe0 BEGIN
/* 473FE0-473FF9 00019 */
uchar _Globals::Unwind_00473fe0() { STUB_BODY(); return 0; }
// !FUNC 0x00473fe0 END

// !FUNC 0x00474020 BEGIN
/* 474020-474028 00008 */
uchar _Globals::Unwind_00474020() { STUB_BODY(); return 0; }
// !FUNC 0x00474020 END

// !FUNC 0x00474028 BEGIN
/* 474028-474030 00008 */
uchar _Globals::Unwind_00474028() { STUB_BODY(); return 0; }
// !FUNC 0x00474028 END

// !FUNC 0x00474030 BEGIN
/* 474030-47403B 0000B */
uchar _Globals::Unwind_00474030() { STUB_BODY(); return 0; }
// !FUNC 0x00474030 END

// !FUNC 0x00474060 BEGIN
/* 474060-474068 00008 */
uchar _Globals::Unwind_00474060() { STUB_BODY(); return 0; }
// !FUNC 0x00474060 END

// !FUNC 0x00474068 BEGIN
/* 474068-474073 0000B */
uchar _Globals::Unwind_00474068() { STUB_BODY(); return 0; }
// !FUNC 0x00474068 END

// !FUNC 0x00474090 BEGIN
/* 474090-474098 00008 */
uchar _Globals::Unwind_00474090() { STUB_BODY(); return 0; }
// !FUNC 0x00474090 END

// !FUNC 0x00474098 BEGIN
/* 474098-4740A3 0000B */
uchar _Globals::Unwind_00474098() { STUB_BODY(); return 0; }
// !FUNC 0x00474098 END

// !FUNC 0x004740a3 BEGIN
/* 4740A3-4740AE 0000B */
uchar _Globals::Unwind_004740a3() { STUB_BODY(); return 0; }
// !FUNC 0x004740a3 END

// !FUNC 0x004740ae BEGIN
/* 4740AE-4740B9 0000B */
uchar _Globals::Unwind_004740ae() { STUB_BODY(); return 0; }
// !FUNC 0x004740ae END

// !FUNC 0x004740b9 BEGIN
/* 4740B9-4740C4 0000B */
uchar _Globals::Unwind_004740b9() { STUB_BODY(); return 0; }
// !FUNC 0x004740b9 END

// !FUNC 0x004740e0 BEGIN
/* 4740E0-4740E8 00008 */
uchar _Globals::Unwind_004740e0() { STUB_BODY(); return 0; }
// !FUNC 0x004740e0 END

// !FUNC 0x00474110 BEGIN
/* 474110-474118 00008 */
uchar _Globals::Unwind_00474110() { STUB_BODY(); return 0; }
// !FUNC 0x00474110 END

// !FUNC 0x00474140 BEGIN
/* 474140-474148 00008 */
uchar _Globals::Unwind_00474140() { STUB_BODY(); return 0; }
// !FUNC 0x00474140 END

// !FUNC 0x00474190 BEGIN
/* 474190-47419B 0000B */
uchar _Globals::Unwind_00474190() { STUB_BODY(); return 0; }
// !FUNC 0x00474190 END

// !FUNC 0x004741c0 BEGIN
/* 4741C0-4741C8 00008 */
uchar _Globals::Unwind_004741c0() { STUB_BODY(); return 0; }
// !FUNC 0x004741c0 END

// !FUNC 0x004741c8 BEGIN
/* 4741C8-4741D0 00008 */
uchar _Globals::Unwind_004741c8() { STUB_BODY(); return 0; }
// !FUNC 0x004741c8 END

// !FUNC 0x004741d0 BEGIN
/* 4741D0-4741D8 00008 */
uchar _Globals::Unwind_004741d0() { STUB_BODY(); return 0; }
// !FUNC 0x004741d0 END

// !FUNC 0x004741d8 BEGIN
/* 4741D8-4741E3 0000B */
uchar _Globals::Unwind_004741d8() { STUB_BODY(); return 0; }
// !FUNC 0x004741d8 END

// !FUNC 0x004741e3 BEGIN
/* 4741E3-4741EB 00008 */
uchar _Globals::Unwind_004741e3() { STUB_BODY(); return 0; }
// !FUNC 0x004741e3 END

// !FUNC 0x004741eb BEGIN
/* 4741EB-4741F3 00008 */
uchar _Globals::Unwind_004741eb() { STUB_BODY(); return 0; }
// !FUNC 0x004741eb END

// !FUNC 0x004741f3 BEGIN
/* 4741F3-4741FB 00008 */
uchar _Globals::Unwind_004741f3() { STUB_BODY(); return 0; }
// !FUNC 0x004741f3 END

// !FUNC 0x004741fb BEGIN
/* 4741FB-474203 00008 */
uchar _Globals::Unwind_004741fb() { STUB_BODY(); return 0; }
// !FUNC 0x004741fb END

// !FUNC 0x00474203 BEGIN
/* 474203-47420E 0000B */
uchar _Globals::Unwind_00474203() { STUB_BODY(); return 0; }
// !FUNC 0x00474203 END

// !FUNC 0x0047420e BEGIN
/* 47420E-474219 0000B */
uchar _Globals::Unwind_0047420e() { STUB_BODY(); return 0; }
// !FUNC 0x0047420e END

// !FUNC 0x00474240 BEGIN
/* 474240-47424E 0000E */
uchar _Globals::Unwind_00474240() { STUB_BODY(); return 0; }
// !FUNC 0x00474240 END

// !FUNC 0x00474280 BEGIN
/* 474280-47428B 0000B */
uchar _Globals::Unwind_00474280() { STUB_BODY(); return 0; }
// !FUNC 0x00474280 END

// !FUNC 0x004742b0 BEGIN
/* 4742B0-4742BE 0000E */
uchar _Globals::Unwind_004742b0() { STUB_BODY(); return 0; }
// !FUNC 0x004742b0 END

// !FUNC 0x004742be BEGIN
/* 4742BE-4742C9 0000B */
uchar _Globals::Unwind_004742be() { STUB_BODY(); return 0; }
// !FUNC 0x004742be END

// !FUNC 0x004742f0 BEGIN
/* 4742F0-4742F8 00008 */
uchar _Globals::Unwind_004742f0() { STUB_BODY(); return 0; }
// !FUNC 0x004742f0 END

// !FUNC 0x004742f8 BEGIN
/* 4742F8-474306 0000E */
uchar _Globals::Unwind_004742f8() { STUB_BODY(); return 0; }
// !FUNC 0x004742f8 END

// !FUNC 0x00474306 BEGIN
/* 474306-474314 0000E */
uchar _Globals::Unwind_00474306() { STUB_BODY(); return 0; }
// !FUNC 0x00474306 END

// !FUNC 0x00474314 BEGIN
/* 474314-474322 0000E */
uchar _Globals::Unwind_00474314() { STUB_BODY(); return 0; }
// !FUNC 0x00474314 END

// !FUNC 0x00474322 BEGIN
/* 474322-474330 0000E */
uchar _Globals::Unwind_00474322() { STUB_BODY(); return 0; }
// !FUNC 0x00474322 END

// !FUNC 0x00474350 BEGIN
/* 474350-474358 00008 */
uchar _Globals::Unwind_00474350() { STUB_BODY(); return 0; }
// !FUNC 0x00474350 END

// !FUNC 0x00474358 BEGIN
/* 474358-474366 0000E */
uchar _Globals::Unwind_00474358() { STUB_BODY(); return 0; }
// !FUNC 0x00474358 END

// !FUNC 0x00474366 BEGIN
/* 474366-474374 0000E */
uchar _Globals::Unwind_00474366() { STUB_BODY(); return 0; }
// !FUNC 0x00474366 END

// !FUNC 0x00474374 BEGIN
/* 474374-474382 0000E */
uchar _Globals::Unwind_00474374() { STUB_BODY(); return 0; }
// !FUNC 0x00474374 END

// !FUNC 0x004743a0 BEGIN
/* 4743A0-4743AB 0000B */
uchar _Globals::Unwind_004743a0() { STUB_BODY(); return 0; }
// !FUNC 0x004743a0 END

// !FUNC 0x004743d0 BEGIN
/* 4743D0-4743DB 0000B */
uchar _Globals::Unwind_004743d0() { STUB_BODY(); return 0; }
// !FUNC 0x004743d0 END

// !FUNC 0x00474400 BEGIN
/* 474400-47440B 0000B */
uchar _Globals::Unwind_00474400() { STUB_BODY(); return 0; }
// !FUNC 0x00474400 END

// !FUNC 0x0047440b BEGIN
/* 47440B-474416 0000B */
uchar _Globals::Unwind_0047440b() { STUB_BODY(); return 0; }
// !FUNC 0x0047440b END

// !FUNC 0x00474416 BEGIN
/* 474416-474421 0000B */
uchar _Globals::Unwind_00474416() { STUB_BODY(); return 0; }
// !FUNC 0x00474416 END

// !FUNC 0x00474450 BEGIN
/* 474450-474458 00008 */
uchar _Globals::Unwind_00474450() { STUB_BODY(); return 0; }
// !FUNC 0x00474450 END

// !FUNC 0x00474480 BEGIN
/* 474480-474488 00008 */
uchar _Globals::Unwind_00474480() { STUB_BODY(); return 0; }
// !FUNC 0x00474480 END

// !FUNC 0x00474488 BEGIN
/* 474488-474493 0000B */
uchar _Globals::Unwind_00474488() { STUB_BODY(); return 0; }
// !FUNC 0x00474488 END

// !FUNC 0x00474493 BEGIN
/* 474493-47449E 0000B */
uchar _Globals::Unwind_00474493() { STUB_BODY(); return 0; }
// !FUNC 0x00474493 END

// !FUNC 0x004744c0 BEGIN
/* 4744C0-4744C8 00008 */
uchar _Globals::Unwind_004744c0() { STUB_BODY(); return 0; }
// !FUNC 0x004744c0 END

// !FUNC 0x004744f0 BEGIN
/* 4744F0-4744F8 00008 */
uchar _Globals::Unwind_004744f0() { STUB_BODY(); return 0; }
// !FUNC 0x004744f0 END

// !FUNC 0x004744f8 BEGIN
/* 4744F8-474503 0000B */
uchar _Globals::Unwind_004744f8() { STUB_BODY(); return 0; }
// !FUNC 0x004744f8 END

// !FUNC 0x00474520 BEGIN
/* 474520-474528 00008 */
uchar _Globals::Unwind_00474520() { STUB_BODY(); return 0; }
// !FUNC 0x00474520 END

// !FUNC 0x00474528 BEGIN
/* 474528-47454F 00027 */
uchar _Globals::Unwind_00474528() { STUB_BODY(); return 0; }
// !FUNC 0x00474528 END

// !FUNC 0x00474570 BEGIN
/* 474570-474578 00008 */
uchar _Globals::Unwind_00474570() { STUB_BODY(); return 0; }
// !FUNC 0x00474570 END

// !FUNC 0x004745a0 BEGIN
/* 4745A0-4745A8 00008 */
uchar _Globals::Unwind_004745a0() { STUB_BODY(); return 0; }
// !FUNC 0x004745a0 END

// !FUNC 0x004745d0 BEGIN
/* 4745D0-4745D8 00008 */
uchar _Globals::Unwind_004745d0() { STUB_BODY(); return 0; }
// !FUNC 0x004745d0 END

// !FUNC 0x00474600 BEGIN
/* 474600-47460B 0000B */
uchar _Globals::Unwind_00474600() { STUB_BODY(); return 0; }
// !FUNC 0x00474600 END

// !FUNC 0x00474630 BEGIN
/* 474630-474638 00008 */
uchar _Globals::Unwind_00474630() { STUB_BODY(); return 0; }
// !FUNC 0x00474630 END

// !FUNC 0x00474660 BEGIN
/* 474660-474668 00008 */
uchar _Globals::Unwind_00474660() { STUB_BODY(); return 0; }
// !FUNC 0x00474660 END

// !FUNC 0x00474690 BEGIN
/* 474690-47469B 0000B */
uchar _Globals::Unwind_00474690() { STUB_BODY(); return 0; }
// !FUNC 0x00474690 END

// !FUNC 0x004746c0 BEGIN
/* 4746C0-4746C8 00008 */
uchar _Globals::Unwind_004746c0() { STUB_BODY(); return 0; }
// !FUNC 0x004746c0 END

// !FUNC 0x004746c8 BEGIN
/* 4746C8-4746D3 0000B */
uchar _Globals::Unwind_004746c8() { STUB_BODY(); return 0; }
// !FUNC 0x004746c8 END

// !FUNC 0x004746f0 BEGIN
/* 4746F0-4746F8 00008 */
uchar _Globals::Unwind_004746f0() { STUB_BODY(); return 0; }
// !FUNC 0x004746f0 END

// !FUNC 0x00474720 BEGIN
/* 474720-474728 00008 */
uchar _Globals::Unwind_00474720() { STUB_BODY(); return 0; }
// !FUNC 0x00474720 END

// !FUNC 0x00474728 BEGIN
/* 474728-47473E 00016 */
uchar _Globals::Unwind_00474728() { STUB_BODY(); return 0; }
// !FUNC 0x00474728 END

// !FUNC 0x0047473e BEGIN
/* 47473E-474749 0000B */
uchar _Globals::Unwind_0047473e() { STUB_BODY(); return 0; }
// !FUNC 0x0047473e END

// !FUNC 0x00474770 BEGIN
/* 474770-474778 00008 */
uchar _Globals::Unwind_00474770() { STUB_BODY(); return 0; }
// !FUNC 0x00474770 END

// !FUNC 0x004747a0 BEGIN
/* 4747A0-4747A8 00008 */
uchar _Globals::Unwind_004747a0() { STUB_BODY(); return 0; }
// !FUNC 0x004747a0 END

// !FUNC 0x004747a8 BEGIN
/* 4747A8-4747CF 00027 */
uchar _Globals::Unwind_004747a8() { STUB_BODY(); return 0; }
// !FUNC 0x004747a8 END

// !FUNC 0x004747cf BEGIN
/* 4747CF-4747DD 0000E */
uchar _Globals::Unwind_004747cf() { STUB_BODY(); return 0; }
// !FUNC 0x004747cf END

// !FUNC 0x00474800 BEGIN
/* 474800-474808 00008 */
uchar _Globals::Unwind_00474800() { STUB_BODY(); return 0; }
// !FUNC 0x00474800 END

// !FUNC 0x00474830 BEGIN
/* 474830-47483B 0000B */
uchar _Globals::Unwind_00474830() { STUB_BODY(); return 0; }
// !FUNC 0x00474830 END

// !FUNC 0x00474860 BEGIN
/* 474860-47486B 0000B */
uchar _Globals::Unwind_00474860() { STUB_BODY(); return 0; }
// !FUNC 0x00474860 END

// !FUNC 0x00474890 BEGIN
/* 474890-47489B 0000B */
uchar _Globals::Unwind_00474890() { STUB_BODY(); return 0; }
// !FUNC 0x00474890 END

// !FUNC 0x004748c0 BEGIN
/* 4748C0-4748CB 0000B */
uchar _Globals::Unwind_004748c0() { STUB_BODY(); return 0; }
// !FUNC 0x004748c0 END

// !FUNC 0x004748f0 BEGIN
/* 4748F0-4748FB 0000B */
uchar _Globals::Unwind_004748f0() { STUB_BODY(); return 0; }
// !FUNC 0x004748f0 END

// !FUNC 0x00474920 BEGIN
/* 474920-47492B 0000B */
uchar _Globals::Unwind_00474920() { STUB_BODY(); return 0; }
// !FUNC 0x00474920 END

// !FUNC 0x00474950 BEGIN
/* 474950-474958 00008 */
uchar _Globals::Unwind_00474950() { STUB_BODY(); return 0; }
// !FUNC 0x00474950 END

// !FUNC 0x00474958 BEGIN
/* 474958-474960 00008 */
uchar _Globals::Unwind_00474958() { STUB_BODY(); return 0; }
// !FUNC 0x00474958 END

// !FUNC 0x00474960 BEGIN
/* 474960-47496B 0000B */
uchar _Globals::Unwind_00474960() { STUB_BODY(); return 0; }
// !FUNC 0x00474960 END

// !FUNC 0x0047496b BEGIN
/* 47496B-474979 0000E */
uchar _Globals::Unwind_0047496b() { STUB_BODY(); return 0; }
// !FUNC 0x0047496b END

// !FUNC 0x004749a0 BEGIN
/* 4749A0-4749A8 00008 */
uchar _Globals::Unwind_004749a0() { STUB_BODY(); return 0; }
// !FUNC 0x004749a0 END

// !FUNC 0x004749a8 BEGIN
/* 4749A8-4749B0 00008 */
uchar _Globals::Unwind_004749a8() { STUB_BODY(); return 0; }
// !FUNC 0x004749a8 END

// !FUNC 0x004749b0 BEGIN
/* 4749B0-4749BB 0000B */
uchar _Globals::Unwind_004749b0() { STUB_BODY(); return 0; }
// !FUNC 0x004749b0 END

// !FUNC 0x004749bb BEGIN
/* 4749BB-4749C9 0000E */
uchar _Globals::Unwind_004749bb() { STUB_BODY(); return 0; }
// !FUNC 0x004749bb END

// !FUNC 0x004749f0 BEGIN
/* 4749F0-4749FB 0000B */
uchar _Globals::Unwind_004749f0() { STUB_BODY(); return 0; }
// !FUNC 0x004749f0 END

// !FUNC 0x00474a30 BEGIN
/* 474A30-474A38 00008 */
uchar _Globals::Unwind_00474a30() { STUB_BODY(); return 0; }
// !FUNC 0x00474a30 END

// !FUNC 0x00474a60 BEGIN
/* 474A60-474A68 00008 */
uchar _Globals::Unwind_00474a60() { STUB_BODY(); return 0; }
// !FUNC 0x00474a60 END

// !FUNC 0x00474a90 BEGIN
/* 474A90-474A98 00008 */
uchar _Globals::Unwind_00474a90() { STUB_BODY(); return 0; }
// !FUNC 0x00474a90 END

// !FUNC 0x00474a98 BEGIN
/* 474A98-474AA3 0000B */
uchar _Globals::Unwind_00474a98() { STUB_BODY(); return 0; }
// !FUNC 0x00474a98 END

// !FUNC 0x00474aa3 BEGIN
/* 474AA3-474AAE 0000B */
uchar _Globals::Unwind_00474aa3() { STUB_BODY(); return 0; }
// !FUNC 0x00474aa3 END

// !FUNC 0x00474ad0 BEGIN
/* 474AD0-474ADB 0000B */
uchar _Globals::Unwind_00474ad0() { STUB_BODY(); return 0; }
// !FUNC 0x00474ad0 END

// !FUNC 0x00474b00 BEGIN
/* 474B00-474B08 00008 */
uchar _Globals::Unwind_00474b00() { STUB_BODY(); return 0; }
// !FUNC 0x00474b00 END

// !FUNC 0x00474b08 BEGIN
/* 474B08-474B13 0000B */
uchar _Globals::Unwind_00474b08() { STUB_BODY(); return 0; }
// !FUNC 0x00474b08 END

// !FUNC 0x00474b13 BEGIN
/* 474B13-474B1E 0000B */
uchar _Globals::Unwind_00474b13() { STUB_BODY(); return 0; }
// !FUNC 0x00474b13 END

// !FUNC 0x00474b1e BEGIN
/* 474B1E-474B36 00018 */
uchar _Globals::Unwind_00474b1e() { STUB_BODY(); return 0; }
// !FUNC 0x00474b1e END

// !FUNC 0x00474b60 BEGIN
/* 474B60-474B68 00008 */
uchar _Globals::Unwind_00474b60() { STUB_BODY(); return 0; }
// !FUNC 0x00474b60 END

// !FUNC 0x00474b90 BEGIN
/* 474B90-474B98 00008 */
uchar _Globals::Unwind_00474b90() { STUB_BODY(); return 0; }
// !FUNC 0x00474b90 END

// !FUNC 0x00474b98 BEGIN
/* 474B98-474BA3 0000B */
uchar _Globals::Unwind_00474b98() { STUB_BODY(); return 0; }
// !FUNC 0x00474b98 END

// !FUNC 0x00474ba3 BEGIN
/* 474BA3-474BB1 0000E */
uchar _Globals::Unwind_00474ba3() { STUB_BODY(); return 0; }
// !FUNC 0x00474ba3 END

// !FUNC 0x00474bb1 BEGIN
/* 474BB1-474BBF 0000E */
uchar _Globals::Unwind_00474bb1() { STUB_BODY(); return 0; }
// !FUNC 0x00474bb1 END

// !FUNC 0x00474be0 BEGIN
/* 474BE0-474BE8 00008 */
uchar _Globals::Unwind_00474be0() { STUB_BODY(); return 0; }
// !FUNC 0x00474be0 END

// !FUNC 0x00474be8 BEGIN
/* 474BE8-474BF0 00008 */
uchar _Globals::Unwind_00474be8() { STUB_BODY(); return 0; }
// !FUNC 0x00474be8 END

// !FUNC 0x00474c10 BEGIN
/* 474C10-474C18 00008 */
uchar _Globals::Unwind_00474c10() { STUB_BODY(); return 0; }
// !FUNC 0x00474c10 END

// !FUNC 0x00474c18 BEGIN
/* 474C18-474C20 00008 */
uchar _Globals::Unwind_00474c18() { STUB_BODY(); return 0; }
// !FUNC 0x00474c18 END

// !FUNC 0x00474c40 BEGIN
/* 474C40-474C4B 0000B */
uchar _Globals::Unwind_00474c40() { STUB_BODY(); return 0; }
// !FUNC 0x00474c40 END

// !FUNC 0x00474c70 BEGIN
/* 474C70-474C78 00008 */
uchar _Globals::Unwind_00474c70() { STUB_BODY(); return 0; }
// !FUNC 0x00474c70 END

// !FUNC 0x00474c78 BEGIN
/* 474C78-474C86 0000E */
uchar _Globals::Unwind_00474c78() { STUB_BODY(); return 0; }
// !FUNC 0x00474c78 END

// !FUNC 0x00474c86 BEGIN
/* 474C86-474C94 0000E */
uchar _Globals::Unwind_00474c86() { STUB_BODY(); return 0; }
// !FUNC 0x00474c86 END

// !FUNC 0x00474cb0 BEGIN
/* 474CB0-474CB8 00008 */
uchar _Globals::Unwind_00474cb0() { STUB_BODY(); return 0; }
// !FUNC 0x00474cb0 END

// !FUNC 0x00474ce0 BEGIN
/* 474CE0-474CEB 0000B */
uchar _Globals::Unwind_00474ce0() { STUB_BODY(); return 0; }
// !FUNC 0x00474ce0 END

// !FUNC 0x00474d10 BEGIN
/* 474D10-474D18 00008 */
uchar _Globals::Unwind_00474d10() { STUB_BODY(); return 0; }
// !FUNC 0x00474d10 END

// !FUNC 0x00474d18 BEGIN
/* 474D18-474D23 0000B */
uchar _Globals::Unwind_00474d18() { STUB_BODY(); return 0; }
// !FUNC 0x00474d18 END

// !FUNC 0x00474d23 BEGIN
/* 474D23-474D3B 00018 */
uchar _Globals::Unwind_00474d23() { STUB_BODY(); return 0; }
// !FUNC 0x00474d23 END

// !FUNC 0x00474d60 BEGIN
/* 474D60-474D68 00008 */
uchar _Globals::Unwind_00474d60() { STUB_BODY(); return 0; }
// !FUNC 0x00474d60 END

// !FUNC 0x00474d68 BEGIN
/* 474D68-474D73 0000B */
uchar _Globals::Unwind_00474d68() { STUB_BODY(); return 0; }
// !FUNC 0x00474d68 END

// !FUNC 0x00474d73 BEGIN
/* 474D73-474D7E 0000B */
uchar _Globals::Unwind_00474d73() { STUB_BODY(); return 0; }
// !FUNC 0x00474d73 END

// !FUNC 0x00474d7e BEGIN
/* 474D7E-474D96 00018 */
uchar _Globals::Unwind_00474d7e() { STUB_BODY(); return 0; }
// !FUNC 0x00474d7e END

// !FUNC 0x00474dc0 BEGIN
/* 474DC0-474DC8 00008 */
uchar _Globals::Unwind_00474dc0() { STUB_BODY(); return 0; }
// !FUNC 0x00474dc0 END

// !FUNC 0x00474dc8 BEGIN
/* 474DC8-474DDE 00016 */
uchar _Globals::Unwind_00474dc8() { STUB_BODY(); return 0; }
// !FUNC 0x00474dc8 END

// !FUNC 0x00474e00 BEGIN
/* 474E00-474E08 00008 */
uchar _Globals::Unwind_00474e00() { STUB_BODY(); return 0; }
// !FUNC 0x00474e00 END

// !FUNC 0x00474e08 BEGIN
/* 474E08-474E10 00008 */
uchar _Globals::Unwind_00474e08() { STUB_BODY(); return 0; }
// !FUNC 0x00474e08 END

// !FUNC 0x00474e10 BEGIN
/* 474E10-474E26 00016 */
uchar _Globals::Unwind_00474e10() { STUB_BODY(); return 0; }
// !FUNC 0x00474e10 END

// !FUNC 0x00474e26 BEGIN
/* 474E26-474E31 0000B */
uchar _Globals::Unwind_00474e26() { STUB_BODY(); return 0; }
// !FUNC 0x00474e26 END

// !FUNC 0x00474e31 BEGIN
/* 474E31-474E3F 0000E */
uchar _Globals::Unwind_00474e31() { STUB_BODY(); return 0; }
// !FUNC 0x00474e31 END

// !FUNC 0x00474e60 BEGIN
/* 474E60-474E68 00008 */
uchar _Globals::Unwind_00474e60() { STUB_BODY(); return 0; }
// !FUNC 0x00474e60 END

// !FUNC 0x00474e68 BEGIN
/* 474E68-474E73 0000B */
uchar _Globals::Unwind_00474e68() { STUB_BODY(); return 0; }
// !FUNC 0x00474e68 END

// !FUNC 0x00474e73 BEGIN
/* 474E73-474E7E 0000B */
uchar _Globals::Unwind_00474e73() { STUB_BODY(); return 0; }
// !FUNC 0x00474e73 END

// !FUNC 0x00474e7e BEGIN
/* 474E7E-474E89 0000B */
uchar _Globals::Unwind_00474e7e() { STUB_BODY(); return 0; }
// !FUNC 0x00474e7e END

// !FUNC 0x00474eb0 BEGIN
/* 474EB0-474EB8 00008 */
uchar _Globals::Unwind_00474eb0() { STUB_BODY(); return 0; }
// !FUNC 0x00474eb0 END

// !FUNC 0x00474eb8 BEGIN
/* 474EB8-474EC6 0000E */
uchar _Globals::Unwind_00474eb8() { STUB_BODY(); return 0; }
// !FUNC 0x00474eb8 END

// !FUNC 0x00474ef0 BEGIN
/* 474EF0-474EFB 0000B */
uchar _Globals::Unwind_00474ef0() { STUB_BODY(); return 0; }
// !FUNC 0x00474ef0 END

// !FUNC 0x00474f20 BEGIN
/* 474F20-474F2B 0000B */
uchar _Globals::Unwind_00474f20() { STUB_BODY(); return 0; }
// !FUNC 0x00474f20 END

// !FUNC 0x00474f50 BEGIN
/* 474F50-474F58 00008 */
uchar _Globals::Unwind_00474f50() { STUB_BODY(); return 0; }
// !FUNC 0x00474f50 END

// !FUNC 0x00474f80 BEGIN
/* 474F80-474F88 00008 */
uchar _Globals::Unwind_00474f80() { STUB_BODY(); return 0; }
// !FUNC 0x00474f80 END

// !FUNC 0x00474f88 BEGIN
/* 474F88-474F93 0000B */
uchar _Globals::Unwind_00474f88() { STUB_BODY(); return 0; }
// !FUNC 0x00474f88 END

// !FUNC 0x00474fb0 BEGIN
/* 474FB0-474FB8 00008 */
uchar _Globals::Unwind_00474fb0() { STUB_BODY(); return 0; }
// !FUNC 0x00474fb0 END

// !FUNC 0x00474fb8 BEGIN
/* 474FB8-474FC3 0000B */
uchar _Globals::Unwind_00474fb8() { STUB_BODY(); return 0; }
// !FUNC 0x00474fb8 END

// !FUNC 0x00474fe0 BEGIN
/* 474FE0-474FE8 00008 */
uchar _Globals::Unwind_00474fe0() { STUB_BODY(); return 0; }
// !FUNC 0x00474fe0 END

// !FUNC 0x00474fe8 BEGIN
/* 474FE8-474FF3 0000B */
uchar _Globals::Unwind_00474fe8() { STUB_BODY(); return 0; }
// !FUNC 0x00474fe8 END

// !FUNC 0x00475010 BEGIN
/* 475010-475018 00008 */
uchar _Globals::Unwind_00475010() { STUB_BODY(); return 0; }
// !FUNC 0x00475010 END

// !FUNC 0x00475040 BEGIN
/* 475040-475048 00008 */
uchar _Globals::Unwind_00475040() { STUB_BODY(); return 0; }
// !FUNC 0x00475040 END

// !FUNC 0x00475048 BEGIN
/* 475048-47506F 00027 */
uchar _Globals::Unwind_00475048() { STUB_BODY(); return 0; }
// !FUNC 0x00475048 END

// !FUNC 0x00475090 BEGIN
/* 475090-475098 00008 */
uchar _Globals::Unwind_00475090() { STUB_BODY(); return 0; }
// !FUNC 0x00475090 END

// !FUNC 0x00475098 BEGIN
/* 475098-4750A3 0000B */
uchar _Globals::Unwind_00475098() { STUB_BODY(); return 0; }
// !FUNC 0x00475098 END

// !FUNC 0x004750c0 BEGIN
/* 4750C0-4750CB 0000B */
uchar _Globals::Unwind_004750c0() { STUB_BODY(); return 0; }
// !FUNC 0x004750c0 END

// !FUNC 0x004750f0 BEGIN
/* 4750F0-4750FB 0000B */
uchar _Globals::Unwind_004750f0() { STUB_BODY(); return 0; }
// !FUNC 0x004750f0 END

// !FUNC 0x00475120 BEGIN
/* 475120-47512B 0000B */
uchar _Globals::Unwind_00475120() { STUB_BODY(); return 0; }
// !FUNC 0x00475120 END

// !FUNC 0x0047512b BEGIN
/* 47512B-475136 0000B */
uchar _Globals::Unwind_0047512b() { STUB_BODY(); return 0; }
// !FUNC 0x0047512b END

// !FUNC 0x00475160 BEGIN
/* 475160-475168 00008 */
uchar _Globals::Unwind_00475160() { STUB_BODY(); return 0; }
// !FUNC 0x00475160 END

// !FUNC 0x00475168 BEGIN
/* 475168-475173 0000B */
uchar _Globals::Unwind_00475168() { STUB_BODY(); return 0; }
// !FUNC 0x00475168 END

// !FUNC 0x00475173 BEGIN
/* 475173-47517E 0000B */
uchar _Globals::Unwind_00475173() { STUB_BODY(); return 0; }
// !FUNC 0x00475173 END

// !FUNC 0x0047517e BEGIN
/* 47517E-475189 0000B */
uchar _Globals::Unwind_0047517e() { STUB_BODY(); return 0; }
// !FUNC 0x0047517e END

// !FUNC 0x00475189 BEGIN
/* 475189-475194 0000B */
uchar _Globals::Unwind_00475189() { STUB_BODY(); return 0; }
// !FUNC 0x00475189 END

// !FUNC 0x004751b0 BEGIN
/* 4751B0-4751B8 00008 */
uchar _Globals::Unwind_004751b0() { STUB_BODY(); return 0; }
// !FUNC 0x004751b0 END

// !FUNC 0x004751e0 BEGIN
/* 4751E0-4751E8 00008 */
uchar _Globals::Unwind_004751e0() { STUB_BODY(); return 0; }
// !FUNC 0x004751e0 END

// !FUNC 0x004751e8 BEGIN
/* 4751E8-4751F0 00008 */
uchar _Globals::Unwind_004751e8() { STUB_BODY(); return 0; }
// !FUNC 0x004751e8 END

// !FUNC 0x004751f0 BEGIN
/* 4751F0-4751F8 00008 */
uchar _Globals::Unwind_004751f0() { STUB_BODY(); return 0; }
// !FUNC 0x004751f0 END

// !FUNC 0x004751f8 BEGIN
/* 4751F8-475200 00008 */
uchar _Globals::Unwind_004751f8() { STUB_BODY(); return 0; }
// !FUNC 0x004751f8 END

// !FUNC 0x00475200 BEGIN
/* 475200-475208 00008 */
uchar _Globals::Unwind_00475200() { STUB_BODY(); return 0; }
// !FUNC 0x00475200 END

// !FUNC 0x00475208 BEGIN
/* 475208-475210 00008 */
uchar _Globals::Unwind_00475208() { STUB_BODY(); return 0; }
// !FUNC 0x00475208 END

// !FUNC 0x00475230 BEGIN
/* 475230-475238 00008 */
uchar _Globals::Unwind_00475230() { STUB_BODY(); return 0; }
// !FUNC 0x00475230 END

// !FUNC 0x00475260 BEGIN
/* 475260-47526B 0000B */
uchar _Globals::Unwind_00475260() { STUB_BODY(); return 0; }
// !FUNC 0x00475260 END

// !FUNC 0x0047526b BEGIN
/* 47526B-475276 0000B */
uchar _Globals::Unwind_0047526b() { STUB_BODY(); return 0; }
// !FUNC 0x0047526b END

// !FUNC 0x00475276 BEGIN
/* 475276-475281 0000B */
uchar _Globals::Unwind_00475276() { STUB_BODY(); return 0; }
// !FUNC 0x00475276 END

// !FUNC 0x004752b0 BEGIN
/* 4752B0-4752C6 00016 */
uchar _Globals::Unwind_004752b0() { STUB_BODY(); return 0; }
// !FUNC 0x004752b0 END

// !FUNC 0x004752c6 BEGIN
/* 4752C6-4752D1 0000B */
uchar _Globals::Unwind_004752c6() { STUB_BODY(); return 0; }
// !FUNC 0x004752c6 END

// !FUNC 0x004752d1 BEGIN
/* 4752D1-4752DC 0000B */
uchar _Globals::Unwind_004752d1() { STUB_BODY(); return 0; }
// !FUNC 0x004752d1 END

// !FUNC 0x004752dc BEGIN
/* 4752DC-4752E7 0000B */
uchar _Globals::Unwind_004752dc() { STUB_BODY(); return 0; }
// !FUNC 0x004752dc END

// !FUNC 0x004752e7 BEGIN
/* 4752E7-4752F2 0000B */
uchar _Globals::Unwind_004752e7() { STUB_BODY(); return 0; }
// !FUNC 0x004752e7 END

// !FUNC 0x00475310 BEGIN
/* 475310-475318 00008 */
uchar _Globals::Unwind_00475310() { STUB_BODY(); return 0; }
// !FUNC 0x00475310 END

// !FUNC 0x00475318 BEGIN
/* 475318-475320 00008 */
uchar _Globals::Unwind_00475318() { STUB_BODY(); return 0; }
// !FUNC 0x00475318 END

// !FUNC 0x00475320 BEGIN
/* 475320-475328 00008 */
uchar _Globals::Unwind_00475320() { STUB_BODY(); return 0; }
// !FUNC 0x00475320 END

// !FUNC 0x00475328 BEGIN
/* 475328-475330 00008 */
uchar _Globals::Unwind_00475328() { STUB_BODY(); return 0; }
// !FUNC 0x00475328 END

// !FUNC 0x00475350 BEGIN
/* 475350-475366 00016 */
uchar _Globals::Unwind_00475350() { STUB_BODY(); return 0; }
// !FUNC 0x00475350 END

// !FUNC 0x00475366 BEGIN
/* 475366-475371 0000B */
uchar _Globals::Unwind_00475366() { STUB_BODY(); return 0; }
// !FUNC 0x00475366 END

// !FUNC 0x00475371 BEGIN
/* 475371-47537C 0000B */
uchar _Globals::Unwind_00475371() { STUB_BODY(); return 0; }
// !FUNC 0x00475371 END

// !FUNC 0x0047537c BEGIN
/* 47537C-475387 0000B */
uchar _Globals::Unwind_0047537c() { STUB_BODY(); return 0; }
// !FUNC 0x0047537c END

// !FUNC 0x00475387 BEGIN
/* 475387-475392 0000B */
uchar _Globals::Unwind_00475387() { STUB_BODY(); return 0; }
// !FUNC 0x00475387 END

// !FUNC 0x004753b0 BEGIN
/* 4753B0-4753B8 00008 */
uchar _Globals::Unwind_004753b0() { STUB_BODY(); return 0; }
// !FUNC 0x004753b0 END

// !FUNC 0x004753e0 BEGIN
/* 4753E0-4753E8 00008 */
uchar _Globals::Unwind_004753e0() { STUB_BODY(); return 0; }
// !FUNC 0x004753e0 END

// !FUNC 0x00475410 BEGIN
/* 475410-475418 00008 */
uchar _Globals::Unwind_00475410() { STUB_BODY(); return 0; }
// !FUNC 0x00475410 END

// !FUNC 0x00475418 BEGIN
/* 475418-475423 0000B */
uchar _Globals::Unwind_00475418() { STUB_BODY(); return 0; }
// !FUNC 0x00475418 END

// !FUNC 0x00475423 BEGIN
/* 475423-47542E 0000B */
uchar _Globals::Unwind_00475423() { STUB_BODY(); return 0; }
// !FUNC 0x00475423 END

// !FUNC 0x00475450 BEGIN
/* 475450-475458 00008 */
uchar _Globals::Unwind_00475450() { STUB_BODY(); return 0; }
// !FUNC 0x00475450 END

// !FUNC 0x00475458 BEGIN
/* 475458-475460 00008 */
uchar _Globals::Unwind_00475458() { STUB_BODY(); return 0; }
// !FUNC 0x00475458 END

// !FUNC 0x00475480 BEGIN
/* 475480-475488 00008 */
uchar _Globals::Unwind_00475480() { STUB_BODY(); return 0; }
// !FUNC 0x00475480 END

// !FUNC 0x00475488 BEGIN
/* 475488-475490 00008 */
uchar _Globals::Unwind_00475488() { STUB_BODY(); return 0; }
// !FUNC 0x00475488 END

// !FUNC 0x004754b0 BEGIN
/* 4754B0-4754B8 00008 */
uchar _Globals::Unwind_004754b0() { STUB_BODY(); return 0; }
// !FUNC 0x004754b0 END

// !FUNC 0x004754e0 BEGIN
/* 4754E0-4754E8 00008 */
uchar _Globals::Unwind_004754e0() { STUB_BODY(); return 0; }
// !FUNC 0x004754e0 END

// !FUNC 0x00475510 BEGIN
/* 475510-475518 00008 */
uchar _Globals::Unwind_00475510() { STUB_BODY(); return 0; }
// !FUNC 0x00475510 END

// !FUNC 0x00475540 BEGIN
/* 475540-47554B 0000B */
uchar _Globals::Unwind_00475540() { STUB_BODY(); return 0; }
// !FUNC 0x00475540 END

// !FUNC 0x00475570 BEGIN
/* 475570-475578 00008 */
uchar _Globals::Unwind_00475570() { STUB_BODY(); return 0; }
// !FUNC 0x00475570 END

// !FUNC 0x004755a0 BEGIN
/* 4755A0-4755A8 00008 */
uchar _Globals::Unwind_004755a0() { STUB_BODY(); return 0; }
// !FUNC 0x004755a0 END

// !FUNC 0x004755d0 BEGIN
/* 4755D0-4755D8 00008 */
uchar _Globals::Unwind_004755d0() { STUB_BODY(); return 0; }
// !FUNC 0x004755d0 END

// !FUNC 0x00475600 BEGIN
/* 475600-475608 00008 */
uchar _Globals::Unwind_00475600() { STUB_BODY(); return 0; }
// !FUNC 0x00475600 END

// !FUNC 0x00475630 BEGIN
/* 475630-47563B 0000B */
uchar _Globals::Unwind_00475630() { STUB_BODY(); return 0; }
// !FUNC 0x00475630 END

// !FUNC 0x00475660 BEGIN
/* 475660-475668 00008 */
uchar _Globals::Unwind_00475660() { STUB_BODY(); return 0; }
// !FUNC 0x00475660 END

// !FUNC 0x00475668 BEGIN
/* 475668-475670 00008 */
uchar _Globals::Unwind_00475668() { STUB_BODY(); return 0; }
// !FUNC 0x00475668 END

// !FUNC 0x00475670 BEGIN
/* 475670-47567B 0000B */
uchar _Globals::Unwind_00475670() { STUB_BODY(); return 0; }
// !FUNC 0x00475670 END

// !FUNC 0x0047567b BEGIN
/* 47567B-475686 0000B */
uchar _Globals::Unwind_0047567b() { STUB_BODY(); return 0; }
// !FUNC 0x0047567b END

// !FUNC 0x004756b0 BEGIN
/* 4756B0-4756B8 00008 */
uchar _Globals::Unwind_004756b0() { STUB_BODY(); return 0; }
// !FUNC 0x004756b0 END

// !FUNC 0x004756b8 BEGIN
/* 4756B8-4756C3 0000B */
uchar _Globals::Unwind_004756b8() { STUB_BODY(); return 0; }
// !FUNC 0x004756b8 END

// !FUNC 0x004756c3 BEGIN
/* 4756C3-4756CE 0000B */
uchar _Globals::Unwind_004756c3() { STUB_BODY(); return 0; }
// !FUNC 0x004756c3 END

// !FUNC 0x004756ce BEGIN
/* 4756CE-4756D9 0000B */
uchar _Globals::Unwind_004756ce() { STUB_BODY(); return 0; }
// !FUNC 0x004756ce END

// !FUNC 0x004756d9 BEGIN
/* 4756D9-4756E4 0000B */
uchar _Globals::Unwind_004756d9() { STUB_BODY(); return 0; }
// !FUNC 0x004756d9 END

// !FUNC 0x00475700 BEGIN
/* 475700-475708 00008 */
uchar _Globals::Unwind_00475700() { STUB_BODY(); return 0; }
// !FUNC 0x00475700 END

// !FUNC 0x00475708 BEGIN
/* 475708-475713 0000B */
uchar _Globals::Unwind_00475708() { STUB_BODY(); return 0; }
// !FUNC 0x00475708 END

// !FUNC 0x00475713 BEGIN
/* 475713-47571E 0000B */
uchar _Globals::Unwind_00475713() { STUB_BODY(); return 0; }
// !FUNC 0x00475713 END

// !FUNC 0x0047571e BEGIN
/* 47571E-475729 0000B */
uchar _Globals::Unwind_0047571e() { STUB_BODY(); return 0; }
// !FUNC 0x0047571e END

// !FUNC 0x00475729 BEGIN
/* 475729-475734 0000B */
uchar _Globals::Unwind_00475729() { STUB_BODY(); return 0; }
// !FUNC 0x00475729 END

// !FUNC 0x00475750 BEGIN
/* 475750-475758 00008 */
uchar _Globals::Unwind_00475750() { STUB_BODY(); return 0; }
// !FUNC 0x00475750 END

// !FUNC 0x00475780 BEGIN
/* 475780-47578B 0000B */
uchar _Globals::Unwind_00475780() { STUB_BODY(); return 0; }
// !FUNC 0x00475780 END

// !FUNC 0x0047578b BEGIN
/* 47578B-475796 0000B */
uchar _Globals::Unwind_0047578b() { STUB_BODY(); return 0; }
// !FUNC 0x0047578b END

// !FUNC 0x00475796 BEGIN
/* 475796-4757A1 0000B */
uchar _Globals::Unwind_00475796() { STUB_BODY(); return 0; }
// !FUNC 0x00475796 END

// !FUNC 0x004757a1 BEGIN
/* 4757A1-4757AC 0000B */
uchar _Globals::Unwind_004757a1() { STUB_BODY(); return 0; }
// !FUNC 0x004757a1 END

// !FUNC 0x004757ac BEGIN
/* 4757AC-4757B7 0000B */
uchar _Globals::Unwind_004757ac() { STUB_BODY(); return 0; }
// !FUNC 0x004757ac END

// !FUNC 0x004757b7 BEGIN
/* 4757B7-4757C2 0000B */
uchar _Globals::Unwind_004757b7() { STUB_BODY(); return 0; }
// !FUNC 0x004757b7 END

// !FUNC 0x004757c2 BEGIN
/* 4757C2-4757CD 0000B */
uchar _Globals::Unwind_004757c2() { STUB_BODY(); return 0; }
// !FUNC 0x004757c2 END

// !FUNC 0x004757cd BEGIN
/* 4757CD-4757D8 0000B */
uchar _Globals::Unwind_004757cd() { STUB_BODY(); return 0; }
// !FUNC 0x004757cd END

// !FUNC 0x004757d8 BEGIN
/* 4757D8-4757E3 0000B */
uchar _Globals::Unwind_004757d8() { STUB_BODY(); return 0; }
// !FUNC 0x004757d8 END

// !FUNC 0x00475800 BEGIN
/* 475800-475808 00008 */
uchar _Globals::Unwind_00475800() { STUB_BODY(); return 0; }
// !FUNC 0x00475800 END

// !FUNC 0x00475830 BEGIN
/* 475830-475838 00008 */
uchar _Globals::Unwind_00475830() { STUB_BODY(); return 0; }
// !FUNC 0x00475830 END

// !FUNC 0x00475838 BEGIN
/* 475838-475843 0000B */
uchar _Globals::Unwind_00475838() { STUB_BODY(); return 0; }
// !FUNC 0x00475838 END

// !FUNC 0x00475843 BEGIN
/* 475843-47584E 0000B */
uchar _Globals::Unwind_00475843() { STUB_BODY(); return 0; }
// !FUNC 0x00475843 END

// !FUNC 0x0047584e BEGIN
/* 47584E-475859 0000B */
uchar _Globals::Unwind_0047584e() { STUB_BODY(); return 0; }
// !FUNC 0x0047584e END

// !FUNC 0x00475859 BEGIN
/* 475859-475864 0000B */
uchar _Globals::Unwind_00475859() { STUB_BODY(); return 0; }
// !FUNC 0x00475859 END

// !FUNC 0x00475864 BEGIN
/* 475864-47586F 0000B */
uchar _Globals::Unwind_00475864() { STUB_BODY(); return 0; }
// !FUNC 0x00475864 END

// !FUNC 0x0047586f BEGIN
/* 47586F-47587A 0000B */
uchar _Globals::Unwind_0047586f() { STUB_BODY(); return 0; }
// !FUNC 0x0047586f END

// !FUNC 0x004758a0 BEGIN
/* 4758A0-4758AB 0000B */
uchar _Globals::Unwind_004758a0() { STUB_BODY(); return 0; }
// !FUNC 0x004758a0 END

// !FUNC 0x004758d0 BEGIN
/* 4758D0-4758D8 00008 */
uchar _Globals::Unwind_004758d0() { STUB_BODY(); return 0; }
// !FUNC 0x004758d0 END

// !FUNC 0x00475900 BEGIN
/* 475900-475908 00008 */
uchar _Globals::Unwind_00475900() { STUB_BODY(); return 0; }
// !FUNC 0x00475900 END

// !FUNC 0x00475908 BEGIN
/* 475908-475913 0000B */
uchar _Globals::Unwind_00475908() { STUB_BODY(); return 0; }
// !FUNC 0x00475908 END

// !FUNC 0x00475930 BEGIN
/* 475930-475938 00008 */
uchar _Globals::Unwind_00475930() { STUB_BODY(); return 0; }
// !FUNC 0x00475930 END

// !FUNC 0x00475938 BEGIN
/* 475938-475943 0000B */
uchar _Globals::Unwind_00475938() { STUB_BODY(); return 0; }
// !FUNC 0x00475938 END

// !FUNC 0x00475943 BEGIN
/* 475943-47594E 0000B */
uchar _Globals::Unwind_00475943() { STUB_BODY(); return 0; }
// !FUNC 0x00475943 END

// !FUNC 0x0047594e BEGIN
/* 47594E-475959 0000B */
uchar _Globals::Unwind_0047594e() { STUB_BODY(); return 0; }
// !FUNC 0x0047594e END

// !FUNC 0x00475959 BEGIN
/* 475959-475964 0000B */
uchar _Globals::Unwind_00475959() { STUB_BODY(); return 0; }
// !FUNC 0x00475959 END

// !FUNC 0x00475964 BEGIN
/* 475964-47596F 0000B */
uchar _Globals::Unwind_00475964() { STUB_BODY(); return 0; }
// !FUNC 0x00475964 END

// !FUNC 0x0047596f BEGIN
/* 47596F-47597A 0000B */
uchar _Globals::Unwind_0047596f() { STUB_BODY(); return 0; }
// !FUNC 0x0047596f END

// !FUNC 0x0047597a BEGIN
/* 47597A-475985 0000B */
uchar _Globals::Unwind_0047597a() { STUB_BODY(); return 0; }
// !FUNC 0x0047597a END

// !FUNC 0x00475985 BEGIN
/* 475985-475990 0000B */
uchar _Globals::Unwind_00475985() { STUB_BODY(); return 0; }
// !FUNC 0x00475985 END

// !FUNC 0x00475990 BEGIN
/* 475990-47599B 0000B */
uchar _Globals::Unwind_00475990() { STUB_BODY(); return 0; }
// !FUNC 0x00475990 END

// !FUNC 0x0047599b BEGIN
/* 47599B-4759A6 0000B */
uchar _Globals::Unwind_0047599b() { STUB_BODY(); return 0; }
// !FUNC 0x0047599b END

// !FUNC 0x004759a6 BEGIN
/* 4759A6-4759B1 0000B */
uchar _Globals::Unwind_004759a6() { STUB_BODY(); return 0; }
// !FUNC 0x004759a6 END

// !FUNC 0x004759b1 BEGIN
/* 4759B1-4759BC 0000B */
uchar _Globals::Unwind_004759b1() { STUB_BODY(); return 0; }
// !FUNC 0x004759b1 END

// !FUNC 0x004759e0 BEGIN
/* 4759E0-4759E8 00008 */
uchar _Globals::Unwind_004759e0() { STUB_BODY(); return 0; }
// !FUNC 0x004759e0 END

// !FUNC 0x004759e8 BEGIN
/* 4759E8-4759F3 0000B */
uchar _Globals::Unwind_004759e8() { STUB_BODY(); return 0; }
// !FUNC 0x004759e8 END

// !FUNC 0x004759f3 BEGIN
/* 4759F3-4759FE 0000B */
uchar _Globals::Unwind_004759f3() { STUB_BODY(); return 0; }
// !FUNC 0x004759f3 END

// !FUNC 0x00475a20 BEGIN
/* 475A20-475A28 00008 */
uchar _Globals::Unwind_00475a20() { STUB_BODY(); return 0; }
// !FUNC 0x00475a20 END

// !FUNC 0x00475a28 BEGIN
/* 475A28-475A33 0000B */
uchar _Globals::Unwind_00475a28() { STUB_BODY(); return 0; }
// !FUNC 0x00475a28 END

// !FUNC 0x00475a50 BEGIN
/* 475A50-475A58 00008 */
uchar _Globals::Unwind_00475a50() { STUB_BODY(); return 0; }
// !FUNC 0x00475a50 END

// !FUNC 0x00475a58 BEGIN
/* 475A58-475A63 0000B */
uchar _Globals::Unwind_00475a58() { STUB_BODY(); return 0; }
// !FUNC 0x00475a58 END

// !FUNC 0x00475a63 BEGIN
/* 475A63-475A6E 0000B */
uchar _Globals::Unwind_00475a63() { STUB_BODY(); return 0; }
// !FUNC 0x00475a63 END

// !FUNC 0x00475a6e BEGIN
/* 475A6E-475A79 0000B */
uchar _Globals::Unwind_00475a6e() { STUB_BODY(); return 0; }
// !FUNC 0x00475a6e END

// !FUNC 0x00475a79 BEGIN
/* 475A79-475A84 0000B */
uchar _Globals::Unwind_00475a79() { STUB_BODY(); return 0; }
// !FUNC 0x00475a79 END

// !FUNC 0x00475aa0 BEGIN
/* 475AA0-475AA8 00008 */
uchar _Globals::Unwind_00475aa0() { STUB_BODY(); return 0; }
// !FUNC 0x00475aa0 END

// !FUNC 0x00475ad0 BEGIN
/* 475AD0-475AD8 00008 */
uchar _Globals::Unwind_00475ad0() { STUB_BODY(); return 0; }
// !FUNC 0x00475ad0 END

// !FUNC 0x00475ad8 BEGIN
/* 475AD8-475AE3 0000B */
uchar _Globals::Unwind_00475ad8() { STUB_BODY(); return 0; }
// !FUNC 0x00475ad8 END

// !FUNC 0x00475ae3 BEGIN
/* 475AE3-475AEE 0000B */
uchar _Globals::Unwind_00475ae3() { STUB_BODY(); return 0; }
// !FUNC 0x00475ae3 END

// !FUNC 0x00475b10 BEGIN
/* 475B10-475B1B 0000B */
uchar _Globals::Unwind_00475b10() { STUB_BODY(); return 0; }
// !FUNC 0x00475b10 END

// !FUNC 0x00475b40 BEGIN
/* 475B40-475B4B 0000B */
uchar _Globals::Unwind_00475b40() { STUB_BODY(); return 0; }
// !FUNC 0x00475b40 END

// !FUNC 0x00475b70 BEGIN
/* 475B70-475B7B 0000B */
uchar _Globals::Unwind_00475b70() { STUB_BODY(); return 0; }
// !FUNC 0x00475b70 END

// !FUNC 0x00475ba0 BEGIN
/* 475BA0-475BAB 0000B */
uchar _Globals::Unwind_00475ba0() { STUB_BODY(); return 0; }
// !FUNC 0x00475ba0 END

// !FUNC 0x00475bd0 BEGIN
/* 475BD0-475BDB 0000B */
uchar _Globals::Unwind_00475bd0() { STUB_BODY(); return 0; }
// !FUNC 0x00475bd0 END

// !FUNC 0x00475c00 BEGIN
/* 475C00-475C0B 0000B */
uchar _Globals::Unwind_00475c00() { STUB_BODY(); return 0; }
// !FUNC 0x00475c00 END

// !FUNC 0x00475c30 BEGIN
/* 475C30-475C3B 0000B */
uchar _Globals::Unwind_00475c30() { STUB_BODY(); return 0; }
// !FUNC 0x00475c30 END

// !FUNC 0x00475c60 BEGIN
/* 475C60-475C6B 0000B */
uchar _Globals::Unwind_00475c60() { STUB_BODY(); return 0; }
// !FUNC 0x00475c60 END

// !FUNC 0x00475c90 BEGIN
/* 475C90-475C9B 0000B */
uchar _Globals::Unwind_00475c90() { STUB_BODY(); return 0; }
// !FUNC 0x00475c90 END

// !FUNC 0x00475cc0 BEGIN
/* 475CC0-475CCB 0000B */
uchar _Globals::Unwind_00475cc0() { STUB_BODY(); return 0; }
// !FUNC 0x00475cc0 END

// !FUNC 0x00475cf0 BEGIN
/* 475CF0-475CFB 0000B */
uchar _Globals::Unwind_00475cf0() { STUB_BODY(); return 0; }
// !FUNC 0x00475cf0 END

// !FUNC 0x00475d20 BEGIN
/* 475D20-475D2B 0000B */
uchar _Globals::Unwind_00475d20() { STUB_BODY(); return 0; }
// !FUNC 0x00475d20 END

// !FUNC 0x00475d50 BEGIN
/* 475D50-475D5B 0000B */
uchar _Globals::Unwind_00475d50() { STUB_BODY(); return 0; }
// !FUNC 0x00475d50 END

// !FUNC 0x00475d80 BEGIN
/* 475D80-475D8B 0000B */
uchar _Globals::Unwind_00475d80() { STUB_BODY(); return 0; }
// !FUNC 0x00475d80 END

// !FUNC 0x00475db0 BEGIN
/* 475DB0-475DBB 0000B */
uchar _Globals::Unwind_00475db0() { STUB_BODY(); return 0; }
// !FUNC 0x00475db0 END

// !FUNC 0x00475de0 BEGIN
/* 475DE0-475DEB 0000B */
uchar _Globals::Unwind_00475de0() { STUB_BODY(); return 0; }
// !FUNC 0x00475de0 END

// !FUNC 0x00475e10 BEGIN
/* 475E10-475E1B 0000B */
uchar _Globals::Unwind_00475e10() { STUB_BODY(); return 0; }
// !FUNC 0x00475e10 END

// !FUNC 0x00475e40 BEGIN
/* 475E40-475E4B 0000B */
uchar _Globals::Unwind_00475e40() { STUB_BODY(); return 0; }
// !FUNC 0x00475e40 END

// !FUNC 0x00475e70 BEGIN
/* 475E70-475E7B 0000B */
uchar _Globals::Unwind_00475e70() { STUB_BODY(); return 0; }
// !FUNC 0x00475e70 END

// !FUNC 0x00475ea0 BEGIN
/* 475EA0-475EA8 00008 */
uchar _Globals::Unwind_00475ea0() { STUB_BODY(); return 0; }
// !FUNC 0x00475ea0 END

// !FUNC 0x00475ea8 BEGIN
/* 475EA8-475EB0 00008 */
uchar _Globals::Unwind_00475ea8() { STUB_BODY(); return 0; }
// !FUNC 0x00475ea8 END

// !FUNC 0x00475ed0 BEGIN
/* 475ED0-475EDB 0000B */
uchar _Globals::Unwind_00475ed0() { STUB_BODY(); return 0; }
// !FUNC 0x00475ed0 END

// !FUNC 0x00475f00 BEGIN
/* 475F00-475F08 00008 */
uchar _Globals::Unwind_00475f00() { STUB_BODY(); return 0; }
// !FUNC 0x00475f00 END

// !FUNC 0x00475f08 BEGIN
/* 475F08-475F13 0000B */
uchar _Globals::Unwind_00475f08() { STUB_BODY(); return 0; }
// !FUNC 0x00475f08 END

// !FUNC 0x00475f30 BEGIN
/* 475F30-475F38 00008 */
uchar _Globals::Unwind_00475f30() { STUB_BODY(); return 0; }
// !FUNC 0x00475f30 END

// !FUNC 0x00475f60 BEGIN
/* 475F60-475F68 00008 */
uchar _Globals::Unwind_00475f60() { STUB_BODY(); return 0; }
// !FUNC 0x00475f60 END

// !FUNC 0x00475f68 BEGIN
/* 475F68-475F70 00008 */
uchar _Globals::Unwind_00475f68() { STUB_BODY(); return 0; }
// !FUNC 0x00475f68 END

// !FUNC 0x00475f90 BEGIN
/* 475F90-475F98 00008 */
uchar _Globals::Unwind_00475f90() { STUB_BODY(); return 0; }
// !FUNC 0x00475f90 END

// !FUNC 0x00475f98 BEGIN
/* 475F98-475FA3 0000B */
uchar _Globals::Unwind_00475f98() { STUB_BODY(); return 0; }
// !FUNC 0x00475f98 END

// !FUNC 0x00475fa3 BEGIN
/* 475FA3-475FAB 00008 */
uchar _Globals::Unwind_00475fa3() { STUB_BODY(); return 0; }
// !FUNC 0x00475fa3 END

// !FUNC 0x00475fab BEGIN
/* 475FAB-475FB3 00008 */
uchar _Globals::Unwind_00475fab() { STUB_BODY(); return 0; }
// !FUNC 0x00475fab END

// !FUNC 0x00475fb3 BEGIN
/* 475FB3-475FBE 0000B */
uchar _Globals::Unwind_00475fb3() { STUB_BODY(); return 0; }
// !FUNC 0x00475fb3 END

// !FUNC 0x00475fbe BEGIN
/* 475FBE-475FC9 0000B */
uchar _Globals::Unwind_00475fbe() { STUB_BODY(); return 0; }
// !FUNC 0x00475fbe END

// !FUNC 0x00475ff0 BEGIN
/* 475FF0-475FF8 00008 */
uchar _Globals::Unwind_00475ff0() { STUB_BODY(); return 0; }
// !FUNC 0x00475ff0 END

// !FUNC 0x00475ff8 BEGIN
/* 475FF8-476006 0000E */
uchar _Globals::Unwind_00475ff8() { STUB_BODY(); return 0; }
// !FUNC 0x00475ff8 END

// !FUNC 0x00476030 BEGIN
/* 476030-47603B 0000B */
uchar _Globals::Unwind_00476030() { STUB_BODY(); return 0; }
// !FUNC 0x00476030 END

// !FUNC 0x00476060 BEGIN
/* 476060-476068 00008 */
uchar _Globals::Unwind_00476060() { STUB_BODY(); return 0; }
// !FUNC 0x00476060 END

// !FUNC 0x00476068 BEGIN
/* 476068-476073 0000B */
uchar _Globals::Unwind_00476068() { STUB_BODY(); return 0; }
// !FUNC 0x00476068 END

// !FUNC 0x004760a0 BEGIN
/* 4760A0-4760A8 00008 */
uchar _Globals::Unwind_004760a0() { STUB_BODY(); return 0; }
// !FUNC 0x004760a0 END

// !FUNC 0x004760a8 BEGIN
/* 4760A8-4760B3 0000B */
uchar _Globals::Unwind_004760a8() { STUB_BODY(); return 0; }
// !FUNC 0x004760a8 END

// !FUNC 0x004760d0 BEGIN
/* 4760D0-4760DB 0000B */
uchar _Globals::Unwind_004760d0() { STUB_BODY(); return 0; }
// !FUNC 0x004760d0 END

// !FUNC 0x00476100 BEGIN
/* 476100-476108 00008 */
uchar _Globals::Unwind_00476100() { STUB_BODY(); return 0; }
// !FUNC 0x00476100 END

// !FUNC 0x00476108 BEGIN
/* 476108-476113 0000B */
uchar _Globals::Unwind_00476108() { STUB_BODY(); return 0; }
// !FUNC 0x00476108 END

// !FUNC 0x00476130 BEGIN
/* 476130-476138 00008 */
uchar _Globals::Unwind_00476130() { STUB_BODY(); return 0; }
// !FUNC 0x00476130 END

// !FUNC 0x00476138 BEGIN
/* 476138-476143 0000B */
uchar _Globals::Unwind_00476138() { STUB_BODY(); return 0; }
// !FUNC 0x00476138 END

// !FUNC 0x00476160 BEGIN
/* 476160-476168 00008 */
uchar _Globals::Unwind_00476160() { STUB_BODY(); return 0; }
// !FUNC 0x00476160 END

// !FUNC 0x00476168 BEGIN
/* 476168-476173 0000B */
uchar _Globals::Unwind_00476168() { STUB_BODY(); return 0; }
// !FUNC 0x00476168 END

// !FUNC 0x00476190 BEGIN
/* 476190-47619B 0000B */
uchar _Globals::Unwind_00476190() { STUB_BODY(); return 0; }
// !FUNC 0x00476190 END

// !FUNC 0x0047619b BEGIN
/* 47619B-4761A6 0000B */
uchar _Globals::Unwind_0047619b() { STUB_BODY(); return 0; }
// !FUNC 0x0047619b END

// !FUNC 0x004761a6 BEGIN
/* 4761A6-4761B1 0000B */
uchar _Globals::Unwind_004761a6() { STUB_BODY(); return 0; }
// !FUNC 0x004761a6 END

// !FUNC 0x004761b1 BEGIN
/* 4761B1-4761BC 0000B */
uchar _Globals::Unwind_004761b1() { STUB_BODY(); return 0; }
// !FUNC 0x004761b1 END

// !FUNC 0x004761e0 BEGIN
/* 4761E0-4761EB 0000B */
uchar _Globals::Unwind_004761e0() { STUB_BODY(); return 0; }
// !FUNC 0x004761e0 END

// !FUNC 0x004761eb BEGIN
/* 4761EB-4761F9 0000E */
uchar _Globals::Unwind_004761eb() { STUB_BODY(); return 0; }
// !FUNC 0x004761eb END

// !FUNC 0x004761f9 BEGIN
/* 4761F9-476207 0000E */
uchar _Globals::Unwind_004761f9() { STUB_BODY(); return 0; }
// !FUNC 0x004761f9 END

// !FUNC 0x00476207 BEGIN
/* 476207-476212 0000B */
uchar _Globals::Unwind_00476207() { STUB_BODY(); return 0; }
// !FUNC 0x00476207 END

// !FUNC 0x00476212 BEGIN
/* 476212-476220 0000E */
uchar _Globals::Unwind_00476212() { STUB_BODY(); return 0; }
// !FUNC 0x00476212 END

// !FUNC 0x00476220 BEGIN
/* 476220-47622E 0000E */
uchar _Globals::Unwind_00476220() { STUB_BODY(); return 0; }
// !FUNC 0x00476220 END

// !FUNC 0x0047622e BEGIN
/* 47622E-476239 0000B */
uchar _Globals::Unwind_0047622e() { STUB_BODY(); return 0; }
// !FUNC 0x0047622e END

// !FUNC 0x00476239 BEGIN
/* 476239-476247 0000E */
uchar _Globals::Unwind_00476239() { STUB_BODY(); return 0; }
// !FUNC 0x00476239 END

// !FUNC 0x00476247 BEGIN
/* 476247-476255 0000E */
uchar _Globals::Unwind_00476247() { STUB_BODY(); return 0; }
// !FUNC 0x00476247 END

// !FUNC 0x00476255 BEGIN
/* 476255-476263 0000E */
uchar _Globals::Unwind_00476255() { STUB_BODY(); return 0; }
// !FUNC 0x00476255 END

// !FUNC 0x00476263 BEGIN
/* 476263-476271 0000E */
uchar _Globals::Unwind_00476263() { STUB_BODY(); return 0; }
// !FUNC 0x00476263 END

// !FUNC 0x00476271 BEGIN
/* 476271-47627F 0000E */
uchar _Globals::Unwind_00476271() { STUB_BODY(); return 0; }
// !FUNC 0x00476271 END

// !FUNC 0x0047627f BEGIN
/* 47627F-47628D 0000E */
uchar _Globals::Unwind_0047627f() { STUB_BODY(); return 0; }
// !FUNC 0x0047627f END

// !FUNC 0x0047628d BEGIN
/* 47628D-47629B 0000E */
uchar _Globals::Unwind_0047628d() { STUB_BODY(); return 0; }
// !FUNC 0x0047628d END

// !FUNC 0x0047629b BEGIN
/* 47629B-4762A9 0000E */
uchar _Globals::Unwind_0047629b() { STUB_BODY(); return 0; }
// !FUNC 0x0047629b END

// !FUNC 0x004762a9 BEGIN
/* 4762A9-4762B7 0000E */
uchar _Globals::Unwind_004762a9() { STUB_BODY(); return 0; }
// !FUNC 0x004762a9 END

// !FUNC 0x004762b7 BEGIN
/* 4762B7-4762C5 0000E */
uchar _Globals::Unwind_004762b7() { STUB_BODY(); return 0; }
// !FUNC 0x004762b7 END

// !FUNC 0x004762f0 BEGIN
/* 4762F0-4762F8 00008 */
uchar _Globals::Unwind_004762f0() { STUB_BODY(); return 0; }
// !FUNC 0x004762f0 END

// !FUNC 0x004762f8 BEGIN
/* 4762F8-47630E 00016 */
uchar _Globals::Unwind_004762f8() { STUB_BODY(); return 0; }
// !FUNC 0x004762f8 END

// !FUNC 0x0047630e BEGIN
/* 47630E-476316 00008 */
uchar _Globals::Unwind_0047630e() { STUB_BODY(); return 0; }
// !FUNC 0x0047630e END

// !FUNC 0x00476316 BEGIN
/* 476316-476321 0000B */
uchar _Globals::Unwind_00476316() { STUB_BODY(); return 0; }
// !FUNC 0x00476316 END

// !FUNC 0x00476321 BEGIN
/* 476321-47632C 0000B */
uchar _Globals::Unwind_00476321() { STUB_BODY(); return 0; }
// !FUNC 0x00476321 END

// !FUNC 0x00476350 BEGIN
/* 476350-47635B 0000B */
uchar _Globals::Unwind_00476350() { STUB_BODY(); return 0; }
// !FUNC 0x00476350 END

// !FUNC 0x0047635b BEGIN
/* 47635B-476369 0000E */
uchar _Globals::Unwind_0047635b() { STUB_BODY(); return 0; }
// !FUNC 0x0047635b END

// !FUNC 0x00476369 BEGIN
/* 476369-476374 0000B */
uchar _Globals::Unwind_00476369() { STUB_BODY(); return 0; }
// !FUNC 0x00476369 END

// !FUNC 0x00476374 BEGIN
/* 476374-476382 0000E */
uchar _Globals::Unwind_00476374() { STUB_BODY(); return 0; }
// !FUNC 0x00476374 END

// !FUNC 0x00476382 BEGIN
/* 476382-476390 0000E */
uchar _Globals::Unwind_00476382() { STUB_BODY(); return 0; }
// !FUNC 0x00476382 END

// !FUNC 0x00476390 BEGIN
/* 476390-47639E 0000E */
uchar _Globals::Unwind_00476390() { STUB_BODY(); return 0; }
// !FUNC 0x00476390 END

// !FUNC 0x0047639e BEGIN
/* 47639E-4763A9 0000B */
uchar _Globals::Unwind_0047639e() { STUB_BODY(); return 0; }
// !FUNC 0x0047639e END

// !FUNC 0x004763a9 BEGIN
/* 4763A9-4763B7 0000E */
uchar _Globals::Unwind_004763a9() { STUB_BODY(); return 0; }
// !FUNC 0x004763a9 END

// !FUNC 0x004763b7 BEGIN
/* 4763B7-4763C2 0000B */
uchar _Globals::Unwind_004763b7() { STUB_BODY(); return 0; }
// !FUNC 0x004763b7 END

// !FUNC 0x004763c2 BEGIN
/* 4763C2-4763D0 0000E */
uchar _Globals::Unwind_004763c2() { STUB_BODY(); return 0; }
// !FUNC 0x004763c2 END

// !FUNC 0x004763d0 BEGIN
/* 4763D0-4763DE 0000E */
uchar _Globals::Unwind_004763d0() { STUB_BODY(); return 0; }
// !FUNC 0x004763d0 END

// !FUNC 0x004763de BEGIN
/* 4763DE-4763E9 0000B */
uchar _Globals::Unwind_004763de() { STUB_BODY(); return 0; }
// !FUNC 0x004763de END

// !FUNC 0x004763e9 BEGIN
/* 4763E9-4763F7 0000E */
uchar _Globals::Unwind_004763e9() { STUB_BODY(); return 0; }
// !FUNC 0x004763e9 END

// !FUNC 0x004763f7 BEGIN
/* 4763F7-476405 0000E */
uchar _Globals::Unwind_004763f7() { STUB_BODY(); return 0; }
// !FUNC 0x004763f7 END

// !FUNC 0x00476405 BEGIN
/* 476405-476410 0000B */
uchar _Globals::Unwind_00476405() { STUB_BODY(); return 0; }
// !FUNC 0x00476405 END

// !FUNC 0x00476410 BEGIN
/* 476410-47641E 0000E */
uchar _Globals::Unwind_00476410() { STUB_BODY(); return 0; }
// !FUNC 0x00476410 END

// !FUNC 0x0047641e BEGIN
/* 47641E-47642C 0000E */
uchar _Globals::Unwind_0047641e() { STUB_BODY(); return 0; }
// !FUNC 0x0047641e END

// !FUNC 0x0047642c BEGIN
/* 47642C-476437 0000B */
uchar _Globals::Unwind_0047642c() { STUB_BODY(); return 0; }
// !FUNC 0x0047642c END

// !FUNC 0x00476437 BEGIN
/* 476437-476445 0000E */
uchar _Globals::Unwind_00476437() { STUB_BODY(); return 0; }
// !FUNC 0x00476437 END

// !FUNC 0x00476470 BEGIN
/* 476470-476478 00008 */
uchar _Globals::Unwind_00476470() { STUB_BODY(); return 0; }
// !FUNC 0x00476470 END

// !FUNC 0x00476478 BEGIN
/* 476478-476480 00008 */
uchar _Globals::Unwind_00476478() { STUB_BODY(); return 0; }
// !FUNC 0x00476478 END

// !FUNC 0x00476480 BEGIN
/* 476480-47648B 0000B */
uchar _Globals::Unwind_00476480() { STUB_BODY(); return 0; }
// !FUNC 0x00476480 END

// !FUNC 0x0047648b BEGIN
/* 47648B-476496 0000B */
uchar _Globals::Unwind_0047648b() { STUB_BODY(); return 0; }
// !FUNC 0x0047648b END

// !FUNC 0x00476496 BEGIN
/* 476496-4764A1 0000B */
uchar _Globals::Unwind_00476496() { STUB_BODY(); return 0; }
// !FUNC 0x00476496 END

// !FUNC 0x004764a1 BEGIN
/* 4764A1-4764AC 0000B */
uchar _Globals::Unwind_004764a1() { STUB_BODY(); return 0; }
// !FUNC 0x004764a1 END

// !FUNC 0x004764d0 BEGIN
/* 4764D0-4764D8 00008 */
uchar _Globals::Unwind_004764d0() { STUB_BODY(); return 0; }
// !FUNC 0x004764d0 END

// !FUNC 0x004764d8 BEGIN
/* 4764D8-4764E3 0000B */
uchar _Globals::Unwind_004764d8() { STUB_BODY(); return 0; }
// !FUNC 0x004764d8 END

// !FUNC 0x004764e3 BEGIN
/* 4764E3-4764EE 0000B */
uchar _Globals::Unwind_004764e3() { STUB_BODY(); return 0; }
// !FUNC 0x004764e3 END

// !FUNC 0x004764ee BEGIN
/* 4764EE-4764F9 0000B */
uchar _Globals::Unwind_004764ee() { STUB_BODY(); return 0; }
// !FUNC 0x004764ee END

// !FUNC 0x004764f9 BEGIN
/* 4764F9-476504 0000B */
uchar _Globals::Unwind_004764f9() { STUB_BODY(); return 0; }
// !FUNC 0x004764f9 END

// !FUNC 0x00476504 BEGIN
/* 476504-47650F 0000B */
uchar _Globals::Unwind_00476504() { STUB_BODY(); return 0; }
// !FUNC 0x00476504 END

// !FUNC 0x00476530 BEGIN
/* 476530-47653B 0000B */
uchar _Globals::Unwind_00476530() { STUB_BODY(); return 0; }
// !FUNC 0x00476530 END

// !FUNC 0x00476560 BEGIN
/* 476560-476568 00008 */
uchar _Globals::Unwind_00476560() { STUB_BODY(); return 0; }
// !FUNC 0x00476560 END

// !FUNC 0x00476590 BEGIN
/* 476590-476598 00008 */
uchar _Globals::Unwind_00476590() { STUB_BODY(); return 0; }
// !FUNC 0x00476590 END

// !FUNC 0x004765c0 BEGIN
/* 4765C0-4765CB 0000B */
uchar _Globals::Unwind_004765c0() { STUB_BODY(); return 0; }
// !FUNC 0x004765c0 END

// !FUNC 0x004765f0 BEGIN
/* 4765F0-4765F8 00008 */
uchar _Globals::Unwind_004765f0() { STUB_BODY(); return 0; }
// !FUNC 0x004765f0 END

// !FUNC 0x00476620 BEGIN
/* 476620-476628 00008 */
uchar _Globals::Unwind_00476620() { STUB_BODY(); return 0; }
// !FUNC 0x00476620 END

// !FUNC 0x00476650 BEGIN
/* 476650-476658 00008 */
uchar _Globals::Unwind_00476650() { STUB_BODY(); return 0; }
// !FUNC 0x00476650 END

// !FUNC 0x00476680 BEGIN
/* 476680-476688 00008 */
uchar _Globals::Unwind_00476680() { STUB_BODY(); return 0; }
// !FUNC 0x00476680 END

// !FUNC 0x004766b0 BEGIN
/* 4766B0-4766BB 0000B */
uchar _Globals::Unwind_004766b0() { STUB_BODY(); return 0; }
// !FUNC 0x004766b0 END

// !FUNC 0x004766bb BEGIN
/* 4766BB-4766C6 0000B */
uchar _Globals::Unwind_004766bb() { STUB_BODY(); return 0; }
// !FUNC 0x004766bb END

// !FUNC 0x004766c6 BEGIN
/* 4766C6-4766D1 0000B */
uchar _Globals::Unwind_004766c6() { STUB_BODY(); return 0; }
// !FUNC 0x004766c6 END

// !FUNC 0x004766d1 BEGIN
/* 4766D1-4766DC 0000B */
uchar _Globals::Unwind_004766d1() { STUB_BODY(); return 0; }
// !FUNC 0x004766d1 END

// !FUNC 0x00476700 BEGIN
/* 476700-47670B 0000B */
uchar _Globals::Unwind_00476700() { STUB_BODY(); return 0; }
// !FUNC 0x00476700 END

// !FUNC 0x00476730 BEGIN
/* 476730-476738 00008 */
uchar _Globals::Unwind_00476730() { STUB_BODY(); return 0; }
// !FUNC 0x00476730 END

// !FUNC 0x00476760 BEGIN
/* 476760-47676B 0000B */
uchar _Globals::Unwind_00476760() { STUB_BODY(); return 0; }
// !FUNC 0x00476760 END

// !FUNC 0x00476790 BEGIN
/* 476790-476798 00008 */
uchar _Globals::Unwind_00476790() { STUB_BODY(); return 0; }
// !FUNC 0x00476790 END

// !FUNC 0x004767c0 BEGIN
/* 4767C0-4767C8 00008 */
uchar _Globals::Unwind_004767c0() { STUB_BODY(); return 0; }
// !FUNC 0x004767c0 END

// !FUNC 0x004767f0 BEGIN
/* 4767F0-4767F8 00008 */
uchar _Globals::Unwind_004767f0() { STUB_BODY(); return 0; }
// !FUNC 0x004767f0 END

// !FUNC 0x004767f8 BEGIN
/* 4767F8-476803 0000B */
uchar _Globals::Unwind_004767f8() { STUB_BODY(); return 0; }
// !FUNC 0x004767f8 END

// !FUNC 0x00476803 BEGIN
/* 476803-47680E 0000B */
uchar _Globals::Unwind_00476803() { STUB_BODY(); return 0; }
// !FUNC 0x00476803 END

// !FUNC 0x0047680e BEGIN
/* 47680E-476819 0000B */
uchar _Globals::Unwind_0047680e() { STUB_BODY(); return 0; }
// !FUNC 0x0047680e END

// !FUNC 0x00476819 BEGIN
/* 476819-476827 0000E */
uchar _Globals::Unwind_00476819() { STUB_BODY(); return 0; }
// !FUNC 0x00476819 END

// !FUNC 0x00476827 BEGIN
/* 476827-476835 0000E */
uchar _Globals::Unwind_00476827() { STUB_BODY(); return 0; }
// !FUNC 0x00476827 END

// !FUNC 0x00476835 BEGIN
/* 476835-47684D 00018 */
uchar _Globals::Unwind_00476835() { STUB_BODY(); return 0; }
// !FUNC 0x00476835 END

// !FUNC 0x0047684d BEGIN
/* 47684D-47685B 0000E */
uchar _Globals::Unwind_0047684d() { STUB_BODY(); return 0; }
// !FUNC 0x0047684d END

// !FUNC 0x0047685b BEGIN
/* 47685B-476869 0000E */
uchar _Globals::Unwind_0047685b() { STUB_BODY(); return 0; }
// !FUNC 0x0047685b END

// !FUNC 0x00476869 BEGIN
/* 476869-476877 0000E */
uchar _Globals::Unwind_00476869() { STUB_BODY(); return 0; }
// !FUNC 0x00476869 END

// !FUNC 0x00476877 BEGIN
/* 476877-476885 0000E */
uchar _Globals::Unwind_00476877() { STUB_BODY(); return 0; }
// !FUNC 0x00476877 END

// !FUNC 0x004768a0 BEGIN
/* 4768A0-4768A8 00008 */
uchar _Globals::Unwind_004768a0() { STUB_BODY(); return 0; }
// !FUNC 0x004768a0 END

// !FUNC 0x004768a8 BEGIN
/* 4768A8-4768CF 00027 */
uchar _Globals::Unwind_004768a8() { STUB_BODY(); return 0; }
// !FUNC 0x004768a8 END

// !FUNC 0x004768cf BEGIN
/* 4768CF-4768F6 00027 */
uchar _Globals::Unwind_004768cf() { STUB_BODY(); return 0; }
// !FUNC 0x004768cf END

// !FUNC 0x004768f6 BEGIN
/* 4768F6-476901 0000B */
uchar _Globals::Unwind_004768f6() { STUB_BODY(); return 0; }
// !FUNC 0x004768f6 END

// !FUNC 0x00476901 BEGIN
/* 476901-47690F 0000E */
uchar _Globals::Unwind_00476901() { STUB_BODY(); return 0; }
// !FUNC 0x00476901 END

// !FUNC 0x0047690f BEGIN
/* 47690F-47691D 0000E */
uchar _Globals::Unwind_0047690f() { STUB_BODY(); return 0; }
// !FUNC 0x0047690f END

// !FUNC 0x0047691d BEGIN
/* 47691D-476935 00018 */
uchar _Globals::Unwind_0047691d() { STUB_BODY(); return 0; }
// !FUNC 0x0047691d END

// !FUNC 0x00476935 BEGIN
/* 476935-476943 0000E */
uchar _Globals::Unwind_00476935() { STUB_BODY(); return 0; }
// !FUNC 0x00476935 END

// !FUNC 0x00476943 BEGIN
/* 476943-476951 0000E */
uchar _Globals::Unwind_00476943() { STUB_BODY(); return 0; }
// !FUNC 0x00476943 END

// !FUNC 0x00476951 BEGIN
/* 476951-47695F 0000E */
uchar _Globals::Unwind_00476951() { STUB_BODY(); return 0; }
// !FUNC 0x00476951 END

// !FUNC 0x0047695f BEGIN
/* 47695F-47696D 0000E */
uchar _Globals::Unwind_0047695f() { STUB_BODY(); return 0; }
// !FUNC 0x0047695f END

// !FUNC 0x00476990 BEGIN
/* 476990-47699B 0000B */
uchar _Globals::Unwind_00476990() { STUB_BODY(); return 0; }
// !FUNC 0x00476990 END

// !FUNC 0x0047699b BEGIN
/* 47699B-4769A6 0000B */
uchar _Globals::Unwind_0047699b() { STUB_BODY(); return 0; }
// !FUNC 0x0047699b END

// !FUNC 0x004769a6 BEGIN
/* 4769A6-4769AE 00008 */
uchar _Globals::Unwind_004769a6() { STUB_BODY(); return 0; }
// !FUNC 0x004769a6 END

// !FUNC 0x004769ae BEGIN
/* 4769AE-4769B9 0000B */
uchar _Globals::Unwind_004769ae() { STUB_BODY(); return 0; }
// !FUNC 0x004769ae END

// !FUNC 0x004769f0 BEGIN
/* 4769F0-476A09 00019 */
uchar _Globals::Unwind_004769f0() { STUB_BODY(); return 0; }
// !FUNC 0x004769f0 END

// !FUNC 0x00476a09 BEGIN
/* 476A09-476A14 0000B */
uchar _Globals::Unwind_00476a09() { STUB_BODY(); return 0; }
// !FUNC 0x00476a09 END

// !FUNC 0x00476a14 BEGIN
/* 476A14-476A1F 0000B */
uchar _Globals::Unwind_00476a14() { STUB_BODY(); return 0; }
// !FUNC 0x00476a14 END

// !FUNC 0x00476a1f BEGIN
/* 476A1F-476A38 00019 */
uchar _Globals::Unwind_00476a1f() { STUB_BODY(); return 0; }
// !FUNC 0x00476a1f END

// !FUNC 0x00476a38 BEGIN
/* 476A38-476A43 0000B */
uchar _Globals::Unwind_00476a38() { STUB_BODY(); return 0; }
// !FUNC 0x00476a38 END

// !FUNC 0x00476a43 BEGIN
/* 476A43-476A4E 0000B */
uchar _Globals::Unwind_00476a43() { STUB_BODY(); return 0; }
// !FUNC 0x00476a43 END

// !FUNC 0x00476a4e BEGIN
/* 476A4E-476A59 0000B */
uchar _Globals::Unwind_00476a4e() { STUB_BODY(); return 0; }
// !FUNC 0x00476a4e END

// !FUNC 0x00476a90 BEGIN
/* 476A90-476A9B 0000B */
uchar _Globals::Unwind_00476a90() { STUB_BODY(); return 0; }
// !FUNC 0x00476a90 END

// !FUNC 0x00476af0 BEGIN
/* 476AF0-476AF8 00008 */
uchar _Globals::Unwind_00476af0() { STUB_BODY(); return 0; }
// !FUNC 0x00476af0 END

// !FUNC 0x00476b20 BEGIN
/* 476B20-476B28 00008 */
uchar _Globals::Unwind_00476b20() { STUB_BODY(); return 0; }
// !FUNC 0x00476b20 END

// !FUNC 0x00476b50 BEGIN
/* 476B50-476B58 00008 */
uchar _Globals::Unwind_00476b50() { STUB_BODY(); return 0; }
// !FUNC 0x00476b50 END

// !FUNC 0x00476b80 BEGIN
/* 476B80-476B88 00008 */
uchar _Globals::Unwind_00476b80() { STUB_BODY(); return 0; }
// !FUNC 0x00476b80 END

// !FUNC 0x00476bb0 BEGIN
/* 476BB0-476BB8 00008 */
uchar _Globals::Unwind_00476bb0() { STUB_BODY(); return 0; }
// !FUNC 0x00476bb0 END

// !FUNC 0x00476be0 BEGIN
/* 476BE0-476BE8 00008 */
uchar _Globals::Unwind_00476be0() { STUB_BODY(); return 0; }
// !FUNC 0x00476be0 END

// !FUNC 0x00476be8 BEGIN
/* 476BE8-476BF6 0000E */
uchar _Globals::Unwind_00476be8() { STUB_BODY(); return 0; }
// !FUNC 0x00476be8 END

// !FUNC 0x00476bf6 BEGIN
/* 476BF6-476C04 0000E */
uchar _Globals::Unwind_00476bf6() { STUB_BODY(); return 0; }
// !FUNC 0x00476bf6 END

// !FUNC 0x00476c20 BEGIN
/* 476C20-476C28 00008 */
uchar _Globals::Unwind_00476c20() { STUB_BODY(); return 0; }
// !FUNC 0x00476c20 END

// !FUNC 0x00476c28 BEGIN
/* 476C28-476C51 00029 */
uchar _Globals::Unwind_00476c28() { STUB_BODY(); return 0; }
// !FUNC 0x00476c28 END

// !FUNC 0x00476c70 BEGIN
/* 476C70-476C7B 0000B */
uchar _Globals::Unwind_00476c70() { STUB_BODY(); return 0; }
// !FUNC 0x00476c70 END

// !FUNC 0x00476cc0 BEGIN
/* 476CC0-476CCB 0000B */
uchar _Globals::Unwind_00476cc0() { STUB_BODY(); return 0; }
// !FUNC 0x00476cc0 END

// !FUNC 0x00476cf0 BEGIN
/* 476CF0-476CFB 0000B */
uchar _Globals::Unwind_00476cf0() { STUB_BODY(); return 0; }
// !FUNC 0x00476cf0 END

// !FUNC 0x00476d20 BEGIN
/* 476D20-476D2B 0000B */
uchar _Globals::Unwind_00476d20() { STUB_BODY(); return 0; }
// !FUNC 0x00476d20 END

// !FUNC 0x00476d50 BEGIN
/* 476D50-476D58 00008 */
uchar _Globals::Unwind_00476d50() { STUB_BODY(); return 0; }
// !FUNC 0x00476d50 END

// !FUNC 0x00476d58 BEGIN
/* 476D58-476D66 0000E */
uchar _Globals::Unwind_00476d58() { STUB_BODY(); return 0; }
// !FUNC 0x00476d58 END

// !FUNC 0x00476d90 BEGIN
/* 476D90-476D98 00008 */
uchar _Globals::Unwind_00476d90() { STUB_BODY(); return 0; }
// !FUNC 0x00476d90 END

// !FUNC 0x00476dc0 BEGIN
/* 476DC0-476DCB 0000B */
uchar _Globals::Unwind_00476dc0() { STUB_BODY(); return 0; }
// !FUNC 0x00476dc0 END

// !FUNC 0x00476df0 BEGIN
/* 476DF0-476DF8 00008 */
uchar _Globals::Unwind_00476df0() { STUB_BODY(); return 0; }
// !FUNC 0x00476df0 END

// !FUNC 0x00476df8 BEGIN
/* 476DF8-476E06 0000E */
uchar _Globals::Unwind_00476df8() { STUB_BODY(); return 0; }
// !FUNC 0x00476df8 END

// !FUNC 0x00476e30 BEGIN
/* 476E30-476E38 00008 */
uchar _Globals::Unwind_00476e30() { STUB_BODY(); return 0; }
// !FUNC 0x00476e30 END

// !FUNC 0x00476e38 BEGIN
/* 476E38-476E46 0000E */
uchar _Globals::Unwind_00476e38() { STUB_BODY(); return 0; }
// !FUNC 0x00476e38 END

// !FUNC 0x00476e70 BEGIN
/* 476E70-476E78 00008 */
uchar _Globals::Unwind_00476e70() { STUB_BODY(); return 0; }
// !FUNC 0x00476e70 END

// !FUNC 0x00476e78 BEGIN
/* 476E78-476E86 0000E */
uchar _Globals::Unwind_00476e78() { STUB_BODY(); return 0; }
// !FUNC 0x00476e78 END

// !FUNC 0x00476eb0 BEGIN
/* 476EB0-476EB8 00008 */
uchar _Globals::Unwind_00476eb0() { STUB_BODY(); return 0; }
// !FUNC 0x00476eb0 END

// !FUNC 0x00476eb8 BEGIN
/* 476EB8-476EC6 0000E */
uchar _Globals::Unwind_00476eb8() { STUB_BODY(); return 0; }
// !FUNC 0x00476eb8 END

// !FUNC 0x00476ef0 BEGIN
/* 476EF0-476EF8 00008 */
uchar _Globals::Unwind_00476ef0() { STUB_BODY(); return 0; }
// !FUNC 0x00476ef0 END

// !FUNC 0x00476f20 BEGIN
/* 476F20-476F28 00008 */
uchar _Globals::Unwind_00476f20() { STUB_BODY(); return 0; }
// !FUNC 0x00476f20 END

// !FUNC 0x00476f28 BEGIN
/* 476F28-476F4F 00027 */
uchar _Globals::Unwind_00476f28() { STUB_BODY(); return 0; }
// !FUNC 0x00476f28 END

// !FUNC 0x00476f70 BEGIN
/* 476F70-476F78 00008 */
uchar _Globals::Unwind_00476f70() { STUB_BODY(); return 0; }
// !FUNC 0x00476f70 END

// !FUNC 0x00476fa0 BEGIN
/* 476FA0-476FAB 0000B */
uchar _Globals::Unwind_00476fa0() { STUB_BODY(); return 0; }
// !FUNC 0x00476fa0 END

// !FUNC 0x00476fd0 BEGIN
/* 476FD0-476FDB 0000B */
uchar _Globals::Unwind_00476fd0() { STUB_BODY(); return 0; }
// !FUNC 0x00476fd0 END

// !FUNC 0x00477000 BEGIN
/* 477000-47700B 0000B */
uchar _Globals::Unwind_00477000() { STUB_BODY(); return 0; }
// !FUNC 0x00477000 END

// !FUNC 0x00477030 BEGIN
/* 477030-47703B 0000B */
uchar _Globals::Unwind_00477030() { STUB_BODY(); return 0; }
// !FUNC 0x00477030 END

// !FUNC 0x00477060 BEGIN
/* 477060-477068 00008 */
uchar _Globals::Unwind_00477060() { STUB_BODY(); return 0; }
// !FUNC 0x00477060 END

// !FUNC 0x00477090 BEGIN
/* 477090-477098 00008 */
uchar _Globals::Unwind_00477090() { STUB_BODY(); return 0; }
// !FUNC 0x00477090 END

// !FUNC 0x00477098 BEGIN
/* 477098-4770A3 0000B */
uchar _Globals::Unwind_00477098() { STUB_BODY(); return 0; }
// !FUNC 0x00477098 END

// !FUNC 0x004770a3 BEGIN
/* 4770A3-4770B1 0000E */
uchar _Globals::Unwind_004770a3() { STUB_BODY(); return 0; }
// !FUNC 0x004770a3 END

// !FUNC 0x004770d0 BEGIN
/* 4770D0-4770D8 00008 */
uchar _Globals::Unwind_004770d0() { STUB_BODY(); return 0; }
// !FUNC 0x004770d0 END

// !FUNC 0x004770d8 BEGIN
/* 4770D8-477101 00029 */
uchar _Globals::Unwind_004770d8() { STUB_BODY(); return 0; }
// !FUNC 0x004770d8 END

// !FUNC 0x00477101 BEGIN
/* 477101-47710F 0000E */
uchar _Globals::Unwind_00477101() { STUB_BODY(); return 0; }
// !FUNC 0x00477101 END

// !FUNC 0x0047710f BEGIN
/* 47710F-47711D 0000E */
uchar _Globals::Unwind_0047710f() { STUB_BODY(); return 0; }
// !FUNC 0x0047710f END

// !FUNC 0x00477140 BEGIN
/* 477140-477148 00008 */
uchar _Globals::Unwind_00477140() { STUB_BODY(); return 0; }
// !FUNC 0x00477140 END

// !FUNC 0x00477148 BEGIN
/* 477148-477150 00008 */
uchar _Globals::Unwind_00477148() { STUB_BODY(); return 0; }
// !FUNC 0x00477148 END

// !FUNC 0x00477150 BEGIN
/* 477150-47715B 0000B */
uchar _Globals::Unwind_00477150() { STUB_BODY(); return 0; }
// !FUNC 0x00477150 END

// !FUNC 0x00477180 BEGIN
/* 477180-477188 00008 */
uchar _Globals::Unwind_00477180() { STUB_BODY(); return 0; }
// !FUNC 0x00477180 END

// !FUNC 0x00477188 BEGIN
/* 477188-4771AF 00027 */
uchar _Globals::Unwind_00477188() { STUB_BODY(); return 0; }
// !FUNC 0x00477188 END

// !FUNC 0x004771af BEGIN
/* 4771AF-4771BD 0000E */
uchar _Globals::Unwind_004771af() { STUB_BODY(); return 0; }
// !FUNC 0x004771af END

// !FUNC 0x004771bd BEGIN
/* 4771BD-4771D5 00018 */
uchar _Globals::Unwind_004771bd() { STUB_BODY(); return 0; }
// !FUNC 0x004771bd END

// !FUNC 0x004771d5 BEGIN
/* 4771D5-4771E3 0000E */
uchar _Globals::Unwind_004771d5() { STUB_BODY(); return 0; }
// !FUNC 0x004771d5 END

// !FUNC 0x004771e3 BEGIN
/* 4771E3-4771F1 0000E */
uchar _Globals::Unwind_004771e3() { STUB_BODY(); return 0; }
// !FUNC 0x004771e3 END

// !FUNC 0x004771f1 BEGIN
/* 4771F1-4771FF 0000E */
uchar _Globals::Unwind_004771f1() { STUB_BODY(); return 0; }
// !FUNC 0x004771f1 END

// !FUNC 0x004771ff BEGIN
/* 4771FF-47720D 0000E */
uchar _Globals::Unwind_004771ff() { STUB_BODY(); return 0; }
// !FUNC 0x004771ff END

// !FUNC 0x0047720d BEGIN
/* 47720D-47721B 0000E */
uchar _Globals::Unwind_0047720d() { STUB_BODY(); return 0; }
// !FUNC 0x0047720d END

// !FUNC 0x0047721b BEGIN
/* 47721B-477229 0000E */
uchar _Globals::Unwind_0047721b() { STUB_BODY(); return 0; }
// !FUNC 0x0047721b END

// !FUNC 0x00477229 BEGIN
/* 477229-477237 0000E */
uchar _Globals::Unwind_00477229() { STUB_BODY(); return 0; }
// !FUNC 0x00477229 END

// !FUNC 0x00477237 BEGIN
/* 477237-477245 0000E */
uchar _Globals::Unwind_00477237() { STUB_BODY(); return 0; }
// !FUNC 0x00477237 END

// !FUNC 0x00477245 BEGIN
/* 477245-477253 0000E */
uchar _Globals::Unwind_00477245() { STUB_BODY(); return 0; }
// !FUNC 0x00477245 END

// !FUNC 0x00477253 BEGIN
/* 477253-477261 0000E */
uchar _Globals::Unwind_00477253() { STUB_BODY(); return 0; }
// !FUNC 0x00477253 END

// !FUNC 0x00477280 BEGIN
/* 477280-47728B 0000B */
uchar _Globals::Unwind_00477280() { STUB_BODY(); return 0; }
// !FUNC 0x00477280 END

// !FUNC 0x004772b0 BEGIN
/* 4772B0-4772BB 0000B */
uchar _Globals::Unwind_004772b0() { STUB_BODY(); return 0; }
// !FUNC 0x004772b0 END

// !FUNC 0x004772e0 BEGIN
/* 4772E0-4772EB 0000B */
uchar _Globals::Unwind_004772e0() { STUB_BODY(); return 0; }
// !FUNC 0x004772e0 END

// !FUNC 0x00477310 BEGIN
/* 477310-47731B 0000B */
uchar _Globals::Unwind_00477310() { STUB_BODY(); return 0; }
// !FUNC 0x00477310 END

// !FUNC 0x00477340 BEGIN
/* 477340-47734B 0000B */
uchar _Globals::Unwind_00477340() { STUB_BODY(); return 0; }
// !FUNC 0x00477340 END

// !FUNC 0x00477370 BEGIN
/* 477370-47737B 0000B */
uchar _Globals::Unwind_00477370() { STUB_BODY(); return 0; }
// !FUNC 0x00477370 END

// !FUNC 0x004773a0 BEGIN
/* 4773A0-4773A8 00008 */
uchar _Globals::Unwind_004773a0() { STUB_BODY(); return 0; }
// !FUNC 0x004773a0 END

// !FUNC 0x004773a8 BEGIN
/* 4773A8-4773D1 00029 */
uchar _Globals::Unwind_004773a8() { STUB_BODY(); return 0; }
// !FUNC 0x004773a8 END

// !FUNC 0x004773f0 BEGIN
/* 4773F0-4773FB 0000B */
uchar _Globals::Unwind_004773f0() { STUB_BODY(); return 0; }
// !FUNC 0x004773f0 END

// !FUNC 0x00477430 BEGIN
/* 477430-477438 00008 */
uchar _Globals::Unwind_00477430() { STUB_BODY(); return 0; }
// !FUNC 0x00477430 END

// !FUNC 0x00477438 BEGIN
/* 477438-477443 0000B */
uchar _Globals::Unwind_00477438() { STUB_BODY(); return 0; }
// !FUNC 0x00477438 END

// !FUNC 0x00477443 BEGIN
/* 477443-47744E 0000B */
uchar _Globals::Unwind_00477443() { STUB_BODY(); return 0; }
// !FUNC 0x00477443 END

// !FUNC 0x0047744e BEGIN
/* 47744E-477459 0000B */
uchar _Globals::Unwind_0047744e() { STUB_BODY(); return 0; }
// !FUNC 0x0047744e END

// !FUNC 0x00477459 BEGIN
/* 477459-477464 0000B */
uchar _Globals::Unwind_00477459() { STUB_BODY(); return 0; }
// !FUNC 0x00477459 END

// !FUNC 0x00477480 BEGIN
/* 477480-477488 00008 */
uchar _Globals::Unwind_00477480() { STUB_BODY(); return 0; }
// !FUNC 0x00477480 END

// !FUNC 0x004774b0 BEGIN
/* 4774B0-4774B8 00008 */
uchar _Globals::Unwind_004774b0() { STUB_BODY(); return 0; }
// !FUNC 0x004774b0 END

// !FUNC 0x004774e0 BEGIN
/* 4774E0-4774E8 00008 */
uchar _Globals::Unwind_004774e0() { STUB_BODY(); return 0; }
// !FUNC 0x004774e0 END

// !FUNC 0x004774e8 BEGIN
/* 4774E8-4774F0 00008 */
uchar _Globals::Unwind_004774e8() { STUB_BODY(); return 0; }
// !FUNC 0x004774e8 END

// !FUNC 0x00477510 BEGIN
/* 477510-477518 00008 */
uchar _Globals::Unwind_00477510() { STUB_BODY(); return 0; }
// !FUNC 0x00477510 END

// !FUNC 0x00477518 BEGIN
/* 477518-477526 0000E */
uchar _Globals::Unwind_00477518() { STUB_BODY(); return 0; }
// !FUNC 0x00477518 END

// !FUNC 0x00477526 BEGIN
/* 477526-47752E 00008 */
uchar _Globals::Unwind_00477526() { STUB_BODY(); return 0; }
// !FUNC 0x00477526 END

// !FUNC 0x00477550 BEGIN
/* 477550-477558 00008 */
uchar _Globals::Unwind_00477550() { STUB_BODY(); return 0; }
// !FUNC 0x00477550 END

// !FUNC 0x00477558 BEGIN
/* 477558-477566 0000E */
uchar _Globals::Unwind_00477558() { STUB_BODY(); return 0; }
// !FUNC 0x00477558 END

// !FUNC 0x00477566 BEGIN
/* 477566-47756E 00008 */
uchar _Globals::Unwind_00477566() { STUB_BODY(); return 0; }
// !FUNC 0x00477566 END

// !FUNC 0x00477590 BEGIN
/* 477590-477598 00008 */
uchar _Globals::Unwind_00477590() { STUB_BODY(); return 0; }
// !FUNC 0x00477590 END

// !FUNC 0x00477598 BEGIN
/* 477598-4775A0 00008 */
uchar _Globals::Unwind_00477598() { STUB_BODY(); return 0; }
// !FUNC 0x00477598 END

// !FUNC 0x004775c0 BEGIN
/* 4775C0-4775C8 00008 */
uchar _Globals::Unwind_004775c0() { STUB_BODY(); return 0; }
// !FUNC 0x004775c0 END

// !FUNC 0x004775c8 BEGIN
/* 4775C8-4775D3 0000B */
uchar _Globals::Unwind_004775c8() { STUB_BODY(); return 0; }
// !FUNC 0x004775c8 END

// !FUNC 0x004775f0 BEGIN
/* 4775F0-4775FB 0000B */
uchar _Globals::Unwind_004775f0() { STUB_BODY(); return 0; }
// !FUNC 0x004775f0 END

// !FUNC 0x004775fb BEGIN
/* 4775FB-477606 0000B */
uchar _Globals::Unwind_004775fb() { STUB_BODY(); return 0; }
// !FUNC 0x004775fb END

// !FUNC 0x00477630 BEGIN
/* 477630-477638 00008 */
uchar _Globals::Unwind_00477630() { STUB_BODY(); return 0; }
// !FUNC 0x00477630 END

// !FUNC 0x00477638 BEGIN
/* 477638-477640 00008 */
uchar _Globals::Unwind_00477638() { STUB_BODY(); return 0; }
// !FUNC 0x00477638 END

// !FUNC 0x00477640 BEGIN
/* 477640-47764B 0000B */
uchar _Globals::Unwind_00477640() { STUB_BODY(); return 0; }
// !FUNC 0x00477640 END

// !FUNC 0x0047764b BEGIN
/* 47764B-477653 00008 */
uchar _Globals::Unwind_0047764b() { STUB_BODY(); return 0; }
// !FUNC 0x0047764b END

// !FUNC 0x00477653 BEGIN
/* 477653-47765B 00008 */
uchar _Globals::Unwind_00477653() { STUB_BODY(); return 0; }
// !FUNC 0x00477653 END

// !FUNC 0x00477690 BEGIN
/* 477690-47769B 0000B */
uchar _Globals::Unwind_00477690() { STUB_BODY(); return 0; }
// !FUNC 0x00477690 END

// !FUNC 0x0047769b BEGIN
/* 47769B-4776A3 00008 */
uchar _Globals::Unwind_0047769b() { STUB_BODY(); return 0; }
// !FUNC 0x0047769b END

// !FUNC 0x004776c0 BEGIN
/* 4776C0-4776C8 00008 */
uchar _Globals::Unwind_004776c0() { STUB_BODY(); return 0; }
// !FUNC 0x004776c0 END

// !FUNC 0x004776c8 BEGIN
/* 4776C8-4776D3 0000B */
uchar _Globals::Unwind_004776c8() { STUB_BODY(); return 0; }
// !FUNC 0x004776c8 END

// !FUNC 0x004776f0 BEGIN
/* 4776F0-4776F8 00008 */
uchar _Globals::Unwind_004776f0() { STUB_BODY(); return 0; }
// !FUNC 0x004776f0 END

// !FUNC 0x00477720 BEGIN
/* 477720-477728 00008 */
uchar _Globals::Unwind_00477720() { STUB_BODY(); return 0; }
// !FUNC 0x00477720 END

// !FUNC 0x00477750 BEGIN
/* 477750-47775B 0000B */
uchar _Globals::Unwind_00477750() { STUB_BODY(); return 0; }
// !FUNC 0x00477750 END

// !FUNC 0x00477780 BEGIN
/* 477780-477788 00008 */
uchar _Globals::Unwind_00477780() { STUB_BODY(); return 0; }
// !FUNC 0x00477780 END

// !FUNC 0x00477788 BEGIN
/* 477788-477793 0000B */
uchar _Globals::Unwind_00477788() { STUB_BODY(); return 0; }
// !FUNC 0x00477788 END

// !FUNC 0x00477793 BEGIN
/* 477793-47779E 0000B */
uchar _Globals::Unwind_00477793() { STUB_BODY(); return 0; }
// !FUNC 0x00477793 END

// !FUNC 0x0047779e BEGIN
/* 47779E-4777A9 0000B */
uchar _Globals::Unwind_0047779e() { STUB_BODY(); return 0; }
// !FUNC 0x0047779e END

// !FUNC 0x004777a9 BEGIN
/* 4777A9-4777B4 0000B */
uchar _Globals::Unwind_004777a9() { STUB_BODY(); return 0; }
// !FUNC 0x004777a9 END

// !FUNC 0x004777b4 BEGIN
/* 4777B4-4777BC 00008 */
uchar _Globals::Unwind_004777b4() { STUB_BODY(); return 0; }
// !FUNC 0x004777b4 END

// !FUNC 0x004777e0 BEGIN
/* 4777E0-4777EB 0000B */
uchar _Globals::Unwind_004777e0() { STUB_BODY(); return 0; }
// !FUNC 0x004777e0 END

// !FUNC 0x00477810 BEGIN
/* 477810-47781B 0000B */
uchar _Globals::Unwind_00477810() { STUB_BODY(); return 0; }
// !FUNC 0x00477810 END

// !FUNC 0x00477840 BEGIN
/* 477840-477848 00008 */
uchar _Globals::Unwind_00477840() { STUB_BODY(); return 0; }
// !FUNC 0x00477840 END

// !FUNC 0x00477848 BEGIN
/* 477848-477856 0000E */
uchar _Globals::Unwind_00477848() { STUB_BODY(); return 0; }
// !FUNC 0x00477848 END

// !FUNC 0x00477856 BEGIN
/* 477856-477864 0000E */
uchar _Globals::Unwind_00477856() { STUB_BODY(); return 0; }
// !FUNC 0x00477856 END

// !FUNC 0x00477864 BEGIN
/* 477864-477872 0000E */
uchar _Globals::Unwind_00477864() { STUB_BODY(); return 0; }
// !FUNC 0x00477864 END

// !FUNC 0x00477872 BEGIN
/* 477872-47787A 00008 */
uchar _Globals::Unwind_00477872() { STUB_BODY(); return 0; }
// !FUNC 0x00477872 END

// !FUNC 0x0047787a BEGIN
/* 47787A-477885 0000B */
uchar _Globals::Unwind_0047787a() { STUB_BODY(); return 0; }
// !FUNC 0x0047787a END

// !FUNC 0x004778a0 BEGIN
/* 4778A0-4778AB 0000B */
uchar _Globals::Unwind_004778a0() { STUB_BODY(); return 0; }
// !FUNC 0x004778a0 END

// !FUNC 0x004778d0 BEGIN
/* 4778D0-4778D8 00008 */
uchar _Globals::Unwind_004778d0() { STUB_BODY(); return 0; }
// !FUNC 0x004778d0 END

// !FUNC 0x004778d8 BEGIN
/* 4778D8-4778E6 0000E */
uchar _Globals::Unwind_004778d8() { STUB_BODY(); return 0; }
// !FUNC 0x004778d8 END

// !FUNC 0x00477910 BEGIN
/* 477910-47791B 0000B */
uchar _Globals::Unwind_00477910() { STUB_BODY(); return 0; }
// !FUNC 0x00477910 END

// !FUNC 0x00477940 BEGIN
/* 477940-477948 00008 */
uchar _Globals::Unwind_00477940() { STUB_BODY(); return 0; }
// !FUNC 0x00477940 END

// !FUNC 0x00477970 BEGIN
/* 477970-47797B 0000B */
uchar _Globals::Unwind_00477970() { STUB_BODY(); return 0; }
// !FUNC 0x00477970 END

// !FUNC 0x004779a0 BEGIN
/* 4779A0-4779AB 0000B */
uchar _Globals::Unwind_004779a0() { STUB_BODY(); return 0; }
// !FUNC 0x004779a0 END

// !FUNC 0x004779ab BEGIN
/* 4779AB-4779B6 0000B */
uchar _Globals::Unwind_004779ab() { STUB_BODY(); return 0; }
// !FUNC 0x004779ab END

// !FUNC 0x004779e0 BEGIN
/* 4779E0-4779EB 0000B */
uchar _Globals::Unwind_004779e0() { STUB_BODY(); return 0; }
// !FUNC 0x004779e0 END

// !FUNC 0x00477a10 BEGIN
/* 477A10-477A1B 0000B */
uchar _Globals::Unwind_00477a10() { STUB_BODY(); return 0; }
// !FUNC 0x00477a10 END

// !FUNC 0x00477a40 BEGIN
/* 477A40-477A48 00008 */
uchar _Globals::Unwind_00477a40() { STUB_BODY(); return 0; }
// !FUNC 0x00477a40 END

// !FUNC 0x00477a48 BEGIN
/* 477A48-477A53 0000B */
uchar _Globals::Unwind_00477a48() { STUB_BODY(); return 0; }
// !FUNC 0x00477a48 END

// !FUNC 0x00477a53 BEGIN
/* 477A53-477A61 0000E */
uchar _Globals::Unwind_00477a53() { STUB_BODY(); return 0; }
// !FUNC 0x00477a53 END

// !FUNC 0x00477a61 BEGIN
/* 477A61-477A79 00018 */
uchar _Globals::Unwind_00477a61() { STUB_BODY(); return 0; }
// !FUNC 0x00477a61 END

// !FUNC 0x00477a79 BEGIN
/* 477A79-477A87 0000E */
uchar _Globals::Unwind_00477a79() { STUB_BODY(); return 0; }
// !FUNC 0x00477a79 END

// !FUNC 0x00477a87 BEGIN
/* 477A87-477A95 0000E */
uchar _Globals::Unwind_00477a87() { STUB_BODY(); return 0; }
// !FUNC 0x00477a87 END

// !FUNC 0x00477a95 BEGIN
/* 477A95-477AA3 0000E */
uchar _Globals::Unwind_00477a95() { STUB_BODY(); return 0; }
// !FUNC 0x00477a95 END

// !FUNC 0x00477aa3 BEGIN
/* 477AA3-477AB1 0000E */
uchar _Globals::Unwind_00477aa3() { STUB_BODY(); return 0; }
// !FUNC 0x00477aa3 END

// !FUNC 0x00477ab1 BEGIN
/* 477AB1-477ABF 0000E */
uchar _Globals::Unwind_00477ab1() { STUB_BODY(); return 0; }
// !FUNC 0x00477ab1 END

// !FUNC 0x00477abf BEGIN
/* 477ABF-477ACD 0000E */
uchar _Globals::Unwind_00477abf() { STUB_BODY(); return 0; }
// !FUNC 0x00477abf END

// !FUNC 0x00477acd BEGIN
/* 477ACD-477ADB 0000E */
uchar _Globals::Unwind_00477acd() { STUB_BODY(); return 0; }
// !FUNC 0x00477acd END

// !FUNC 0x00477adb BEGIN
/* 477ADB-477AE9 0000E */
uchar _Globals::Unwind_00477adb() { STUB_BODY(); return 0; }
// !FUNC 0x00477adb END

// !FUNC 0x00477ae9 BEGIN
/* 477AE9-477AF7 0000E */
uchar _Globals::Unwind_00477ae9() { STUB_BODY(); return 0; }
// !FUNC 0x00477ae9 END

// !FUNC 0x00477af7 BEGIN
/* 477AF7-477B05 0000E */
uchar _Globals::Unwind_00477af7() { STUB_BODY(); return 0; }
// !FUNC 0x00477af7 END

// !FUNC 0x00477b05 BEGIN
/* 477B05-477B10 0000B */
uchar _Globals::Unwind_00477b05() { STUB_BODY(); return 0; }
// !FUNC 0x00477b05 END

// !FUNC 0x00477b10 BEGIN
/* 477B10-477B1B 0000B */
uchar _Globals::Unwind_00477b10() { STUB_BODY(); return 0; }
// !FUNC 0x00477b10 END

// !FUNC 0x00477b1b BEGIN
/* 477B1B-477B26 0000B */
uchar _Globals::Unwind_00477b1b() { STUB_BODY(); return 0; }
// !FUNC 0x00477b1b END

// !FUNC 0x00477b50 BEGIN
/* 477B50-477B5B 0000B */
uchar _Globals::Unwind_00477b50() { STUB_BODY(); return 0; }
// !FUNC 0x00477b50 END

// !FUNC 0x00477b5b BEGIN
/* 477B5B-477B66 0000B */
uchar _Globals::Unwind_00477b5b() { STUB_BODY(); return 0; }
// !FUNC 0x00477b5b END

// !FUNC 0x00477b66 BEGIN
/* 477B66-477B71 0000B */
uchar _Globals::Unwind_00477b66() { STUB_BODY(); return 0; }
// !FUNC 0x00477b66 END

// !FUNC 0x00477b90 BEGIN
/* 477B90-477B98 00008 */
uchar _Globals::Unwind_00477b90() { STUB_BODY(); return 0; }
// !FUNC 0x00477b90 END

// !FUNC 0x00477b98 BEGIN
/* 477B98-477BA6 0000E */
uchar _Globals::Unwind_00477b98() { STUB_BODY(); return 0; }
// !FUNC 0x00477b98 END

// !FUNC 0x00477bd0 BEGIN
/* 477BD0-477BD8 00008 */
uchar _Globals::Unwind_00477bd0() { STUB_BODY(); return 0; }
// !FUNC 0x00477bd0 END

// !FUNC 0x00477c00 BEGIN
/* 477C00-477C0B 0000B */
uchar _Globals::Unwind_00477c00() { STUB_BODY(); return 0; }
// !FUNC 0x00477c00 END

// !FUNC 0x00477c30 BEGIN
/* 477C30-477C3B 0000B */
uchar _Globals::Unwind_00477c30() { STUB_BODY(); return 0; }
// !FUNC 0x00477c30 END

// !FUNC 0x00477c60 BEGIN
/* 477C60-477C6B 0000B */
uchar _Globals::Unwind_00477c60() { STUB_BODY(); return 0; }
// !FUNC 0x00477c60 END

// !FUNC 0x00477c90 BEGIN
/* 477C90-477C9B 0000B */
uchar _Globals::Unwind_00477c90() { STUB_BODY(); return 0; }
// !FUNC 0x00477c90 END

// !FUNC 0x00477cc0 BEGIN
/* 477CC0-477CCB 0000B */
uchar _Globals::Unwind_00477cc0() { STUB_BODY(); return 0; }
// !FUNC 0x00477cc0 END

// !FUNC 0x00477cf0 BEGIN
/* 477CF0-477CFB 0000B */
uchar _Globals::Unwind_00477cf0() { STUB_BODY(); return 0; }
// !FUNC 0x00477cf0 END

// !FUNC 0x00477d20 BEGIN
/* 477D20-477D28 00008 */
uchar _Globals::Unwind_00477d20() { STUB_BODY(); return 0; }
// !FUNC 0x00477d20 END

// !FUNC 0x00477d28 BEGIN
/* 477D28-477D33 0000B */
uchar _Globals::Unwind_00477d28() { STUB_BODY(); return 0; }
// !FUNC 0x00477d28 END

// !FUNC 0x00477d33 BEGIN
/* 477D33-477D41 0000E */
uchar _Globals::Unwind_00477d33() { STUB_BODY(); return 0; }
// !FUNC 0x00477d33 END

// !FUNC 0x00477d41 BEGIN
/* 477D41-477D4C 0000B */
uchar _Globals::Unwind_00477d41() { STUB_BODY(); return 0; }
// !FUNC 0x00477d41 END

// !FUNC 0x00477d4c BEGIN
/* 477D4C-477D57 0000B */
uchar _Globals::Unwind_00477d4c() { STUB_BODY(); return 0; }
// !FUNC 0x00477d4c END

// !FUNC 0x00477d57 BEGIN
/* 477D57-477D62 0000B */
uchar _Globals::Unwind_00477d57() { STUB_BODY(); return 0; }
// !FUNC 0x00477d57 END

// !FUNC 0x00477d62 BEGIN
/* 477D62-477D6D 0000B */
uchar _Globals::Unwind_00477d62() { STUB_BODY(); return 0; }
// !FUNC 0x00477d62 END

// !FUNC 0x00477d90 BEGIN
/* 477D90-477D98 00008 */
uchar _Globals::Unwind_00477d90() { STUB_BODY(); return 0; }
// !FUNC 0x00477d90 END

// !FUNC 0x00477d98 BEGIN
/* 477D98-477DA3 0000B */
uchar _Globals::Unwind_00477d98() { STUB_BODY(); return 0; }
// !FUNC 0x00477d98 END

// !FUNC 0x00477da3 BEGIN
/* 477DA3-477DB1 0000E */
uchar _Globals::Unwind_00477da3() { STUB_BODY(); return 0; }
// !FUNC 0x00477da3 END

// !FUNC 0x00477dd0 BEGIN
/* 477DD0-477DD8 00008 */
uchar _Globals::Unwind_00477dd0() { STUB_BODY(); return 0; }
// !FUNC 0x00477dd0 END

// !FUNC 0x00477dd8 BEGIN
/* 477DD8-477DE3 0000B */
uchar _Globals::Unwind_00477dd8() { STUB_BODY(); return 0; }
// !FUNC 0x00477dd8 END

// !FUNC 0x00477e00 BEGIN
/* 477E00-477E0B 0000B */
uchar _Globals::Unwind_00477e00() { STUB_BODY(); return 0; }
// !FUNC 0x00477e00 END

// !FUNC 0x00477e30 BEGIN
/* 477E30-477E3B 0000B */
uchar _Globals::Unwind_00477e30() { STUB_BODY(); return 0; }
// !FUNC 0x00477e30 END

// !FUNC 0x00477e60 BEGIN
/* 477E60-477E68 00008 */
uchar _Globals::Unwind_00477e60() { STUB_BODY(); return 0; }
// !FUNC 0x00477e60 END

// !FUNC 0x00477e90 BEGIN
/* 477E90-477E98 00008 */
uchar _Globals::Unwind_00477e90() { STUB_BODY(); return 0; }
// !FUNC 0x00477e90 END

// !FUNC 0x00477e98 BEGIN
/* 477E98-477EA6 0000E */
uchar _Globals::Unwind_00477e98() { STUB_BODY(); return 0; }
// !FUNC 0x00477e98 END

// !FUNC 0x00477ed0 BEGIN
/* 477ED0-477ED8 00008 */
uchar _Globals::Unwind_00477ed0() { STUB_BODY(); return 0; }
// !FUNC 0x00477ed0 END

// !FUNC 0x00477f00 BEGIN
/* 477F00-477F08 00008 */
uchar _Globals::Unwind_00477f00() { STUB_BODY(); return 0; }
// !FUNC 0x00477f00 END

// !FUNC 0x00477f08 BEGIN
/* 477F08-477F13 0000B */
uchar _Globals::Unwind_00477f08() { STUB_BODY(); return 0; }
// !FUNC 0x00477f08 END

// !FUNC 0x00477f30 BEGIN
/* 477F30-477F3B 0000B */
uchar _Globals::Unwind_00477f30() { STUB_BODY(); return 0; }
// !FUNC 0x00477f30 END

// !FUNC 0x00477f60 BEGIN
/* 477F60-477F6B 0000B */
uchar _Globals::Unwind_00477f60() { STUB_BODY(); return 0; }
// !FUNC 0x00477f60 END

// !FUNC 0x00477f90 BEGIN
/* 477F90-477F9B 0000B */
uchar _Globals::Unwind_00477f90() { STUB_BODY(); return 0; }
// !FUNC 0x00477f90 END

// !FUNC 0x00477fc0 BEGIN
/* 477FC0-477FC8 00008 */
uchar _Globals::Unwind_00477fc0() { STUB_BODY(); return 0; }
// !FUNC 0x00477fc0 END

// !FUNC 0x00477fc8 BEGIN
/* 477FC8-477FD3 0000B */
uchar _Globals::Unwind_00477fc8() { STUB_BODY(); return 0; }
// !FUNC 0x00477fc8 END

// !FUNC 0x00477ff0 BEGIN
/* 477FF0-477FFB 0000B */
uchar _Globals::Unwind_00477ff0() { STUB_BODY(); return 0; }
// !FUNC 0x00477ff0 END

// !FUNC 0x00478020 BEGIN
/* 478020-47802B 0000B */
uchar _Globals::Unwind_00478020() { STUB_BODY(); return 0; }
// !FUNC 0x00478020 END

// !FUNC 0x00478050 BEGIN
/* 478050-47805B 0000B */
uchar _Globals::Unwind_00478050() { STUB_BODY(); return 0; }
// !FUNC 0x00478050 END

// !FUNC 0x00478080 BEGIN
/* 478080-47808B 0000B */
uchar _Globals::Unwind_00478080() { STUB_BODY(); return 0; }
// !FUNC 0x00478080 END

// !FUNC 0x004780b0 BEGIN
/* 4780B0-4780B8 00008 */
uchar _Globals::Unwind_004780b0() { STUB_BODY(); return 0; }
// !FUNC 0x004780b0 END

// !FUNC 0x004780b8 BEGIN
/* 4780B8-4780C3 0000B */
uchar _Globals::Unwind_004780b8() { STUB_BODY(); return 0; }
// !FUNC 0x004780b8 END

// !FUNC 0x004780c3 BEGIN
/* 4780C3-4780D1 0000E */
uchar _Globals::Unwind_004780c3() { STUB_BODY(); return 0; }
// !FUNC 0x004780c3 END

// !FUNC 0x004780d1 BEGIN
/* 4780D1-4780DC 0000B */
uchar _Globals::Unwind_004780d1() { STUB_BODY(); return 0; }
// !FUNC 0x004780d1 END

// !FUNC 0x004780dc BEGIN
/* 4780DC-4780E7 0000B */
uchar _Globals::Unwind_004780dc() { STUB_BODY(); return 0; }
// !FUNC 0x004780dc END

// !FUNC 0x004780e7 BEGIN
/* 4780E7-4780F2 0000B */
uchar _Globals::Unwind_004780e7() { STUB_BODY(); return 0; }
// !FUNC 0x004780e7 END

// !FUNC 0x004780f2 BEGIN
/* 4780F2-4780FD 0000B */
uchar _Globals::Unwind_004780f2() { STUB_BODY(); return 0; }
// !FUNC 0x004780f2 END

// !FUNC 0x00478120 BEGIN
/* 478120-478128 00008 */
uchar _Globals::Unwind_00478120() { STUB_BODY(); return 0; }
// !FUNC 0x00478120 END

// !FUNC 0x00478128 BEGIN
/* 478128-478133 0000B */
uchar _Globals::Unwind_00478128() { STUB_BODY(); return 0; }
// !FUNC 0x00478128 END

// !FUNC 0x00478133 BEGIN
/* 478133-478141 0000E */
uchar _Globals::Unwind_00478133() { STUB_BODY(); return 0; }
// !FUNC 0x00478133 END

// !FUNC 0x00478160 BEGIN
/* 478160-478168 00008 */
uchar _Globals::Unwind_00478160() { STUB_BODY(); return 0; }
// !FUNC 0x00478160 END

// !FUNC 0x00478168 BEGIN
/* 478168-478173 0000B */
uchar _Globals::Unwind_00478168() { STUB_BODY(); return 0; }
// !FUNC 0x00478168 END

// !FUNC 0x00478190 BEGIN
/* 478190-47819B 0000B */
uchar _Globals::Unwind_00478190() { STUB_BODY(); return 0; }
// !FUNC 0x00478190 END

// !FUNC 0x004781c0 BEGIN
/* 4781C0-4781C8 00008 */
uchar _Globals::Unwind_004781c0() { STUB_BODY(); return 0; }
// !FUNC 0x004781c0 END

// !FUNC 0x004781c8 BEGIN
/* 4781C8-4781D3 0000B */
uchar _Globals::Unwind_004781c8() { STUB_BODY(); return 0; }
// !FUNC 0x004781c8 END

// !FUNC 0x004781d3 BEGIN
/* 4781D3-4781DE 0000B */
uchar _Globals::Unwind_004781d3() { STUB_BODY(); return 0; }
// !FUNC 0x004781d3 END

// !FUNC 0x00478200 BEGIN
/* 478200-47820B 0000B */
uchar _Globals::Unwind_00478200() { STUB_BODY(); return 0; }
// !FUNC 0x00478200 END

// !FUNC 0x00478230 BEGIN
/* 478230-478238 00008 */
uchar _Globals::Unwind_00478230() { STUB_BODY(); return 0; }
// !FUNC 0x00478230 END

// !FUNC 0x00478238 BEGIN
/* 478238-478243 0000B */
uchar _Globals::Unwind_00478238() { STUB_BODY(); return 0; }
// !FUNC 0x00478238 END

// !FUNC 0x00478260 BEGIN
/* 478260-478268 00008 */
uchar _Globals::Unwind_00478260() { STUB_BODY(); return 0; }
// !FUNC 0x00478260 END

// !FUNC 0x00478268 BEGIN
/* 478268-478273 0000B */
uchar _Globals::Unwind_00478268() { STUB_BODY(); return 0; }
// !FUNC 0x00478268 END

// !FUNC 0x00478290 BEGIN
/* 478290-478298 00008 */
uchar _Globals::Unwind_00478290() { STUB_BODY(); return 0; }
// !FUNC 0x00478290 END

// !FUNC 0x004782c0 BEGIN
/* 4782C0-4782CB 0000B */
uchar _Globals::Unwind_004782c0() { STUB_BODY(); return 0; }
// !FUNC 0x004782c0 END

// !FUNC 0x004782f0 BEGIN
/* 4782F0-4782F8 00008 */
uchar _Globals::Unwind_004782f0() { STUB_BODY(); return 0; }
// !FUNC 0x004782f0 END

// !FUNC 0x00478320 BEGIN
/* 478320-478328 00008 */
uchar _Globals::Unwind_00478320() { STUB_BODY(); return 0; }
// !FUNC 0x00478320 END

// !FUNC 0x00478350 BEGIN
/* 478350-478358 00008 */
uchar _Globals::Unwind_00478350() { STUB_BODY(); return 0; }
// !FUNC 0x00478350 END

// !FUNC 0x00478358 BEGIN
/* 478358-478363 0000B */
uchar _Globals::Unwind_00478358() { STUB_BODY(); return 0; }
// !FUNC 0x00478358 END

// !FUNC 0x00478363 BEGIN
/* 478363-47836B 00008 */
uchar _Globals::Unwind_00478363() { STUB_BODY(); return 0; }
// !FUNC 0x00478363 END

// !FUNC 0x00478390 BEGIN
/* 478390-478398 00008 */
uchar _Globals::Unwind_00478390() { STUB_BODY(); return 0; }
// !FUNC 0x00478390 END

// !FUNC 0x00478398 BEGIN
/* 478398-4783BF 00027 */
uchar _Globals::Unwind_00478398() { STUB_BODY(); return 0; }
// !FUNC 0x00478398 END

// !FUNC 0x004783bf BEGIN
/* 4783BF-4783CD 0000E */
uchar _Globals::Unwind_004783bf() { STUB_BODY(); return 0; }
// !FUNC 0x004783bf END

// !FUNC 0x004783cd BEGIN
/* 4783CD-4783DB 0000E */
uchar _Globals::Unwind_004783cd() { STUB_BODY(); return 0; }
// !FUNC 0x004783cd END

// !FUNC 0x00478400 BEGIN
/* 478400-47840B 0000B */
uchar _Globals::Unwind_00478400() { STUB_BODY(); return 0; }
// !FUNC 0x00478400 END

// !FUNC 0x00478430 BEGIN
/* 478430-47843B 0000B */
uchar _Globals::Unwind_00478430() { STUB_BODY(); return 0; }
// !FUNC 0x00478430 END

// !FUNC 0x00478460 BEGIN
/* 478460-47846B 0000B */
uchar _Globals::Unwind_00478460() { STUB_BODY(); return 0; }
// !FUNC 0x00478460 END

// !FUNC 0x0047846b BEGIN
/* 47846B-478476 0000B */
uchar _Globals::Unwind_0047846b() { STUB_BODY(); return 0; }
// !FUNC 0x0047846b END

// !FUNC 0x00478476 BEGIN
/* 478476-478481 0000B */
uchar _Globals::Unwind_00478476() { STUB_BODY(); return 0; }
// !FUNC 0x00478476 END

// !FUNC 0x004784a0 BEGIN
/* 4784A0-4784AB 0000B */
uchar _Globals::Unwind_004784a0() { STUB_BODY(); return 0; }
// !FUNC 0x004784a0 END

// !FUNC 0x004784ab BEGIN
/* 4784AB-4784B6 0000B */
uchar _Globals::Unwind_004784ab() { STUB_BODY(); return 0; }
// !FUNC 0x004784ab END

// !FUNC 0x004784b6 BEGIN
/* 4784B6-4784C1 0000B */
uchar _Globals::Unwind_004784b6() { STUB_BODY(); return 0; }
// !FUNC 0x004784b6 END

// !FUNC 0x004784e0 BEGIN
/* 4784E0-4784E8 00008 */
uchar _Globals::Unwind_004784e0() { STUB_BODY(); return 0; }
// !FUNC 0x004784e0 END

// !FUNC 0x00478510 BEGIN
/* 478510-478518 00008 */
uchar _Globals::Unwind_00478510() { STUB_BODY(); return 0; }
// !FUNC 0x00478510 END

// !FUNC 0x00478518 BEGIN
/* 478518-47853F 00027 */
uchar _Globals::Unwind_00478518() { STUB_BODY(); return 0; }
// !FUNC 0x00478518 END

// !FUNC 0x00478560 BEGIN
/* 478560-478568 00008 */
uchar _Globals::Unwind_00478560() { STUB_BODY(); return 0; }
// !FUNC 0x00478560 END

// !FUNC 0x00478568 BEGIN
/* 478568-478573 0000B */
uchar _Globals::Unwind_00478568() { STUB_BODY(); return 0; }
// !FUNC 0x00478568 END

// !FUNC 0x00478573 BEGIN
/* 478573-47857E 0000B */
uchar _Globals::Unwind_00478573() { STUB_BODY(); return 0; }
// !FUNC 0x00478573 END

// !FUNC 0x0047857e BEGIN
/* 47857E-47858C 0000E */
uchar _Globals::Unwind_0047857e() { STUB_BODY(); return 0; }
// !FUNC 0x0047857e END

// !FUNC 0x0047858c BEGIN
/* 47858C-47859A 0000E */
uchar _Globals::Unwind_0047858c() { STUB_BODY(); return 0; }
// !FUNC 0x0047858c END

// !FUNC 0x0047859a BEGIN
/* 47859A-4785A8 0000E */
uchar _Globals::Unwind_0047859a() { STUB_BODY(); return 0; }
// !FUNC 0x0047859a END

// !FUNC 0x004785a8 BEGIN
/* 4785A8-4785B6 0000E */
uchar _Globals::Unwind_004785a8() { STUB_BODY(); return 0; }
// !FUNC 0x004785a8 END

// !FUNC 0x004785b6 BEGIN
/* 4785B6-4785C4 0000E */
uchar _Globals::Unwind_004785b6() { STUB_BODY(); return 0; }
// !FUNC 0x004785b6 END

// !FUNC 0x004785c4 BEGIN
/* 4785C4-4785D2 0000E */
uchar _Globals::Unwind_004785c4() { STUB_BODY(); return 0; }
// !FUNC 0x004785c4 END

// !FUNC 0x004785f0 BEGIN
/* 4785F0-4785F8 00008 */
uchar _Globals::Unwind_004785f0() { STUB_BODY(); return 0; }
// !FUNC 0x004785f0 END

// !FUNC 0x004785f8 BEGIN
/* 4785F8-478603 0000B */
uchar _Globals::Unwind_004785f8() { STUB_BODY(); return 0; }
// !FUNC 0x004785f8 END

// !FUNC 0x00478620 BEGIN
/* 478620-47862B 0000B */
uchar _Globals::Unwind_00478620() { STUB_BODY(); return 0; }
// !FUNC 0x00478620 END

// !FUNC 0x0047862b BEGIN
/* 47862B-478636 0000B */
uchar _Globals::Unwind_0047862b() { STUB_BODY(); return 0; }
// !FUNC 0x0047862b END

// !FUNC 0x00478636 BEGIN
/* 478636-478641 0000B */
uchar _Globals::Unwind_00478636() { STUB_BODY(); return 0; }
// !FUNC 0x00478636 END

// !FUNC 0x00478660 BEGIN
/* 478660-478668 00008 */
uchar _Globals::Unwind_00478660() { STUB_BODY(); return 0; }
// !FUNC 0x00478660 END

// !FUNC 0x00478668 BEGIN
/* 478668-47868F 00027 */
uchar _Globals::Unwind_00478668() { STUB_BODY(); return 0; }
// !FUNC 0x00478668 END

// !FUNC 0x0047868f BEGIN
/* 47868F-47869D 0000E */
uchar _Globals::Unwind_0047868f() { STUB_BODY(); return 0; }
// !FUNC 0x0047868f END

// !FUNC 0x0047869d BEGIN
/* 47869D-4786AB 0000E */
uchar _Globals::Unwind_0047869d() { STUB_BODY(); return 0; }
// !FUNC 0x0047869d END

// !FUNC 0x004786ab BEGIN
/* 4786AB-4786B9 0000E */
uchar _Globals::Unwind_004786ab() { STUB_BODY(); return 0; }
// !FUNC 0x004786ab END

// !FUNC 0x004786b9 BEGIN
/* 4786B9-4786C7 0000E */
uchar _Globals::Unwind_004786b9() { STUB_BODY(); return 0; }
// !FUNC 0x004786b9 END

// !FUNC 0x004786c7 BEGIN
/* 4786C7-4786DF 00018 */
uchar _Globals::Unwind_004786c7() { STUB_BODY(); return 0; }
// !FUNC 0x004786c7 END

// !FUNC 0x00478700 BEGIN
/* 478700-47870B 0000B */
uchar _Globals::Unwind_00478700() { STUB_BODY(); return 0; }
// !FUNC 0x00478700 END

// !FUNC 0x00478730 BEGIN
/* 478730-478738 00008 */
uchar _Globals::Unwind_00478730() { STUB_BODY(); return 0; }
// !FUNC 0x00478730 END

// !FUNC 0x00478738 BEGIN
/* 478738-478743 0000B */
uchar _Globals::Unwind_00478738() { STUB_BODY(); return 0; }
// !FUNC 0x00478738 END

// !FUNC 0x00478743 BEGIN
/* 478743-47874E 0000B */
uchar _Globals::Unwind_00478743() { STUB_BODY(); return 0; }
// !FUNC 0x00478743 END

// !FUNC 0x0047874e BEGIN
/* 47874E-47875C 0000E */
uchar _Globals::Unwind_0047874e() { STUB_BODY(); return 0; }
// !FUNC 0x0047874e END

// !FUNC 0x0047875c BEGIN
/* 47875C-47876A 0000E */
uchar _Globals::Unwind_0047875c() { STUB_BODY(); return 0; }
// !FUNC 0x0047875c END

// !FUNC 0x0047876a BEGIN
/* 47876A-478778 0000E */
uchar _Globals::Unwind_0047876a() { STUB_BODY(); return 0; }
// !FUNC 0x0047876a END

// !FUNC 0x00478778 BEGIN
/* 478778-478786 0000E */
uchar _Globals::Unwind_00478778() { STUB_BODY(); return 0; }
// !FUNC 0x00478778 END

// !FUNC 0x00478786 BEGIN
/* 478786-478794 0000E */
uchar _Globals::Unwind_00478786() { STUB_BODY(); return 0; }
// !FUNC 0x00478786 END

// !FUNC 0x00478794 BEGIN
/* 478794-4787A2 0000E */
uchar _Globals::Unwind_00478794() { STUB_BODY(); return 0; }
// !FUNC 0x00478794 END

// !FUNC 0x004787a2 BEGIN
/* 4787A2-4787AA 00008 */
uchar _Globals::Unwind_004787a2() { STUB_BODY(); return 0; }
// !FUNC 0x004787a2 END

// !FUNC 0x004787d0 BEGIN
/* 4787D0-4787D8 00008 */
uchar _Globals::Unwind_004787d0() { STUB_BODY(); return 0; }
// !FUNC 0x004787d0 END

// !FUNC 0x004787d8 BEGIN
/* 4787D8-4787E3 0000B */
uchar _Globals::Unwind_004787d8() { STUB_BODY(); return 0; }
// !FUNC 0x004787d8 END

// !FUNC 0x004787e3 BEGIN
/* 4787E3-4787F1 0000E */
uchar _Globals::Unwind_004787e3() { STUB_BODY(); return 0; }
// !FUNC 0x004787e3 END

// !FUNC 0x004787f1 BEGIN
/* 4787F1-4787FF 0000E */
uchar _Globals::Unwind_004787f1() { STUB_BODY(); return 0; }
// !FUNC 0x004787f1 END

// !FUNC 0x004787ff BEGIN
/* 4787FF-47880D 0000E */
uchar _Globals::Unwind_004787ff() { STUB_BODY(); return 0; }
// !FUNC 0x004787ff END

// !FUNC 0x0047880d BEGIN
/* 47880D-47881B 0000E */
uchar _Globals::Unwind_0047880d() { STUB_BODY(); return 0; }
// !FUNC 0x0047880d END

// !FUNC 0x0047881b BEGIN
/* 47881B-478833 00018 */
uchar _Globals::Unwind_0047881b() { STUB_BODY(); return 0; }
// !FUNC 0x0047881b END

// !FUNC 0x00478850 BEGIN
/* 478850-47885B 0000B */
uchar _Globals::Unwind_00478850() { STUB_BODY(); return 0; }
// !FUNC 0x00478850 END

// !FUNC 0x00478880 BEGIN
/* 478880-47888B 0000B */
uchar _Globals::Unwind_00478880() { STUB_BODY(); return 0; }
// !FUNC 0x00478880 END

// !FUNC 0x004788b0 BEGIN
/* 4788B0-4788B8 00008 */
uchar _Globals::Unwind_004788b0() { STUB_BODY(); return 0; }
// !FUNC 0x004788b0 END

// !FUNC 0x004788b8 BEGIN
/* 4788B8-4788C3 0000B */
uchar _Globals::Unwind_004788b8() { STUB_BODY(); return 0; }
// !FUNC 0x004788b8 END

// !FUNC 0x004788c3 BEGIN
/* 4788C3-4788D1 0000E */
uchar _Globals::Unwind_004788c3() { STUB_BODY(); return 0; }
// !FUNC 0x004788c3 END

// !FUNC 0x004788d1 BEGIN
/* 4788D1-4788DF 0000E */
uchar _Globals::Unwind_004788d1() { STUB_BODY(); return 0; }
// !FUNC 0x004788d1 END

// !FUNC 0x004788df BEGIN
/* 4788DF-4788E7 00008 */
uchar _Globals::Unwind_004788df() { STUB_BODY(); return 0; }
// !FUNC 0x004788df END

// !FUNC 0x004788e7 BEGIN
/* 4788E7-4788F2 0000B */
uchar _Globals::Unwind_004788e7() { STUB_BODY(); return 0; }
// !FUNC 0x004788e7 END

// !FUNC 0x004788f2 BEGIN
/* 4788F2-4788FD 0000B */
uchar _Globals::Unwind_004788f2() { STUB_BODY(); return 0; }
// !FUNC 0x004788f2 END

// !FUNC 0x004788fd BEGIN
/* 4788FD-478908 0000B */
uchar _Globals::Unwind_004788fd() { STUB_BODY(); return 0; }
// !FUNC 0x004788fd END

// !FUNC 0x00478908 BEGIN
/* 478908-478913 0000B */
uchar _Globals::Unwind_00478908() { STUB_BODY(); return 0; }
// !FUNC 0x00478908 END

// !FUNC 0x00478913 BEGIN
/* 478913-47891B 00008 */
uchar _Globals::Unwind_00478913() { STUB_BODY(); return 0; }
// !FUNC 0x00478913 END

// !FUNC 0x0047891b BEGIN
/* 47891B-478926 0000B */
uchar _Globals::Unwind_0047891b() { STUB_BODY(); return 0; }
// !FUNC 0x0047891b END

// !FUNC 0x00478926 BEGIN
/* 478926-478931 0000B */
uchar _Globals::Unwind_00478926() { STUB_BODY(); return 0; }
// !FUNC 0x00478926 END

// !FUNC 0x00478931 BEGIN
/* 478931-47893C 0000B */
uchar _Globals::Unwind_00478931() { STUB_BODY(); return 0; }
// !FUNC 0x00478931 END

// !FUNC 0x0047893c BEGIN
/* 47893C-478947 0000B */
uchar _Globals::Unwind_0047893c() { STUB_BODY(); return 0; }
// !FUNC 0x0047893c END

// !FUNC 0x00478947 BEGIN
/* 478947-478952 0000B */
uchar _Globals::Unwind_00478947() { STUB_BODY(); return 0; }
// !FUNC 0x00478947 END

// !FUNC 0x00478952 BEGIN
/* 478952-47895D 0000B */
uchar _Globals::Unwind_00478952() { STUB_BODY(); return 0; }
// !FUNC 0x00478952 END

// !FUNC 0x0047895d BEGIN
/* 47895D-478968 0000B */
uchar _Globals::Unwind_0047895d() { STUB_BODY(); return 0; }
// !FUNC 0x0047895d END

// !FUNC 0x00478968 BEGIN
/* 478968-478973 0000B */
uchar _Globals::Unwind_00478968() { STUB_BODY(); return 0; }
// !FUNC 0x00478968 END

// !FUNC 0x00478973 BEGIN
/* 478973-47897E 0000B */
uchar _Globals::Unwind_00478973() { STUB_BODY(); return 0; }
// !FUNC 0x00478973 END

// !FUNC 0x0047897e BEGIN
/* 47897E-478989 0000B */
uchar _Globals::Unwind_0047897e() { STUB_BODY(); return 0; }
// !FUNC 0x0047897e END

// !FUNC 0x004789b0 BEGIN
/* 4789B0-4789B8 00008 */
uchar _Globals::Unwind_004789b0() { STUB_BODY(); return 0; }
// !FUNC 0x004789b0 END

// !FUNC 0x004789e0 BEGIN
/* 4789E0-4789E8 00008 */
uchar _Globals::Unwind_004789e0() { STUB_BODY(); return 0; }
// !FUNC 0x004789e0 END

// !FUNC 0x00478a10 BEGIN
/* 478A10-478A18 00008 */
uchar _Globals::Unwind_00478a10() { STUB_BODY(); return 0; }
// !FUNC 0x00478a10 END

// !FUNC 0x00478a18 BEGIN
/* 478A18-478A23 0000B */
uchar _Globals::Unwind_00478a18() { STUB_BODY(); return 0; }
// !FUNC 0x00478a18 END

// !FUNC 0x00478a40 BEGIN
/* 478A40-478A48 00008 */
uchar _Globals::Unwind_00478a40() { STUB_BODY(); return 0; }
// !FUNC 0x00478a40 END

// !FUNC 0x00478a48 BEGIN
/* 478A48-478A6F 00027 */
uchar _Globals::Unwind_00478a48() { STUB_BODY(); return 0; }
// !FUNC 0x00478a48 END

// !FUNC 0x00478a90 BEGIN
/* 478A90-478A98 00008 */
uchar _Globals::Unwind_00478a90() { STUB_BODY(); return 0; }
// !FUNC 0x00478a90 END

// !FUNC 0x00478ac0 BEGIN
/* 478AC0-478AC8 00008 */
uchar _Globals::Unwind_00478ac0() { STUB_BODY(); return 0; }
// !FUNC 0x00478ac0 END

// !FUNC 0x00478af0 BEGIN
/* 478AF0-478AFB 0000B */
uchar _Globals::Unwind_00478af0() { STUB_BODY(); return 0; }
// !FUNC 0x00478af0 END

// !FUNC 0x00478b20 BEGIN
/* 478B20-478B2B 0000B */
uchar _Globals::Unwind_00478b20() { STUB_BODY(); return 0; }
// !FUNC 0x00478b20 END

// !FUNC 0x00478b50 BEGIN
/* 478B50-478B5B 0000B */
uchar _Globals::Unwind_00478b50() { STUB_BODY(); return 0; }
// !FUNC 0x00478b50 END

// !FUNC 0x00478b80 BEGIN
/* 478B80-478B8B 0000B */
uchar _Globals::Unwind_00478b80() { STUB_BODY(); return 0; }
// !FUNC 0x00478b80 END

// !FUNC 0x00478bb0 BEGIN
/* 478BB0-478BB8 00008 */
uchar _Globals::Unwind_00478bb0() { STUB_BODY(); return 0; }
// !FUNC 0x00478bb0 END

// !FUNC 0x00478bb8 BEGIN
/* 478BB8-478BC0 00008 */
uchar _Globals::Unwind_00478bb8() { STUB_BODY(); return 0; }
// !FUNC 0x00478bb8 END

// !FUNC 0x00478be0 BEGIN
/* 478BE0-478BE8 00008 */
uchar _Globals::Unwind_00478be0() { STUB_BODY(); return 0; }
// !FUNC 0x00478be0 END

// !FUNC 0x00478be8 BEGIN
/* 478BE8-478BFE 00016 */
uchar _Globals::Unwind_00478be8() { STUB_BODY(); return 0; }
// !FUNC 0x00478be8 END

// !FUNC 0x00478c20 BEGIN
/* 478C20-478C28 00008 */
uchar _Globals::Unwind_00478c20() { STUB_BODY(); return 0; }
// !FUNC 0x00478c20 END

// !FUNC 0x00478c28 BEGIN
/* 478C28-478C33 0000B */
uchar _Globals::Unwind_00478c28() { STUB_BODY(); return 0; }
// !FUNC 0x00478c28 END

// !FUNC 0x00478c33 BEGIN
/* 478C33-478C4B 00018 */
uchar _Globals::Unwind_00478c33() { STUB_BODY(); return 0; }
// !FUNC 0x00478c33 END

// !FUNC 0x00478c70 BEGIN
/* 478C70-478C78 00008 */
uchar _Globals::Unwind_00478c70() { STUB_BODY(); return 0; }
// !FUNC 0x00478c70 END

// !FUNC 0x00478c78 BEGIN
/* 478C78-478C8E 00016 */
uchar _Globals::Unwind_00478c78() { STUB_BODY(); return 0; }
// !FUNC 0x00478c78 END

// !FUNC 0x00478c8e BEGIN
/* 478C8E-478C96 00008 */
uchar _Globals::Unwind_00478c8e() { STUB_BODY(); return 0; }
// !FUNC 0x00478c8e END

// !FUNC 0x00478c96 BEGIN
/* 478C96-478CA1 0000B */
uchar _Globals::Unwind_00478c96() { STUB_BODY(); return 0; }
// !FUNC 0x00478c96 END

// !FUNC 0x00478ca1 BEGIN
/* 478CA1-478CA9 00008 */
uchar _Globals::Unwind_00478ca1() { STUB_BODY(); return 0; }
// !FUNC 0x00478ca1 END

// !FUNC 0x00478ca9 BEGIN
/* 478CA9-478CB1 00008 */
uchar _Globals::Unwind_00478ca9() { STUB_BODY(); return 0; }
// !FUNC 0x00478ca9 END

// !FUNC 0x00478cb1 BEGIN
/* 478CB1-478CBC 0000B */
uchar _Globals::Unwind_00478cb1() { STUB_BODY(); return 0; }
// !FUNC 0x00478cb1 END

// !FUNC 0x00478cbc BEGIN
/* 478CBC-478CC7 0000B */
uchar _Globals::Unwind_00478cbc() { STUB_BODY(); return 0; }
// !FUNC 0x00478cbc END

// !FUNC 0x00478cc7 BEGIN
/* 478CC7-478CD2 0000B */
uchar _Globals::Unwind_00478cc7() { STUB_BODY(); return 0; }
// !FUNC 0x00478cc7 END

// !FUNC 0x00478cd2 BEGIN
/* 478CD2-478CDD 0000B */
uchar _Globals::Unwind_00478cd2() { STUB_BODY(); return 0; }
// !FUNC 0x00478cd2 END

// !FUNC 0x00478cdd BEGIN
/* 478CDD-478CE8 0000B */
uchar _Globals::Unwind_00478cdd() { STUB_BODY(); return 0; }
// !FUNC 0x00478cdd END

// !FUNC 0x00478ce8 BEGIN
/* 478CE8-478CF3 0000B */
uchar _Globals::Unwind_00478ce8() { STUB_BODY(); return 0; }
// !FUNC 0x00478ce8 END

// !FUNC 0x00478cf3 BEGIN
/* 478CF3-478CFE 0000B */
uchar _Globals::Unwind_00478cf3() { STUB_BODY(); return 0; }
// !FUNC 0x00478cf3 END

// !FUNC 0x00478cfe BEGIN
/* 478CFE-478D09 0000B */
uchar _Globals::Unwind_00478cfe() { STUB_BODY(); return 0; }
// !FUNC 0x00478cfe END

// !FUNC 0x00478d09 BEGIN
/* 478D09-478D14 0000B */
uchar _Globals::Unwind_00478d09() { STUB_BODY(); return 0; }
// !FUNC 0x00478d09 END

// !FUNC 0x00478d14 BEGIN
/* 478D14-478D1F 0000B */
uchar _Globals::Unwind_00478d14() { STUB_BODY(); return 0; }
// !FUNC 0x00478d14 END

// !FUNC 0x00478d1f BEGIN
/* 478D1F-478D27 00008 */
uchar _Globals::Unwind_00478d1f() { STUB_BODY(); return 0; }
// !FUNC 0x00478d1f END

// !FUNC 0x00478d27 BEGIN
/* 478D27-478D32 0000B */
uchar _Globals::Unwind_00478d27() { STUB_BODY(); return 0; }
// !FUNC 0x00478d27 END

// !FUNC 0x00478d50 BEGIN
/* 478D50-478D58 00008 */
uchar _Globals::Unwind_00478d50() { STUB_BODY(); return 0; }
// !FUNC 0x00478d50 END

// !FUNC 0x00478d80 BEGIN
/* 478D80-478D88 00008 */
uchar _Globals::Unwind_00478d80() { STUB_BODY(); return 0; }
// !FUNC 0x00478d80 END

// !FUNC 0x00478dd0 BEGIN
/* 478DD0-478DD8 00008 */
uchar _Globals::Unwind_00478dd0() { STUB_BODY(); return 0; }
// !FUNC 0x00478dd0 END

// !FUNC 0x00478e00 BEGIN
/* 478E00-478E08 00008 */
uchar _Globals::Unwind_00478e00() { STUB_BODY(); return 0; }
// !FUNC 0x00478e00 END

// !FUNC 0x00478e08 BEGIN
/* 478E08-478E2F 00027 */
uchar _Globals::Unwind_00478e08() { STUB_BODY(); return 0; }
// !FUNC 0x00478e08 END

// !FUNC 0x00478e2f BEGIN
/* 478E2F-478E56 00027 */
uchar _Globals::Unwind_00478e2f() { STUB_BODY(); return 0; }
// !FUNC 0x00478e2f END

// !FUNC 0x00478e56 BEGIN
/* 478E56-478E61 0000B */
uchar _Globals::Unwind_00478e56() { STUB_BODY(); return 0; }
// !FUNC 0x00478e56 END

// !FUNC 0x00478e80 BEGIN
/* 478E80-478E8B 0000B */
uchar _Globals::Unwind_00478e80() { STUB_BODY(); return 0; }
// !FUNC 0x00478e80 END

// !FUNC 0x00478eb0 BEGIN
/* 478EB0-478EB8 00008 */
uchar _Globals::Unwind_00478eb0() { STUB_BODY(); return 0; }
// !FUNC 0x00478eb0 END

// !FUNC 0x00478ee0 BEGIN
/* 478EE0-478EE8 00008 */
uchar _Globals::Unwind_00478ee0() { STUB_BODY(); return 0; }
// !FUNC 0x00478ee0 END

// !FUNC 0x00478ee8 BEGIN
/* 478EE8-478F0F 00027 */
uchar _Globals::Unwind_00478ee8() { STUB_BODY(); return 0; }
// !FUNC 0x00478ee8 END

// !FUNC 0x00478f30 BEGIN
/* 478F30-478F38 00008 */
uchar _Globals::Unwind_00478f30() { STUB_BODY(); return 0; }
// !FUNC 0x00478f30 END

// !FUNC 0x00478f38 BEGIN
/* 478F38-478F5F 00027 */
uchar _Globals::Unwind_00478f38() { STUB_BODY(); return 0; }
// !FUNC 0x00478f38 END

// !FUNC 0x00478f5f BEGIN
/* 478F5F-478F6A 0000B */
uchar _Globals::Unwind_00478f5f() { STUB_BODY(); return 0; }
// !FUNC 0x00478f5f END

// !FUNC 0x00478f90 BEGIN
/* 478F90-478F98 00008 */
uchar _Globals::Unwind_00478f90() { STUB_BODY(); return 0; }
// !FUNC 0x00478f90 END

// !FUNC 0x00478f98 BEGIN
/* 478F98-478FA3 0000B */
uchar _Globals::Unwind_00478f98() { STUB_BODY(); return 0; }
// !FUNC 0x00478f98 END

// !FUNC 0x00478fa3 BEGIN
/* 478FA3-478FAE 0000B */
uchar _Globals::Unwind_00478fa3() { STUB_BODY(); return 0; }
// !FUNC 0x00478fa3 END

// !FUNC 0x00478fd0 BEGIN
/* 478FD0-478FD8 00008 */
uchar _Globals::Unwind_00478fd0() { STUB_BODY(); return 0; }
// !FUNC 0x00478fd0 END

// !FUNC 0x00478fd8 BEGIN
/* 478FD8-478FE0 00008 */
uchar _Globals::Unwind_00478fd8() { STUB_BODY(); return 0; }
// !FUNC 0x00478fd8 END

// !FUNC 0x00478fe0 BEGIN
/* 478FE0-478FE8 00008 */
uchar _Globals::Unwind_00478fe0() { STUB_BODY(); return 0; }
// !FUNC 0x00478fe0 END

// !FUNC 0x00478fe8 BEGIN
/* 478FE8-478FF0 00008 */
uchar _Globals::Unwind_00478fe8() { STUB_BODY(); return 0; }
// !FUNC 0x00478fe8 END

// !FUNC 0x00478ff0 BEGIN
/* 478FF0-478FFB 0000B */
uchar _Globals::Unwind_00478ff0() { STUB_BODY(); return 0; }
// !FUNC 0x00478ff0 END

// !FUNC 0x00478ffb BEGIN
/* 478FFB-479003 00008 */
uchar _Globals::Unwind_00478ffb() { STUB_BODY(); return 0; }
// !FUNC 0x00478ffb END

// !FUNC 0x00479040 BEGIN
/* 479040-479048 00008 */
uchar _Globals::Unwind_00479040() { STUB_BODY(); return 0; }
// !FUNC 0x00479040 END

// !FUNC 0x00479070 BEGIN
/* 479070-479078 00008 */
uchar _Globals::Unwind_00479070() { STUB_BODY(); return 0; }
// !FUNC 0x00479070 END

// !FUNC 0x00479078 BEGIN
/* 479078-479080 00008 */
uchar _Globals::Unwind_00479078() { STUB_BODY(); return 0; }
// !FUNC 0x00479078 END

// !FUNC 0x00479080 BEGIN
/* 479080-479088 00008 */
uchar _Globals::Unwind_00479080() { STUB_BODY(); return 0; }
// !FUNC 0x00479080 END

// !FUNC 0x004790b0 BEGIN
/* 4790B0-4790B8 00008 */
uchar _Globals::Unwind_004790b0() { STUB_BODY(); return 0; }
// !FUNC 0x004790b0 END

// !FUNC 0x004790b8 BEGIN
/* 4790B8-4790C3 0000B */
uchar _Globals::Unwind_004790b8() { STUB_BODY(); return 0; }
// !FUNC 0x004790b8 END

// !FUNC 0x004790e0 BEGIN
/* 4790E0-4790E8 00008 */
uchar _Globals::Unwind_004790e0() { STUB_BODY(); return 0; }
// !FUNC 0x004790e0 END

// !FUNC 0x004790e8 BEGIN
/* 4790E8-47910F 00027 */
uchar _Globals::Unwind_004790e8() { STUB_BODY(); return 0; }
// !FUNC 0x004790e8 END

// !FUNC 0x00479130 BEGIN
/* 479130-479138 00008 */
uchar _Globals::Unwind_00479130() { STUB_BODY(); return 0; }
// !FUNC 0x00479130 END

// !FUNC 0x00479138 BEGIN
/* 479138-479143 0000B */
uchar _Globals::Unwind_00479138() { STUB_BODY(); return 0; }
// !FUNC 0x00479138 END

// !FUNC 0x00479143 BEGIN
/* 479143-47914E 0000B */
uchar _Globals::Unwind_00479143() { STUB_BODY(); return 0; }
// !FUNC 0x00479143 END

// !FUNC 0x0047914e BEGIN
/* 47914E-479159 0000B */
uchar _Globals::Unwind_0047914e() { STUB_BODY(); return 0; }
// !FUNC 0x0047914e END

// !FUNC 0x00479159 BEGIN
/* 479159-479164 0000B */
uchar _Globals::Unwind_00479159() { STUB_BODY(); return 0; }
// !FUNC 0x00479159 END

// !FUNC 0x00479164 BEGIN
/* 479164-479172 0000E */
uchar _Globals::Unwind_00479164() { STUB_BODY(); return 0; }
// !FUNC 0x00479164 END

// !FUNC 0x00479172 BEGIN
/* 479172-479180 0000E */
uchar _Globals::Unwind_00479172() { STUB_BODY(); return 0; }
// !FUNC 0x00479172 END

// !FUNC 0x004791a0 BEGIN
/* 4791A0-4791A8 00008 */
uchar _Globals::Unwind_004791a0() { STUB_BODY(); return 0; }
// !FUNC 0x004791a0 END

// !FUNC 0x004791a8 BEGIN
/* 4791A8-4791B3 0000B */
uchar _Globals::Unwind_004791a8() { STUB_BODY(); return 0; }
// !FUNC 0x004791a8 END

// !FUNC 0x004791b3 BEGIN
/* 4791B3-4791BE 0000B */
uchar _Globals::Unwind_004791b3() { STUB_BODY(); return 0; }
// !FUNC 0x004791b3 END

// !FUNC 0x004791be BEGIN
/* 4791BE-4791C9 0000B */
uchar _Globals::Unwind_004791be() { STUB_BODY(); return 0; }
// !FUNC 0x004791be END

// !FUNC 0x004791c9 BEGIN
/* 4791C9-4791D4 0000B */
uchar _Globals::Unwind_004791c9() { STUB_BODY(); return 0; }
// !FUNC 0x004791c9 END

// !FUNC 0x004791d4 BEGIN
/* 4791D4-4791E2 0000E */
uchar _Globals::Unwind_004791d4() { STUB_BODY(); return 0; }
// !FUNC 0x004791d4 END

// !FUNC 0x004791e2 BEGIN
/* 4791E2-4791F0 0000E */
uchar _Globals::Unwind_004791e2() { STUB_BODY(); return 0; }
// !FUNC 0x004791e2 END

// !FUNC 0x004791f0 BEGIN
/* 4791F0-4791FE 0000E */
uchar _Globals::Unwind_004791f0() { STUB_BODY(); return 0; }
// !FUNC 0x004791f0 END

// !FUNC 0x004791fe BEGIN
/* 4791FE-47920C 0000E */
uchar _Globals::Unwind_004791fe() { STUB_BODY(); return 0; }
// !FUNC 0x004791fe END

// !FUNC 0x0047920c BEGIN
/* 47920C-47921A 0000E */
uchar _Globals::Unwind_0047920c() { STUB_BODY(); return 0; }
// !FUNC 0x0047920c END

// !FUNC 0x0047921a BEGIN
/* 47921A-479222 00008 */
uchar _Globals::Unwind_0047921a() { STUB_BODY(); return 0; }
// !FUNC 0x0047921a END

// !FUNC 0x00479240 BEGIN
/* 479240-479248 00008 */
uchar _Globals::Unwind_00479240() { STUB_BODY(); return 0; }
// !FUNC 0x00479240 END

// !FUNC 0x00479248 BEGIN
/* 479248-479253 0000B */
uchar _Globals::Unwind_00479248() { STUB_BODY(); return 0; }
// !FUNC 0x00479248 END

// !FUNC 0x00479253 BEGIN
/* 479253-47925E 0000B */
uchar _Globals::Unwind_00479253() { STUB_BODY(); return 0; }
// !FUNC 0x00479253 END

// !FUNC 0x0047925e BEGIN
/* 47925E-479269 0000B */
uchar _Globals::Unwind_0047925e() { STUB_BODY(); return 0; }
// !FUNC 0x0047925e END

// !FUNC 0x00479269 BEGIN
/* 479269-479274 0000B */
uchar _Globals::Unwind_00479269() { STUB_BODY(); return 0; }
// !FUNC 0x00479269 END

// !FUNC 0x00479274 BEGIN
/* 479274-479282 0000E */
uchar _Globals::Unwind_00479274() { STUB_BODY(); return 0; }
// !FUNC 0x00479274 END

// !FUNC 0x00479282 BEGIN
/* 479282-479290 0000E */
uchar _Globals::Unwind_00479282() { STUB_BODY(); return 0; }
// !FUNC 0x00479282 END

// !FUNC 0x00479290 BEGIN
/* 479290-47929E 0000E */
uchar _Globals::Unwind_00479290() { STUB_BODY(); return 0; }
// !FUNC 0x00479290 END

// !FUNC 0x0047929e BEGIN
/* 47929E-4792AC 0000E */
uchar _Globals::Unwind_0047929e() { STUB_BODY(); return 0; }
// !FUNC 0x0047929e END

// !FUNC 0x004792ac BEGIN
/* 4792AC-4792BA 0000E */
uchar _Globals::Unwind_004792ac() { STUB_BODY(); return 0; }
// !FUNC 0x004792ac END

// !FUNC 0x004792ba BEGIN
/* 4792BA-4792C2 00008 */
uchar _Globals::Unwind_004792ba() { STUB_BODY(); return 0; }
// !FUNC 0x004792ba END

// !FUNC 0x004792e0 BEGIN
/* 4792E0-4792EB 0000B */
uchar _Globals::Unwind_004792e0() { STUB_BODY(); return 0; }
// !FUNC 0x004792e0 END

// !FUNC 0x00479310 BEGIN
/* 479310-47931B 0000B */
uchar _Globals::Unwind_00479310() { STUB_BODY(); return 0; }
// !FUNC 0x00479310 END

// !FUNC 0x00479340 BEGIN
/* 479340-479348 00008 */
uchar _Globals::Unwind_00479340() { STUB_BODY(); return 0; }
// !FUNC 0x00479340 END

// !FUNC 0x00479348 BEGIN
/* 479348-47936F 00027 */
uchar _Globals::Unwind_00479348() { STUB_BODY(); return 0; }
// !FUNC 0x00479348 END

// !FUNC 0x0047936f BEGIN
/* 47936F-47937A 0000B */
uchar _Globals::Unwind_0047936f() { STUB_BODY(); return 0; }
// !FUNC 0x0047936f END

// !FUNC 0x004793a0 BEGIN
/* 4793A0-4793A8 00008 */
uchar _Globals::Unwind_004793a0() { STUB_BODY(); return 0; }
// !FUNC 0x004793a0 END

// !FUNC 0x004793d0 BEGIN
/* 4793D0-4793D8 00008 */
uchar _Globals::Unwind_004793d0() { STUB_BODY(); return 0; }
// !FUNC 0x004793d0 END

// !FUNC 0x00479400 BEGIN
/* 479400-479422 00022 */
uchar _Globals::Unwind_00479400() { STUB_BODY(); return 0; }
// !FUNC 0x00479400 END

// !FUNC 0x00479450 BEGIN
/* 479450-479458 00008 */
uchar _Globals::Unwind_00479450() { STUB_BODY(); return 0; }
// !FUNC 0x00479450 END

// !FUNC 0x00479458 BEGIN
/* 479458-479460 00008 */
uchar _Globals::Unwind_00479458() { STUB_BODY(); return 0; }
// !FUNC 0x00479458 END

// !FUNC 0x00479460 BEGIN
/* 479460-479468 00008 */
uchar _Globals::Unwind_00479460() { STUB_BODY(); return 0; }
// !FUNC 0x00479460 END

// !FUNC 0x00479468 BEGIN
/* 479468-479470 00008 */
uchar _Globals::Unwind_00479468() { STUB_BODY(); return 0; }
// !FUNC 0x00479468 END

// !FUNC 0x00479470 BEGIN
/* 479470-479478 00008 */
uchar _Globals::Unwind_00479470() { STUB_BODY(); return 0; }
// !FUNC 0x00479470 END

// !FUNC 0x00479478 BEGIN
/* 479478-479480 00008 */
uchar _Globals::Unwind_00479478() { STUB_BODY(); return 0; }
// !FUNC 0x00479478 END

// !FUNC 0x004794a0 BEGIN
/* 4794A0-4794A8 00008 */
uchar _Globals::Unwind_004794a0() { STUB_BODY(); return 0; }
// !FUNC 0x004794a0 END

// !FUNC 0x004794a8 BEGIN
/* 4794A8-4794B0 00008 */
uchar _Globals::Unwind_004794a8() { STUB_BODY(); return 0; }
// !FUNC 0x004794a8 END

// !FUNC 0x004794d0 BEGIN
/* 4794D0-4794D8 00008 */
uchar _Globals::Unwind_004794d0() { STUB_BODY(); return 0; }
// !FUNC 0x004794d0 END

// !FUNC 0x004794d8 BEGIN
/* 4794D8-4794E0 00008 */
uchar _Globals::Unwind_004794d8() { STUB_BODY(); return 0; }
// !FUNC 0x004794d8 END

// !FUNC 0x00479500 BEGIN
/* 479500-479519 00019 */
uchar _Globals::Unwind_00479500() { STUB_BODY(); return 0; }
// !FUNC 0x00479500 END

// !FUNC 0x00479540 BEGIN
/* 479540-479548 00008 */
uchar _Globals::Unwind_00479540() { STUB_BODY(); return 0; }
// !FUNC 0x00479540 END

// !FUNC 0x00479570 BEGIN
/* 479570-479578 00008 */
uchar _Globals::Unwind_00479570() { STUB_BODY(); return 0; }
// !FUNC 0x00479570 END

// !FUNC 0x00479578 BEGIN
/* 479578-479580 00008 */
uchar _Globals::Unwind_00479578() { STUB_BODY(); return 0; }
// !FUNC 0x00479578 END

// !FUNC 0x004795a0 BEGIN
/* 4795A0-4795AB 0000B */
uchar _Globals::Unwind_004795a0() { STUB_BODY(); return 0; }
// !FUNC 0x004795a0 END

// !FUNC 0x004795ab BEGIN
/* 4795AB-4795B6 0000B */
uchar _Globals::Unwind_004795ab() { STUB_BODY(); return 0; }
// !FUNC 0x004795ab END

// !FUNC 0x004795e0 BEGIN
/* 4795E0-4795EB 0000B */
uchar _Globals::Unwind_004795e0() { STUB_BODY(); return 0; }
// !FUNC 0x004795e0 END

// !FUNC 0x00479610 BEGIN
/* 479610-47961B 0000B */
uchar _Globals::Unwind_00479610() { STUB_BODY(); return 0; }
// !FUNC 0x00479610 END

// !FUNC 0x0047961b BEGIN
/* 47961B-479626 0000B */
uchar _Globals::Unwind_0047961b() { STUB_BODY(); return 0; }
// !FUNC 0x0047961b END

// !FUNC 0x00479650 BEGIN
/* 479650-479658 00008 */
uchar _Globals::Unwind_00479650() { STUB_BODY(); return 0; }
// !FUNC 0x00479650 END

// !FUNC 0x00479680 BEGIN
/* 479680-47968B 0000B */
uchar _Globals::Unwind_00479680() { STUB_BODY(); return 0; }
// !FUNC 0x00479680 END

// !FUNC 0x0047968b BEGIN
/* 47968B-479696 0000B */
uchar _Globals::Unwind_0047968b() { STUB_BODY(); return 0; }
// !FUNC 0x0047968b END

// !FUNC 0x004796c0 BEGIN
/* 4796C0-4796CB 0000B */
uchar _Globals::Unwind_004796c0() { STUB_BODY(); return 0; }
// !FUNC 0x004796c0 END

// !FUNC 0x004796cb BEGIN
/* 4796CB-4796D6 0000B */
uchar _Globals::Unwind_004796cb() { STUB_BODY(); return 0; }
// !FUNC 0x004796cb END

// !FUNC 0x00479720 BEGIN
/* 479720-479728 00008 */
uchar _Globals::Unwind_00479720() { STUB_BODY(); return 0; }
// !FUNC 0x00479720 END

// !FUNC 0x00479770 BEGIN
/* 479770-479778 00008 */
uchar _Globals::Unwind_00479770() { STUB_BODY(); return 0; }
// !FUNC 0x00479770 END

// !FUNC 0x00479778 BEGIN
/* 479778-479783 0000B */
uchar _Globals::Unwind_00479778() { STUB_BODY(); return 0; }
// !FUNC 0x00479778 END

// !FUNC 0x00479783 BEGIN
/* 479783-47978E 0000B */
uchar _Globals::Unwind_00479783() { STUB_BODY(); return 0; }
// !FUNC 0x00479783 END

// !FUNC 0x004797b0 BEGIN
/* 4797B0-4797B8 00008 */
uchar _Globals::Unwind_004797b0() { STUB_BODY(); return 0; }
// !FUNC 0x004797b0 END

// !FUNC 0x004797b8 BEGIN
/* 4797B8-4797C3 0000B */
uchar _Globals::Unwind_004797b8() { STUB_BODY(); return 0; }
// !FUNC 0x004797b8 END

// !FUNC 0x004797c3 BEGIN
/* 4797C3-4797CE 0000B */
uchar _Globals::Unwind_004797c3() { STUB_BODY(); return 0; }
// !FUNC 0x004797c3 END

// !FUNC 0x004797ce BEGIN
/* 4797CE-4797D9 0000B */
uchar _Globals::Unwind_004797ce() { STUB_BODY(); return 0; }
// !FUNC 0x004797ce END

// !FUNC 0x004797d9 BEGIN
/* 4797D9-4797E1 00008 */
uchar _Globals::Unwind_004797d9() { STUB_BODY(); return 0; }
// !FUNC 0x004797d9 END

// !FUNC 0x00479800 BEGIN
/* 479800-479808 00008 */
uchar _Globals::Unwind_00479800() { STUB_BODY(); return 0; }
// !FUNC 0x00479800 END

// !FUNC 0x00479808 BEGIN
/* 479808-479813 0000B */
uchar _Globals::Unwind_00479808() { STUB_BODY(); return 0; }
// !FUNC 0x00479808 END

// !FUNC 0x00479813 BEGIN
/* 479813-47981E 0000B */
uchar _Globals::Unwind_00479813() { STUB_BODY(); return 0; }
// !FUNC 0x00479813 END

// !FUNC 0x0047981e BEGIN
/* 47981E-479829 0000B */
uchar _Globals::Unwind_0047981e() { STUB_BODY(); return 0; }
// !FUNC 0x0047981e END

// !FUNC 0x00479829 BEGIN
/* 479829-479831 00008 */
uchar _Globals::Unwind_00479829() { STUB_BODY(); return 0; }
// !FUNC 0x00479829 END

// !FUNC 0x00479850 BEGIN
/* 479850-47985B 0000B */
uchar _Globals::Unwind_00479850() { STUB_BODY(); return 0; }
// !FUNC 0x00479850 END

// !FUNC 0x00479880 BEGIN
/* 479880-47988B 0000B */
uchar _Globals::Unwind_00479880() { STUB_BODY(); return 0; }
// !FUNC 0x00479880 END

// !FUNC 0x004798b0 BEGIN
/* 4798B0-4798B8 00008 */
uchar _Globals::Unwind_004798b0() { STUB_BODY(); return 0; }
// !FUNC 0x004798b0 END

// !FUNC 0x004798e0 BEGIN
/* 4798E0-4798E8 00008 */
uchar _Globals::Unwind_004798e0() { STUB_BODY(); return 0; }
// !FUNC 0x004798e0 END

// !FUNC 0x00479910 BEGIN
/* 479910-47991B 0000B */
uchar _Globals::Unwind_00479910() { STUB_BODY(); return 0; }
// !FUNC 0x00479910 END

// !FUNC 0x00479940 BEGIN
/* 479940-479948 00008 */
uchar _Globals::Unwind_00479940() { STUB_BODY(); return 0; }
// !FUNC 0x00479940 END

// !FUNC 0x00479948 BEGIN
/* 479948-479953 0000B */
uchar _Globals::Unwind_00479948() { STUB_BODY(); return 0; }
// !FUNC 0x00479948 END

// !FUNC 0x00479970 BEGIN
/* 479970-479978 00008 */
uchar _Globals::Unwind_00479970() { STUB_BODY(); return 0; }
// !FUNC 0x00479970 END

// !FUNC 0x00479978 BEGIN
/* 479978-479983 0000B */
uchar _Globals::Unwind_00479978() { STUB_BODY(); return 0; }
// !FUNC 0x00479978 END

// !FUNC 0x004799a0 BEGIN
/* 4799A0-4799AB 0000B */
uchar _Globals::Unwind_004799a0() { STUB_BODY(); return 0; }
// !FUNC 0x004799a0 END

// !FUNC 0x004799ab BEGIN
/* 4799AB-4799B3 00008 */
uchar _Globals::Unwind_004799ab() { STUB_BODY(); return 0; }
// !FUNC 0x004799ab END

// !FUNC 0x004799d0 BEGIN
/* 4799D0-4799D8 00008 */
uchar _Globals::Unwind_004799d0() { STUB_BODY(); return 0; }
// !FUNC 0x004799d0 END

// !FUNC 0x004799d8 BEGIN
/* 4799D8-4799E3 0000B */
uchar _Globals::Unwind_004799d8() { STUB_BODY(); return 0; }
// !FUNC 0x004799d8 END

// !FUNC 0x00479a00 BEGIN
/* 479A00-479A08 00008 */
uchar _Globals::Unwind_00479a00() { STUB_BODY(); return 0; }
// !FUNC 0x00479a00 END

// !FUNC 0x00479ac0 BEGIN
/* 479AC0-479AC8 00008 */
uchar _Globals::Unwind_00479ac0() { STUB_BODY(); return 0; }
// !FUNC 0x00479ac0 END

// !FUNC 0x00479ac8 BEGIN
/* 479AC8-479AEF 00027 */
uchar _Globals::Unwind_00479ac8() { STUB_BODY(); return 0; }
// !FUNC 0x00479ac8 END

// !FUNC 0x00479b10 BEGIN
/* 479B10-479B1B 0000B */
uchar _Globals::Unwind_00479b10() { STUB_BODY(); return 0; }
// !FUNC 0x00479b10 END

// !FUNC 0x00479b50 BEGIN
/* 479B50-479B58 00008 */
uchar _Globals::Unwind_00479b50() { STUB_BODY(); return 0; }
// !FUNC 0x00479b50 END

// !FUNC 0x00479b80 BEGIN
/* 479B80-479B88 00008 */
uchar _Globals::Unwind_00479b80() { STUB_BODY(); return 0; }
// !FUNC 0x00479b80 END

// !FUNC 0x00479b88 BEGIN
/* 479B88-479BAF 00027 */
uchar _Globals::Unwind_00479b88() { STUB_BODY(); return 0; }
// !FUNC 0x00479b88 END

// !FUNC 0x00479bd0 BEGIN
/* 479BD0-479BD8 00008 */
uchar _Globals::Unwind_00479bd0() { STUB_BODY(); return 0; }
// !FUNC 0x00479bd0 END

// !FUNC 0x00479bd8 BEGIN
/* 479BD8-479BE3 0000B */
uchar _Globals::Unwind_00479bd8() { STUB_BODY(); return 0; }
// !FUNC 0x00479bd8 END

// !FUNC 0x00479be3 BEGIN
/* 479BE3-479BEB 00008 */
uchar _Globals::Unwind_00479be3() { STUB_BODY(); return 0; }
// !FUNC 0x00479be3 END

// !FUNC 0x00479c10 BEGIN
/* 479C10-479C18 00008 */
uchar _Globals::Unwind_00479c10() { STUB_BODY(); return 0; }
// !FUNC 0x00479c10 END

// !FUNC 0x00479c18 BEGIN
/* 479C18-479C3F 00027 */
uchar _Globals::Unwind_00479c18() { STUB_BODY(); return 0; }
// !FUNC 0x00479c18 END

// !FUNC 0x00479c3f BEGIN
/* 479C3F-479C4A 0000B */
uchar _Globals::Unwind_00479c3f() { STUB_BODY(); return 0; }
// !FUNC 0x00479c3f END

// !FUNC 0x00479c70 BEGIN
/* 479C70-479C7B 0000B */
uchar _Globals::Unwind_00479c70() { STUB_BODY(); return 0; }
// !FUNC 0x00479c70 END

// !FUNC 0x00479ca0 BEGIN
/* 479CA0-479CA8 00008 */
uchar _Globals::Unwind_00479ca0() { STUB_BODY(); return 0; }
// !FUNC 0x00479ca0 END

// !FUNC 0x00479ca8 BEGIN
/* 479CA8-479CCF 00027 */
uchar _Globals::Unwind_00479ca8() { STUB_BODY(); return 0; }
// !FUNC 0x00479ca8 END

// !FUNC 0x00479cf0 BEGIN
/* 479CF0-479CF8 00008 */
uchar _Globals::Unwind_00479cf0() { STUB_BODY(); return 0; }
// !FUNC 0x00479cf0 END

// !FUNC 0x00479d20 BEGIN
/* 479D20-479D28 00008 */
uchar _Globals::Unwind_00479d20() { STUB_BODY(); return 0; }
// !FUNC 0x00479d20 END

// !FUNC 0x00479d28 BEGIN
/* 479D28-479D30 00008 */
uchar _Globals::Unwind_00479d28() { STUB_BODY(); return 0; }
// !FUNC 0x00479d28 END

// !FUNC 0x00479d50 BEGIN
/* 479D50-479D58 00008 */
uchar _Globals::Unwind_00479d50() { STUB_BODY(); return 0; }
// !FUNC 0x00479d50 END

// !FUNC 0x00479d80 BEGIN
/* 479D80-479D8B 0000B */
uchar _Globals::Unwind_00479d80() { STUB_BODY(); return 0; }
// !FUNC 0x00479d80 END

// !FUNC 0x00479db0 BEGIN
/* 479DB0-479DB8 00008 */
uchar _Globals::Unwind_00479db0() { STUB_BODY(); return 0; }
// !FUNC 0x00479db0 END

// !FUNC 0x00479de0 BEGIN
/* 479DE0-479DE8 00008 */
uchar _Globals::Unwind_00479de0() { STUB_BODY(); return 0; }
// !FUNC 0x00479de0 END

// !FUNC 0x00479de8 BEGIN
/* 479DE8-479DF3 0000B */
uchar _Globals::Unwind_00479de8() { STUB_BODY(); return 0; }
// !FUNC 0x00479de8 END

// !FUNC 0x00479df3 BEGIN
/* 479DF3-479DFE 0000B */
uchar _Globals::Unwind_00479df3() { STUB_BODY(); return 0; }
// !FUNC 0x00479df3 END

// !FUNC 0x00479dfe BEGIN
/* 479DFE-479E09 0000B */
uchar _Globals::Unwind_00479dfe() { STUB_BODY(); return 0; }
// !FUNC 0x00479dfe END

// !FUNC 0x00479e30 BEGIN
/* 479E30-479E38 00008 */
uchar _Globals::Unwind_00479e30() { STUB_BODY(); return 0; }
// !FUNC 0x00479e30 END

// !FUNC 0x00479e38 BEGIN
/* 479E38-479E43 0000B */
uchar _Globals::Unwind_00479e38() { STUB_BODY(); return 0; }
// !FUNC 0x00479e38 END

// !FUNC 0x00479e43 BEGIN
/* 479E43-479E4E 0000B */
uchar _Globals::Unwind_00479e43() { STUB_BODY(); return 0; }
// !FUNC 0x00479e43 END

// !FUNC 0x00479e4e BEGIN
/* 479E4E-479E59 0000B */
uchar _Globals::Unwind_00479e4e() { STUB_BODY(); return 0; }
// !FUNC 0x00479e4e END

// !FUNC 0x00479e80 BEGIN
/* 479E80-479E88 00008 */
uchar _Globals::Unwind_00479e80() { STUB_BODY(); return 0; }
// !FUNC 0x00479e80 END

// !FUNC 0x00479eb0 BEGIN
/* 479EB0-479EB8 00008 */
uchar _Globals::Unwind_00479eb0() { STUB_BODY(); return 0; }
// !FUNC 0x00479eb0 END

// !FUNC 0x00479eb8 BEGIN
/* 479EB8-479EC3 0000B */
uchar _Globals::Unwind_00479eb8() { STUB_BODY(); return 0; }
// !FUNC 0x00479eb8 END

// !FUNC 0x00479ec3 BEGIN
/* 479EC3-479ECE 0000B */
uchar _Globals::Unwind_00479ec3() { STUB_BODY(); return 0; }
// !FUNC 0x00479ec3 END

// !FUNC 0x00479ece BEGIN
/* 479ECE-479ED9 0000B */
uchar _Globals::Unwind_00479ece() { STUB_BODY(); return 0; }
// !FUNC 0x00479ece END

// !FUNC 0x00479ed9 BEGIN
/* 479ED9-479EE4 0000B */
uchar _Globals::Unwind_00479ed9() { STUB_BODY(); return 0; }
// !FUNC 0x00479ed9 END

// !FUNC 0x00479f00 BEGIN
/* 479F00-479F08 00008 */
uchar _Globals::Unwind_00479f00() { STUB_BODY(); return 0; }
// !FUNC 0x00479f00 END

// !FUNC 0x00479f30 BEGIN
/* 479F30-479F3B 0000B */
uchar _Globals::Unwind_00479f30() { STUB_BODY(); return 0; }
// !FUNC 0x00479f30 END

// !FUNC 0x00479f60 BEGIN
/* 479F60-479F68 00008 */
uchar _Globals::Unwind_00479f60() { STUB_BODY(); return 0; }
// !FUNC 0x00479f60 END

// !FUNC 0x00479f90 BEGIN
/* 479F90-479F9B 0000B */
uchar _Globals::Unwind_00479f90() { STUB_BODY(); return 0; }
// !FUNC 0x00479f90 END

// !FUNC 0x00479fc0 BEGIN
/* 479FC0-479FC8 00008 */
uchar _Globals::Unwind_00479fc0() { STUB_BODY(); return 0; }
// !FUNC 0x00479fc0 END

// !FUNC 0x00479fc8 BEGIN
/* 479FC8-479FD3 0000B */
uchar _Globals::Unwind_00479fc8() { STUB_BODY(); return 0; }
// !FUNC 0x00479fc8 END

// !FUNC 0x00479ff0 BEGIN
/* 479FF0-479FF8 00008 */
uchar _Globals::Unwind_00479ff0() { STUB_BODY(); return 0; }
// !FUNC 0x00479ff0 END

// !FUNC 0x00479ff8 BEGIN
/* 479FF8-47A01F 00027 */
uchar _Globals::Unwind_00479ff8() { STUB_BODY(); return 0; }
// !FUNC 0x00479ff8 END

// !FUNC 0x0047a01f BEGIN
/* 47A01F-47A02A 0000B */
uchar _Globals::Unwind_0047a01f() { STUB_BODY(); return 0; }
// !FUNC 0x0047a01f END

// !FUNC 0x0047a050 BEGIN
/* 47A050-47A05B 0000B */
uchar _Globals::Unwind_0047a050() { STUB_BODY(); return 0; }
// !FUNC 0x0047a050 END

// !FUNC 0x0047a080 BEGIN
/* 47A080-47A08B 0000B */
uchar _Globals::Unwind_0047a080() { STUB_BODY(); return 0; }
// !FUNC 0x0047a080 END

// !FUNC 0x0047a08b BEGIN
/* 47A08B-47A093 00008 */
uchar _Globals::Unwind_0047a08b() { STUB_BODY(); return 0; }
// !FUNC 0x0047a08b END

// !FUNC 0x0047a093 BEGIN
/* 47A093-47A09E 0000B */
uchar _Globals::Unwind_0047a093() { STUB_BODY(); return 0; }
// !FUNC 0x0047a093 END

// !FUNC 0x0047a09e BEGIN
/* 47A09E-47A0A6 00008 */
uchar _Globals::Unwind_0047a09e() { STUB_BODY(); return 0; }
// !FUNC 0x0047a09e END

// !FUNC 0x0047a0d0 BEGIN
/* 47A0D0-47A0DB 0000B */
uchar _Globals::Unwind_0047a0d0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a0d0 END

// !FUNC 0x0047a100 BEGIN
/* 47A100-47A108 00008 */
uchar _Globals::Unwind_0047a100() { STUB_BODY(); return 0; }
// !FUNC 0x0047a100 END

// !FUNC 0x0047a130 BEGIN
/* 47A130-47A138 00008 */
uchar _Globals::Unwind_0047a130() { STUB_BODY(); return 0; }
// !FUNC 0x0047a130 END

// !FUNC 0x0047a160 BEGIN
/* 47A160-47A168 00008 */
uchar _Globals::Unwind_0047a160() { STUB_BODY(); return 0; }
// !FUNC 0x0047a160 END

// !FUNC 0x0047a190 BEGIN
/* 47A190-47A198 00008 */
uchar _Globals::Unwind_0047a190() { STUB_BODY(); return 0; }
// !FUNC 0x0047a190 END

// !FUNC 0x0047a1c0 BEGIN
/* 47A1C0-47A1C8 00008 */
uchar _Globals::Unwind_0047a1c0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a1c0 END

// !FUNC 0x0047a1f0 BEGIN
/* 47A1F0-47A1F8 00008 */
uchar _Globals::Unwind_0047a1f0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a1f0 END

// !FUNC 0x0047a1f8 BEGIN
/* 47A1F8-47A203 0000B */
uchar _Globals::Unwind_0047a1f8() { STUB_BODY(); return 0; }
// !FUNC 0x0047a1f8 END

// !FUNC 0x0047a203 BEGIN
/* 47A203-47A20E 0000B */
uchar _Globals::Unwind_0047a203() { STUB_BODY(); return 0; }
// !FUNC 0x0047a203 END

// !FUNC 0x0047a20e BEGIN
/* 47A20E-47A219 0000B */
uchar _Globals::Unwind_0047a20e() { STUB_BODY(); return 0; }
// !FUNC 0x0047a20e END

// !FUNC 0x0047a219 BEGIN
/* 47A219-47A224 0000B */
uchar _Globals::Unwind_0047a219() { STUB_BODY(); return 0; }
// !FUNC 0x0047a219 END

// !FUNC 0x0047a240 BEGIN
/* 47A240-47A248 00008 */
uchar _Globals::Unwind_0047a240() { STUB_BODY(); return 0; }
// !FUNC 0x0047a240 END

// !FUNC 0x0047a248 BEGIN
/* 47A248-47A253 0000B */
uchar _Globals::Unwind_0047a248() { STUB_BODY(); return 0; }
// !FUNC 0x0047a248 END

// !FUNC 0x0047a253 BEGIN
/* 47A253-47A25E 0000B */
uchar _Globals::Unwind_0047a253() { STUB_BODY(); return 0; }
// !FUNC 0x0047a253 END

// !FUNC 0x0047a25e BEGIN
/* 47A25E-47A269 0000B */
uchar _Globals::Unwind_0047a25e() { STUB_BODY(); return 0; }
// !FUNC 0x0047a25e END

// !FUNC 0x0047a269 BEGIN
/* 47A269-47A274 0000B */
uchar _Globals::Unwind_0047a269() { STUB_BODY(); return 0; }
// !FUNC 0x0047a269 END

// !FUNC 0x0047a290 BEGIN
/* 47A290-47A29B 0000B */
uchar _Globals::Unwind_0047a290() { STUB_BODY(); return 0; }
// !FUNC 0x0047a290 END

// !FUNC 0x0047a2c0 BEGIN
/* 47A2C0-47A2C8 00008 */
uchar _Globals::Unwind_0047a2c0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a2c0 END

// !FUNC 0x0047a2f0 BEGIN
/* 47A2F0-47A2FB 0000B */
uchar _Globals::Unwind_0047a2f0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a2f0 END

// !FUNC 0x0047a330 BEGIN
/* 47A330-47A338 00008 */
uchar _Globals::Unwind_0047a330() { STUB_BODY(); return 0; }
// !FUNC 0x0047a330 END

// !FUNC 0x0047a338 BEGIN
/* 47A338-47A343 0000B */
uchar _Globals::Unwind_0047a338() { STUB_BODY(); return 0; }
// !FUNC 0x0047a338 END

// !FUNC 0x0047a360 BEGIN
/* 47A360-47A368 00008 */
uchar _Globals::Unwind_0047a360() { STUB_BODY(); return 0; }
// !FUNC 0x0047a360 END

// !FUNC 0x0047a368 BEGIN
/* 47A368-47A370 00008 */
uchar _Globals::Unwind_0047a368() { STUB_BODY(); return 0; }
// !FUNC 0x0047a368 END

// !FUNC 0x0047a370 BEGIN
/* 47A370-47A37B 0000B */
uchar _Globals::Unwind_0047a370() { STUB_BODY(); return 0; }
// !FUNC 0x0047a370 END

// !FUNC 0x0047a37b BEGIN
/* 47A37B-47A386 0000B */
uchar _Globals::Unwind_0047a37b() { STUB_BODY(); return 0; }
// !FUNC 0x0047a37b END

// !FUNC 0x0047a3b0 BEGIN
/* 47A3B0-47A3B8 00008 */
uchar _Globals::Unwind_0047a3b0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a3b0 END

// !FUNC 0x0047a3e0 BEGIN
/* 47A3E0-47A3E8 00008 */
uchar _Globals::Unwind_0047a3e0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a3e0 END

// !FUNC 0x0047a3e8 BEGIN
/* 47A3E8-47A3F3 0000B */
uchar _Globals::Unwind_0047a3e8() { STUB_BODY(); return 0; }
// !FUNC 0x0047a3e8 END

// !FUNC 0x0047a410 BEGIN
/* 47A410-47A418 00008 */
uchar _Globals::Unwind_0047a410() { STUB_BODY(); return 0; }
// !FUNC 0x0047a410 END

// !FUNC 0x0047a440 BEGIN
/* 47A440-47A448 00008 */
uchar _Globals::Unwind_0047a440() { STUB_BODY(); return 0; }
// !FUNC 0x0047a440 END

// !FUNC 0x0047a470 BEGIN
/* 47A470-47A478 00008 */
uchar _Globals::Unwind_0047a470() { STUB_BODY(); return 0; }
// !FUNC 0x0047a470 END

// !FUNC 0x0047a4a0 BEGIN
/* 47A4A0-47A4A8 00008 */
uchar _Globals::Unwind_0047a4a0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a4a0 END

// !FUNC 0x0047a4d0 BEGIN
/* 47A4D0-47A4DB 0000B */
uchar _Globals::Unwind_0047a4d0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a4d0 END

// !FUNC 0x0047a500 BEGIN
/* 47A500-47A50B 0000B */
uchar _Globals::Unwind_0047a500() { STUB_BODY(); return 0; }
// !FUNC 0x0047a500 END

// !FUNC 0x0047a540 BEGIN
/* 47A540-47A548 00008 */
uchar _Globals::Unwind_0047a540() { STUB_BODY(); return 0; }
// !FUNC 0x0047a540 END

// !FUNC 0x0047a548 BEGIN
/* 47A548-47A553 0000B */
uchar _Globals::Unwind_0047a548() { STUB_BODY(); return 0; }
// !FUNC 0x0047a548 END

// !FUNC 0x0047a570 BEGIN
/* 47A570-47A57B 0000B */
uchar _Globals::Unwind_0047a570() { STUB_BODY(); return 0; }
// !FUNC 0x0047a570 END

// !FUNC 0x0047a5d0 BEGIN
/* 47A5D0-47A5DB 0000B */
uchar _Globals::Unwind_0047a5d0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a5d0 END

// !FUNC 0x0047a600 BEGIN
/* 47A600-47A60B 0000B */
uchar _Globals::Unwind_0047a600() { STUB_BODY(); return 0; }
// !FUNC 0x0047a600 END

// !FUNC 0x0047a630 BEGIN
/* 47A630-47A638 00008 */
uchar _Globals::Unwind_0047a630() { STUB_BODY(); return 0; }
// !FUNC 0x0047a630 END

// !FUNC 0x0047a638 BEGIN
/* 47A638-47A643 0000B */
uchar _Globals::Unwind_0047a638() { STUB_BODY(); return 0; }
// !FUNC 0x0047a638 END

// !FUNC 0x0047a660 BEGIN
/* 47A660-47A668 00008 */
uchar _Globals::Unwind_0047a660() { STUB_BODY(); return 0; }
// !FUNC 0x0047a660 END

// !FUNC 0x0047a668 BEGIN
/* 47A668-47A673 0000B */
uchar _Globals::Unwind_0047a668() { STUB_BODY(); return 0; }
// !FUNC 0x0047a668 END

// !FUNC 0x0047a690 BEGIN
/* 47A690-47A698 00008 */
uchar _Globals::Unwind_0047a690() { STUB_BODY(); return 0; }
// !FUNC 0x0047a690 END

// !FUNC 0x0047a698 BEGIN
/* 47A698-47A6A3 0000B */
uchar _Globals::Unwind_0047a698() { STUB_BODY(); return 0; }
// !FUNC 0x0047a698 END

// !FUNC 0x0047a6c0 BEGIN
/* 47A6C0-47A6CB 0000B */
uchar _Globals::Unwind_0047a6c0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a6c0 END

// !FUNC 0x0047a6f0 BEGIN
/* 47A6F0-47A6FB 0000B */
uchar _Globals::Unwind_0047a6f0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a6f0 END

// !FUNC 0x0047a720 BEGIN
/* 47A720-47A72B 0000B */
uchar _Globals::Unwind_0047a720() { STUB_BODY(); return 0; }
// !FUNC 0x0047a720 END

// !FUNC 0x0047a750 BEGIN
/* 47A750-47A758 00008 */
uchar _Globals::Unwind_0047a750() { STUB_BODY(); return 0; }
// !FUNC 0x0047a750 END

// !FUNC 0x0047a758 BEGIN
/* 47A758-47A763 0000B */
uchar _Globals::Unwind_0047a758() { STUB_BODY(); return 0; }
// !FUNC 0x0047a758 END

// !FUNC 0x0047a763 BEGIN
/* 47A763-47A76E 0000B */
uchar _Globals::Unwind_0047a763() { STUB_BODY(); return 0; }
// !FUNC 0x0047a763 END

// !FUNC 0x0047a76e BEGIN
/* 47A76E-47A779 0000B */
uchar _Globals::Unwind_0047a76e() { STUB_BODY(); return 0; }
// !FUNC 0x0047a76e END

// !FUNC 0x0047a7a0 BEGIN
/* 47A7A0-47A7A8 00008 */
uchar _Globals::Unwind_0047a7a0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a7a0 END

// !FUNC 0x0047a7a8 BEGIN
/* 47A7A8-47A7CF 00027 */
uchar _Globals::Unwind_0047a7a8() { STUB_BODY(); return 0; }
// !FUNC 0x0047a7a8 END

// !FUNC 0x0047a7cf BEGIN
/* 47A7CF-47A7DA 0000B */
uchar _Globals::Unwind_0047a7cf() { STUB_BODY(); return 0; }
// !FUNC 0x0047a7cf END

// !FUNC 0x0047a7da BEGIN
/* 47A7DA-47A7E5 0000B */
uchar _Globals::Unwind_0047a7da() { STUB_BODY(); return 0; }
// !FUNC 0x0047a7da END

// !FUNC 0x0047a820 BEGIN
/* 47A820-47A82B 0000B */
uchar _Globals::Unwind_0047a820() { STUB_BODY(); return 0; }
// !FUNC 0x0047a820 END

// !FUNC 0x0047a870 BEGIN
/* 47A870-47A878 00008 */
uchar _Globals::Unwind_0047a870() { STUB_BODY(); return 0; }
// !FUNC 0x0047a870 END

// !FUNC 0x0047a878 BEGIN
/* 47A878-47A883 0000B */
uchar _Globals::Unwind_0047a878() { STUB_BODY(); return 0; }
// !FUNC 0x0047a878 END

// !FUNC 0x0047a883 BEGIN
/* 47A883-47A88B 00008 */
uchar _Globals::Unwind_0047a883() { STUB_BODY(); return 0; }
// !FUNC 0x0047a883 END

// !FUNC 0x0047a8b0 BEGIN
/* 47A8B0-47A8B8 00008 */
uchar _Globals::Unwind_0047a8b0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a8b0 END

// !FUNC 0x0047a900 BEGIN
/* 47A900-47A908 00008 */
uchar _Globals::Unwind_0047a900() { STUB_BODY(); return 0; }
// !FUNC 0x0047a900 END

// !FUNC 0x0047a908 BEGIN
/* 47A908-47A913 0000B */
uchar _Globals::Unwind_0047a908() { STUB_BODY(); return 0; }
// !FUNC 0x0047a908 END

// !FUNC 0x0047a913 BEGIN
/* 47A913-47A91E 0000B */
uchar _Globals::Unwind_0047a913() { STUB_BODY(); return 0; }
// !FUNC 0x0047a913 END

// !FUNC 0x0047a91e BEGIN
/* 47A91E-47A929 0000B */
uchar _Globals::Unwind_0047a91e() { STUB_BODY(); return 0; }
// !FUNC 0x0047a91e END

// !FUNC 0x0047a950 BEGIN
/* 47A950-47A958 00008 */
uchar _Globals::Unwind_0047a950() { STUB_BODY(); return 0; }
// !FUNC 0x0047a950 END

// !FUNC 0x0047a958 BEGIN
/* 47A958-47A97F 00027 */
uchar _Globals::Unwind_0047a958() { STUB_BODY(); return 0; }
// !FUNC 0x0047a958 END

// !FUNC 0x0047a97f BEGIN
/* 47A97F-47A98A 0000B */
uchar _Globals::Unwind_0047a97f() { STUB_BODY(); return 0; }
// !FUNC 0x0047a97f END

// !FUNC 0x0047a98a BEGIN
/* 47A98A-47A995 0000B */
uchar _Globals::Unwind_0047a98a() { STUB_BODY(); return 0; }
// !FUNC 0x0047a98a END

// !FUNC 0x0047a9b0 BEGIN
/* 47A9B0-47A9BB 0000B */
uchar _Globals::Unwind_0047a9b0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a9b0 END

// !FUNC 0x0047a9e0 BEGIN
/* 47A9E0-47A9E8 00008 */
uchar _Globals::Unwind_0047a9e0() { STUB_BODY(); return 0; }
// !FUNC 0x0047a9e0 END

// !FUNC 0x0047a9e8 BEGIN
/* 47A9E8-47A9F3 0000B */
uchar _Globals::Unwind_0047a9e8() { STUB_BODY(); return 0; }
// !FUNC 0x0047a9e8 END

// !FUNC 0x0047aa10 BEGIN
/* 47AA10-47AA18 00008 */
uchar _Globals::Unwind_0047aa10() { STUB_BODY(); return 0; }
// !FUNC 0x0047aa10 END

// !FUNC 0x0047aa18 BEGIN
/* 47AA18-47AA23 0000B */
uchar _Globals::Unwind_0047aa18() { STUB_BODY(); return 0; }
// !FUNC 0x0047aa18 END

// !FUNC 0x0047aa23 BEGIN
/* 47AA23-47AA2E 0000B */
uchar _Globals::Unwind_0047aa23() { STUB_BODY(); return 0; }
// !FUNC 0x0047aa23 END

// !FUNC 0x0047aa50 BEGIN
/* 47AA50-47AA5B 0000B */
uchar _Globals::Unwind_0047aa50() { STUB_BODY(); return 0; }
// !FUNC 0x0047aa50 END

// !FUNC 0x0047aa80 BEGIN
/* 47AA80-47AA88 00008 */
uchar _Globals::Unwind_0047aa80() { STUB_BODY(); return 0; }
// !FUNC 0x0047aa80 END

// !FUNC 0x0047aab0 BEGIN
/* 47AAB0-47AABB 0000B */
uchar _Globals::Unwind_0047aab0() { STUB_BODY(); return 0; }
// !FUNC 0x0047aab0 END

// !FUNC 0x0047aae0 BEGIN
/* 47AAE0-47AAEB 0000B */
uchar _Globals::Unwind_0047aae0() { STUB_BODY(); return 0; }
// !FUNC 0x0047aae0 END

// !FUNC 0x0047ab10 BEGIN
/* 47AB10-47AB18 00008 */
uchar _Globals::Unwind_0047ab10() { STUB_BODY(); return 0; }
// !FUNC 0x0047ab10 END

// !FUNC 0x0047ab18 BEGIN
/* 47AB18-47AB23 0000B */
uchar _Globals::Unwind_0047ab18() { STUB_BODY(); return 0; }
// !FUNC 0x0047ab18 END

// !FUNC 0x0047ab23 BEGIN
/* 47AB23-47AB2E 0000B */
uchar _Globals::Unwind_0047ab23() { STUB_BODY(); return 0; }
// !FUNC 0x0047ab23 END

// !FUNC 0x0047ab50 BEGIN
/* 47AB50-47AB58 00008 */
uchar _Globals::Unwind_0047ab50() { STUB_BODY(); return 0; }
// !FUNC 0x0047ab50 END

// !FUNC 0x0047ab58 BEGIN
/* 47AB58-47AB63 0000B */
uchar _Globals::Unwind_0047ab58() { STUB_BODY(); return 0; }
// !FUNC 0x0047ab58 END

// !FUNC 0x0047ab63 BEGIN
/* 47AB63-47AB6E 0000B */
uchar _Globals::Unwind_0047ab63() { STUB_BODY(); return 0; }
// !FUNC 0x0047ab63 END

// !FUNC 0x0047ab6e BEGIN
/* 47AB6E-47AB79 0000B */
uchar _Globals::Unwind_0047ab6e() { STUB_BODY(); return 0; }
// !FUNC 0x0047ab6e END

// !FUNC 0x0047aba0 BEGIN
/* 47ABA0-47ABAB 0000B */
uchar _Globals::Unwind_0047aba0() { STUB_BODY(); return 0; }
// !FUNC 0x0047aba0 END

// !FUNC 0x0047ac30 BEGIN
/* 47AC30-47AC38 00008 */
uchar _Globals::Unwind_0047ac30() { STUB_BODY(); return 0; }
// !FUNC 0x0047ac30 END

// !FUNC 0x0047ac38 BEGIN
/* 47AC38-47AC43 0000B */
uchar _Globals::Unwind_0047ac38() { STUB_BODY(); return 0; }
// !FUNC 0x0047ac38 END

// !FUNC 0x0047ac60 BEGIN
/* 47AC60-47AC68 00008 */
uchar _Globals::Unwind_0047ac60() { STUB_BODY(); return 0; }
// !FUNC 0x0047ac60 END

// !FUNC 0x0047acd0 BEGIN
/* 47ACD0-47ACD8 00008 */
uchar _Globals::Unwind_0047acd0() { STUB_BODY(); return 0; }
// !FUNC 0x0047acd0 END

// !FUNC 0x0047acd8 BEGIN
/* 47ACD8-47ACE3 0000B */
uchar _Globals::Unwind_0047acd8() { STUB_BODY(); return 0; }
// !FUNC 0x0047acd8 END

// !FUNC 0x0047ace3 BEGIN
/* 47ACE3-47ACEE 0000B */
uchar _Globals::Unwind_0047ace3() { STUB_BODY(); return 0; }
// !FUNC 0x0047ace3 END

// !FUNC 0x0047acee BEGIN
/* 47ACEE-47ACF9 0000B */
uchar _Globals::Unwind_0047acee() { STUB_BODY(); return 0; }
// !FUNC 0x0047acee END

// !FUNC 0x0047ad20 BEGIN
/* 47AD20-47AD28 00008 */
uchar _Globals::Unwind_0047ad20() { STUB_BODY(); return 0; }
// !FUNC 0x0047ad20 END

// !FUNC 0x0047ad28 BEGIN
/* 47AD28-47AD4F 00027 */
uchar _Globals::Unwind_0047ad28() { STUB_BODY(); return 0; }
// !FUNC 0x0047ad28 END

// !FUNC 0x0047ad4f BEGIN
/* 47AD4F-47AD76 00027 */
uchar _Globals::Unwind_0047ad4f() { STUB_BODY(); return 0; }
// !FUNC 0x0047ad4f END

// !FUNC 0x0047ad76 BEGIN
/* 47AD76-47AD9D 00027 */
uchar _Globals::Unwind_0047ad76() { STUB_BODY(); return 0; }
// !FUNC 0x0047ad76 END

// !FUNC 0x0047adc0 BEGIN
/* 47ADC0-47ADCB 0000B */
uchar _Globals::Unwind_0047adc0() { STUB_BODY(); return 0; }
// !FUNC 0x0047adc0 END

// !FUNC 0x0047adf0 BEGIN
/* 47ADF0-47ADF8 00008 */
uchar _Globals::Unwind_0047adf0() { STUB_BODY(); return 0; }
// !FUNC 0x0047adf0 END

// !FUNC 0x0047ae20 BEGIN
/* 47AE20-47AE28 00008 */
uchar _Globals::Unwind_0047ae20() { STUB_BODY(); return 0; }
// !FUNC 0x0047ae20 END

// !FUNC 0x0047ae28 BEGIN
/* 47AE28-47AE36 0000E */
uchar _Globals::Unwind_0047ae28() { STUB_BODY(); return 0; }
// !FUNC 0x0047ae28 END

// !FUNC 0x0047ae60 BEGIN
/* 47AE60-47AE68 00008 */
uchar _Globals::Unwind_0047ae60() { STUB_BODY(); return 0; }
// !FUNC 0x0047ae60 END

// !FUNC 0x0047ae68 BEGIN
/* 47AE68-47AE76 0000E */
uchar _Globals::Unwind_0047ae68() { STUB_BODY(); return 0; }
// !FUNC 0x0047ae68 END

// !FUNC 0x0047ae76 BEGIN
/* 47AE76-47AE84 0000E */
uchar _Globals::Unwind_0047ae76() { STUB_BODY(); return 0; }
// !FUNC 0x0047ae76 END

// !FUNC 0x0047aea0 BEGIN
/* 47AEA0-47AEAB 0000B */
uchar _Globals::Unwind_0047aea0() { STUB_BODY(); return 0; }
// !FUNC 0x0047aea0 END

// !FUNC 0x0047aed0 BEGIN
/* 47AED0-47AED8 00008 */
uchar _Globals::Unwind_0047aed0() { STUB_BODY(); return 0; }
// !FUNC 0x0047aed0 END

// !FUNC 0x0047af00 BEGIN
/* 47AF00-47AF08 00008 */
uchar _Globals::Unwind_0047af00() { STUB_BODY(); return 0; }
// !FUNC 0x0047af00 END

// !FUNC 0x0047af30 BEGIN
/* 47AF30-47AF38 00008 */
uchar _Globals::Unwind_0047af30() { STUB_BODY(); return 0; }
// !FUNC 0x0047af30 END

// !FUNC 0x0047af60 BEGIN
/* 47AF60-47AF68 00008 */
uchar _Globals::Unwind_0047af60() { STUB_BODY(); return 0; }
// !FUNC 0x0047af60 END

// !FUNC 0x0047af68 BEGIN
/* 47AF68-47AF70 00008 */
uchar _Globals::Unwind_0047af68() { STUB_BODY(); return 0; }
// !FUNC 0x0047af68 END

// !FUNC 0x0047afb0 BEGIN
/* 47AFB0-47AFB8 00008 */
uchar _Globals::Unwind_0047afb0() { STUB_BODY(); return 0; }
// !FUNC 0x0047afb0 END

// !FUNC 0x0047aff0 BEGIN
/* 47AFF0-47B015 00025 */
uchar _Globals::CBulanci_RegisterAppDescriptor() { STUB_BODY(); return 0; }
// !FUNC 0x0047aff0 END

// !FUNC 0x0042f590 BEGIN
/* 42F590-42F5B5 00025 */
uint _Globals::CDSView_PostMessage_NullSafe(void* param_1, ushort param_2, ushort param_3, uint param_4, uint param_5) { STUB_BODY(); return 0; }
// !FUNC 0x0042f590 END

// !FUNC 0x0043df90 BEGIN
/* 43DF90-43E032 000A2 */
uchar _Globals::BlitOpaque_BGR24_to_BGR24(int* param_1, int* param_2, int param_3, int param_4, int param_5, int param_6) { STUB_BODY(); return 0; }
// !FUNC 0x0043df90 END

// !FUNC 0x0042f390 BEGIN
/* 42F390-42F3DD 0004D */
uint _Globals::CDSView_PostMessage(ushort param_1, ushort param_2, uint param_3, uint param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0042f390 END

