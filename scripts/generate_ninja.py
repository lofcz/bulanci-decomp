from ninja_syntax import Writer
from pathlib import Path
import os
from helpers import has_functions
from project import DecompUnit

WORKSPACE_PATH = Path(__file__).parent.parent

# bulanci.exe was linked with VC8 (Visual Studio 2005 RTM, link.exe 8.00.50727).
# We vendor that toolchain under tools/msvc8/. Override with BULANCI_CL if you
# keep it elsewhere - the rest of the install must still be siblings of cl.exe.
DEFAULT_CL = os.environ.get("BULANCI_CL", r"tools\msvc8\Bin\cl.exe")

# Mirrors the original game's optimisation flags as far as Rich Header analysis
# allows us to infer them. /EHsc is the post-VS2005 spelling of /GX; we use it
# to avoid the D9035 deprecation warning. /GR keeps RTTI on (the binary uses
# it heavily). Refine per-unit later when we start hitting real matches.
DEFAULT_CL_FLAGS = "/Zi /O2 /GR /EHsc"


def generateNinja(decompUnits: list[DecompUnit], output_path: Path):
    with (output_path / "build.ninja").open("w") as f:
        writer = Writer(f, width=140)
        writer.variable("ninja_required_version", "1.5")

        for decompUnit in decompUnits:
            writer.variable(f"builddir_{decompUnit.directory_name}", str(decompUnit.buildSrc))

        writer.variable("cl", DEFAULT_CL)
        # The vendored compiler's CRT/C++ headers live in tools/msvc8/Include
        # and the Windows user-mode SDK lives next to it under
        # tools/msvc8/PlatformSDK/Include (windows.h, winuser.h, ...).
        # If the user pointed BULANCI_CL elsewhere we derive both from cl's
        # parent so things stay consistent.
        cl_dir = Path(DEFAULT_CL).resolve().parent if Path(DEFAULT_CL).exists() else Path("tools/msvc8/Bin")
        msvc_include = cl_dir.parent / "Include"
        psdk_include = cl_dir.parent / "PlatformSDK" / "Include"
        unitsImports = " ".join([f"/I {str(decUnit.includePath)}" for decUnit in decompUnits])
        include_flags = f"/I include/ {unitsImports} /I {msvc_include}"
        if psdk_include.exists():
            include_flags += f" /I {psdk_include}"
        writer.variable(
            "cl_flags",
            f"{DEFAULT_CL_FLAGS} {include_flags} /I tools/",
        )
        # Compile + immediately demangle the COFF symbol table so the
        # resulting .obj's symbols match what ExportDelinker writes into
        # the target objects (`CDSApp::FUN_xxxx` rather than the MSVC-
        # mangled `?FUN_xxxx@CDSApp@@QAEXXZ` cl.exe emits).  Pairing in
        # `objdiff-cli report generate` is by exact raw-symbol equality,
        # not by demangling, so we rewrite the names ourselves.
        # See `scripts/internal/compile_unit.py` (the wrapper) and
        # `scripts/internal/demangle_obj_symbols.py` (the rewriter).
        writer.rule(
            "cc",
            "python scripts\\internal\\compile_unit.py --out $out "
            "-- $cl /nologo $cl_flags /c $in /Fd$out.pdb /Fo$out",
            deps="msvc",
        )

        any_build = False
        for decompUnit in decompUnits:
            for unit, namespaces in decompUnit.units.items():
                if not has_functions(namespaces, decompUnit.mappings):
                    continue
                src = decompUnit.srcPath / (unit + ".cpp")
                hdr = decompUnit.includePath / (unit + ".h")
                # Only emit a build edge for units whose stub source already exists -
                # this keeps `ninja` happy in first-run mode where generate_sources.py
                # has not been run yet.
                if not src.exists():
                    continue
                writer.build(
                    f"$builddir_{decompUnit.directory_name}/" + unit + ".obj",
                    "cc",
                    [str(src)],
                    implicit=[str(hdr)] if hdr.exists() else None,
                )
                any_build = True

        if not any_build:
            writer.comment("No buildable units yet - run scripts/generate_sources.py first.")


if __name__ == "__main__":
    os.chdir(WORKSPACE_PATH)
    unit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    generateNinja([unit], WORKSPACE_PATH)
