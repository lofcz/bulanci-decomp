import { memo, useMemo } from "react";
import { Search, X, FolderOpen, Globe, HardDrive, ChevronDown } from "lucide-react";
import type { CatalogPayload } from "@/types";
import type { DataSourceInfo } from "@/data/DataSource";
import { Input } from "@/components/ui/input";
import { Button } from "@/components/ui/button";
import { Separator } from "@/components/ui/separator";
import { cn } from "@/lib/utils";
import { FolderTree } from "./FolderTree";

export interface Filters {
  named:    "named" | "unnamed" | null;
  category: string | null;
  class:    string | null;
  /** Folder filter.  A folder path (`"menu/widgets"`) matches that
   *  exact folder *and any descendant* (prefix model — clicking a
   *  parent acts as "show the whole subtree").  The sentinel value
   *  `UNSORTED_FOLDER` matches assets without a folder. */
  folder:   string | null;
  search:   string;
}

interface SidebarProps {
  payload: CatalogPayload;
  filters: Filters;
  onFiltersChange: (next: Filters) => void;
  /** Stable folder-only setter for the folder tree (so its rows don't
   *  close over the whole changing `filters` object). */
  onFolderChange: (folder: string | null) => void;
  visibleCount: number;
  sourceInfo?: DataSourceInfo;
  onChangeSource?: () => void;
}

export const Sidebar = memo(function Sidebar({ payload, filters, onFiltersChange, onFolderChange, visibleCount, sourceInfo, onChangeSource }: SidebarProps) {
  const { totals, coverage, categoryCounts, classCounts } = payload;

  const categoryItems = useMemo(
    () => Object.entries(categoryCounts)
      .sort(([a, av], [b, bv]) => bv - av || a.localeCompare(b)),
    [categoryCounts],
  );

  const classItems = useMemo(
    () => Object.entries(classCounts).sort(([a], [b]) => a.localeCompare(b)),
    [classCounts],
  );

  const clearAll = () =>
    onFiltersChange({ named: null, category: null, class: null, folder: null, search: "" });

  const anyActive =
    filters.named || filters.category || filters.class || filters.folder || filters.search;

  return (
    <aside className="flex h-screen flex-col border-r border-border bg-card/40 backdrop-blur-xs">
      <div className="p-5 pb-3">
        <h1 className="mb-1 text-sm font-bold uppercase tracking-[0.18em] text-primary">
          Asset Catalog
        </h1>
        <p className="text-[11px] leading-relaxed text-muted-foreground">
          <span className="font-semibold text-foreground">{totals.assets}</span> assets
          {" · "}
          <span className="font-semibold text-foreground">{totals.named}</span> named
          {" · "}
          <span className="font-semibold text-foreground">{totals.unnamed}</span> to-do
          <br />
          coverage:{" "}
          <span className="font-semibold text-foreground">{coverage.withConcreteXref ?? "?"}</span> concrete,{" "}
          {coverage.orphanOnly ?? 0} orphan, {coverage.deadOnly ?? 0} dead
        </p>
      </div>

      <div className="px-5">
        <div className="relative">
          <Search className="absolute left-2.5 top-2.5 size-4 text-muted-foreground" />
          <Input
            placeholder="search id, slug, notes…"
            value={filters.search}
            onChange={(ev) => onFiltersChange({ ...filters, search: ev.target.value })}
            className="pl-8 h-9"
          />
        </div>
        <p className="mt-2 text-[10px] text-muted-foreground">
          {visibleCount} of {totals.assets} match
        </p>
      </div>

      <div className="px-2 py-2">
        <FacetGroup
          title="Name status"
          items={[
            ["all",     null,       totals.assets],
            ["named",   "named",    totals.named],
            ["unnamed", "unnamed",  totals.unnamed],
          ]}
          value={filters.named}
          onChange={(v) => onFiltersChange({ ...filters, named: v as Filters["named"] })}
        />
        <Separator className="mt-2" />
      </div>

      {/* The folder tree fills the remaining vertical space; it owns
          its own virtualised scroll so we explicitly *don't* wrap it
          in the surrounding ScrollArea. */}
      <div className="min-h-0 flex-1">
        <FolderTree
          payload={payload}
          activeFolder={filters.folder}
          onFolderChange={onFolderChange}
        />
      </div>

      <details className="border-t border-border px-2 py-1 text-xs [&_summary::-webkit-details-marker]:hidden">
        <summary className="flex cursor-pointer items-center gap-1 px-3 py-1 text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground hover:text-foreground">
          <ChevronDown className="size-3 transition-transform duration-150 [details:not([open])>summary>&]:-rotate-90" /> Category
        </summary>
        <FacetGroup
          items={[["all", null, totals.assets], ...categoryItems.map(([k, v]) => [k, k, v] as const)]}
          value={filters.category}
          onChange={(v) => onFiltersChange({ ...filters, category: v })}
        />
      </details>
      <details className="border-t border-border px-2 py-1 text-xs [&_summary::-webkit-details-marker]:hidden">
        <summary className="flex cursor-pointer items-center gap-1 px-3 py-1 text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground hover:text-foreground">
          <ChevronDown className="size-3 transition-transform duration-150 [details:not([open])>summary>&]:-rotate-90" /> Class
        </summary>
        <FacetGroup
          items={[["all", null, totals.assets], ...classItems.map(([k, v]) => [k, k, v] as const)]}
          value={filters.class}
          onChange={(v) => onFiltersChange({ ...filters, class: v })}
        />
      </details>

      {anyActive && (
        <div className="border-t border-border p-3">
          <Button variant="outline" size="sm" onClick={clearAll} className="w-full text-[11px]">
            <X className="size-3" /> clear all filters
          </Button>
        </div>
      )}

      {sourceInfo && (
        <div className="border-t border-border px-3 py-2 text-[10px] text-muted-foreground">
          <div className="flex items-center gap-1.5">
            {sourceInfo.kind === "fs"
              ? <HardDrive className="size-3" />
              : <Globe className="size-3" />}
            <span className="flex-1 truncate font-mono" title={sourceInfo.label}>
              {sourceInfo.label}
            </span>
            {onChangeSource && (
              <button
                type="button"
                onClick={onChangeSource}
                className="rounded p-1 hover:bg-accent/10"
                title="Switch data source"
              >
                <FolderOpen className="size-3" />
              </button>
            )}
          </div>
        </div>
      )}
    </aside>
  );
});

