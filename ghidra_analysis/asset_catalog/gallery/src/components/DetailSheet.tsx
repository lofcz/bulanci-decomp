import { memo } from "react";
import { Folder } from "lucide-react";
import type { CatalogEntry, FolderMeta } from "@/types";
import type { HandleManifest } from "@/data/handleManifest";
import {
  Sheet, SheetContent, SheetHeader, SheetTitle, SheetDescription,
} from "@/components/ui/sheet";
import { ScrollArea } from "@/components/ui/scroll-area";
import { PreviewSection } from "./PreviewSection";
import { MetaSection, SectionHeading } from "./MetaSection";
import { CategoriesSection } from "./CategoriesSection";
import { NotesSection } from "./NotesSection";
import { XrefsSection } from "./XrefsSection";
import { FilesSection } from "./FilesSection";
import { BankSamples } from "./BankSamples";
import { AssetEditor } from "./AssetEditor";
import { LiveEngineSection } from "./LiveEngineSection";
import { cn } from "@/lib/utils";

interface Props {
  entry: CatalogEntry | null;
  manifest: HandleManifest | null;
  scriptRegistry: Record<string, { slug?: string; notes?: string }>;
  folderMeta:     Record<string, FolderMeta>;
  open: boolean;
  onOpenChange: (open: boolean) => void;
  onCategoryClick: (category: string) => void;
  onFolderClick?:  (folder: string) => void;
}

export const DetailSheet = memo(function DetailSheet({ entry, manifest, folderMeta, open, onOpenChange, onCategoryClick, onFolderClick }: Props) {
  return (
    <Sheet open={open} onOpenChange={onOpenChange}>
      <SheetContent side="right" className="flex w-full flex-col p-0 sm:max-w-2xl">
        {entry && (
          <>
            <SheetHeader>
              <SheetTitle className={entry.slug ? "" : "italic text-muted-foreground"}>
                {entry.slug || "(unnamed)"}
              </SheetTitle>
              <SheetDescription>
                {entry.idHex} · {entry.id} · {entry.className}
                {entry.folder && (
                  <span className="ml-2 inline-flex items-center gap-1 text-sky-300/90">
                    <Folder className="size-3" /> {entry.folder}
                  </span>
                )}
              </SheetDescription>
            </SheetHeader>
            <ScrollArea className="flex-1">
              <div className="space-y-6 p-6">
                <PreviewSection entry={entry} />
                <AssetEditor entry={entry} />
                <LiveEngineSection entry={entry} manifest={manifest} />
                <MetaSection entry={entry} />
                {entry.folder && (
                  <FolderSection
                    folder={entry.folder}
                    meta={folderMeta[entry.folder]}
                    onFolderClick={onFolderClick}
                  />
                )}
                {entry.audioBank && <BankSamples bank={entry.audioBank} entry={entry} />}
                <CategoriesSection entry={entry} onCategoryClick={onCategoryClick} />
                <NotesSection entry={entry} />
                <XrefsSection entry={entry} />
                <FilesSection entry={entry} />
              </div>
            </ScrollArea>
          </>
        )}
      </SheetContent>
    </Sheet>
  );
});

/** Renders folder breadcrumbs + optional folder-level notes.  Each
 *  breadcrumb is a button so clicking a parent narrows the gallery to
 *  the whole subtree at that level. */
function FolderSection({ folder, meta, onFolderClick }: {
  folder: string;
  meta?: FolderMeta;
  onFolderClick?: (folder: string) => void;
}) {
  const parts = folder.split("/");
  return (
    <section>
      <SectionHeading>Folder</SectionHeading>
      <nav className="flex flex-wrap items-center gap-1 font-mono text-[11px]">
        {parts.map((leaf, i) => {
          const path = parts.slice(0, i + 1).join("/");
          const isLast = i === parts.length - 1;
          return (
            <span key={path} className="inline-flex items-center gap-1">
              <button
                type="button"
                onClick={() => onFolderClick?.(path)}
                className={cn(
                  "rounded px-1.5 py-0.5 transition-colors",
                  isLast
                    ? "bg-sky-500/15 text-sky-200"
                    : "text-sky-300/80 hover:bg-sky-500/10 hover:text-sky-200",
                )}
              >
                {leaf}
              </button>
              {!isLast && <span className="text-muted-foreground">/</span>}
            </span>
          );
        })}
      </nav>
      {meta?.notes && (
        <p className="mt-2 rounded-md bg-card/40 px-3 py-2 text-xs leading-relaxed text-muted-foreground">
          {meta.notes}
        </p>
      )}
    </section>
  );
}
