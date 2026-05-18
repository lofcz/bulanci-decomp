#ifndef _C_D_S_MPX
#define _C_D_S_MPX

#include <globals.h>

class CDSMpx {
public:
	/* 432F10 */ uchar* FUN_00432f10();
	/* 432F20 */ uchar FUN_00432f20(uchar param_1);
	/* 432F30 */ uchar FUN_00432f30(uchar param_1);
	/* 432FC0 */ uint* FUN_00432fc0(uchar param_1);
	/* 4466A0 */ uchar FUN_004466a0(int* param_1);
	/* 4469A0 */ uchar FUN_004469a0(uint* param_1, void* param_2, void* param_3);
	/* 446B00 */ uint* FUN_00446b00(int param_1);
	/* 446B90 */ uchar FUN_00446b90(int param_1);
};

#endif