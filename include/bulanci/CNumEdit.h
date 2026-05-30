#ifndef _C_NUM_EDIT
#define _C_NUM_EDIT

#include <globals.h>

/* Forward declarations of types referenced by parameter / return
 * positions but not (yet) defined in this project. Stubs only use
 * pointers so a struct forward-decl is enough. */
struct localeinfo_struct;

class CNumEdit {
public:
	// !DECL 0x004031a0 BEGIN
	/* 4031A0 */ uchar CNumEdit_GetValue(uint* param_1);
	// !DECL 0x004031a0 END
	// !DECL 0x00404550 BEGIN
	/* 404550 */ uint CNumEdit_ValidateRange(short param_1);
	// !DECL 0x00404550 END
	// !DECL 0x00406290 BEGIN
	/* 406290 */ uint CNumEdit_OnTextChanged(int param_1);
	// !DECL 0x00406290 END
	// !DECL 0x00406750 BEGIN
	/* 406750 */ uchar* CNumEdit_GetTypeDescriptor();
	// !DECL 0x00406750 END
	// !DECL 0x00407220 BEGIN
	/* 407220 */ uchar CNumEdit_OnFocusOut(void* param_1);
	// !DECL 0x00407220 END
	// !DECL 0x004073e0 BEGIN
	/* 4073E0 */ uint* CNumEdit_Allocate();
	// !DECL 0x004073e0 END
	// !DECL 0x00407de0 BEGIN
	/* 407DE0 */ uchar CNumEdit_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x00407de0 END
	// !DECL 0x00407e00 BEGIN
	/* 407E00 */ uchar CNumEdit_vDtor(void* param_1);
	// !DECL 0x00407e00 END
	// !DECL 0x0042d2f0 BEGIN
	/* 42D2F0 */ uint CNumEdit_ParseDecimalValue(uint* param_1);
	// !DECL 0x0042d2f0 END
	// !DECL 0x00448ed6 BEGIN
	/* 448ED6 */ static uchar CNumEdit_ParseWideDigitsToUint(wchar_t* param_1);
	// !DECL 0x00448ed6 END
	// !DECL 0x0044eb1e BEGIN
	/* 44EB1E */ static ulong wcstoxl(localeinfo_struct* param_1, wchar_t* param_2, wchar_t** param_3, int param_4, int param_5);
	// !DECL 0x0044eb1e END
	// !DECL 0x0044ed15 BEGIN
	/* 44ED15 */ static long _wcstol(wchar_t* param_1, wchar_t** param_2, int param_3);
	// !DECL 0x0044ed15 END
	// !DECL 0x0044ed3e BEGIN
	/* 44ED3E */ static int _iswctype_l(int param_1, int param_2, int param_3);
	// !DECL 0x0044ed3e END
	// !DECL 0x00451606 BEGIN
	/* 451606 */ static int _wchartodigit(ushort param_1);
	// !DECL 0x00451606 END
	// !DECL 0x004522a9 BEGIN
	/* 4522A9 */ static int _crtGetStringTypeW_stat(localeinfo_struct* param_1, ulong param_2, wchar_t* param_3, int param_4, ushort* param_5, int param_6, int param_7);
	// !DECL 0x004522a9 END
	// !DECL 0x004524cd BEGIN
	/* 4524CD */ static uchar _crtGetStringTypeW(localeinfo_struct* param_1, ulong param_2, wchar_t* param_3, int param_4, ushort* param_5, int param_6, int param_7);
	// !DECL 0x004524cd END
};

#endif
