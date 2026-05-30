import type { CatalogEntry } from "@/types";
import { SectionHeading } from "./MetaSection";

interface Props {
  entry: CatalogEntry;
  onCategoryClick: (category: string) => void;
}

export function CategoriesSection({ entry, onCategoryClick }: Props) {
  if (entry.categories.length === 0 && entry.tags.length === 0) return null;
  return (
    <section>
      <SectionHeading>Categories</SectionHeading>
      <div className="flex flex-wrap gap-1.5">
        {entry.categories.map(c => (
          <button
            key={c}
            type="button"
            onClick={() => onCategoryClick(c)}
            className="rounded-md bg-card px-2 py-0.5 font-mono text-[11px] text-primary transition-colors hover:bg-primary hover:text-primary-foreground"
          >
            {c}
          </button>
        ))}
      </div>
      {entry.tags.length > 0 && (
        <div className="mt-2 flex flex-wrap gap-1">
          {entry.tags.map(t => (
            <span key={t} className="rounded-full bg-primary/15 px-2 py-0.5 text-[10px] text-primary">
              {t}
            </span>
          ))}
        </div>
      )}
    </section>
  );
}
