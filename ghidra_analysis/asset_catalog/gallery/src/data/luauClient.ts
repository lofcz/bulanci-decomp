// Lightweight Luau LSP client.
//
// Connects the Scripts panel's Monaco editors to `luau-lsp` through the dev
// server's `/__lsp` WebSocket proxy (see gallery vite.config.ts) without the
// heavyweight `monaco-languageclient` + `monaco-vscode-api` takeover: we open
// a raw JSON-RPC connection and bridge just what the panel needs —
// completion, hover, and diagnostics — to Monaco providers. Everything is
// best-effort: if `luau-lsp` isn't installed (or the proxy is unreachable, or
// a request errors) the editor degrades to plain syntax highlighting.

import type * as Monaco from "monaco-editor";
import { toSocket, WebSocketMessageReader, WebSocketMessageWriter } from "vscode-ws-jsonrpc";
import { createMessageConnection, type MessageConnection } from "vscode-jsonrpc";

import { LUAU_LANGUAGE_ID } from "@/lib/monaco";

const LSP_WS_PATH = "/__lsp";
const LSP_INFO_URL = "/__lsp/info";

interface LspInfo { available: boolean; root: string }

let sessionP: Promise<LuauSession | null> | null = null;

/** Resolve (and cache) the shared session. Returns null when no server is
 *  reachable — callers should treat the editor as LSP-less in that case. */
export function getLuauSession(monaco: typeof Monaco): Promise<LuauSession | null> {
  if (!sessionP) {
    sessionP = (async () => {
      let info: LspInfo | null = null;
      try {
        const r = await fetch(LSP_INFO_URL, { cache: "no-store" });
        if (r.ok) info = await r.json() as LspInfo;
      } catch { /* no proxy (static host / FS source) */ }
      if (!info?.available) return null;
      try {
        const s = new LuauSession(monaco);
        await s.start();
        return s;
      } catch (e) {
        console.warn("[luau-lsp] session failed to start:", e);
        return null;
      }
    })();
  }
  return sessionP;
}

export class LuauSession {
  private conn!: MessageConnection;
  private readonly open = new Map<string, number>(); // uri -> version
  private providersBound = false;

  constructor(private monaco: typeof Monaco) {}

  async start(): Promise<void> {
    const proto = location.protocol === "https:" ? "wss" : "ws";
    const url = `${proto}://${location.host}${LSP_WS_PATH}`;
    const ws = new WebSocket(url);
    await new Promise<void>((resolve, reject) => {
      ws.onopen = () => resolve();
      ws.onerror = () => reject(new Error("ws error"));
    });
    const socket = toSocket(ws);
    const reader = new WebSocketMessageReader(socket);
    const writer = new WebSocketMessageWriter(socket);
    this.conn = createMessageConnection(reader, writer);

    this.conn.onNotification("textDocument/publishDiagnostics", (p: PublishDiagnostics) => this.onDiagnostics(p));
    // luau-lsp asks the client for config; answer with an empty set so it
    // falls back to its defaults rather than stalling.
    this.conn.onRequest("workspace/configuration", (p: { items: unknown[] }) => p.items.map(() => ({})));
    this.conn.onRequest("client/registerCapability", () => null);
    this.conn.listen();

    await this.conn.sendRequest("initialize", {
      processId: null,
      clientInfo: { name: "bulanci-scene-editor" },
      rootUri: null,
      capabilities: {
        textDocument: {
          synchronization: { dynamicRegistration: false },
          completion: { completionItem: { snippetSupport: true, documentationFormat: ["markdown", "plaintext"] } },
          hover: { contentFormat: ["markdown", "plaintext"] },
          publishDiagnostics: {},
        },
      },
    });
    this.conn.sendNotification("initialized", {});
    this.bindProviders();
  }

  /** Tell the server a document is open (or re-open after a model swap). */
  didOpen(uri: string, text: string): void {
    this.open.set(uri, 1);
    this.conn.sendNotification("textDocument/didOpen", {
      textDocument: { uri, languageId: "luau", version: 1, text },
    });
  }

  didChange(uri: string, text: string): void {
    const version = (this.open.get(uri) ?? 0) + 1;
    this.open.set(uri, version);
    this.conn.sendNotification("textDocument/didChange", {
      textDocument: { uri, version },
      contentChanges: [{ text }],
    });
  }

  didClose(uri: string): void {
    if (!this.open.has(uri)) return;
    this.open.delete(uri);
    this.conn.sendNotification("textDocument/didClose", { textDocument: { uri } });
  }

  // ---- Monaco providers (registered once for the luau language) ----------

