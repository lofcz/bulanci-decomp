#ifndef _C_START_GAME2
#define _C_START_GAME2

#include <globals.h>

class CStartGame2 {
public:
	// !DECL 0x00405440 BEGIN
	/* 405440 */ uchar CStaticText_SetLayoutAndInvalidate(uint param_1, uint param_2);
	// !DECL 0x00405440 END
	// !DECL 0x00407d50 BEGIN
	/* 407D50 */ uchar CNumEdit_BuildAt(uint param_1, uint param_2, uint param_3, uint param_4, uint param_5, uint param_6, uint param_7, uint param_8);
	// !DECL 0x00407d50 END
	// !DECL 0x0040bb40 BEGIN
	/* 40BB40 */ uchar* CStartGame2_GetTypeDescriptor();
	// !DECL 0x0040bb40 END
	// !DECL 0x0040bb50 BEGIN
	/* 40BB50 */ uchar CStartGame2_VectorDtorWrapper_thunk_Sub70(uchar param_1);
	// !DECL 0x0040bb50 END
	// !DECL 0x0040bb60 BEGIN
	/* 40BB60 */ uchar CStartGame2_VectorDtorWrapper_thunk_Sub18(uchar param_1);
	// !DECL 0x0040bb60 END
	// !DECL 0x0040bb70 BEGIN
	/* 40BB70 */ uchar CStartGame2_VectorDtorWrapper_thunk_Sub4(uchar param_1);
	// !DECL 0x0040bb70 END
	// !DECL 0x0040bb80 BEGIN
	/* 40BB80 */ uchar CStartGame2_VectorDtorWrapper_thunk_Sub10(uchar param_1);
	// !DECL 0x0040bb80 END
	// !DECL 0x0040d520 BEGIN
	/* 40D520 */ void CStartGame2_UpdateDuplicateSerialWarning();
	// !DECL 0x0040d520 END
	// !DECL 0x0040d570 BEGIN
	/* 40D570 */ void CStartGame2_UpdateGamemodeCaption();
	// !DECL 0x0040d570 END
	// !DECL 0x0040d6b0 BEGIN
	/* 40D6B0 */ uchar CStartGame2_OnCustomMsg(ushort param_1, int param_2, uint param_3);
	// !DECL 0x0040d6b0 END
	// !DECL 0x0040d7e0 BEGIN
	/* 40D7E0 */ uchar CStartGame2_OnBroadcast(short param_1, int param_2, uint param_3);
	// !DECL 0x0040d7e0 END
	// !DECL 0x0040e760 BEGIN
	/* 40E760 */ uchar CStartGame1_IdentityCast_Sub6c(int param_1);
	// !DECL 0x0040e760 END
	// !DECL 0x0040e770 BEGIN
	/* 40E770 */ uchar CStartGame1_ReleaseViaVtable_Sub70(int param_1);
	// !DECL 0x0040e770 END
	// !DECL 0x0040ed90 BEGIN
	/* 40ED90 */ uchar CStartGame2_VectorDtorWrapper(uchar param_1);
	// !DECL 0x0040ed90 END
	// !DECL 0x0040f610 BEGIN
	/* 40F610 */ uint CStartGame2_Tick_CheckDuplicateNames(int param_1);
	// !DECL 0x0040f610 END
	// !DECL 0x0040f7d0 BEGIN
	/* 40F7D0 */ uchar CKeybShow_Build(int param_1);
	// !DECL 0x0040f7d0 END
	// !DECL 0x0040fc20 BEGIN
	/* 40FC20 */ uchar CStartGame2_OnCmd(ushort param_1);
	// !DECL 0x0040fc20 END
	// !DECL 0x00412c70 BEGIN
	/* 412C70 */ uchar CGame_NetSendSetAvatar_t03(uchar param_1, void* param_2, char param_3);
	// !DECL 0x00412c70 END
	// !DECL 0x00412cd0 BEGIN
	/* 412CD0 */ uchar CGame_NetSendSlotCount_t04(void* param_1, char param_2);
	// !DECL 0x00412cd0 END
	// !DECL 0x00412d40 BEGIN
	/* 412D40 */ uchar CGame_NetSendAdminByte_t64(void* param_1, char param_2);
	// !DECL 0x00412d40 END
	// !DECL 0x00412e90 BEGIN
	/* 412E90 */ uchar CStartGame2_ApplySetAvatar(uchar param_1, char param_2, void* param_3);
	// !DECL 0x00412e90 END
	// !DECL 0x00412ed0 BEGIN
	/* 412ED0 */ uchar CStartGame2_ApplySlotCountIfChanged(uchar param_1, int param_2, void* param_3);
	// !DECL 0x00412ed0 END
	// !DECL 0x00412f10 BEGIN
	/* 412F10 */ uchar CStartGame2_SetGameTypeAdminByte(char param_1, void* param_2);
	// !DECL 0x00412f10 END
	// !DECL 0x004139b0 BEGIN
	/* 4139B0 */ uchar CGame_NetSendRename_t02(uchar param_1, void* param_2, char param_3);
	// !DECL 0x004139b0 END
	// !DECL 0x00413a80 BEGIN
	/* 413A80 */ uchar CStartGame2_ApplyPlayerRename(uchar param_1, int param_2, void* param_3);
	// !DECL 0x00413a80 END
	// !DECL 0x00414340 BEGIN
	/* 414340 */ uchar CGame_NetSendSetLevel_t05(void* param_1, char param_2);
	// !DECL 0x00414340 END
	// !DECL 0x00414420 BEGIN
	/* 414420 */ uchar CStartGame2_SetSelectedLevel(int param_1, void* param_2);
	// !DECL 0x00414420 END
	// !DECL 0x004224a0 BEGIN
	/* 4224A0 */ static uchar CStartGame2_AssignBindingLabelFromScanCode(uchar param_1, uchar param_2);
	// !DECL 0x004224a0 END
	// !DECL 0x0042d7a0 BEGIN
	/* 42D7A0 */ uchar CDsString_CompareHandles(uint* param_1);
	// !DECL 0x0042d7a0 END
	// !DECL 0x0042ec40 BEGIN
	/* 42EC40 */ uint CStartGame2_EnqueueEvent(ushort param_1, ushort param_2, uint param_3, uint param_4);
	// !DECL 0x0042ec40 END

	// !DECL 0x004104f0 BEGIN
	/* 4104F0 */ void* CStartGame2_ctor(uchar param_1);
	// !DECL 0x004104f0 END
};

#endif
