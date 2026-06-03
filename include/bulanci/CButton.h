#ifndef _C_BUTTON
#define _C_BUTTON

#include <globals.h>

#include "CBulanci.h"

class CButton {
public:
	// !DECL 0x004030b0 BEGIN
	/* 4030B0 */ uchar CButton_PlayClickSound(int param_1);
	// !DECL 0x004030b0 END
	// !DECL 0x004030e0 BEGIN
	/* 4030E0 */ uchar CButton_OnMouseDown(uint param_1, short param_2);
	// !DECL 0x004030e0 END
	// !DECL 0x00403f20 BEGIN
	/* 403F20 */ uchar CButton_OnMouseUp(uint param_1, uchar param_2);
	// !DECL 0x00403f20 END
	// !DECL 0x00403f90 BEGIN
	/* 403F90 */ uint CButton_OnKeyDown(uchar param_1, char param_2);
	// !DECL 0x00403f90 END
	// !DECL 0x00404060 BEGIN
	/* 404060 */ uchar CButton_OnFocusReceived(short param_1);
	// !DECL 0x00404060 END
	// !DECL 0x00404b10 BEGIN
	/* 404B10 */ void* CButton_ctor(void* param_1);
	// !DECL 0x00404b10 END
	// !DECL 0x00404ba0 BEGIN
	/* 404BA0 */ uchar* CButton_GetTypeDescriptor();
	// !DECL 0x00404ba0 END
	// !DECL 0x00404bb0 BEGIN
	/* 404BB0 */ uchar CButton_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x00404bb0 END
	// !DECL 0x00404bc0 BEGIN
	/* 404BC0 */ uchar CButton_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x00404bc0 END
	// !DECL 0x00404bd0 BEGIN
	/* 404BD0 */ uchar CButton_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x00404bd0 END
	// !DECL 0x00405600 BEGIN
	/* 405600 */ void CButton_dtor(uchar param_1);
	// !DECL 0x00405600 END
	// !DECL 0x004056b0 BEGIN
	/* 4056B0 */ void CButton_Render(uchar param_1);
	// !DECL 0x004056b0 END
	// !DECL 0x00406670 BEGIN
	/* 406670 */ void* CButton_vDtor(uchar param_1);
	// !DECL 0x00406670 END
	// !DECL 0x004087f0 BEGIN
	/* 4087F0 */ uint* CButton_BuildAt(int param_1, ushort param_2, uchar param_3, uint param_4, uint param_5, uint param_6, uint* param_7);
	// !DECL 0x004087f0 END

	CButton(int param_1, ushort param_2, CBulanci param_3, uint param_4, uint param_5, uint param_6, uint* param_7) {
		CButton_BuildAt(param_1, param_2, param_3, param_4, param_5, param_6, param_7);
	}
};

#endif