  private bindProviders(): void {
    if (this.providersBound) return;
    this.providersBound = true;
    const m = this.monaco;

    m.languages.registerCompletionItemProvider(LUAU_LANGUAGE_ID, {
      triggerCharacters: [".", ":", "(", "\"", "'", "/", "@"],
      provideCompletionItems: async (model, position) => {
        try {
          const res = await this.conn.sendRequest("textDocument/completion", {
            textDocument: { uri: model.uri.toString() },
            position: { line: position.lineNumber - 1, character: position.column - 1 },
          }) as LspCompletion | LspCompletionItem[] | null;
          const items = Array.isArray(res) ? res : (res?.items ?? []);
          const word = model.getWordUntilPosition(position);
          const range = new m.Range(position.lineNumber, word.startColumn, position.lineNumber, word.endColumn);
          return {
            suggestions: items.map((it) => ({
              label: it.label,
              kind: mapCompletionKind(m, it.kind),
              insertText: it.insertText ?? it.label,
              insertTextRules: it.insertTextFormat === 2 ? m.languages.CompletionItemInsertTextRule.InsertAsSnippet : undefined,
              detail: it.detail,
              documentation: docToMarkdown(it.documentation),
              range,
            })),
          };
        } catch { return { suggestions: [] }; }
      },
    });

    m.languages.registerHoverProvider(LUAU_LANGUAGE_ID, {
      provideHover: async (model, position) => {
        try {
          const res = await this.conn.sendRequest("textDocument/hover", {
            textDocument: { uri: model.uri.toString() },
            position: { line: position.lineNumber - 1, character: position.column - 1 },
          }) as LspHover | null;
          if (!res?.contents) return null;
          const value = hoverToString(res.contents);
          if (!value) return null;
          return { contents: [{ value }] };
        } catch { return null; }
      },
    });
  }

  private onDiagnostics(p: PublishDiagnostics): void {
    const model = this.monaco.editor.getModel(this.monaco.Uri.parse(p.uri));
    if (!model) return;
    const markers: Monaco.editor.IMarkerData[] = (p.diagnostics ?? []).map((d) => ({
      severity: mapSeverity(this.monaco, d.severity),
      message: d.message,
      startLineNumber: d.range.start.line + 1,
      startColumn: d.range.start.character + 1,
      endLineNumber: d.range.end.line + 1,
      endColumn: d.range.end.character + 1,
      source: d.source ?? "luau",
    }));
    this.monaco.editor.setModelMarkers(model, "luau-lsp", markers);
  }
}

// ---- LSP <-> Monaco mappings ---------------------------------------------

interface LspCompletionItem {
  label: string;
  kind?: number;
  insertText?: string;
  insertTextFormat?: number;
  detail?: string;
  documentation?: string | { value: string };
}
interface LspCompletion { items: LspCompletionItem[] }
interface LspHover { contents: string | { value: string } | Array<string | { value: string }> }
interface LspDiagnostic {
  range: { start: { line: number; character: number }; end: { line: number; character: number } };
  message: string;
  severity?: number;
  source?: string;
}
interface PublishDiagnostics { uri: string; diagnostics: LspDiagnostic[] }

function mapCompletionKind(m: typeof Monaco, kind?: number): Monaco.languages.CompletionItemKind {
  const K = m.languages.CompletionItemKind;
  // LSP CompletionItemKind -> Monaco (subset; defaults to Text).
  const table: Record<number, Monaco.languages.CompletionItemKind> = {
    2: K.Method, 3: K.Function, 4: K.Constructor, 5: K.Field, 6: K.Variable,
    7: K.Class, 8: K.Interface, 9: K.Module, 10: K.Property, 14: K.Keyword,
    15: K.Snippet, 21: K.Constant, 22: K.Struct, 25: K.TypeParameter,
  };
  return (kind != null && table[kind]) || K.Text;
}

function mapSeverity(m: typeof Monaco, sev?: number): Monaco.MarkerSeverity {
  const S = m.MarkerSeverity;
  switch (sev) {
    case 1: return S.Error;
    case 2: return S.Warning;
    case 3: return S.Info;
    case 4: return S.Hint;
    default: return S.Error;
  }
}

function docToMarkdown(doc?: string | { value: string }): { value: string } | undefined {
  if (!doc) return undefined;
  return { value: typeof doc === "string" ? doc : doc.value };
}

function hoverToString(contents: LspHover["contents"]): string {
  if (typeof contents === "string") return contents;
  if (Array.isArray(contents)) return contents.map((c) => (typeof c === "string" ? c : c.value)).join("\n\n");
  return contents.value;
}
