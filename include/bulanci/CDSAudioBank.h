#ifndef _C_D_S_AUDIO_BANK
#define _C_D_S_AUDIO_BANK

#include <globals.h>

class CDSAudioBank {
public:
	// !DECL 0x00406340 BEGIN
	/* 406340 */ uchar FUN_00406340(int param_1);
	// !DECL 0x00406340 END
	// !DECL 0x00429240 BEGIN
	/* 429240 */ uchar FUN_00429240(int param_1);
	// !DECL 0x00429240 END
	// !DECL 0x004292f0 BEGIN
	/* 4292F0 */ uchar* FUN_004292f0();
	// !DECL 0x004292f0 END
	// !DECL 0x00429310 BEGIN
	/* 429310 */ uchar FUN_00429310(uchar param_1);
	// !DECL 0x00429310 END
	// !DECL 0x00429320 BEGIN
	/* 429320 */ uchar FUN_00429320(uchar param_1);
	// !DECL 0x00429320 END
	// !DECL 0x00429330 BEGIN
	/* 429330 */ uchar FUN_00429330(uchar param_1);
	// !DECL 0x00429330 END
	// !DECL 0x004293e0 BEGIN
	/* 4293E0 */ uchar FUN_004293e0(uint* param_1);
	// !DECL 0x004293e0 END
	// !DECL 0x004294b0 BEGIN
	/* 4294B0 */ uint* FUN_004294b0(uchar param_1);
	// !DECL 0x004294b0 END
	// !DECL 0x00429530 BEGIN
	/* 429530 */ uint* FUN_00429530(int* param_1, uint param_2, uchar* param_3);
	// !DECL 0x00429530 END
	// !DECL 0x00429600 BEGIN
	/* 429600 */ uchar FUN_00429600(int* param_1);
	// !DECL 0x00429600 END
	// !DECL 0x0042985a BEGIN
	/* 42985A */ uchar Catch_0042985a();
	// !DECL 0x0042985a END
};

#endif
