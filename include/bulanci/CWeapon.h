#ifndef _C_WEAPON
#define _C_WEAPON

#include <globals.h>

class CWeapon {
public:
	// !DECL 0x00416750 BEGIN
	/* 416750 */ uchar CWeapon_ReleaseViaVtable(int* param_1);
	// !DECL 0x00416750 END
	// !DECL 0x004179a0 BEGIN
	/* 4179A0 */ void SetAmmo(int param_1);
	// !DECL 0x004179a0 END
	// !DECL 0x00417a00 BEGIN
	/* 417A00 */ void CWeapon_UpdateStateFromParams(uint param_1, uint param_2, int param_3);
	// !DECL 0x00417a00 END
	// !DECL 0x00418f60 BEGIN
	/* 418F60 */ uchar* CWeapon_GetClassTable();
	// !DECL 0x00418f60 END
	// !DECL 0x0041bf00 BEGIN
	/* 41BF00 */ void Update(uchar param_1);
	// !DECL 0x0041bf00 END
	// !DECL 0x0041bf70 BEGIN
	/* 41BF70 */ void CWeapon_HideAssociatedView();
	// !DECL 0x0041bf70 END
	// !DECL 0x0041c550 BEGIN
	/* 41C550 */ uchar CWeapon_dtor(uint* param_1);
	// !DECL 0x0041c550 END
	// !DECL 0x0041c600 BEGIN
	/* 41C600 */ uchar Init(uint param_1, int* param_2);
	// !DECL 0x0041c600 END
	// !DECL 0x0041dae0 BEGIN
	/* 41DAE0 */ void* CWeapon_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0041dae0 END
	// !DECL 0x004212b0 BEGIN
	/* 4212B0 */ uchar CWeapon_Fire(uint param_1, ushort param_2);
	// !DECL 0x004212b0 END

	// !DECL 0x0041bf80 BEGIN
	/* 41BF80 */ void CWeapon_SetTrackHolder(CBulanek* param_1);
	// !DECL 0x0041bf80 END
	// !DECL 0x0041dbc0 BEGIN
	/* 41DBC0 */ void CWeapon_ctor(CBulanek* param_1, uchar param_2);
	// !DECL 0x0041dbc0 END
	// !DECL 0x0041fce0 BEGIN
	/* 41FCE0 */ void CWeapon_FirePistol();
	// !DECL 0x0041fce0 END
};

#endif
