#ifndef STD
#define STD

#include <globals.h>

namespace std {

class bad_alloc {
public:
	// !DECL 0x00447be4 BEGIN
	/* 447BE4 */ bad_alloc();
	// !DECL 0x00447be4 END
	// !DECL 0x00447c08 BEGIN
	/* 447C08 */ void* FUN_00447c08(uchar param_1);
	// !DECL 0x00447c08 END
	// !DECL 0x0044bf84 BEGIN
	/* 44BF84 */ char* FUN_0044bf84(int param_1);
	// !DECL 0x0044bf84 END
};

class bad_exception {
public:
	// !DECL 0x0044b047 BEGIN
	/* 44B047 */ void* FUN_0044b047(uchar param_1);
	// !DECL 0x0044b047 END
};

class exception {
public:
	// !DECL 0x0044beb0 BEGIN
	/* 44BEB0 */ exception(char** param_1);
	// !DECL 0x0044beb0 END
	// !DECL 0x0044befe BEGIN
	/* 44BEFE */ uchar exception_0044befe(char** param_1, int param_2);
	// !DECL 0x0044befe END
	// !DECL 0x0044bf16 BEGIN
	/* 44BF16 */ uchar exception_0044bf16(void* param_1);
	// !DECL 0x0044bf16 END
	// !DECL 0x0044bf91 BEGIN
	/* 44BF91 */ void* FUN_0044bf91(uchar param_1);
	// !DECL 0x0044bf91 END
};

} // namespace std

#endif
