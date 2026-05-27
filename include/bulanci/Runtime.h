#ifndef _RUNTIME
#define _RUNTIME

#include <globals.h>

/* Forward declarations of types referenced by parameter / return
 * positions but not (yet) defined in this project. Stubs only use
 * pointers so a struct forward-decl is enough. */
struct EHExceptionRecord;
struct EHRegistrationNode;
struct FILE;
struct LONG;
struct _CONTEXT;
struct _EXCEPTION_POINTERS;
struct _LDBL12;
struct _StartAddress;
struct _func_3814;
struct _func_void_void_ptr;
struct _s_FuncInfo;
struct _s_TryBlockMapEntry;
struct localeinfo_struct;

namespace Runtime {

class MSVCRT {
public:
	// !DECL 0x00447303 BEGIN
	/* 447303 */ uchar _CxxThrowException_8(uint param_1, uchar* param_2);
	// !DECL 0x00447303 END
	// !DECL 0x00447392 BEGIN
	/* 447392 */ static void _free(void* param_1);
	// !DECL 0x00447392 END
	// !DECL 0x00447420 BEGIN
	/* 447420 */ static int* _calloc_impl(uint param_1, uint param_2, uint* param_3);
	// !DECL 0x00447420 END
	// !DECL 0x0044753e BEGIN
	/* 44753E */ static void* _calloc(size_t param_1, size_t param_2);
	// !DECL 0x0044753e END
	// !DECL 0x0044757d BEGIN
	/* 44757D */ static void* _realloc(void* param_1, size_t param_2);
	// !DECL 0x0044757d END
	// !DECL 0x00447798 BEGIN
	/* 447798 */ static void* _recalloc(void* param_1, size_t param_2, size_t param_3);
	// !DECL 0x00447798 END
	// !DECL 0x004477ec BEGIN
	/* 4477EC */ static int _rand();
	// !DECL 0x004477ec END
	// !DECL 0x0044780e BEGIN
	/* 44780E */ void _JumpToContinuation(void* param_1, EHRegistrationNode* param_2);
	// !DECL 0x0044780e END
	// !DECL 0x00447845 BEGIN
	/* 447845 */ void _UnwindNestedFrames(EHRegistrationNode* param_1, EHExceptionRecord* param_2);
	// !DECL 0x00447845 END
	// !DECL 0x004478fd BEGIN
	/* 4478FD */ static int _CallSETranslator(EHExceptionRecord* param_1, EHRegistrationNode* param_2, void* param_3, void* param_4, _s_FuncInfo* param_5, int param_6, EHRegistrationNode* param_7);
	// !DECL 0x004478fd END
	// !DECL 0x00447a6f BEGIN
	/* 447A6F */ static _s_TryBlockMapEntry* _GetRangeOfTrysToCheck(_s_FuncInfo* param_1, int param_2, int param_3, uint* param_4, uint* param_5);
	// !DECL 0x00447a6f END
	// !DECL 0x00447ae2 BEGIN
	/* 447AE2 */ static uint* _CreateFrameInfo(uint* param_1, uint param_2);
	// !DECL 0x00447ae2 END
	// !DECL 0x00447b0a BEGIN
	/* 447B0A */ static uint _IsExceptionObjectToBeDestroyed(int param_1);
	// !DECL 0x00447b0a END
	// !DECL 0x00447b2b BEGIN
	/* 447B2B */ static uchar _FindAndUnlinkFrame(void* param_1);
	// !DECL 0x00447b2b END
	// !DECL 0x00447b77 BEGIN
	/* 447B77 */ static void* _CallCatchBlock2(EHRegistrationNode* param_1, _s_FuncInfo* param_2, void* param_3, int param_4, ulong param_5);
	// !DECL 0x00447b77 END
	// !DECL 0x00447bd5 BEGIN
	/* 447BD5 */ void _security_check_cookie(int param_1);
	// !DECL 0x00447bd5 END
	// !DECL 0x00447cb0 BEGIN
	/* 447CB0 */ uchar _alloca_probe();
	// !DECL 0x00447cb0 END
	// !DECL 0x00447ce0 BEGIN
	/* 447CE0 */ static void* _memset(void* param_1, int param_2, size_t param_3);
	// !DECL 0x00447ce0 END
	// !DECL 0x00447d5a BEGIN
	/* 447D5A */ static uint _onexit_nolock(uint param_1);
	// !DECL 0x00447d5a END
	// !DECL 0x00447e36 BEGIN
	/* 447E36 */ static int _onexit(int param_1);
	// !DECL 0x00447e36 END
	// !DECL 0x00447e72 BEGIN
	/* 447E72 */ static int _atexit(_func_3814* param_1);
	// !DECL 0x00447e72 END
	// !DECL 0x00447e90 BEGIN
	/* 447E90 */ static void* _memcpy(void* param_1, void* param_2, size_t param_3);
	// !DECL 0x00447e90 END
	// !DECL 0x00448200 BEGIN
	/* 448200 */ int _alldiv(uint param_1, uint param_2, uint param_3, uint param_4);
	// !DECL 0x00448200 END
	// !DECL 0x004482b0 BEGIN
	/* 4482B0 */ longlong _allmul(uint param_1, int param_2, uint param_3, int param_4);
	// !DECL 0x004482b0 END
	// !DECL 0x004486bc BEGIN
	/* 4486BC */ void _ArrayUnwind(void* param_1, uint param_2, int param_3, _func_void_void_ptr* param_4);
	// !DECL 0x004486bc END
	// !DECL 0x004487ad BEGIN
	/* 4487AD */ static void _freea(void* param_1);
	// !DECL 0x004487ad END
	// !DECL 0x0044884a BEGIN
	/* 44884A */ static int _wcslwr_s_l_stat(wchar_t* param_1, uint param_2, localeinfo_struct* param_3);
	// !DECL 0x0044884a END
	// !DECL 0x00448a04 BEGIN
	/* 448A04 */ static wchar_t* _wcslwr(wchar_t* param_1);
	// !DECL 0x00448a04 END
	// !DECL 0x00448a98 BEGIN
	/* 448A98 */ uchar _cfltcvt_init();
	// !DECL 0x00448a98 END
	// !DECL 0x00448af8 BEGIN
	/* 448AF8 */ static void _fpmath(int param_1);
	// !DECL 0x00448af8 END
	// !DECL 0x00448b20 BEGIN
	/* 448B20 */ uint _alloca_probe_16();
	// !DECL 0x00448b20 END
	// !DECL 0x00448b36 BEGIN
	/* 448B36 */ uint _alloca_probe_8();
	// !DECL 0x00448b36 END
	// !DECL 0x00448c60 BEGIN
	/* 448C60 */ int _tmainCRTStartup();
	// !DECL 0x00448c60 END
	// !DECL 0x00448e40 BEGIN
	/* 448E40 */ uchar _mainCRTStartup();
	// !DECL 0x00448e40 END
	// !DECL 0x00448ef0 BEGIN
	/* 448EF0 */ static void* _memmove(void* param_1, void* param_2, size_t param_3);
	// !DECL 0x00448ef0 END
	// !DECL 0x00449255 BEGIN
	/* 449255 */ static wchar_t* _wcsncpy(wchar_t* param_1, wchar_t* param_2, size_t param_3);
	// !DECL 0x00449255 END
	// !DECL 0x00449292 BEGIN
	/* 449292 */ static int _vswprintf_l(wchar_t* param_1, size_t param_2, wchar_t* param_3, int param_4, int param_5);
	// !DECL 0x00449292 END
	// !DECL 0x00449329 BEGIN
	/* 449329 */ static int _vswprintf(wchar_t* param_1, wchar_t* param_2, int param_3);
	// !DECL 0x00449329 END
	// !DECL 0x00449340 BEGIN
	/* 449340 */ static int* _V6_HeapAlloc(uint* param_1);
	// !DECL 0x00449340 END
	// !DECL 0x0044938f BEGIN
	/* 44938F */ static void* _malloc(size_t param_1);
	// !DECL 0x0044938f END
	// !DECL 0x00449452 BEGIN
	/* 449452 */ static size_t _msize(void* param_1);
	// !DECL 0x00449452 END
	// !DECL 0x004494f5 BEGIN
	/* 4494F5 */ int _swprintf(wchar_t* param_1, ...);
	// !DECL 0x004494f5 END
	// !DECL 0x00449590 BEGIN
	/* 449590 */ int _aulldiv(uint param_1, uint param_2, uint param_3, uint param_4);
	// !DECL 0x00449590 END
	// !DECL 0x004495f8 BEGIN
	/* 4495F8 */ static void _endthreadex(uint param_1);
	// !DECL 0x004495f8 END
	// !DECL 0x00449631 BEGIN
	/* 449631 */ uchar _callthreadstartex();
	// !DECL 0x00449631 END
	// !DECL 0x004496f2 BEGIN
	/* 4496F2 */ static int _beginthreadex(void* param_1, uint param_2, _StartAddress* param_3, void* param_4, uint param_5, uint* param_6);
	// !DECL 0x004496f2 END
	// !DECL 0x004497a0 BEGIN
	/* 4497A0 */ static int _get_errno_from_oserr(ulong param_1);
	// !DECL 0x004497a0 END
	// !DECL 0x004497db BEGIN
	/* 4497DB */ static int* _errno();
	// !DECL 0x004497db END
	// !DECL 0x004497ee BEGIN
	/* 4497EE */ static ulong* _doserrno();
	// !DECL 0x004497ee END
	// !DECL 0x00449801 BEGIN
	/* 449801 */ static void _dosmaperr(ulong param_1);
	// !DECL 0x00449801 END
	// !DECL 0x0044981f BEGIN
	/* 44981F */ long _CxxUnhandledExceptionFilter(_EXCEPTION_POINTERS* param_1);
	// !DECL 0x0044981f END
	// !DECL 0x00449940 BEGIN
	/* 449940 */ static int _strcmp(char* param_1, char* param_2);
	// !DECL 0x00449940 END
	// !DECL 0x004499c8 BEGIN
	/* 4499C8 */ uint _heap_select();
	// !DECL 0x004499c8 END
	// !DECL 0x00449a23 BEGIN
	/* 449A23 */ static int _heap_init();
	// !DECL 0x00449a23 END
	// !DECL 0x00449a7d BEGIN
	/* 449A7D */ static int _mtinitlocks();
	// !DECL 0x00449a7d END
	// !DECL 0x00449ac6 BEGIN
	/* 449AC6 */ static void _mtdeletelocks();
	// !DECL 0x00449ac6 END
	// !DECL 0x00449b30 BEGIN
	/* 449B30 */ static int _mtinitlocknum(int param_1);
	// !DECL 0x00449b30 END
	// !DECL 0x00449bf3 BEGIN
	/* 449BF3 */ static void _lock(int param_1);
	// !DECL 0x00449bf3 END
	// !DECL 0x00449c24 BEGIN
	/* 449C24 */ static uint _sbh_heap_init(uint param_1);
	// !DECL 0x00449c24 END
	// !DECL 0x00449c97 BEGIN
	/* 449C97 */ static uchar _sbh_free_block(uint* param_1, int param_2);
	// !DECL 0x00449c97 END
	// !DECL 0x00449fab BEGIN
	/* 449FAB */ uint* _sbh_alloc_new_region();
	// !DECL 0x00449fab END
	// !DECL 0x0044a05b BEGIN
	/* 44A05B */ static int _sbh_alloc_new_group(int param_1);
	// !DECL 0x0044a05b END
	// !DECL 0x0044a161 BEGIN
	/* 44A161 */ static uint _sbh_resize_block(uint* param_1, int param_2, int param_3);
	// !DECL 0x0044a161 END
	// !DECL 0x0044a440 BEGIN
	/* 44A440 */ static int* _sbh_alloc_block(uint* param_1);
	// !DECL 0x0044a440 END
	// !DECL 0x0044a724 BEGIN
	/* 44A724 */ static uchar _SEH_prolog4(uint param_1, int param_2);
	// !DECL 0x0044a724 END
	// !DECL 0x0044a769 BEGIN
	/* 44A769 */ uchar _SEH_epilog4();
	// !DECL 0x0044a769 END
	// !DECL 0x0044a780 BEGIN
	/* 44A780 */ static uint _except_handler4(uint param_1, int param_2);
	// !DECL 0x0044a780 END
	// !DECL 0x0044a920 BEGIN
	/* 44A920 */ static int _callnewh(size_t param_1);
	// !DECL 0x0044a920 END
	// !DECL 0x0044a94c BEGIN
	/* 44A94C */ static void _invoke_watson(wchar_t* param_1, wchar_t* param_2, wchar_t* param_3, uint param_4, int param_5);
	// !DECL 0x0044a94c END
	// !DECL 0x0044aa48 BEGIN
	/* 44AA48 */ static uchar _invalid_parameter(wchar_t* param_1, wchar_t* param_2, wchar_t* param_3, uint param_4, int param_5);
	// !DECL 0x0044aa48 END
	// !DECL 0x0044aa6c BEGIN
	/* 44AA6C */ static int _encode_pointer(int param_1);
	// !DECL 0x0044aa6c END
	// !DECL 0x0044aacf BEGIN
	/* 44AACF */ uchar _encoded_null();
	// !DECL 0x0044aacf END
	// !DECL 0x0044aad8 BEGIN
	/* 44AAD8 */ static int _decode_pointer(int param_1);
	// !DECL 0x0044aad8 END
	// !DECL 0x0044ab5f BEGIN
	/* 44AB5F */ uchar _set_flsgetvalue();
	// !DECL 0x0044ab5f END
	// !DECL 0x0044ab89 BEGIN
	/* 44AB89 */ uchar _fls_setvalue_8(uint param_1, uint param_2);
	// !DECL 0x0044ab89 END
	// !DECL 0x0044aba2 BEGIN
	/* 44ABA2 */ static void _mtterm();
	// !DECL 0x0044aba2 END
	// !DECL 0x0044abdf BEGIN
	/* 44ABDF */ static void _initptd(int param_1, int param_2);
	// !DECL 0x0044abdf END
	// !DECL 0x0044ac93 BEGIN
	/* 44AC93 */ static int _getptd_noexit();
	// !DECL 0x0044ac93 END
	// !DECL 0x0044ad16 BEGIN
	/* 44AD16 */ static int _getptd();
	// !DECL 0x0044ad16 END
	// !DECL 0x0044ad2e BEGIN
	/* 44AD2E */ uchar _freefls_4(void* param_1);
	// !DECL 0x0044ad2e END
	// !DECL 0x0044ae4f BEGIN
	/* 44AE4F */ static void _freeptd(int param_1);
	// !DECL 0x0044ae4f END
	// !DECL 0x0044aeb8 BEGIN
	/* 44AEB8 */ static int _mtinit();
	// !DECL 0x0044aeb8 END
	// !DECL 0x0044b069 BEGIN
	/* 44B069 */ static uint _TypeMatch(uchar* param_1, uchar* param_2, uint* param_3);
	// !DECL 0x0044b069 END
	// !DECL 0x0044b109 BEGIN
	/* 44B109 */ static uchar _FrameUnwindToState(int param_1, uint param_2, int param_3, int param_4);
	// !DECL 0x0044b109 END
	// !DECL 0x0044b22f BEGIN
	/* 44B22F */ static uchar _DestructExceptionObject(int* param_1);
	// !DECL 0x0044b22f END
	// !DECL 0x0044b283 BEGIN
	/* 44B283 */ static int _AdjustPointer(int param_1, int* param_2);
	// !DECL 0x0044b283 END
	// !DECL 0x0044b506 BEGIN
	/* 44B506 */ static char _BuildCatchObjectHelper(int param_1, int* param_2, uint* param_3, uchar* param_4);
	// !DECL 0x0044b506 END
	// !DECL 0x0044b685 BEGIN
	/* 44B685 */ static uchar _BuildCatchObject(int param_1, int* param_2, uint* param_3, uchar* param_4);
	// !DECL 0x0044b685 END
	// !DECL 0x0044bbe2 BEGIN
	/* 44BBE2 */ static uint _InternalCxxFrameHandler(EHExceptionRecord* param_1, EHRegistrationNode* param_2, _CONTEXT* param_3, void* param_4, _s_FuncInfo* param_5, int param_6, EHRegistrationNode* param_7, uchar param_8);
	// !DECL 0x0044bbe2 END
	// !DECL 0x0044bd12 BEGIN
	/* 44BD12 */ static void _inconsistency();
	// !DECL 0x0044bd12 END
	// !DECL 0x0044bd49 BEGIN
	/* 44BD49 */ uchar _initp_eh_hooks();
	// !DECL 0x0044bd49 END
	// !DECL 0x0044bd60 BEGIN
	/* 44BD60 */ uchar _CallSettingFrame_12(uint param_1, uint param_2, int param_3);
	// !DECL 0x0044bd60 END
	// !DECL 0x0044bdac BEGIN
	/* 44BDAC */ static void _report_gsfailure();
	// !DECL 0x0044bdac END
	// !DECL 0x0044c004 BEGIN
	/* 44C004 */ static void* _VEC_memzero(void* param_1, uint param_2, uint param_3);
	// !DECL 0x0044c004 END
	// !DECL 0x0044c0a7 BEGIN
	/* 44C0A7 */ static void* _malloc_crt(size_t param_1);
	// !DECL 0x0044c0a7 END
	// !DECL 0x0044c0e7 BEGIN
	/* 44C0E7 */ static void* _calloc_crt(size_t param_1, size_t param_2);
	// !DECL 0x0044c0e7 END
	// !DECL 0x0044c12f BEGIN
	/* 44C12F */ static void* _realloc_crt(void* param_1, size_t param_2);
	// !DECL 0x0044c12f END
	// !DECL 0x0044c17a BEGIN
	/* 44C17A */ static void _amsg_exit(int param_1);
	// !DECL 0x0044c17a END
	// !DECL 0x0044c1c4 BEGIN
	/* 44C1C4 */ static void _crtExitProcess(int param_1);
	// !DECL 0x0044c1c4 END
	// !DECL 0x0044c1eb BEGIN
	/* 44C1EB */ static uchar _initterm(uint* param_1);
	// !DECL 0x0044c1eb END
	// !DECL 0x0044c203 BEGIN
	/* 44C203 */ static uchar _initterm_e(uint* param_1, uint* param_2);
	// !DECL 0x0044c203 END
	// !DECL 0x0044c223 BEGIN
	/* 44C223 */ static errno_t _get_osplatform(uint* param_1);
	// !DECL 0x0044c223 END
	// !DECL 0x0044c25a BEGIN
	/* 44C25A */ static errno_t _get_winmajor(uint* param_1);
	// !DECL 0x0044c25a END
	// !DECL 0x0044c296 BEGIN
	/* 44C296 */ static int _cinit(int param_1);
	// !DECL 0x0044c296 END
	// !DECL 0x0044c3f6 BEGIN
	/* 44C3F6 */ static void _exit(int param_1);
	// !DECL 0x0044c3f6 END
	// !DECL 0x0044c407 BEGIN
	/* 44C407 */ static uchar _exit_0044c407(int param_1);
	// !DECL 0x0044c407 END
	// !DECL 0x0044c418 BEGIN
	/* 44C418 */ static void _cexit();
	// !DECL 0x0044c418 END
	// !DECL 0x0044c436 BEGIN
	/* 44C436 */ static void _init_pointers();
	// !DECL 0x0044c436 END
	// !DECL 0x0044c509 BEGIN
	/* 44C509 */ static uint* _VEC_memcpy(uint* param_1, uint* param_2, uint param_3);
	// !DECL 0x0044c509 END
	// !DECL 0x0044c7fa BEGIN
	/* 44C7FA */ static int _updatetmbcinfo();
	// !DECL 0x0044c7fa END
	// !DECL 0x0044c918 BEGIN
	/* 44C918 */ static uint _setmbcp_nolock(uint param_1, int param_2);
	// !DECL 0x0044c918 END
	// !DECL 0x0044cac3 BEGIN
	/* 44CAC3 */ static int _setmbcp(int param_1);
	// !DECL 0x0044cac3 END
	// !DECL 0x0044cc5d BEGIN
	/* 44CC5D */ uint _initmbctable();
	// !DECL 0x0044cc5d END
	// !DECL 0x0044cc7b BEGIN
	/* 44CC7B */ static uchar _freetlocinfo(void* param_1);
	// !DECL 0x0044cc7b END
	// !DECL 0x0044cdbb BEGIN
	/* 44CDBB */ static uchar _addlocaleref(LONG* param_1);
	// !DECL 0x0044cdbb END
	// !DECL 0x0044ce41 BEGIN
	/* 44CE41 */ static LONG* _removelocaleref(LONG* param_1);
	// !DECL 0x0044ce41 END
	// !DECL 0x0044cf81 BEGIN
	/* 44CF81 */ static errno_t _wcscpy_s(wchar_t* param_1, int param_2, wchar_t* param_3);
	// !DECL 0x0044cf81 END
	// !DECL 0x0044cfed BEGIN
	/* 44CFED */ static int _crtLCMapStringW_stat(localeinfo_struct* param_1, ulong param_2, ulong param_3, wchar_t* param_4, int param_5, wchar_t* param_6, int param_7, int param_8);
	// !DECL 0x0044cfed END
	// !DECL 0x0044d24e BEGIN
	/* 44D24E */ static int _crtLCMapStringW(int param_1, DWORD param_2, int param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0044d24e END
	// !DECL 0x0044d28e BEGIN
	/* 44D28E */ static size_t _wcsnlen(wchar_t* param_1, size_t param_2);
	// !DECL 0x0044d28e END
	// !DECL 0x0044d2aa BEGIN
	/* 44D2AA */ static void _abort();
	// !DECL 0x0044d2aa END
	// !DECL 0x0044d3bb BEGIN
	/* 44D3BB */ static void _NMSG_WRITE(int param_1);
	// !DECL 0x0044d3bb END
	// !DECL 0x0044d55b BEGIN
	/* 44D55B */ static void _FF_MSGBANNER();
	// !DECL 0x0044d55b END
	// !DECL 0x0044d59e BEGIN
	/* 44D59E */ static void _forcdecpt_l(char* param_1, int param_2);
	// !DECL 0x0044d59e END
	// !DECL 0x0044d60f BEGIN
	/* 44D60F */ static void _cropzeros_l(char* param_1, int param_2);
	// !DECL 0x0044d60f END
	// !DECL 0x0044d6a5 BEGIN
	/* 44D6A5 */ static void _fassign_l(int param_1, char* param_2, char* param_3, int param_4);
	// !DECL 0x0044d6a5 END
	// !DECL 0x0044d6fc BEGIN
	/* 44D6FC */ uchar _shift();
	// !DECL 0x0044d6fc END
	// !DECL 0x0044d735 BEGIN
	/* 44D735 */ static int _cftoe2_l(uint param_1, int param_2, int param_3, int* param_4, char param_5, localeinfo_struct* param_6);
	// !DECL 0x0044d735 END
	// !DECL 0x0044d8a2 BEGIN
	/* 44D8A2 */ static uchar _cftoe_l(double* param_1, uchar* param_2, uint param_3, int param_4, int param_5, localeinfo_struct* param_6);
	// !DECL 0x0044d8a2 END
	// !DECL 0x0044d970 BEGIN
	/* 44D970 */ static errno_t _cftoe(double* param_1, char* param_2, size_t param_3, int param_4, int param_5);
	// !DECL 0x0044d970 END
	// !DECL 0x0044d98e BEGIN
	/* 44D98E */ static int _cftoa_l(double* param_1, uchar* param_2, uint param_3, size_t param_4, int param_5, localeinfo_struct* param_6);
	// !DECL 0x0044d98e END
	// !DECL 0x0044dcff BEGIN
	/* 44DCFF */ uint _cftof2_l(int param_1, size_t param_2, char param_3, localeinfo_struct* param_4);
	// !DECL 0x0044dcff END
	// !DECL 0x0044ddf4 BEGIN
	/* 44DDF4 */ static errno_t _cftof_l(double* param_1, uchar* param_2, int param_3, size_t param_4, localeinfo_struct* param_5);
	// !DECL 0x0044ddf4 END
	// !DECL 0x0044dead BEGIN
	/* 44DEAD */ static uchar _cftog_l(double* param_1, uchar* param_2, uint param_3, size_t param_4, int param_5, localeinfo_struct* param_6);
	// !DECL 0x0044dead END
	// !DECL 0x0044dfa5 BEGIN
	/* 44DFA5 */ static errno_t _cfltcvt_l(double* param_1, char* param_2, size_t param_3, int param_4, int param_5, int param_6, int param_7);
	// !DECL 0x0044dfa5 END
	// !DECL 0x0044e02b BEGIN
	/* 44E02B */ static errno_t _cfltcvt(double* param_1, char* param_2, size_t param_3, int param_4, int param_5, int param_6);
	// !DECL 0x0044e02b END
	// !DECL 0x0044e04c BEGIN
	/* 44E04C */ uchar _initp_misc_cfltcvt_tab();
	// !DECL 0x0044e04c END
	// !DECL 0x0044e06b BEGIN
	/* 44E06B */ uchar _setdefaultprecision();
	// !DECL 0x0044e06b END
	// !DECL 0x0044e094 BEGIN
	/* 44E094 */ uint _ms_p5_test_fdiv();
	// !DECL 0x0044e094 END
	// !DECL 0x0044e0d0 BEGIN
	/* 44E0D0 */ uchar _ms_p5_mp_test_fdiv();
	// !DECL 0x0044e0d0 END
	// !DECL 0x0044e0f9 BEGIN
	/* 44E0F9 */ static int _XcptFilter(ulong param_1, _EXCEPTION_POINTERS* param_2);
	// !DECL 0x0044e0f9 END
	// !DECL 0x0044e268 BEGIN
	/* 44E268 */ uchar* _wincmdln();
	// !DECL 0x0044e268 END
	// !DECL 0x0044e2c5 BEGIN
	/* 44E2C5 */ static int _setenvp();
	// !DECL 0x0044e2c5 END
	// !DECL 0x0044e538 BEGIN
	/* 44E538 */ static int _setargv();
	// !DECL 0x0044e538 END
	// !DECL 0x0044e5f1 BEGIN
	/* 44E5F1 */ static LPVOID _crtGetEnvironmentStringsA();
	// !DECL 0x0044e5f1 END
	// !DECL 0x0044e726 BEGIN
	/* 44E726 */ static int _ioinit();
	// !DECL 0x0044e726 END
	// !DECL 0x0044e966 BEGIN
	/* 44E966 */ uchar _RTC_Initialize();
	// !DECL 0x0044e966 END
	// !DECL 0x0044e9ae BEGIN
	/* 44E9AE */ static void _security_init_cookie();
	// !DECL 0x0044e9ae END
	// !DECL 0x0044edc0 BEGIN
	/* 44EDC0 */ static int _flsbuf(int param_1, FILE* param_2);
	// !DECL 0x0044edc0 END
	// !DECL 0x0044efb9 BEGIN
	/* 44EFB9 */ static int _woutput_l(FILE* param_1, wchar_t* param_2, int param_3, int param_4);
	// !DECL 0x0044efb9 END
	// !DECL 0x0044f8d0 BEGIN
	/* 44F8D0 */ static BOOL _ValidateImageBase(int param_1);
	// !DECL 0x0044f8d0 END
	// !DECL 0x0044f900 BEGIN
	/* 44F900 */ static int _FindPESection(int param_1, DWORD_PTR param_2);
	// !DECL 0x0044f900 END
	// !DECL 0x0044f942 BEGIN
	/* 44F942 */ static BOOL _IsNonwritableInCurrentImage(int param_1);
	// !DECL 0x0044f942 END
	// !DECL 0x0044f9ae BEGIN
	/* 44F9AE */ static int _ValidateRead(void* param_1, uint param_2);
	// !DECL 0x0044f9ae END
	// !DECL 0x0044f9bb BEGIN
	/* 44F9BB */ static errno_t _strcpy_s(char* param_1, int param_2, char* param_3);
	// !DECL 0x0044f9bb END
	// !DECL 0x0044fa20 BEGIN
	/* 44FA20 */ static size_t _strlen(char* param_1);
	// !DECL 0x0044fa20 END
	// !DECL 0x0044fab5 BEGIN
	/* 44FAB5 */ uint _crtInitCritSecNoSpinCount_8(LPCRITICAL_SECTION param_1);
	// !DECL 0x0044fab5 END
	// !DECL 0x0044fac5 BEGIN
	/* 44FAC5 */ static int _crtInitCritSecAndSpinCount(uint param_1, uint param_2);
	// !DECL 0x0044fac5 END
	// !DECL 0x0044fb8c BEGIN
	/* 44FB8C */ static uchar _local_unwind4(uint* param_1, int param_2, uint param_3);
	// !DECL 0x0044fb8c END
	// !DECL 0x0044fc62 BEGIN
	/* 44FC62 */ uchar _seh_longjmp_unwind4_4(int param_1);
	// !DECL 0x0044fc62 END
	// !DECL 0x0044fc7e BEGIN
	/* 44FC7E */ uchar _EH4_CallFilterFunc(uchar* param_1);
	// !DECL 0x0044fc7e END
	// !DECL 0x0044fc95 BEGIN
	/* 44FC95 */ uchar _EH4_TransferToHandler(uchar* param_1);
	// !DECL 0x0044fc95 END
	// !DECL 0x0044fcae BEGIN
	/* 44FCAE */ uchar _EH4_GlobalUnwind(PVOID param_1);
	// !DECL 0x0044fcae END
	// !DECL 0x0044fcc8 BEGIN
	/* 44FCC8 */ uchar _EH4_LocalUnwind(int param_1, uint param_2, uint param_3, uint* param_4);
	// !DECL 0x0044fcc8 END
	// !DECL 0x0044fce7 BEGIN
	/* 44FCE7 */ static uchar _EH_prolog3_catch(int param_1);
	// !DECL 0x0044fce7 END
	// !DECL 0x0044fd20 BEGIN
	/* 44FD20 */ static uchar _global_unwind2(PVOID param_1);
	// !DECL 0x0044fd20 END
	// !DECL 0x0044fd85 BEGIN
	/* 44FD85 */ static uchar _local_unwind2(int param_1, uint param_2);
	// !DECL 0x0044fd85 END
	// !DECL 0x0044fe2c BEGIN
	/* 44FE2C */ uint _NLG_Notify1(uint param_1);
	// !DECL 0x0044fe2c END
	// !DECL 0x0044fe35 BEGIN
	/* 44FE35 */ void _NLG_Notify(ulong param_1);
	// !DECL 0x0044fe35 END
	// !DECL 0x0044fea7 BEGIN
	/* 44FEA7 */ uint _get_sse2_info();
	// !DECL 0x0044fea7 END
	// !DECL 0x0044ff14 BEGIN
	/* 44FF14 */ static uchar _initp_misc_winsig(uint param_1);
	// !DECL 0x0044ff14 END
	// !DECL 0x0044ff61 BEGIN
	/* 44FF61 */ static int _get_sigabrt();
	// !DECL 0x0044ff61 END
	// !DECL 0x0044ff6e BEGIN
	/* 44FF6E */ static int _raise(int param_1);
	// !DECL 0x0044ff6e END
	// !DECL 0x00450128 BEGIN
	/* 450128 */ static int _crtLCMapStringA_stat(localeinfo_struct* param_1, ulong param_2, ulong param_3, char* param_4, int param_5, char* param_6, int param_7, int param_8, int param_9);
	// !DECL 0x00450128 END
	// !DECL 0x004504ca BEGIN
	/* 4504CA */ static int _crtLCMapStringA(int param_1, int param_2, DWORD param_3, int param_4, int param_5, int param_6, int param_7, int param_8, BOOL param_9);
	// !DECL 0x004504ca END
	// !DECL 0x0045050d BEGIN
	/* 45050D */ static int _crtGetStringTypeA_stat(localeinfo_struct* param_1, ulong param_2, char* param_3, int param_4, ushort* param_5, int param_6, int param_7, int param_8);
	// !DECL 0x0045050d END
	// !DECL 0x004506c5 BEGIN
	/* 4506C5 */ static BOOL _crtGetStringTypeA(int param_1, DWORD param_2, int param_3, int param_4, int param_5, int param_6, BOOL param_7);
	// !DECL 0x004506c5 END
	// !DECL 0x00450705 BEGIN
	/* 450705 */ static uchar _free_lc_time(uint* param_1);
	// !DECL 0x00450705 END
	// !DECL 0x00450895 BEGIN
	/* 450895 */ static uchar _free_lconv_num(uint* param_1);
	// !DECL 0x00450895 END
	// !DECL 0x004508d5 BEGIN
	/* 4508D5 */ static uchar _free_lconv_mon(int param_1);
	// !DECL 0x004508d5 END
	// !DECL 0x0045095e BEGIN
	/* 45095E */ static errno_t _strcat_s(char* param_1, int param_2, char* param_3);
	// !DECL 0x0045095e END
	// !DECL 0x004509d0 BEGIN
	/* 4509D0 */ static size_t _strcspn(char* param_1, char* param_2);
	// !DECL 0x004509d0 END
	// !DECL 0x00450a16 BEGIN
	/* 450A16 */ static errno_t _strncpy_s(char* param_1, int param_2, char* param_3, int param_4);
	// !DECL 0x00450a16 END
	// !DECL 0x00450ad0 BEGIN
	/* 450AD0 */ static char* _strpbrk(char* param_1, char* param_2);
	// !DECL 0x00450ad0 END
	// !DECL 0x00450b10 BEGIN
	/* 450B10 */ static long _ansicp(int param_1);
	// !DECL 0x00450b10 END
	// !DECL 0x00450b57 BEGIN
	/* 450B57 */ static int _convertcp(int param_1, int param_2, char* param_3, uint* param_4, int param_5, int param_6);
	// !DECL 0x00450b57 END
	// !DECL 0x00450d09 BEGIN
	/* 450D09 */ static int _crtMessageBoxA(int param_1, int param_2, int param_3);
	// !DECL 0x00450d09 END
	// !DECL 0x00450ea7 BEGIN
	/* 450EA7 */ static int _set_error_mode(int param_1);
	// !DECL 0x00450ea7 END
	// !DECL 0x00450eed BEGIN
	/* 450EED */ static int _isdigit_l(int param_1, int param_2);
	// !DECL 0x00450eed END
	// !DECL 0x00450f3c BEGIN
	/* 450F3C */ static int _isdigit(int param_1);
	// !DECL 0x00450f3c END
	// !DECL 0x00450f70 BEGIN
	/* 450F70 */ static char* _strrchr(char* param_1, int param_2);
	// !DECL 0x00450f70 END
	// !DECL 0x00450f9d BEGIN
	/* 450F9D */ static int _tolower_l(int param_1, int param_2);
	// !DECL 0x00450f9d END
	// !DECL 0x004510b4 BEGIN
	/* 4510B4 */ static int _tolower(int param_1);
	// !DECL 0x004510b4 END
	// !DECL 0x00451227 BEGIN
	/* 451227 */ static errno_t _fptostr(char* param_1, size_t param_2, int param_3, int param_4);
	// !DECL 0x00451227 END
	// !DECL 0x004512e4 BEGIN
	/* 4512E4 */ static uchar _dtold(uint* param_1, uint* param_2);
	// !DECL 0x004512e4 END
	// !DECL 0x0045139f BEGIN
	/* 45139F */ static int _fltout2(int param_1, int param_2, char* param_3, size_t param_4);
	// !DECL 0x0045139f END
	// !DECL 0x00451430 BEGIN
	/* 451430 */ int _alldvrm(uint param_1, uint param_2, uint param_3, uint param_4);
	// !DECL 0x00451430 END
	// !DECL 0x00451510 BEGIN
	/* 451510 */ ulonglong _aullshr(uchar param_1, uint param_2);
	// !DECL 0x00451510 END
	// !DECL 0x00451538 BEGIN
	/* 451538 */ static errno_t _controlfp_s(uint* param_1, uint param_2, uint param_3);
	// !DECL 0x00451538 END
	// !DECL 0x004515f3 BEGIN
	/* 4515F3 */ static int _ismbblead(uint param_1);
	// !DECL 0x004515f3 END
	// !DECL 0x00451790 BEGIN
	/* 451790 */ int _aulldvrm(uint param_1, uint param_2, uint param_3, uint param_4);
	// !DECL 0x00451790 END
	// !DECL 0x0045250b BEGIN
	/* 45250B */ static uchar _mtold12(char* param_1, int param_2, uint* param_3);
	// !DECL 0x0045250b END
	// !DECL 0x004526d7 BEGIN
	/* 4526D7 */ static longlong _lseeki64_nolock(int param_1, longlong param_2, int param_3);
	// !DECL 0x004526d7 END
	// !DECL 0x0045275a BEGIN
	/* 45275A */ static longlong _lseeki64(int param_1, longlong param_2, int param_3);
	// !DECL 0x0045275a END
	// !DECL 0x00452873 BEGIN
	/* 452873 */ static int _write_nolock(int param_1, void* param_2, uint param_3);
	// !DECL 0x00452873 END
	// !DECL 0x00452e39 BEGIN
	/* 452E39 */ static int _write(int param_1, void* param_2, uint param_3);
	// !DECL 0x00452e39 END
	// !DECL 0x00452f15 BEGIN
	/* 452F15 */ static void _getbuf(FILE* param_1);
	// !DECL 0x00452f15 END
	// !DECL 0x00452f59 BEGIN
	/* 452F59 */ static int _isatty(int param_1);
	// !DECL 0x00452f59 END
	// !DECL 0x00453132 BEGIN
	/* 453132 */ static int _fileno(FILE* param_1);
	// !DECL 0x00453132 END
	// !DECL 0x0045315f BEGIN
	/* 45315F */ static int _fputwc_nolock(wchar_t param_1, FILE* param_2);
	// !DECL 0x0045315f END
	// !DECL 0x0045333d BEGIN
	/* 45333D */ static int _get_printf_count_output();
	// !DECL 0x0045333d END
	// !DECL 0x00453353 BEGIN
	/* 453353 */ static int _mbtowc_l(wchar_t* param_1, char* param_2, size_t param_3, int param_4);
	// !DECL 0x00453353 END
	// !DECL 0x00453466 BEGIN
	/* 453466 */ static int _mbtowc(wchar_t* param_1, char* param_2, size_t param_3);
	// !DECL 0x00453466 END
	// !DECL 0x0045347d BEGIN
	/* 45347D */ static int _isleadbyte_l(int param_1, int param_2);
	// !DECL 0x0045347d END
	// !DECL 0x004534b3 BEGIN
	/* 4534B3 */ static int _isleadbyte(int param_1);
	// !DECL 0x004534b3 END
	// !DECL 0x004534c1 BEGIN
	/* 4534C1 */ static long _atol(char* param_1);
	// !DECL 0x004534c1 END
	// !DECL 0x004534d2 BEGIN
	/* 4534D2 */ static int _isctype_l(int param_1, int param_2, int param_3);
	// !DECL 0x004534d2 END
	// !DECL 0x00453588 BEGIN
	/* 453588 */ static uint _strgtold12_l(_LDBL12* param_1, char** param_2, char* param_3, int param_4, int param_5, int param_6, int param_7, int param_8);
	// !DECL 0x00453588 END
	// !DECL 0x00454514 BEGIN
	/* 454514 */ uint _hw_cw();
	// !DECL 0x00454514 END
	// !DECL 0x004545a2 BEGIN
	/* 4545A2 */ uint _hw_cw_sse2(uint param_1, uint param_2);
	// !DECL 0x004545a2 END
	// !DECL 0x00454642 BEGIN
	/* 454642 */ static uint _control87(uint param_1, uint param_2);
	// !DECL 0x00454642 END
	// !DECL 0x004549c6 BEGIN
	/* 4549C6 */ static int _get_osfhandle(int param_1);
	// !DECL 0x004549c6 END
	// !DECL 0x00454a37 BEGIN
	/* 454A37 */ static int _lock_fhandle(int param_1);
	// !DECL 0x00454a37 END
	// !DECL 0x00454af9 BEGIN
	/* 454AF9 */ static int _putwch_nolock(wchar_t param_1);
	// !DECL 0x00454af9 END
	// !DECL 0x00454de1 BEGIN
	/* 454DE1 */ static int _flswbuf(int param_1, FILE* param_2);
	// !DECL 0x00454de1 END
	// !DECL 0x00454f51 BEGIN
	/* 454F51 */ static errno_t _wctomb_s_l(int* param_1, char* param_2, size_t param_3, wchar_t param_4, int param_5);
	// !DECL 0x00454f51 END
	// !DECL 0x004550b0 BEGIN
	/* 4550B0 */ static errno_t _wctomb_s(int* param_1, char* param_2, int param_3, wchar_t param_4);
	// !DECL 0x004550b0 END
	// !DECL 0x004552f6 BEGIN
	/* 4552F6 */ static long _strtol(char* param_1, char** param_2, int param_3);
	// !DECL 0x004552f6 END
	// !DECL 0x00455320 BEGIN
	/* 455320 */ static int _ascii_strnicmp(char* param_1, char* param_2, size_t param_3);
	// !DECL 0x00455320 END
	// !DECL 0x00455381 BEGIN
	/* 455381 */ static uchar _set_fpsr_sse2(uint param_1);
	// !DECL 0x00455381 END
	// !DECL 0x004553f3 BEGIN
	/* 4553F3 */ static void _initconout();
	// !DECL 0x004553f3 END
};

} // namespace Runtime

#endif
