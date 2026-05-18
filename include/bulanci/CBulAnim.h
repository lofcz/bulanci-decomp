#ifndef _C_BUL_ANIM
#define _C_BUL_ANIM

#include <globals.h>

class CBulAnim {
public:
	/* 40B020 */ uchar FUN_0040b020(short param_1, uint param_2);
	/* 40B820 */ uchar* FUN_0040b820();
	/* 40B830 */ uchar* FUN_0040b830();
	/* 40B840 */ uchar FUN_0040b840(int param_1);
	/* 40B850 */ uchar FUN_0040b850(char param_1);
	/* 40B870 */ uchar* FUN_0040b870();
	/* 40B880 */ uchar FUN_0040b880(int param_1);
	/* 40B890 */ uchar FUN_0040b890(uchar param_1);
	/* 40B8A0 */ uchar FUN_0040b8a0(uchar param_1);
	/* 438F20 */ uchar FUN_00438f20(uint param_1, int param_2, int param_3);
	/* 438F60 */ uchar FUN_00438f60(uint param_1, int* param_2);
	/* 4391D0 */ uchar FUN_004391d0(uint param_1, int* param_2);
	/* 4392A0 */ uchar FUN_004392a0(uint param_1, short param_2);
	/* 439520 */ uchar FUN_00439520(uchar param_1);
	/* 439530 */ uchar FUN_00439530(uchar param_1);
	/* 439540 */ uchar FUN_00439540(uchar param_1);
	/* 439550 */ uchar FUN_00439550(int param_1);
	/* 4396F0 */ uint* FUN_004396f0(uchar param_1);
};

#endif