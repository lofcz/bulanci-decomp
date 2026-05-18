#ifndef _C_RADIO
#define _C_RADIO

#include <globals.h>

class CRadio {
public:
	/* 402FB0 */ uchar FUN_00402fb0(uint param_1, short param_2);
	/* 403010 */ uint FUN_00403010();
	/* 403020 */ uchar FUN_00403020(int* param_1);
	/* 403AC0 */ uchar FUN_00403ac0(int param_1);
	/* 403D10 */ uchar FUN_00403d10(char* param_1);
	/* 405240 */ uchar FUN_00405240(uint param_1, uchar param_2);
	/* 4073A0 */ uchar* FUN_004073a0();
	/* 4073B0 */ uchar FUN_004073b0(uchar param_1);
	/* 4073C0 */ uchar FUN_004073c0(uchar param_1);
	/* 4073D0 */ uchar FUN_004073d0(uchar param_1);
	/* 407F30 */ uint* FUN_00407f30(uchar param_1);
	/* 40AA00 */ uchar FUN_0040aa00(uchar* param_1);
};

#endif