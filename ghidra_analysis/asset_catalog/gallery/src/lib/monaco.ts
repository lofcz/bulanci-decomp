// Lazy Monaco loader + Luau language registration.
//
// Monaco (~3 MB) is only needed by the Scripts panel, so it's dynamically
// imported on first use and shared thereafter. We register a minimal "luau"
// language (a Lua-flavoured Monarch tokenizer) for syntax highlighting; real
// intelligence (completion/hover/diagnostics) is layered on by `luauClient`
// when the `luau-lsp` proxy is reachable. Only the core editor worker is
// wired up — language-specific workers aren't used since the LSP is external.

import type * as Monaco from "monaco-editor";

export const LUAU_LANGUAGE_ID = "luau";

let monacoP: Promise<typeof Monaco> | null = null;

export function loadMonaco(): Promise<typeof Monaco> {
  if (monacoP) return monacoP;
  monacoP = (async () => {
    (self as unknown as { MonacoEnvironment?: unknown }).MonacoEnvironment = {
      getWorker() {
        return new Worker(
          new URL("monaco-editor/esm/vs/editor/editor.worker.js", import.meta.url),
          { type: "module" },
        );
      },
    };
    const monaco = await import("monaco-editor");
    registerLuau(monaco);
    return monaco;
  })();
  return monacoP;
}

let registered = false;
function registerLuau(monaco: typeof Monaco): void {
  if (registered) return;
  registered = true;

  monaco.languages.register({ id: LUAU_LANGUAGE_ID, extensions: [".luau", ".lua"], aliases: ["Luau", "luau"] });

  monaco.languages.setLanguageConfiguration(LUAU_LANGUAGE_ID, {
    comments: { lineComment: "--", blockComment: ["--[[", "]]"] },
    brackets: [["{", "}"], ["[", "]"], ["(", ")"]],
    autoClosingPairs: [
      { open: "{", close: "}" }, { open: "[", close: "]" }, { open: "(", close: ")" },
      { open: '"', close: '"' }, { open: "'", close: "'" },
    ],
    surroundingPairs: [
      { open: "{", close: "}" }, { open: "[", close: "]" }, { open: "(", close: ")" },
      { open: '"', close: '"' }, { open: "'", close: "'" },
    ],
  });

  monaco.languages.setMonarchTokensProvider(LUAU_LANGUAGE_ID, {
    defaultToken: "",
    keywords: [
      "and", "break", "do", "else", "elseif", "end", "false", "for", "function",
      "if", "in", "local", "nil", "not", "or", "repeat", "return", "then", "true",
      "until", "while", "continue", "export", "type",
    ],
    builtins: ["self", "string", "table", "math", "os", "coroutine", "bit32", "task", "require", "pairs", "ipairs", "next", "select", "tonumber", "tostring", "type", "typeof", "pcall", "xpcall", "error", "assert", "print"],
    tokenizer: {
      root: [
        [/--\[\[/, "comment", "@blockComment"],
        [/--.*$/, "comment"],
        [/[a-zA-Z_]\w*/, { cases: { "@keywords": "keyword", "@builtins": "predefined", "@default": "identifier" } }],
        [/"([^"\\]|\\.)*$/, "string.invalid"],
        [/'([^'\\]|\\.)*$/, "string.invalid"],
        [/"/, "string", "@stringDouble"],
        [/'/, "string", "@stringSingle"],
        [/\[\[/, "string", "@longString"],
        [/\d+\.?\d*([eE][-+]?\d+)?/, "number"],
        [/0[xX][0-9a-fA-F]+/, "number.hex"],
        [/[{}()[\]]/, "@brackets"],
        [/[<>!=]=|[-+*/%^#<>=]/, "operator"],
      ],
      blockComment: [
        [/]]/, "comment", "@pop"],
        [/./, "comment"],
      ],
      stringDouble: [
        [/[^"\\]+/, "string"],
        [/\\./, "string.escape"],
        [/"/, "string", "@pop"],
      ],
      stringSingle: [
        [/[^'\\]+/, "string"],
        [/\\./, "string.escape"],
        [/'/, "string", "@pop"],
      ],
      longString: [
        [/]]/, "string", "@pop"],
        [/./, "string"],
      ],
    },
  });
}
