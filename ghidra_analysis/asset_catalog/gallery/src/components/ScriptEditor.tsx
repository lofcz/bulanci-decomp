// One Luau script editor (Monaco) for a scene's code-behind.
//
// Loads a pak-relative `.luau` file through the active data source, edits it
// in Monaco with Luau syntax + (when the `luau-lsp` proxy is up) live
// completion/hover/diagnostics, and saves back through the source. The model
// is keyed by the file's on-disk URI so luau-lsp resolves the `.luaurc`
// alias map (`@bulanci`, the lib aliases) exactly as the engine does.

import { useEffect, useRef, useState } from "react";
import type * as Monaco from "monaco-editor";
import { Loader2, Save, Check, AlertTriangle } from "lucide-react";

import { useDataSource } from "@/data/DataSourceContext";
import { loadMonaco, LUAU_LANGUAGE_ID } from "@/lib/monaco";
import { getLuauSession, type LuauSession } from "@/data/luauClient";
import { Button } from "@/components/ui/button";

interface LspInfo { available: boolean; root: string }

let _lspInfoP: Promise<LspInfo | null> | null = null;
function lspInfo(): Promise<LspInfo | null> {
  if (!_lspInfoP) {
    _lspInfoP = fetch("/__lsp/info", { cache: "no-store" })
      .then((r) => (r.ok ? r.json() as Promise<LspInfo> : null))
      .catch(() => null);
  }
  return _lspInfoP;
}

function joinUri(root: string, pak: string, rel: string): string {
  const norm = `${root}/${pak}/${rel}`.replace(/\\/g, "/");
  return norm;
}

export function ScriptEditor({ pak, rel }: { pak: string; rel: string }) {
  const source = useDataSource();
  const hostRef = useRef<HTMLDivElement>(null);
  const editorRef = useRef<Monaco.editor.IStandaloneCodeEditor | null>(null);
  const modelRef = useRef<Monaco.editor.ITextModel | null>(null);
  const sessionRef = useRef<LuauSession | null>(null);
  const uriRef = useRef<string>("");
  const changeTimer = useRef<number | null>(null);

  const [loading, setLoading] = useState(true);
  const [dirty, setDirty] = useState(false);
  const [saving, setSaving] = useState(false);
  const [msg, setMsg] = useState<string | null>(null);
  const [err, setErr] = useState<string | null>(null);
  const [lspOn, setLspOn] = useState(false);

  // Mount Monaco for this (pak, rel). Re-runs when the file changes.
  useEffect(() => {
    let disposed = false;
    setLoading(true); setErr(null); setMsg(null); setDirty(false);

    (async () => {
      try {
        const [monaco, text, info] = await Promise.all([
          loadMonaco(),
          source.readPakText?.(pak, rel) ?? Promise.resolve(""),
          lspInfo(),
        ]);
        if (disposed || !hostRef.current) return;

        const useLsp = !!info?.available && !!info.root;
        const uri = useLsp
          ? monaco.Uri.file(joinUri(info!.root, pak, rel)).toString()
          : `inmemory://script/${pak}/${rel}`;
        uriRef.current = uri;

        const parsed = monaco.Uri.parse(uri);
        // Reuse/replace any stale model for this uri.
        monaco.editor.getModel(parsed)?.dispose();
        const model = monaco.editor.createModel(text ?? "", LUAU_LANGUAGE_ID, parsed);
        modelRef.current = model;

        const editor = monaco.editor.create(hostRef.current, {
          model,
          theme: "vs-dark",
          automaticLayout: true,
          fontSize: 12,
          minimap: { enabled: false },
          scrollBeyondLastLine: false,
          tabSize: 2,
          renderWhitespace: "selection",
        });
        editorRef.current = editor;

        editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyS, () => { void doSave(); });

        // Best-effort LSP attach.
        if (useLsp) {
          const session = await getLuauSession(monaco);
          if (!disposed && session) {
            sessionRef.current = session;
            session.didOpen(uri, model.getValue());
            setLspOn(true);
          }
        }

        model.onDidChangeContent(() => {
          setDirty(true);
          if (changeTimer.current) window.clearTimeout(changeTimer.current);
          changeTimer.current = window.setTimeout(() => {
            sessionRef.current?.didChange(uriRef.current, model.getValue());
          }, 300);
        });

        setLoading(false);
      } catch (e) {
        if (!disposed) { setErr(String((e as Error).message ?? e)); setLoading(false); }
      }
    })();

    return () => {
      disposed = true;
      if (changeTimer.current) window.clearTimeout(changeTimer.current);
      sessionRef.current?.didClose(uriRef.current);
      editorRef.current?.dispose();
      modelRef.current?.dispose();
      editorRef.current = null;
      modelRef.current = null;
    };
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [pak, rel]);

  const doSave = async () => {
    const model = modelRef.current;
    if (!model) return;
    if (!source.writePakText) { setErr("data source is read-only"); return; }
    setSaving(true); setErr(null); setMsg(null);
    try {
      await source.writePakText(pak, rel, model.getValue());
      setDirty(false);
      setMsg("saved");
      window.setTimeout(() => setMsg(null), 2000);
    } catch (e) {
      setErr(String((e as Error).message ?? e));
    } finally {
      setSaving(false);
    }
  };

  return (
    <div className="flex h-full min-h-0 flex-col">
      <div className="flex items-center gap-2 border-b border-border/60 px-2 py-1.5">
        <span className="truncate font-mono text-[11px] text-muted-foreground">{pak} / {rel}</span>
        <span className={`ml-1 rounded px-1 text-[9px] ${lspOn ? "bg-emerald-500/15 text-emerald-300" : "bg-card text-muted-foreground"}`}>
          {lspOn ? "lsp" : "no lsp"}
        </span>
        <div className="ml-auto flex items-center gap-2">
          {msg && <span className="inline-flex items-center gap-1 text-[11px] text-emerald-300"><Check className="size-3" /> {msg}</span>}
          {err && <span className="inline-flex items-center gap-1 truncate text-[11px] text-rose-300"><AlertTriangle className="size-3 shrink-0" /> {err}</span>}
          <Button type="button" size="sm" onClick={() => void doSave()} disabled={saving || !dirty} className="h-6 gap-1.5 text-[11px]">
            {saving ? <Loader2 className="size-3 animate-spin" /> : <Save className="size-3" />} save
          </Button>
        </div>
      </div>
      <div className="relative min-h-0 flex-1">
        {loading && (
          <div className="absolute inset-0 z-10 flex items-center justify-center bg-background/60 text-[12px] text-muted-foreground">
            <Loader2 className="mr-2 size-4 animate-spin" /> loading editor…
          </div>
        )}
        <div ref={hostRef} className="h-full w-full" />
      </div>
    </div>
  );
}
