#ifndef _C_D_S_QUEUE_STREAM
#define _C_D_S_QUEUE_STREAM

#include <globals.h>

class CDSQueueStream {
public:
	// !DECL 0x00428960 BEGIN
	/* 428960 */ uchar CloseStream(int param_1);
	// !DECL 0x00428960 END
	// !DECL 0x00428a20 BEGIN
	/* 428A20 */ uchar* FUN_00428a20();
	// !DECL 0x00428a20 END
	// !DECL 0x00428a30 BEGIN
	/* 428A30 */ uchar FUN_00428a30(uchar param_1);
	// !DECL 0x00428a30 END
	// !DECL 0x00428a40 BEGIN
	/* 428A40 */ uchar FUN_00428a40(uchar param_1);
	// !DECL 0x00428a40 END
	// !DECL 0x00428a50 BEGIN
	/* 428A50 */ uchar FUN_00428a50(uint* param_1);
	// !DECL 0x00428a50 END
	// !DECL 0x00428ab0 BEGIN
	/* 428AB0 */ void* FUN_00428ab0(uchar param_1);
	// !DECL 0x00428ab0 END
	// !DECL 0x00428d30 BEGIN
	/* 428D30 */ uchar FUN_00428d30(int param_1);
	// !DECL 0x00428d30 END
	// !DECL 0x00433c60 BEGIN
	/* 433C60 */ uchar FUN_00433c60(int param_1);
	// !DECL 0x00433c60 END
	// !DECL 0x00433d60 BEGIN
	/* 433D60 */ uchar FUN_00433d60(int param_1);
	// !DECL 0x00433d60 END
	// !DECL 0x0043beb0 BEGIN
	/* 43BEB0 */ uchar GetSize(uint param_1);
	// !DECL 0x0043beb0 END
	// !DECL 0x0043bee0 BEGIN
	/* 43BEE0 */ uchar TellPosition(uint param_1);
	// !DECL 0x0043bee0 END
	// !DECL 0x0043bf10 BEGIN
	/* 43BF10 */ uchar SetStreamSize(uint param_1, uint param_2);
	// !DECL 0x0043bf10 END
	// !DECL 0x0043bf80 BEGIN
	/* 43BF80 */ uchar SeekPosition(int param_1, uint param_2, int param_3);
	// !DECL 0x0043bf80 END
	// !DECL 0x0043c000 BEGIN
	/* 43C000 */ uchar ReadBytes(void* param_1, void* param_2);
	// !DECL 0x0043c000 END
	// !DECL 0x0043c0b0 BEGIN
	/* 43C0B0 */ uchar WriteBytes(void* param_1, void* param_2);
	// !DECL 0x0043c0b0 END
	// !DECL 0x0043c1a0 BEGIN
	/* 43C1A0 */ uchar GetStreamName(uchar param_1);
	// !DECL 0x0043c1a0 END
};

#endif
