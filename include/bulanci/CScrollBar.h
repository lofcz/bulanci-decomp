#ifndef _C_SCROLL_BAR
#define _C_SCROLL_BAR

#include <globals.h>

class CScrollBar {
public:
	// !DECL 0x00402e80 BEGIN
	/* 402E80 */ static uchar CScrollBar_BlitVerticalTrack(CPoemScroller* param_1, int param_2, int param_3, int param_4, int param_5);
	// !DECL 0x00402e80 END
	// !DECL 0x00402ee0 BEGIN
	/* 402EE0 */ static uchar CScrollBar_BlitHorizontalTrack(CPoemScroller* param_1, int param_2, int param_3, int param_4, int param_5);
	// !DECL 0x00402ee0 END
	// !DECL 0x00402f60 BEGIN
	/* 402F60 */ void CScrollBar_OnMouseMove();
	// !DECL 0x00402f60 END
	// !DECL 0x00403530 BEGIN
	/* 403530 */ void CScrollBar_Render();
	// !DECL 0x00403530 END
	// !DECL 0x00403860 BEGIN
	/* 403860 */ int CScrollBar_HitTest(int* param_1);
	// !DECL 0x00403860 END
	// !DECL 0x00403980 BEGIN
	/* 403980 */ uchar CScrollBar_OnKeyDown(int* param_1);
	// !DECL 0x00403980 END
	// !DECL 0x00403a80 BEGIN
	/* 403A80 */ void CScrollBar_OnTimerTick();
	// !DECL 0x00403a80 END
	// !DECL 0x00404790 BEGIN
	/* 404790 */ uchar CScrollBar_ctor(uchar param_1);
	// !DECL 0x00404790 END
	// !DECL 0x00404840 BEGIN
	/* 404840 */ uchar* CScrollBar_GetTypeDescriptor();
	// !DECL 0x00404840 END
	// !DECL 0x00404850 BEGIN
	/* 404850 */ uchar CScrollBar_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x00404850 END
	// !DECL 0x00404860 BEGIN
	/* 404860 */ uchar CScrollBar_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x00404860 END
	// !DECL 0x00404870 BEGIN
	/* 404870 */ uchar CScrollBar_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x00404870 END
	// !DECL 0x00404880 BEGIN
	/* 404880 */ uchar CScrollBar_AdjustorThunk68_Dtor(uchar param_1);
	// !DECL 0x00404880 END
	// !DECL 0x00404fe0 BEGIN
	/* 404FE0 */ void CScrollBar_dtor(int* param_1);
	// !DECL 0x00404fe0 END
	// !DECL 0x00406430 BEGIN
	/* 406430 */ uchar CScrollBar_vDtor(uchar param_1);
	// !DECL 0x00406430 END
	// !DECL 0x00406770 BEGIN
	/* 406770 */ void CScrollBar_Invalidate(uchar param_1);
	// !DECL 0x00406770 END
	// !DECL 0x00407460 BEGIN
	/* 407460 */ void CScrollBar_OnMouseDown(int* param_1, short param_2);
	// !DECL 0x00407460 END
	// !DECL 0x004074c0 BEGIN
	/* 4074C0 */ uchar CScrollBar_OnMouseUp(uint param_1, uchar param_2);
	// !DECL 0x004074c0 END
	// !DECL 0x00408370 BEGIN
	/* 408370 */ uchar CScrollBar_BuildAt(int param_1, int param_2, int param_3, int param_4, uint* param_5);
	// !DECL 0x00408370 END

	// !DECL 0x00404f70 BEGIN
	/* 404F70 */ int CScrollBar_SetValue(int param_1);
	// !DECL 0x00404f70 END
	// !DECL 0x004051d0 BEGIN
	/* 4051D0 */ void CScrollBar_SetScrollRange(int param_1, int param_2);
	// !DECL 0x004051d0 END
};

#endif
