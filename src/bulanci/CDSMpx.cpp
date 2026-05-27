#include "CDSMpx.h"

// !FUNC 0x00432f10 BEGIN
/* 432F10-432F16 00006 */
uchar* CDSMpx::FUN_00432f10() {
    return reinterpret_cast<uchar*>(&DAT_004b8510);
}
// !FUNC 0x00432f10 END

// !FUNC 0x00432f20 BEGIN
/* 432F20-432F28 00008 */
uchar CDSMpx::FUN_00432f20(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00432f20 END

// !FUNC 0x00432f30 BEGIN
/* 432F30-432F38 00008 */
uchar CDSMpx::FUN_00432f30(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00432f30 END

// !FUNC 0x00432f40 BEGIN
/* 432F40-432FBB 0007B */
uchar CDSMpx::FUN_00432f40(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00432f40 END

// !FUNC 0x00432fc0 BEGIN
/* 432FC0-432FDE 0001E */
void* CDSMpx::FUN_00432fc0(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00432fc0 END

// !FUNC 0x0043c380 BEGIN
/* 43C380-43C39B 0001B */
uchar CDSMpx::FUN_0043c380(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043c380 END

// !FUNC 0x0043c590 BEGIN
/* 43C590-43C619 00089 */
uchar CDSMpx::FUN_0043c590(uchar* param_1, uint param_2, char param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0043c590 END

// !FUNC 0x004465e0 BEGIN
/* 4465E0-446617 00037 */
uchar CDSMpx::ResetDecoderState(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004465e0 END

// !FUNC 0x00446640 BEGIN
/* 446640-446665 00025 */
uchar CDSMpx::ReinitDecoder(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00446640 END

// !FUNC 0x00446670 BEGIN
/* 446670-446699 00029 */
int CDSMpx::QuantizeQ31ToS16(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00446670 END

// !FUNC 0x004466a0 BEGIN
/* 4466A0-4466BA 0001A */
uchar CDSMpx::FUN_004466a0(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004466a0 END

// !FUNC 0x004466c0 BEGIN
/* 4466C0-446770 000B0 */
uint CDSMpx::RefillInputBuffer(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004466c0 END

// !FUNC 0x00446770 BEGIN
/* 446770-4468A2 00132 */
uint CDSMpx::DecodeFrame(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00446770 END

// !FUNC 0x00446940 BEGIN
/* 446940-446995 00055 */
uchar CDSMpx::ReadPCM(void* param_1, void* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00446940 END

// !FUNC 0x004469a0 BEGIN
/* 4469A0-4469B8 00018 */
uchar CDSMpx::ForwardReadPcmToInstance(uint* param_1, void* param_2, void* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004469a0 END

// !FUNC 0x00446a00 BEGIN
/* 446A00-446ADF 000DF */
void* CDSMpx::AttachBitstream(int* param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00446a00 END

// !FUNC 0x00446b00 BEGIN
/* 446B00-446B88 00088 */
void* CDSMpx::CreateFromHandle(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00446b00 END

// !FUNC 0x00446b90 BEGIN
/* 446B90-446BF9 00069 */
uchar CDSMpx::ResolveResource(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00446b90 END

// !FUNC 0x004567b0 BEGIN
/* 4567B0-4567F5 00045 */
uchar CDSMpx::FUN_004567b0(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004567b0 END

// !FUNC 0x00456820 BEGIN
/* 456820-45684E 0002E */
uchar CDSMpx::FUN_00456820(uint* param_1, uint param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00456820 END

// !FUNC 0x00456850 BEGIN
/* 456850-4568A3 00053 */
uint CDSMpx::mad_stream_sync(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00456850 END

// !FUNC 0x004568b0 BEGIN
/* 4568B0-456910 00060 */
void CDSMpx::mad_synth_mute(int* param_1) { STUB_BODY(); }
// !FUNC 0x004568b0 END

// !FUNC 0x00456910 BEGIN
/* 456910-457A91 01181 */
uchar CDSMpx::mad_synth_frame_dct32_full(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00456910 END

// !FUNC 0x00457aa0 BEGIN
/* 457AA0-45844C 009AC */
uchar CDSMpx::DecodeGranuleStereo(int param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00457aa0 END

// !FUNC 0x00458450 BEGIN
/* 458450-458E0D 009BD */
uchar CDSMpx::DecodeGranuleMono(int param_1, int param_2, int param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x00458450 END

// !FUNC 0x00458e10 BEGIN
/* 458E10-458EA6 00096 */
uchar CDSMpx::DispatchLayerDecoder(int param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00458e10 END

// !FUNC 0x00458eb0 BEGIN
/* 458EB0-458EDC 0002C */
void CDSMpx::mad_synth_init(int* param_1) { STUB_BODY(); }
// !FUNC 0x00458eb0 END

// !FUNC 0x00458ee0 BEGIN
/* 458EE0-458F18 00038 */
void CDSMpx::mad_header_init(uint* param_1) { STUB_BODY(); }
// !FUNC 0x00458ee0 END

// !FUNC 0x00458f50 BEGIN
/* 458F50-459128 001D8 */
uint CDSMpx::mad_header_decode(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00458f50 END

// !FUNC 0x00459130 BEGIN
/* 459130-4592CA 0019A */
uint CDSMpx::EstimateAvgBitrate(uint* param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00459130 END

// !FUNC 0x004592d0 BEGIN
/* 4592D0-4594FC 0022C */
uint CDSMpx::SeekToValidFrame(int* param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004592d0 END

// !FUNC 0x00459500 BEGIN
/* 459500-45959B 0009B */
uint CDSMpx::ReadOneFrame(int* param_1, uint* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00459500 END

// !FUNC 0x004595a0 BEGIN
/* 4595A0-45961A 0007A */
void CDSMpx::mad_frame_mute(int param_1) { STUB_BODY(); }
// !FUNC 0x004595a0 END

// !FUNC 0x00459620 BEGIN
/* 459620-459641 00021 */
void CDSMpx::mad_frame_init(uint* param_1) { STUB_BODY(); }
// !FUNC 0x00459620 END

// !FUNC 0x00459650 BEGIN
/* 459650-459667 00017 */
// libmad 0.15.1b bit.c::mad_bit_init.  Upstream signature is
// `void(struct mad_bitptr*, unsigned char const*)`.  mad_bitptr layout
// (8 bytes): { unsigned char const *byte; ushort cache; ushort left; }
void CDSMpx::mad_bit_init(uint* param_1, uint param_2) {
    *param_1 = param_2;
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(param_1) + 4) = 0;
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(param_1) + 6) = 8;
}
// !FUNC 0x00459650 END

// !FUNC 0x00459670 BEGIN
/* 459670-45968F 0001F */
int CDSMpx::mad_bit_length(int* param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00459670 END

// !FUNC 0x00459690 BEGIN
/* 459690-4596A1 00011 */
// libmad 0.15.1b bit.c::mad_bit_nextbyte. Returns pointer to next unprocessed
// byte: bitptr->byte when bit pointer is byte-aligned (left == CHAR_BIT),
// else byte+1.  MSVC8 /O2 hoists the load above the branch (matches the
// observed `mov [eax] / jz` shape at 0x00459690).
int CDSMpx::mad_bit_nextbyte(int* param_1) {
    int iVar1 = *param_1;
    if (*reinterpret_cast<short*>(reinterpret_cast<char*>(param_1) + 6) != 8) {
        iVar1 = iVar1 + 1;
    }
    return iVar1;
}
// !FUNC 0x00459690 END

// !FUNC 0x004596b0 BEGIN
/* 4596B0-4596F0 00040 */
void CDSMpx::mad_bit_skip(int* param_1, uint param_2) { STUB_BODY(); }
// !FUNC 0x004596b0 END

// !FUNC 0x004596f0 BEGIN
/* 4596F0-4597A4 000B4 */
uint CDSMpx::mad_bit_read(int* param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004596f0 END

// !FUNC 0x004597b0 BEGIN
/* 4597B0-4598FF 0014F */
uint CDSMpx::mad_bit_crc(uint param_1, uint param_2, uint param_3, ushort param_4) { STUB_BODY(); return 0; }
// !FUNC 0x004597b0 END

// !FUNC 0x0045c100 BEGIN
/* 45C100-45C47F 0037F */
int CDSMpx::mad_layer_III(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045c100 END

// !FUNC 0x0045c480 BEGIN
/* 45C480-45C4D8 00058 */
uint CDSMpx::mad_layer_I_sample() { STUB_BODY(); return 0; }
// !FUNC 0x0045c480 END

// !FUNC 0x0045c4e0 BEGIN
/* 45C4E0-45C7F2 00312 */
uint CDSMpx::mad_layer_I(int param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045c4e0 END

// !FUNC 0x0045c800 BEGIN
/* 45C800-45C8BC 000BC */
uchar CDSMpx::mad_layer_II_samples(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045c800 END

// !FUNC 0x0045c8c0 BEGIN
/* 45C8C0-45CF4B 0068B */
uint CDSMpx::mad_layer_II(char* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045c8c0 END

// !FUNC 0x0045cf80 BEGIN
/* 45CF80-45CF95 00015 */
uchar CDSMpx::FUN_0045cf80(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045cf80 END

// !FUNC 0x0045cfa0 BEGIN
/* 45CFA0-45CFC0 00020 */
uchar CDSMpx::FUN_0045cfa0() { STUB_BODY(); return 0; }
// !FUNC 0x0045cfa0 END

// !FUNC 0x0045cfc0 BEGIN
/* 45CFC0-45D02A 0006A */
uint CDSMpx::FUN_0045cfc0(uint param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0045cfc0 END

// !FUNC 0x0045d030 BEGIN
/* 45D030-45D222 001F2 */
uchar CDSMpx::FUN_0045d030(int* param_1, int param_2, uint param_3, uint param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0045d030 END

