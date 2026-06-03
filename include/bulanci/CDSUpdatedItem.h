#ifndef _C_D_S_UPDATED_ITEM
#define _C_D_S_UPDATED_ITEM

#include <globals.h>

class CDSUpdatedItem {
public:
	// !DECL 0x0042ea70 BEGIN
	/* 42EA70 */ uchar* CDSUpdatedItem_GetTypeInfo();
	// !DECL 0x0042ea70 END
	// !DECL 0x0042f060 BEGIN
	/* 42F060 */ void CDSUpdatedItem_ctor(void* param_1);
	// !DECL 0x0042f060 END

	// !DECL 0x0042eaa0 BEGIN
	/* 42EAA0 */ bool Scheduler_IsSlotLive(uint param_1);
	// !DECL 0x0042eaa0 END
	// !DECL 0x0042eac0 BEGIN
	/* 42EAC0 */ void Scheduler_FreeSlotIfLive(uint param_1);
	// !DECL 0x0042eac0 END
	// !DECL 0x0042ebb0 BEGIN
	/* 42EBB0 */ void Scheduler_EnsureCapacity(uint param_1);
	// !DECL 0x0042ebb0 END
	// !DECL 0x0042f1e0 BEGIN
	/* 42F1E0 */ void* Scheduler_GetEventSlot(uint param_1);
	// !DECL 0x0042f1e0 END
	// !DECL 0x0042f210 BEGIN
	/* 42F210 */ void Scheduler_RegisterEventSlot(uint param_1, uint param_2, uint param_3);
	// !DECL 0x0042f210 END
	// !DECL 0x0042f290 BEGIN
	/* 42F290 */ void Scheduler_SetEventLastFireMs(uint param_1, int param_2);
	// !DECL 0x0042f290 END
	// !DECL 0x0042f2d0 BEGIN
	/* 42F2D0 */ void Scheduler_SetEventDelayMs(uint param_1, uint param_2);
	// !DECL 0x0042f2d0 END
	// !DECL 0x0042f300 BEGIN
	/* 42F300 */ uchar Scheduler_ArmSlot(uint param_1);
	// !DECL 0x0042f300 END
	// !DECL 0x0042f330 BEGIN
	/* 42F330 */ uchar Scheduler_AckSlot(uint param_1, int param_2);
	// !DECL 0x0042f330 END
};

#endif
