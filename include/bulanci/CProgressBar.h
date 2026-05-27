#ifndef _C_PROGRESS_BAR
#define _C_PROGRESS_BAR

#include <globals.h>

class CProgressBar {
public:
	// !DECL 0x0040ad80 BEGIN
	/* 40AD80 */ uchar* CProgressBar_GetTypeDescriptor();
	// !DECL 0x0040ad80 END
	// !DECL 0x0040b1f0 BEGIN
	/* 40B1F0 */ uchar CProgressBar_Render(int param_1);
	// !DECL 0x0040b1f0 END
	// !DECL 0x0040bd50 BEGIN
	/* 40BD50 */ uint* CProgressBar_Allocate();
	// !DECL 0x0040bd50 END
	// !DECL 0x0040df70 BEGIN
	/* 40DF70 */ CDSChained* CProgressBar_ctor(CDSChained* param_1);
	// !DECL 0x0040df70 END
};

#endif