type FacetItem = readonly [label: string, value: string | null, count: number];

function FacetGroup({
  title, items, value, onChange,
}: {
  /** Optional heading; we omit it when the facet is already wrapped
   *  in a `<details>` summary. */
  title?: string;
  items: FacetItem[] | readonly FacetItem[];
  value: string | null;
  onChange: (v: string | null) => void;
}) {
  return (
    <div className="px-3 pb-2">
      {title && (
        <h2 className="mb-1 text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground">
          {title}
        </h2>
      )}
      <ul className="m-0 list-none space-y-[1px] p-0 max-h-44 overflow-y-auto">
        {items.map(([label, val, count]) => {
          const active = value === val;
          return (
            <li key={label}>
              <button
                type="button"
                onClick={() => onChange(active ? null : val)}
                className={cn(
                  "flex w-full items-center justify-between rounded px-2 py-1 text-left text-xs transition-colors",
                  active
                    ? "bg-primary/90 text-primary-foreground font-semibold"
                    : "hover:bg-accent/10",
                )}
              >
                <span className="truncate">{label}</span>
                <span className={cn(
                  "ml-2 shrink-0 text-[10px] tabular-nums",
                  active ? "text-primary-foreground/70" : "text-muted-foreground",
                )}>
                  {count}
                </span>
              </button>
            </li>
          );
        })}
      </ul>
    </div>
  );
}
