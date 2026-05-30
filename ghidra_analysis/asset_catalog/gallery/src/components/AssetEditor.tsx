// In-detail-panel editor for an asset's identity: slug, folder,
// notes, ship flag.  Lives at the top of `DetailSheet` so the
// "what is this thing" line is also the "what do I want it to be"
// line — same UX shape as VS Code's file-rename and Windows
// Explorer's properties dialog.
//
// All writes go through `useEditableRegistry()` so they hit the
// debounced auto-save.  When the data source is read-only the editor
// renders as a static read-out and surfaces a hint about how to
// enable writes (open via the File System Access API).

import { useCallback, useEffect, useMemo, useState } from "react";
import { ChevronDown, FolderOpen, FolderTree as FolderTreeIcon, Save, Pencil, Box } from "lucide-react";

import type { CatalogEntry } from "@/types";
import { useEditableRegistry, useSaveState } from "@/data/EditableRegistry";
import { buildFolderTree, type FolderNode } from "@/data/registryEdits";
import { SectionHeading } from "./MetaSection";
import { Input } from "@/components/ui/input";
import { Textarea } from "@/components/ui/textarea";
import { Button } from "@/components/ui/button";
import {
  Popover, PopoverTrigger, PopoverContent,
} from "@/components/ui/popover";
import { cn } from "@/lib/utils";

const SLUG_RE = /^[a-z][a-z0-9_]*$/;

interface Props { entry: CatalogEntry }

export function AssetEditor({ entry }: Props) {
  const editable = useEditableRegistry();
  const reg      = editable.registry;
  const existing = reg.assets?.[entry.key];

  // Local field state — committed on blur / Enter so each keystroke
  // doesn't trigger a full re-derive of the payload.  We sync the
  // fields back to the source of truth whenever the entry or
  // underlying registry record changes (e.g. when a different asset
  // is selected, or someone moved this asset via DnD).
  const [slug,   setSlug]   = useState(existing?.slug   ?? "");
  const [notes,  setNotes]  = useState(existing?.notes  ?? "");
  useEffect(() => { setSlug (existing?.slug  ?? ""); }, [entry.key, existing?.slug]);
  useEffect(() => { setNotes(existing?.notes ?? ""); }, [entry.key, existing?.notes]);

  const writable = editable.writable;

  const commitSlug = useCallback(() => {
    const next = slug.trim();
    if (next === (existing?.slug ?? "")) return;
    if (next && !SLUG_RE.test(next)) {
      // Validation error — bounce back to the persisted value.
      setSlug(existing?.slug ?? "");
      return;
    }
    try {
      editable.upsertAssetAction(entry.key, { slug: next || null });
    } catch (err) {
      console.error(err);
      setSlug(existing?.slug ?? "");
    }
  }, [slug, existing?.slug, entry.key, editable]);

  const commitNotes = useCallback(() => {
    const next = notes;
    if (next === (existing?.notes ?? "")) return;
    if (!existing?.slug && !next) return;            // nothing to upsert into
    try {
      editable.upsertAssetAction(entry.key, { notes: next || null });
    } catch (err) {
      console.error(err);
      setNotes(existing?.notes ?? "");
    }
  }, [notes, existing?.notes, existing?.slug, entry.key, editable]);

  const commitFolder = useCallback((next: string | null) => {
    const prev = existing?.folder ?? null;
    if (next === prev) return;
    try {
      if (existing) {
        editable.setAssetFolderAction(entry.key, next);
      } else if (next) {
        // No registry record yet — upsertAsset needs a slug.  In
        // practice we only allow folder edits once a slug exists.
        // The UI disables the folder picker in that case.
      }
    } catch (err) {
      console.error(err);
    }
  }, [existing, entry.key, editable]);

  // Read-only fallback for HTTP-without-Vite scenarios.
  if (!writable) {
    return (
      <section>
        <SectionHeading>Edit</SectionHeading>
        <ReadOnlyBanner />
      </section>
    );
  }

  return (
    <section>
      <SectionHeading>Edit</SectionHeading>
      <div className="space-y-3 rounded-md bg-card/40 p-3">
        <Field label="slug" hint={!existing?.slug ? "name this asset" : undefined}>
          <Input
            value={slug}
            placeholder="e.g. menu_sfx_bank"
            onChange={(ev) => setSlug(ev.target.value)}
            onBlur={commitSlug}
            onKeyDown={(ev) => { if (ev.key === "Enter") commitSlug(); }}
            className={cn(
              "h-7 font-mono text-xs",
              slug && !SLUG_RE.test(slug) && "border-rose-400/60 focus-visible:ring-rose-400/30",
            )}
          />
        </Field>

        <Field label="folder" hint={!existing?.slug ? "name the asset first" : undefined}>
          <FolderPicker
            value={existing?.folder ?? null}
            disabled={!existing?.slug}
            onChange={commitFolder}
          />
        </Field>

        <Field label="notes">
          <Textarea
            value={notes}
            rows={2}
            placeholder="what / where / why"
            onChange={(ev) => setNotes(ev.target.value)}
            onBlur={commitNotes}
            disabled={!existing?.slug}
            className="text-xs"
          />
        </Field>

        {existing?.slug && (
          <div className="flex items-center gap-2 text-[10px] text-muted-foreground">
            <ShipToggle
              value={!!existing.ship}
              onChange={(v) => editable.upsertAssetAction(entry.key, { ship: v || null })}
            />
            <SaveBadge />
          </div>
        )}
      </div>
    </section>
  );
}

