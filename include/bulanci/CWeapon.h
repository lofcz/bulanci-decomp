#ifndef _C_WEAPON
#define _C_WEAPON

#include <globals.h>

class CWeapon {
public:
	/* 416750 */ uchar FUN_00416750(int* param_1);
	/* 417A00 */ uchar FUN_00417a00(uint param_1, uint param_2, int param_3);
	/* 418F60 */ uchar* FUN_00418f60();
	/* 41BF00 */ uchar FUN_0041bf00(int* param_1);
	/* 41BF70 */ uchar FUN_0041bf70(int param_1);
	/* 41C550 */ uchar FUN_0041c550(uint* param_1);
	/* 41C600 */ uchar FUN_0041c600(uint param_1, int* param_2);
	/* 41CB70 */ uint* FUN_0041cb70(int param_1);
	/* 41DAE0 */ uint* FUN_0041dae0(uchar param_1);
	/* 4212B0 */ uchar FUN_004212b0(uint param_1, ushort param_2);
};

#endif