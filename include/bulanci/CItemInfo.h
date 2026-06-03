#ifndef _C_ITEM_INFO
#define _C_ITEM_INFO

#include <globals.h>

class CItemInfo {
public:
	// !DECL 0x00403500 BEGIN
	/* 403500 */ uchar* CItemInfo_GetTypeInfo();
	// !DECL 0x00403500 END
	// !DECL 0x00434b10 BEGIN
	/* 434B10 */ uint CDSObject_GetThis();
	// !DECL 0x00434b10 END

	// !DECL 0x00404c50 BEGIN
	/* 404C50 */ void* CreateObject();
	// !DECL 0x00404c50 END
};

#endif
