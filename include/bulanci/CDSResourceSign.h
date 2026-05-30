#ifndef _C_D_S_RESOURCE_SIGN
#define _C_D_S_RESOURCE_SIGN

#include <globals.h>

class CDSResourceSign {
public:
	// !DECL 0x0042e680 BEGIN
	/* 42E680 */ uchar CDSResourceSign_ReadDateField(int* param_1);
	// !DECL 0x0042e680 END
	// !DECL 0x0042e6a0 BEGIN
	/* 42E6A0 */ uchar CDSResourceSign_WriteDateField(int* param_1);
	// !DECL 0x0042e6a0 END
	// !DECL 0x00434310 BEGIN
	/* 434310 */ void CDSResourceSign_WriteToStream(CDSFilterStream* param_1);
	// !DECL 0x00434310 END
	// !DECL 0x004343a0 BEGIN
	/* 4343A0 */ uchar CDSResourceSign_ReadFromStream(int* param_1);
	// !DECL 0x004343a0 END
	// !DECL 0x00434540 BEGIN
	/* 434540 */ uint* CDSResourceSign_ctor(uint* param_1);
	// !DECL 0x00434540 END
	// !DECL 0x004345c0 BEGIN
	/* 4345C0 */ uchar* CDSResourceSign_GetClassData();
	// !DECL 0x004345c0 END
	// !DECL 0x004345d0 BEGIN
	/* 4345D0 */ uchar CDSResourceSign_scalar_deleting_dtor_thunk_n0x4(uchar param_1);
	// !DECL 0x004345d0 END
	// !DECL 0x004345e0 BEGIN
	/* 4345E0 */ uchar CDSResourceSign_scalar_deleting_dtor_thunk_n0x8(uchar param_1);
	// !DECL 0x004345e0 END
	// !DECL 0x004345f0 BEGIN
	/* 4345F0 */ uchar CDSResourceSign_scalar_deleting_dtor_thunk_n0x10(uchar param_1);
	// !DECL 0x004345f0 END
	// !DECL 0x00434610 BEGIN
	/* 434610 */ uchar CDSResourceSign_Release(int param_1);
	// !DECL 0x00434610 END
	// !DECL 0x00434640 BEGIN
	/* 434640 */ uchar CDSResourceSign_Release_thunk_n0x4(int param_1);
	// !DECL 0x00434640 END
	// !DECL 0x00434650 BEGIN
	/* 434650 */ uchar CDSResourceSign_Release_thunk_n0x8(int param_1);
	// !DECL 0x00434650 END
	// !DECL 0x00434660 BEGIN
	/* 434660 */ uchar CDSResourceSign_Release_thunk_n0x10(int param_1);
	// !DECL 0x00434660 END
	// !DECL 0x00434670 BEGIN
	/* 434670 */ uchar CDSResourceSign_dtor(uint* param_1);
	// !DECL 0x00434670 END
	// !DECL 0x004349a0 BEGIN
	/* 4349A0 */ void* CDSResourceSign_scalar_deleting_dtor(uchar param_1);
	// !DECL 0x004349a0 END

	// !DECL 0x0047d1d0 BEGIN
	/* 47D1D0 */ uchar RegisterCDSResourceSignAsClass94();
	// !DECL 0x0047d1d0 END
	// !DECL 0x0047d200 BEGIN
	/* 47D200 */ uchar RegisterCDSResourceSignAuxClass1();
	// !DECL 0x0047d200 END
	// !DECL 0x0047d230 BEGIN
	/* 47D230 */ uchar RegisterCDSResourceSignAuxClass2();
	// !DECL 0x0047d230 END
	// !DECL 0x0047d260 BEGIN
	/* 47D260 */ uchar RegisterCDSResourceSignAuxClass3();
	// !DECL 0x0047d260 END
};

#endif
