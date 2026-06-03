#ifndef _C_EXIT_DLG
#define _C_EXIT_DLG

#include <globals.h>

class CExitDlg {
public:
	// !DECL 0x0040b290 BEGIN
	/* 40B290 */ uchar CExitDlg_RouteSyntheticCloseEvent(ushort param_1);
	// !DECL 0x0040b290 END
	// !DECL 0x00411b50 BEGIN
	/* 411B50 */ void* CExitDlg_ctor(void* param_1);
	// !DECL 0x00411b50 END
	// !DECL 0x00411de0 BEGIN
	/* 411DE0 */ uchar* CExitDlg_GetClassTable();
	// !DECL 0x00411de0 END
	// !DECL 0x004390d0 BEGIN
	/* 4390D0 */ uchar CAnim_SetTimelineActive(uchar param_1);
	// !DECL 0x004390d0 END
};

#endif
