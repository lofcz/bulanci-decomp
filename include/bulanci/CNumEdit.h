#ifndef _C_NUM_EDIT
#define _C_NUM_EDIT

#include <globals.h>

/* Forward declarations of types referenced by parameter / return
 * positions but not (yet) defined in this project. Stubs only use
 * pointers so a struct forward-decl is enough. */
struct localeinfo_struct;

class CNumEdit {
public:
	/* 4031A0 */ uchar FUN_004031a0(uint* param_1);
	/* 404550 */ uint FUN_00404550(short param_1);
	/* 406290 */ uint FUN_00406290(int param_1);
	/* 406750 */ uchar* FUN_00406750();
	/* 407220 */ uchar FUN_00407220(void* param_1);
	/* 407DE0 */ uchar FUN_00407de0(uchar param_1);
	/* 407E00 */ uchar FUN_00407e00(void* param_1);
	/* 42D2F0 */ uint FUN_0042d2f0(uint* param_1);
	/* 448ED6 */ static uchar FUN_00448ed6(wchar_t* param_1);
	/* 44EB1E */ static ulong wcstoxl(localeinfo_struct* param_1, wchar_t* param_2, wchar_t** param_3, int param_4, int param_5);
	/* 44ED15 */ static long _wcstol(wchar_t* param_1, wchar_t** param_2, int param_3);
	/* 44ED3E */ static int _iswctype_l(int param_1, int param_2, localeinfo_struct* param_3);
	/* 451606 */ static int _wchartodigit(ushort param_1);
	/* 4522A9 */ static int _crtGetStringTypeW_stat(localeinfo_struct* param_1, ulong param_2, wchar_t* param_3, int param_4, ushort* param_5, int param_6, int param_7);
	/* 4524CD */ static uchar _crtGetStringTypeW(localeinfo_struct* param_1, ulong param_2, wchar_t* param_3, int param_4, ushort* param_5, int param_6, int param_7);
};

#endif