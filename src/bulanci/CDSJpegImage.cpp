#include "CDSJpegImage.h"

// !PROLOGUE BEGIN
// ----------------------------------------------------------------------------
// libjpeg-6b structure layouts inlined for this translation unit.
//
// The encoder helpers in this file were transcribed from the Independent JPEG
// Group's release 6b (27-Mar-1998, jcparam.c / jcapimin.c).  Bulanci links
// the encoder + decoder statically inside the engine, so we replay enough of
// `jpeg_compress_struct` to access fields by name while keeping the
// MSVC8 / 32-bit layout that Ghidra's signatures (`int*`, raw offsets) expect.
//
// The `// !PROLOGUE BEGIN/END` markers are preserved across `sync_units`.
struct JpegQuantTbl_layout {
    unsigned short quantval[64];   // 0x00
    int sent_table;                // 0x80
};
struct JpegHuffTbl_layout {
    unsigned char bits[17];        // 0x00
    unsigned char huffval[256];    // 0x11
    int sent_table;                // 0x114
};
struct JpegCompress_layout {
    int* err;                      // 0x00
    int* mem;                      // 0x04
    int* progress;                 // 0x08
    int* client_data;              // 0x0c
    int  is_decompressor;          // 0x10
    int  global_state;             // 0x14
    int* dest;                     // 0x18
    unsigned int image_width;      // 0x1c
    unsigned int image_height;     // 0x20
    int  input_components;         // 0x24
    int  in_color_space;           // 0x28
    double input_gamma;            // 0x30 (8-byte align inserts 4 pad at 0x2c)
    int  data_precision;           // 0x38
    int  num_components;           // 0x3c
    int  jpeg_color_space;         // 0x40
    int* comp_info;                // 0x44
    JpegQuantTbl_layout* quant_tbl_ptrs[4];   // 0x48..0x57
    JpegHuffTbl_layout*  dc_huff_tbl_ptrs[4]; // 0x58..0x67
    JpegHuffTbl_layout*  ac_huff_tbl_ptrs[4]; // 0x68..0x77
    unsigned char arith_dc_L[16];  // 0x78..0x87
    unsigned char arith_dc_U[16];  // 0x88..0x97
    unsigned char arith_ac_K[16];  // 0x98..0xa7
    int  num_scans;                // 0xa8
    int* scan_info;                // 0xac
    int  raw_data_in;              // 0xb0
    int  arith_code;               // 0xb4
    int  optimize_coding;          // 0xb8
    int  CCIR601_sampling;         // 0xbc
    int  smoothing_factor;         // 0xc0
    int  dct_method;               // 0xc4
    unsigned int restart_interval; // 0xc8
    int  restart_in_rows;          // 0xcc
    int  write_JFIF_header;        // 0xd0
    unsigned char JFIF_major_version; // 0xd4
    unsigned char JFIF_minor_version; // 0xd5
    unsigned char density_unit;       // 0xd6
    unsigned char _pad_d7;            // 0xd7
    unsigned short X_density;         // 0xd8
    unsigned short Y_density;         // 0xda
    int  write_Adobe_marker;          // 0xdc
};
// jpeg_component_info (subset, 0x54 bytes = MAX_COMPONENTS*0x54 = 0x348).
struct JpegCompInfo_layout {
    int component_id;        // 0x00
    int component_index;     // 0x04
    int h_samp_factor;       // 0x08
    int v_samp_factor;       // 0x0c
    int quant_tbl_no;        // 0x10
    int dc_tbl_no;           // 0x14
    int ac_tbl_no;           // 0x18
    unsigned char rest[0x54 - 0x1c];
};

// libjpeg-6b allocators (mapped under _Globals in mapping.csv).
class _Globals {
public:
    static uchar FUN_0045d1d0(int param_1); // jpeg_alloc_quant_table
    static uchar FUN_0045d1f0(int param_1); // jpeg_alloc_huff_table
};
// !PROLOGUE END

