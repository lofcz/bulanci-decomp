#ifndef TYPE_INFO
#define TYPE_INFO

#include <globals.h>

class type_info {
public:
	/* 44734D */ ~type_info();
	/* 44735B */ void* scalar_deleting_destructor(uint param_1);
	/* 447377 */ bool operator==(void* param_1);
	/* 4498D0 */ static void _Type_info_dtor(void* param_1);
};

#endif