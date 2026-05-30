#include "CBulanek.h"

// !FUNC 0x00412850 BEGIN
/* 412850-412894 00044 */
uchar CBulanek::CBulanek_CopyScoreBlockToSlot(uchar param_1, uint param_2, uint* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00412850 END

// !FUNC 0x00412a40 BEGIN
/* 412A40-412A74 00034 */
uchar CBulanek::CGame_NetSendDamage_t0c(uchar param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00412a40 END

// !FUNC 0x00412b10 BEGIN
/* 412B10-412B5D 0004D */
uchar CBulanek::CGame_NetSendPlayerState_t0d(uchar param_1, uchar param_2, ushort* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00412b10 END

// !FUNC 0x00412c40 BEGIN
/* 412C40-412C6C 0002C */
uchar CBulanek::CGame_NetSendPlayerDie_t11(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412c40 END

// !FUNC 0x00416490 BEGIN
/* 416490-4164AC 0001C */
uint CBulanek::CBulanek_IsInKnockdownAnimBand(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416490 END

// !FUNC 0x00416630 BEGIN
/* 416630-41663A 0000A */
uint CBulanek::CBulanek_GetActiveWeaponKind(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416630 END

// !FUNC 0x00416690 BEGIN
/* 416690-4166B0 00020 */
uint CBulanek::CBulanek_GetDelayedQuipSlot(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416690 END

// !FUNC 0x004166b0 BEGIN
/* 4166B0-4166D0 00020 */
uint CBulanek::CBulanek_GetHitQuipSlot(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004166b0 END

// !FUNC 0x004166d0 BEGIN
/* 4166D0-4166FA 0002A */
uchar CBulanek::FUN_004166d0(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004166d0 END

// !FUNC 0x00416700 BEGIN
/* 416700-416717 00017 */
uchar CBulanek::FUN_00416700(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416700 END

// !FUNC 0x00416830 BEGIN
/* 416830-416859 00029 */
uchar CBulanek::CBulanek_PostScriptEvent(uchar param_1, ushort param_2, uint param_3, uint param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00416830 END

// !FUNC 0x004168c0 BEGIN
/* 4168C0-4168CD 0000D */
uchar CBulanek::CBulanek_SetCloneFlag(uchar param_1) {
    *(reinterpret_cast<uchar*>(this) + 0x341) = param_1;
    return param_1;
}
// !FUNC 0x004168c0 END

// !FUNC 0x004172d0 BEGIN
/* 4172D0-417375 000A5 */
uchar CBulanek::FUN_004172d0(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004172d0 END

// !FUNC 0x00417380 BEGIN
/* 417380-4173E2 00062 */
uchar CBulanek::CBulanek_OnShow(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417380 END

// !FUNC 0x00417410 BEGIN
/* 417410-417454 00044 */
void CBulanek::CBulanek_CopyCollisionRectLocal(int* param_1) { STUB_BODY(); }
// !FUNC 0x00417410 END

// !FUNC 0x00417460 BEGIN
/* 417460-41749E 0003E */
uchar CBulanek::CBulanek_OffsetCollisionRectByFacing(int* param_1, int* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00417460 END

// !FUNC 0x00417570 BEGIN
/* 417570-4175E6 00076 */
void CBulanek::CBulanek_ResetAmmoAndPlayReload(uchar param_1) { STUB_BODY(); }
// !FUNC 0x00417570 END

// !FUNC 0x004175f0 BEGIN
/* 4175F0-41763C 0004C */
uchar CBulanek::CBulanek_AdaptDisplaySize(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004175f0 END

// !FUNC 0x00417640 BEGIN
/* 417640-4178C4 00284 */
void CBulanek::CBulanek_Draw(void* param_1) { STUB_BODY(); }
// !FUNC 0x00417640 END

// !FUNC 0x004178d0 BEGIN
/* 4178D0-4178E8 00018 */
uchar CBulanek::CBulanek_SetAnimStateAndNotify(uint param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004178d0 END

// !FUNC 0x004178f0 BEGIN
/* 4178F0-417903 00013 */
uchar CBulanek::CBulanek_UpdateStateFromParams(uint param_1, uint param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004178f0 END

// !FUNC 0x00417910 BEGIN
/* 417910-41794F 0003F */
uchar CBulanek::FUN_00417910(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417910 END

// !FUNC 0x004179e0 BEGIN
/* 4179E0-4179F8 00018 */
uchar CBulanek::CBulanek_SetFacingFromByte(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004179e0 END

// !FUNC 0x00417e40 BEGIN
/* 417E40-417E76 00036 */
bool CBulanek::CGaming_IsPlayerSlotOccupied(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417e40 END

// !FUNC 0x00417f80 BEGIN
/* 417F80-417FAD 0002D */
uint CBulanek::CBulanek_GetKillerGenderByteAtSlot(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417f80 END

// !FUNC 0x004180e0 BEGIN
/* 4180E0-418255 00175 */
uchar CBulanek::FUN_004180e0(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004180e0 END

// !FUNC 0x004184a0 BEGIN
/* 4184A0-418595 000F5 */
uchar CBulanek::CGaming_InsertEntityByDepth(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004184a0 END

// !FUNC 0x00418e90 BEGIN
/* 418E90-418E96 00006 */
uchar* CBulanek::CBulanek_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b3734);
}
// !FUNC 0x00418e90 END

// !FUNC 0x00418ea0 BEGIN
/* 418EA0-418EAB 0000B */
uchar CBulanek::CBulanek_DtorScalar_thunk_Sub88(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00418ea0 END

// !FUNC 0x00418eb0 BEGIN
/* 418EB0-418EB8 00008 */
uchar CBulanek::CBulanek_DtorScalar_thunk_Sub10(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00418eb0 END

// !FUNC 0x00418ec0 BEGIN
/* 418EC0-418EC8 00008 */
uchar CBulanek::CBulanek_DtorScalar_thunk_Sub18(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00418ec0 END

// !FUNC 0x00418ee0 BEGIN
/* 418EE0-418EE8 00008 */
uchar CBulanek::CBulanek_DtorScalar_thunk_Sub4(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00418ee0 END

// !FUNC 0x004192b0 BEGIN
/* 4192B0-41947A 001CA */
uint CBulanek::CBulanek_CheckSlotCollision(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004192b0 END

// !FUNC 0x004194b0 BEGIN
/* 4194B0-4194C7 00017 */
uint CBulanek::CBulanek_TryApplyPickupMask(uint param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004194b0 END

// !FUNC 0x004194c7 BEGIN
/* 4194C7-419525 0005E */
uint CBulanek::CBulanek_CheckSlotMaskCollisions(uint param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004194c7 END

// !FUNC 0x00419560 BEGIN
/* 419560-4195B9 00059 */
int CBulanek::CBulanek_GetSpatialBucketKey(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00419560 END

// !FUNC 0x004195c0 BEGIN
/* 4195C0-4195E6 00026 */
uchar CBulanek::CBulanek_GetWorldCollisionRect(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004195c0 END

// !FUNC 0x004197b0 BEGIN
/* 4197B0-41986E 000BE */
uint CBulanek::CBulanek_SetFacingTrack(int param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004197b0 END

// !FUNC 0x00419a00 BEGIN
/* 419A00-419A79 00079 */
uchar CBulanek::CDeath_ctor(uint param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00419a00 END

// !FUNC 0x00419aa0 BEGIN
/* 419AA0-419AFF 0005F */
uchar CBulanek::CDeath2_ctor(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00419aa0 END

// !FUNC 0x0041a390 BEGIN
/* 41A390-41A441 000B1 */
void CBulanek::AddEntity(void* param_1, uchar param_2) { STUB_BODY(); }
// !FUNC 0x0041a390 END

// !FUNC 0x0041adb0 BEGIN
/* 41ADB0-41AECF 0011F */
uchar CBulanek::CBulanek_dtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041adb0 END

// !FUNC 0x0041aed0 BEGIN
/* 41AED0-41AF6F 0009F */
uchar CBulanek::CBulanek_SchedulerTick(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041aed0 END

// !FUNC 0x0041b180 BEGIN
/* 41B180-41B18E 0000E */
uchar CBulanek::CWeapon_OnSchedulerEvent(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041b180 END

// !FUNC 0x0041bc20 BEGIN
/* 41BC20-41BC3E 0001E */
uchar CBulanek::CBulanek_DtorScalar(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041bc20 END

// !FUNC 0x0041c610 BEGIN
/* 41C610-41C857 00247 */
uchar CBulanek::CBulanek_ResolveAndBindAnimTrack(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041c610 END

// !FUNC 0x0041c860 BEGIN
/* 41C860-41C993 00133 */
void CBulanek::CBulanek_BindDeathTombstoneAnim(void* param_1) { STUB_BODY(); }
// !FUNC 0x0041c860 END

// !FUNC 0x0041d090 BEGIN
/* 41D090-41D240 001B0 */
uint CBulanek::CBulanek_CreateRespawnTeleportPair(int* param_1, int param_2, int param_3, char param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0041d090 END

// !FUNC 0x0041db00 BEGIN
/* 41DB00-41DBB9 000B9 */
uint CBulanek::CBulanek_OnTakeDamage(int param_1, int param_2, char param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0041db00 END

// !FUNC 0x0041ea90 BEGIN
/* 41EA90-41EB9D 0010D */
uchar CBulanek::CBulanek_ApplyPickupEffect(ODSImage param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041ea90 END

// !FUNC 0x0041eca0 BEGIN
/* 41ECA0-41ED01 00061 */
uchar CBulanek::CBulanek_CycleWeaponPickup(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041eca0 END

// !FUNC 0x0041ed10 BEGIN
/* 41ED10-41ED51 00041 */
uchar CBulanek::FUN_0041ed10(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041ed10 END

// !FUNC 0x0041f900 BEGIN
/* 41F900-41FB8E 0028E */
uchar CBulanek::CBulanek_OnDeath(uchar param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0041f900 END

// !FUNC 0x0041fb90 BEGIN
/* 41FB90-41FCA0 00110 */
void CBulanek::CBulanek_Update(int* param_1) { STUB_BODY(); }
// !FUNC 0x0041fb90 END

// !FUNC 0x00420910 BEGIN
/* 420910-4209A5 00095 */
uchar CBulanek::CBulanek_ApplyAction(int param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00420910 END

// !FUNC 0x00420a90 BEGIN
/* 420A90-420B24 00094 */
uchar CBulanek::CBulanek_TryBotRandomAction(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00420a90 END

// !FUNC 0x00420b30 BEGIN
/* 420B30-420D21 001F1 */
void CBulanek::CBulanek_WeaponSchedulerCallback(uint param_1) { STUB_BODY(); }
// !FUNC 0x00420b30 END

// !FUNC 0x00420d40 BEGIN
/* 420D40-421185 00445 */
uchar CBulanek::CBulanek_OnEvent(ushort param_1, uint* param_2, uint* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00420d40 END

// !FUNC 0x00422470 BEGIN
/* 422470-422493 00023 */
uchar CBulanek::FUN_00422470(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00422470 END

// !FUNC 0x0042c160 BEGIN
/* 42C160-42C185 00025 */
uchar CBulanek::FUN_0042c160(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c160 END

// !FUNC 0x0042c190 BEGIN
/* 42C190-42C1B5 00025 */
uchar CBulanek::FUN_0042c190(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c190 END


// !FUNC 0x0042eac0 BEGIN
/* 42EAC0-42EAF2 00032 */
void CBulanek::Scheduler_FreeSlotIfLive(uint param_1) { STUB_BODY(); }
// !FUNC 0x0042eac0 END

// !FUNC 0x0042f880 BEGIN
/* 42F880-42F88A 0000A */
uchar CBulanek::FUN_0042f880(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f880 END

// !FUNC 0x0042f890 BEGIN
/* 42F890-42F89A 0000A */
uchar CBulanek::FUN_0042f890(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042f890 END

// !FUNC 0x0042fa20 BEGIN
/* 42FA20-42FA44 00024 */
uchar CBulanek::FUN_0042fa20(uchar param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042fa20 END

// !FUNC 0x0042fa50 BEGIN
/* 42FA50-42FA75 00025 */
uchar CBulanek::FUN_0042fa50(uchar param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042fa50 END

// !FUNC 0x0042cc80 BEGIN
/* 42CC80-42CCCB 0004B */
uchar CBulanek::CDSView::SetPosition(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0042cc80 END

