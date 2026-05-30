#ifndef _C_D_S_VIEW
#define _C_D_S_VIEW

#include <globals.h>

class CDSView {
public:
	// !DECL 0x00403390 BEGIN
	/* 403390 */ uchar* CDSView_GetClassTable();
	// !DECL 0x00403390 END
	// !DECL 0x004034a0 BEGIN
	/* 4034A0 */ uchar CDSObject_ReleaseViaVtable_ThisMinus0x10(int param_1);
	// !DECL 0x004034a0 END
	// !DECL 0x0040ad90 BEGIN
	/* 40AD90 */ uchar CDSView_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x0040ad90 END
	// !DECL 0x0040ada0 BEGIN
	/* 40ADA0 */ void* CDSView_DtorScalar(uchar param_1);
	// !DECL 0x0040ada0 END
	// !DECL 0x00416f40 BEGIN
	/* 416F40 */ uchar CDSView_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x00416f40 END
	// !DECL 0x00421910 BEGIN
	/* 421910 */ uchar CDSView_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x00421910 END
	// !DECL 0x00423db0 BEGIN
	/* 423DB0 */ uchar CDSObject_ReleaseViaVtable_ThisMinus18(int param_1);
	// !DECL 0x00423db0 END
	// !DECL 0x0042c040 BEGIN
	/* 42C040 */ void CDSView_DispatchEvent(CDSEventRecord* param_1);
	// !DECL 0x0042c040 END
	// !DECL 0x0042c0c0 BEGIN
	/* 42C0C0 */ uint CDSView_OnKeyDown(int param_1);
	// !DECL 0x0042c0c0 END
	// !DECL 0x0042ca30 BEGIN
	/* 42CA30 */ uchar CDSView_InvalidateRectClipped(int* param_1, uint param_2);
	// !DECL 0x0042ca30 END
	// !DECL 0x0042cea0 BEGIN
	/* 42CEA0 */ void CDSView_dtor(void* param_1, uint param_2);
	// !DECL 0x0042cea0 END
	// !DECL 0x0042cf50 BEGIN
	/* 42CF50 */ uchar CDSView_OnLButtonDownAcquireFocus(int* param_1);
	// !DECL 0x0042cf50 END

	// !DECL 0x0042bfc0 BEGIN
	/* 42BFC0 */ uchar AddChildInternal(uchar param_1, int param_2);
	// !DECL 0x0042bfc0 END
	// !DECL 0x0042c3c0 BEGIN
	/* 42C3C0 */ uchar EndModal(ushort param_1);
	// !DECL 0x0042c3c0 END
	// !DECL 0x0042c990 BEGIN
	/* 42C990 */ uchar Show(int* param_1);
	// !DECL 0x0042c990 END
	// !DECL 0x0042d040 BEGIN
	/* 42D040 */ uchar Hide(int* param_1);
	// !DECL 0x0042d040 END
	// !DECL 0x0042d0b0 BEGIN
	/* 42D0B0 */ uchar AddChild(uchar param_1, int param_2);
	// !DECL 0x0042d0b0 END
};

#endif
