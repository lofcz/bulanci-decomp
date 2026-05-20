#ifndef ZLIB
#define ZLIB

#include <globals.h>

class zlib {
public:
	// !DECL 0x0046edc0 BEGIN
	/* 46EDC0 */ static uint inflateReset(int param_1);
	// !DECL 0x0046edc0 END
	// !DECL 0x0046ee10 BEGIN
	/* 46EE10 */ static uint inflateEnd(int param_1);
	// !DECL 0x0046ee10 END
	// !DECL 0x0046ee60 BEGIN
	/* 46EE60 */ static uint inflateInit2(int param_1, int param_2, char* param_3, int param_4);
	// !DECL 0x0046ee60 END
	// !DECL 0x0046ef60 BEGIN
	/* 46EF60 */ static uint inflateInit(int param_1, char* param_2, int param_3);
	// !DECL 0x0046ef60 END
	// !DECL 0x0046ef80 BEGIN
	/* 46EF80 */ static uint inflate(int* param_1, int param_2);
	// !DECL 0x0046ef80 END
	// !DECL 0x0046f3e0 BEGIN
	/* 46F3E0 */ static uint deflate(int* param_1, uint param_2);
	// !DECL 0x0046f3e0 END
	// !DECL 0x0046f660 BEGIN
	/* 46F660 */ static uint deflateEnd(int param_1);
	// !DECL 0x0046f660 END
	// !DECL 0x004702c0 BEGIN
	/* 4702C0 */ static uint deflateInit2(int param_1, uint param_2, int param_3, int param_4, int param_5, uint param_6, char* param_7, int param_8);
	// !DECL 0x004702c0 END
	// !DECL 0x004704c0 BEGIN
	/* 4704C0 */ static uint deflateInit(int param_1, uint param_2, char* param_3, int param_4);
	// !DECL 0x004704c0 END
	// !DECL 0x004704f0 BEGIN
	/* 4704F0 */ static uchar inflate_blocks_reset(int* param_1, int param_2, int* param_3);
	// !DECL 0x004704f0 END
	// !DECL 0x00470570 BEGIN
	/* 470570 */ static int* inflate_blocks_new(int param_1, int param_2, int param_3);
	// !DECL 0x00470570 END
	// !DECL 0x00470620 BEGIN
	/* 470620 */ static uchar inflate_blocks(uint* param_1, int* param_2, int param_3);
	// !DECL 0x00470620 END
	// !DECL 0x004710e0 BEGIN
	/* 4710E0 */ static uint inflate_blocks_free(int* param_1, int param_2);
	// !DECL 0x004710e0 END
	// !DECL 0x00471260 BEGIN
	/* 471260 */ static int* zcalloc(uint param_1, size_t param_2, size_t param_3);
	// !DECL 0x00471260 END
	// !DECL 0x00471290 BEGIN
	/* 471290 */ uchar init_block(uint param_1, int param_2);
	// !DECL 0x00471290 END
	// !DECL 0x004733d0 BEGIN
	/* 4733D0 */ static void inflate_codes_free(uint param_1, int param_2);
	// !DECL 0x004733d0 END
};

#endif
