/* Minimal TU for objdiff on _Globals::jpeg_CreateDecompress only.
 * Full _Globals.cpp does not compile yet (mapping types / Runtime.h).
 * build/Src/bulanci/_Globals.obj is produced from this file via
 *   python scripts/internal/compile_globals_jpeg_slice.py
 */
#include <globals.h>

extern "C" void* memset(void*, int, size_t);

namespace Runtime {
namespace MSVCRT {
__declspec(noinline) void* _memset(void* param_1, int param_2, size_t param_3) {
    return memset(param_1, param_2, param_3);
}
}  // namespace MSVCRT
}  // namespace Runtime

class _Globals {
public:
    static uchar jinit_memory_mgr(int param_1);
    static uchar jinit_marker_reader(int param_1);
    static uchar jinit_input_controller(int param_1);
    static uchar jpeg_CreateDecompress(int* param_1, int param_2, int param_3);
};

__declspec(noinline) uchar _Globals::jpeg_CreateDecompress(int* param_1, int param_2, int param_3) {
    int iVar1;
    int* ecx;
    int* edx;

    param_1[1] = 0;
    if (param_2 != 0x3e) {
        ecx = reinterpret_cast<int*>(*param_1);
        ecx[2] = 0xc;
        edx = reinterpret_cast<int*>(*param_1);
        edx[3] = 0x3e;
        ecx = reinterpret_cast<int*>(*param_1);
        ecx[4] = param_2;
        edx = reinterpret_cast<int*>(*param_1);
        reinterpret_cast<void(__cdecl*)(int*)>(*edx)(param_1);
    }
    if (param_3 != 0x1b0) {
        ecx = reinterpret_cast<int*>(*param_1);
        ecx[2] = 0x15;
        edx = reinterpret_cast<int*>(*param_1);
        edx[3] = 0x1b0;
        ecx = reinterpret_cast<int*>(*param_1);
        ecx[4] = param_3;
        edx = reinterpret_cast<int*>(*param_1);
        reinterpret_cast<void(__cdecl*)(int*)>(*edx)(param_1);
    }
    iVar1 = *param_1;
    Runtime::MSVCRT::_memset(param_1, 0, 0x1b0);
    *param_1 = iVar1;
    *reinterpret_cast<uchar*>(reinterpret_cast<char*>(param_1) + 0x10) = 1;
    _Globals::jinit_memory_mgr((int)param_1);
    param_1[6] = 0;
    param_1[0x24] = 0;
    param_1[0x25] = 0;
    param_1[0x26] = 0;
    param_1[0x27] = 0;
    param_1[0x28] = 0;
    param_1[0x2c] = 0;
    param_1[0x29] = 0;
    param_1[0x2d] = 0;
    param_1[0x2a] = 0;
    param_1[0x2e] = 0;
    param_1[0x2b] = 0;
    param_1[0x2f] = 0;
    param_1[0x43] = 0;
    _Globals::jinit_marker_reader((int)param_1);
    _Globals::jinit_input_controller((int)param_1);
    param_1[5] = 200;
    return 0;
}
