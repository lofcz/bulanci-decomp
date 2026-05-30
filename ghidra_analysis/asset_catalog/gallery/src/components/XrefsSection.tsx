import type { CatalogEntry, XrefCode, XrefScript } from "@/types";
import { SectionHeading } from "./MetaSection";

const GROUPS: ReadonlyArray<readonly [keyof CatalogEntry["xrefs"], string]> = [
  ["script",     "Script references"],
  ["insn",       "Instruction xrefs (Ghidra disassembly)"],
  ["data",       "Data xrefs"],
  ["ghidraText", "Ghidra decomp text"],
  ["idaText",    "IDA decomp text"],
  ["analysisMd", "Analysis markdown notes"],
];

const CAP = 30;

export function XrefsSection({ entry }: { entry: CatalogEntry }) {
  let total = 0;
  const blocks: React.ReactNode[] = [];
  for (const [kind, label] of GROUPS) {
    const arr = (entry.xrefs[kind] as unknown as Array<XrefCode | XrefScript> | undefined) || [];
    if (arr.length === 0) continue;
    total += arr.length;
    blocks.push(
      <XrefGroup key={String(kind)} label={label} items={arr} kind={kind as string} />,
    );
  }
  return (
    <section>
      <SectionHeading>Cross-references ({total})</SectionHeading>
      {blocks.length === 0
        ? <div className="rounded-md bg-card/40 p-3 text-xs italic text-muted-foreground">No cross-references.</div>
        : <div className="space-y-3">{blocks}</div>}
    </section>
  );
}

function XrefGroup({
  label, items, kind,
}: { label: string; items: Array<XrefCode | XrefScript>; kind: string }) {
  const shown = items.slice(0, CAP);
  const more  = items.length - shown.length;
  return (
    <div className="border-l-2 border-primary/40 pl-3">
      <h4 className="mb-1 text-[11px] font-semibold">
        {label}
        <span className="ml-2 font-normal text-muted-foreground">{items.length}</span>
      </h4>
      <div className="space-y-1.5">
        {shown.map((x, i) =>
          kind === "script"
            ? <ScriptRow key={i} x={x as XrefScript} />
            : <CodeRow   key={i} x={x as XrefCode} />,
        )}
        {more > 0 && (
          <div className="font-mono text-[10px] text-muted-foreground">
            … {more} more (see catalog.md)
          </div>
        )}
      </div>
    </div>
  );
}

function ScriptRow({ x }: { x: XrefScript }) {
  return (
    <div className="font-mono text-[10.5px] leading-snug">
      <span className="text-muted-foreground">
        {x.sourcePack}/{x.scriptClass} {x.scriptId} @{x.offset}
      </span>
      {x.snippet && <span className="ml-1 text-foreground/90">  {x.snippet}</span>}
    </div>
  );
}

function CodeRow({ x }: { x: XrefCode }) {
  const loc = [x.file, x.line ? `:${x.line}` : ""].join("");
  return (
    <div className="font-mono text-[10.5px] leading-snug">
      <div className="text-muted-foreground">
        {loc}
        {x.func && <span className="ml-1"> in {x.func}</span>}
      </div>
      {x.snippet && (
        <div className="break-all text-foreground/90 whitespace-pre-wrap">{x.snippet}</div>
      )}
    </div>
  );
}
