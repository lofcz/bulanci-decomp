#ifndef _C_SCROLLER
#define _C_SCROLLER

#include <globals.h>

class CScroller {
public:
	// !DECL 0x00403120 BEGIN
	/* 403120 */ uchar CScroller_IDSUpdated_ArmSlot(void* param_1);
	// !DECL 0x00403120 END
	// !DECL 0x004033d0 BEGIN
	/* 4033D0 */ uchar CScroller_ctor(uchar param_1);
	// !DECL 0x004033d0 END
	// !DECL 0x00403470 BEGIN
	/* 403470 */ uchar* CScroller_GetClassTable();
	// !DECL 0x00403470 END
	// !DECL 0x00403480 BEGIN
	/* 403480 */ uchar CScroller_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x00403480 END
	// !DECL 0x00403490 BEGIN
	/* 403490 */ uchar CScroller_ScalarDeletingDtor_thunk_Sub68(uchar param_1);
	// !DECL 0x00403490 END
	// !DECL 0x004034b0 BEGIN
	/* 4034B0 */ uchar CScroller_ScalarDeletingDtor_thunk_004034b0(uchar param_1);
	// !DECL 0x004034b0 END
	// !DECL 0x004034c0 BEGIN
	/* 4034C0 */ uchar CScroller_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x004034c0 END
	// !DECL 0x004034d0 BEGIN
	/* 4034D0 */ uchar* CScroller_GetTypeInfo();
	// !DECL 0x004034d0 END
	// !DECL 0x004034e0 BEGIN
	/* 4034E0 */ void* CScroller_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004034e0 END
	// !DECL 0x004040c0 BEGIN
	/* 4040C0 */ uchar CScroller_ResetScrollSchedulerSlot(int param_1);
	// !DECL 0x004040c0 END
	// !DECL 0x00404110 BEGIN
	/* 404110 */ uchar CScroller_OnViewEvent(short param_1, int param_2);
	// !DECL 0x00404110 END
	// !DECL 0x00404de0 BEGIN
	/* 404DE0 */ uchar CDSChain_AdjustThisOffset(int param_1);
	// !DECL 0x00404de0 END
	// !DECL 0x00405110 BEGIN
	/* 405110 */ uint CScroller_HandlePageKeys(char param_1);
	// !DECL 0x00405110 END
	// !DECL 0x004057c0 BEGIN
	/* 4057C0 */ uchar CScroller_Render(CListViewer* param_1);
	// !DECL 0x004057c0 END
	// !DECL 0x00405930 BEGIN
	/* 405930 */ bool CScroller_OnKeyDown(char param_1);
	// !DECL 0x00405930 END
	// !DECL 0x00405af0 BEGIN
	/* 405AF0 */ uchar CScroller_SetRect(int* param_1);
	// !DECL 0x00405af0 END
	// !DECL 0x004069b0 BEGIN
	/* 4069B0 */ uchar CScroller_OnKeyUp(char param_1);
	// !DECL 0x004069b0 END
	// !DECL 0x00406cc0 BEGIN
	/* 406CC0 */ bool CScroller_OnKeyUp_00406cc0(char param_1);
	// !DECL 0x00406cc0 END
	// !DECL 0x004089c0 BEGIN
	/* 4089C0 */ void* CScroller_BuildAt(int param_1, int param_2, uint param_3, int param_4);
	// !DECL 0x004089c0 END
	// !DECL 0x004229a0 BEGIN
	/* 4229A0 */ uchar CDSObject_ReleaseViaVtable(int param_1);
	// !DECL 0x004229a0 END
	// !DECL 0x0042cbb0 BEGIN
	/* 42CBB0 */ uchar CScroller_OffsetChildViewRect(int param_1, int param_2);
	// !DECL 0x0042cbb0 END
};

#endif