// ---------------------------------------------------------------------
// pieces
// ---------------------------------------------------------------------

function Field({ label, hint, children }: {
  label: string;
  hint?: string;
  children: React.ReactNode;
}) {
  return (
    <label className="block">
      <div className="mb-1 flex items-center gap-1.5 text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground">
        {label}
        {hint && <span className="rounded bg-amber-500/10 px-1 py-px text-[9px] font-normal lowercase text-amber-300/90">{hint}</span>}
      </div>
      {children}
    </label>
  );
}

/** Combobox-ish folder picker: button shows current folder, click to
 *  open a small popover with the current folder tree (already projected
 *  from `editable.registry`) + free-typed "new folder" input. */
function FolderPicker({ value, disabled, onChange }: {
  value: string | null;
  disabled?: boolean;
  onChange: (next: string | null) => void;
}) {
  const editable = useEditableRegistry();
  const [open, setOpen] = useState(false);
  const [draft, setDraft] = useState("");

  const tree = useMemo(() => buildFolderTree(editable.registry), [editable.registry]);

  const pick = (path: string | null) => {
    onChange(path);
    setOpen(false);
  };
  const submitDraft = () => {
    const path = draft.trim();
    if (!path) return;
    pick(path);
    setDraft("");
  };

  return (
    <Popover open={open} onOpenChange={setOpen}>
      <PopoverTrigger asChild>
        <Button
          type="button"
          variant="outline"
          disabled={disabled}
          className="h-7 w-full justify-between gap-2 px-2 font-mono text-xs"
        >
          <span className="flex items-center gap-1.5 truncate text-foreground">
            {value
              ? <><FolderOpen className="size-3 text-sky-300/90" /> <span className="truncate">{value}</span></>
              : <span className="italic text-muted-foreground">(unsorted)</span>}
          </span>
          <ChevronDown className="size-3 shrink-0 text-muted-foreground" />
        </Button>
      </PopoverTrigger>
      <PopoverContent className="w-80 p-2" align="start">
        <div className="mb-2 flex items-center gap-1 text-[10px] uppercase tracking-[0.08em] text-muted-foreground">
          <FolderTreeIcon className="size-3" /> choose a folder
        </div>
        <ul className="max-h-72 space-y-px overflow-y-auto text-xs">
          <FolderRow
            depth={0}
            label="(unsorted)"
            path={null}
            count={null}
            active={value === null}
            onClick={() => pick(null)}
          />
          <FolderRows nodes={tree.children} value={value} onPick={pick} />
        </ul>
        <div className="mt-2 flex items-center gap-1.5 border-t border-border pt-2">
          <Input
            value={draft}
            placeholder="new folder, e.g. menu/extras"
            className="h-6 flex-1 font-mono text-xs"
            onChange={(ev) => setDraft(ev.target.value)}
            onKeyDown={(ev) => { if (ev.key === "Enter") { ev.preventDefault(); submitDraft(); } }}
          />
          <Button type="button" size="sm" variant="default" onClick={submitDraft} disabled={!draft.trim()}>
            <Pencil className="size-3" /> add
          </Button>
        </div>
      </PopoverContent>
    </Popover>
  );
}

