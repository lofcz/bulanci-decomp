#ifndef _C_GAME_TYPE_DLG
#define _C_GAME_TYPE_DLG

#include <globals.h>

class CGameTypeDlg {
public:
	// !DECL 0x0040aad0 BEGIN
	/* 40AAD0 */ void CGameTypeDlg_RefreshFocusOrEnable();
	// !DECL 0x0040aad0 END
	// !DECL 0x0040ab00 BEGIN
	/* 40AB00 */ void CGameTypeDlg_OnNotify(short param_1, int param_2, uint param_3);
	// !DECL 0x0040ab00 END
	// !DECL 0x0040bc00 BEGIN
	/* 40BC00 */ uchar* CGameTypeDlg_GetClassTable();
	// !DECL 0x0040bc00 END
	// !DECL 0x0040ee40 BEGIN
	/* 40EE40 */ uchar CGameTypeDlg_CreateObject();
	// !DECL 0x0040ee40 END

	// !DECL 0x0040d930 BEGIN
	/* 40D930 */ uchar CGameTypeDlg_BuildUi(int param_1);
	// !DECL 0x0040d930 END
};

#endif
