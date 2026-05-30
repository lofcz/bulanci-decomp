import { memo, useEffect, useRef, useState } from "react";
import { useVirtualizer } from "@tanstack/react-virtual";
import type { CatalogEntry } from "@/types";
import { Tile } from "./Tile";

interface TileGridProps {
  entries: CatalogEntry[];
  selectedKey: string | null;
  onSelect: (key: string) => void;
  writable: boolean;
}

// Visual constants — kept in sync with the Tile's intrinsic size and
// the grid gap.  Used by the virtualizer to compute column counts and
// row heights ahead of mount.
const TILE_MIN_WIDTH = 170;
const TILE_HEIGHT    = 210;   // 130px preview + ~80px caption
const GAP            = 12;
const PADDING        = 16;


function TileGridInner({ entries, selectedKey, onSelect, writable }: TileGridProps) {
  const scrollRef = useRef<HTMLDivElement | null>(null);
  const [cols, setCols] = useState(1);

  // Track the scroll container's width so the row virtualizer knows
  // how many tiles fit per row.
  useEffect(() => {
    const el = scrollRef.current;
    if (!el) return;
    const compute = () => {
      const inner = el.clientWidth - PADDING * 2;
      const next  = Math.max(
        1,
        Math.floor((inner + GAP) / (TILE_MIN_WIDTH + GAP)),
      );
      setCols(prev => (prev === next ? prev : next));
    };
    compute();
    const ro = new ResizeObserver(compute);
    ro.observe(el);
    return () => ro.disconnect();
  }, []);

  const rowCount = Math.ceil(entries.length / cols);
  const rowVirtualizer = useVirtualizer({
    count: rowCount,
    getScrollElement: () => scrollRef.current,
    estimateSize: () => TILE_HEIGHT + GAP,
    overscan: 4,
  });

  if (entries.length === 0) {
    return (
      <div
        ref={scrollRef}
        className="h-full overflow-y-auto"
      >
        <div className="p-10 text-center text-sm text-muted-foreground">
          No assets match the current filters.
        </div>
      </div>
    );
  }

  return (
    <div ref={scrollRef} className="h-full overflow-y-auto">
      {/* Virtualizer needs a sized parent it can absolutely position
          rows inside.  `totalSize` already accounts for the row gap
          (we baked it into `estimateSize`); pad the container so the
          first/last row inset matches the original CSS-grid look. */}
      <div
        style={{
          height: `${rowVirtualizer.getTotalSize() + PADDING * 2}px`,
          paddingTop:    `${PADDING}px`,
          paddingBottom: `${PADDING}px`,
          position: "relative",
        }}
      >
        {rowVirtualizer.getVirtualItems().map(virtRow => {
          const start = virtRow.index * cols;
          const rowItems = entries.slice(start, start + cols);
          return (
            <div
              key={virtRow.key}
              data-row-index={virtRow.index}
              style={{
                position: "absolute",
                top: 0,
                left: 0,
                right: 0,
                transform: `translateY(${virtRow.start + PADDING}px)`,
                paddingLeft:  `${PADDING}px`,
                paddingRight: `${PADDING}px`,
                display: "grid",
                gridTemplateColumns: `repeat(${cols}, minmax(0, 1fr))`,
                gap: `${GAP}px`,
              }}
            >
              {rowItems.map(e => (
                <Tile
                  key={e.key}
                  entry={e}
                  selected={selectedKey === e.key}
                  onClick={() => onSelect(e.key)}
                  writable={writable}
                />
              ))}
            </div>
          );
        })}
      </div>
    </div>
  );
}

// Memo so flipping `selectedKey` doesn't re-render every tile —
// only the previously-selected and newly-selected ones update.
export const TileGrid = memo(TileGridInner);
