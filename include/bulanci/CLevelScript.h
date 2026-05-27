#ifndef _C_LEVEL_SCRIPT
#define _C_LEVEL_SCRIPT

#include <globals.h>

class CLevelScript {
public:
	// !DECL 0x00416a90 BEGIN
	/* 416A90 */ uchar CLevelScriptTimer_FireOnTimer(int param_1);
	// !DECL 0x00416a90 END
	// !DECL 0x004185c0 BEGIN
	/* 4185C0 */ CDSScript* ctor(CDSScript* param_1);
	// !DECL 0x004185c0 END
	// !DECL 0x00418680 BEGIN
	/* 418680 */ uchar* FUN_00418680();
	// !DECL 0x00418680 END
	// !DECL 0x00418690 BEGIN
	/* 418690 */ uchar FUN_00418690(uchar param_1);
	// !DECL 0x00418690 END
	// !DECL 0x004186a0 BEGIN
	/* 4186A0 */ uchar FUN_004186a0(uchar param_1);
	// !DECL 0x004186a0 END
	// !DECL 0x004186b0 BEGIN
	/* 4186B0 */ uchar FUN_004186b0(uchar param_1);
	// !DECL 0x004186b0 END
	// !DECL 0x004186c0 BEGIN
	/* 4186C0 */ uchar FUN_004186c0(int param_1);
	// !DECL 0x004186c0 END
	// !DECL 0x004186d0 BEGIN
	/* 4186D0 */ uchar FUN_004186d0(uchar param_1);
	// !DECL 0x004186d0 END
	// !DECL 0x004186e0 BEGIN
	/* 4186E0 */ uchar FUN_004186e0(int param_1);
	// !DECL 0x004186e0 END
	// !DECL 0x004186f0 BEGIN
	/* 4186F0 */ uchar FUN_004186f0(uint* param_1);
	// !DECL 0x004186f0 END
	// !DECL 0x00418be0 BEGIN
	/* 418BE0 */ uchar FUN_00418be0(int param_1);
	// !DECL 0x00418be0 END
	// !DECL 0x00418bf0 BEGIN
	/* 418BF0 */ uchar FUN_00418bf0(int param_1);
	// !DECL 0x00418bf0 END
	// !DECL 0x0041a450 BEGIN
	/* 41A450 */ void* FUN_0041a450(uchar param_1);
	// !DECL 0x0041a450 END
	// !DECL 0x004216c0 BEGIN
	/* 4216C0 */ uchar FUN_004216c0(int param_1);
	// !DECL 0x004216c0 END
	// !DECL 0x00421810 BEGIN
	/* 421810 */ uchar FUN_00421810(int param_1);
	// !DECL 0x00421810 END
	// !DECL 0x00421820 BEGIN
	/* 421820 */ uchar FUN_00421820(int param_1);
	// !DECL 0x00421820 END
	// !DECL 0x00422770 BEGIN
	/* 422770 */ uchar FUN_00422770(int param_1);
	// !DECL 0x00422770 END
	// !DECL 0x00438e40 BEGIN
	/* 438E40 */ uchar FUN_00438e40(int* param_1);
	// !DECL 0x00438e40 END

