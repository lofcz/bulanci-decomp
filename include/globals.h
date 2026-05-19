#ifndef _BULANCI_GLOBALS_H
#define _BULANCI_GLOBALS_H

#define GLUE(a, b) a##b
#define STATIC_ASSERT(cond) typedef char GLUE(static_assertion_failed, __LINE__)[(cond) ? 1 : -1]

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef __int64 longlong;
typedef unsigned __int64 ulonglong;

/*
 * Curated Win32 / DirectX typedef stubs.
 *
 * `scripts/ghidra/GenerateMapping.java` preserves typedef names whose
 * base type is a primitive or a struct (`HRESULT`, `DWORD`, `BOOL`,
 * `WAVEFORMATEX`, `GUID`, ...) so they read naturally in mapping.csv
 * and the generated stubs. We pre-declare the most common ones here
 * so the stubs compile against the vendored MSVC8 + DXSDK without
 * having to pull in the full Windows headers in every translation
 * unit. Typedefs not listed here are forward-declared by the stub
 * generator as `struct Name;` - that's fine because every stub body
 * is empty and the parameter is only ever held by pointer.
 *
 * Keep this list short: each entry adds friction if you ever bring
 * in the real `windows.h`. Anything project-specific belongs in its
 * own header.
 */
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long HRESULT;
typedef char CHAR;
typedef wchar_t WCHAR;
typedef int errno_t;
typedef void *PVOID;
typedef void *LPVOID;

#define DWORD_PTR DWORD

#define NULL 0
#define TRUE 1
#define FALSE 0

#endif
