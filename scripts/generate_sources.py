"""Bootstrap stub generator.

This script emits the *initial* C++ scaffold for every unit listed in
`config/bulanci/units_listing.csv` from `config/bulanci/mapping.csv`.

Splat-style ownership rules (see `docs/DECOMP.md`):
  * `src/bulanci/<unit>.cpp` and `include/bulanci/<unit>.h` are hand-owned
    once they exist.  This script refuses to overwrite them by default.
  * On every subsequent Ghidra promotion pass, run
    `scripts/sync_units.py` instead: it diffs the per-function markers
    against the current `mapping.csv` and moves blocks between unit
    files atomically, preserving any hand-written matched bodies.
  * `--force` overrides the bootstrap check (nuclear: wipes hand-edits).

Marker format (parsed by `sync_units.py`):

    // !FUNC 0x00418c00 BEGIN
    /* 00418C00-00418C5A 0005A */
    void ODSImage::FUN_00418c00(int* param_1) { STUB_BODY(); }
    // !FUNC 0x00418c00 END

    class ODSImage {
    public:
        // !DECL 0x00418c00 BEGIN
        /* 00418C00 */ void FUN_00418c00(int* param_1);
        // !DECL 0x00418c00 END
    };

Addresses are the canonical function-start address (8-digit lowercase
hex with `0x` prefix); they're the stable identity even after Ghidra
renames a `FUN_xxx` into `CFoo::Init`.
"""

import argparse
import os
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from _claim_guard import resolve_agent, guard_units  # noqa: E402

from project import Function, FunctionType, DecompUnit

WORKSPACE_PATH = Path(__file__).parent.parent

# Types we know are safe to use as-is (primitives + things declared in
# include/globals.h). Anything else that shows up as a parameter or return
# type gets forward-declared as `struct Name;` at the top of the header so
# pointer-to-Name compiles. Empty stub bodies never dereference the pointer
# so a forward declaration is sufficient.
#
# Keep in sync with `include/globals.h` - any typedef declared there
# should be listed here so the stub generator emits the bare typedef
# name instead of a (broken) `struct Name;` forward declaration.
# Populated by ``set_project_classes()`` at the start of a generate /
# sync run with the union of every class name declared in
# ``units_listing.csv``.  ``_safe_arg_type`` / ``_safe_return_type``
# consult it so a by-value reference to a cross-unit class (e.g.
# ``CBulanci param_3`` in ``CButton.h``) survives the auto-emit pass
# instead of being demoted to ``int``.  The convention is that every
# unit's header ``#include``s the headers it needs (preserved verbatim
# by ``sync_units`` from the pre-class extras), so by-value uses of
# project classes resolve to a complete type at compile time.
_PROJECT_CLASSES: set[str] = set()


def set_project_classes(classes: set[str]) -> None:
    """Replace the project-wide class set used by ``_safe_arg_type``.

    Pass the union of every leaf class name declared in
    ``units_listing.csv`` (see ``DecompUnit.units``).  Called once per
    generate / sync invocation; module-level state is fine because the
    pipeline is single-threaded.
    """
    global _PROJECT_CLASSES
    _PROJECT_CLASSES = set(classes or ())


KNOWN_TYPES = {
    "void", "bool", "char", "uchar", "short", "ushort", "int", "uint",
    "long", "ulong", "longlong", "ulonglong", "float", "double",
    "wchar_t", "size_t", "ptrdiff_t",
    # Curated Win32 / DirectX scalar + handle typedefs from globals.h.
    "BOOL", "BYTE", "WORD", "DWORD", "DWORD_PTR", "HRESULT",
    "CHAR", "WCHAR", "errno_t",
    "PVOID", "LPVOID",
    # Pre-existing Win32 handle types kept for backwards compatibility
    # with already-generated stubs that reference them.
    "LPCRITICAL_SECTION", "HINSTANCE", "HWND",
}

# Regex of characters that may appear inside a C++ identifier (plus `:` so
# nested qualified names like `std::bad_alloc` are accepted). Used to derive
# a safe alias for unknown type names; we no longer use it to filter
# function names since we never drop functions - we sanitise instead.
_VALID_IDENT_CHARS = re.compile(r"^[A-Za-z_~][A-Za-z0-9_]*$")


