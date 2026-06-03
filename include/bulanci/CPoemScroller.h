#ifndef _C_POEM_SCROLLER
#define _C_POEM_SCROLLER

#include <globals.h>

class CPoemScroller {
public:
	// !DECL 0x004032a0 BEGIN
	/* 4032A0 */ uint DeletingDestructorThunk_18(int* param_1);
	// !DECL 0x004032a0 END
	// !DECL 0x00404e80 BEGIN
	/* 404E80 */ static uchar BlitStatic(void* param_1, int* param_2, int param_3, uint param_4);
	// !DECL 0x00404e80 END
	// !DECL 0x004240f0 BEGIN
	/* 4240F0 */ void CPoemScroller_Render(uchar param_1);
	// !DECL 0x004240f0 END
	// !DECL 0x00425ca0 BEGIN
	/* 425CA0 */ void CPoemScroller_dtor(uchar param_1);
	// !DECL 0x00425ca0 END
	// !DECL 0x00425da0 BEGIN
	/* 425DA0 */ uchar* GetClassIdentifier();
	// !DECL 0x00425da0 END
	// !DECL 0x00425db0 BEGIN
	/* 425DB0 */ uchar CPoemScroller_ScalarDeletingDtor_thunk_n0x4(uchar param_1);
	// !DECL 0x00425db0 END
	// !DECL 0x00425dc0 BEGIN
	/* 425DC0 */ uchar CPoemScroller_ScalarDeletingDtor_thunk_n0x68(uchar param_1);
	// !DECL 0x00425dc0 END
	// !DECL 0x00425dd0 BEGIN
	/* 425DD0 */ uchar CPoemScroller_ScalarDeletingDtor_thunk_n0x10(uchar param_1);
	// !DECL 0x00425dd0 END
	// !DECL 0x00425de0 BEGIN
	/* 425DE0 */ uchar CPoemScroller_ScalarDeletingDtor_thunk_n0x18(uchar param_1);
	// !DECL 0x00425de0 END
	// !DECL 0x00425df0 BEGIN
	/* 425DF0 */ void PickNextPoem(void* param_1);
	// !DECL 0x00425df0 END
	// !DECL 0x00426030 BEGIN
	/* 426030 */ void OnScrollTick(int* param_1);
	// !DECL 0x00426030 END
	// !DECL 0x004262c0 BEGIN
	/* 4262C0 */ void* CPoemScroller_Constructor(void* param_1);
	// !DECL 0x004262c0 END
	// !DECL 0x004264c5 BEGIN
	/* 4264C5 */ uchar CatchExceptionHelper();
	// !DECL 0x004264c5 END
	// !DECL 0x004264e0 BEGIN
	/* 4264E0 */ void* CPoemScroller_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004264e0 END
	// !DECL 0x00435c20 BEGIN
	/* 435C20 */ int CPoemScroller_ColorDistanceL1(uint param_1);
	// !DECL 0x00435c20 END
	// !DECL 0x00436110 BEGIN
	/* 436110 */ uchar CPoemScroller_GetPaletteEntry(uint* param_1, int param_2);
	// !DECL 0x00436110 END
	// !DECL 0x00436130 BEGIN
	/* 436130 */ uchar CPoemScroller_SampleSourcePixel(uint param_1, uint param_2);
	// !DECL 0x00436130 END
	// !DECL 0x00436160 BEGIN
	/* 436160 */ uint CPoemScroller_WriteDestPixelRemapped(uint param_1, uint param_2, uint param_3);
	// !DECL 0x00436160 END
	// !DECL 0x004361a0 BEGIN
	/* 4361A0 */ uchar CPoemScroller_OpaqueBlitPixel_Indexed1(int* param_1, uint param_2, uint param_3);
	// !DECL 0x004361a0 END
	// !DECL 0x00436210 BEGIN
	/* 436210 */ int CPoemScroller_FindNearestPaletteIndex(uchar param_1);
	// !DECL 0x00436210 END
	// !DECL 0x00436270 BEGIN
	/* 436270 */ uint CPoemScroller_BlitPixelViaFormatTable(uint param_1, uint param_2, uint param_3);
	// !DECL 0x00436270 END
	// !DECL 0x00436310 BEGIN
	/* 436310 */ uchar CDSImage_DrawHorizontalLine(int param_1, int param_2, int param_3, uint param_4, int param_5);
	// !DECL 0x00436310 END
	// !DECL 0x00436420 BEGIN
	/* 436420 */ uchar CDSImage_DrawVerticalLine(int param_1, int param_2, int param_3, uint param_4, int param_5);
	// !DECL 0x00436420 END
	// !DECL 0x00436530 BEGIN
	/* 436530 */ uchar CPoemScroller_DrawRectOutline(int* param_1, uint param_2, int param_3);
	// !DECL 0x00436530 END
	// !DECL 0x004365f0 BEGIN
	/* 4365F0 */ uchar CPoemScroller_FillRect(int* param_1, uint param_2, int param_3);
	// !DECL 0x004365f0 END
	// !DECL 0x00436760 BEGIN
	/* 436760 */ uchar CPoemScroller_ClearBlitMask(int param_1);
	// !DECL 0x00436760 END
	// !DECL 0x00436770 BEGIN
	/* 436770 */ uchar BlitOpaqueFallback(int* param_1, void* param_2, int* param_3);
	// !DECL 0x00436770 END
	// !DECL 0x004368d0 BEGIN
	/* 4368D0 */ uchar BlitDispatch(int* param_1, void* param_2, int* param_3, uint* param_4);
	// !DECL 0x004368d0 END
	// !DECL 0x00436e10 BEGIN
	/* 436E10 */ uchar CPoemScroller_SetBlitMask(int param_1);
	// !DECL 0x00436e10 END
	// !DECL 0x00445740 BEGIN
	/* 445740 */ static uchar DrawVerticalLineGradient_16bpp(int param_1, int param_2, int param_3, int param_4, int param_5, uint param_6, int param_7);
	// !DECL 0x00445740 END
	// !DECL 0x00445900 BEGIN
	/* 445900 */ static uchar CPoemScroller_FillColumn24bpp(int param_1, int param_2, int param_3, int param_4, int param_5, uint param_6, int param_7);
	// !DECL 0x00445900 END
	// !DECL 0x00445a70 BEGIN
	/* 445A70 */ static uchar CPoemScroller_FillColumn32bpp(int param_1, int param_2, int param_3, int param_4, int param_5, uint param_6, int param_7);
	// !DECL 0x00445a70 END
	// !DECL 0x00445bd0 BEGIN
	/* 445BD0 */ static uchar CPoemScroller_FillSpan16bpp(int param_1, int param_2, int param_3, int param_4, int param_5, uint param_6, int param_7);
	// !DECL 0x00445bd0 END
	// !DECL 0x00445da0 BEGIN
	/* 445DA0 */ static uchar CPoemScroller_FillSpan24bpp(int param_1, int param_2, int param_3, int param_4, int param_5, uint param_6, int param_7);
	// !DECL 0x00445da0 END
	// !DECL 0x00445f10 BEGIN
	/* 445F10 */ static uchar CPoemScroller_FillSpan32bpp(int param_1, int param_2, int param_3, int param_4, int param_5, uint param_6, int param_7);
	// !DECL 0x00445f10 END
	// !DECL 0x00446060 BEGIN
	/* 446060 */ static uchar BlitAlphaFill_RGB565(int* param_1, int param_2, int param_3, uint param_4, int param_5);
	// !DECL 0x00446060 END
	// !DECL 0x00446250 BEGIN
	/* 446250 */ static uchar BlitAlphaFill_BGR24(int* param_1, int param_2, int param_3, uint param_4, int param_5);
	// !DECL 0x00446250 END
	// !DECL 0x004463e0 BEGIN
	/* 4463E0 */ static uchar CPoemScroller_FillRect32bpp(int* param_1, int param_2, int param_3, uint param_4, int param_5);
	// !DECL 0x004463e0 END
};

#endif
