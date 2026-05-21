#include "CDSFileStream.h"

// !PROLOGUE BEGIN
typedef long* PLONG;
typedef DWORD* LPDWORD;
#include "_Globals.h"

extern "C" {
    __declspec(dllimport) BOOL __stdcall ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, void*);
    __declspec(dllimport) BOOL __stdcall WriteFile(HANDLE, const void*, DWORD, LPDWORD, void*);
    __declspec(dllimport) BOOL __stdcall LockFile(HANDLE, DWORD, DWORD, DWORD, DWORD);
    __declspec(dllimport) BOOL __stdcall UnlockFile(HANDLE, DWORD, DWORD, DWORD, DWORD);
    __declspec(dllimport) DWORD __stdcall SetFilePointer(HANDLE, long, PLONG, DWORD);
    __declspec(dllimport) DWORD __stdcall GetLastError();
    __declspec(dllimport) BOOL __stdcall SetEndOfFile(HANDLE);
    __declspec(dllimport) DWORD __stdcall GetFileSize(HANDLE, LPDWORD);
    __declspec(dllimport) BOOL __stdcall FlushFileBuffers(HANDLE);
}

inline HANDLE FileHandle(CDSFileStream* self) {
    return *reinterpret_cast<HANDLE*>(reinterpret_cast<char*>(self) + 16);
}

inline int* SelfOrNull(CDSFileStream* self) {
    /* MSVC8 /O2 emits this as `lea eax,[esi-0xc]; neg; sbb; and eax,esi` --
     * a branchless `(self != 0xc) ? self : 0`.  The `char*` arithmetic
     * coaxes the LEA encoding over the longer mov+sub form. */
    char* adjusted = reinterpret_cast<char*>(self) - 0xc;
    return adjusted ? reinterpret_cast<int*>(self) : 0;
}
// !PROLOGUE END

