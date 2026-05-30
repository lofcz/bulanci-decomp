import { useMemo } from "react";
import { cn } from "@/lib/utils";

/** Parse `[CENTER]` / `[LEFT]` / `[RIGHT]` / `[TITLE]` markers and emit
 *  the lines together with an alignment so they can be rendered. */
export function parsePoem(raw: string): Array<{ kind: "line" | "blank"; align: "left" | "center" | "right"; text: string; isTitle: boolean }> {
  const lines = raw.replace(/\r\n/g, "\n").split("\n");
  const out: ReturnType<typeof parsePoem> = [];
  for (const rawLine of lines) {
    const line = rawLine.trim();
    if (line === "") {
      out.push({ kind: "blank", align: "left", text: "", isTitle: false });
      continue;
    }
    const m = /^\[(LEFT|CENTER|RIGHT|TITLE)\]\s*(.*)$/i.exec(line);
    if (m) {
      const tag = m[1].toUpperCase();
      const align = tag === "TITLE" ? "center" : (tag.toLowerCase() as "left" | "center" | "right");
      out.push({ kind: "line", align, text: m[2], isTitle: tag === "TITLE" });
    } else {
      out.push({ kind: "line", align: "left", text: line, isTitle: false });
    }
  }
  return out;
}

interface PoemBlockProps {
  text: string;
  /** Cap rendered non-blank line count. 0 = render all. */
  maxLines?: number;
  /** Compact spacing/font for thumbnails. */
  condensed?: boolean;
  className?: string;
}

export function PoemBlock({ text, maxLines = 0, condensed = false, className }: PoemBlockProps) {
  const { displayed, truncated } = useMemo(() => {
    const parsed = parsePoem(text);
    if (maxLines <= 0) return { displayed: parsed, truncated: false };
    const out: typeof parsed = [];
    let kept = 0;
    for (const item of parsed) {
      out.push(item);
      if (item.kind === "line") {
        kept++;
        if (kept >= maxLines) {
          return { displayed: out, truncated: parsed.length > out.length };
        }
      }
    }
    return { displayed: out, truncated: false };
  }, [text, maxLines]);

  return (
    <div className={cn(
      "font-serif",
      condensed
        ? "text-[10px] leading-tight text-muted-foreground"
        : "text-sm leading-relaxed text-foreground",
      className,
    )}>
      {displayed.map((item, i) => {
        if (item.kind === "blank") {
          return <div key={i} className={condensed ? "h-1" : "h-3"} aria-hidden />;
        }
        return (
          <div
            key={i}
            className={cn(
              "whitespace-pre-wrap break-words",
              item.align === "left"   && "text-left",
              item.align === "center" && "text-center",
              item.align === "right"  && "text-right",
              item.isTitle           && "font-semibold text-primary",
            )}
          >
            {item.text}
          </div>
        );
      })}
      {truncated && (
        <div className="text-center italic text-muted-foreground pt-1">…</div>
      )}
    </div>
  );
}
