#ifndef _C_HELP_SCRIPT
#define _C_HELP_SCRIPT

#include <globals.h>

class CHelpScript {
public:
	// !DECL 0x004215e0 BEGIN
	/* 4215E0 */ void* ctor(void* param_1);
	// !DECL 0x004215e0 END
	// !DECL 0x00421680 BEGIN
	/* 421680 */ uchar* CHelpScript_GetClassTable();
	// !DECL 0x00421680 END
	// !DECL 0x00421690 BEGIN
	/* 421690 */ uchar CHelpScript_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x00421690 END
	// !DECL 0x004216a0 BEGIN
	/* 4216A0 */ uchar CHelpScript_ScalarDeletingDtor_thunk_Sub438(uchar param_1);
	// !DECL 0x004216a0 END
	// !DECL 0x004216b0 BEGIN
	/* 4216B0 */ uchar CHelpScript_AdjustOffsetDtor(uchar param_1);
	// !DECL 0x004216b0 END
	// !DECL 0x004216f0 BEGIN
	/* 4216F0 */ void CHelpScript_dtor(void* param_1);
	// !DECL 0x004216f0 END
	// !DECL 0x00421920 BEGIN
	/* 421920 */ void* CHelpScript_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00421920 END
	// !DECL 0x00421940 BEGIN
	/* 421940 */ static uchar HelpBuildStaticTextAuto(int param_1);
	// !DECL 0x00421940 END
	// !DECL 0x00421a00 BEGIN
	/* 421A00 */ static uchar HelpBuildStaticTextSized(int param_1);
	// !DECL 0x00421a00 END
	// !DECL 0x00421af0 BEGIN
	/* 421AF0 */ static uchar HelpBuildHelpButtonWidget(int param_1);
	// !DECL 0x00421af0 END
	// !DECL 0x004221a0 BEGIN
	/* 4221A0 */ static CDSBitmap* HelpBuildHelpBitmapWidget(int param_1);
	// !DECL 0x004221a0 END
};

#endif
