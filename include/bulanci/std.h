#ifndef STD
#define STD

#include <globals.h>

namespace std {

class bad_alloc {
public:
	/* 447BE4 */ bad_alloc();
	/* 447C08 */ void* FUN_00447c08(uchar param_1);
	/* 44BF84 */ char* FUN_0044bf84(int param_1);
};

class bad_exception {
public:
	/* 44B047 */ void* FUN_0044b047(uchar param_1);
};

class exception {
public:
	/* 44BEB0 */ exception(char** param_1);
	/* 44BEFE */ uchar exception_0044befe(char** param_1, int param_2);
	/* 44BF16 */ uchar exception_0044bf16(void* param_1);
	/* 44BF91 */ void* FUN_0044bf91(uchar param_1);
};

} // namespace std

#endif