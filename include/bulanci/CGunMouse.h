#ifndef _C_GUN_MOUSE
#define _C_GUN_MOUSE

#include <globals.h>

class CGunMouse {
public:
	// !DECL 0x004238e0 BEGIN
	/* 4238E0 */ uchar CGunMouse_Deactivate(int param_1);
	// !DECL 0x004238e0 END
	// !DECL 0x00423900 BEGIN
	/* 423900 */ uchar CGunMouse_OnMouseMove(uint param_1, int* param_2);
	// !DECL 0x00423900 END
	// !DECL 0x00423ac0 BEGIN
	/* 423AC0 */ uchar CGunMouse_CoordRing_Clear(int param_1);
	// !DECL 0x00423ac0 END
	// !DECL 0x00423b50 BEGIN
	/* 423B50 */ uchar CGunMouse_Activate(int param_1);
	// !DECL 0x00423b50 END
	// !DECL 0x00423bd0 BEGIN
	/* 423BD0 */ uchar CGunMouse_OnAnimTick(uint param_1, short param_2);
	// !DECL 0x00423bd0 END
	// !DECL 0x00424370 BEGIN
	/* 424370 */ uint CGunMouse_CoordRing_Push(uint* param_1);
	// !DECL 0x00424370 END
	// !DECL 0x004243b0 BEGIN
	/* 4243B0 */ uint CGunMouse_CoordRing_Pop(uint* param_1);
	// !DECL 0x004243b0 END
	// !DECL 0x00424610 BEGIN
	/* 424610 */ uchar CGunMouse_Draw(int param_1);
	// !DECL 0x00424610 END
	// !DECL 0x004249b0 BEGIN
	/* 4249B0 */ uchar CGunMouse_Erase(int param_1);
	// !DECL 0x004249b0 END
	// !DECL 0x00425730 BEGIN
	/* 425730 */ uchar CGunMouse_dtor(uint* param_1);
	// !DECL 0x00425730 END
	// !DECL 0x00425850 BEGIN
	/* 425850 */ uchar* FUN_00425850();
	// !DECL 0x00425850 END
	// !DECL 0x00425860 BEGIN
	/* 425860 */ uchar FUN_00425860(uchar param_1);
	// !DECL 0x00425860 END
	// !DECL 0x004262a0 BEGIN
	/* 4262A0 */ void* FUN_004262a0(uchar param_1);
	// !DECL 0x004262a0 END

};

#endif
