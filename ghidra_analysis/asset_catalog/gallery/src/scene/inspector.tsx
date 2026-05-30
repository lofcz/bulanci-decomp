// Property inspector — the galacean editor-ui "side panel with properties".
//
// Renders the selected control's fields from the engine's reflected registry
// (`control-registry.json`), so the editable props never drift from the Rust
// control model. Controls are galacean editor-ui (Collapsible sections,
// InputNumber, Input, Select, SegmentControl) themed by the root ThemeProvider;
// every edit is funnelled through the store's `updateSelected` so it is a
// single undoable step.

import { Plus } from "lucide-react";
import {
  Collapsible, InputNumber, Input, Select, SelectItem,
  SegmentControl, SegmentControlItem,
} from "@galacean/editor-ui";

import type { Binding, FlatNode, ControlRegistry, Layout } from "@/scene/model";

// Sentinel for the "(always visible)" option — galacean Select round-trips a
// real string cleanly, so we avoid binding the empty string as a value.
const STATE_NONE = "__always__";

// --------------------------------------------------------------- field rows

function Row({ label, children }: { label: string; children: React.ReactNode }) {
  return (
    <label className="flex items-center justify-between gap-2 py-0.5 text-[11px]">
      <span className="text-muted-foreground">{label}</span>
      <div className="w-40 shrink-0">{children}</div>
    </label>
  );
}

function NumField({ label, value, step = 1, onChange }: {
  label: string; value: number; step?: number; onChange: (n: number) => void;
}) {
  return (
    <Row label={label}>
      <InputNumber size="sm" value={Number.isFinite(value) ? value : 0} step={step}
        onValueChange={(v: number) => onChange(Number.isFinite(v) ? v : 0)} />
    </Row>
  );
}

function OptNumField({ label, value, onChange }: {
  label: string; value: number | null; onChange: (n: number | null) => void;
}) {
  return (
    <Row label={label}>
      <Input size="sm" value={value == null ? "" : String(value)} placeholder="auto"
        onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
          const t = e.target.value.trim();
          onChange(t === "" ? null : (parseFloat(t) || 0));
        }} />
    </Row>
  );
}

function TextField({ label, value, onChange, list, placeholder }: {
  label: string; value: string; onChange: (s: string) => void; list?: string; placeholder?: string;
}) {
  return (
    <Row label={label}>
      <Input size="sm" value={value} list={list} placeholder={placeholder}
        onChange={(e: React.ChangeEvent<HTMLInputElement>) => onChange(e.target.value)} />
    </Row>
  );
}

function SelectField({ label, value, options, onChange }: {
  label: string; value: string; options: string[]; onChange: (s: string) => void;
}) {
  return (
    <Row label={label}>
      <Select size="sm" value={value} valueType="string" onValueChange={(v: string) => onChange(v)}>
        {options.map((o) => <SelectItem key={o} value={o}>{o}</SelectItem>)}
      </Select>
    </Row>
  );
}

/** Color field: hex8 text + a native swatch (galacean's ColorPicker uses a
 *  richer Color model than our `rrggbbaa` strings, so a swatch keeps the data
 *  model intact while still being clickable). */
function ColorField({ label, value, onChange }: { label: string; value: string; onChange: (s: string) => void }) {
  const hex6 = "#" + (value || "ffffffff").replace(/[^0-9a-f]/gi, "").padEnd(6, "f").slice(0, 6);
  return (
    <Row label={label}>
      <div className="flex items-center gap-1.5">
        <input type="color" value={hex6}
          onChange={(e) => onChange(e.target.value.slice(1) + (value.slice(6) || "ff"))}
          className="h-5 w-6 shrink-0 cursor-pointer rounded border border-border bg-transparent p-0" />
        <Input size="sm" value={value} placeholder="rrggbbaa"
          onChange={(e: React.ChangeEvent<HTMLInputElement>) => onChange(e.target.value)} />
      </div>
    </Row>
  );
}

