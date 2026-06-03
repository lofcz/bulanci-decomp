#ifndef _C_PANEL
#define _C_PANEL

#include <globals.h>

class CPanel {
public:
	// !DECL 0x00412550 BEGIN
	/* 412550 */ uchar CPanel_FindPlayerSlot(uchar param_1);
	// !DECL 0x00412550 END
	// !DECL 0x00426ce0 BEGIN
	/* 426CE0 */ void CPanel_OnEvent(short param_1);
	// !DECL 0x00426ce0 END
	// !DECL 0x00427510 BEGIN
	/* 427510 */ void* CPanel_ctor(void* param_1);
	// !DECL 0x00427510 END
	// !DECL 0x00427590 BEGIN
	/* 427590 */ uchar* CPanel_GetTypeDescriptor();
	// !DECL 0x00427590 END
	// !DECL 0x004275a0 BEGIN
	/* 4275A0 */ uchar CPanel_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x004275a0 END
	// !DECL 0x004275b0 BEGIN
	/* 4275B0 */ uchar CPanel_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x004275b0 END
	// !DECL 0x004275c0 BEGIN
	/* 4275C0 */ uchar CPanel_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x004275c0 END
	// !DECL 0x004275d0 BEGIN
	/* 4275D0 */ void CPanel_dtor(uchar param_1);
	// !DECL 0x004275d0 END
	// !DECL 0x00427640 BEGIN
	/* 427640 */ void CPanel_Render(char param_1);
	// !DECL 0x00427640 END
	// !DECL 0x00427c10 BEGIN
	/* 427C10 */ void* CPanel_vDtor(uchar param_1);
	// !DECL 0x00427c10 END
};

#endif
