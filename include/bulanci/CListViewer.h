#ifndef _C_LIST_VIEWER
#define _C_LIST_VIEWER

#include <globals.h>

class CListViewer {
public:
	// !DECL 0x00404180 BEGIN
	/* 404180 */ uchar FUN_00404180(int* param_1);
	// !DECL 0x00404180 END
	// !DECL 0x004041e0 BEGIN
	/* 4041E0 */ uchar FUN_004041e0(int* param_1);
	// !DECL 0x004041e0 END
	// !DECL 0x00404240 BEGIN
	/* 404240 */ uchar FUN_00404240(int* param_1);
	// !DECL 0x00404240 END
	// !DECL 0x00404290 BEGIN
	/* 404290 */ uchar FUN_00404290(int param_1, int* param_2);
	// !DECL 0x00404290 END
	// !DECL 0x004044f0 BEGIN
	/* 4044F0 */ uchar FUN_004044f0(int* param_1, int* param_2);
	// !DECL 0x004044f0 END
	// !DECL 0x00405b30 BEGIN
	/* 405B30 */ uint CListViewer_HitTestItem(int param_1, int param_2);
	// !DECL 0x00405b30 END
	// !DECL 0x00405dd0 BEGIN
	/* 405DD0 */ uchar CListViewer_OnMouseClick(int* param_1);
	// !DECL 0x00405dd0 END
	// !DECL 0x00405df0 BEGIN
	/* 405DF0 */ uchar FUN_00405df0(void* param_1);
	// !DECL 0x00405df0 END
	// !DECL 0x00405e10 BEGIN
	/* 405E10 */ uint CListViewer_AddItem(int param_1);
	// !DECL 0x00405e10 END
	// !DECL 0x00406d00 BEGIN
	/* 406D00 */ uchar FUN_00406d00(int* param_1);
	// !DECL 0x00406d00 END
	// !DECL 0x00407f50 BEGIN
	/* 407F50 */ uint* CListViewer_ctor(uint* param_1);
	// !DECL 0x00407f50 END
	// !DECL 0x00407fd0 BEGIN
	/* 407FD0 */ uchar* CListViewer_GetTypeDescriptor();
	// !DECL 0x00407fd0 END
	// !DECL 0x00407fe0 BEGIN
	/* 407FE0 */ uchar FUN_00407fe0(uchar param_1);
	// !DECL 0x00407fe0 END
	// !DECL 0x00407ff0 BEGIN
	/* 407FF0 */ uchar FUN_00407ff0(uchar param_1);
	// !DECL 0x00407ff0 END
	// !DECL 0x00408000 BEGIN
	/* 408000 */ uchar FUN_00408000(uchar param_1);
	// !DECL 0x00408000 END
	// !DECL 0x00408010 BEGIN
	/* 408010 */ uchar FUN_00408010(uchar param_1);
	// !DECL 0x00408010 END
	// !DECL 0x00408020 BEGIN
	/* 408020 */ uchar CListViewer_dtor(uint* param_1);
	// !DECL 0x00408020 END
	// !DECL 0x004081a0 BEGIN
	/* 4081A0 */ uchar FUN_004081a0(int* param_1);
	// !DECL 0x004081a0 END
	// !DECL 0x00408330 BEGIN
	/* 408330 */ uint* CListViewer_vDtor(uchar param_1);
	// !DECL 0x00408330 END
	// !DECL 0x00408c00 BEGIN
	/* 408C00 */ uint* CListViewer_BuildAt(int param_1, int param_2, uint param_3, int param_4, uint param_5, uint param_6, uint param_7);
	// !DECL 0x00408c00 END
};

#endif
