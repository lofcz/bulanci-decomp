#ifndef __LOCALE_UPDATE
#define __LOCALE_UPDATE

#include <globals.h>

/* Forward declarations of types referenced by parameter / return
 * positions but not (yet) defined in this project. Stubs only use
 * pointers so a struct forward-decl is enough. */
struct LONG;
struct localeinfo_struct;

class _LocaleUpdate {
public:
	// !DECL 0x004487c8 BEGIN
	/* 4487C8 */ _LocaleUpdate(localeinfo_struct* param_1);
	// !DECL 0x004487c8 END
	// !DECL 0x0044cecd BEGIN
	/* 44CECD */ LONG* _updatetlocinfoEx_nolock();
	// !DECL 0x0044cecd END
	// !DECL 0x0044cf0b BEGIN
	/* 44CF0B */ static threadlocaleinfostruct* _updatetlocinfo();
	// !DECL 0x0044cf0b END
	// !DECL 0x0044cf75 BEGIN
	/* 44CF75 */ uchar FUN_0044cf75();
	// !DECL 0x0044cf75 END
};

#endif