def _sanitize_symbol_name(name: str) -> str:
    """Turn a Ghidra-rendered symbol name into a valid C++ identifier
    without losing information. Mirrors `ExportDelinker.java`'s
    `sanitizeSymbolName()` so the COFF symbols on both sides of the
    diff match for synthetic Ghidra names.

    Rules:
      * Operator overloads (`operator==`, `operator new[]`, ...) are
        passed through unchanged - those are legal C++ method names.
      * Otherwise every non-`[A-Za-z0-9_~]` character is replaced with
        `_` and runs of `_` are collapsed. Leading / trailing `_` are
        kept because that's how Ghidra produces some legal symbols
        (e.g. `_AtlGetThreadACPThunk`).

    Examples:
      `Catch@0041520d`               -> `Catch_0041520d`
      `` `scalar_deleting_destructor' `` -> `scalar_deleting_destructor`
      `operator==`                   -> `operator==` (unchanged)
    """
    if name.startswith("operator") and len(name) > len("operator"):
        # `operator` followed by punctuation / type name; legal in C++.
        return name
    # Allow ~ only at the start (destructor prefix).
    out_chars: list[str] = []
    for i, c in enumerate(name):
        if c.isalnum() or c == "_":
            out_chars.append(c)
        elif c == "~" and i == 0:
            out_chars.append(c)
        else:
            out_chars.append("_")
    # Collapse runs of `__` introduced by sanitisation.
    sanitised = re.sub(r"_+", "_", "".join(out_chars))
    # Strip outer underscores caused by stripped backticks/quotes but
    # keep a single leading `_` (legal C++ identifier start).
    sanitised = sanitised.strip("_")
    if name.startswith("_") and not sanitised.startswith("_"):
        sanitised = "_" + sanitised
    return sanitised or "_anon"


def _strip_pointer(ty: str) -> str:
    """Return the base type name without trailing `*` and whitespace."""
    return ty.rstrip(" *")


def _is_pointer(ty: str) -> bool:
    return ty.rstrip().endswith("*")


def _normalize_type(ty: str) -> str:
    """Smooth over Ghidra type spellings that aren't valid C++.

    - `undefined1[16]*` / `byte[N]*` / etc. -> `void*` (these only appear
      as opaque buffer pointers in our stubs).
    - `undefined`, `undefined1` (by value) -> `int` (treated as an arbitrary
      integer-sized result; we never actually use it).
    """
    if "[" in ty:
        return "void*" if _is_pointer(ty) else "int"
    if _strip_pointer(ty) in {"undefined", "undefined1", "undefined2", "undefined4", "undefined8"}:
        return "void*" if _is_pointer(ty) else "int"
    return ty


def _is_unknown_struct(ty: str) -> bool:
    """True iff `ty` (already stripped of `*`) is an unknown struct/class
    name that we forward-declare in headers.

    A project class (registered via ``set_project_classes``) is treated
    as **known**: every unit's header includes the headers it needs, so
    a by-value or by-pointer reference resolves to a complete type at
    compile time.  This keeps cross-unit prototypes (``CBulanci
    param_3`` in ``CButton_BuildAt``) intact instead of demoting them
    to ``int``.
    """
    base = _strip_pointer(ty)
    if not base or base in KNOWN_TYPES:
        return False
    if base in _PROJECT_CLASSES:
        return False
    if "::" in base:
        return False
    return bool(_VALID_IDENT_CHARS.match(base))


def _safe_return_type(ty: str, in_unit_classes: set[str] | None = None) -> str:
    """Forward-declared structs are incomplete types: legal to point to,
    illegal to return by value. For empty-body stubs we don't care about
    the precise return type, so substitute `int` whenever we'd otherwise
    emit a by-value reference to a forward-declared struct.

    If `in_unit_classes` is provided, a pointer return type whose base
    name matches a leaf class declared in the same unit is rewritten to
    `void*` for the same reason `_safe_arg_type` does: at file scope a
    `Foo* T::method()` declaration resolves `Foo` against the global
    namespace, which collides with the in-class injected class name in
    the header declaration of the same method."""
    ty = _normalize_type(ty)
    base = _strip_pointer(ty)
    if in_unit_classes and base in in_unit_classes:
        return "void*" if _is_pointer(ty) else "int"
    if _is_pointer(ty):
        return ty
    if _is_unknown_struct(ty):
        return "int"
    return ty