// !FUNC 0x00401540 BEGIN
/* 401540-4015FC 000BC */
uint* CDSFileStream::FUN_00401540(int param_1, uchar* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00401540 END

// !FUNC 0x00401600 BEGIN
/* 401600-401606 00006 */
uchar* CDSFileStream::FUN_00401600() {
    return reinterpret_cast<uchar*>(&DAT_004b7c20);
}
// !FUNC 0x00401600 END

// !FUNC 0x00401610 BEGIN
/* 401610-401616 00006 */
uchar* CDSFileStream::FUN_00401610() {
    return reinterpret_cast<uchar*>(&DAT_004b7c28);
}
// !FUNC 0x00401610 END

// !FUNC 0x00401620 BEGIN
/* 401620-401626 00006 */
uchar* CDSFileStream::FUN_00401620() {
    return reinterpret_cast<uchar*>(&DAT_004b7ce0);
}
// !FUNC 0x00401620 END

// !FUNC 0x00401630 BEGIN
/* 401630-401636 00006 */
uchar* CDSFileStream::FUN_00401630() {
    return reinterpret_cast<uchar*>(&DAT_004b7e78);
}
// !FUNC 0x00401630 END

// !FUNC 0x00401640 BEGIN
/* 401640-401654 00014 */
void CDSFileStream::CloseStream(int param_1) { STUB_BODY(); }
// !FUNC 0x00401640 END

// !FUNC 0x00401660 BEGIN
/* 401660-401689 00029 */
uchar CDSFileStream::FUN_00401660(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401660 END

// !FUNC 0x00401690 BEGIN
/* 401690-401698 00008 */
uchar CDSFileStream::FUN_00401690(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401690 END

// !FUNC 0x004016a0 BEGIN
/* 4016A0-4016A8 00008 */
uchar CDSFileStream::FUN_004016a0(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004016a0 END

// !FUNC 0x004016b0 BEGIN
/* 4016B0-4016B8 00008 */
uchar CDSFileStream::ScalarDeletingDtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004016b0 END

// !FUNC 0x004016c0 BEGIN
/* 4016C0-40173B 0007B */
uchar CDSFileStream::FUN_004016c0(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004016c0 END

// !FUNC 0x00401740 BEGIN
/* 401740-401766 00026 */
uint* CDSFileStream::GetStreamName(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401740 END

// !FUNC 0x00401770 BEGIN
/* 401770-40178E 0001E */
uint* CDSFileStream::FUN_00401770(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401770 END

// !FUNC 0x00409260 BEGIN
/* 409260-409268 00008 */
uchar CDSFileStream::FUN_00409260(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409260 END

// !FUNC 0x00409450 BEGIN
/* 409450-409458 00008 */
uchar CDSFileStream::FUN_00409450(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409450 END

// !FUNC 0x00409490 BEGIN
/* 409490-409498 00008 */
uchar CDSFileStream::FUN_00409490(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409490 END

// !FUNC 0x00409970 BEGIN
/* 409970-409978 00008 */
uchar CDSFileStream::FUN_00409970(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00409970 END

// !FUNC 0x00429300 BEGIN
/* 429300-429308 00008 */
uchar CDSFileStream::FUN_00429300(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00429300 END

// !FUNC 0x004333e0 BEGIN
/* 4333E0-4333FB 0001B */
void CDSFileStream::CloseFileHandle(int param_1) { STUB_BODY(); }
// !FUNC 0x004333e0 END

// !FUNC 0x00433400 BEGIN
/* 433400-433461 00061 */
/* IDSStream slot 4 -- ReadBytes(buf, count).
 *
 * Reads up to `count` bytes from the file HANDLE at +0x10 of the
 * IDSStream subobject.  Zero-length short-circuits (no kernel32 call).
 *
 *   ReadFile failure       -> RaiseStreamException(errno=1, ctx)
 *   Short read (got < req) -> ThrowStreamErrorNoReturn(errno=1, ctx,
 *                                                     ERROR_HANDLE_EOF=0x26)
 */
void CDSFileStream::ReadBytes(void* param_1, DWORD param_2) {
    DWORD nBytesToRead = param_2;
    DWORD nBytesRead = 0;
    if (param_2 != 0) {
        BOOL ok = ReadFile(FileHandle(this), param_1, nBytesToRead, &nBytesRead, NULL);
        if (ok == 0) {
            /* RaiseStreamException(1, SelfOrNull(this)) -- TODO: helper is a
             * non-static _Globals member in the current source layout; the
             * binary expects a __cdecl free function. Wire up once the
             * _Globals helpers are promoted out of the class. */
            reinterpret_cast<_Globals*>(SelfOrNull(this))->RaiseStreamException(1, SelfOrNull(this));
        }
        if (nBytesToRead != nBytesRead) {
            /* ThrowStreamErrorNoReturn(1, SelfOrNull(this), 0x26) */
            reinterpret_cast<_Globals*>(SelfOrNull(this))->ThrowStreamErrorNoReturn(1, SelfOrNull(this), 0x26);
        }
    }
}
// !FUNC 0x00433400 END

// !FUNC 0x00433470 BEGIN
/* 433470-4334BE 0004E */
/* IDSStream slot 5 -- WriteBytes(buf, count).
 *
 * Writes `count` bytes to the file HANDLE at +0x10.  Zero-length is a
 * no-op.  Any WriteFile failure OR short write raises
 * RaiseStreamException(errno=2, ctx).
 */
void CDSFileStream::WriteBytes(void* param_1, DWORD param_2) {
    if (param_2 != 0) {
        DWORD nBytesToWrite = param_2;
        /* MSVC8 reuses param_2's incoming stack slot as nBytesWritten:
         * writing through &param_2 makes the slot escape, and the explicit
         * `param_2 = 0` right before the call matches the target's
         * `mov [esp+0x24], 0` placed AFTER all WriteFile args are pushed. */
        param_2 = 0;
        BOOL ok = WriteFile(FileHandle(this), param_1, nBytesToWrite, &param_2, NULL);
        if (ok == 0 || param_2 != nBytesToWrite) {
            /* RaiseStreamException(2, SelfOrNull(this)) */
            reinterpret_cast<_Globals*>(SelfOrNull(this))->RaiseStreamException(2, SelfOrNull(this));
        }
    }
}
// !FUNC 0x00433470 END

// !FUNC 0x004334c0 BEGIN
/* 4334C0-433507 00047 */
/* IDSStream slot 11 -- LockRegion(offLo, offHi, lenLo, lenHi).
 *
 * Direct passthrough to kernel32 LockFile.
 *   Failure -> RaiseStreamException(errno=5, ctx)
 */
void CDSFileStream::LockRegion(DWORD param_1, DWORD param_2, DWORD param_3, DWORD param_4) {
    BOOL ok = LockFile(FileHandle(this), param_1, param_2, param_3, param_4);
    if (ok == 0) {
        /* RaiseStreamException(5, SelfOrNull(this)) */
            reinterpret_cast<_Globals*>(SelfOrNull(this))->RaiseStreamException(5, SelfOrNull(this));
        }
}
// !FUNC 0x004334c0 END

// !FUNC 0x00433510 BEGIN
/* 433510-433557 00047 */
/* IDSStream slot 12 -- UnlockRegion(offLo, offHi, lenLo, lenHi).
 *
 * Direct passthrough to kernel32 UnlockFile.
 *   Failure -> RaiseStreamException(errno=6, ctx)
 */
void CDSFileStream::UnlockRegion(DWORD param_1, DWORD param_2, DWORD param_3, DWORD param_4) {
    BOOL ok = UnlockFile(FileHandle(this), param_1, param_2, param_3, param_4);
    if (ok == 0) {
        /* RaiseStreamException(6, SelfOrNull(this)) */
        reinterpret_cast<_Globals*>(SelfOrNull(this))->RaiseStreamException(6, SelfOrNull(this));
    }
}
// !FUNC 0x00433510 END

// !FUNC 0x00433560 BEGIN
/* 433560-4335D3 00073 */
/* IDSStream slot 10 -- SeekPosition(offsetLo, offsetHi, origin).
 *
 * Maps `origin` (0=BEGIN, 1=CURRENT, 2=END) onto the matching FILE_*
 * values for SetFilePointer.  The high DWORD of the offset is passed
 * via a stack temporary (PLONG out-param of SetFilePointer).
 *   -1 return && GetLastError != 0 -> ThrowStreamErrorNoReturn(errno=3,
 *                                                              ctx, win32err)
 */
void CDSFileStream::SeekPosition(long param_1, long param_2, DWORD param_3) {
    DWORD method;
    if (param_3 == 0)      method = 0; /* FILE_BEGIN   */
    else if (param_3 == 1) method = 1; /* FILE_CURRENT */
    else if (param_3 == 2) method = 2; /* FILE_END     */
    else                    method = param_3;

    DWORD highTmp = static_cast<DWORD>(param_2);
    DWORD lowRet = SetFilePointer(FileHandle(this),
                                  static_cast<long>(param_1),
                                  reinterpret_cast<PLONG>(&highTmp),
                                  method);
    if (lowRet == 0xffffffff) {
        DWORD win32Err = GetLastError();
        if (win32Err != 0) {
            /* ThrowStreamErrorNoReturn(3, SelfOrNull(this), win32Err) */
            reinterpret_cast<_Globals*>(SelfOrNull(this))->ThrowStreamErrorNoReturn(3, SelfOrNull(this), win32Err);
        }
    }
}
// !FUNC 0x00433560 END

// !FUNC 0x004335e0 BEGIN
/* 4335E0-433658 00078 */
/* IDSStream slot 9 -- SetStreamSize(sizeLo, sizeHi).
 *
 * 1. Tell()             (vtable +0x20, captures current position).
 * 2. If size != UINT64_MAX, Seek(size, BEGIN)  (vtable +0x28).
 * 3. SetEndOfFile(handle) -- failure -> RaiseStreamException(errno=4).
 * 4. If size != UINT64_MAX, Seek(savedPos)     (vtable +0x28).
 *
 * NB: the second Seek receives `savedPos` as a 64-bit value (low+high
 * registers).  Ghidra renders it as a single `uVar2` so we replicate
 * with a `longlong` and pass low+high via the vtable.  We keep the
 * Tell/Seek calls as raw vtable indirects to match the original.
 */
void CDSFileStream::SetStreamSize(uint param_1, uint param_2) {
    /* `*(int*)this` = primary vtable pointer; offsets +0x20 / +0x28
     * are IDSStream slots Tell() and Seek() respectively. */
    typedef longlong (__thiscall *TellFn)(void*);
    typedef void     (__thiscall *SeekFn)(void*, longlong, DWORD);

    void** vt = *reinterpret_cast<void***>(this);
    TellFn pTell = reinterpret_cast<TellFn>(vt[8]);  /* +0x20 / 4 = slot 8 */
    SeekFn pSeek = reinterpret_cast<SeekFn>(vt[10]); /* +0x28 / 4 = slot 10 */

    longlong savedPos = pTell(this);
    if ((param_1 & param_2) != 0xffffffffu) {
        longlong target = (static_cast<longlong>(param_2) << 32) | static_cast<ulonglong>(param_1);
        pSeek(this, target, 0 /* FILE_BEGIN */);
    }
    BOOL ok = SetEndOfFile(FileHandle(this));
    if (ok == 0) {
        /* RaiseStreamException(4, SelfOrNull(this)) */
        reinterpret_cast<_Globals*>(SelfOrNull(this))->RaiseStreamException(4, SelfOrNull(this));
    }
    if ((param_1 & param_2) != 0xffffffffu) {
        pSeek(this, savedPos, 0 /* FILE_BEGIN */);
    }
}
// !FUNC 0x004335e0 END

// !FUNC 0x00433660 BEGIN
/* 433660-4336B1 00051 */
/* IDSStream slot 7 -- GetSize : longlong.
 *
 * GetFileSize returns the low DWORD as its value and writes the high
 * DWORD via the LPDWORD out-param.
 *
 *   low == 0xFFFFFFFF && GetLastError != 0 ->
 *       ThrowStreamErrorNoReturn(errno=4, ctx, win32err)
 *
 * Result = (high << 32) | low.  Ghidra renders the 64-bit assemble
 * as `__allmul(local, 0, 0, 1)` because MSVC lowers `(longlong)low + ((longlong)high << 32)`
 * through the CRT helper on x86 -- it's just `(high:low)`.
 */
longlong CDSFileStream::GetSize() {
    DWORD high = 0;
    DWORD low = GetFileSize(FileHandle(this), &high);
    if (low == 0xffffffffu) {
        DWORD win32Err = GetLastError();
        if (win32Err != 0) {
            /* ThrowStreamErrorNoReturn(4, SelfOrNull(this), win32Err) */
            reinterpret_cast<_Globals*>(SelfOrNull(this))->ThrowStreamErrorNoReturn(4, SelfOrNull(this), win32Err);
        }
    }
    return (static_cast<longlong>(high) << 32) | static_cast<ulonglong>(low);
}
// !FUNC 0x00433660 END

// !FUNC 0x004336c0 BEGIN
/* 4336C0-43371D 0005D */
/* IDSStream slot 8 -- TellPosition : longlong.
 *
 * Calls SetFilePointer(handle, 0, &high, FILE_CURRENT).  The return is
 * the low DWORD; the high DWORD is written into the out-param.
 *
 *   ret == 0xFFFFFFFF && GetLastError != 0 ->
 *       ThrowStreamErrorNoReturn(errno=3, ctx, win32err)
 *
 * Ghidra renders the 64-bit reassembly as
 *   `__allmul(low, low>>31, 0, 1)`
 * which is just `((longlong)(int)low) << 32` -- a sign-extended shift.
 * That's identical to `(high:low)` since `high` holds the same value
 * after the SetFilePointer call.
 */
longlong CDSFileStream::TellPosition() {
    DWORD high = 0;
    DWORD low = SetFilePointer(FileHandle(this), 0,
                               reinterpret_cast<PLONG>(&high),
                               1 /* FILE_CURRENT */);
    if (low == 0xffffffffu) {
        DWORD win32Err = GetLastError();
        if (win32Err != 0) {
            /* ThrowStreamErrorNoReturn(3, SelfOrNull(this), win32Err) */
            reinterpret_cast<_Globals*>(SelfOrNull(this))->ThrowStreamErrorNoReturn(3, SelfOrNull(this), win32Err);
        }
    }
    return (static_cast<longlong>(high) << 32) | static_cast<ulonglong>(low);
}
// !FUNC 0x004336c0 END

// !FUNC 0x00433720 BEGIN
/* 433720-433747 00027 */
/* IDSStream slot 6 -- FlushStream.
 *
 * Calls FlushFileBuffers(handle).  Failure ->
 * RaiseStreamException(errno=7, ctx).
 */
void CDSFileStream::FlushStream() {
    BOOL ok = FlushFileBuffers(FileHandle(this));
    if (ok == 0) {
        /* RaiseStreamException(7, SelfOrNull(this)) */
        reinterpret_cast<_Globals*>(SelfOrNull(this))->RaiseStreamException(7, SelfOrNull(this));
    }
}
// !FUNC 0x00433720 END

// !FUNC 0x00433750 BEGIN
/* 433750-433877 00127 */
uchar CDSFileStream::FUN_00433750(int param_1, DWORD param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00433750 END

// !FUNC 0x004338d0 BEGIN
/* 4338D0-433940 00070 */
uchar CDSFileStream::FUN_004338d0(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004338d0 END