/** Node visibility-state picker: a dropdown of the scene's declared states
 *  (plus "always") and a "+" to declare a new one and assign it in one go. */
function StateField({ value, states, onChange, onAddState }: {
  value: string; states: string[];
  onChange: (s: string) => void; onAddState: (name: string) => string;
}) {
  const missing = value !== "" && !states.includes(value);
  return (
    <Row label="state">
      <div className="flex items-center gap-1">
        <Select size="sm" value={value === "" ? STATE_NONE : value} valueType="string"
          onValueChange={(v: string) => onChange(v === STATE_NONE ? "" : v)}>
          <SelectItem value={STATE_NONE}>(always)</SelectItem>
          {states.map((s) => <SelectItem key={s} value={s}>{s}</SelectItem>)}
          {missing && <SelectItem value={value}>{value} (undeclared)</SelectItem>}
        </Select>
        <button type="button" title="declare new state and assign it"
          onClick={() => {
            const raw = window.prompt("New state name (e.g. sg1):");
            if (raw == null) return;
            const created = onAddState(raw);
            if (created) onChange(created);
          }}
          className="inline-flex size-6 shrink-0 items-center justify-center rounded border border-border bg-card text-muted-foreground hover:text-foreground">
          <Plus className="size-3" />
        </button>
      </div>
    </Row>
  );
}

// --------------------------------------------------------------- inspector

export function Inspector({
  node, registry, assetList, states, onAddState, onChange,
}: {
  node: FlatNode; registry: ControlRegistry; assetList: string;
  states: string[]; onAddState: (name: string) => string;
  onChange: (fn: (n: FlatNode) => FlatNode) => void;
}) {
  const set = (patch: Partial<FlatNode>) => onChange((n) => ({ ...n, ...patch }));
  const setLayout = (patch: Partial<Layout>) => onChange((n) => ({ ...n, layout: { ...n.layout, ...patch } }));
  const kindFields = registry.kinds[node.kind] ?? [];
  const flex = node.layout.mode === "flex";
  const absolute = node.layout.mode !== "flex" && node.layout.mode !== "block";

  return (
    <div className="flex flex-col gap-1.5">
      <Collapsible defaultOpen title={<SectionTitle>{node.kind} · common</SectionTitle>}>
        <TextField label="name" value={node.name} onChange={(name) => set({ name })} />
        <StateField value={node.state} states={states} onAddState={onAddState} onChange={(state) => set({ state })} />
        <ColorField label="tint" value={node.tint} onChange={(tint) => set({ tint })} />
        <NumField label="scale x" value={node.sx} step={0.1} onChange={(sx) => set({ sx })} />
        <NumField label="scale y" value={node.sy} step={0.1} onChange={(sy) => set({ sy })} />
        <NumField label="rotation" value={node.rot} onChange={(rot) => set({ rot })} />
        {node.kind === "sprite" && (
          <NumField label="preview frame" value={node.previewFrame} onChange={(previewFrame) => set({ previewFrame })} />
        )}
      </Collapsible>

      <Collapsible defaultOpen title={<SectionTitle>layout</SectionTitle>}>
        <Row label="mode">
          <SegmentControl size="sm" value={node.layout.mode}
            onValueChange={(mode: string) => mode && setLayout({ mode })}>
            <SegmentControlItem value="absolute">abs</SegmentControlItem>
            <SegmentControlItem value="flex">flex</SegmentControlItem>
            <SegmentControlItem value="block">block</SegmentControlItem>
          </SegmentControl>
        </Row>
        {absolute && (
          <>
            <NumField label="x" value={node.layout.x} onChange={(x) => setLayout({ x })} />
            <NumField label="y" value={node.layout.y} onChange={(y) => setLayout({ y })} />
          </>
        )}
        <OptNumField label="width" value={node.layout.width} onChange={(width) => setLayout({ width })} />
        <OptNumField label="height" value={node.layout.height} onChange={(height) => setLayout({ height })} />
        {flex && (
          <>
            <SelectField label="direction" value={node.layout.direction} options={["row", "column"]} onChange={(direction) => setLayout({ direction })} />
            <SelectField label="justify" value={node.layout.justify} options={["start", "center", "end", "between", "around", "evenly", "stretch"]} onChange={(justify) => setLayout({ justify })} />
            <SelectField label="align" value={node.layout.align} options={["start", "center", "end", "stretch", "baseline"]} onChange={(align) => setLayout({ align })} />
            <NumField label="gap" value={node.layout.gap} onChange={(gap) => setLayout({ gap })} />
            <NumField label="padding" value={node.layout.padding} onChange={(padding) => setLayout({ padding })} />
          </>
        )}
      </Collapsible>

      {kindFields.length > 0 && (
        <Collapsible defaultOpen title={<SectionTitle>{node.kind} props</SectionTitle>}>
          {kindFields.includes("handle") && <TextField label="handle" value={node.handle} onChange={(handle) => set({ handle })} list={assetList} placeholder="folder/slug" />}
          {kindFields.includes("frame") && <NumField label="frame" value={node.frame} onChange={(frame) => set({ frame })} />}
          {kindFields.includes("text") && <TextField label="text" value={node.text} onChange={(text) => set({ text })} />}
          {kindFields.includes("label") && <TextField label="label" value={node.label} onChange={(label) => set({ label })} />}
          {kindFields.includes("size") && <NumField label="size" value={node.size} onChange={(size) => set({ size })} />}
          {kindFields.includes("spacing") && <NumField label="spacing" value={node.spacing} onChange={(spacing) => set({ spacing })} />}
          {kindFields.includes("anchor") && <SelectField label="anchor" value={node.anchor} options={["baseline", "top"]} onChange={(anchor) => set({ anchor })} />}
          {kindFields.includes("color") && <ColorField label="color" value={node.color} onChange={(color) => set({ color })} />}
        </Collapsible>
      )}

      <Collapsible defaultOpen title={<SectionTitle>bindings</SectionTitle>}>
        <BindingsEditor node={node} bindable={registry.bindable} onChange={onChange} />
      </Collapsible>
    </div>
  );
}