def _safe_arg_type(ty: str, in_unit_classes: set[str] | None = None) -> str:
    """Sanitised parameter type. Stubs never look at the actual value, so
    forward-declared struct pointers and `void*` placeholders are fine.
    For by-value unknown struct types (e.g. `_CRT_DOUBLE`) we substitute
    `int`: the stub body never touches the parameter, and an empty
    forward-declared struct would have the wrong size for calling-
    convention matching anyway.

    If `in_unit_classes` is provided, any base type whose name matches a
    leaf class declared in the same unit is rewritten to `void*` to avoid
    MSVC's global-vs-class scope shadowing (e.g. ` ::exception` forward
    decl colliding with `std::exception` inner class). The stub body
    never reads the pointer so the loss of type fidelity is harmless."""
    ty = _normalize_type(ty)
    base = _strip_pointer(ty)
    if in_unit_classes and base in in_unit_classes:
        return "void*" if _is_pointer(ty) else "int"
    if not _is_pointer(ty) and _is_unknown_struct(ty):
        return "int"
    return ty


def _is_implicit_constructor(fun: "Function") -> bool:
    """Returns True iff fun's name matches the class it's a member of.
    Ghidra occasionally marks constructors as ordinary functions, which
    leaks an illegal return type onto a constructor declaration."""
    ns = fun.namespace or ""
    leaf = ns.rsplit("::", 1)[-1] if "::" in ns else ns
    return bool(leaf) and fun.name == leaf


def _leaf_class_names(unitNamespaces) -> set[str]:
    """For each namespace in this unit, return the set of leaf class names.
    Used to detect collisions between forward-declared types and inner
    classes (e.g. forward-declared `::exception` shadows `std::exception`
    in MSVC's name lookup)."""
    leaves: set[str] = set()
    for ns in unitNamespaces:
        leaf = ns.rsplit("::", 1)[-1] if "::" in ns else ns
        if leaf:
            leaves.add(leaf)
    return leaves


def _sanitize_and_dedup(funByNamespaces, unitNamespaces) -> None:
    """In-place rename pass that mirrors `ExportDelinker.java`.

    Two transformations, applied in order:

    1. Symbol-name sanitisation: replace illegal-in-C++ characters such
       as `@` (from Ghidra's `Catch@<addr>` synthetic exception handlers)
       and stripped backtick quotes (from MSVC vftable / scalar-deleting
       destructor thunks) with `_`. See `_sanitize_symbol_name()`.

    2. Duplicate dedup: for any (namespace, sanitised-name) pair that
       has more than one entry, keep the lowest-address copy and suffix
       all others with `_<8-hex-address>`. Mirrors
       `ExportDelinker.java`'s `deduplicateSymbolNames()`.

    The result: every function in `mapping.csv` is emitted into the stub
    sources, with a name that's both valid C++ and bytewise identical to
    the symbol ExportDelinker writes into the target COFF.
    """
    for ns in unitNamespaces:
        funs = funByNamespaces.get(ns, [])
        for f in funs:
            f.name = _sanitize_symbol_name(f.name)
        groups: dict[str, list[Function]] = {}
        for f in funs:
            groups.setdefault(f.name, []).append(f)
        for name, dups in groups.items():
            if len(dups) <= 1:
                continue
            dups.sort(key=lambda f: f.start)
            for f in dups[1:]:
                f.name = f"{name}_{f.start:08x}"


def _collect_unknown_types(funByNamespaces, unitNamespaces) -> list[str]:
    """Walk every function in the unit and return unknown base type names
    (sorted, de-duplicated). Used for forward declaration emission."""
    seen: set[str] = set()
    for ns in unitNamespaces:
        for fun in funByNamespaces.get(ns, []):
            for ty in [fun.returnType] + list(fun.args):
                ty_norm = _normalize_type(ty)
                if not _is_pointer(ty_norm):
                    # by-value uses are either KNOWN_TYPES, `int`, or
                    # rewritten by _safe_return_type to `int` - no forward
                    # declaration required.
                    continue
                base = _strip_pointer(ty_norm)
                if base in KNOWN_TYPES or base == "void":
                    continue
                if base in _PROJECT_CLASSES:
                    # Definition lives in another unit's header, which
                    # is included via the preserved pre-class extras
                    # (`#include "Foo.h"`).  Emitting a `struct Foo;`
                    # forward decl alongside the include is harmless
                    # but noisy; skip it.
                    continue
                if "::" in base:
                    continue
                if not _VALID_IDENT_CHARS.match(base):
                    continue
                seen.add(base)
    return sorted(seen)


