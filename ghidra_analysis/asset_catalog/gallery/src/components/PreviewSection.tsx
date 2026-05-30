import { useState } from "react";
import type { CatalogEntry } from "@/types";
import { JpegAnimPlayer } from "./JpegAnimPlayer";
import { PoemBlock } from "./Poem";
import { Card } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { FontPreview, FontGlyphTable } from "./FontRenderer";
import { SectionHeading } from "./MetaSection";
import { useOverlayUrl } from "@/data/DataSourceContext";

interface Props { entry: CatalogEntry }

const DEFAULT_FONT_SAMPLE = "Příliš žluťoučký kůň úpěl ďábelské ódy";

export function PreviewSection({ entry }: Props) {
  if (entry.className === "Font" && entry.fontMeta) {
    return <FontDetail entry={entry} />;
  }
  return (
    <Card className="flex min-h-[120px] items-center justify-center bg-black/40 p-3">
      <PreviewBody entry={entry} />
    </Card>
  );
}

function FontDetail({ entry }: Props) {
  const [sample, setSample] = useState(DEFAULT_FONT_SAMPLE);
  const font = entry.fontMeta!;
  return (
    <div className="space-y-4">
      <Card className="flex min-h-[120px] flex-col items-center justify-center gap-3 bg-black/40 p-4">
        <FontPreview font={font} text={sample} scale={3} />
        <Input
          value={sample}
          onChange={(ev) => setSample(ev.target.value)}
          placeholder="type to render…"
          className="max-w-md text-xs"
        />
      </Card>
      <section>
        <SectionHeading>Glyph table ({font.characters.filter(c => c.unicodeChar).length})</SectionHeading>
        <FontGlyphTable font={font} />
      </section>
    </div>
  );
}

function PreviewBody({ entry }: Props) {
  const audioFile = (entry.className === "Mp3" || entry.className === "AudioBank") ? entry.preview : null;
  const audioUrl  = useOverlayUrl(audioFile);

  const gifFilename   = entry.anim?.kind === "gif" ? entry.anim.filename : null;
  const stripFilename = entry.anim?.kind === "gif" ? (entry.anim.strip ?? null) : null;
  const gifUrl   = useOverlayUrl(gifFilename);
  const stripUrl = useOverlayUrl(stripFilename);

  const stillFile = (!audioFile && entry.anim?.kind !== "gif" && entry.anim?.kind !== "frames")
                  ? entry.preview : null;
  const stillUrl  = useOverlayUrl(stillFile);

  if (audioFile) {
    return <audio controls src={audioUrl ?? undefined} className="w-full" />;
  }
  if (entry.className === "Poem" && entry.poemText) {
    return (
      <div className="w-full rounded-md bg-card/40 p-5">
        <PoemBlock text={entry.poemText} />
      </div>
    );
  }
  if (entry.anim?.kind === "gif") {
    return (
      <div className="flex w-full flex-col items-center gap-3">
        {gifUrl && (
          <img src={gifUrl} alt={entry.idHex}
               className="pixelated max-h-[220px] max-w-full rounded-md bg-black" />
        )}
        {stripUrl && (
          <img src={stripUrl} alt={`${entry.idHex} atlas strip`}
               className="pixelated max-h-[80px] max-w-full rounded-md opacity-70" />
        )}
        <p className="text-[10px] uppercase tracking-wider text-muted-foreground">
          animated{stripUrl ? " · atlas strip below" : ""}
        </p>
      </div>
    );
  }
  if (entry.anim?.kind === "frames") {
    return <JpegAnimPlayer frames={entry.anim.frames} audioFile={entry.anim.audioFile ?? null} />;
  }
  if (stillFile && stillUrl) {
    return (
      <img src={stillUrl} alt={entry.idHex} className="pixelated max-h-[280px] max-w-full" />
    );
  }
  if (entry.className === "Script" || entry.className === "HistoryScript") {
    return (
      <p className="text-xs italic text-muted-foreground">
        Preview: see “Files” below for raw text / asm.
      </p>
    );
  }
  return (
    <p className="text-xs italic text-muted-foreground">
      No preview for {entry.className}.
    </p>
  );
}
