#ifndef _C_SETUP_DLG
#define _C_SETUP_DLG

#include <globals.h>

class CSetupDlg {
public:
	// !DECL 0x0040e290 BEGIN
	/* 40E290 */ void* CSetupDlgCtor(void* param_1);
	// !DECL 0x0040e290 END
	// !DECL 0x0040e4d0 BEGIN
	/* 40E4D0 */ uchar* CSetupDlg_GetClassTable();
	// !DECL 0x0040e4d0 END
	// !DECL 0x0040e4e0 BEGIN
	/* 40E4E0 */ void CSetupDlg_UpdateVolumeLabel(int param_1);
	// !DECL 0x0040e4e0 END
	// !DECL 0x0040e5b0 BEGIN
	/* 40E5B0 */ void CSetupDlg_OnVolumeFocus(short param_1, int param_2);
	// !DECL 0x0040e5b0 END

	// !DECL 0x0040e590 BEGIN
	/* 40E590 */ void CSetupDlg_SetVolumeBinding(int param_1);
	// !DECL 0x0040e590 END
};

#endif
