#ifndef _C_D_S_STRM_RES_INFO
#define _C_D_S_STRM_RES_INFO

#include <globals.h>

class CDSStrmResInfo {
public:
	// !DECL 0x00433940 BEGIN
	/* 433940 */ uchar CDSStrmResInfo_Serialize(int* param_1);
	// !DECL 0x00433940 END
	// !DECL 0x00433980 BEGIN
	/* 433980 */ void CDSStrmResInfo_Deserialize(uchar param_1);
	// !DECL 0x00433980 END
	// !DECL 0x00433a90 BEGIN
	/* 433A90 */ uchar* CDSStrmResInfo_GetTypeInfo();
	// !DECL 0x00433a90 END
};

#endif
