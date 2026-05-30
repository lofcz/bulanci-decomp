// Conditional-state CRUD — the scene's declared `doc.states`.
//
// States are named booleans (one bit each, max 64) that gate node visibility:
// a control with `state: "sg1"` is only drawn when `sg1` is active. The engine
// seeds the runtime set from `doc.states` on (re)load (`SceneDoc::instantiate`)
// before scripts run, so editing states here + "save + live" hot-reloads the
// running game with the new declaration. Scripts (boot.luau etc.) may still
// flip states at runtime — hence the per-state "default" is the *initial* value
// only, and the eye toggle is an editor-preview that never persists.
//
// Every structural edit (add / rename / delete / default) funnels through the
// store's undoable Travels recipe; rename/delete cascade onto node `state` tags
// so references never dangle.

import { memo, useState } from "react";
import { useShallow } from "zustand/react/shallow";
import { Plus, Trash2, Eye, EyeOff, Pencil, Power } from "lucide-react";

import { cn } from "@/lib/utils";
import { useSceneStore, countStateUsage } from "@/scene/store";
import type { StateInit } from "@/scene/model";

const EMPTY_STATES: StateInit[] = [];

export const StatesSection = memo(function StatesSection() {
  // Subscribe narrowly: `meta.states` keeps its identity until a state edit and
  // the `nodes` map ref changes only on a committed node edit (NOT during a
  // drag, which touches only the drag slice) — so this section re-renders on
  // state CRUD or a node edit, never on selection, drag, or unrelated churn.
  const states = useSceneStore(useShallow((s) => s.meta?.states ?? EMPTY_STATES));
  const nodes = useSceneStore((s) => s.nodes);
  const preview = useSceneStore((s) => s.states);
  const hasDoc = useSceneStore((s) => s.meta != null);
  const a = useSceneStore(useShallow((s) => ({
    addState: s.addState, renameState: s.renameState, deleteState: s.deleteState,
    setStateDefault: s.setStateDefault, toggleState: s.toggleState,
  })));

  if (!hasDoc) return null;
  const usage = countStateUsage(Object.values(nodes));

  return (
    <div className="border-t border-border/60 p-2">
      <div className="mb-1.5 flex items-center justify-between">
        <span className="text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground">states</span>
        <span className="text-[9px] text-muted-foreground/60">{states.length}/64</span>
      </div>

      {states.length === 0 && (
        <p className="mb-1.5 text-[10px] text-muted-foreground/70">
          No states. Add one, then tag nodes with it in the inspector to gate visibility.
        </p>
      )}

      <div className="space-y-0.5">
        {states.map((s) => (
          <StateRow
            key={s.name}
            name={s.name}
            on={s.on}
            previewOn={!!preview[s.name]}
            used={usage[s.name] ?? 0}
            onPreview={() => a.toggleState(s.name)}
            onRename={(next) => a.renameState(s.name, next)}
            onDefault={() => a.setStateDefault(s.name, !s.on)}
            onDelete={() => {
              const used = usage[s.name] ?? 0;
              if (used > 0 && !window.confirm(`Delete state "${s.name}"? It will be removed from ${used} node${used === 1 ? "" : "s"}.`)) return;
              a.deleteState(s.name);
            }}
          />
        ))}
      </div>

      <AddStateRow onAdd={a.addState} />
    </div>
  );
});

function StateRow({
  name, on, previewOn, used, onPreview, onRename, onDefault, onDelete,
}: {
  name: string; on: boolean; previewOn: boolean; used: number;
  onPreview: () => void; onRename: (next: string) => void;
  onDefault: () => void; onDelete: () => void;
}) {
  const [editing, setEditing] = useState(false);
  const [draft, setDraft] = useState(name);

  const startEdit = () => { setDraft(name); setEditing(true); };
  const commit = () => {
    setEditing(false);
    const next = draft.trim();
    if (next && next !== name) onRename(next);
  };

  return (
    <div className="group flex items-center gap-1 rounded px-1 py-0.5 hover:bg-card/60">
      <button
        type="button" title={previewOn ? "previewing on — click to hide" : "previewing off — click to show"}
        onClick={onPreview}
        className={cn("shrink-0", previewOn ? "text-emerald-300" : "text-muted-foreground/50 hover:text-muted-foreground")}
      >
        {previewOn ? <Eye className="size-3.5" /> : <EyeOff className="size-3.5" />}
      </button>

      {editing ? (
        <input
          autoFocus value={draft}
          onChange={(e) => setDraft(e.target.value)}
          onBlur={commit}
          onKeyDown={(e) => {
            if (e.key === "Enter") commit();
            else if (e.key === "Escape") { setEditing(false); setDraft(name); }
          }}
          className="h-5 min-w-0 flex-1 rounded border border-sky-500/60 bg-card px-1 font-mono text-[10px] text-foreground outline-none"
        />
      ) : (
        <button
          type="button" onDoubleClick={startEdit} onClick={onPreview} title="double-click to rename"
          className={cn("min-w-0 flex-1 truncate text-left font-mono text-[10px]", previewOn ? "text-foreground" : "text-muted-foreground")}
        >
          {name}
        </button>
      )}

      {used > 0 && <span className="shrink-0 text-[9px] text-muted-foreground/60" title={`tagged on ${used} node${used === 1 ? "" : "s"}`}>·{used}</span>}

      <button
        type="button" title={on ? "default ON at load (click for off)" : "default OFF at load (click for on)"}
        onClick={onDefault}
        className={cn(
          "inline-flex shrink-0 items-center gap-0.5 rounded px-1 py-px text-[9px]",
          on ? "bg-emerald-500/15 text-emerald-300" : "bg-card text-muted-foreground/70 hover:text-muted-foreground",
        )}
      >
        <Power className="size-2.5" />{on ? "on" : "off"}
      </button>

      <button type="button" title="rename" onClick={startEdit}
        className="shrink-0 text-muted-foreground/0 group-hover:text-muted-foreground hover:!text-foreground">
        <Pencil className="size-3" />
      </button>
      <button type="button" title="delete state" onClick={onDelete}
        className="shrink-0 text-muted-foreground/0 group-hover:text-muted-foreground hover:!text-rose-300">
        <Trash2 className="size-3" />
      </button>
    </div>
  );
}

function AddStateRow({ onAdd }: { onAdd: (name: string) => string }) {
  const [val, setVal] = useState("");
  const add = () => { const created = onAdd(val); if (created) setVal(""); };
  return (
    <div className="mt-1.5 flex items-center gap-1">
      <input
        value={val} placeholder="new state…"
        onChange={(e) => setVal(e.target.value)}
        onKeyDown={(e) => { if (e.key === "Enter") add(); }}
        className="h-5 min-w-0 flex-1 rounded border border-border bg-card px-1 font-mono text-[10px] text-foreground outline-none placeholder:text-muted-foreground/50 focus:border-sky-500/60"
      />
      <button type="button" title="add state" onClick={add} disabled={!val.trim()}
        className="inline-flex shrink-0 items-center gap-0.5 rounded border border-border bg-card px-1.5 py-0.5 text-[10px] text-muted-foreground hover:text-foreground disabled:opacity-40">
        <Plus className="size-3" /> add
      </button>
    </div>
  );
}
