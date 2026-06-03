#ifndef _C_SCORE_ITEM
#define _C_SCORE_ITEM

#include <globals.h>

class CScoreItem {
public:
	// !DECL 0x00408f50 BEGIN
	/* 408F50 */ void Deserialize(int* param_1);
	// !DECL 0x00408f50 END
	// !DECL 0x00408f90 BEGIN
	/* 408F90 */ void Serialize(int* param_1);
	// !DECL 0x00408f90 END
	// !DECL 0x004093a0 BEGIN
	/* 4093A0 */ uchar* GetClassIdentifier();
	// !DECL 0x004093a0 END
	// !DECL 0x004093b0 BEGIN
	/* 4093B0 */ uchar CScoreItem_ScalarDeletingDtor_thunk_n0x4(uchar param_1);
	// !DECL 0x004093b0 END
	// !DECL 0x004093c0 BEGIN
	/* 4093C0 */ uchar CScoreItem_dtor(uint* param_1);
	// !DECL 0x004093c0 END
	// !DECL 0x00409a90 BEGIN
	/* 409A90 */ void* CScoreItem_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00409a90 END

	// !DECL 0x00409080 BEGIN
	/* 409080 */ int CScoreItem_MatchesKillsDeathsAndName(void* param_1, void* param_2);
	// !DECL 0x00409080 END
};

#endif
