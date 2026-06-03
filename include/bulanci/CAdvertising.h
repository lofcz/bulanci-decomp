#ifndef _C_ADVERTISING
#define _C_ADVERTISING

#include <globals.h>

class CAdvertising {
public:
	// !DECL 0x0040aca0 BEGIN
	/* 40ACA0 */ uchar ArmDismissTimer(uint param_1);
	// !DECL 0x0040aca0 END
	// !DECL 0x0040acc0 BEGIN
	/* 40ACC0 */ uint CAdvertising_Dismiss(int param_1);
	// !DECL 0x0040acc0 END
	// !DECL 0x0040b4b0 BEGIN
	/* 40B4B0 */ uchar OnTimerTick(void* param_1);
	// !DECL 0x0040b4b0 END
	// !DECL 0x0040b4d0 BEGIN
	/* 40B4D0 */ void OnLButtonDown(int param_1, int param_2, uint param_3);
	// !DECL 0x0040b4d0 END
	// !DECL 0x0040b500 BEGIN
	/* 40B500 */ uint OnKeyDown(char param_1, char param_2);
	// !DECL 0x0040b500 END
	// !DECL 0x0040e5f0 BEGIN
	/* 40E5F0 */ void* CAdvertising_ctor();
	// !DECL 0x0040e5f0 END
	// !DECL 0x0040e710 BEGIN
	/* 40E710 */ uchar* GetClassId();
	// !DECL 0x0040e710 END
	// !DECL 0x0040e720 BEGIN
	/* 40E720 */ uchar CAdvertising_ScalarDeletingDtor_thunk_Sub10(uchar param_1);
	// !DECL 0x0040e720 END
	// !DECL 0x0040e730 BEGIN
	/* 40E730 */ uchar CAdvertising_ScalarDeletingDtor_thunk_Sub70(uchar param_1);
	// !DECL 0x0040e730 END
	// !DECL 0x0040e740 BEGIN
	/* 40E740 */ uchar deleting_destructor_thunk_0x18(uchar param_1);
	// !DECL 0x0040e740 END
	// !DECL 0x0040fb70 BEGIN
	/* 40FB70 */ void* CAdvertising_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0040fb70 END
	// !DECL 0x0040fe30 BEGIN
	/* 40FE30 */ uchar LoadSplashImage(uint param_1);
	// !DECL 0x0040fe30 END

	// !DECL 0x00401a10 BEGIN
	/* 401A10 */ void CAdvertising_dtor();
	// !DECL 0x00401a10 END
};

#endif
