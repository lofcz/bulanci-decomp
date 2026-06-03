#ifndef _C_SPELLS
#define _C_SPELLS

#include <globals.h>

class CSpells {
public:
	// !DECL 0x00426da0 BEGIN
	/* 426DA0 */ void CSpells_OnEvent(short param_1, uint param_2);
	// !DECL 0x00426da0 END
	// !DECL 0x004274b0 BEGIN
	/* 4274B0 */ uchar* CSpells_GetClassTable();
	// !DECL 0x004274b0 END
	// !DECL 0x004278c0 BEGIN
	/* 4278C0 */ void Draw(void* param_1);
	// !DECL 0x004278c0 END
	// !DECL 0x00427b10 BEGIN
	/* 427B10 */ uint* CSpells_Allocate();
	// !DECL 0x00427b10 END

	// !DECL 0x00426d70 BEGIN
	/* 426D70 */ void CSpells_SetAmmoEmptyAndInvalidate(uchar param_1, uchar param_2);
	// !DECL 0x00426d70 END
	// !DECL 0x00427d40 BEGIN
	/* 427D40 */ void* CSpells_ctor(int param_1, int param_2);
	// !DECL 0x00427d40 END
};

#endif
