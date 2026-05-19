#ifndef _C_D_S_DIRECT_SOUND
#define _C_D_S_DIRECT_SOUND

#include <globals.h>

class CDSDirectSound {
public:
	/* 43A350 */ uchar FUN_0043a350(int param_1);
	/* 43C8A0 */ uchar* FUN_0043c8a0();
	/* 43CA50 */ uchar FUN_0043ca50(uchar param_1);
	/* 43CA60 */ uchar FUN_0043ca60(uchar param_1);
	/* 43CA70 */ uchar FUN_0043ca70(int param_1);
	/* 43CA80 */ uchar FUN_0043ca80(uchar param_1);
	/* 43CB00 */ uint* FUN_0043cb00(uchar param_1);
	/* 43CCB0 */ uchar FUN_0043ccb0(int param_1);
	/* 43CD00 */ uchar FUN_0043cd00(int param_1);
	/* 43CDC0 */ uchar FUN_0043cdc0(int* param_1);
};

#endif