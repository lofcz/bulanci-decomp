#ifndef _C_PAUSE_DLG
#define _C_PAUSE_DLG

#include <globals.h>

class CPauseDlg {
public:
	// !DECL 0x0040ac20 BEGIN
	/* 40AC20 */ uint CPauseDlg_OnKeyDown(char param_1, char param_2);
	// !DECL 0x0040ac20 END
	// !DECL 0x0040ac50 BEGIN
	/* 40AC50 */ uchar FUN_0040ac50(int param_1);
	// !DECL 0x0040ac50 END
	// !DECL 0x0040b2f0 BEGIN
	/* 40B2F0 */ uchar CPauseDlg_OnNotify(short param_1, char param_2, int param_3);
	// !DECL 0x0040b2f0 END
	// !DECL 0x0040b410 BEGIN
	/* 40B410 */ void CPauseDlg_OnCommand(ushort param_1);
	// !DECL 0x0040b410 END
	// !DECL 0x0040be10 BEGIN
	/* 40BE10 */ uchar* CPauseDlg_GetClassTable();
	// !DECL 0x0040be10 END
	// !DECL 0x0040f090 BEGIN
	/* 40F090 */ uint* CPauseDlg_Allocate();
	// !DECL 0x0040f090 END
};

#endif
