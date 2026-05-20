#include "CTcpIpConfig.h"

// !PROLOGUE BEGIN
#include "CBulanci.h"
#include "CWindow.h"
#include "CStaticText.h"
#include "CEdit.h"
#include "CButton.h"
#include "_Globals.h"

#define g_pStaticTextsPool (*reinterpret_cast<const wchar_t***>(&PTR_PTR_004afbbc))
// !PROLOGUE END

// !FUNC 0x0040c060 BEGIN
/* 40C060-40C2BB 0025B */
CTcpIpConfig::CTcpIpConfig() : CWindow(0, 0, 0x138, 0xa0, 1) {
    uint* param_1 = reinterpret_cast<uint*>(this);

    CStaticText* staticText = reinterpret_cast<CStaticText*>(_Globals::FUN_00447c42(0x98));
    if (staticText != 0) {
        CBulanci staticTextStr(const_cast<short*>(reinterpret_cast<const short*>(g_pStaticTextsPool[31])));
        staticText->CStaticText_BuildAt(
            0x14, 0x14, 0x124, 0x3c,
            staticTextStr,
            1, 5, 0x100ae
        );
    }
    reinterpret_cast<_Globals*>(this)->FUN_0042d0b0(reinterpret_cast<int*>(staticText), 0);

    CEdit* edit = reinterpret_cast<CEdit*>(_Globals::FUN_00447c42(0xb8));
    if (edit != 0) {
        edit->CEdit_BuildAt(
            0x14, 0x46, 0x124, 0x3c,
            0x12, 2, 0, 0x100af
        );
    }
    reinterpret_cast<_Globals*>(this)->FUN_0042d0b0(reinterpret_cast<int*>(edit), 0);

    CButton* okBtn = reinterpret_cast<CButton*>(_Globals::FUN_00447c42(0x98));
    if (okBtn != 0) {
        CBulanci okBtnStr(const_cast<short*>(reinterpret_cast<const short*>(g_pStaticTextsPool[29])));
        okBtn->CButton_BuildAt(
            0x49, 0x73,
            okBtnStr,
            0x8002, 1, 0x14, 0
        );
    }
    reinterpret_cast<_Globals*>(this)->FUN_0042d0b0(reinterpret_cast<int*>(okBtn), 0);

    CButton* cancelBtn = reinterpret_cast<CButton*>(_Globals::FUN_00447c42(0x98));
    if (cancelBtn != 0) {
        CBulanci cancelBtnStr(const_cast<short*>(reinterpret_cast<const short*>(g_pStaticTextsPool[30])));
        cancelBtn->CButton_BuildAt(
            0xa3, 0x73,
            cancelBtnStr,
            0x8003, 0, 0x14, 0
        );
    }
    reinterpret_cast<_Globals*>(this)->FUN_0042d0b0(reinterpret_cast<int*>(cancelBtn), 0);

    int height = param_1[11] - param_1[9];
    int width = param_1[10] - param_1[8];

    *reinterpret_cast<ushort*>(reinterpret_cast<char*>(param_1) + 0x46) |= 8;

    param_1[11] = height;
    param_1[8] = 300;
    param_1[9] = 0;
    param_1[10] = width + 300;
}
// !FUNC 0x0040c060 END

// !FUNC 0x0040c2c0 BEGIN
/* 40C2C0-40C2C6 00006 */
uchar* CTcpIpConfig::GetClassMeta() {
    return reinterpret_cast<uchar*>(&DAT_004b34e8);
}
// !FUNC 0x0040c2c0 END