// !FUNC 0x00431510 BEGIN
/* 431510-43154E 0003E */
// Bulanci wrapper: install a CDSStreamStorage-backed jpeg_source_mgr.
// Layout matches the reverse-engineered `cinfo` fingerprint at offset 0x18
// (cinfo->src) where libjpeg-6b expects a `jpeg_source_mgr *`.  The three
// `LAB_*` addresses are the init_source / fill_input_buffer / skip_input_data
// trampolines compiled earlier in the unit; bytes_in_buffer is filled in by
// fill_input_buffer when the decoder pulls from the wrapped CDSStreamStorage.
uchar CDSJpegImage::FUN_00431510(int param_1, uint param_2) {
    int iVar1;
    int uVar2;

    if (*reinterpret_cast<int*>(param_1 + 0x18) == 0) {
        uVar2 = (*reinterpret_cast<int(__cdecl**)(int, int, int)>(
                    *reinterpret_cast<int*>(param_1 + 4)))(param_1, 0, 0x1c);
        *reinterpret_cast<int*>(param_1 + 0x18) = uVar2;
    }
    iVar1 = *reinterpret_cast<int*>(param_1 + 0x18);
    *reinterpret_cast<uchar**>(iVar1 + 8) = &LAB_00431480;
    *reinterpret_cast<uchar**>(iVar1 + 0xc) = &LAB_004314b0;
    *reinterpret_cast<uchar**>(iVar1 + 0x10) = &LAB_004314e0;
    *reinterpret_cast<uint*>(iVar1 + 0x14) = param_2;
    return 0;
}
// !FUNC 0x00431510 END

