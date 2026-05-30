#ifndef _C_SPELLS
#define _C_SPELLS

#include <globals.h>

class CSpells {
public:
	// !DECL 0x00426da0 BEGIN
	/* 426DA0 */ void OnEvent(short param_1, uint param_2);
	// !DECL 0x00426da0 END
	// !DECL 0x004274b0 BEGIN
	/* 4274B0 */ uchar* CSpells_GetClassTable();
	// !DECL 0x004274b0 END
	// !DECL 0x004278c0 BEGIN
	/* 4278C0 */ void Draw(void* param_1);
	// !DECL 0x004278c0 END
	// !DECL 0x00427b10 BEGIN
	/* 427B10 */ uchar CSpells_Allocate();
	// !DECL 0x00427b10 END
};

#endif
