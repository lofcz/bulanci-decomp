import { Folder } from "lucide-react";
import type { CatalogEntry } from "@/types";

interface Props { entry: CatalogEntry }

export function MetaSection({ entry }: Props) {
  const rows: Array<[string, React.ReactNode]> = [
    ["resource id", <span className="font-mono">{entry.idHex} ({entry.id})</span>],
    ["class",       <span>{entry.className} <span className="text-muted-foreground">(#{entry.classId})</span></span>],
    ["size",        entry.size ? `${entry.size.toLocaleString()} bytes` : "—"],
  ];
  if (entry.folder) {
    rows.push([
      "folder",
      <span className="inline-flex items-center gap-1 font-mono text-sky-300/90">
        <Folder className="size-3 text-sky-400/70" />
        {entry.folder}
      </span>,
    ]);
  }
  if (entry.indexResourceIDHex) {
    rows.push([
      "index id",
      <span className="font-mono">
        {entry.indexResourceIDHex} ({entry.indexResourceID})
        <span className="ml-1 text-[10px] text-muted-foreground">absorbed AudioBankIndex</span>
      </span>,
    ]);
  }
  if (entry.audioBank) {
    rows.push(["samples", `${entry.audioBank.sampleCount}`]);
    rows.push(["bank bytes", `${entry.audioBank.totalBankBytes.toLocaleString()}`]);
  }
  return (
    <section>
      <SectionHeading>Metadata</SectionHeading>
      <dl className="grid grid-cols-[110px_1fr] gap-x-3 gap-y-1.5 text-xs">
        {rows.map(([k, v]) => (
          <Row key={k} k={k} v={v} />
        ))}
      </dl>
    </section>
  );
}

function Row({ k, v }: { k: string; v: React.ReactNode }) {
  return (
    <>
      <dt className="text-muted-foreground">{k}</dt>
      <dd className="text-foreground break-words">{v}</dd>
    </>
  );
}

export function SectionHeading({ children }: { children: React.ReactNode }) {
  return (
    <h3 className="mb-2 text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground">
      {children}
    </h3>
  );
}
