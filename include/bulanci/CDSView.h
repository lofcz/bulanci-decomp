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
	/* 42CEA0 */ void CDSView_dtor(uchar param_1, uint param_2);
	// !DECL 0x0042cea0 END
	// !DECL 0x0042cf50 BEGIN
	/* 42CF50 */ void CDSView_OnLButtonDownAcquireFocus();
	// !DECL 0x0042cf50 END

	// !DECL 0x0042bfc0 BEGIN
	/* 42BFC0 */ uchar AddChildInternal(uchar param_1, void* param_2);
	// !DECL 0x0042bfc0 END
	// !DECL 0x0042c3c0 BEGIN
	/* 42C3C0 */ uchar EndModal(ushort param_1);
	// !DECL 0x0042c3c0 END
	// !DECL 0x0042c990 BEGIN
	/* 42C990 */ uchar Show(int* param_1);
	// !DECL 0x0042c990 END
	// !DECL 0x0042d0b0 BEGIN
	/* 42D0B0 */ uchar CDSView_AddChild(uchar param_1, void* param_2);
	// !DECL 0x0042d0b0 END

	// !DECL 0x0042c120 BEGIN
	/* 42C120 */ void CDSView_SetMouseMoveDefault();
	// !DECL 0x0042c120 END
	// !DECL 0x0042c1c0 BEGIN
	/* 42C1C0 */ int* CDSView_ResolveInputChainFromHitTest(uint param_1);
	// !DECL 0x0042c1c0 END
	// !DECL 0x0042c430 BEGIN
	/* 42C430 */ void CDSView_GetParentBounds(uint* param_1, int* param_2);
	// !DECL 0x0042c430 END
	// !DECL 0x0042c480 BEGIN
	/* 42C480 */ void CDSView_SetRect(int* param_1);
	// !DECL 0x0042c480 END
	// !DECL 0x0042c580 BEGIN
	/* 42C580 */ void CDSView_ComputeAnchoredRect(int* param_1, int* param_2);
	// !DECL 0x0042c580 END
	// !DECL 0x0042c770 BEGIN
	/* 42C770 */ uchar CDSApp_BroadcastSyntheticEventToChildren(ushort param_1, uint param_2, uint param_3);
	// !DECL 0x0042c770 END
	// !DECL 0x0042c7d0 BEGIN
	/* 42C7D0 */ uchar CDSApp_RouteSyntheticCloseEvent(ushort param_1);
	// !DECL 0x0042c7d0 END
	// !DECL 0x0042cae0 BEGIN
	/* 42CAE0 */ uint CDSView_AdaptDisplaySize(uchar param_1);
	// !DECL 0x0042cae0 END
	// !DECL 0x0042ccf0 BEGIN
	/* 42CCF0 */ void CDSView_RenderChildrenClipped();
	// !DECL 0x0042ccf0 END
	// !DECL 0x0042cf60 BEGIN
	/* 42CF60 */ void CDSView_OnMouseUpModalInputRefresh();
	// !DECL 0x0042cf60 END
	// !DECL 0x0042cfa0 BEGIN
	/* 42CFA0 */ void CDSView_RefreshModalFocusFromChildren();
	// !DECL 0x0042cfa0 END
	// !DECL 0x0042cff0 BEGIN
	/* 42CFF0 */ void CDSView_TeardownModalFocusChain();
	// !DECL 0x0042cff0 END
	// !DECL 0x0042d080 BEGIN
	/* 42D080 */ void CDSView_SetAsDefaultFocusChild();
	// !DECL 0x0042d080 END
	// !DECL 0x0042d1a0 BEGIN
	/* 42D1A0 */ ushort CDSView_DoModal(void* param_1);
	// !DECL 0x0042d1a0 END
	// !DECL 0x0042ec90 BEGIN
	/* 42EC90 */ uint IDSEventHandler_DispatchIfMask(uint* param_1);
	// !DECL 0x0042ec90 END
};

#endif
