#ifndef _C_VOLUME
#define _C_VOLUME

#include <globals.h>

class CVolume {
public:
	/* 40BE30 */ uchar* FUN_0040be30();
	/* 40BE40 */ uchar FUN_0040be40(uchar param_1);
	/* 40BE50 */ uchar FUN_0040be50(uchar param_1);
	/* 40BE60 */ uchar FUN_0040be60(uchar param_1);
	/* 40BE70 */ uchar FUN_0040be70(uchar param_1);
	/* 40E230 */ uchar FUN_0040e230(int param_1);
	/* 40E270 */ uchar FUN_0040e270(uint param_1, uchar param_2);
	/* 40FA70 */ uchar FUN_0040fa70(uint* param_1);
	/* 40FB00 */ uchar FUN_0040fb00(int* param_1, short param_2);
	/* 40FC00 */ uint* FUN_0040fc00(uchar param_1);
};

#endif