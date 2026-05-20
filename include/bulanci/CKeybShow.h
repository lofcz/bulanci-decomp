#ifndef _C_KEYB_SHOW
#define _C_KEYB_SHOW

#include <globals.h>

class CKeybShow {
public:
	// !DECL 0x0040bbf0 BEGIN
	/* 40BBF0 */ uchar* CKeybShow_GetTypeDescriptor();
	// !DECL 0x0040bbf0 END
	// !DECL 0x0040edb0 BEGIN
	/* 40EDB0 */ uint* CKeybShow_Allocate();
	// !DECL 0x0040edb0 END
};

#endif
