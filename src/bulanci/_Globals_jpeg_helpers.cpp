#include <globals.h>

class _Globals {
public:
    static uchar jinit_memory_mgr(int param_1);
    static uchar jinit_marker_reader(int param_1);
    static uchar jinit_input_controller(int param_1);
};

__declspec(noinline) uchar _Globals::jinit_memory_mgr(int param_1) {
    (void)param_1;
    return 0;
}
__declspec(noinline) uchar _Globals::jinit_marker_reader(int param_1) {
    (void)param_1;
    return 0;
}
__declspec(noinline) uchar _Globals::jinit_input_controller(int param_1) {
    (void)param_1;
    return 0;
}
