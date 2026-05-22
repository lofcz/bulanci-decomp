#ifndef _O_D_S_IMAGE
#define _O_D_S_IMAGE

#include <globals.h>

class ODSImage {
public:
	// !DECL 0x00418ef0 BEGIN
	/* 418EF0 */ uint* FUN_00418ef0(uint* param_1);
	// !DECL 0x00418ef0 END
	// !DECL 0x00419070 BEGIN
	/* 419070 */ uint* FUN_00419070(uint* param_1);
	// !DECL 0x00419070 END
	// !DECL 0x0041dbc0 BEGIN
	/* 41DBC0 */ uint* FUN_0041dbc0(uint param_1, uchar param_2);
	// !DECL 0x0041dbc0 END
	// !DECL 0x004228f0 BEGIN
	/* 4228F0 */ uint* FUN_004228f0(uint* param_1);
	// !DECL 0x004228f0 END
	// !DECL 0x004253d0 BEGIN
	/* 4253D0 */ uint CGunMouse_CoordRing_Reserve(int param_1);
	// !DECL 0x004253d0 END

	// !DECL 0x00426060 BEGIN
	/* 426060 */ uint* CGunMouse_ctor(uint* param_1);
	// !DECL 0x00426060 END

	// !DECL 0x00418c00 BEGIN
	/* 418C00 */ void* ODSImage_ctor(int* param_1);
	// !DECL 0x00418c00 END
	// !DECL 0x00439050 BEGIN
	/* 439050 */ uchar SetOwner(void* param_1);
	// !DECL 0x00439050 END
};

#endif
