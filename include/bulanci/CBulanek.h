#ifndef _C_BULANEK
#define _C_BULANEK

#include <globals.h>

class CBulanek {
public:
	// !DECL 0x00412850 BEGIN
	/* 412850 */ uchar CBulanek_CopyScoreBlockToSlot(uchar param_1, uint param_2, uint* param_3);
	// !DECL 0x00412850 END
	// !DECL 0x00412a40 BEGIN
	/* 412A40 */ uchar CGame_NetSendDamage_t0c(uchar param_1, uchar param_2);
	// !DECL 0x00412a40 END
	// !DECL 0x00412b10 BEGIN
	/* 412B10 */ uchar CGame_NetSendPlayerState_t0d(uchar param_1, uchar param_2, ushort* param_3);
	// !DECL 0x00412b10 END
	// !DECL 0x00412c40 BEGIN
	/* 412C40 */ uchar CGame_NetSendPlayerDie_t11(uchar param_1);
	// !DECL 0x00412c40 END
	// !DECL 0x00416490 BEGIN
	/* 416490 */ uint CBulanek_IsInKnockdownAnimBand(int param_1);
	// !DECL 0x00416490 END
	// !DECL 0x00416630 BEGIN
	/* 416630 */ uint CBulanek_GetActiveWeaponKind(int param_1);
	// !DECL 0x00416630 END
	// !DECL 0x00416690 BEGIN
	/* 416690 */ uint CBulanek_GetDelayedQuipSlot(int param_1);
	// !DECL 0x00416690 END
	// !DECL 0x004166b0 BEGIN
	/* 4166B0 */ uint CBulanek_GetHitQuipSlot(int param_1);
	// !DECL 0x004166b0 END
	// !DECL 0x004166d0 BEGIN
	/* 4166D0 */ uchar FUN_004166d0(int* param_1);
	// !DECL 0x004166d0 END
	// !DECL 0x00416700 BEGIN
	/* 416700 */ uchar FUN_00416700(void* param_1);
	// !DECL 0x00416700 END
	// !DECL 0x00416830 BEGIN
	/* 416830 */ uchar CBulanek_PostScriptEvent(uchar param_1, ushort param_2, uint param_3, uint param_4);
	// !DECL 0x00416830 END
	// !DECL 0x004168c0 BEGIN
	/* 4168C0 */ uchar CBulanek_SetCloneFlag(uchar param_1);
	// !DECL 0x004168c0 END
	// !DECL 0x004172d0 BEGIN
	/* 4172D0 */ uchar FUN_004172d0(int param_1);
	// !DECL 0x004172d0 END
	// !DECL 0x00417380 BEGIN
	/* 417380 */ uchar CBulanek_OnShow(char param_1);
	// !DECL 0x00417380 END
	// !DECL 0x00417410 BEGIN
	/* 417410 */ void CBulanek_CopyCollisionRectLocal(int* param_1);
	// !DECL 0x00417410 END
	// !DECL 0x00417460 BEGIN
	/* 417460 */ uchar CBulanek_OffsetCollisionRectByFacing(int* param_1, int* param_2, int param_3);
	// !DECL 0x00417460 END
	// !DECL 0x00417570 BEGIN
	/* 417570 */ void CBulanek_ResetAmmoAndPlayReload(uchar param_1);
	// !DECL 0x00417570 END
	// !DECL 0x004175f0 BEGIN
	/* 4175F0 */ uchar CBulanek_AdaptDisplaySize(uint* param_1);
	// !DECL 0x004175f0 END
	// !DECL 0x00417640 BEGIN
	/* 417640 */ void CBulanek_Draw(void* param_1);
	// !DECL 0x00417640 END
	// !DECL 0x004178d0 BEGIN
	/* 4178D0 */ uchar CBulanek_SetAnimStateAndNotify(uint param_1, uint* param_2);
	// !DECL 0x004178d0 END
	// !DECL 0x004178f0 BEGIN
	/* 4178F0 */ uchar CBulanek_UpdateStateFromParams(uint param_1, uint param_2, int param_3);
	// !DECL 0x004178f0 END
	// !DECL 0x00417910 BEGIN
	/* 417910 */ uchar FUN_00417910(uchar param_1);
	// !DECL 0x00417910 END
	// !DECL 0x004179e0 BEGIN
	/* 4179E0 */ uchar CBulanek_SetFacingFromByte(uchar param_1);
	// !DECL 0x004179e0 END
	// !DECL 0x00417e40 BEGIN
	/* 417E40 */ bool CGaming_IsPlayerSlotOccupied(uchar param_1);
	// !DECL 0x00417e40 END
	// !DECL 0x00417f80 BEGIN
	/* 417F80 */ uint CBulanek_GetKillerGenderByteAtSlot(uchar param_1);
	// !DECL 0x00417f80 END
	// !DECL 0x004180e0 BEGIN
	/* 4180E0 */ uchar FUN_004180e0(int param_1);
	// !DECL 0x004180e0 END
	// !DECL 0x004184a0 BEGIN
	/* 4184A0 */ uchar CGaming_InsertEntityByDepth(void* param_1);
	// !DECL 0x004184a0 END
	// !DECL 0x00418e90 BEGIN
	/* 418E90 */ uchar* CBulanek_GetClassTable();
	// !DECL 0x00418e90 END
	// !DECL 0x00418ea0 BEGIN
	/* 418EA0 */ uchar CBulanek_DtorScalar_thunk_Sub88(uchar param_1);
	// !DECL 0x00418ea0 END
	// !DECL 0x00418eb0 BEGIN
	/* 418EB0 */ uchar CBulanek_DtorScalar_thunk_Sub10(uchar param_1);
	// !DECL 0x00418eb0 END
	// !DECL 0x00418ec0 BEGIN
	/* 418EC0 */ uchar CBulanek_DtorScalar_thunk_Sub18(uchar param_1);
	// !DECL 0x00418ec0 END
	// !DECL 0x00418ee0 BEGIN
	/* 418EE0 */ uchar CBulanek_DtorScalar_thunk_Sub4(uchar param_1);
	// !DECL 0x00418ee0 END
	// !DECL 0x004192b0 BEGIN
	/* 4192B0 */ uint CBulanek_CheckSlotCollision(uchar param_1);
	// !DECL 0x004192b0 END
	// !DECL 0x004194b0 BEGIN
	/* 4194B0 */ uint CBulanek_TryApplyPickupMask(uint param_1, uint param_2);
	// !DECL 0x004194b0 END
	// !DECL 0x004194c7 BEGIN
	/* 4194C7 */ uint CBulanek_CheckSlotMaskCollisions(uint param_1, uint param_2, uint param_3);
	// !DECL 0x004194c7 END
	// !DECL 0x00419560 BEGIN
	/* 419560 */ int CBulanek_GetSpatialBucketKey(uchar param_1);
	// !DECL 0x00419560 END
	// !DECL 0x004195c0 BEGIN
	/* 4195C0 */ uchar CBulanek_GetWorldCollisionRect(int* param_1);
	// !DECL 0x004195c0 END
	// !DECL 0x004197b0 BEGIN
	/* 4197B0 */ uint CBulanek_SetFacingTrack(int param_1, char param_2);
	// !DECL 0x004197b0 END
	// !DECL 0x00419a00 BEGIN
	/* 419A00 */ uchar CDeath_ctor(uint param_1, uint param_2, uint param_3);
	// !DECL 0x00419a00 END
	// !DECL 0x00419aa0 BEGIN
	/* 419AA0 */ uchar CDeath2_ctor(uint param_1);
	// !DECL 0x00419aa0 END
	// !DECL 0x0041a390 BEGIN
	/* 41A390 */ void AddEntity(void* param_1, uchar param_2);
	// !DECL 0x0041a390 END
	// !DECL 0x0041adb0 BEGIN
	/* 41ADB0 */ uchar CBulanek_dtor(uchar param_1);
	// !DECL 0x0041adb0 END
	// !DECL 0x0041aed0 BEGIN
	/* 41AED0 */ uchar CBulanek_SchedulerTick(void* param_1);
	// !DECL 0x0041aed0 END
	// !DECL 0x0041b180 BEGIN
	/* 41B180 */ uchar CWeapon_OnSchedulerEvent(int param_1);
	// !DECL 0x0041b180 END
	// !DECL 0x0041bc20 BEGIN
	/* 41BC20 */ uchar CBulanek_DtorScalar(uchar param_1);
	// !DECL 0x0041bc20 END
	// !DECL 0x0041c610 BEGIN
	/* 41C610 */ uchar CBulanek_ResolveAndBindAnimTrack(void* param_1);
	// !DECL 0x0041c610 END
	// !DECL 0x0041c860 BEGIN
	/* 41C860 */ void CBulanek_BindDeathTombstoneAnim(void* param_1);
	// !DECL 0x0041c860 END
	// !DECL 0x0041d090 BEGIN
	/* 41D090 */ uint CBulanek_CreateRespawnTeleportPair(int* param_1, int param_2, int param_3, char param_4);
	// !DECL 0x0041d090 END
	// !DECL 0x0041db00 BEGIN
	/* 41DB00 */ uint CBulanek_OnTakeDamage(int param_1, int param_2, char param_3);
	// !DECL 0x0041db00 END
	// !DECL 0x0041ea90 BEGIN
	/* 41EA90 */ uchar CBulanek_ApplyPickupEffect(ODSImage param_1);
	// !DECL 0x0041ea90 END
	// !DECL 0x0041eca0 BEGIN
	/* 41ECA0 */ uchar CBulanek_CycleWeaponPickup(uchar param_1);
	// !DECL 0x0041eca0 END
	// !DECL 0x0041ed10 BEGIN
	/* 41ED10 */ uchar FUN_0041ed10(uchar param_1);
	// !DECL 0x0041ed10 END
	// !DECL 0x0041f900 BEGIN
	/* 41F900 */ uchar CBulanek_OnDeath(uchar param_1, uchar param_2);
	// !DECL 0x0041f900 END
	// !DECL 0x0041fb90 BEGIN
	/* 41FB90 */ void CBulanek_Update(int* param_1);
	// !DECL 0x0041fb90 END
	// !DECL 0x00420910 BEGIN
	/* 420910 */ uchar CBulanek_ApplyAction(int param_1, char param_2);
	// !DECL 0x00420910 END
	// !DECL 0x00420a90 BEGIN
	/* 420A90 */ uchar CBulanek_TryBotRandomAction(uchar param_1);
	// !DECL 0x00420a90 END
	// !DECL 0x00420b30 BEGIN
	/* 420B30 */ void CBulanek_WeaponSchedulerCallback(uint param_1);
	// !DECL 0x00420b30 END
	// !DECL 0x00420d40 BEGIN
	/* 420D40 */ uchar CBulanek_OnEvent(ushort param_1, uint* param_2, uint* param_3);
	// !DECL 0x00420d40 END
	// !DECL 0x00422470 BEGIN
	/* 422470 */ static uchar FUN_00422470(int* param_1);
	// !DECL 0x00422470 END
	// !DECL 0x0042c160 BEGIN
	/* 42C160 */ uchar FUN_0042c160(int param_1);
	// !DECL 0x0042c160 END
	// !DECL 0x0042c190 BEGIN
	/* 42C190 */ uchar FUN_0042c190(int param_1);
	// !DECL 0x0042c190 END
	// !DECL 0x0042eac0 BEGIN
	/* 42EAC0 */ void Scheduler_FreeSlotIfLive(uint param_1);
	// !DECL 0x0042eac0 END
	// !DECL 0x0042f880 BEGIN
	/* 42F880 */ uchar FUN_0042f880(int param_1);
	// !DECL 0x0042f880 END
	// !DECL 0x0042f890 BEGIN
	/* 42F890 */ uchar FUN_0042f890(int param_1);
	// !DECL 0x0042f890 END
	// !DECL 0x0042fa20 BEGIN
	/* 42FA20 */ uchar FUN_0042fa20(uchar param_1, int param_2);
	// !DECL 0x0042fa20 END
	// !DECL 0x0042fa50 BEGIN
	/* 42FA50 */ uchar FUN_0042fa50(uchar param_1, int param_2);
	// !DECL 0x0042fa50 END
};


class CDSView {
public:
	// !DECL 0x0042cc80 BEGIN
	/* 42CC80 */ uchar SetPosition(int param_1, int param_2);
	// !DECL 0x0042cc80 END
};

#endif
