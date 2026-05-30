import { memo } from "react";
import { Music, AlignJustify, FileText, Folder } from "lucide-react";
import type { CatalogEntry } from "@/types";
import { cn } from "@/lib/utils";
import { Badge } from "@/components/ui/badge";
import { PoemBlock } from "./Poem";
import { FontPreview } from "./FontRenderer";
import { useOverlayUrl } from "@/data/DataSourceContext";
import { ASSET_DRAG_TYPE } from "./FolderTree";

const FONT_SAMPLE = "Příliš žluťoučký kůň úpěl ďábelské ódy";

interface TileProps {
  entry: CatalogEntry;
  selected: boolean;
  onClick: () => void;
  /** Whether the registry is writable — passed down (not read from
   *  context) so a registry mutation never re-renders every tile. */
  writable: boolean;
}

function TileInner({ entry, selected, onClick, writable }: TileProps) {
  // Tiles are HTML5 drag sources; the folder tree listens for the
  // `ASSET_DRAG_TYPE` MIME on its rows.  We disable dragging when the
  // registry is read-only so dropping silently goes nowhere.
  const dragProps = writable ? {
    draggable: true,
    onDragStart: (e: React.DragEvent) => {
      e.dataTransfer.effectAllowed = "move";
      e.dataTransfer.setData(ASSET_DRAG_TYPE, entry.key);
      // NB: deliberately do NOT also set `text/plain`. The folder tree is
      // a react-arborist tree whose rows are react-dnd drop targets; its
      // HTML5 backend latches onto any *native* drag type (text/plain,
      // text/uri-list, Files…) and then re-renders the whole tree + its
      // drag-preview layer on every `dragover` frame, which also corrupts
      // the native drag image. Carrying only our custom MIME keeps react-dnd
      // dormant — the drop handlers read `ASSET_DRAG_TYPE` directly.
    },
  } : {};
  return (
    <button
      type="button"
      onClick={onClick}
      {...dragProps}
      className={cn(
        "group flex flex-col overflow-hidden rounded-lg border bg-card text-left",
        "min-h-[200px] transition-all duration-150",
        "hover:border-primary/60 hover:-translate-y-0.5 hover:shadow-lg",
        selected
          ? "border-primary ring-2 ring-primary/50"
          : "border-border",
        writable && "cursor-grab active:cursor-grabbing",
      )}
    >
      <TilePreview entry={entry} />
      <div className="flex flex-1 flex-col gap-1 p-2.5 pt-2">
        <div className="font-mono text-[10px] text-muted-foreground">
          {entry.idHex} · {entry.id}
        </div>
        <div className={cn(
          "text-xs font-semibold leading-tight break-words",
          entry.slug ? "text-foreground" : "italic font-normal text-muted-foreground",
        )}>
          {entry.slug || "(unnamed)"}
        </div>
        {entry.folder && (
          <div
            className="flex items-center gap-1 truncate font-mono text-[9px] text-muted-foreground"
            title={entry.folder}
          >
            <Folder className="size-2.5 shrink-0 text-sky-400/70" />
            <span className="truncate">{entry.folder}</span>
          </div>
        )}
        {entry.categories.length > 0 && (
          <div className="mt-auto truncate text-[9px] uppercase tracking-wide text-primary/80">
            {entry.categories.join(" · ")}
          </div>
        )}
      </div>
    </button>
  );
}

function TilePreview({ entry }: { entry: CatalogEntry }) {
  const cls = entry.className;
  const isAudio = cls === "Mp3" || cls === "AudioBank";
  const isPoem  = cls === "Poem";
  const isFont  = cls === "Font";
  const isText  = cls === "Script" || cls === "HistoryScript" || cls === "DsmInner";
  const isVisual = entry.preview && !isAudio && !isPoem && !isFont && !isText;
  const previewUrl = useOverlayUrl(isVisual ? entry.preview : null);

  return (
    <div className="relative flex h-[130px] w-full shrink-0 items-center justify-center overflow-hidden border-b border-border bg-[#0d0f12]">
      {isVisual && previewUrl && (
        <img
          src={previewUrl}
          alt={entry.idHex}
          loading="lazy"
          className="max-h-full max-w-full pixelated"
        />
      )}
      {isPoem && entry.poemText && (
        <div className="w-full h-full overflow-hidden p-2">
          <PoemBlock text={entry.poemText} maxLines={5} condensed />
        </div>
      )}
      {isPoem && !entry.poemText && (
        <PlaceholderGlyph icon={<AlignJustify className="size-8 text-blue-300" />} label="poem" />
      )}
      {isFont && entry.fontMeta && (
        <div className="flex w-full flex-col items-center justify-center gap-1 overflow-hidden px-2">
          <FontPreview font={entry.fontMeta} text={FONT_SAMPLE} scale={2} maxWidth={200} />
          <FontPreview font={entry.fontMeta} text="0123456789 !?." scale={2} maxWidth={200} />
          <span className="text-[8px] uppercase tracking-wider text-muted-foreground">
            lineHeight {entry.fontMeta.lineHeight}px
          </span>
        </div>
      )}
      {isFont && !entry.fontMeta && (
        <PlaceholderGlyph icon={<FileText className="size-8 text-amber-300" />} label="font" />
      )}
      {isAudio && (
        <PlaceholderGlyph icon={<Music className="size-8 text-emerald-400" />} label={cls.toLowerCase()} />
      )}
      {isText && (
        <PlaceholderGlyph icon={<FileText className="size-8 text-blue-300" />} label={cls.toLowerCase()} />
      )}
      {!isVisual && !isAudio && !isPoem && !isFont && !isText && (
        <span className="text-[10px] italic text-muted-foreground">no preview</span>
      )}

      <Badge variant="outline" className="absolute left-1.5 top-1.5 bg-black/60 border-black/20 backdrop-blur-xs text-[8px]">
        {cls}
      </Badge>
      {entry.deadOnly && (
        <Badge variant="warn" className="absolute right-1.5 top-1.5 text-[8px]">DEAD</Badge>
      )}
      {!entry.deadOnly && entry.orphanOnly && (
        <Badge variant="accent" className="absolute right-1.5 top-1.5 text-[8px]">ORPHAN</Badge>
      )}
    </div>
  );
}

function PlaceholderGlyph({ icon, label }: { icon: React.ReactNode; label: string }) {
  return (
    <div className="flex flex-col items-center justify-center gap-2">
      {icon}
      <span className="text-[10px] uppercase tracking-wider text-muted-foreground">
        {label}
      </span>
    </div>
  );
}

// Memo so changing the selected key only re-renders the two tiles
// whose `selected` prop flipped.  `onClick` is intentionally excluded
// (the grid hands a fresh closure each render but it's always wired to
// the same stable `onSelect`); identity-compare the data + flags.
export const Tile = memo(TileInner, (a, b) =>
  a.entry === b.entry && a.selected === b.selected && a.writable === b.writable,
);
