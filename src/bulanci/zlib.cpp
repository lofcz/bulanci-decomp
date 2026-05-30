#include "zlib.h"

// !PROLOGUE BEGIN
// `zlib::zcalloc` (0x471260) wraps the MSVCRT `_calloc` entry point at
// 0x44753E, declared in `Runtime.h` as `Runtime::MSVCRT::_calloc`.
// Pulled in here so the EXACT 19-byte body of zcalloc compiles to the
// same `call _calloc` relocation Ghidra emits in the baseline COFF.
#include "Runtime.h"
// !PROLOGUE END

// !FUNC 0x0046edc0 BEGIN
/* 46EDC0-46EE02 00042 */
uint zlib::inflateReset(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0046edc0 END

// !FUNC 0x0046ee10 BEGIN
/* 46EE10-46EE59 00049 */
// Opacity barrier stub for `inflateEnd` -- see "Stub opacity barriers"
// in tools/bulanci_unpack/ghidra_analysis/gzip_stream.md.  These four
// tricks (noinline, volatile return, volatile-read of param_1, inline
// asm ecx/edx clobber) make the matched callers
// (CDSGZipStream::Decompress, ...) compile to the original byte
// sequence.  Drop them all simultaneously when the real zlib 1.1.3 body
// is dropped in.
__declspec(noinline) uint zlib::inflateEnd(int param_1) {
    static volatile uint __r = 0;
    uint v = __r + *(volatile uint*)param_1;
    __asm {
        mov ecx, ecx
        mov edx, edx
    }
    return v;
}
// !FUNC 0x0046ee10 END

// !FUNC 0x0046ee60 BEGIN
/* 46EE60-46EF58 000F8 */
// Opacity barrier stub for `inflateInit2_`.  See inflateEnd above.
__declspec(noinline) uint zlib::inflateInit2(int param_1, int param_2, char* param_3, int param_4) {
    static volatile uint __r = 0;
    uint v = __r + *(volatile uint*)param_1;
    __asm {
        mov ecx, ecx
        mov edx, edx
    }
    return v;
}
// !FUNC 0x0046ee60 END

// !FUNC 0x0046ef60 BEGIN
/* 46EF60-46EF7A 0001A */
// zlib 1.1.3 inflate.c:inflateInit_(z, version, stream_size)
//   int inflateInit_(z, version, stream_size) {
//     return inflateInit2_(z, DEF_WBITS, version, stream_size);
//   }
// DEF_WBITS = 15.  EXACT 26/26 bytes.
// noinline so /O2 cannot fold the wrapper into CDSGZipStream::Decompress
// (CDSGZipStream_Decompress_static) -- without it the caller pushes 4 args and calls
// inflateInit2_ directly, dropping the CALL-to-wrapper the original emits.
// The wrapper body itself compiles identically with or without noinline (26
// bytes, EXACT) because the directive only affects caller-side decisions.
__declspec(noinline) uint zlib::inflateInit(int param_1, char* param_2, int param_3) {
    return zlib::inflateInit2(param_1, 15, param_2, param_3);
}
// !FUNC 0x0046ef60 END

// !FUNC 0x0046ef80 BEGIN
/* 46EF80-46F324 003A4 */
// Opacity barrier stub for `inflate` (the 150-line state machine in
// inflate.c).  See inflateEnd above.
__declspec(noinline) uint zlib::inflate(int* param_1, int param_2) {
    static volatile uint __r = 0;
    uint v = __r + *(volatile uint*)param_1;
    __asm {
        mov ecx, ecx
        mov edx, edx
    }
    return v;
}
// !FUNC 0x0046ef80 END

// !FUNC 0x0046f3e0 BEGIN
/* 46F3E0-46F657 00277 */
// Opacity barrier stub for `deflate`.  See inflateEnd above.
__declspec(noinline) uint zlib::deflate(int* param_1, uint param_2) {
    static volatile uint __r = 0;
    uint v = __r + *(volatile uint*)param_1;
    __asm {
        mov ecx, ecx
        mov edx, edx
    }
    return v;
}
// !FUNC 0x0046f3e0 END

// !FUNC 0x0046f660 BEGIN
/* 46F660-46F71E 000BE */
// Opacity barrier stub for `deflateEnd`.  See inflateEnd above.
__declspec(noinline) uint zlib::deflateEnd(int param_1) {
    static volatile uint __r = 0;
    uint v = __r + *(volatile uint*)param_1;
    __asm {
        mov ecx, ecx
        mov edx, edx
    }
    return v;
}
// !FUNC 0x0046f660 END

// !FUNC 0x004702c0 BEGIN
/* 4702C0-4704BA 001FA */
// Opacity barrier stub for `deflateInit2_`.  See inflateEnd above.
__declspec(noinline) uint zlib::deflateInit2(int param_1, uint param_2, int param_3, int param_4, int param_5, uint param_6, char* param_7, int param_8) {
    static volatile uint __r = 0;
    uint v = __r + *(volatile uint*)param_1;
    __asm {
        mov ecx, ecx
        mov edx, edx
    }
    return v;
}
// !FUNC 0x004702c0 END

// !FUNC 0x004704c0 BEGIN
/* 4704C0-4704E5 00025 */
// zlib 1.1.3 deflate.c:deflateInit_(z, level, version, stream_size)
//   int deflateInit_(z, level, version, stream_size) {
//     return deflateInit2_(z, level, Z_DEFLATED, MAX_WBITS,
//                          DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, version,
//                          stream_size);
//   }
// Constants: Z_DEFLATED = 8, MAX_WBITS = 15, DEF_MEM_LEVEL = 8.
// EXACT 37/37 bytes.  noinline (see inflateInit_) so
// CDSGZipStream::Compress preserves the CALL-to-wrapper rather than
// folding to a direct deflateInit2_ call.
__declspec(noinline) uint zlib::deflateInit(int param_1, uint param_2, char* param_3, int param_4) {
    return zlib::deflateInit2(param_1, param_2, 8, 15, 8, 0, param_3, param_4);
}
// !FUNC 0x004704c0 END

// !FUNC 0x004704f0 BEGIN
/* 4704F0-47056D 0007D */
uchar zlib::inflate_blocks_reset(int* param_1, int param_2, int* param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004704f0 END

// !FUNC 0x00470570 BEGIN
/* 470570-470617 000A7 */
int* zlib::inflate_blocks_new(int param_1, int param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00470570 END

// !FUNC 0x00470620 BEGIN
/* 470620-47109B 00A7B */
uchar zlib::inflate_blocks(uint* param_1, int* param_2, int param_3) { STUB_BODY(); return 0; }
// !FUNC 0x00470620 END

// !FUNC 0x004710e0 BEGIN
/* 4710E0-47111F 0003F */
uint zlib::inflate_blocks_free(int* param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x004710e0 END

// !FUNC 0x00471260 BEGIN
/* 471260-471273 00013 */
// zlib 1.1.3 zutil.c:zcalloc(opaque, items, size)
//   voidpf zcalloc (opaque, items, size) {
//     if (opaque) items += size - size; /* compiler-happy noop */
//     return (voidpf)calloc(items, size);
//   }
// /O2 sees the noop and drops it, so the target is just
// `push size; push items; call _calloc; add esp,8; ret` (19 bytes EXACT).
int* zlib::zcalloc(uint param_1, size_t param_2, size_t param_3) {
    return (int*)Runtime::MSVCRT::_calloc(param_2, param_3);
}
// !FUNC 0x00471260 END

// !FUNC 0x00471290 BEGIN
/* 471290-4712FE 0006E */
uchar zlib::init_block(uint param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00471290 END

// !FUNC 0x004733d0 BEGIN
/* 4733D0-4733E6 00016 */
// zlib 1.1.3 infcodes.c:inflate_codes_free(c, z)
//   void inflate_codes_free(c, z) { ZFREE(z, c); }
// ZFREE expands to `(*z->zfree)(z->opaque, c)` -- a function-pointer call
// through the z_stream_s.zfree slot (+0x24) with z->opaque (+0x28) as the
// first arg.  22 bytes EXACT.
void zlib::inflate_codes_free(uint param_1, int param_2) {
    typedef void (__cdecl *zfree_t)(void*, void*);
    zfree_t zf       = *(zfree_t*)(param_2 + 0x24);
    void   *opaque   = *(void**)(param_2 + 0x28);
    zf(opaque, (void*)param_1);
}
// !FUNC 0x004733d0 END

