#ifndef _C_D_S_COLLECTION
#define _C_D_S_COLLECTION

#include <globals.h>

class CDSCollection {
public:
	// !DECL 0x00401230 BEGIN
	/* 401230 */ uchar* CDSCollection_GetTypeInfo();
	// !DECL 0x00401230 END
	// !DECL 0x00401240 BEGIN
	/* 401240 */ void* CDSCollection_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00401240 END
	// !DECL 0x00401270 BEGIN
	/* 401270 */ uchar CDSCollection_ScalarDeletingDtorThunk(uchar param_1);
	// !DECL 0x00401270 END
	// !DECL 0x0042fd40 BEGIN
	/* 42FD40 */ void* CDSCollection_DeserializeElement(CDSStreamStorage* param_1);
	// !DECL 0x0042fd40 END
	// !DECL 0x0042fdd1 BEGIN
	/* 42FDD1 */ uchar Catch_0042fdd1();
	// !DECL 0x0042fdd1 END
	// !DECL 0x0042ff20 BEGIN
	/* 42FF20 */ uchar CDSCollection_SerializeElement(uint* param_1);
	// !DECL 0x0042ff20 END
	// !DECL 0x00431000 BEGIN
	/* 431000 */ uchar CDSCollection_Resize(int param_1, char param_2);
	// !DECL 0x00431000 END
	// !DECL 0x00431210 BEGIN
	/* 431210 */ void CDSCollection_Save(int* param_1);
	// !DECL 0x00431210 END
	// !DECL 0x00431260 BEGIN
	/* 431260 */ uchar CDSCollection_ctor(void* param_1);
	// !DECL 0x00431260 END
	// !DECL 0x00431360 BEGIN
	/* 431360 */ void CDSCollection_Load(CDSStreamStorage* param_1);
	// !DECL 0x00431360 END
};

#endif
