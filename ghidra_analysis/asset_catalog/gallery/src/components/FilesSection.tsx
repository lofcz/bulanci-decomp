import type { CatalogEntry } from "@/types";
import { SectionHeading } from "./MetaSection";
import { useOverlayUrl } from "@/data/DataSourceContext";

export function FilesSection({ entry }: { entry: CatalogEntry }) {
  if (!entry.files || entry.files.length === 0) return null;
  return (
    <section>
      <SectionHeading>Source files (unpacked/overlay/)</SectionHeading>
      <div className="flex flex-col gap-0.5 font-mono text-[10.5px]">
        {entry.files.map(f => <FileLink key={f} filename={f} />)}
      </div>
    </section>
  );
}

function FileLink({ filename }: { filename: string }) {
  const url = useOverlayUrl(filename);
  // FS blob URLs can't usefully open in a new tab (browsers strip the
  // file extension from the URL), but the link still triggers a
  // download / preview just fine.
  return (
    <a
      href={url ?? "#"}
      target={url ? "_blank" : undefined}
      rel="noopener noreferrer"
      className="break-all text-sky-300 hover:text-primary hover:underline"
      onClick={(ev) => { if (!url) ev.preventDefault(); }}
    >
      {filename}
    </a>
  );
}