	// !DECL 0x00416ab0 BEGIN
	/* 416AB0 */ static int CLevelScriptOpExt_SetActive(int param_1);
	// !DECL 0x00416ab0 END
	// !DECL 0x00416ae0 BEGIN
	/* 416AE0 */ static uchar CLevelScriptOpExt_SetInsertMode(int param_1);
	// !DECL 0x00416ae0 END
	// !DECL 0x00416b10 BEGIN
	/* 416B10 */ static int* CLevelScriptOpExt_HideView(int param_1);
	// !DECL 0x00416b10 END
	// !DECL 0x00416b30 BEGIN
	/* 416B30 */ static int* CLevelScriptOpExt_ShowView(int param_1);
	// !DECL 0x00416b30 END
	// !DECL 0x00416b50 BEGIN
	/* 416B50 */ static uchar CLevelScriptOpExt_GetSlot(int param_1);
	// !DECL 0x00416b50 END
	// !DECL 0x00416b70 BEGIN
	/* 416B70 */ static int CLevelScriptOpExt_SetAnim(int param_1);
	// !DECL 0x00416b70 END
	// !DECL 0x00416bb0 BEGIN
	/* 416BB0 */ static int CLevelScriptOpExt_AnimResume(int param_1);
	// !DECL 0x00416bb0 END
	// !DECL 0x00416bd0 BEGIN
	/* 416BD0 */ static int CLevelScriptOpExt_SetAnimFrame(int param_1);
	// !DECL 0x00416bd0 END
	// !DECL 0x00416c00 BEGIN
	/* 416C00 */ static uint CLevelScriptOpExt_RegisterTimer(int param_1);
	// !DECL 0x00416c00 END
	// !DECL 0x00416c40 BEGIN
	/* 416C40 */ static uint CLevelScriptOpExt_TimerStop(int param_1);
	// !DECL 0x00416c40 END
	// !DECL 0x00416c70 BEGIN
	/* 416C70 */ static uint CLevelScriptOpExt_TimerStart(int param_1);
	// !DECL 0x00416c70 END
	// !DECL 0x00416c90 BEGIN
	/* 416C90 */ static uint CLevelScriptOpExt_TimerRelease(int param_1);
	// !DECL 0x00416c90 END
	// !DECL 0x00416cb0 BEGIN
	/* 416CB0 */ static uint CLevelScriptOpExt_TimerSetData(int param_1);
	// !DECL 0x00416cb0 END
	// !DECL 0x00416d30 BEGIN
	/* 416D30 */ static bool CLevelScriptOpExt_IsServer(int param_1);
	// !DECL 0x00416d30 END
	// !DECL 0x00416d50 BEGIN
	/* 416D50 */ static int* CLevelScriptOpExt_StrmSend(int param_1);
	// !DECL 0x00416d50 END
	// !DECL 0x00416d70 BEGIN
	/* 416D70 */ static int* CLevelScriptOpExt_SetCommStrm(int param_1);
	// !DECL 0x00416d70 END
	// !DECL 0x00416d90 BEGIN
	/* 416D90 */ static char CLevelScriptOpExt_IsNet(int param_1);
	// !DECL 0x00416d90 END
	// !DECL 0x00416dc0 BEGIN
	/* 416DC0 */ static int CLevelScriptOpExt_PlayAnim(int param_1);
	// !DECL 0x00416dc0 END
	// !DECL 0x00416df0 BEGIN
	/* 416DF0 */ static uint* CLevelScriptOpExt_SeekAnim(int param_1);
	// !DECL 0x00416df0 END
	// !DECL 0x00416e40 BEGIN
	/* 416E40 */ static CDSCollection* CLevelScriptOpExt_CollResize(int param_1);
	// !DECL 0x00416e40 END
	// !DECL 0x00416e80 BEGIN
	/* 416E80 */ static void* CLevelScriptOpExt_CollRemove(int param_1);
	// !DECL 0x00416e80 END
	// !DECL 0x00416ee0 BEGIN
	/* 416EE0 */ static uint CLevelScriptOpExt_FreeObject(int param_1);
	// !DECL 0x00416ee0 END
	// !DECL 0x00416f00 BEGIN
	/* 416F00 */ static int CLevelScriptOpExt_EnableFireThrough(int param_1);
	// !DECL 0x00416f00 END
	// !DECL 0x00418770 BEGIN
	/* 418770 */ static int CLevelScriptOpExt_BindToSlot(int param_1);
	// !DECL 0x00418770 END
	// !DECL 0x004187a0 BEGIN
	/* 4187A0 */ static int CLevelScriptOpExt_SetObstacleBounds(int param_1);
	// !DECL 0x004187a0 END
	// !DECL 0x00418800 BEGIN
	/* 418800 */ static CObstacle* CLevelScriptOpExt_CreateObstacle(int param_1);
	// !DECL 0x00418800 END
	// !DECL 0x004188b0 BEGIN
	/* 4188B0 */ static CBulanek* CLevelScriptOpExt_ResortDepth(int param_1);
	// !DECL 0x004188b0 END
	// !DECL 0x004188e0 BEGIN
	/* 4188E0 */ static CBulanek* CLevelScriptOpExt_TranslateTo(int param_1);
	// !DECL 0x004188e0 END
	// !DECL 0x00418920 BEGIN
	/* 418920 */ static uint CLevelScriptOpExt_EvalSeq3(int param_1);
	// !DECL 0x00418920 END
	// !DECL 0x00418950 BEGIN
	/* 418950 */ static uint CLevelScriptOpExt_SpawnAtView(int param_1);
	// !DECL 0x00418950 END
	// !DECL 0x00418980 BEGIN
	/* 418980 */ static int CLevelScriptOpExt_SetAnimDirection(int param_1);
	// !DECL 0x00418980 END
	// !DECL 0x004189b0 BEGIN
	/* 4189B0 */ static uint CLevelScriptOpExt_MapSet(int param_1);
	// !DECL 0x004189b0 END
	// !DECL 0x004189e0 BEGIN
	/* 4189E0 */ static uint CLevelScriptOpExt_MapGet(int param_1);
	// !DECL 0x004189e0 END
	// !DECL 0x00418a00 BEGIN
	/* 418A00 */ static bool CLevelScriptOpExt_IsViewKind(int param_1);
	// !DECL 0x00418a00 END
	// !DECL 0x00418a50 BEGIN
	/* 418A50 */ uint* CLevelScriptOpExt_NewCollection();
	// !DECL 0x00418a50 END
	// !DECL 0x00418a90 BEGIN
	/* 418A90 */ static uint CLevelScriptOpExt_GetField0C(int param_1);
	// !DECL 0x00418a90 END
	// !DECL 0x00418ab0 BEGIN
	/* 418AB0 */ static uint CLevelScriptOpExt_ArrayGet(int param_1);
	// !DECL 0x00418ab0 END
	// !DECL 0x00418ae0 BEGIN
	/* 418AE0 */ static int CLevelScriptOpExt_ArraySet(int param_1);
	// !DECL 0x00418ae0 END
	// !DECL 0x00418b20 BEGIN
	/* 418B20 */ static void* CLevelScriptOpExt_CollInsert(int param_1);
	// !DECL 0x00418b20 END
	// !DECL 0x00418b60 BEGIN
	/* 418B60 */ static uchar CLevelScriptOpExt_GetImage(int param_1);
	// !DECL 0x00418b60 END
	// !DECL 0x0041a470 BEGIN
	/* 41A470 */ static void* CLevelScriptOpExt_SetOrderAxis(int param_1);
	// !DECL 0x0041a470 END
	// !DECL 0x0041a4a0 BEGIN
	/* 41A4A0 */ static CBulanek* CLevelScriptOpExt_InsertView(int param_1);
	// !DECL 0x0041a4a0 END
	// !DECL 0x0041a4d0 BEGIN
	/* 41A4D0 */ static int* CLevelScriptOpExt_RemoveView(int param_1);
	// !DECL 0x0041a4d0 END
	// !DECL 0x0041bb00 BEGIN
	/* 41BB00 */ static uint CLevelScriptOpExt_SetMusic(int param_1);
	// !DECL 0x0041bb00 END
	// !DECL 0x0041bb30 BEGIN
	/* 41BB30 */ static uint CLevelScriptOpExt_DefineDangerZone(int param_1);
	// !DECL 0x0041bb30 END
	// !DECL 0x0041bb80 BEGIN
	/* 41BB80 */ static uint CLevelScriptOpExt_DefineTraceArea(int param_1);
	// !DECL 0x0041bb80 END
	// !DECL 0x0041d6e0 BEGIN
	/* 41D6E0 */ static CAnim* CLevelScriptOpExt_CreateAnim(int param_1);
	// !DECL 0x0041d6e0 END
	// !DECL 0x0041d820 BEGIN
	/* 41D820 */ static CGameView* CLevelScriptOpExt_CreateImage(int param_1);
	// !DECL 0x0041d820 END
	// !DECL 0x0041d8f0 BEGIN
	/* 41D8F0 */ static uint CLevelScriptOpExt_LoadPreface(int param_1);
	// !DECL 0x0041d8f0 END
	// !DECL 0x0041d990 BEGIN
	/* 41D990 */ static int CLevelScriptOpExt_SetViewImage(int param_1);
	// !DECL 0x0041d990 END
	// !DECL 0x0041da50 BEGIN
	/* 41DA50 */ static CMina* CLevelScriptOpExt_CreateMine(int param_1);
	// !DECL 0x0041da50 END
	// !DECL 0x0041e3e0 BEGIN
	/* 41E3E0 */ static uint CLevelScriptOpExt_KillObject(int param_1);
	// !DECL 0x0041e3e0 END
	// !DECL 0x0041e430 BEGIN
	/* 41E430 */ static uint CLevelScriptOpExt_TeleportPlayerTo(int param_1);
	// !DECL 0x0041e430 END
	// !DECL 0x0041f610 BEGIN
	/* 41F610 */ static uint CLevelScriptOpExt_SpawnEnemyAt(int param_1);
	// !DECL 0x0041f610 END
	// !DECL 0x0041f6a0 BEGIN
	/* 41F6A0 */ static uint CLevelScriptOpExt_SpawnOpponentEx(int param_1);
	// !DECL 0x0041f6a0 END
	// !DECL 0x0041f730 BEGIN
	/* 41F730 */ static uint CLevelScriptOpExt_InsertOpponent(int param_1);
	// !DECL 0x0041f730 END
	// !DECL 0x00420610 BEGIN
	/* 420610 */ static uint CLevelScriptOpExt_InsertBulanci(int param_1);
	// !DECL 0x00420610 END
	// !DECL 0x00420630 BEGIN
	/* 420630 */ static uint CLevelScriptOpExt_InsertVampires(int param_1);
	// !DECL 0x00420630 END
};

#endif
