#ifndef __LOCALE_UPDATE
#define __LOCALE_UPDATE

#include <globals.h>

/* Forward declarations of types referenced by parameter / return
 * positions but not (yet) defined in this project. Stubs only use
 * pointers so a struct forward-decl is enough. */
struct LONG;
struct localeinfo_struct;
struct threadlocaleinfostruct;

class _LocaleUpdate {
public:
	/* 4487C8 */ _LocaleUpdate(localeinfo_struct* param_1);
	/* 44CECD */ LONG* _updatetlocinfoEx_nolock();
	/* 44CF0B */ static threadlocaleinfostruct* _updatetlocinfo();
	/* 44CF75 */ uchar FUN_0044cf75();
};

#endif