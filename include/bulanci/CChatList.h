#ifndef _C_CHAT_LIST
#define _C_CHAT_LIST

#include <globals.h>

class CChatList {
public:
	// !DECL 0x00404c80 BEGIN
	/* 404C80 */ uint* FUN_00404c80(uint* param_1);
	// !DECL 0x00404c80 END
	// !DECL 0x004058f0 BEGIN
	/* 4058F0 */ uchar FUN_004058f0(int param_1, int param_2);
	// !DECL 0x004058f0 END
	// !DECL 0x00405b10 BEGIN
	/* 405B10 */ uchar FUN_00405b10(uint param_1, uint param_2);
	// !DECL 0x00405b10 END
	// !DECL 0x00405ca0 BEGIN
	/* 405CA0 */ uint FUN_00405ca0(int param_1);
	// !DECL 0x00405ca0 END
	// !DECL 0x00405ea0 BEGIN
	/* 405EA0 */ uchar FUN_00405ea0(void* param_1);
	// !DECL 0x00405ea0 END
	// !DECL 0x0040d010 BEGIN
	/* 40D010 */ void* CChatList_BuildAt(int* param_1, uint param_2, uint param_3);
	// !DECL 0x0040d010 END
	// !DECL 0x0040d090 BEGIN
	/* 40D090 */ uchar* CChatList_GetTypeDescriptor();
	// !DECL 0x0040d090 END
	// !DECL 0x0040d0b0 BEGIN
	/* 40D0B0 */ uchar CChatList_OnEvent(short param_1, int param_2, uint* param_3);
	// !DECL 0x0040d0b0 END
	// !DECL 0x0040ff20 BEGIN
	/* 40FF20 */ uint* CChatList_Allocate();
	// !DECL 0x0040ff20 END
};

#endif
