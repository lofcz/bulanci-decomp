# DirectX `.gdt` data-type archive

`directx_feb2007.gdt` contains every public C struct / union / enum /
typedef pulled out of the Microsoft DirectX SDK (February 2007)
headers (`ddraw.h`, `dsound.h`, `dinput.h`) plus the bits of the
Platform SDK they transitively rely on (`windows.h`, `objbase.h`,
`mmsystem.h`, ...).

It exists so that Ghidra's decompiler can render the game's COM-style
DirectX call sites as

```c
pDD->lpVtbl->CreateSurface(pDD, &desc, &surface, NULL);
```

rather than the impenetrable

```c
(**(code **)(*(code *)pDD + 0x24))(pDD, &desc, &surface, 0);
```

## Rebuild

The archive is built deterministically from the vendored SDK + MSVC
toolchain. Two commands from a clean clone:

```pwsh
python scripts\build_dx_gdt.py        # cl /E -> cleanup -> Ghidra CParser -> .gdt
python scripts\apply_dx_gdt.py        # resolve types into bulanci.exe's DTM
```

`build_dx_gdt.py` is phased - `--phase {preprocess,clean,parse,verify,all}` -
so iterating on cleanup regexes doesn't require re-running cl.exe each
time.

## What's in here

Approx. 2200 composites, 150 enums, 4900 typedefs, ~2.2 MB on disk.
Canary types covered by the verify pass:

- `IDirectDraw7Vtbl`, `IDirectDrawSurface7Vtbl`, ...
- `IDirectSoundVtbl`, `IDirectSoundBuffer8Vtbl`
- `IDirectInput8AVtbl`, `IDirectInputDevice8AVtbl`
- `WAVEFORMATEX`, `DDSURFACEDESC2`, `DSBUFFERDESC`

## Why commit the .gdt?

It's a one-shot derivative of the vendored SDK that takes 10-15
seconds to rebuild and tracks the SDK version exactly. Committing it
saves every fresh clone that build step, and lets reviewers see when
the underlying types change (the .gdt is a binary archive but its
size moves visibly with edits).

## Why not generate everything via `cl /E /TC`?

We do! But the bundled Ghidra C parser is strict about MSVC keywords
(`__declspec`, `__forceinline`, `__asm`, inline function bodies, ...)
that the preprocessor doesn't expand. `scripts/build_dx_gdt.py`'s
`clean` phase strips those before handing the flattened `.i` to
Ghidra's `CParserUtils.parseHeaderFiles` via
`scripts/ghidra/ParseHeadersToGdt.java`.
