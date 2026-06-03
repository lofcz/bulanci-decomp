#include "CGameView.h"

// !FUNC 0x00416f30 BEGIN
/* 416F30-416F36 00006 */
uchar* CGameView::CGameView_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b3804);
}
// !FUNC 0x00416f30 END

// !FUNC 0x00416f50 BEGIN
/* 416F50-416F57 00007 */
int CGameView::CGameView_GetSpatialBucketKey() {
    return *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x6c)
         + *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x2c);
}
// !FUNC 0x00416f50 END

// !FUNC 0x00417030 BEGIN
/* 417030-4170D7 000A7 */
uchar CGameView::CShot_SubobjectCtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417030 END

// !FUNC 0x00417210 BEGIN
/* 417210-417256 00046 */
uchar CGameView::CGameView_GetWorldCollisionRect(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00417210 END

// !FUNC 0x00417f40 BEGIN
/* 417F40-417F74 00034 */
uint CGameView::CGameView_LookupBulletFrameStrip(uchar param_1, int param_2, uchar param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00417f40 END

// !FUNC 0x00418290 BEGIN
/* 418290-4182C9 00039 */
uchar CGameView::CLevelScript_FireOnSlotPlaced_FromView(uchar param_1, uchar param_2, uchar param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00418290 END

// !FUNC 0x004182d0 BEGIN
/* 4182D0-4182FD 0002D */
uchar CGameView::CGameView_FireOnSlotDisplaced(uchar param_1, uchar param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004182d0 END

// !FUNC 0x00418c60 BEGIN
/* 418C60-418D00 000A0 */
void* CGameView::CGameView_CreateObject() { STUB_BODY(); return 0; }
// !FUNC 0x00418c60 END


// !FUNC 0x00419010 BEGIN
/* 419010-419066 00056 */
void CGameView::CGameView_Update(int* param_1) { STUB_BODY(); }
// !FUNC 0x00419010 END

// !FUNC 0x004191a0 BEGIN
/* 4191A0-419273 000D3 */
void* CGameView::CGameView_ctor(int param_1, int param_2, int* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004191a0 END

// !FUNC 0x0041acf0 BEGIN
/* 41ACF0-41AD7D 0008D */
void CGameView::CGameView_OnEvent(short param_1, uint param_2, uint* param_3) { STUB_BODY(); }
// !FUNC 0x0041acf0 END


// !FUNC 0x00439720 BEGIN
/* 439720-43972A 0000A */
void CGameView::TM_SetFrameDelayOverrideMs(int param_1) {
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x44) = param_1;
}
// !FUNC 0x00439720 END

// !FUNC 0x00416590 BEGIN
/* 416590-4165A6 00016 */
void CGameView::CGameView_InitGamingFields(void* param_1) { STUB_BODY(); }
// !FUNC 0x00416590 END

// !FUNC 0x00419070 BEGIN
/* 419070-419115 000A5 */
CAnim* CGameView::CBitmap_ViewHeader_Init(CAnim* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00419070 END

// !FUNC 0x00419130 BEGIN
/* 419130-41913B 0000B */
void CGameView::CBitmap_OnDraw() { STUB_BODY(); }
// !FUNC 0x00419130 END

// !FUNC 0x00419280 BEGIN
/* 419280-4192AD 0002D */
void CGameView::CBitmap_FireOnBitmapEvtFromView(uint param_1, ushort param_2) { STUB_BODY(); }
// !FUNC 0x00419280 END

// !FUNC 0x0042c160 BEGIN
/* 42C160-42C185 00025 */
uchar CGameView::CGameView_InsertBeforeSiblingAnchor(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c160 END

// !FUNC 0x0042c190 BEGIN
/* 42C190-42C1B5 00025 */
uchar CGameView::CGameView_RemoveFromSiblingAnchor(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042c190 END

