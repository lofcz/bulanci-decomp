import type { CatalogEntry } from "@/types";
import { SectionHeading } from "./MetaSection";

interface Props { entry: CatalogEntry }

export function NotesSection({ entry }: Props) {
  return (
    <section>
      <SectionHeading>Notes</SectionHeading>
      {entry.notes ? (
        <div className="whitespace-pre-wrap rounded-md bg-card/40 p-3 text-xs leading-relaxed">
          {entry.notes}
        </div>
      ) : (
        <div className="rounded-md bg-card/40 p-3 text-xs italic text-muted-foreground">
          No notes yet.
        </div>
      )}
      <details className="mt-2 text-[10px] text-muted-foreground">
        <summary className="cursor-pointer hover:text-foreground">how to add a note</summary>
        <div className="mt-1 leading-relaxed">
          Add to <code className="rounded bg-card px-1 text-primary">ghidra_analysis/asset_catalog/registry.json</code>:
          <pre className="mt-1 whitespace-pre overflow-x-auto rounded bg-black/30 p-2 text-primary">
{`"assets": {
  "${entry.key}": {
    "slug":  "your_slug",
    "notes": "what / where / why"
  }
}`}
          </pre>
          then refresh the browser, or use the in-app editor when running locally.
        </div>
      </details>
    </section>
  );
}
