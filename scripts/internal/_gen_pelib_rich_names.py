"""One-shot: regenerate pelib_rich_product_names.py from pelib RichHeader.cpp excerpt."""
import re
from pathlib import Path

SRC = Path(__file__).resolve().parents[2] / (
    ".cursor/projects/c-Users-mstagl-dev-Documents-GitHub-bulanci/"
    "agent-tools/da8ca73d-bbee-4615-8df1-d18c19f5d865.txt"
)
# Fallback: fetch path used when run from repo via copied excerpt
ALT = Path(
    r"C:\Users\mstagl-dev\.cursor\projects\c-Users-mstagl-dev-Documents-GitHub-bulanci"
    r"\agent-tools\da8ca73d-bbee-4615-8df1-d18c19f5d865.txt"
)

def main() -> None:
    src = SRC if SRC.is_file() else ALT
    text = src.read_text(encoding="utf-8", errors="replace")
    names: list[str] = []
    for line in text.splitlines():
        m = re.search(r'"([^"]+)".*//\s*0x([0-9A-Fa-f]+)', line)
        if m:
            names.append(m.group(1).strip())
    out = Path(__file__).resolve().parent / "pelib_rich_product_names.py"
    body = "".join(f"    {n!r},\n" for n in names)
    out.write_text(
        "# Synced from avast/pelib src/pelib/RichHeader.cpp productNames[]\n"
        "PELIB_PRODUCT_NAMES: tuple[str, ...] = (\n"
        + body
        + ")\n",
        encoding="utf-8",
    )
    print(f"wrote {len(names)} names -> {out}")


if __name__ == "__main__":
    main()