function FolderRows({ nodes, value, onPick }: {
  nodes: FolderNode[];
  value: string | null;
  onPick: (p: string) => void;
}) {
  // Flatten with depth so we can render a single-pass list, avoiding
  // nested `<ul>` indent gymnastics.  The dataset is small (~20 entries),
  // so virtualization isn't needed here.
  const flat: Array<{ node: FolderNode; depth: number }> = [];
  const walk = (n: FolderNode, depth: number) => {
    flat.push({ node: n, depth });
    for (const c of n.children) walk(c, depth + 1);
  };
  for (const n of nodes) walk(n, 0);
  return (
    <>
      {flat.map(({ node, depth }) => (
        <FolderRow
          key={node.path}
          depth={depth}
          label={node.name}
          path={node.path}
          count={node.assetCount}
          active={value === node.path}
          onClick={() => onPick(node.path)}
        />
      ))}
    </>
  );
}

function FolderRow({ depth, label, path, count, active, onClick }: {
  depth: number;
  label: string;
  path: string | null;
  count: number | null;
  active: boolean;
  onClick: () => void;
}) {
  return (
    <li>
      <button
        type="button"
        onClick={onClick}
        title={path ?? "(unsorted)"}
        style={{ paddingLeft: 6 + depth * 12 }}
        className={cn(
          "flex w-full items-center justify-between rounded py-1 pr-2 text-left transition-colors",
          active ? "bg-primary/85 text-primary-foreground" : "hover:bg-accent/10",
          !path && !active && "italic text-muted-foreground",
        )}
      >
        <span className="truncate">{label}</span>
        {count !== null && (
          <span className={cn(
            "ml-2 shrink-0 text-[9px] tabular-nums",
            active ? "text-primary-foreground/70" : "text-muted-foreground",
          )}>
            {count}
          </span>
        )}
      </button>
    </li>
  );
}

function ShipToggle({ value, onChange }: { value: boolean; onChange: (v: boolean) => void }) {
  return (
    <label className="inline-flex cursor-pointer items-center gap-1.5 text-[10px] uppercase tracking-[0.08em]">
      <input
        type="checkbox"
        checked={value}
        onChange={(ev) => onChange(ev.target.checked)}
        className="size-3 accent-primary"
      />
      ship
    </label>
  );
}

function SaveBadge() {
  const state = useSaveState();
  switch (state.kind) {
    case "idle":   return null;
    case "dirty":  return <span className="ml-auto inline-flex items-center gap-1 text-amber-300"><Save className="size-2.5" /> edited</span>;
    case "saving": return <span className="ml-auto inline-flex items-center gap-1 text-sky-300 animate-pulse"><Save className="size-2.5" /> saving…</span>;
    case "saved":  return <span className="ml-auto inline-flex items-center gap-1 text-emerald-300"><Save className="size-2.5" /> saved</span>;
    case "error":  return <span className="ml-auto inline-flex items-center gap-1 text-rose-300" title={state.message}><Save className="size-2.5" /> save failed</span>;
  }
}

function ReadOnlyBanner() {
  return (
    <div className="flex items-start gap-2 rounded-md bg-card/40 p-3 text-[11px] text-muted-foreground">
      <Box className="mt-0.5 size-3.5 shrink-0" />
      <div>
        <p className="font-semibold text-foreground">Read-only</p>
        <p className="mt-1 leading-relaxed">
          Open the repo via the File System Access API or run the Vite dev server
          (<code className="rounded bg-card px-1 text-primary">bun run dev</code>) to enable in-app editing.
          You can still edit{" "}
          <code className="rounded bg-card px-1 text-primary">registry.json</code>{" "}
          by hand.
        </p>
      </div>
    </div>
  );
}
