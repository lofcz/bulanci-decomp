#ifndef _C_D_S_DIRECT_PLAY_SENDER
#define _C_D_S_DIRECT_PLAY_SENDER

#include <globals.h>

class CDSDirectPlaySender {
public:
	// !DECL 0x0043ad20 BEGIN
	/* 43AD20 */ void CDSDirectPlaySender_ThreadEntry(uchar param_1);
	// !DECL 0x0043ad20 END
	// !DECL 0x0043b120 BEGIN
	/* 43B120 */ uchar* CDSDirectPlaySender_GetTypeID_thunk();
	// !DECL 0x0043b120 END
	// !DECL 0x0043b130 BEGIN
	/* 43B130 */ uchar* CDSDirectPlaySender_GetTypeID();
	// !DECL 0x0043b130 END
	// !DECL 0x0043b140 BEGIN
	/* 43B140 */ uchar CDSDirectPlaySender_scalar_deleting_dtor_thunk(uchar param_1);
	// !DECL 0x0043b140 END
	// !DECL 0x0043b150 BEGIN
	/* 43B150 */ void CDSDirectPlaySender_dtor_body(uchar param_1);
	// !DECL 0x0043b150 END
	// !DECL 0x0043b530 BEGIN
	/* 43B530 */ void* CDSDirectPlaySender_scalar_deleting_dtor(uchar param_1);
	// !DECL 0x0043b530 END
};

#endif