def generateSources(
    decompUnit: DecompUnit,
    force: bool = False,
    *,
    agent: str | None = None,
    allow_unclaimed: bool = True,
):
    """Scaffold any missing unit files.  Existing files are left alone
    unless `force=True` (in which case they're overwritten - destructive
    to hand-edits).  Future namespace shifts should go through
    `scripts/sync_units.py`.

    ``agent`` enables the same per-claim filter used by ``sync_units``
    and ``rename_matched_bodies``.  By default new units are allowed
    even without a claim (``allow_unclaimed=True``) because
    scaffolding a brand-new unit is intrinsically work nobody else can
    have claimed yet -- but ``--force`` rebuilds need a real claim.
    """
    os.makedirs(str(decompUnit.srcPath), exist_ok=True)
    os.makedirs(str(decompUnit.includePath), exist_ok=True)

    # Publish the project-wide leaf class set so by-value cross-unit
    # references survive auto-emission.  See ``set_project_classes``.
    project_classes: set[str] = set()
    for namespaces in decompUnit.units.values():
        for ns in namespaces:
            for part in ns.split("::"):
                if part:
                    project_classes.add(part)
    set_project_classes(project_classes)

    skipped = 0
    scaffolded = 0
    candidate = list(decompUnit.units.items())
    # ``--force`` is the destructive path; require an explicit claim
    # for those units even though scaffolding new units is usually
    # safe.
    enforce_unclaimed = allow_unclaimed and not force
    permitted = set(
        guard_units(
            agent,
            [u for u, _ in candidate],
            action="generate_sources",
            allow_unclaimed=enforce_unclaimed,
        )
    )
    units_skipped_by_claim = 0
    for unit, namespaces in candidate:
        if agent and unit not in permitted:
            units_skipped_by_claim += 1
            continue
        if generateUnitSources(decompUnit.mappings, unit, namespaces,
                               decompUnit.srcPath, decompUnit.includePath,
                               force=force):
            scaffolded += 1
        else:
            skipped += 1
    print(f"  scaffolded {scaffolded} unit(s); preserved {skipped} existing unit(s).")
    if units_skipped_by_claim:
        print(f"  ~ skipped {units_skipped_by_claim} unit(s) not claimed by {agent}")
    if skipped and not force:
        print("  (existing units are hand-owned; run scripts/sync_units.py to "
              "migrate per-function blocks, or pass --force to rebuild from scratch.)")


def generateUnitSources(funByNamespaces, unitName, unitNamespaces, source_dir, include_dir, force: bool = False) -> bool:
    """Returns True when at least one file (header or source) was
    (re)written.  Both files have identical lifecycle semantics: if
    either one already exists and `force` is False, both are preserved
    to avoid drifting headers from their .cpp."""
    # Sanitise + dedup names BEFORE either header or source is emitted so
    # both files agree on the suffixed/sanitised name and the resulting
    # COFF symbols line up with what ExportDelinker writes for the target.
    _sanitize_and_dedup(funByNamespaces, unitNamespaces)

    header_file = include_dir / (unitName + ".h")
    source_file = source_dir / (unitName + '.cpp')

    if not force and (header_file.exists() or source_file.exists()):
        # Hand-owned: leave both alone.  sync_units.py handles per-function
        # migration in place; this generator only bootstraps new units.
        return False

    with header_file.open("w", encoding="utf-8", newline="") as f:
        generateUnitHeader(f, funByNamespaces, unitName, unitNamespaces)
    with source_file.open("w", encoding="utf-8", newline="") as f:
        generateUnitSource(f, funByNamespaces, unitName, unitNamespaces)
    return True


# --- Marker emission ------------------------------------------------------
#
# All migration tooling keys off these exact strings.  The format is
# `// !FUNC 0xXXXXXXXX BEGIN` / `// !DECL 0xXXXXXXXX BEGIN` with 8-digit
# lowercase hex.  `sync_units.py` uses the same regex; keep in sync.

def _func_begin(addr: int) -> str:
    return f"// !FUNC 0x{addr:08x} BEGIN\n"


def _func_end(addr: int) -> str:
    return f"// !FUNC 0x{addr:08x} END\n"


def _decl_begin(addr: int) -> str:
    return f"\t// !DECL 0x{addr:08x} BEGIN\n"


