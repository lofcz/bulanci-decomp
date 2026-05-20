#ifndef _C_VOLUME
#define _C_VOLUME

#include <globals.h>

class CVolume {
public:
	// !DECL 0x0040be30 BEGIN
	/* 40BE30 */ uchar* CVolume_GetTypeDescriptor();
	// !DECL 0x0040be30 END
	// !DECL 0x0040be40 BEGIN
	/* 40BE40 */ uchar CVolume_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x0040be40 END
	// !DECL 0x0040be50 BEGIN
	/* 40BE50 */ uchar CVolume_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x0040be50 END
	// !DECL 0x0040be60 BEGIN
	/* 40BE60 */ uchar CVolume_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x0040be60 END
	// !DECL 0x0040be70 BEGIN
	/* 40BE70 */ uchar CVolume_AdjustorThunk68_Dtor(uchar param_1);
	// !DECL 0x0040be70 END
	// !DECL 0x0040e1b0 BEGIN
	/* 40E1B0 */ uint* CVolume_BuildAt(int* param_1, uint* param_2);
	// !DECL 0x0040e1b0 END
	// !DECL 0x0040e230 BEGIN
	/* 40E230 */ uchar CVolume_StopAudioPreview(int param_1);
	// !DECL 0x0040e230 END
	// !DECL 0x0040e270 BEGIN
	/* 40E270 */ uchar CVolume_OnMouseUp(uint param_1, uchar param_2);
	// !DECL 0x0040e270 END
	// !DECL 0x0040f140 BEGIN
	/* 40F140 */ uint* CVolume_Allocate();
	// !DECL 0x0040f140 END
	// !DECL 0x0040fa70 BEGIN
	/* 40FA70 */ uchar CVolume_dtor(uint* param_1);
	// !DECL 0x0040fa70 END
	// !DECL 0x0040fb00 BEGIN
	/* 40FB00 */ uchar CVolume_OnMouseDown(int* param_1, short param_2);
	// !DECL 0x0040fb00 END
	// !DECL 0x0040fc00 BEGIN
	/* 40FC00 */ uint* CVolume_vDtor(uchar param_1);
	// !DECL 0x0040fc00 END
};

#endif
