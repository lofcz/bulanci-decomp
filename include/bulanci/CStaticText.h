#ifndef _C_STATIC_TEXT
#define _C_STATIC_TEXT

#include <globals.h>

#include "CBulanci.h"

class CStaticText {
public:
	// !DECL 0x00403040 BEGIN
	/* 403040 */ uchar CStaticText_SetStyle(void* param_1, int param_2);
	// !DECL 0x00403040 END
	// !DECL 0x00404910 BEGIN
	/* 404910 */ uint* CStaticText_ctor(uint* param_1);
	// !DECL 0x00404910 END
	// !DECL 0x00404970 BEGIN
	/* 404970 */ uchar* CStaticText_GetTypeDescriptor();
	// !DECL 0x00404970 END
	// !DECL 0x00404980 BEGIN
	/* 404980 */ uchar CStaticText_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x00404980 END
	// !DECL 0x00404990 BEGIN
	/* 404990 */ uchar CStaticText_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x00404990 END
	// !DECL 0x004049a0 BEGIN
	/* 4049A0 */ uchar CStaticText_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x004049a0 END
	// !DECL 0x004052e0 BEGIN
	/* 4052E0 */ uchar CStaticText_dtor(uint* param_1);
	// !DECL 0x004052e0 END
	// !DECL 0x004054d0 BEGIN
	/* 4054D0 */ uchar CStaticText_Render(int param_1);
	// !DECL 0x004054d0 END
	// !DECL 0x004064c0 BEGIN
	/* 4064C0 */ uint* CStaticText_vDtor(uchar param_1);
	// !DECL 0x004064c0 END
	// !DECL 0x00406a50 BEGIN
	/* 406A50 */ uint* CStaticText_BuildAtAuto(int param_1, uint param_2, uint param_3, uint param_4, uint param_5);
	// !DECL 0x00406a50 END
	// !DECL 0x00406ba0 BEGIN
	/* 406BA0 */ uint* CStaticText_BuildAt(uint param_1, uint param_2, int param_3, uint param_4, CBulanci param_5, uint param_6, uint param_7, uint param_8);
	// !DECL 0x00406ba0 END

	CStaticText(RECT rect, CBulanci text, uint param_6, uint param_7, uint param_8, uint param_9) {
		CStaticText_BuildAt(rect.left, rect.top, rect.right, rect.bottom, text, param_6, param_7, param_8);
	}
};

#endif