def _decl_end(addr: int) -> str:
    return f"\t// !DECL 0x{addr:08x} END\n"


def _classify_namespaces(unitNamespaces):
    """Decide which strings are C++ `namespace` scopes vs classes.

    A namespace string `X::Y::Z` implies that `X` and `X::Y` are real C++
    namespaces (or classes wrapping further nested types). We currently
    only support a single level of `namespace` wrapping (good enough for
    `std::bad_alloc`, `std::exception`), so the top-most segment becomes
    the namespace and the rest is treated as a class.

    Returns:
      ns_set: top-level C++ namespace names (zero or one for a typical unit).
      class_set: fully-qualified class names that should be emitted with
        the `class` keyword (possibly inside a namespace).
    """
    all_ns = set(unitNamespaces)
    ns_set: set[str] = set()
    class_set: set[str] = set()
    for n in all_ns:
        if "::" in n:
            top = n.split("::", 1)[0]
            ns_set.add(top)
            class_set.add(n)
        else:
            # No nesting -> class at top level (unless something else marks
            # it as a namespace).
            class_set.add(n)
    # If a flat name has children, promote it to a namespace.
    for n in list(class_set):
        if n in ns_set:
            class_set.discard(n)
    return ns_set, class_set


def generateUnitHeader(headerFile, funByNamespaces, unitName, unitNamespaces):
    all_huge_class_name = make_all_huge_name(unitName)
    headerFile.write(f"#ifndef {all_huge_class_name}\n")
    headerFile.write(f"#define {all_huge_class_name}\n\n")
    headerFile.write("#include <globals.h>\n\n")

    leaves = _leaf_class_names(unitNamespaces)
    unknown_types = [
        t for t in _collect_unknown_types(funByNamespaces, unitNamespaces)
        # Skip forward-decls that would collide with an in-unit class
        # (e.g. `struct exception;` next to `class std { class exception {};};`).
        # _safe_arg_type below rewrites such references to `void*`.
        if t not in leaves
    ]
    if unknown_types:
        headerFile.write(
            "/* Forward declarations of types referenced by parameter / return\n"
            " * positions but not (yet) defined in this project. Stubs only use\n"
            " * pointers so a struct forward-decl is enough. */\n"
        )
        for t in unknown_types:
            headerFile.write(f"struct {t};\n")
        headerFile.write("\n")

    ns_set, class_set = _classify_namespaces(unitNamespaces)

    # Emit classes whose name has no `::` first (plain unit-level classes).
    plain_classes = sorted(c for c in class_set if "::" not in c)
    for ns in plain_classes:
        generateUnitHeaderClass(headerFile, funByNamespaces, ns, ns, leaves)

    # Then emit namespace blocks, wrapping their nested classes inside.
    plain_namespaces = sorted(n for n in ns_set if "::" not in n)
    for top_ns in plain_namespaces:
        headerFile.write(f"namespace {top_ns} {{\n\n")
        for sub in sorted(class_set):
            if sub.startswith(top_ns + "::"):
                leaf = sub.split("::")[-1]
                generateUnitHeaderClass(headerFile, funByNamespaces, sub, leaf, leaves)
        headerFile.write(f"}} // namespace {top_ns}\n\n")

    headerFile.write("#endif")


def make_all_huge_name(name: str):
    result = ""
    for c in name:
        if c >= 'A' and c <= 'Z':
            result += '_'
        result += c
    return result.upper()


def generateUnitHeaderClass(headerFile, funByNamespaces, ns, classDisplayName, leaves: set[str]):
    """Emit `class <classDisplayName> { ... };` collecting all functions whose
    Ghidra namespace == `ns`. `classDisplayName` is the unqualified name to
    use (e.g. `bad_alloc` when wrapped inside `namespace std`)."""
    headerFile.write(f"class {classDisplayName} {{\n")
    if ns in funByNamespaces:
        nsFunctions = funByNamespaces[ns]
        headerFile.write("public:\n")
        for fun in nsFunctions:
            # _sanitize_and_dedup has already rewritten any non-identifier
            # characters in fun.name so emission is unconditional here. We
            # never drop a function: every entry in mapping.csv should
            # surface in the stub so total_code and the function count
            # stay in lockstep with what ExportDelinker writes.
            generateUnitHeaderFunction(headerFile, fun, leaves)
    headerFile.write("};\n\n")


