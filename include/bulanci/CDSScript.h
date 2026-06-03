#ifndef _C_D_S_SCRIPT
#define _C_D_S_SCRIPT

#include <globals.h>

class CDSScript {
public:
	// !DECL 0x00438310 BEGIN
	/* 438310 */ void CDSScript_InstallOpcodeTable(int param_1, void* param_2, int param_3);
	// !DECL 0x00438310 END
	// !DECL 0x00438350 BEGIN
	/* 438350 */ uint ReadI32(int param_1);
	// !DECL 0x00438350 END
	// !DECL 0x00438360 BEGIN
	/* 438360 */ ushort ReadU16(int param_1);
	// !DECL 0x00438360 END
	// !DECL 0x00438380 BEGIN
	/* 438380 */ uint ReadU8(void* param_1);
	// !DECL 0x00438380 END
	// !DECL 0x00438390 BEGIN
	/* 438390 */ void* ctor(void* param_1);
	// !DECL 0x00438390 END
	// !DECL 0x004383e0 BEGIN
	/* 4383E0 */ uchar* CDSScript_GetTypeInfo();
	// !DECL 0x004383e0 END
	// !DECL 0x004383f0 BEGIN
	/* 4383F0 */ uchar CDSScript_AdjustOffsetDtor(uchar param_1);
	// !DECL 0x004383f0 END
	// !DECL 0x00438400 BEGIN
	/* 438400 */ void CDSScript_dtor(void* param_1);
	// !DECL 0x00438400 END
	// !DECL 0x004384c0 BEGIN
	/* 4384C0 */ void ReadSubExpr(void* param_1);
	// !DECL 0x004384c0 END
	// !DECL 0x00438c40 BEGIN
	/* 438C40 */ uint CallExport(int param_1, int param_2, void* param_3);
	// !DECL 0x00438c40 END
	// !DECL 0x00438c90 BEGIN
	/* 438C90 */ void* CDSScript_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00438c90 END
};

#endif
