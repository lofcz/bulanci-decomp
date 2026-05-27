#ifndef _C_D_S_FLX_FILE
#define _C_D_S_FLX_FILE

#include <globals.h>

class CDSFlxFile {
public:
	// !DECL 0x00432740 BEGIN
	/* 432740 */ static uchar DecodeRleColor(int param_1, ushort* param_2, uchar* param_3);
	// !DECL 0x00432740 END
	// !DECL 0x00432780 BEGIN
	/* 432780 */ static uchar DecodeDeltaColor(int param_1, ushort* param_2, uchar* param_3);
	// !DECL 0x00432780 END
	// !DECL 0x004327e0 BEGIN
	/* 4327E0 */ static uchar DecodeCopyColor(size_t param_1, void* param_2, void* param_3);
	// !DECL 0x004327e0 END
	// !DECL 0x00432800 BEGIN
	/* 432800 */ static uchar DecodePaletteRgb(int param_1, uchar* param_2, ushort* param_3);
	// !DECL 0x00432800 END
	// !DECL 0x00432840 BEGIN
	/* 432840 */ uchar ResetCursor(uint* param_1);
	// !DECL 0x00432840 END
	// !DECL 0x00432850 BEGIN
	/* 432850 */ static uchar DecodeRegionList(int param_1, uchar* param_2, void* param_3);
	// !DECL 0x00432850 END
	// !DECL 0x004328e0 BEGIN
	/* 4328E0 */ uchar* GetClassRegistry();
	// !DECL 0x004328e0 END
	// !DECL 0x004328f0 BEGIN
	/* 4328F0 */ uchar Sub1cDtor(uchar param_1);
	// !DECL 0x004328f0 END
	// !DECL 0x00432900 BEGIN
	/* 432900 */ uchar Sub20ChainOp(int param_1);
	// !DECL 0x00432900 END
	// !DECL 0x00432910 BEGIN
	/* 432910 */ uchar Sub04Dtor(uchar param_1);
	// !DECL 0x00432910 END
	// !DECL 0x00432920 BEGIN
	/* 432920 */ uchar Sub24Dtor(uchar param_1);
	// !DECL 0x00432920 END
	// !DECL 0x00432930 BEGIN
	/* 432930 */ uchar Sub24ChainOp(int param_1);
	// !DECL 0x00432930 END
	// !DECL 0x00432940 BEGIN
	/* 432940 */ uchar Sub28Dtor(uchar param_1);
	// !DECL 0x00432940 END
	// !DECL 0x00432950 BEGIN
	/* 432950 */ uchar ReleaseRef(int param_1);
	// !DECL 0x00432950 END
	// !DECL 0x00432980 BEGIN
	/* 432980 */ uchar Sub1cRelease(int param_1);
	// !DECL 0x00432980 END
	// !DECL 0x00432990 BEGIN
	/* 432990 */ uchar Sub04Release(int param_1);
	// !DECL 0x00432990 END
	// !DECL 0x004329a0 BEGIN
	/* 4329A0 */ uchar Sub24Release(int param_1);
	// !DECL 0x004329a0 END
	// !DECL 0x004329b0 BEGIN
	/* 4329B0 */ uchar Sub28Release(int param_1);
	// !DECL 0x004329b0 END
	// !DECL 0x004329c0 BEGIN
	/* 4329C0 */ uchar DestructInPlace(uint* param_1);
	// !DECL 0x004329c0 END
	// !DECL 0x00432aa0 BEGIN
	/* 432AA0 */ void* ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00432aa0 END
	// !DECL 0x00432ac0 BEGIN
	/* 432AC0 */ uchar BindStream(int* param_1);
	// !DECL 0x00432ac0 END
	// !DECL 0x00432b60 BEGIN
	/* 432B60 */ uchar CloseStream(int param_1);
	// !DECL 0x00432b60 END
	// !DECL 0x00432be0 BEGIN
	/* 432BE0 */ uchar CreateBoundClone(int param_1);
	// !DECL 0x00432be0 END
	// !DECL 0x00432c60 BEGIN
	/* 432C60 */ uchar DecodeFrame(uint* param_1, void* param_2);
	// !DECL 0x00432c60 END
	// !DECL 0x00436e80 BEGIN
	/* 436E80 */ uchar NotifyMove(uint param_1);
	// !DECL 0x00436e80 END
	// !DECL 0x00436eb0 BEGIN
	/* 436EB0 */ uchar NotifyRegionList(uint param_1, uint param_2);
	// !DECL 0x00436eb0 END
	// !DECL 0x00436ff0 BEGIN
	/* 436FF0 */ uchar AllocMaskPlane(int param_1);
	// !DECL 0x00436ff0 END
};

#endif