def render_decl_line(fun: Function, leaves: set[str]) -> str:
    """Return the single-line declaration body that sits between
    `// !DECL <addr> BEGIN` and `// !DECL <addr> END`.  Shared between
    the bootstrap generator and `sync_units.py` so a re-synced block
    matches a fresh scaffold byte-for-byte."""
    parts: list[str] = [f"\t/* {fun.start:X} */ "]
    if fun.type is FunctionType.STATIC:
        parts.append("static ")
    if fun.type is not FunctionType.CONSTRUCTOR and not _is_implicit_constructor(fun):
        parts.append(f"{_safe_return_type(fun.returnType, leaves)} ")
    parts.append(f"{fun.name}(")
    parts.append(", ".join(
        f"{_safe_arg_type(arg, leaves)} param_{param_idx+1}"
        for param_idx, arg in enumerate(fun.args)))
    if fun.hasVarArgs:
        if len(fun.args) > 0:
            parts.append(", ")
        parts.append("...")
    parts.append(");\n")
    return "".join(parts)


def generateUnitHeaderFunction(headerFile, fun: Function, leaves: set[str]):
    headerFile.write(_decl_begin(fun.start))
    headerFile.write(render_decl_line(fun, leaves))
    headerFile.write(_decl_end(fun.start))


def generateUnitSource(source_file, funByNamespaces, unitName, unitNamespaces):
    source_file.write(f"#include \"{unitName}.h\"\n\n")
    leaves = _leaf_class_names(unitNamespaces)
    for ns in unitNamespaces:
        if ns in funByNamespaces:
            for fun in funByNamespaces[ns]:
                generateUnitSourceFunction(source_file, fun, leaves)


def render_func_body(fun: Function, leaves: set[str]) -> str:
    """Return the full function block body (signature + STUB body) that
    sits between `// !FUNC <addr> BEGIN` and `// !FUNC <addr> END`.
    Shared between the bootstrap generator and `sync_units.py`."""
    is_ctor_like = fun.type is FunctionType.CONSTRUCTOR or _is_implicit_constructor(fun)
    safe_ret = _safe_return_type(fun.returnType, leaves)
    parts: list[str] = [f"/* {fun.start:X}-{fun.end:X} {fun.size:05X} */\n"]
    if not is_ctor_like:
        parts.append(f"{safe_ret} ")
    parts.append(f"{fun.namespace}::{fun.name}(")
    parts.append(", ".join(
        f"{_safe_arg_type(arg, leaves)} param_{param_idx+1}"
        for param_idx, arg in enumerate(fun.args)))
    if fun.hasVarArgs:
        if len(fun.args) > 0:
            parts.append(", ")
        parts.append("...")
    parts.append(") { STUB_BODY();")
    if safe_ret != 'void' and not is_ctor_like:
        parts.append(" return 0;")
    parts.append(" }\n")
    return "".join(parts)


def generateUnitSourceFunction(source_file, fun: Function, leaves: set[str]):
    source_file.write(_func_begin(fun.start))
    source_file.write(render_func_body(fun, leaves))
    source_file.write(_func_end(fun.start))
    source_file.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--force", action="store_true",
                        help="Overwrite existing unit files even if they were hand-edited. "
                             "Use only for one-time migrations (current PR moved to markers) "
                             "or to wipe & re-scaffold from a clean slate. Prefer "
                             "`scripts/sync_units.py` for ongoing namespace shifts.")
    parser.add_argument(
        "--agent",
        default=None,
        help=(
            "Restrict scaffolding to units claimed by this agent id "
            "(see scripts/agent_coord.py).  Defaults to $BULANCI_AGENT. "
            "Leave unset on the refresher / main checkout."
        ),
    )
    parser.add_argument(
        "--strict-claims",
        dest="allow_unclaimed",
        action="store_false",
        default=True,
        help=(
            "When --agent is set, also require a live claim for "
            "previously-unscaffolded units (default: allow scaffolding "
            "new units without a claim; only --force needs one)."
        ),
    )
    args = parser.parse_args()
    os.chdir(WORKSPACE_PATH)
    unit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    agent = resolve_agent(args.agent)
    if agent:
        print(f"generate_sources: enforcing claims for agent={agent}")
    generateSources(
        unit,
        force=args.force,
        agent=agent,
        allow_unclaimed=args.allow_unclaimed,
    )
