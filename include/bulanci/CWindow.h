#ifndef _C_WINDOW
#define _C_WINDOW

#include <globals.h>

class CWindow {
public:
	CWindow() {}
	CWindow(uint param_1, uint param_2, uint param_3, uint param_4, uchar param_5) {
		CWindow_BuildAt(param_1, param_2, param_3, param_4, param_5);
	}

	// !DECL 0x00403d60 BEGIN
	/* 403D60 */ uint CWindow_OnKeyDownTabEscEnter(char param_1, char param_2);
	// !DECL 0x00403d60 END
	// !DECL 0x004049b0 BEGIN
	/* 4049B0 */ uchar* CWindow_GetTypeDescriptor();
	// !DECL 0x004049b0 END
	// !DECL 0x00405560 BEGIN
	/* 405560 */ void* CWindow_BuildAt(int param_1, int param_2, int param_3, int param_4, uchar param_5);
	// !DECL 0x00405560 END
	// !DECL 0x004055c0 BEGIN
	/* 4055C0 */ void CWindow_Render(void* param_1);
	// !DECL 0x004055c0 END
	// !DECL 0x004064e0 BEGIN
	/* 4064E0 */ uint* CWindow_dtor();
	// !DECL 0x004064e0 END
	// !DECL 0x0040baa0 BEGIN
	/* 40BAA0 */ void* CWindow_ctor(void* param_1);
	// !DECL 0x0040baa0 END
	// !DECL 0x0040bc10 BEGIN
	/* 40BC10 */ uchar CWindow_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x0040bc10 END
	// !DECL 0x0040be20 BEGIN
	/* 40BE20 */ uchar CWindow_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x0040be20 END
	// !DECL 0x0040c540 BEGIN
	/* 40C540 */ uchar CWindow_ScalarDeletingDtor_thunk_0040c540(uchar param_1);
	// !DECL 0x0040c540 END
	// !DECL 0x0040f120 BEGIN
	/* 40F120 */ uchar CWindow_vDtor(uchar param_1);
	// !DECL 0x0040f120 END
	// !DECL 0x0042c660 BEGIN
	/* 42C660 */ uint CWindow_FindNextFocusable(char param_1);
	// !DECL 0x0042c660 END
	// !DECL 0x0042ccd0 BEGIN
	/* 42CCD0 */ uchar CWindow_FocusSibling(char param_1);
	// !DECL 0x0042ccd0 END
};

#endif
