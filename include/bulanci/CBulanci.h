#ifndef _C_BULANCI
#define _C_BULANCI

#include <globals.h>

/* Forward declarations of types referenced by parameter / return
 * positions but not (yet) defined in this project. Stubs only use
 * pointers so a struct forward-decl is enough. */
struct _PtFuncCompare;

class CBulanci {
public:
	uint* m_ptr;

	CBulanci() : m_ptr(0) {}
	CBulanci(short* param_1) {
		m_ptr = 0;
		FUN_0042d510(param_1);
	}
	~CBulanci();
	// !DECL 0x00401350 BEGIN
	/* 401350 */ int* FUN_00401350(int* param_1);
	// !DECL 0x00401350 END
	// !DECL 0x004013b0 BEGIN
	/* 4013B0 */ uint* FUN_004013b0(int param_1);
	// !DECL 0x004013b0 END
	// !DECL 0x004014b0 BEGIN
	/* 4014B0 */ uint* FUN_004014b0(uint* param_1);
	// !DECL 0x004014b0 END
	// !DECL 0x00401790 BEGIN
	/* 401790 */ uint* FUN_00401790(int* param_1);
	// !DECL 0x00401790 END
	// !DECL 0x00401970 BEGIN
	/* 401970 */ uchar FUN_00401970(int param_1);
	// !DECL 0x00401970 END
	// !DECL 0x00401be0 BEGIN
	/* 401BE0 */ uchar FUN_00401be0(int param_1);
	// !DECL 0x00401be0 END
	// !DECL 0x00401c30 BEGIN
	/* 401C30 */ uchar* Catch_00401c30();
	// !DECL 0x00401c30 END
	// !DECL 0x00401c59 BEGIN
	/* 401C59 */ bool FUN_00401c59();
	// !DECL 0x00401c59 END
	// !DECL 0x00401d80 BEGIN
	/* 401D80 */ int* FUN_00401d80(int param_1, int param_2);
	// !DECL 0x00401d80 END
	// !DECL 0x0040214f BEGIN
	/* 40214F */ uint Catch_0040214f();
	// !DECL 0x0040214f END
	// !DECL 0x00402180 BEGIN
	/* 402180 */ uchar CBulanci_InitResourceBank(int param_1);
	// !DECL 0x00402180 END
	// !DECL 0x00402490 BEGIN
	/* 402490 */ uchar CBulanci_OnEvent_MenuStateMachine(ushort param_1, uint param_2);
	// !DECL 0x00402490 END
	// !DECL 0x004026f0 BEGIN
	/* 4026F0 */ uint* CBulanci_ctor(uint* param_1);
	// !DECL 0x004026f0 END
	// !DECL 0x00402830 BEGIN
	/* 402830 */ uchar* CBulanci_GetAppDescriptor();
	// !DECL 0x00402830 END
	// !DECL 0x00402840 BEGIN
	/* 402840 */ uint FUN_00402840(int param_1);
	// !DECL 0x00402840 END
	// !DECL 0x00402850 BEGIN
	/* 402850 */ uchar FUN_00402850(uchar param_1);
	// !DECL 0x00402850 END
	// !DECL 0x00402860 BEGIN
	/* 402860 */ uchar FUN_00402860(uchar param_1);
	// !DECL 0x00402860 END
	// !DECL 0x00402870 BEGIN
	/* 402870 */ uchar FUN_00402870(uchar param_1);
	// !DECL 0x00402870 END
	// !DECL 0x00402880 BEGIN
	/* 402880 */ uchar* FUN_00402880();
	// !DECL 0x00402880 END
	// !DECL 0x00402890 BEGIN
	/* 402890 */ uchar* FUN_00402890();
	// !DECL 0x00402890 END
	// !DECL 0x004028a0 BEGIN
	/* 4028A0 */ uint CDSView_HitTest(int* param_1);
	// !DECL 0x004028a0 END
	// !DECL 0x004028d0 BEGIN
	/* 4028D0 */ uchar FUN_004028d0(uint* param_1);
	// !DECL 0x004028d0 END
	// !DECL 0x00402990 BEGIN
	/* 402990 */ int* CBulanci_BuildBitmapCache(int param_1, int param_2, int* param_3);
	// !DECL 0x00402990 END
	// !DECL 0x00402b00 BEGIN
	/* 402B00 */ uint* FUN_00402b00(uchar param_1);
	// !DECL 0x00402b00 END
	// !DECL 0x00402b20 BEGIN
	/* 402B20 */ uchar CBulanci_OnCreate(int* param_1);
	// !DECL 0x00402b20 END
	// !DECL 0x00402bc0 BEGIN
	/* 402BC0 */ uchar FUN_00402bc0(int* param_1);
	// !DECL 0x00402bc0 END
	// !DECL 0x00402c30 BEGIN
	/* 402C30 */ uchar FUN_00402c30(int* param_1);
	// !DECL 0x00402c30 END
	// !DECL 0x00402df0 BEGIN
	/* 402DF0 */ uchar FUN_00402df0(int param_1);
	// !DECL 0x00402df0 END
	// !DECL 0x00402e00 BEGIN
	/* 402E00 */ uchar FUN_00402e00(int param_1);
	// !DECL 0x00402e00 END
	// !DECL 0x00402e10 BEGIN
	/* 402E10 */ uchar FUN_00402e10(int param_1);
	// !DECL 0x00402e10 END
	// !DECL 0x004049c0 BEGIN
	/* 4049C0 */ uchar FUN_004049c0(int param_1);
	// !DECL 0x004049c0 END
	// !DECL 0x004049d0 BEGIN
	/* 4049D0 */ uchar FUN_004049d0(int param_1);
	// !DECL 0x004049d0 END
	// !DECL 0x00409080 BEGIN
	/* 409080 */ uint FUN_00409080(int param_1);
	// !DECL 0x00409080 END
	// !DECL 0x00409620 BEGIN
	/* 409620 */ uchar CLevelScore_InitializeDefaultScores(int param_1);
	// !DECL 0x00409620 END
	// !DECL 0x004096d0 BEGIN
	/* 4096D0 */ uchar FUN_004096d0(void* param_1, int* param_2, int* param_3, int* param_4, char param_5, char param_6);
	// !DECL 0x004096d0 END
	// !DECL 0x00409b10 BEGIN
	/* 409B10 */ uint* CLevelScore_AddPlayerScore(int* param_1, uint param_2, uint param_3);
	// !DECL 0x00409b10 END
	// !DECL 0x00409bc0 BEGIN
	/* 409BC0 */ uchar FUN_00409bc0(void* param_1, int* param_2, int* param_3, uint param_4, char param_5);
	// !DECL 0x00409bc0 END
	// !DECL 0x00409cd0 BEGIN
	/* 409CD0 */ uchar FUN_00409cd0(uchar* param_1);
	// !DECL 0x00409cd0 END
	// !DECL 0x00409f0d BEGIN
	/* 409F0D */ uint Catch_00409f0d();
	// !DECL 0x00409f0d END
	// !DECL 0x00409f60 BEGIN
	/* 409F60 */ uchar FUN_00409f60(int* param_1);
	// !DECL 0x00409f60 END
	// !DECL 0x0040a0b0 BEGIN
	/* 40A0B0 */ uchar FUN_0040a0b0(int param_1);
	// !DECL 0x0040a0b0 END
	// !DECL 0x0040a290 BEGIN
	/* 40A290 */ uchar FUN_0040a290(int param_1);
	// !DECL 0x0040a290 END
	// !DECL 0x0040a440 BEGIN
	/* 40A440 */ uchar FUN_0040a440(int param_1);
	// !DECL 0x0040a440 END
	// !DECL 0x0040a62f BEGIN
	/* 40A62F */ uchar* Catch_0040a62f();
	// !DECL 0x0040a62f END
	// !DECL 0x0040a650 BEGIN
	/* 40A650 */ uchar FUN_0040a650();
	// !DECL 0x0040a650 END
	// !DECL 0x0040ab90 BEGIN
	/* 40AB90 */ uchar FUN_0040ab90(int param_1, int param_2);
	// !DECL 0x0040ab90 END
	// !DECL 0x0040abc0 BEGIN
	/* 40ABC0 */ uchar CLoadingLevel_SetProgress(int param_1, int param_2);
	// !DECL 0x0040abc0 END
	// !DECL 0x004101d0 BEGIN
	/* 4101D0 */ uint* FUN_004101d0(uint param_1, uint param_2, uint param_3, uint param_4);
	// !DECL 0x004101d0 END
	// !DECL 0x00410e20 BEGIN
	/* 410E20 */ int* FUN_00410e20(int* param_1);
	// !DECL 0x00410e20 END
	// !DECL 0x00411010 BEGIN
	/* 411010 */ uint* FUN_00411010(void* param_1);
	// !DECL 0x00411010 END
	// !DECL 0x00411df0 BEGIN
	/* 411DF0 */ int* FUN_00411df0(int param_1);
	// !DECL 0x00411df0 END
	// !DECL 0x00412680 BEGIN
	/* 412680 */ int FUN_00412680(uchar param_1);
	// !DECL 0x00412680 END
	// !DECL 0x004126e0 BEGIN
	/* 4126E0 */ int FUN_004126e0(uchar param_1);
	// !DECL 0x004126e0 END
	// !DECL 0x00413940 BEGIN
	/* 413940 */ uint FUN_00413940(void* param_1);
	// !DECL 0x00413940 END
	// !DECL 0x00413ce0 BEGIN
	/* 413CE0 */ uchar CGame_StartGame(int* param_1);
	// !DECL 0x00413ce0 END
	// !DECL 0x0041408a BEGIN
	/* 41408A */ uchar Catch_0041408a();
	// !DECL 0x0041408a END
	// !DECL 0x00414280 BEGIN
	/* 414280 */ uchar CGame_GetOrCreateLevelScore(void* param_1);
	// !DECL 0x00414280 END
	// !DECL 0x00416770 BEGIN
	/* 416770 */ uchar FUN_00416770();
	// !DECL 0x00416770 END
	// !DECL 0x00416a70 BEGIN
	/* 416A70 */ uchar FUN_00416a70(uint param_1, uint param_2);
	// !DECL 0x00416a70 END
	// !DECL 0x00417c80 BEGIN
	/* 417C80 */ uchar FUN_00417c80(int param_1);
	// !DECL 0x00417c80 END
	// !DECL 0x0041b390 BEGIN
	/* 41B390 */ uchar FUN_0041b390(int* param_1);
	// !DECL 0x0041b390 END
	// !DECL 0x0041b6d0 BEGIN
	/* 41B6D0 */ uchar CGaming_LoadBackgroundMusic(int* param_1);
	// !DECL 0x0041b6d0 END
	// !DECL 0x0041b821 BEGIN
	/* 41B821 */ uchar Catch_0041b821();
	// !DECL 0x0041b821 END
	// !DECL 0x0041cfb0 BEGIN
	/* 41CFB0 */ uchar CBulanci_AllocAnimFromSprite(uint* param_1, uint param_2, char param_3);
	// !DECL 0x0041cfb0 END
	// !DECL 0x0041d2e0 BEGIN
	/* 41D2E0 */ uchar CGaming_LoadLevelAssetAndMusic(void* param_1);
	// !DECL 0x0041d2e0 END
	// !DECL 0x0041d580 BEGIN
	/* 41D580 */ uchar Catch_0041d580();
	// !DECL 0x0041d580 END
	// !DECL 0x0041d59d BEGIN
	/* 41D59D */ uint Catch_0041d59d();
	// !DECL 0x0041d59d END
	// !DECL 0x0041ff90 BEGIN
	/* 41FF90 */ uint* CGaming_ctor(int* param_1);
	// !DECL 0x0041ff90 END
	// !DECL 0x00420455 BEGIN
	/* 420455 */ uchar Catch_00420455();
	// !DECL 0x00420455 END
	// !DECL 0x00422550 BEGIN
	/* 422550 */ static void* FUN_00422550(uchar* param_1, uint param_2, int param_3, uint param_4, char param_5);
	// !DECL 0x00422550 END
	// !DECL 0x00423ea0 BEGIN
	/* 423EA0 */ uchar CRuch_EnableDisable(char param_1);
	// !DECL 0x00423ea0 END
	// !DECL 0x00423f70 BEGIN
	/* 423F70 */ uchar CMenu_EnableAllRuch(char param_1);
	// !DECL 0x00423f70 END
	// !DECL 0x00424bc0 BEGIN
	/* 424BC0 */ uint* CSwitch_ctor(uint param_1, uint param_2, int param_3, ushort param_4);
	// !DECL 0x00424bc0 END
	// !DECL 0x004252a0 BEGIN
	/* 4252A0 */ uchar CMenu_LoadInfoOverlayBg(uint param_1);
	// !DECL 0x004252a0 END
	// !DECL 0x004252f0 BEGIN
	/* 4252F0 */ uchar CMenu_SetDayNightBg(char param_1);
	// !DECL 0x004252f0 END
	// !DECL 0x00425400 BEGIN
	/* 425400 */ uchar CMenu_PollDayNight(char param_1);
	// !DECL 0x00425400 END
	// !DECL 0x004265e0 BEGIN
	/* 4265E0 */ uint* CMenu_ctor_with_ui(uint param_1, int param_2);
	// !DECL 0x004265e0 END
	// !DECL 0x00427100 BEGIN
	/* 427100 */ uchar FUN_00427100(uint param_1, char param_2);
	// !DECL 0x00427100 END
	// !DECL 0x00427d40 BEGIN
	/* 427D40 */ uint* FUN_00427d40(uint param_1, uint param_2);
	// !DECL 0x00427d40 END
	// !DECL 0x00427e30 BEGIN
	/* 427E30 */ uint* FUN_00427e30(int param_1, uint param_2);
	// !DECL 0x00427e30 END
	// !DECL 0x00427f70 BEGIN
	/* 427F70 */ uint* FUN_00427f70(uint param_1, int param_2, uint param_3);
	// !DECL 0x00427f70 END
	// !DECL 0x004280f0 BEGIN
	/* 4280F0 */ uint* FUN_004280f0(uint param_1);
	// !DECL 0x004280f0 END
	// !DECL 0x00429990 BEGIN
	/* 429990 */ uchar FUN_00429990(uint param_1);
	// !DECL 0x00429990 END
	// !DECL 0x00429a40 BEGIN
	/* 429A40 */ uchar FUN_00429a40(int param_1);
	// !DECL 0x00429a40 END
	// !DECL 0x00429a80 BEGIN
	/* 429A80 */ uchar FUN_00429a80(int param_1);
	// !DECL 0x00429a80 END
	// !DECL 0x00429af0 BEGIN
	/* 429AF0 */ int FUN_00429af0(int param_1, int param_2, int param_3);
	// !DECL 0x00429af0 END
	// !DECL 0x00429b90 BEGIN
	/* 429B90 */ uchar FUN_00429b90(int param_1);
	// !DECL 0x00429b90 END
	// !DECL 0x00429bb0 BEGIN
	/* 429BB0 */ uchar CDSApp_OnDestroy(void* param_1);
	// !DECL 0x00429bb0 END
	// !DECL 0x00429d00 BEGIN
	/* 429D00 */ uchar CDSApp_OnActivateApp(char param_1);
	// !DECL 0x00429d00 END
	// !DECL 0x00429d60 BEGIN
	/* 429D60 */ ushort CDSApp_Run(void* param_1);
	// !DECL 0x00429d60 END
	// !DECL 0x0042a000 BEGIN
	/* 42A000 */ uint CDSView_OnKeyDown(char param_1, char param_2);
	// !DECL 0x0042a000 END
	// !DECL 0x0042a1c0 BEGIN
	/* 42A1C0 */ uchar FUN_0042a1c0(int param_1);
	// !DECL 0x0042a1c0 END
	// !DECL 0x0042a330 BEGIN
	/* 42A330 */ uchar FUN_0042a330(int* param_1, int param_2, int param_3, int* param_4);
	// !DECL 0x0042a330 END
	// !DECL 0x0042a500 BEGIN
	/* 42A500 */ uchar CDSApp_SetWindowed(char param_1);
	// !DECL 0x0042a500 END
	// !DECL 0x0042a5c0 BEGIN
	/* 42A5C0 */ uchar CDSApp_MouseQueue(ushort param_1, ushort param_2, uint param_3);
	// !DECL 0x0042a5c0 END
	// !DECL 0x0042a660 BEGIN
	/* 42A660 */ uint CDSApp_WndProcDispatch(uint param_1, ushort param_2, uint param_3);
	// !DECL 0x0042a660 END
	// !DECL 0x0042b170 BEGIN
	/* 42B170 */ uchar CDSApp_ctor(short* param_1, short* param_2);
	// !DECL 0x0042b170 END
	// !DECL 0x0042b39b BEGIN
	/* 42B39B */ uchar* Catch_0042b39b();
	// !DECL 0x0042b39b END
	// !DECL 0x0042b3ae BEGIN
	/* 42B3AE */ uchar FUN_0042b3ae();
	// !DECL 0x0042b3ae END
	// !DECL 0x0042b8c0 BEGIN
	/* 42B8C0 */ uchar FUN_0042b8c0(int* param_1);
	// !DECL 0x0042b8c0 END
	// !DECL 0x0042bd70 BEGIN
	/* 42BD70 */ uchar CDSView_Invalidate(int* param_1, char param_2);
	// !DECL 0x0042bd70 END
	// !DECL 0x0042be90 BEGIN
	/* 42BE90 */ uchar FUN_0042be90(int* param_1);
	// !DECL 0x0042be90 END
	// !DECL 0x0042bf40 BEGIN
	/* 42BF40 */ uchar FUN_0042bf40(char param_1);
	// !DECL 0x0042bf40 END
	// !DECL 0x0042c0e0 BEGIN
	/* 42C0E0 */ uint CDSView_OnKeyUp(int param_1);
	// !DECL 0x0042c0e0 END
	// !DECL 0x0042c100 BEGIN
	/* 42C100 */ uint CDSView_OnChar(int param_1);
	// !DECL 0x0042c100 END
	// !DECL 0x0042c2e0 BEGIN
	/* 42C2E0 */ int FUN_0042c2e0(int param_1);
	// !DECL 0x0042c2e0 END
	// !DECL 0x0042c320 BEGIN
	/* 42C320 */ uchar FUN_0042c320(int param_1);
	// !DECL 0x0042c320 END
	// !DECL 0x0042c370 BEGIN
	/* 42C370 */ uchar FUN_0042c370(int param_1);
	// !DECL 0x0042c370 END
	// !DECL 0x0042c3e0 BEGIN
	/* 42C3E0 */ uint CDSView_IsModalDoneRecursive(uint param_1);
	// !DECL 0x0042c3e0 END
	// !DECL 0x0042c430 BEGIN
	/* 42C430 */ uchar CDSView_GetParentBounds(uint* param_1, int* param_2);
	// !DECL 0x0042c430 END
	// !DECL 0x0042c480 BEGIN
	/* 42C480 */ uchar CDSView_SetRect(int* param_1);
	// !DECL 0x0042c480 END
	// !DECL 0x0042c540 BEGIN
	/* 42C540 */ uchar FUN_0042c540(int* param_1, int param_2, int param_3);
	// !DECL 0x0042c540 END
	// !DECL 0x0042c580 BEGIN
	/* 42C580 */ uchar CDSView_ComputeAnchoredRect(int* param_1, int* param_2);
	// !DECL 0x0042c580 END
	// !DECL 0x0042c770 BEGIN
	/* 42C770 */ uchar FUN_0042c770(ushort param_1, uint param_2, uint param_3);
	// !DECL 0x0042c770 END
	// !DECL 0x0042cae0 BEGIN
	/* 42CAE0 */ uint CDSApp_AdaptDisplaySize(int* param_1);
	// !DECL 0x0042cae0 END
	// !DECL 0x0042cbf0 BEGIN
	/* 42CBF0 */ uchar FUN_0042cbf0(int param_1, int param_2);
	// !DECL 0x0042cbf0 END
	// !DECL 0x0042d310 BEGIN
	/* 42D310 */ static uint FUN_0042d310(int param_1);
	// !DECL 0x0042d310 END
	// !DECL 0x0042d330 BEGIN
	/* 42D330 */ static uint FUN_0042d330(int param_1);
	// !DECL 0x0042d330 END
	// !DECL 0x0042d3f0 BEGIN
	/* 42D3F0 */ int FUN_0042d3f0(ushort* param_1);
	// !DECL 0x0042d3f0 END
	// !DECL 0x0042d510 BEGIN
	/* 42D510 */ int* FUN_0042d510(short* param_1);
	// !DECL 0x0042d510 END
	// !DECL 0x0042d7b0 BEGIN
	/* 42D7B0 */ int FUN_0042d7b0(short param_1, int param_2);
	// !DECL 0x0042d7b0 END
	// !DECL 0x0042d7f0 BEGIN
	/* 42D7F0 */ uint* FUN_0042d7f0(uint* param_1, int param_2, int param_3);
	// !DECL 0x0042d7f0 END
	// !DECL 0x0042d8c0 BEGIN
	/* 42D8C0 */ uint* FUN_0042d8c0(DWORD param_1);
	// !DECL 0x0042d8c0 END
	// !DECL 0x0042d970 BEGIN
	/* 42D970 */ static uchar FUN_0042d970(int* param_1);
	// !DECL 0x0042d970 END
	// !DECL 0x0042dab0 BEGIN
	/* 42DAB0 */ uchar FUN_0042dab0(int param_1, int param_2);
	// !DECL 0x0042dab0 END
	// !DECL 0x0042dc30 BEGIN
	/* 42DC30 */ int* FUN_0042dc30(int* param_1);
	// !DECL 0x0042dc30 END
	// !DECL 0x0042dc70 BEGIN
	/* 42DC70 */ uchar FUN_0042dc70(int* param_1, void* param_2, int* param_3);
	// !DECL 0x0042dc70 END
	// !DECL 0x0042dec0 BEGIN
	/* 42DEC0 */ uchar FUN_0042dec0(int* param_1, void* param_2);
	// !DECL 0x0042dec0 END
	// !DECL 0x0042dfc0 BEGIN
	/* 42DFC0 */ void* FUN_0042dfc0(short* param_1, short* param_2);
	// !DECL 0x0042dfc0 END
	// !DECL 0x0042e0e0 BEGIN
	/* 42E0E0 */ int* FUN_0042e0e0(int* param_1);
	// !DECL 0x0042e0e0 END
	// !DECL 0x0042e230 BEGIN
	/* 42E230 */ void* FUN_0042e230(short* param_1, short* param_2);
	// !DECL 0x0042e230 END
	// !DECL 0x0042e2f0 BEGIN
	/* 42E2F0 */ int* FUN_0042e2f0(int* param_1);
	// !DECL 0x0042e2f0 END
	// !DECL 0x0042e400 BEGIN
	/* 42E400 */ uchar FUN_0042e400(int* param_1);
	// !DECL 0x0042e400 END
	// !DECL 0x0042e6c0 BEGIN
	/* 42E6C0 */ uchar PackTimeToDword(uint* param_1, uint param_2, uint param_3, uint param_4);
	// !DECL 0x0042e6c0 END
	// !DECL 0x0042e730 BEGIN
	/* 42E730 */ int FUN_0042e730(int* param_1);
	// !DECL 0x0042e730 END
	// !DECL 0x0042e850 BEGIN
	/* 42E850 */ void* FUN_0042e850(int param_1);
	// !DECL 0x0042e850 END
	// !DECL 0x0042e880 BEGIN
	/* 42E880 */ void* FUN_0042e880(ushort* param_1);
	// !DECL 0x0042e880 END
	// !DECL 0x0042e8b0 BEGIN
	/* 42E8B0 */ static void* BuildLocalDateTime(void* param_1);
	// !DECL 0x0042e8b0 END
	// !DECL 0x0042ea20 BEGIN
	/* 42EA20 */ static uchar InitializeClassIdLookup(int param_1);
	// !DECL 0x0042ea20 END
	// !DECL 0x0042f3e0 BEGIN
	/* 42F3E0 */ uint FUN_0042f3e0(int* param_1);
	// !DECL 0x0042f3e0 END
	// !DECL 0x0042f4b0 BEGIN
	/* 42F4B0 */ static uchar FUN_0042f4b0(int param_1, uint param_2, uint param_3, uint param_4);
	// !DECL 0x0042f4b0 END
	// !DECL 0x0042f8b0 BEGIN
	/* 42F8B0 */ int FUN_0042f8b0(int param_1);
	// !DECL 0x0042f8b0 END
	// !DECL 0x0042f940 BEGIN
	/* 42F940 */ uchar FUN_0042f940(int param_1, char param_2);
	// !DECL 0x0042f940 END
	// !DECL 0x0042f980 BEGIN
	/* 42F980 */ uchar FUN_0042f980(void* param_1);
	// !DECL 0x0042f980 END
	// !DECL 0x0042fae0 BEGIN
	/* 42FAE0 */ uchar FUN_0042fae0(_PtFuncCompare* param_1);
	// !DECL 0x0042fae0 END
	// !DECL 0x004332d0 BEGIN
	/* 4332D0 */ uchar FUN_004332d0(int* param_1, int* param_2);
	// !DECL 0x004332d0 END
	// !DECL 0x00433320 BEGIN
	/* 433320 */ uchar FUN_00433320(int param_1);
	// !DECL 0x00433320 END
	// !DECL 0x00434160 BEGIN
	/* 434160 */ uchar FUN_00434160(int* param_1);
	// !DECL 0x00434160 END
	// !DECL 0x0043420c BEGIN
	/* 43420C */ uchar Catch_0043420c();
	// !DECL 0x0043420c END
	// !DECL 0x004356e0 BEGIN
	/* 4356E0 */ uchar FUN_004356e0(int* param_1, uint param_2);
	// !DECL 0x004356e0 END
	// !DECL 0x00435960 BEGIN
	/* 435960 */ uint* FUN_00435960(int* param_1, uint param_2);
	// !DECL 0x00435960 END
	// !DECL 0x004359f3 BEGIN
	/* 4359F3 */ uchar Catch_004359f3();
	// !DECL 0x004359f3 END
	// !DECL 0x00437e20 BEGIN
	/* 437E20 */ uchar FUN_00437e20(int param_1, int param_2, uchar* param_3);
	// !DECL 0x00437e20 END
	// !DECL 0x00437f40 BEGIN
	/* 437F40 */ uint FUN_00437f40(int param_1);
	// !DECL 0x00437f40 END
	// !DECL 0x00437fe0 BEGIN
	/* 437FE0 */ uchar FUN_00437fe0(uint param_1, int* param_2);
	// !DECL 0x00437fe0 END
	// !DECL 0x00438160 BEGIN
	/* 438160 */ uchar FUN_00438160(int param_1, int* param_2);
	// !DECL 0x00438160 END
	// !DECL 0x00438340 BEGIN
	/* 438340 */ uchar FUN_00438340();
	// !DECL 0x00438340 END
	// !DECL 0x00438f80 BEGIN
	/* 438F80 */ uchar CDSView_EmptyHook27();
	// !DECL 0x00438f80 END
	// !DECL 0x0043c9b0 BEGIN
	/* 43C9B0 */ uchar FUN_0043c9b0(int param_1);
	// !DECL 0x0043c9b0 END
	// !DECL 0x00446550 BEGIN
	/* 446550 */ uchar InitAlphaBlendLut();
	// !DECL 0x00446550 END
	// !DECL 0x004477df BEGIN
	/* 4477DF */ static uchar FUN_004477df(ulong param_1);
	// !DECL 0x004477df END
	// !DECL 0x004482f0 BEGIN
	/* 4482F0 */ static uchar swap();
	// !DECL 0x004482f0 END
	// !DECL 0x00448320 BEGIN
	/* 448320 */ static uchar shortsort(uchar* param_1, uchar* param_2, int param_3, uchar* param_4);
	// !DECL 0x00448320 END
	// !DECL 0x004483b0 BEGIN
	/* 4483B0 */ static void _qsort(void* param_1, size_t param_2, size_t param_3, _PtFuncCompare* param_4);
	// !DECL 0x004483b0 END
	// !DECL 0x0044877d BEGIN
	/* 44877D */ static wchar_t* _wcsrchr(wchar_t* param_1, wchar_t param_2);
	// !DECL 0x0044877d END
};

#endif
