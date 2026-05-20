#ifndef _C_D_S_MPX
#define _C_D_S_MPX

#include <globals.h>

class CDSMpx {
public:
	// !DECL 0x00432f10 BEGIN
	/* 432F10 */ uchar* FUN_00432f10();
	// !DECL 0x00432f10 END
	// !DECL 0x00432f20 BEGIN
	/* 432F20 */ uchar FUN_00432f20(uchar param_1);
	// !DECL 0x00432f20 END
	// !DECL 0x00432f30 BEGIN
	/* 432F30 */ uchar FUN_00432f30(uchar param_1);
	// !DECL 0x00432f30 END
	// !DECL 0x00432f40 BEGIN
	/* 432F40 */ uchar FUN_00432f40(uint* param_1);
	// !DECL 0x00432f40 END
	// !DECL 0x00432fc0 BEGIN
	/* 432FC0 */ uint* FUN_00432fc0(uchar param_1);
	// !DECL 0x00432fc0 END
	// !DECL 0x0043c380 BEGIN
	/* 43C380 */ uchar FUN_0043c380(int param_1);
	// !DECL 0x0043c380 END
	// !DECL 0x0043c590 BEGIN
	/* 43C590 */ uchar FUN_0043c590(uchar* param_1, uint param_2, char param_3);
	// !DECL 0x0043c590 END
	// !DECL 0x004465e0 BEGIN
	/* 4465E0 */ uchar ResetDecoderState(int param_1);
	// !DECL 0x004465e0 END
	// !DECL 0x00446640 BEGIN
	/* 446640 */ uchar ReinitDecoder(int param_1);
	// !DECL 0x00446640 END
	// !DECL 0x00446670 BEGIN
	/* 446670 */ static int QuantizeQ31ToS16(int param_1);
	// !DECL 0x00446670 END
	// !DECL 0x004466a0 BEGIN
	/* 4466A0 */ uchar FUN_004466a0(int* param_1);
	// !DECL 0x004466a0 END
	// !DECL 0x004466c0 BEGIN
	/* 4466C0 */ uint RefillInputBuffer(int param_1);
	// !DECL 0x004466c0 END
	// !DECL 0x00446770 BEGIN
	/* 446770 */ uint DecodeFrame(int param_1);
	// !DECL 0x00446770 END
	// !DECL 0x00446940 BEGIN
	/* 446940 */ uchar ReadPCM(void* param_1, void* param_2);
	// !DECL 0x00446940 END
	// !DECL 0x004469a0 BEGIN
	/* 4469A0 */ uchar ForwardReadPcmToInstance(uint* param_1, void* param_2, void* param_3);
	// !DECL 0x004469a0 END
	// !DECL 0x00446a00 BEGIN
	/* 446A00 */ uint* AttachBitstream(int* param_1, uint param_2);
	// !DECL 0x00446a00 END
	// !DECL 0x00446b00 BEGIN
	/* 446B00 */ uint* CreateFromHandle(int param_1);
	// !DECL 0x00446b00 END
	// !DECL 0x00446b90 BEGIN
	/* 446B90 */ uchar ResolveResource(int param_1);
	// !DECL 0x00446b90 END
	// !DECL 0x004567b0 BEGIN
	/* 4567B0 */ static uchar FUN_004567b0(uint* param_1);
	// !DECL 0x004567b0 END
	// !DECL 0x00456820 BEGIN
	/* 456820 */ static uchar FUN_00456820(int* param_1, int param_2, int param_3);
	// !DECL 0x00456820 END
	// !DECL 0x00456850 BEGIN
	/* 456850 */ static uint mad_stream_sync(int param_1);
	// !DECL 0x00456850 END
	// !DECL 0x004568b0 BEGIN
	/* 4568B0 */ static void mad_synth_mute(int* param_1);
	// !DECL 0x004568b0 END
	// !DECL 0x00456910 BEGIN
	/* 456910 */ uchar mad_synth_frame_dct32_full(int* param_1);
	// !DECL 0x00456910 END
	// !DECL 0x00457aa0 BEGIN
	/* 457AA0 */ static uchar DecodeGranuleStereo(int param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00457aa0 END
	// !DECL 0x00458450 BEGIN
	/* 458450 */ static uchar DecodeGranuleMono(int param_1, int param_2, int param_3, int param_4);
	// !DECL 0x00458450 END
	// !DECL 0x00458e10 BEGIN
	/* 458E10 */ static uchar DispatchLayerDecoder(int param_1, int* param_2);
	// !DECL 0x00458e10 END
	// !DECL 0x00458eb0 BEGIN
	/* 458EB0 */ static void mad_synth_init(int* param_1);
	// !DECL 0x00458eb0 END
	// !DECL 0x00458ee0 BEGIN
	/* 458EE0 */ static void mad_header_init(uint* param_1);
	// !DECL 0x00458ee0 END
	// !DECL 0x00458f50 BEGIN
	/* 458F50 */ static uint mad_header_decode(int param_1);
	// !DECL 0x00458f50 END
	// !DECL 0x00459130 BEGIN
	/* 459130 */ static uint EstimateAvgBitrate(uint* param_1, int* param_2);
	// !DECL 0x00459130 END
	// !DECL 0x004592d0 BEGIN
	/* 4592D0 */ static uint SeekToValidFrame(int* param_1, uint* param_2);
	// !DECL 0x004592d0 END
	// !DECL 0x00459500 BEGIN
	/* 459500 */ static uint ReadOneFrame(int* param_1, uint* param_2);
	// !DECL 0x00459500 END
	// !DECL 0x004595a0 BEGIN
	/* 4595A0 */ static void mad_frame_mute(int param_1);
	// !DECL 0x004595a0 END
	// !DECL 0x00459620 BEGIN
	/* 459620 */ static void mad_frame_init(uint* param_1);
	// !DECL 0x00459620 END
	// !DECL 0x00459650 BEGIN
	/* 459650 */ static void mad_bit_init(uint* param_1, uint param_2);
	// !DECL 0x00459650 END
	// !DECL 0x00459670 BEGIN
	/* 459670 */ static int mad_bit_length(int* param_1, int* param_2);
	// !DECL 0x00459670 END
	// !DECL 0x00459690 BEGIN
	/* 459690 */ static int mad_bit_nextbyte(int* param_1);
	// !DECL 0x00459690 END
	// !DECL 0x004596b0 BEGIN
	/* 4596B0 */ static void mad_bit_skip(int* param_1, uint param_2);
	// !DECL 0x004596b0 END
	// !DECL 0x004596f0 BEGIN
	/* 4596F0 */ static uint mad_bit_read(int* param_1, uint param_2);
	// !DECL 0x004596f0 END
	// !DECL 0x004597b0 BEGIN
	/* 4597B0 */ static uint mad_bit_crc(uint param_1, uint param_2, uint param_3, ushort param_4);
	// !DECL 0x004597b0 END
	// !DECL 0x0045c100 BEGIN
	/* 45C100 */ static uchar mad_layer_III(int param_1, int param_2);
	// !DECL 0x0045c100 END
	// !DECL 0x0045c480 BEGIN
	/* 45C480 */ uint mad_layer_I_sample();
	// !DECL 0x0045c480 END
	// !DECL 0x0045c4e0 BEGIN
	/* 45C4E0 */ static uchar mad_layer_I(int param_1, int param_2);
	// !DECL 0x0045c4e0 END
	// !DECL 0x0045c800 BEGIN
	/* 45C800 */ static uchar mad_layer_II_samples(uint* param_1);
	// !DECL 0x0045c800 END
	// !DECL 0x0045c8c0 BEGIN
	/* 45C8C0 */ static uchar mad_layer_II(char* param_1, int param_2);
	// !DECL 0x0045c8c0 END
	// !DECL 0x0045cf80 BEGIN
	/* 45CF80 */ uchar FUN_0045cf80(uint param_1);
	// !DECL 0x0045cf80 END
	// !DECL 0x0045cfa0 BEGIN
	/* 45CFA0 */ uchar FUN_0045cfa0();
	// !DECL 0x0045cfa0 END
	// !DECL 0x0045cfc0 BEGIN
	/* 45CFC0 */ static uint FUN_0045cfc0(uint param_1, uint param_2, uint param_3);
	// !DECL 0x0045cfc0 END
	// !DECL 0x0045d030 BEGIN
	/* 45D030 */ static uchar FUN_0045d030(int* param_1, int param_2, uint param_3, uint param_4);
	// !DECL 0x0045d030 END
};

#endif
