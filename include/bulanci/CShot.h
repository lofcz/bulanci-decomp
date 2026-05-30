#ifndef _C_SHOT
#define _C_SHOT

#include <globals.h>

class CShot {
public:
	// !DECL 0x004170e0 BEGIN
	/* 4170E0 */ uchar* CShot_GetClassTable();
	// !DECL 0x004170e0 END
	// !DECL 0x004170f0 BEGIN
	/* 4170F0 */ void CShot_dtor(uchar param_1);
	// !DECL 0x004170f0 END
	// !DECL 0x00417150 BEGIN
	/* 417150 */ uchar CShot_DtorScalar_thunk_Sub18(uchar param_1);
	// !DECL 0x00417150 END
	// !DECL 0x00417160 BEGIN
	/* 417160 */ uchar CShot_DtorScalar_thunk_Sub4(uchar param_1);
	// !DECL 0x00417160 END
	// !DECL 0x00417170 BEGIN
	/* 417170 */ uchar CShot_DtorScalar_thunk_Sub88(uchar param_1);
	// !DECL 0x00417170 END
	// !DECL 0x00417180 BEGIN
	/* 417180 */ uchar CShot_DtorScalar_thunk_Sub10(uchar param_1);
	// !DECL 0x00417180 END
	// !DECL 0x00417190 BEGIN
	/* 417190 */ uchar CShot_DtorScalar(uchar param_1);
	// !DECL 0x00417190 END
	// !DECL 0x00417b30 BEGIN
	/* 417B30 */ uchar CShot_ComputePelletRect(int* param_1, int* param_2, int param_3);
	// !DECL 0x00417b30 END
	// !DECL 0x00417bd0 BEGIN
	/* 417BD0 */ uchar CShot_Draw(uchar param_1);
	// !DECL 0x00417bd0 END
	// !DECL 0x00418ed0 BEGIN
	/* 418ED0 */ uchar CShot_ReleaseViaVtable_thunk_Sub88(int param_1);
	// !DECL 0x00418ed0 END
	// !DECL 0x00419b40 BEGIN
	/* 419B40 */ uchar FUN_00419b40(void* param_1);
	// !DECL 0x00419b40 END
	// !DECL 0x0041a980 BEGIN
	/* 41A980 */ uchar CDSChain_AdjustThisOffset_ThisMinus84(int param_1);
	// !DECL 0x0041a980 END
	// !DECL 0x0041b1d0 BEGIN
	/* 41B1D0 */ uchar CShot_SchedulerTick(void* param_1);
	// !DECL 0x0041b1d0 END
	// !DECL 0x0041de60 BEGIN
	/* 41DE60 */ uint TraceCollision(int* param_1, int* param_2, uchar* param_3);
	// !DECL 0x0041de60 END
	// !DECL 0x0041df60 BEGIN
	/* 41DF60 */ uchar CShot_Update(int* param_1);
	// !DECL 0x0041df60 END
	// !DECL 0x0041edf0 BEGIN
	/* 41EDF0 */ uint CShot_Ctor(uint* param_1, CGameView* param_2, uchar param_3, uchar param_4, uchar param_5, int param_6);
	// !DECL 0x0041edf0 END
};

#endif
