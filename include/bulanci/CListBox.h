#ifndef _C_LIST_BOX
#define _C_LIST_BOX

#include <globals.h>

namespace CListBox {

class CListBox {
public:
	// !DECL 0x00405fc0 BEGIN
	/* 405FC0 */ uchar CListBox_RenderItem(int* param_1, int param_2);
	// !DECL 0x00405fc0 END
	// !DECL 0x00408200 BEGIN
	/* 408200 */ uchar CListBox_dtor(uint* param_1);
	// !DECL 0x00408200 END
	// !DECL 0x00408290 BEGIN
	/* 408290 */ uchar* CListBox_GetTypeDescriptor();
	// !DECL 0x00408290 END
	// !DECL 0x00408350 BEGIN
	/* 408350 */ void* CListBox_vDtor(uchar param_1);
	// !DECL 0x00408350 END
	// !DECL 0x00408df0 BEGIN
	/* 408DF0 */ uint* CListBox_Allocate();
	// !DECL 0x00408df0 END
};

} // namespace CListBox

#endif
