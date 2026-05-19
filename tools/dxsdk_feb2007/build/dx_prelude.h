/*
 * dx_prelude.h - C-parser-safe stubs for MSVC-specific keywords / SAL
 * annotations / DECLSPEC attributes that the DirectX headers (and the
 * Windows SDK transitively pulled in by them) rely on.
 *
 * Force-included into the wrapper .c via `cl.exe /FI dx_prelude.h`.
 *
 * Goals:
 *   - Define MSVC-only macros that aren't keywords (SAL annotations,
 *     DECLSPEC_NOTHROW, INTERFACE-related helpers) to nothing, so the
 *     `cl /E` output is purely standard-C-shaped.
 *   - Keep real *keywords* (`__declspec`, `__forceinline`, ...) alone
 *     here; we strip those in a post-process regex pass because cl.exe
 *     will not honour `/D` for keywords.
 *
 * Owners: scripts/build_dx_gdt.py
 */
#ifndef DX_PRELUDE_H
#define DX_PRELUDE_H

/* -----------------------------------------------------------------
 * Force C-compilation of the (C++ flavoured) DirectX interface decls.
 * COM_NO_WINDOWS_H trims out a chunk of objbase.h we don't need.
 * CINTERFACE asks the DX headers to expand the C-style vtable structs
 * rather than C++ interface classes, which is exactly what Ghidra
 * needs to render `pDD->lpVtbl->CreateSurface(...)` calls.
 * ------------------------------------------------------------- */
#define CINTERFACE 1
#define COBJMACROS 1
#define NONAMELESSUNION 1
/* WIN32_LEAN_AND_MEAN is intentionally NOT defined - dsound.h's
 * WAVEFORMATEX comes from mmsystem.h which windows.h only pulls in
 * when the lean macro is absent. */

/* -----------------------------------------------------------------
 * SAL 2.0 annotations - already expanded by sal.h to nothing in
 * release builds, but we redefine to be 100% sure.  These show up as
 * bare identifiers in the cl /E output otherwise.
 * ------------------------------------------------------------- */
#define _In_
#define _In_opt_
#define _In_z_
#define _In_opt_z_
#define _In_reads_(s)
#define _In_reads_opt_(s)
#define _In_reads_bytes_(s)
#define _In_reads_bytes_opt_(s)
#define _Out_
#define _Out_opt_
#define _Out_writes_(s)
#define _Out_writes_opt_(s)
#define _Out_writes_bytes_(s)
#define _Out_writes_bytes_opt_(s)
#define _Inout_
#define _Inout_opt_
#define _Inout_z_
#define _Inout_updates_(s)
#define _Inout_updates_opt_(s)
#define _Inout_updates_bytes_(s)
#define _Inout_updates_bytes_opt_(s)
#define _Outptr_
#define _Outptr_opt_
#define _Outptr_result_maybenull_
#define _Outptr_opt_result_maybenull_
#define _Pre_satisfies_(c)
#define _Post_satisfies_(c)
#define _When_(c, a)
#define _Always_(a)
#define _Field_size_(s)
#define _Field_size_opt_(s)
#define _Field_size_bytes_(s)
#define _Field_size_bytes_opt_(s)
#define _Field_z_
#define _Reserved_
#define _Notnull_
#define _Maybenull_
#define _Frees_ptr_opt_

/* -----------------------------------------------------------------
 * Calling convention / linkage attribute macros frequently sprinkled
 * around. None of these change ABI for our purposes; we want them
 * gone so the parser sees plain return-type / function-pointer types.
 * ------------------------------------------------------------- */
#define DECLSPEC_IMPORT
#define DECLSPEC_EXPORT
#define DECLSPEC_NOTHROW
#define DECLSPEC_DEPRECATED
#define DECLSPEC_NOINLINE
#define DECLSPEC_SELECTANY
#define DECLSPEC_ALIGN(x)
#define DECLSPEC_UUID(x)
#define DECLSPEC_NOVTABLE
#define DECLSPEC_ALLOCATOR
#define WINAPI_INLINE inline
#define FORCEINLINE static
#define NTAPI
#define NTSYSAPI
#define APIENTRY

/* -----------------------------------------------------------------
 * Bits of the DirectX headers' interface-declaration glue.  Without
 * CINTERFACE/COBJMACROS the headers emit either a C++ class or
 * macro-heavy COBJMACROS soup.  With them we get clean vtable structs;
 * the helpers below just plug the few remaining holes.
 * ------------------------------------------------------------- */
#define EXTERN_C extern
#define STDAPI                  extern HRESULT
#define STDAPI_(t)              extern t
#define STDAPIV                 extern HRESULT
#define STDAPIV_(t)             extern t
#define STDMETHODIMP            HRESULT
#define STDMETHODIMP_(t)        t
#define PURE                    = 0

/* THIS / THIS_ are normally injected by the COM macros to refer to
 * the implicit interface-pointer parameter on C-style vtable thunks.
 * Under CINTERFACE the headers expand them to the interface pointer
 * type, which is fine; this is just a safety net. */
#ifndef THIS_
#define THIS_  void *,
#endif
#ifndef THIS
#define THIS   void *
#endif

#endif /* DX_PRELUDE_H */
