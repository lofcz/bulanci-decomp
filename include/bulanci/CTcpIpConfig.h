#ifndef _C_TCP_IP_CONFIG
#define _C_TCP_IP_CONFIG

#include <globals.h>

#include "CWindow.h"

class CTcpIpConfig : public CWindow {
public:
	// !DECL 0x0040c060 BEGIN
	/* 40C060 */ CTcpIpConfig();
	// !DECL 0x0040c060 END
	// !DECL 0x0040c2c0 BEGIN
	/* 40C2C0 */ uchar* GetClassMeta();
	// !DECL 0x0040c2c0 END
};

#endif
