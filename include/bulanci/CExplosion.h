#ifndef _C_EXPLOSION
#define _C_EXPLOSION

#include <globals.h>

class CExplosion {
public:
	// !DECL 0x0041aad0 BEGIN
	/* 41AAD0 */ uint* CExplosion_InitVtables(uint* param_1);
	// !DECL 0x0041aad0 END
	// !DECL 0x0041ab20 BEGIN
	/* 41AB20 */ uchar* CExplosion_GetClassTable();
	// !DECL 0x0041ab20 END
	// !DECL 0x0041b250 BEGIN
	/* 41B250 */ uint CExplosion_DamageAtPoint(tagRECT* param_1, int* param_2, int param_3, char param_4, uchar param_5);
	// !DECL 0x0041b250 END
	// !DECL 0x0041ce30 BEGIN
	/* 41CE30 */ void* CExplosion_Ctor(void* param_1, uchar param_2);
	// !DECL 0x0041ce30 END
	// !DECL 0x0041e140 BEGIN
	/* 41E140 */ void CExplosion_ApplyAreaDamage();
	// !DECL 0x0041e140 END
	// !DECL 0x0041efe0 BEGIN
	/* 41EFE0 */ uchar CExplosion_OnEvent(uint param_1, short param_2);
	// !DECL 0x0041efe0 END
};

#endif
