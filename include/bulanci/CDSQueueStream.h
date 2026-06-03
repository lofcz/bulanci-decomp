#ifndef _C_D_S_QUEUE_STREAM
#define _C_D_S_QUEUE_STREAM

#include <globals.h>

class CDSQueueStream {
public:
	// !DECL 0x00428960 BEGIN
	/* 428960 */ uchar CloseStream(int param_1);
	// !DECL 0x00428960 END
	// !DECL 0x00428a20 BEGIN
	/* 428A20 */ uchar* CDSQueueStream_GetTypeInfo();
	// !DECL 0x00428a20 END
	// !DECL 0x00428a30 BEGIN
	/* 428A30 */ uchar CDSQueueStream_ScalarDeletingDtor_thunk_Sub0c(uchar param_1);
	// !DECL 0x00428a30 END
	// !DECL 0x00428a40 BEGIN
	/* 428A40 */ uchar CDSQueueStream_ScalarDeletingDtorThunk(uchar param_1);
	// !DECL 0x00428a40 END
	// !DECL 0x00428a50 BEGIN
	/* 428A50 */ uchar CDSQueueStream_dtor(uint* param_1);
	// !DECL 0x00428a50 END
	// !DECL 0x00428ab0 BEGIN
	/* 428AB0 */ void* CDSQueueStream_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00428ab0 END
	// !DECL 0x00428d30 BEGIN
	/* 428D30 */ uchar CDSQueueStream_ReleaseRefThunk(int param_1);
	// !DECL 0x00428d30 END
	// !DECL 0x00433c60 BEGIN
	/* 433C60 */ uchar CDSQueueStream_ReleaseRefcount(int param_1);
	// !DECL 0x00433c60 END
	// !DECL 0x00433d60 BEGIN
	/* 433D60 */ uchar CDSQueueStream_ReleaseRefcount_thunk_Sub0c(int param_1);
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
	/* 43BF80 */ uchar SeekPosition(uint param_1, uint param_2, int param_3);
	// !DECL 0x0043bf80 END
	// !DECL 0x0043c000 BEGIN
	/* 43C000 */ void ReadBytes(void* param_1, size_t param_2);
	// !DECL 0x0043c000 END
	// !DECL 0x0043c0b0 BEGIN
	/* 43C0B0 */ void WriteBytes(void* param_1, size_t param_2);
	// !DECL 0x0043c0b0 END
	// !DECL 0x0043c1a0 BEGIN
	/* 43C1A0 */ uchar GetStreamName(uchar param_1);
	// !DECL 0x0043c1a0 END
};

#endif
