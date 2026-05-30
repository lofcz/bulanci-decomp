#ifndef _C_ICON
#define _C_ICON

#include <globals.h>

class CIcon {
public:
	// !DECL 0x00403070 BEGIN
	/* 403070 */ uchar CIcon_OnMouseDown(uint param_1, short param_2);
	// !DECL 0x00403070 END
	// !DECL 0x004030a0 BEGIN
	/* 4030A0 */ uchar CIcon_Invalidate(int* param_1);
	// !DECL 0x004030a0 END
	// !DECL 0x00403e40 BEGIN
	/* 403E40 */ void CIcon_Render(uchar param_1);
	// !DECL 0x00403e40 END
	// !DECL 0x00403ea0 BEGIN
	/* 403EA0 */ uchar CIcon_OnMouseUp(uint param_1, uchar param_2);
	// !DECL 0x00403ea0 END
	// !DECL 0x004049e0 BEGIN
	/* 4049E0 */ void* CIcon_ctor(void* param_1);
	// !DECL 0x004049e0 END
	// !DECL 0x00404a60 BEGIN
	/* 404A60 */ uchar* CIcon_GetTypeDescriptor();
	// !DECL 0x00404a60 END
	// !DECL 0x00404a70 BEGIN
	/* 404A70 */ uchar CIcon_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x00404a70 END
	// !DECL 0x00404a80 BEGIN
	/* 404A80 */ uchar CIcon_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x00404a80 END
	// !DECL 0x00404a90 BEGIN
	/* 404A90 */ uchar CIcon_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x00404a90 END
	// !DECL 0x00404aa0 BEGIN
	/* 404AA0 */ void CIcon_dtor(void* param_1);
	// !DECL 0x00404aa0 END
	// !DECL 0x004065e0 BEGIN
	/* 4065E0 */ uchar CIcon_vDtor(uchar param_1);
	// !DECL 0x004065e0 END
	// !DECL 0x004086e0 BEGIN
	/* 4086E0 */ uchar CIcon_BuildAt(uint param_1, uint param_2, ushort param_3, int param_4);
	// !DECL 0x004086e0 END
};

#endif
