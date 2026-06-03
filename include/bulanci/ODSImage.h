#ifndef _O_D_S_IMAGE
#define _O_D_S_IMAGE

#include <globals.h>

class ODSImage {
public:
	// !DECL 0x00418ef0 BEGIN
	/* 418EF0 */ uint* ODSImage_FactoryCtor(uint* param_1, int param_2);
	// !DECL 0x00418ef0 END
	// !DECL 0x004228f0 BEGIN
	/* 4228F0 */ uint* CDSBitmap_SubobjectCtor(uint* param_1);
	// !DECL 0x004228f0 END
	// !DECL 0x004253d0 BEGIN
	/* 4253D0 */ int CGunMouse_CoordRing_Reserve(int param_1);
	// !DECL 0x004253d0 END

	// !DECL 0x00426060 BEGIN
	/* 426060 */ CGunMouse* CGunMouse_ctor(CGunMouse* param_1);
	// !DECL 0x00426060 END

	// !DECL 0x00418c00 BEGIN
	/* 418C00 */ void* ODSImage_ctor(CDSImage* param_1);
	// !DECL 0x00418c00 END
	// !DECL 0x00439050 BEGIN
	/* 439050 */ void SetOwner(uchar param_1);
	// !DECL 0x00439050 END

	// !DECL 0x00439100 BEGIN
	/* 439100 */ void ODSImage_SetImage(CDSImage* param_1);
	// !DECL 0x00439100 END
};

#endif
