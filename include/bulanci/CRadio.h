#ifndef _C_RADIO
#define _C_RADIO

#include <globals.h>

class CRadio {
public:
	// !DECL 0x00402fb0 BEGIN
	/* 402FB0 */ uchar CRadio_OnMouseDown(uint param_1, short param_2);
	// !DECL 0x00402fb0 END
	// !DECL 0x00403010 BEGIN
	/* 403010 */ uint CRadio_IsRadioMarker();
	// !DECL 0x00403010 END
	// !DECL 0x00403020 BEGIN
	/* 403020 */ uchar CRadio_Invalidate(int* param_1);
	// !DECL 0x00403020 END
	// !DECL 0x00403ac0 BEGIN
	/* 403AC0 */ uchar CRadio_Render(int param_1);
	// !DECL 0x00403ac0 END
	// !DECL 0x00403cc0 BEGIN
	/* 403CC0 */ uchar CRadio_SetSelected(char param_1);
	// !DECL 0x00403cc0 END
	// !DECL 0x00403d10 BEGIN
	/* 403D10 */ uchar CRadio_SetSelectedFromPtr(char* param_1);
	// !DECL 0x00403d10 END
	// !DECL 0x00405240 BEGIN
	/* 405240 */ uchar CRadio_OnMouseUp(uint param_1, uchar param_2);
	// !DECL 0x00405240 END
	// !DECL 0x00407300 BEGIN
	/* 407300 */ uint* CRadio_ctor(uint* param_1);
	// !DECL 0x00407300 END
	// !DECL 0x004073a0 BEGIN
	/* 4073A0 */ uchar* CRadio_GetTypeDescriptor();
	// !DECL 0x004073a0 END
	// !DECL 0x004073b0 BEGIN
	/* 4073B0 */ uchar CRadio_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x004073b0 END
	// !DECL 0x004073c0 BEGIN
	/* 4073C0 */ uchar CRadio_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x004073c0 END
	// !DECL 0x004073d0 BEGIN
	/* 4073D0 */ uchar CRadio_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x004073d0 END
	// !DECL 0x00407500 BEGIN
	/* 407500 */ uchar CRadio_dtor(uint* param_1);
	// !DECL 0x00407500 END
	// !DECL 0x00407f30 BEGIN
	/* 407F30 */ uint* CRadio_vDtor(uchar param_1);
	// !DECL 0x00407f30 END
	// !DECL 0x00408540 BEGIN
	/* 408540 */ uint* CRadio_BuildAt(uint param_1, uint param_2);
	// !DECL 0x00408540 END
	// !DECL 0x0040aa00 BEGIN
	/* 40AA00 */ uchar FUN_0040aa00(uchar* param_1);
	// !DECL 0x0040aa00 END
};

#endif
