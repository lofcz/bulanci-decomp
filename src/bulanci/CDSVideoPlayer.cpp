#include "CDSVideoPlayer.h"

// !FUNC 0x00439b90 BEGIN
/* 439B90-439BB8 00028 */
uchar CDSVideoPlayer::CDSVideoPlayer_AdvanceFrameAndPauseIfDone(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439b90 END

// !FUNC 0x00439d10 BEGIN
/* 439D10-439D16 00006 */
uchar* CDSVideoPlayer::CDSVideoPlayer_GetTypeInfo() {
    return reinterpret_cast<uchar*>(&DAT_004b834c);
}
// !FUNC 0x00439d10 END

// !FUNC 0x00439d20 BEGIN
/* 439D20-439D28 00008 */
uchar CDSVideoPlayer::CDSVideoPlayer_DtorScalar(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439d20 END

// !FUNC 0x00439d30 BEGIN
/* 439D30-439DF5 000C5 */
uchar CDSVideoPlayer::CDSVideoPlayer_TM_Destructor(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439d30 END

// !FUNC 0x00439fc0 BEGIN
/* 439FC0-439FDE 0001E */
void* CDSVideoPlayer::CDSVideoPlayer_ScalarDeletingDtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439fc0 END

// !FUNC 0x00439710 BEGIN
/* 439710-43971A 0000A */
uchar CDSVideoPlayer::CDSVideoPlayer_SetNotifyCookie(uint param_1) {
    *reinterpret_cast<uint*>(reinterpret_cast<char*>(this) + 0x3c) = param_1;
    return static_cast<uchar>(param_1);
}
// !FUNC 0x00439710 END

// !FUNC 0x00439940 BEGIN
/* 439940-439965 00025 */
void CDSVideoPlayer::TM_Play(uchar param_1) { STUB_BODY(); }
// !FUNC 0x00439940 END

// !FUNC 0x00439c70 BEGIN
/* 439C70-439D06 00096 */
void* CDSVideoPlayer::ConstructTrackManager(void* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00439c70 END

