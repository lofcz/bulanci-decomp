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
# allows us to infer them. /O2 + /GR + /GX (now /EHsc on modern MSVC) is a
# reasonable baseline for matching VC8 commercial code. Refine per-unit later.
DEFAULT_CL_FLAGS = "/Zi /O2 /GR /GX"


def generateNinja(decompUnits: list[DecompUnit], output_path: Path):
    with (output_path / "build.ninja").open("w") as f:
        writer = Writer(f, width=140)
        writer.variable("ninja_required_version", "1.5")

        for decompUnit in decompUnits:
            writer.variable(f"builddir_{decompUnit.directory_name}", str(decompUnit.buildSrc))

        writer.variable("cl", DEFAULT_CL)
        unitsImports = " ".join([f"/I {str(decUnit.includePath)}" for decUnit in decompUnits])
        writer.variable(
            "cl_flags",
            f"{DEFAULT_CL_FLAGS} /I include/ {unitsImports} /I tools/",
        )
        writer.rule("cc", "$cl /nologo $cl_flags /c $in /Fd$out.pdb /Fo$out", deps="msvc")

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
