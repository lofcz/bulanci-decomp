#ifndef _C_D_S_AUDIO_BANK
#define _C_D_S_AUDIO_BANK

#include <globals.h>

class CDSAudioBank {
public:
	/* 4292F0 */ uchar* FUN_004292f0();
	/* 429310 */ uchar FUN_00429310(uchar param_1);
	/* 429320 */ uchar FUN_00429320(uchar param_1);
	/* 429330 */ uchar FUN_00429330(uchar param_1);
	/* 4294B0 */ uint* FUN_004294b0(uchar param_1);
	/* 429600 */ uchar FUN_00429600(int* param_1);
};

#endif