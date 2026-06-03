# Round 7 — FUN Task 04 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 4 |
| **band** | sim |
| **seed_address** | `0x004298d0` |
| **prior_hint** | (none) |

## Status

**DONE** — Already renamed; verified COM + image bind path.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x004298d0` | `CDSBackBuffer_BindImageFromSurface` | `IDirectSound` buffer at `param_1+0x4C` vtable+100 (`0x6C`-byte format); on success `CDSImage_BindFromSurfaceDesc` on `param_1+4` | **Xrefs_to:** `FUN_0042a590` (×2), `CBulanci_RebuildBackBufferSurface@0x0042a1e8`. **Returns** `bool` |

## Ghidra deltas

**none** — name already applied in Ghidra.

## Frida

**none**

## Remaining UNK

- Whether `param_1` is always `CDSBackBuffer+4` subobject vs. other hosts (callers pass `int` in fastcall cluster).
