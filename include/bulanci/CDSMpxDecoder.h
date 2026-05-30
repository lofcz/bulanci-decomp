#ifndef _C_D_S_MPX_DECODER
#define _C_D_S_MPX_DECODER

#include <globals.h>

class CDSMpxDecoder {
public:
	// !DECL 0x004468b0 BEGIN
	/* 4468B0 */ void CDSMpxDecoder_dtor(void* param_1);
	// !DECL 0x004468b0 END
	// !DECL 0x00446930 BEGIN
	/* 446930 */ uchar* CDSMpxDecoder_GetTypeInfo();
	// !DECL 0x00446930 END
	// !DECL 0x00446ae0 BEGIN
	/* 446AE0 */ void* CDSMpxDecoder_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00446ae0 END
};

#endif
