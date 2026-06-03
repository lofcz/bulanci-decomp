#ifndef _C_GAME_COUNTER
#define _C_GAME_COUNTER

#include <globals.h>

class CGameCounter {
public:
	// !DECL 0x0040b140 BEGIN
	/* 40B140 */ void CGameCounter_OnEvent(short param_1, uint param_2);
	// !DECL 0x0040b140 END
	// !DECL 0x0040bc20 BEGIN
	/* 40BC20 */ void* CGameCounter_Constructor(void* param_1);
	// !DECL 0x0040bc20 END
	// !DECL 0x0040bca0 BEGIN
	/* 40BCA0 */ uchar* GetClassIdentifier();
	// !DECL 0x0040bca0 END
	// !DECL 0x0040bcb0 BEGIN
	/* 40BCB0 */ uchar CGameCounter_ScalarDeletingDtor_thunk_n0x10(uchar param_1);
	// !DECL 0x0040bcb0 END
	// !DECL 0x0040bcc0 BEGIN
	/* 40BCC0 */ uchar CGameCounter_ScalarDeletingDtor_thunk_n0x18(uchar param_1);
	// !DECL 0x0040bcc0 END
	// !DECL 0x0040bcd0 BEGIN
	/* 40BCD0 */ uchar CGameCounter_ScalarDeletingDtor_thunk_n0x4(uchar param_1);
	// !DECL 0x0040bcd0 END
	// !DECL 0x0040ef40 BEGIN
	/* 40EF40 */ void* CGameCounter_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0040ef40 END
};

#endif
