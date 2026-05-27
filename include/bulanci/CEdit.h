#ifndef _C_EDIT
#define _C_EDIT

#include <globals.h>

class CEdit {
public:
	// !DECL 0x00403150 BEGIN
	/* 403150 */ uchar CEdit_GetTextLength(int param_1);
	// !DECL 0x00403150 END
	// !DECL 0x00403170 BEGIN
	/* 403170 */ uchar CEdit_SaveData(void* param_1);
	// !DECL 0x00403170 END
	// !DECL 0x00403190 BEGIN
	/* 403190 */ uint CEdit_GetDataSize();
	// !DECL 0x00403190 END
	// !DECL 0x00404530 BEGIN
	/* 404530 */ uchar CEdit_MeasureCharWidth(uchar param_1);
	// !DECL 0x00404530 END
	// !DECL 0x00404d10 BEGIN
	/* 404D10 */ uint* CEdit_ctor(uint* param_1);
	// !DECL 0x00404d10 END
	// !DECL 0x00404dc0 BEGIN
	/* 404DC0 */ uchar* CEdit_GetTypeDescriptor();
	// !DECL 0x00404dc0 END
	// !DECL 0x00404dd0 BEGIN
	/* 404DD0 */ uchar CEdit_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x00404dd0 END
	// !DECL 0x00404df0 BEGIN
	/* 404DF0 */ uchar CEdit_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x00404df0 END
	// !DECL 0x00406050 BEGIN
	/* 406050 */ uchar CEdit_dtor(uchar param_1);
	// !DECL 0x00406050 END
	// !DECL 0x00406100 BEGIN
	/* 406100 */ uchar CEdit_Render(int param_1);
	// !DECL 0x00406100 END
	// !DECL 0x00406230 BEGIN
	/* 406230 */ uchar CEdit_OnTimerTick(uchar param_1, uchar param_2);
	// !DECL 0x00406230 END
	// !DECL 0x00406270 BEGIN
	/* 406270 */ uint CEdit_SetText(int param_1);
	// !DECL 0x00406270 END
	// !DECL 0x00406390 BEGIN
	/* 406390 */ uchar CEdit_InsertTextAt(int* param_1, size_t param_2, int* param_3);
	// !DECL 0x00406390 END
	// !DECL 0x00406760 BEGIN
	/* 406760 */ uchar CEdit_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x00406760 END
	// !DECL 0x00406eb0 BEGIN
	/* 406EB0 */ uint CEdit_LayoutToCaret(uint param_1);
	// !DECL 0x00406eb0 END
	// !DECL 0x00407040 BEGIN
	/* 407040 */ uchar CEdit_OnFocusEvent(short param_1, void* param_2);
	// !DECL 0x00407040 END
	// !DECL 0x00407760 BEGIN
	/* 407760 */ void* CEdit_BuildAt(uint param_1, uint param_2, uint param_3, uint param_4, uint param_5, uint param_6, uint param_7, uint param_8);
	// !DECL 0x00407760 END

	CEdit(RECT rect, uint param_5, uint param_6, uint param_7, uint param_8) {
		CEdit_BuildAt(rect.left, rect.top, rect.right, rect.bottom, param_5, param_6, param_7, param_8);
	}

	// !DECL 0x004078f0 BEGIN
	/* 4078F0 */ char FUN_004078f0(uchar* param_1, uchar* param_2);
	// !DECL 0x004078f0 END
	// !DECL 0x00407b40 BEGIN
	/* 407B40 */ uchar CEdit_OnChar(uchar param_1);
	// !DECL 0x00407b40 END
	// !DECL 0x00407d20 BEGIN
	/* 407D20 */ uchar CEdit_LoadData(uint* param_1);
	// !DECL 0x00407d20 END
	// !DECL 0x0040ba00 BEGIN
	/* 40BA00 */ uchar FUN_0040ba00(uchar param_1);
	// !DECL 0x0040ba00 END
	// !DECL 0x0040ba10 BEGIN
	/* 40BA10 */ void* CEdit_vDtor(uchar param_1);
	// !DECL 0x0040ba10 END
};

#endif
