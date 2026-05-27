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
	/* 408200 */ uchar CListBox_dtor(CScroller* param_1);
	// !DECL 0x00408200 END
	// !DECL 0x00408290 BEGIN
	/* 408290 */ uchar* CListBox_GetTypeDescriptor();
	// !DECL 0x00408290 END
	// !DECL 0x004082a0 BEGIN
	/* 4082A0 */ uchar FUN_004082a0(uchar param_1);
	// !DECL 0x004082a0 END
	// !DECL 0x004082b0 BEGIN
	/* 4082B0 */ uchar FUN_004082b0(uchar param_1);
	// !DECL 0x004082b0 END
	// !DECL 0x00408350 BEGIN
	/* 408350 */ void* CListBox_vDtor(uchar param_1);
	// !DECL 0x00408350 END
	// !DECL 0x00408cc0 BEGIN
	/* 408CC0 */ void* CListBox_BuildAt(int param_1, int param_2, uint param_3, int param_4, uint param_5, uint param_6);
	// !DECL 0x00408cc0 END
	// !DECL 0x00408df0 BEGIN
	/* 408DF0 */ uint* CListBox_Allocate();
	// !DECL 0x00408df0 END
	// !DECL 0x0040b130 BEGIN
	/* 40B130 */ uchar FUN_0040b130(uchar param_1);
	// !DECL 0x0040b130 END
	// !DECL 0x0040d0a0 BEGIN
	/* 40D0A0 */ uchar FUN_0040d0a0(uchar param_1);
	// !DECL 0x0040d0a0 END
};

} // namespace CListBox

#endif
