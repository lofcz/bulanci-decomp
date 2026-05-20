#ifndef _C_CHAT_EDIT
#define _C_CHAT_EDIT

#include <globals.h>

class CChatEdit {
public:
	// !DECL 0x0040b9f0 BEGIN
	/* 40B9F0 */ uchar* CChatEdit_GetTypeDescriptor();
	// !DECL 0x0040b9f0 END
	// !DECL 0x0040ce50 BEGIN
	/* 40CE50 */ uint* CChatEdit_BuildAt(uint* param_1, uint param_2, uint param_3, uint param_4);
	// !DECL 0x0040ce50 END
	// !DECL 0x0040cee0 BEGIN
	/* 40CEE0 */ char CChatEdit_OnChar(uchar* param_1, uchar* param_2);
	// !DECL 0x0040cee0 END
	// !DECL 0x0040eca0 BEGIN
	/* 40ECA0 */ uint* CChatEdit_Allocate();
	// !DECL 0x0040eca0 END
};

#endif
