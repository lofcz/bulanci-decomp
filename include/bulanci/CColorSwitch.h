#ifndef _C_COLOR_SWITCH
#define _C_COLOR_SWITCH

#include <globals.h>

class CColorSwitch {
public:
	// !DECL 0x0040aa10 BEGIN
	/* 40AA10 */ uchar CColorSwitch_OnEvent(short param_1);
	// !DECL 0x0040aa10 END
	// !DECL 0x0040aa40 BEGIN
	/* 40AA40 */ uchar CColorSwitch_SetSelected(int param_1);
	// !DECL 0x0040aa40 END
	// !DECL 0x0040aa60 BEGIN
	/* 40AA60 */ uchar CColorSwitch_SetSelectedFromPtr(void* param_1);
	// !DECL 0x0040aa60 END
	// !DECL 0x0040b950 BEGIN
	/* 40B950 */ uchar* CColorSwitch_GetTypeDescriptor();
	// !DECL 0x0040b950 END
	// !DECL 0x0040b960 BEGIN
	/* 40B960 */ uchar CColorSwitch_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x0040b960 END
	// !DECL 0x0040b970 BEGIN
	/* 40B970 */ uchar CColorSwitch_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x0040b970 END
	// !DECL 0x0040b980 BEGIN
	/* 40B980 */ uchar CColorSwitch_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x0040b980 END
	// !DECL 0x0040b990 BEGIN
	/* 40B990 */ uchar CColorSwitch_dtor(uchar param_1);
	// !DECL 0x0040b990 END
	// !DECL 0x0040ccd0 BEGIN
	/* 40CCD0 */ uchar CColorSwitch_Render(int param_1);
	// !DECL 0x0040ccd0 END
	// !DECL 0x0040ce00 BEGIN
	/* 40CE00 */ uchar CColorSwitch_OnMouseClick(int* param_1);
	// !DECL 0x0040ce00 END
	// !DECL 0x0040ebf0 BEGIN
	/* 40EBF0 */ uint* CColorSwitch_ctor();
	// !DECL 0x0040ebf0 END
	// !DECL 0x0040ec80 BEGIN
	/* 40EC80 */ void* CColorSwitch_vDtor(uchar param_1);
	// !DECL 0x0040ec80 END
	// !DECL 0x004100d0 BEGIN
	/* 4100D0 */ void* CColorSwitch_BuildAt(uint param_1, uint param_2, uint param_3);
	// !DECL 0x004100d0 END
};

#endif