// !FUNC 0x00431cc0 BEGIN
/* 431CC0-431CEB 0002B */
uchar CDSJpegImage::FUN_00431cc0(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431cc0 END

// !FUNC 0x00431d50 BEGIN
/* 431D50-431D56 00006 */
uchar* CDSJpegImage::FUN_00431d50() {
    return reinterpret_cast<uchar*>(&DAT_004b7d80);
}
// !FUNC 0x00431d50 END

// !FUNC 0x00431d60 BEGIN
/* 431D60-431D63 00003 */
uchar CDSJpegImage::FUN_00431d60() { STUB_BODY(); return 0; }
// !FUNC 0x00431d60 END

// !FUNC 0x00431d70 BEGIN
/* 431D70-431D78 00008 */
uchar CDSJpegImage::FUN_00431d70(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431d70 END

// !FUNC 0x00431d80 BEGIN
/* 431D80-431D88 00008 */
uchar CDSJpegImage::FUN_00431d80(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431d80 END

// !FUNC 0x00431da0 BEGIN
/* 431DA0-431DA8 00008 */
uchar CDSJpegImage::FUN_00431da0(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431da0 END

// !FUNC 0x00431db0 BEGIN
/* 431DB0-431DB8 00008 */
uchar CDSJpegImage::FUN_00431db0(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431db0 END

// !FUNC 0x00431de0 BEGIN
/* 431DE0-431E4E 0006E */
uchar CDSJpegImage::FUN_00431de0(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00431de0 END

// !FUNC 0x00431e50 BEGIN
/* 431E50-43200F 001BF */
uchar CDSJpegImage::FUN_00431e50(uint param_1, void* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00431e50 END

// !FUNC 0x00432030 BEGIN
/* 432030-432066 00036 */
uchar CDSJpegImage::FUN_00432030(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00432030 END

// !FUNC 0x00432090 BEGIN
/* 432090-4320AE 0001E */
uint* CDSJpegImage::FUN_00432090(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00432090 END

// !FUNC 0x0045ecc0 BEGIN
/* 45ECC0-45ED7E 000BE */
uchar CDSJpegImage::FUN_0045ecc0(int* param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0045ecc0 END

// !FUNC 0x0045ed80 BEGIN
/* 45ED80-45EDE8 00068 */
// libjpeg-6b: jpeg_suppress_tables (jcapimin.c) -- mark every allocated
// quant / Huffman table's sent_table flag so future scans can choose whether
// to re-emit them.  Note: the libjpeg signature takes a j_compress_ptr but
// the Ghidra-inferred prototype is `int` (just the cinfo address).
uchar CDSJpegImage::FUN_0045ed80(int param_1, uchar param_2) {
    JpegCompress_layout* cinfo = reinterpret_cast<JpegCompress_layout*>(param_1);
    for (int i = 0; i < 4; i++) {
        JpegQuantTbl_layout* qtbl = cinfo->quant_tbl_ptrs[i];
        if (qtbl != 0) qtbl->sent_table = param_2;
    }
    for (int i = 0; i < 4; i++) {
        JpegHuffTbl_layout* htbl = cinfo->dc_huff_tbl_ptrs[i];
        if (htbl != 0) htbl->sent_table = param_2;
        htbl = cinfo->ac_huff_tbl_ptrs[i];
        if (htbl != 0) htbl->sent_table = param_2;
    }
    return 0;
}
// !FUNC 0x0045ed80 END

// !FUNC 0x0045edf0 BEGIN
/* 45EDF0-45EEDE 000EE */
uchar CDSJpegImage::FUN_0045edf0(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0045edf0 END

// !FUNC 0x0045eee0 BEGIN
/* 45EEE0-45EF5A 0007A */
uchar CDSJpegImage::FUN_0045eee0(int* param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045eee0 END

// !FUNC 0x0045ef60 BEGIN
/* 45EF60-45EFDB 0007B */
uchar CDSJpegImage::FUN_0045ef60(int* param_1, uint param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x0045ef60 END

// !FUNC 0x0045efe0 BEGIN
/* 45EFE0-45F1AD 001CD */
// libjpeg-6b: jpeg_add_quant_table (jcparam.c).
// Build a quant table at slot `which_tbl` from `basic_table[64]` scaled by
// scale_factor%, optionally clamping to 1..255 when force_baseline.
uchar CDSJpegImage::FUN_0045efe0(int* param_1, int param_2, int param_3,
                                 int param_4, char param_5) {
    JpegCompress_layout* cinfo = reinterpret_cast<JpegCompress_layout*>(param_1);

    if (cinfo->global_state != 100) {
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 8) = 0x15;
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 0xc) = cinfo->global_state;
        (*reinterpret_cast<void(__cdecl**)(int*)>(*reinterpret_cast<int*>(cinfo->err)))(
            reinterpret_cast<int*>(cinfo));
    }
    if ((unsigned)param_2 >= 4) {
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 8) = 0x33;
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 0xc) = param_2;
        (*reinterpret_cast<void(__cdecl**)(int*)>(*reinterpret_cast<int*>(cinfo->err)))(
            reinterpret_cast<int*>(cinfo));
    }

    if (cinfo->quant_tbl_ptrs[param_2] == 0) {
        cinfo->quant_tbl_ptrs[param_2] =
            reinterpret_cast<JpegQuantTbl_layout*>(
                static_cast<int>(_Globals::FUN_0045d1d0(reinterpret_cast<int>(cinfo))));
    }
    JpegQuantTbl_layout* qtbl = cinfo->quant_tbl_ptrs[param_2];
    const unsigned int* basic = reinterpret_cast<const unsigned int*>(param_3);
    for (int i = 0; i < 64; i++) {
        long temp = ((long)basic[i] * param_4 + 50L) / 100L;
        if (temp <= 0L) temp = 1L;
        if (temp > 32767L) temp = 32767L;
        if (param_5 && temp > 255L) temp = 255L;
        qtbl->quantval[i] = (unsigned short)temp;
    }
    qtbl->sent_table = 0;
    return 0;
}
// !FUNC 0x0045efe0 END

// !FUNC 0x0045f1b0 BEGIN
/* 45F1B0-45F1E4 00034 */
// libjpeg-6b: jpeg_set_linear_quality (jcparam.c).  Sets the luminance /
// chrominance quant tables from the JPEG spec K.1 defaults, scaled by
// `scale_factor` (percentage).  `force_baseline` clamps entries to 1..255.
uchar CDSJpegImage::FUN_0045f1b0(int* param_1, int param_2, char param_3) {
    static const unsigned int std_luminance_quant_tbl[64] = {
        16,  11,  10,  16,  24,  40,  51,  61,
        12,  12,  14,  19,  26,  58,  60,  55,
        14,  13,  16,  24,  40,  57,  69,  56,
        14,  17,  22,  29,  51,  87,  80,  62,
        18,  22,  37,  56,  68, 109, 103,  77,
        24,  35,  55,  64,  81, 104, 113,  92,
        49,  64,  78,  87, 103, 121, 120, 101,
        72,  92,  95,  98, 112, 100, 103,  99
    };
    static const unsigned int std_chrominance_quant_tbl[64] = {
        17,  18,  24,  47,  99,  99,  99,  99,
        18,  21,  26,  66,  99,  99,  99,  99,
        24,  26,  56,  99,  99,  99,  99,  99,
        47,  66,  99,  99,  99,  99,  99,  99,
        99,  99,  99,  99,  99,  99,  99,  99,
        99,  99,  99,  99,  99,  99,  99,  99,
        99,  99,  99,  99,  99,  99,  99,  99,
        99,  99,  99,  99,  99,  99,  99,  99
    };
    CDSJpegImage::FUN_0045efe0(param_1, 0,
        reinterpret_cast<int>(std_luminance_quant_tbl), param_2, param_3);
    CDSJpegImage::FUN_0045efe0(param_1, 1,
        reinterpret_cast<int>(std_chrominance_quant_tbl), param_2, param_3);
    return 0;
}
// !FUNC 0x0045f1b0 END

// !FUNC 0x0045f1f0 BEGIN
/* 45F1F0-45F228 00038 */
// libjpeg-6b: jpeg_quality_scaling (jcparam.c) -- convert a user-friendly
// quality rating in [1..100] into a percentage scaling factor.
int CDSJpegImage::FUN_0045f1f0(int param_1) {
    if (param_1 <= 0) param_1 = 1;
    if (param_1 > 100) param_1 = 100;
    if (param_1 < 50)
        param_1 = 5000 / param_1;
    else
        param_1 = 200 - param_1 * 2;
    return param_1;
}
// !FUNC 0x0045f1f0 END

// !FUNC 0x0045f230 BEGIN
/* 45F230-45F251 00021 */
// libjpeg-6b: jpeg_set_quality (jcparam.c) -- public 0..100 quality entry
// point.  Converts to a linear scaling and forwards to jpeg_set_linear_quality.
uchar CDSJpegImage::FUN_0045f230(int* param_1, int param_2, char param_3) {
    int scaled = CDSJpegImage::FUN_0045f1f0(param_2);
    CDSJpegImage::FUN_0045f1b0(param_1, scaled, param_3);
    return 0;
}
// !FUNC 0x0045f230 END

// !FUNC 0x0045f260 BEGIN
/* 45F260-45F304 000A4 */
uchar CDSJpegImage::FUN_0045f260(int* param_1, void* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0045f260 END

// !FUNC 0x0045f310 BEGIN
/* 45F310-45F362 00052 */
uchar CDSJpegImage::FUN_0045f310() { STUB_BODY(); return 0; }
// !FUNC 0x0045f310 END

// !FUNC 0x0045f370 BEGIN
/* 45F370-45F63A 002CA */
// libjpeg-6b: jpeg_set_colorspace (jcparam.c) -- pick component_id /
// sampling / quant / Huffman selectors based on the JPEG colorspace.
// The SET_COMP() macro in the reference source expands inline; MSVC at
// /O2 lays it out as one stride per component.
#define SET_COMP_inline(ci_, id_, hs_, vs_, q_, dt_, at_) do {        \
    JpegCompInfo_layout* cp_ = &comps[(ci_)];                          \
    cp_->component_id   = (id_);                                       \
    cp_->h_samp_factor  = (hs_);                                       \
    cp_->v_samp_factor  = (vs_);                                       \
    cp_->quant_tbl_no   = (q_);                                        \
    cp_->dc_tbl_no      = (dt_);                                       \
    cp_->ac_tbl_no      = (at_);                                       \
} while (0)
uchar CDSJpegImage::FUN_0045f370(int* param_1, int param_2) {
    JpegCompress_layout* cinfo = reinterpret_cast<JpegCompress_layout*>(param_1);
    JpegCompInfo_layout* comps = reinterpret_cast<JpegCompInfo_layout*>(cinfo->comp_info);

    if (cinfo->global_state != 100) {
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 8) = 0x15;
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 0xc) = cinfo->global_state;
        (*reinterpret_cast<void(__cdecl**)(int*)>(*reinterpret_cast<int*>(cinfo->err)))(
            reinterpret_cast<int*>(cinfo));
    }
    cinfo->jpeg_color_space = param_2;
    cinfo->write_JFIF_header = 0;
    cinfo->write_Adobe_marker = 0;

    switch (param_2) {
    case 1: /* JCS_GRAYSCALE */
        cinfo->write_JFIF_header = 1;
        cinfo->num_components = 1;
        SET_COMP_inline(0, 1, 1, 1, 0, 0, 0);
        break;
    case 2: /* JCS_RGB */
        cinfo->write_Adobe_marker = 1;
        cinfo->num_components = 3;
        SET_COMP_inline(0, 0x52, 1, 1, 0, 0, 0);
        SET_COMP_inline(1, 0x47, 1, 1, 0, 0, 0);
        SET_COMP_inline(2, 0x42, 1, 1, 0, 0, 0);
        break;
    case 3: /* JCS_YCbCr */
        cinfo->write_JFIF_header = 1;
        cinfo->num_components = 3;
        SET_COMP_inline(0, 1, 2, 2, 0, 0, 0);
        SET_COMP_inline(1, 2, 1, 1, 1, 1, 1);
        SET_COMP_inline(2, 3, 1, 1, 1, 1, 1);
        break;
    case 4: /* JCS_CMYK */
        cinfo->write_Adobe_marker = 1;
        cinfo->num_components = 4;
        SET_COMP_inline(0, 0x43, 1, 1, 0, 0, 0);
        SET_COMP_inline(1, 0x4D, 1, 1, 0, 0, 0);
        SET_COMP_inline(2, 0x59, 1, 1, 0, 0, 0);
        SET_COMP_inline(3, 0x4B, 1, 1, 0, 0, 0);
        break;
    case 5: /* JCS_YCCK */
        cinfo->write_Adobe_marker = 1;
        cinfo->num_components = 4;
        SET_COMP_inline(0, 1, 2, 2, 0, 0, 0);
        SET_COMP_inline(1, 2, 1, 1, 1, 1, 1);
        SET_COMP_inline(2, 3, 1, 1, 1, 1, 1);
        SET_COMP_inline(3, 4, 2, 2, 0, 0, 0);
        break;
    case 0: /* JCS_UNKNOWN */ {
        cinfo->num_components = cinfo->input_components;
        if ((unsigned)cinfo->num_components - 1 >= 10) {
            *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 8) = 0x17;
            *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 0xc) = cinfo->num_components;
            *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 0x10) = 10;
            (*reinterpret_cast<void(__cdecl**)(int*)>(*reinterpret_cast<int*>(cinfo->err)))(
                reinterpret_cast<int*>(cinfo));
        }
        for (int ci = 0; ci < cinfo->num_components; ci++) {
            SET_COMP_inline(ci, ci, 1, 1, 0, 0, 0);
        }
        break;
    }
    default:
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 8) = 0xa;
        (*reinterpret_cast<void(__cdecl**)(int*)>(*reinterpret_cast<int*>(cinfo->err)))(
            reinterpret_cast<int*>(cinfo));
    }
    return 0;
}
#undef SET_COMP_inline
// !FUNC 0x0045f370 END

// !FUNC 0x0045f660 BEGIN
/* 45F660-45F6C2 00062 */
// libjpeg-6b: jpeg_default_colorspace (jcparam.c).  Maps cinfo->in_color_space
// (param_1[10]) to a sensible jpeg_color_space and dispatches to
// jpeg_set_colorspace.  Note: JCS_RGB and JCS_YCbCr both map to JCS_YCbCr.
uchar CDSJpegImage::FUN_0045f660(int* param_1) {
    JpegCompress_layout* cinfo = reinterpret_cast<JpegCompress_layout*>(param_1);
    switch (cinfo->in_color_space) {
    case 0: /* JCS_UNKNOWN */
        CDSJpegImage::FUN_0045f370(param_1, 0); break;
    case 1: /* JCS_GRAYSCALE */
        CDSJpegImage::FUN_0045f370(param_1, 1); break;
    case 2: /* JCS_RGB */
    case 3: /* JCS_YCbCr */
        CDSJpegImage::FUN_0045f370(param_1, 3); break;
    case 4: /* JCS_CMYK */
        CDSJpegImage::FUN_0045f370(param_1, 4); break;
    case 5: /* JCS_YCCK */
        CDSJpegImage::FUN_0045f370(param_1, 5); break;
    default:
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 8) = 9;
        (*reinterpret_cast<void(__cdecl**)(int*)>(*reinterpret_cast<int*>(cinfo->err)))(
            reinterpret_cast<int*>(cinfo));
    }
    return 0;
}
// !FUNC 0x0045f660 END

// !FUNC 0x0045f6e0 BEGIN
/* 45F6E0-45F7DA 000FA */
// libjpeg-6b: jpeg_set_defaults (jcparam.c).  Apply colorspace-independent
// defaults to a fresh compress context, lazily allocate comp_info, then
// dispatch through jpeg_default_colorspace.  std_huff_tables is implemented
// here in FUN_0045f310.
uchar CDSJpegImage::FUN_0045f6e0(int* param_1) {
    JpegCompress_layout* cinfo = reinterpret_cast<JpegCompress_layout*>(param_1);

    if (cinfo->global_state != 100) {
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 8) = 0x14;
        *reinterpret_cast<int*>(reinterpret_cast<int>(cinfo->err) + 0xc) = cinfo->global_state;
        (*reinterpret_cast<void(__cdecl**)(int*)>(*reinterpret_cast<int*>(cinfo->err)))(
            reinterpret_cast<int*>(cinfo));
    }
    if (cinfo->comp_info == 0) {
        cinfo->comp_info = reinterpret_cast<int*>(
            (*reinterpret_cast<int(__cdecl**)(int*, int, int)>(
                *reinterpret_cast<int*>(cinfo->mem)))(param_1, 0, 0x348));
    }
    cinfo->data_precision = 8;
    CDSJpegImage::FUN_0045f230(param_1, 75, 1);
    /* TODO: std_huff_tables(cinfo) -- FUN_0045f310's Ghidra signature
       currently lacks the cinfo parameter, so we can't call it from a
       static helper.  Patch this once the namespace/argument detection is
       fixed; the binary call site is what objdiff is looking for. */

    for (int i = 0; i < 16; i++) {
        cinfo->arith_dc_L[i] = 0;
        cinfo->arith_dc_U[i] = 1;
        cinfo->arith_ac_K[i] = 5;
    }
    cinfo->scan_info = 0;
    cinfo->num_scans = 0;
    cinfo->raw_data_in = 0;
    cinfo->arith_code = 0;
    cinfo->optimize_coding = 0;
    if (cinfo->data_precision > 8) cinfo->optimize_coding = 1;
    cinfo->CCIR601_sampling = 0;
    cinfo->smoothing_factor = 0;
    cinfo->dct_method = 0;
    cinfo->restart_interval = 0;
    cinfo->restart_in_rows = 0;
    cinfo->JFIF_major_version = 1;
    cinfo->JFIF_minor_version = 1;
    cinfo->density_unit = 0;
    cinfo->X_density = 1;
    cinfo->Y_density = 1;

    CDSJpegImage::FUN_0045f660(param_1);
    return 0;
}
// !FUNC 0x0045f6e0 END

// !FUNC 0x00460d60 BEGIN
/* 460D60-460DB3 00053 */
uchar CDSJpegImage::FUN_00460d60(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00460d60 END

// !FUNC 0x00460dc0 BEGIN
/* 460DC0-460E75 000B5 */
uchar CDSJpegImage::FUN_00460dc0(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00460dc0 END

// !FUNC 0x00467600 BEGIN
/* 467600-46768E 0008E */
uchar CDSJpegImage::FUN_00467600(int* param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00467600 END

// !FUNC 0x00467dc0 BEGIN
/* 467DC0-467EC2 00102 */
uchar CDSJpegImage::FUN_00467dc0(int param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00467dc0 END

// !FUNC 0x00468d70 BEGIN
/* 468D70-468DC4 00054 */
uchar CDSJpegImage::FUN_00468d70(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00468d70 END

// !FUNC 0x00469900 BEGIN
/* 469900-46993F 0003F */
uchar CDSJpegImage::FUN_00469900(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00469900 END

// !FUNC 0x0046a750 BEGIN
/* 46A750-46A7E2 00092 */
uchar CDSJpegImage::FUN_0046a750(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0046a750 END

// !FUNC 0x0046abd0 BEGIN
/* 46ABD0-46ACED 0011D */
uchar CDSJpegImage::FUN_0046abd0() { STUB_BODY(); return 0; }
// !FUNC 0x0046abd0 END

// !FUNC 0x0046acf0 BEGIN
/* 46ACF0-46AD9E 000AE */
uchar CDSJpegImage::FUN_0046acf0(int* param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0046acf0 END

// !FUNC 0x0046b590 BEGIN
/* 46B590-46B6D7 00147 */
uchar CDSJpegImage::FUN_0046b590(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0046b590 END

// !FUNC 0x0046bbc0 BEGIN
/* 46BBC0-46BDC1 00201 */
uchar CDSJpegImage::FUN_0046bbc0(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0046bbc0 END

// !FUNC 0x0046bdf0 BEGIN
/* 46BDF0-46BFC8 001D8 */
uchar CDSJpegImage::FUN_0046bdf0() { STUB_BODY(); return 0; }
// !FUNC 0x0046bdf0 END

// !FUNC 0x0046bfd0 BEGIN
/* 46BFD0-46C3C8 003F8 */
uchar CDSJpegImage::FUN_0046bfd0() { STUB_BODY(); return 0; }
// !FUNC 0x0046bfd0 END

// !FUNC 0x0046c8f0 BEGIN
/* 46C8F0-46C99F 000AF */
uchar CDSJpegImage::FUN_0046c8f0(int param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0046c8f0 END

