#ifndef _C_D_S_JPEG_IMAGE
#define _C_D_S_JPEG_IMAGE

#include <globals.h>

class CDSJpegImage {
public:
	// !DECL 0x00431510 BEGIN
	/* 431510 */ static uchar jpeg_CDSStreamStorage_dst(int param_1, uint param_2);
	// !DECL 0x00431510 END
	// !DECL 0x00431cc0 BEGIN
	/* 431CC0 */ void CDSJpegImage_Load(void* param_1);
	// !DECL 0x00431cc0 END
	// !DECL 0x00431d50 BEGIN
	/* 431D50 */ uchar* CDSJpegImage_GetTypeInfo();
	// !DECL 0x00431d50 END
	// !DECL 0x00431d60 BEGIN
	/* 431D60 */ uchar CDSJpegImage_AlwaysReturnsOne();
	// !DECL 0x00431d60 END
	// !DECL 0x00431d70 BEGIN
	/* 431D70 */ uchar CDSJpegImage_ScalarDeletingDtor_thunk_Sub4(uchar param_1);
	// !DECL 0x00431d70 END
	// !DECL 0x00431d80 BEGIN
	/* 431D80 */ uchar CDSJpegImage_ScalarDeletingDtor_thunk_Sub4c(uchar param_1);
	// !DECL 0x00431d80 END
	// !DECL 0x00431da0 BEGIN
	/* 431DA0 */ uchar CDSJpegImage_ScalarDeletingDtorThunk(uchar param_1);
	// !DECL 0x00431da0 END
	// !DECL 0x00431db0 BEGIN
	/* 431DB0 */ uchar CDSJpegImage_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x00431db0 END
	// !DECL 0x00431de0 BEGIN
	/* 431DE0 */ void CDSJpegImage_dtor(uchar param_1);
	// !DECL 0x00431de0 END
	// !DECL 0x00431e50 BEGIN
	/* 431E50 */ static void CompressFromImage(void* param_1, CDSImage* param_2, int param_3);
	// !DECL 0x00431e50 END
	// !DECL 0x00432030 BEGIN
	/* 432030 */ void CDSJpegImage_Save(void* param_1);
	// !DECL 0x00432030 END
	// !DECL 0x00432090 BEGIN
	/* 432090 */ void* CDSJpegImage_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00432090 END
	// !DECL 0x0045ecc0 BEGIN
	/* 45ECC0 */ static uchar jpeg_CreateCompress(int* param_1, int param_2, int param_3);
	// !DECL 0x0045ecc0 END
	// !DECL 0x0045ed80 BEGIN
	/* 45ED80 */ static uchar jpeg_suppress_tables(int param_1, uchar param_2);
	// !DECL 0x0045ed80 END
	// !DECL 0x0045edf0 BEGIN
	/* 45EDF0 */ static uchar jpeg_finish_compress(int* param_1);
	// !DECL 0x0045edf0 END
	// !DECL 0x0045eee0 BEGIN
	/* 45EEE0 */ static uchar jpeg_start_compress(int* param_1, char param_2);
	// !DECL 0x0045eee0 END
	// !DECL 0x0045ef60 BEGIN
	/* 45EF60 */ static uchar jpeg_write_scanlines(int* param_1, uint param_2, uint param_3);
	// !DECL 0x0045ef60 END
	// !DECL 0x0045efe0 BEGIN
	/* 45EFE0 */ static uchar jpeg_add_quant_table(int* param_1, int param_2, int param_3, int param_4, char param_5);
	// !DECL 0x0045efe0 END
	// !DECL 0x0045f1b0 BEGIN
	/* 45F1B0 */ static uchar jpeg_set_linear_quality(int* param_1, int param_2, char param_3);
	// !DECL 0x0045f1b0 END
	// !DECL 0x0045f1f0 BEGIN
	/* 45F1F0 */ static int jpeg_quality_scaling(int param_1);
	// !DECL 0x0045f1f0 END
	// !DECL 0x0045f230 BEGIN
	/* 45F230 */ static uchar jpeg_set_quality(int* param_1, int param_2, char param_3);
	// !DECL 0x0045f230 END
	// !DECL 0x0045f260 BEGIN
	/* 45F260 */ static uchar jpeg_add_quant_table_0045f260(int* param_1, void* param_2);
	// !DECL 0x0045f260 END
	// !DECL 0x0045f310 BEGIN
	/* 45F310 */ uchar jpeg_add_default_quant_tables();
	// !DECL 0x0045f310 END
	// !DECL 0x0045f370 BEGIN
	/* 45F370 */ static uchar jpeg_set_colorspace(int* param_1, int param_2);
	// !DECL 0x0045f370 END
	// !DECL 0x0045f660 BEGIN
	/* 45F660 */ static uchar jpeg_default_colorspace(int* param_1);
	// !DECL 0x0045f660 END
	// !DECL 0x0045f6e0 BEGIN
	/* 45F6E0 */ static uchar jpeg_set_defaults(int* param_1);
	// !DECL 0x0045f6e0 END
	// !DECL 0x00460d60 BEGIN
	/* 460D60 */ static uchar jinit_marker_writer(int param_1);
	// !DECL 0x00460d60 END
	// !DECL 0x00460dc0 BEGIN
	/* 460DC0 */ static uchar jinit_compress_master(int* param_1);
	// !DECL 0x00460dc0 END
	// !DECL 0x00467600 BEGIN
	/* 467600 */ static uchar jinit_c_main_controller(int* param_1, char param_2);
	// !DECL 0x00467600 END
	// !DECL 0x00467dc0 BEGIN
	/* 467DC0 */ static uchar jinit_c_coef_controller(int param_1, char param_2);
	// !DECL 0x00467dc0 END
	// !DECL 0x00468d70 BEGIN
	/* 468D70 */ static uchar jinit_huff_encoder(int param_1);
	// !DECL 0x00468d70 END
	// !DECL 0x00469900 BEGIN
	/* 469900 */ static void jinit_phuff_encoder(int param_1);
	// !DECL 0x00469900 END
	// !DECL 0x0046a750 BEGIN
	/* 46A750 */ static uchar jpeg_jinit_inverse_dct(int* param_1);
	// !DECL 0x0046a750 END
	// !DECL 0x0046abd0 BEGIN
	/* 46ABD0 */ uchar FUN_0046abd0();
	// !DECL 0x0046abd0 END
	// !DECL 0x0046acf0 BEGIN
	/* 46ACF0 */ static void jinit_c_prep_controller(int* param_1, char param_2);
	// !DECL 0x0046acf0 END
	// !DECL 0x0046b590 BEGIN
	/* 46B590 */ static uchar FUN_0046b590(int* param_1);
	// !DECL 0x0046b590 END
	// !DECL 0x0046bbc0 BEGIN
	/* 46BBC0 */ static uchar jinit_color_converter(int* param_1);
	// !DECL 0x0046bbc0 END
	// !DECL 0x0046bdf0 BEGIN
	/* 46BDF0 */ uchar initial_setup();
	// !DECL 0x0046bdf0 END
	// !DECL 0x0046bfd0 BEGIN
	/* 46BFD0 */ uchar jpeg_validate_script();
	// !DECL 0x0046bfd0 END
	// !DECL 0x0046c8f0 BEGIN
	/* 46C8F0 */ static uchar FUN_0046c8f0(int param_1, char param_2);
	// !DECL 0x0046c8f0 END

	// !DECL 0x00431b70 BEGIN
	/* 431B70 */ static uchar DecompressToImage(int* param_1, uint param_2, void* param_3);
	// !DECL 0x00431b70 END

	// !DECL 0x00431cf0 BEGIN
	/* 431CF0 */ void* CDSJpegImage_InitVtables(void* param_1);
	// !DECL 0x00431cf0 END
};

#endif
