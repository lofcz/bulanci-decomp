#ifndef _C_D_S_FONT
#define _C_D_S_FONT

#include <globals.h>

class CDSFont {
public:
	// !DECL 0x004372b0 BEGIN
	/* 4372B0 */ uchar Read(int* param_1);
	// !DECL 0x004372b0 END
	// !DECL 0x004372f0 BEGIN
	/* 4372F0 */ uchar ReadNoAlloc(int* param_1);
	// !DECL 0x004372f0 END
	// !DECL 0x00437330 BEGIN
	/* 437330 */ uint GetCharWidth(uchar param_1, uchar* param_2);
	// !DECL 0x00437330 END
	// !DECL 0x00437370 BEGIN
	/* 437370 */ uchar DrawChar(uchar param_1, void* param_2, int* param_3, int param_4, uchar* param_5, uint* param_6);
	// !DECL 0x00437370 END
	// !DECL 0x00437510 BEGIN
	/* 437510 */ uchar* FUN_00437510();
	// !DECL 0x00437510 END
};

#endif