function SectionTitle({ children }: { children: React.ReactNode }) {
  return <span className="text-[10px] font-bold uppercase tracking-[0.08em] text-muted-foreground">{children}</span>;
}

function BindingsEditor({ node, bindable, onChange }: {
  node: FlatNode; bindable: string[]; onChange: (fn: (n: FlatNode) => FlatNode) => void;
}) {
  const set = (binds: Binding[]) => onChange((n) => ({ ...n, bindings: binds }));
  return (
    <div className="flex flex-col gap-1 pt-1">
      {node.bindings.map((b, i) => (
        <div key={i} className="flex items-center gap-1 text-[10px]">
          <div className="w-24 shrink-0">
            <Select size="xs" value={b.prop} valueType="string" placeholder="prop"
              onValueChange={(prop: string) => set(node.bindings.map((x, j) => j === i ? { ...x, prop } : x))}>
              {bindable.map((p) => <SelectItem key={p} value={p}>{p}</SelectItem>)}
            </Select>
          </div>
          <span className="text-muted-foreground">←</span>
          <Input size="xs" value={b.key} placeholder="bind key"
            onChange={(e: React.ChangeEvent<HTMLInputElement>) =>
              set(node.bindings.map((x, j) => j === i ? { ...x, key: e.target.value } : x))} />
          <button type="button" className="text-rose-300 hover:text-rose-200"
            onClick={() => set(node.bindings.filter((_, j) => j !== i))}>×</button>
        </div>
      ))}
      <button type="button"
        onClick={() => set([...node.bindings, { prop: bindable[0] ?? "visible", key: "" }])}
        className="mt-0.5 inline-flex w-fit items-center gap-1 rounded bg-card px-1.5 py-0.5 text-[10px] text-muted-foreground hover:text-foreground">
        + binding
      </button>
    </div>
  );
}
