#ifndef _C_HELP_DLG
#define _C_HELP_DLG

#include <globals.h>

class CHelpDlg {
public:
	// !DECL 0x00421c10 BEGIN
	/* 421C10 */ void CHelpDlg_LoadHelpPage(int param_1);
	// !DECL 0x00421c10 END
	// !DECL 0x00421dd0 BEGIN
	/* 421DD0 */ uchar CHelpDlg_OnSyntheticPageNav(ushort param_1);
	// !DECL 0x00421dd0 END
	// !DECL 0x00421e40 BEGIN
	/* 421E40 */ void* CHelpDlgCtor(void* param_1);
	// !DECL 0x00421e40 END
	// !DECL 0x004220a0 BEGIN
	/* 4220A0 */ uchar* CHelpDlg_GetClassTable();
	// !DECL 0x004220a0 END
	// !DECL 0x004220b0 BEGIN
	/* 4220B0 */ uchar CHelpDlg_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x004220b0 END
	// !DECL 0x004220c0 BEGIN
	/* 4220C0 */ uchar CHelpDlg_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x004220c0 END
	// !DECL 0x004220d0 BEGIN
	/* 4220D0 */ uchar CHelpDlg_ScalarDeletingDtor_thunk_004220d0(uchar param_1);
	// !DECL 0x004220d0 END
	// !DECL 0x004220e0 BEGIN
	/* 4220E0 */ void CHelpDlg_dtor(void* param_1);
	// !DECL 0x004220e0 END
	// !DECL 0x004222f0 BEGIN
	/* 4222F0 */ void* CHelpDlg_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004222f0 END
};

#endif
