#include "CWeapon.h"

// !PROLOGUE BEGIN
#include "_Globals.h"
// !PROLOGUE END

// !FUNC 0x00416750 BEGIN
/* 416750-41676C 0001C */
uchar CWeapon::CWeapon_ReleaseViaVtable(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416750 END

// !FUNC 0x004179a0 BEGIN
/* 4179A0-4179BC 0001C */
void CWeapon::SetAmmo(int param_1) {
    _Globals::TriggerSoundEffect(param_1, reinterpret_cast<char*>(this)[100], *reinterpret_cast<int**>(reinterpret_cast<char*>(this) + 0x54) + 8);
}
// !FUNC 0x004179a0 END

// !FUNC 0x00417a00 BEGIN
/* 417A00-417A13 00013 */
void CWeapon::CWeapon_UpdateStateFromParams(uint param_1, uint param_2, int param_3) {
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x58) = *reinterpret_cast<int*>(param_3 + 4);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x5c) = *reinterpret_cast<int*>(param_3 + 8);
}
// !FUNC 0x00417a00 END

// !FUNC 0x00418f60 BEGIN
/* 418F60-418F66 00006 */
uchar* CWeapon::CWeapon_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b3754);
}
// !FUNC 0x00418f60 END

// !FUNC 0x0041bf00 BEGIN
/* 41BF00-41BF66 00066 */
void CWeapon::Update(uchar param_1) { STUB_BODY(); }
// !FUNC 0x0041bf00 END

// !FUNC 0x0041bf70 BEGIN
/* 41BF70-41BF7F 0000F */
uchar CWeapon::CWeapon_HideAssociatedView(int param_1) {
    CWeapon* weapon = *reinterpret_cast<CWeapon**>(reinterpret_cast<char*>(param_1) + 0x50);
    if (weapon != NULL) {
        weapon->Update(reinterpret_cast<int*>(weapon));
    }
    return 0;
}
// !FUNC 0x0041bf70 END

// !FUNC 0x0041c550 BEGIN
/* 41C550-41C5F1 000A1 */
uchar CWeapon::CWeapon_dtor(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041c550 END

// !FUNC 0x0041c600 BEGIN
/* 41C600-41C610 00010 */
uchar CWeapon::Init(uint param_1, int* param_2) {
    reinterpret_cast<_Globals*>(reinterpret_cast<char*>(this) - 16)->CWeapon_SetTrackHolder(param_2);
    return 0;
}
// !FUNC 0x0041c600 END

// !FUNC 0x0041dae0 BEGIN
/* 41DAE0-41DAFE 0001E */
void* CWeapon::CWeapon_ScalarDeletingDtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041dae0 END

// !FUNC 0x004212b0 BEGIN
/* 4212B0-4215AD 002FD */
uchar CWeapon::CWeapon_Fire(uint param_1, ushort param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004212b0 END

// !FUNC 0x0041bf80 BEGIN
/* 41BF80-41C003 00083 */
void CWeapon::CWeapon_SetTrackHolder(CBulanek* param_1) { STUB_BODY(); }
// !FUNC 0x0041bf80 END

// !FUNC 0x0041dbc0 BEGIN
/* 41DBC0-41DD6D 001AD */
void CWeapon::CWeapon_ctor(CBulanek* param_1, uchar param_2) { STUB_BODY(); }
// !FUNC 0x0041dbc0 END

// !FUNC 0x0041fce0 BEGIN
/* 41FCE0-41FDE4 00104 */
void CWeapon::CWeapon_FirePistol() { STUB_BODY(); }
// !FUNC 0x0041fce0 END

