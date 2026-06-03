#ifndef _C_LEVEL_SCORE
#define _C_LEVEL_SCORE

#include <globals.h>

class CLevelScore {
public:
	// !DECL 0x00409050 BEGIN
	/* 409050 */ uchar Deserialize(int* param_1);
	// !DECL 0x00409050 END
	// !DECL 0x00409310 BEGIN
	/* 409310 */ uchar* GetClassIdentifier();
	// !DECL 0x00409310 END
	// !DECL 0x00409320 BEGIN
	/* 409320 */ uchar CLevelScore_ScalarDeletingDtor_thunk_n0x4(uchar param_1);
	// !DECL 0x00409320 END
	// !DECL 0x00409330 BEGIN
	/* 409330 */ void CLevelScore_dtor();
	// !DECL 0x00409330 END
	// !DECL 0x00409690 BEGIN
	/* 409690 */ void Serialize(int* param_1);
	// !DECL 0x00409690 END
	// !DECL 0x00409a40 BEGIN
	/* 409A40 */ void* CLevelScore_scalar_deleting_dtor(uchar param_1);
	// !DECL 0x00409a40 END

	// !DECL 0x00409620 BEGIN
	/* 409620 */ void CLevelScore_InitializeDefaultScores();
	// !DECL 0x00409620 END
	// !DECL 0x00409b10 BEGIN
	/* 409B10 */ CDSChain* CLevelScore_AddPlayerScore(uint* param_1, uchar* param_2, uchar* param_3);
	// !DECL 0x00409b10 END
	// !DECL 0x0042fae0 BEGIN
	/* 42FAE0 */ uchar CDSChain_SortChildrenWithComparator(_PtFuncCompare* param_1);
	// !DECL 0x0042fae0 END
};

#endif
