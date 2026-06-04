#!/usr/bin/env python3
"""Generate goto-free **Luau** from a curated Bulanci ``*.script.asm``.

Pipeline (one unified path)::

    res_XXXX_Script.bin  --(bulanci_unpack disassembler)-->  *.script.asm
    *.script.asm         --(this module)-->                   *.lua  (Luau)

Why parse the ``.asm`` (and not the raw bytecode)?  The ``.asm`` is the
*human source of truth*: it carries the curated semantic function names
(``place_bunny_on_log(x, y)``) and the docstrings that only exist there.
Generating from it means an analyst editing the ``.asm`` (renaming a helper,
fixing a comment) directly shapes the emitted Luau, and there is a single
decode path instead of two divergent ones.

Luau has **no ``goto``** (it is Lua 5.1 + extensions; ``goto`` arrived in Lua
5.2 and Luau deliberately omits it).  An earlier bytecode->Lua transpiler
emitted ``goto``/labels, so its output could never compile under ``mlua``'s
Luau compiler.  This module instead reconstructs structured control flow
(``if/elseif/else`` + ``while``/``break``/``continue`` + early ``return``)
from the per-function control-flow graph, so the result is valid Luau.

The module is split into stages, each independently testable:

  1. ``parse_asm``      : ``.asm`` text  -> list[Function] (IR + raw instrs)
  2. ``build_cfg``      : Function       -> CFG (basic blocks, reachable only)
  3. ``structure``      : CFG            -> structured AST (no goto)   [stage 2]
  4. ``emit_luau``      : AST            -> Luau source                [stage 2]

Run ``python asm_to_luau.py --self-test`` to parse+CFG every script under
``unpacked/`` and report anomalies (unparsed reachable instrs, dangling
branch targets, detected loops).
"""

from __future__ import annotations

import json
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional, Union

REPO = Path(__file__).resolve().parents[2]

# Comparison opcodes: name -> Lua operator. ``IfX(a, b, target)`` means
# "if a <op> b then goto target; else fall through".
IF_OPS = {
    "IfEqual": "==",
    "IfLess": "<",
    "IfGreater": ">",
    "IfLessEq": "<=",
    "IfGreaterEq": ">=",
    "IfNotEqual": "~=",
}

# Export index -> lifecycle role name (matches bulanci_unpack export table).
EXPORT_NAMES = {
    0: "GetInfo", 1: "OnInit", 2: "OnDeinit", 3: "OnBitmapEvt",
    4: "OnSlotPlaced", 5: "OnSlotDisplaced", 6: "OnTimer", 7: "OnEnter",
    8: "OnLeave", 9: "OnNetCustom", 10: "OnGameStart",
}


# --------------------------------------------------------------------------
# Expression IR
# --------------------------------------------------------------------------
@dataclass
class Num:
    value: int

@dataclass
class Str:
    value: str

@dataclass
class FnRef:
    addr: int

@dataclass
class Op:
    """A generic opcode expression, e.g. ``CreateImage(x, y, 65643)``."""
    name: str
    args: list

@dataclass
class Anim:
    """``CreateAnim(x, y, delay=D, frames=[...])`` (opcode 45)."""
    x: object
    y: object
    delay: int
    frames: list

@dataclass
class Sw:
    """``Switch(scrut; v0=>L0, ...)`` (opcode 24). Only appears top-level."""
    name: str
    scrut: object
    cases: list  # list[(value:int, target:int)]


Expr = Union[Num, Str, FnRef, Op, Anim, Sw]


# --------------------------------------------------------------------------
# Tokenizer for a single command string
# --------------------------------------------------------------------------
_TOKEN_RE = re.compile(
    r"""
      (?P<WS>\s+)
    | (?P<STRING>"(?:\\.|[^"\\])*")
    | (?P<FNREF>fn@0x[0-9A-Fa-f]+)
    | (?P<ARROW>=>)
    | (?P<NUMBER>-?0x[0-9A-Fa-f]+|-?\d+)
    | (?P<IDENT>[A-Za-z_][A-Za-z0-9_]*)
    | (?P<PUNCT>[()\[\],;=])
    """,
    re.VERBOSE,
)


@dataclass
class Tok:
    kind: str
    text: str


def tokenize(s: str) -> list:
    out = []
    pos = 0
    while pos < len(s):
        m = _TOKEN_RE.match(s, pos)
        if not m:
            raise ValueError(f"tokenize: junk at {pos}: {s[pos:pos+20]!r} (in {s!r})")
        pos = m.end()
        kind = m.lastgroup
        if kind == "WS":
            continue
        out.append(Tok(kind, m.group()))
    return out


class _P:
    def __init__(self, toks):
        self.toks = toks
        self.i = 0

    def peek(self) -> Optional[Tok]:
        return self.toks[self.i] if self.i < len(self.toks) else None

    def next(self) -> Tok:
        t = self.toks[self.i]
        self.i += 1
        return t

    def expect(self, text: str) -> Tok:
        t = self.peek()
        if t is None or t.text != text:
            raise ValueError(f"expected {text!r}, got {t}")
        return self.next()


def _num(t: Tok) -> int:
    return int(t.text, 16) if t.text.lower().startswith(("0x", "-0x")) else int(t.text)


def strip_inline_comment(cmd: str) -> str:
    """Drop a trailing ``  ; ...`` annotation the disassembler appends to some
    instructions (e.g. ``CreateImage(...)  ; res 0x10030 <slug>``).

    Only a ``;`` at paren/bracket depth 0 and outside a string literal starts
    the comment -- the ``;`` inside ``Switch(scrut; cases)`` lives at depth >= 1
    and string contents are skipped.
    """
    depth = 0
    in_str = False
    esc = False
    for i, ch in enumerate(cmd):
        if in_str:
            if esc:
                esc = False
            elif ch == "\\":
                esc = True
            elif ch == '"':
                in_str = False
            continue
        if ch == '"':
            in_str = True
        elif ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        elif ch == ";" and depth == 0:
            return cmd[:i].rstrip()
    return cmd.rstrip()


def parse_command(s: str) -> Expr:
    """Parse one disassembly command string into an Expr."""
    p = _P(tokenize(s))
    node = parse_expr(p)
    if p.peek() is not None:
        raise ValueError(f"trailing tokens after {node}: {p.peek()} (in {s!r})")
    return node


def parse_expr(p: _P) -> Expr:
    t = p.peek()
    if t is None:
        raise ValueError("unexpected end of command")
    if t.kind == "NUMBER":
        p.next()
        return Num(_num(t))
    if t.kind == "STRING":
        p.next()
        return Str(json.loads(t.text))
    if t.kind == "FNREF":
        p.next()
        return FnRef(int(t.text[len("fn@"):], 16))
    if t.kind == "IDENT":
        name = t.text
        p.next()
        if p.peek() is not None and p.peek().text == "(":
            return parse_call(p, name)
        return Op(name, [])
    raise ValueError(f"unexpected token {t}")


def parse_call(p: _P, name: str) -> Expr:
    p.expect("(")
    if name in ("Switch", "Select"):
        scrut = parse_expr(p)
        cases = []
        if p.peek() is not None and p.peek().text == ";":
            p.next()
            while p.peek() is not None and p.peek().text != ")":
                val = _num(p.next())
                p.expect("=>")
                lbl = _num(p.next())
                cases.append((val, lbl))
                if p.peek() is not None and p.peek().text == ",":
                    p.next()
        p.expect(")")
        return Sw(name, scrut, cases)
    if name == "CreateAnim":
        x = parse_expr(p); p.expect(",")
        y = parse_expr(p); p.expect(",")
        p.expect("delay"); p.expect("="); delay = _num(p.next()); p.expect(",")
        p.expect("frames"); p.expect("="); p.expect("[")
        frames = []
        while p.peek() is not None and p.peek().text != "]":
            frames.append(_num(p.next()))
            if p.peek() is not None and p.peek().text == ",":
                p.next()
        p.expect("]"); p.expect(")")
        return Anim(x, y, delay, frames)
    # Generic call. Tolerate the help/history "count=N, [..]" textstyle shape
    # by accepting bare ``ident=`` kwargs and ``[ .. ]`` lists as plain args.
    args = []
    while p.peek() is not None and p.peek().text != ")":
        args.append(parse_arg(p))
        if p.peek() is not None and p.peek().text == ",":
            p.next()
    p.expect(")")
    return Op(name, args)


def parse_arg(p: _P) -> Expr:
    t = p.peek()
    # kwarg "name=expr" (textstyle count=) -> keep only the value
    if t is not None and t.kind == "IDENT":
        nxt = p.toks[p.i + 1] if p.i + 1 < len(p.toks) else None
        if nxt is not None and nxt.text == "=":
            p.next(); p.next()
            return parse_arg(p)
    if t is not None and t.text == "[":
        p.next()
        items = []
        while p.peek() is not None and p.peek().text != "]":
            items.append(parse_expr(p))
            if p.peek() is not None and p.peek().text == ",":
                p.next()
        p.expect("]")
        return Op("__list__", items)
    return parse_expr(p)


# --------------------------------------------------------------------------
# .asm parsing -> Function IR
# --------------------------------------------------------------------------
@dataclass
class Instr:
    off: int
    raw: str                 # the original command text (for diagnostics)
    node: Optional[Expr]     # parsed expr, or None if it failed to parse
    err: Optional[str] = None


@dataclass
class Function:
    start: int
    is_export: bool
    export_index: Optional[int]
    name: str                # emitted Lua name (role for exports, curated/gen otherwise)
    params: list             # parameter names (from curated signature, may be derived)
    doc: str                 # docstring (from .asm comment)
    var_count: int
    instrs: list = field(default_factory=list)  # list[Instr], offset-sorted


_HDR_RE = re.compile(
    r"^fn (?:export#(?P<idx>\d+) )?@ (?P<addr>0x[0-9A-Fa-f]+)"
    r"(?::|  ; (?P<info>.*))?\s*$"
)
_SIG_RE = re.compile(r"^(?P<name>[A-Za-z_]\w*)\((?P<params>[^)]*)\)\s*(?:--\s*(?P<doc>.*))?$")
_INSTR_RE = re.compile(r"^\s+@(?P<addr>0x[0-9A-Fa-f]+)\s+(?P<cmd>.*)$")
_VARCOUNT_RE = re.compile(r"^\s*;\s*varCount=(?P<n>\d+)\s*$")


def _parse_header(line: str):
    m = _HDR_RE.match(line)
    if not m:
        return None
    addr = int(m.group("addr"), 16)
    idx = m.group("idx")
    idx = int(idx) if idx is not None else None
    info = (m.group("info") or "").strip()
    name = None
    params = None
    doc = ""
    sig = _SIG_RE.match(info) if info else None
    if sig:
        name = sig.group("name")
        params = [p.strip() for p in sig.group("params").split(",") if p.strip()]
        doc = (sig.group("doc") or "").strip()
    elif info:
        doc = info  # free-form comment (e.g. "helper (call-target)")
    return addr, idx, name, params, doc


def parse_asm(text: str) -> list:
    """Parse ``.asm`` text into a list of :class:`Function` (offset-bounded)."""
    lines = text.splitlines()

    # Pass 1: locate every function header (addr + metadata + body line span).
    headers = []  # (addr, idx, name, params, doc, body_line_start)
    for li, line in enumerate(lines):
        h = _parse_header(line)
        if h is not None:
            headers.append((*h, li))

    entry_addrs = sorted({h[0] for h in headers})
    INF = 1 << 30

    def next_start(addr: int) -> int:
        for a in entry_addrs:
            if a > addr:
                return a
        return INF

    funcs = []
    for hi, (addr, idx, name, params, doc, line0) in enumerate(headers):
        bound = next_start(addr)
        line_end = headers[hi + 1][5] if hi + 1 < len(headers) else len(lines)
        var_count = 0
        instrs = []
        for line in lines[line0 + 1:line_end]:
            mv = _VARCOUNT_RE.match(line)
            if mv:
                var_count = int(mv.group("n"))
                continue
            mi = _INSTR_RE.match(line)
            if not mi:
                continue  # blank / unreachable-tail comment / etc.
            ioff = int(mi.group("addr"), 16)
            if ioff >= bound:
                continue  # over-walked tail belonging to the next function
            cmd = strip_inline_comment(mi.group("cmd").strip())
            try:
                node = parse_command(cmd)
                instrs.append(Instr(ioff, cmd, node))
            except Exception as exc:  # noqa: BLE001 - defer; may be unreachable
                instrs.append(Instr(ioff, cmd, None, str(exc)))
        instrs.sort(key=lambda x: x.off)

        is_export = idx is not None
        if is_export:
            disp = EXPORT_NAMES.get(idx, f"export_{idx}")
        elif name:
            disp = name
        else:
            disp = f"fn_0x{addr:04x}"
        funcs.append(Function(
            start=addr, is_export=is_export, export_index=idx, name=disp,
            params=params or [], doc=doc, var_count=var_count, instrs=instrs,
        ))
    return funcs


# --------------------------------------------------------------------------
# Instruction classification + CFG
# --------------------------------------------------------------------------
def classify(node: Expr):
    """Return a terminator/stmt tuple for a top-level instruction node."""
    if isinstance(node, Sw):
        return ("switch", node.scrut, node.cases)
    if isinstance(node, Op):
        if node.name == "Goto":
            return ("goto", node.args[0].value)
        if node.name == "Return":
            return ("return", node.args[0] if node.args else Num(0))
        if node.name in IF_OPS:
            return ("if", node.name, node.args[0], node.args[1], node.args[2].value)
    return ("stmt", node)


def is_terminator_node(node: Expr) -> bool:
    if isinstance(node, Sw):
        return True
    if isinstance(node, Op) and (node.name in IF_OPS or node.name in ("Goto", "Return")):
        return True
    return False


@dataclass
class Block:
    off: int                      # leader offset
    stmts: list = field(default_factory=list)   # list[Expr] side-effecting
    term: tuple = ("fallthrough", None)          # terminator descriptor
    succs: list = field(default_factory=list)    # list[(label, target_off)]


@dataclass
class CFG:
    func: Function
    entry: int
    blocks: dict                  # off -> Block (reachable only)
    order: list                   # reachable offsets in original order


def build_cfg(func: Function) -> CFG:
    instrs = func.instrs
    if not instrs:
        b = Block(off=func.start + 1, term=("return", Num(0)))
        return CFG(func, b.off, {b.off: b}, [b.off])

    offs = [ins.off for ins in instrs]
    off_set = set(offs)
    by_off = {ins.off: ins for ins in instrs}
    entry = offs[0]

    # ---- find leaders ----
    leaders = {entry}
    for k, ins in enumerate(instrs):
        if ins.node is None:
            continue
        cl = classify(ins.node)
        kind = cl[0]
        nxt = offs[k + 1] if k + 1 < len(offs) else None
        if kind == "if":
            tgt = cl[4]
            if tgt in off_set:
                leaders.add(tgt)
            if nxt is not None:
                leaders.add(nxt)
        elif kind == "goto":
            tgt = cl[1]
            if tgt in off_set:
                leaders.add(tgt)
            if nxt is not None:
                leaders.add(nxt)
        elif kind == "switch":
            for _v, t in cl[2]:
                if t in off_set:
                    leaders.add(t)
            if nxt is not None:
                leaders.add(nxt)
        elif kind == "return":
            if nxt is not None:
                leaders.add(nxt)

    # ---- build blocks ----
    blocks: dict = {}
    cur: Optional[Block] = None
    for k, ins in enumerate(instrs):
        if ins.off in leaders:
            cur = Block(off=ins.off)
            blocks[ins.off] = cur
        nxt = offs[k + 1] if k + 1 < len(offs) else None
        node = ins.node
        cl = classify(node) if node is not None else ("stmt", node)
        kind = cl[0]
        if kind == "stmt":
            cur.stmts.append(node)
            # if the next instr is a leader, this block falls through
            if nxt is not None and nxt in leaders:
                cur.term = ("fallthrough", nxt)
                cur.succs = [("", nxt)]
        elif kind == "return":
            cur.term = ("return", cl[1])
            cur.succs = []
        elif kind == "goto":
            cur.term = ("goto", cl[1])
            cur.succs = [("", cl[1])]
        elif kind == "if":
            _, opname, a, b, tgt = cl
            cur.term = ("if", opname, a, b, tgt, nxt)
            cur.succs = [("then", tgt), ("else", nxt)]
        elif kind == "switch":
            _, scrut, cases = cl
            cur.term = ("switch", scrut, cases, nxt)
            cur.succs = [("case", t) for _v, t in cases] + [("default", nxt)]
        # If we appended a stmt and the next instr is NOT a leader, the block
        # continues; nothing to do.

    # ---- reachability from entry (drops garbage / unreachable tail) ----
    reachable = set()
    stack = [entry]
    while stack:
        o = stack.pop()
        if o in reachable or o not in blocks:
            continue
        reachable.add(o)
        for _lbl, t in blocks[o].succs:
            if t is not None and t in blocks and t not in reachable:
                stack.append(t)

    rblocks = {o: b for o, b in blocks.items() if o in reachable}
    rorder = [o for o in sorted(reachable)]
    return CFG(func, entry, rblocks, rorder)


# --------------------------------------------------------------------------
# Dominators / post-dominators / natural loops
# --------------------------------------------------------------------------
EXIT = -1  # virtual exit node for post-dominator computation


def _preds(succ_map):
    pred = {n: set() for n in succ_map}
    for n, ss in succ_map.items():
        for s in ss:
            pred.setdefault(s, set()).add(n)
    return pred


def _idoms(entry, succ_map):
    """Immediate dominators (simple iterative set algorithm)."""
    nodes = list(succ_map)
    pred = _preds(succ_map)
    alln = set(nodes)
    dom = {n: set(alln) for n in nodes}
    dom[entry] = {entry}
    changed = True
    while changed:
        changed = False
        for n in nodes:
            if n == entry:
                continue
            ps = [p for p in pred.get(n, ()) if p in dom]
            inter = set(alln)
            if ps:
                inter = set.intersection(*(dom[p] for p in ps))
            new = {n} | inter
            if new != dom[n]:
                dom[n] = new
                changed = True
    idom = {}
    for n in nodes:
        if n == entry:
            continue
        cands = dom[n] - {n}
        # immediate dominator = the candidate dominated by all others (the
        # one with the largest dominator set among candidates).
        idom[n] = max(cands, key=lambda c: len(dom[c])) if cands else entry
    return idom, dom


def _build_succ(cfg: CFG):
    """Forward successor map over reachable blocks (targets within cfg)."""
    succ = {}
    for o, blk in cfg.blocks.items():
        ss = []
        for _lbl, t in blk.succs:
            if t in cfg.blocks:
                ss.append(t)
        succ[o] = ss
    return succ


def _build_postdom_succ(cfg: CFG, succ):
    """Successor map augmented with the virtual EXIT (for post-doms)."""
    psucc = {EXIT: []}
    for o, blk in cfg.blocks.items():
        ss = list(succ[o])
        if not ss:  # return / sink -> exit
            ss = [EXIT]
        psucc[o] = ss
    return psucc


def _post_idoms(cfg: CFG, succ):
    """Immediate post-dominators (dominators on the reversed graph)."""
    psucc = _build_postdom_succ(cfg, succ)
    # reverse edges
    rev = {n: [] for n in psucc}
    for n, ss in psucc.items():
        for s in ss:
            rev.setdefault(s, []).append(n)
            rev.setdefault(n, rev.get(n, []))
    pidom, _ = _idoms(EXIT, rev)
    return pidom


def _natural_loops(cfg: CFG, succ, idom, dom):
    """Return {header_off: set(loop_block_offs)} for reducible back-edges."""
    pred = _preds(succ)
    loops = {}
    for n in cfg.blocks:
        for s in succ[n]:
            # back-edge: s dominates n
            if s in dom.get(n, ()):  # s is a dominator of n
                header = s
                body = {header}
                stack = [n]
                while stack:
                    x = stack.pop()
                    if x not in body:
                        body.add(x)
                        for p in pred.get(x, ()):
                            stack.append(p)
                loops.setdefault(header, set()).update(body)
    return loops


# --------------------------------------------------------------------------
# Expression emitter (Expr -> Luau source string)
# --------------------------------------------------------------------------
def _snake(name: str) -> str:
    out = []
    for i, ch in enumerate(name):
        if ch.isupper() and i > 0 and not name[i - 1].isupper():
            out.append("_")
        out.append(ch.lower())
    return "".join(out)


_INV_OP = {"==": "~=", "~=": "==", "<": ">=", ">": "<=", "<=": ">", ">=": "<"}


class Emitter:
    """Per-script emitter; holds the fn-address -> Lua-name map."""

    def __init__(self, fn_names: dict):
        self.fn_names = fn_names  # addr -> lua name
        self.warnings = []

    def fname(self, addr: int) -> str:
        n = self.fn_names.get(addr)
        if n is None:
            self.warnings.append(f"unresolved fn@{addr:#x}")
            return f"fn_0x{addr:04x}"
        return n

    def expr(self, node, fn: "FnEmit") -> str:
        if isinstance(node, Num):
            return str(node.value)
        if isinstance(node, Str):
            return json.dumps(node.value, ensure_ascii=False)
        if isinstance(node, FnRef):
            return self.fname(node.addr)
        if isinstance(node, Anim):
            frames = "{" + ", ".join(str(f) for f in node.frames) + "}"
            return (f"engine.create_anim({self.expr(node.x, fn)}, "
                    f"{self.expr(node.y, fn)}, {node.delay}, {frames})")
        if isinstance(node, Op):
            return self._op(node, fn)
        raise ValueError(f"cannot emit {node!r}")

    def _op(self, node: Op, fn: "FnEmit") -> str:
        name, args = node.name, node.args
        E = lambda a: self.expr(a, fn)
        if name == "IntConst":
            return E(args[0])
        if name == "__list__":
            return "{" + ", ".join(E(a) for a in args) + "}"
        if name == "Rand":
            return f"math.random({E(args[0])}, {E(args[1])})"
        if name == "Add":
            return f"({E(args[0])} + {E(args[1])})"
        if name == "Sub":
            return f"({E(args[0])} - {E(args[1])})"
        if name == "Mul":
            return f"({E(args[0])} * {E(args[1])})"
        if name == "Div":
            return f"math.floor({E(args[0])} / {E(args[1])})"
        if name == "Min":
            return f"math.min({E(args[0])}, {E(args[1])})"
        if name == "Max":
            return f"math.max({E(args[0])}, {E(args[1])})"
        if name == "Clamp":
            return f"engine.clamp({E(args[0])}, {E(args[1])}, {E(args[2])})"
        if name == "Negate":
            return f"(-{E(args[0])})"
        if name == "Not":
            return f"bit32.bnot({E(args[0])})"
        if name in ("And", "Or", "ShiftLeft", "ShiftRight"):
            fnmap = {"And": "band", "Or": "bor", "ShiftLeft": "lshift", "ShiftRight": "rshift"}
            return f"bit32.{fnmap[name]}({E(args[0])}, {E(args[1])})"
        if name == "GetGlobalVar":
            return f"engine.get_global({E(args[0])})"
        if name == "SetGlobalVar":
            return f"engine.set_global({E(args[0])}, {E(args[1])})"
        if name == "GetLocalVar":
            return fn.local_ref(args[0].value)
        if name == "SetLocalVar":
            # value form (rare; reachable code never nests it -- guarded below)
            return f"engine.__setlocal_expr({fn.local_ref(args[0].value)}, {E(args[1])})"
        if name == "ThisId":
            return "engine.this_id()"
        if name == "Call":
            tgt = self.fname(args[0].addr)
            rest = ", ".join(E(a) for a in args[1:])
            return f"{tgt}({rest})"
        # generic engine call
        return f"engine.{_snake(name)}(" + ", ".join(E(a) for a in args) + ")"

    def cond(self, opname, a, b, fn, invert=False) -> str:
        op = IF_OPS[opname]
        if invert:
            op = _INV_OP[op]
        return f"{self.expr(a, fn)} {op} {self.expr(b, fn)}"


# --------------------------------------------------------------------------
# Per-function structuring + emission
# --------------------------------------------------------------------------
def _indent(lines, n=1):
    pad = "    " * n
    return [pad + l if l else l for l in lines]


class FnEmit:
    """Resolves local/parameter references for one function."""

    def __init__(self, func: Function, nparams: int, param_names: list):
        self.var_count = func.var_count
        self.nparams = nparams
        self.param_names = param_names

    def local_ref(self, idx: int) -> str:
        if idx < self.var_count:
            return f"v{idx}"
        pos = idx - self.var_count
        if 0 <= pos < len(self.param_names):
            return self.param_names[pos]
        return f"p{pos}"


def _is_return_block(blk: Block) -> bool:
    return blk.term[0] == "return"


def structure_function(cfg: CFG, em: Emitter, fnem: FnEmit) -> list:
    """Return the Luau body lines (un-indented) for one function."""
    blocks = cfg.blocks
    succ = _build_succ(cfg)
    idom, dom = _idoms(cfg.entry, succ)
    pidom = _post_idoms(cfg, succ)
    loops = _natural_loops(cfg, succ, idom, dom)
    loop_headers = set(loops)
    sw_counter = [0]

    def stmts(blk):
        out = []
        for s in blk.stmts:
            out.append(em.expr_stmt(s, fnem))
        return out

    def emit_region(off, stop, loopctx, is_body_root=False):
        lines = []
        first = True
        while True:
            if off is None or off == stop:
                return lines, True
            if loopctx is not None and off == loopctx[1]:  # loop exit
                lines.append("break")
                return lines, False
            if loopctx is not None and off == loopctx[0] and not (is_body_root and first):
                lines.append("continue")
                return lines, False
            if off in loop_headers and (loopctx is None or loopctx[0] != off):
                lp, fell = emit_loop(off, stop, loopctx)
                lines += lp
                return lines, fell
            blk = blocks[off]
            lines += stmts(blk)
            first = False
            t = blk.term
            kind = t[0]
            if kind == "return":
                lines.append("return " + em.expr(t[1], fnem))
                return lines, False
            if kind in ("goto", "fallthrough"):
                off = t[1]
                continue
            if kind == "if":
                _, opname, a, b, tgt, nxt = t
                then_off, else_off = tgt, nxt
                join = pidom.get(off, EXIT)
                joinp = None if join == EXIT else join
                then_lines, then_fell = emit_region(then_off, joinp, loopctx)
                else_lines, else_fell = emit_region(else_off, joinp, loopctx)
                # Guard idiom: a branch that always exits (return/break/continue)
                # need not nest the other; continue inline.
                if not then_fell and joinp is None and else_lines:
                    lines.append(f"if {em.cond(opname, a, b, fnem)} then")
                    lines += _indent(then_lines)
                    lines.append("end")
                    off = else_off
                    continue
                if not else_fell and joinp is None and then_lines:
                    lines.append(f"if {em.cond(opname, a, b, fnem, invert=True)} then")
                    lines += _indent(else_lines)
                    lines.append("end")
                    off = then_off
                    continue
                if not else_lines:
                    lines.append(f"if {em.cond(opname, a, b, fnem)} then")
                    lines += _indent(then_lines)
                    lines.append("end")
                elif not then_lines:
                    lines.append(f"if {em.cond(opname, a, b, fnem, invert=True)} then")
                    lines += _indent(else_lines)
                    lines.append("end")
                else:
                    lines.append(f"if {em.cond(opname, a, b, fnem)} then")
                    lines += _indent(then_lines)
                    lines.append("else")
                    lines += _indent(else_lines)
                    lines.append("end")
                if joinp is None:
                    return lines, False
                off = joinp
                continue
            if kind == "switch":
                _, scrut, cases, nxt = t
                join = pidom.get(off, EXIT)
                joinp = None if join == EXIT else join
                sw_counter[0] += 1
                tmp = "_sw" if sw_counter[0] == 1 else f"_sw{sw_counter[0]}"
                lines.append(f"local {tmp} = {em.expr(scrut, fnem)}")
                arm_fell_any = False
                first_arm = True
                for val, tgt in cases:
                    kw = "if" if first_arm else "elseif"
                    first_arm = False
                    arm_lines, fell = emit_region(tgt, joinp, loopctx)
                    arm_fell_any = arm_fell_any or fell
                    lines.append(f"{kw} {tmp} == {val} then")
                    lines += _indent(arm_lines)
                # default arm
                def_lines, def_fell = emit_region(nxt, joinp, loopctx)
                arm_fell_any = arm_fell_any or def_fell
                if def_lines:
                    lines.append("else")
                    lines += _indent(def_lines)
                lines.append("end")
                if joinp is None:
                    if not arm_fell_any:
                        return lines, False
                    # some arm fell through to exit without explicit return:
                    # nothing more to emit.
                    return lines, True
                off = joinp
                continue
            # unknown terminator -> stop defensively
            return lines, False

    def emit_loop(h, stop, outer):
        loop = loops[h]
        exits = set()
        for n in loop:
            for s in succ[n]:
                if s not in loop:
                    exits.add(s)
        real_exits = [e for e in exits if not _is_return_block(blocks[e])]
        loop_exit = real_exits[0] if real_exits else None
        if len(real_exits) > 1:
            em.warnings.append(f"loop@{h:#x} has {len(real_exits)} real exits; using first")
        ctx = (h, loop_exit)
        body, _ = emit_region(h, None, ctx, is_body_root=True)
        out = ["while true do"]
        out += _indent(body)
        out.append("end")
        if loop_exit is not None:
            cont, fell = emit_region(loop_exit, stop, outer)
            out += cont
            return out, fell
        return out, False

    body, _ = emit_region(cfg.entry, None, None)
    return body


# Attach a statement-context expression emitter to Emitter (assignments etc.)
def _expr_stmt(self: Emitter, node, fn: FnEmit) -> str:
    if isinstance(node, Op) and node.name == "SetLocalVar":
        return f"{fn.local_ref(node.args[0].value)} = {self.expr(node.args[1], fn)}"
    return self.expr(node, fn)


Emitter.expr_stmt = _expr_stmt  # type: ignore[attr-defined]


# --------------------------------------------------------------------------
# Whole-module emission
# --------------------------------------------------------------------------
def _collect_call_arities(funcs):
    arity = {}

    def walk(node):
        if isinstance(node, Op):
            if node.name == "Call" and node.args and isinstance(node.args[0], FnRef):
                a = node.args[0].addr
                arity[a] = max(arity.get(a, 0), len(node.args) - 1)
            for x in node.args:
                walk(x)
        elif isinstance(node, Sw):
            walk(node.scrut)
        elif isinstance(node, Anim):
            walk(node.x); walk(node.y)
    for fn in funcs:
        for ins in fn.instrs:
            if ins.node is not None:
                walk(ins.node)
    return arity


def _max_local_index(func: Function) -> int:
    hi = -1

    def walk(node):
        nonlocal hi
        if isinstance(node, Op):
            if node.name in ("GetLocalVar", "SetLocalVar") and node.args and isinstance(node.args[0], Num):
                hi = max(hi, node.args[0].value)
            for x in node.args:
                walk(x)
        elif isinstance(node, Sw):
            walk(node.scrut)
        elif isinstance(node, Anim):
            walk(node.x); walk(node.y)
    for ins in func.instrs:
        if ins.node is not None:
            walk(ins.node)
    return hi


def transpile_asm(text: str) -> str:
    funcs = parse_asm(text)
    # name map
    fn_names = {}
    for fn in funcs:
        fn_names[fn.start] = fn.name
    arities = _collect_call_arities(funcs)
    em = Emitter(fn_names)

    out = [
        "--!nonstrict",
        "-- Generated by asm_to_luau.py from the curated *.script.asm.",
        "-- DO NOT EDIT BY HAND: edit the .asm and regenerate. Luau has no goto;",
        "-- control flow is reconstructed as if/elseif/else + while/break/continue.",
        'local engine = require("engine")',
        "",
    ]

    # Forward-declare every helper (non-export) as a local up front.  Lua does
    # not hoist `local function`, so without this an earlier function calling a
    # helper defined later in the file would bind to a (nil) global.  With the
    # names declared here, the later `function NAME(...)` bodies assign to these
    # locals and all cross-references resolve.  Exports stay global entry points.
    local_names = [fn.name for fn in funcs if not fn.is_export]
    if local_names:
        out.append("-- forward declarations (helpers are file-locals)")
        for chunk in _chunk_names(local_names):
            out.append("local " + ", ".join(chunk))
        out.append("")

    for fn in funcs:
        nparams = max(
            _max_local_index(fn) - fn.var_count + 1,
            arities.get(fn.start, 0),
            len(fn.params),
            0,
        )
        pnames = [fn.params[i] if i < len(fn.params) else f"p{i}" for i in range(nparams)]
        fnem = FnEmit(fn, nparams, pnames)
        cfg = build_cfg(fn)

        if fn.doc:
            for dl in _wrap_doc(fn.doc):
                out.append("-- " + dl)
        sig = ", ".join(pnames)
        # Both exports and helpers use `function NAME(...)`; helper names were
        # forward-declared as locals above, so this assigns to the local (not a
        # new global) while exports remain global engine entry points.
        out.append(f"function {fn.name}({sig})")
        decls = []
        if fn.var_count > 0:
            names = ", ".join(f"v{i}" for i in range(fn.var_count))
            zeros = ", ".join("0" for _ in range(fn.var_count))
            decls.append(f"local {names} = {zeros}")
        body = decls + structure_function(cfg, em, fnem)
        out += _indent(body)
        out.append("end")
        out.append("")

    if em.warnings:
        out.append("-- emitter warnings: " + "; ".join(sorted(set(em.warnings))))
    return "\n".join(out) + "\n"


def _chunk_names(names, width: int = 92):
    """Group names so each `local a, b, c` line stays within `width` chars."""
    lines, cur, cur_len = [], [], len("local ")
    for n in names:
        add = len(n) + (2 if cur else 0)
        if cur and cur_len + add > width:
            lines.append(cur)
            cur, cur_len = [n], len("local ") + len(n)
        else:
            cur.append(n)
            cur_len += add
    if cur:
        lines.append(cur)
    return lines


def _wrap_doc(doc: str, width: int = 96):
    words = doc.split()
    lines, cur = [], ""
    for w in words:
        if cur and len(cur) + 1 + len(w) > width:
            lines.append(cur)
            cur = w
        else:
            cur = (cur + " " + w).strip()
    if cur:
        lines.append(cur)
    return lines


# --------------------------------------------------------------------------
# CLI
# --------------------------------------------------------------------------
def _iter_asm_files():
    """All curated script .asm under unpacked/, deduped by resource name.

    Covers level (``*_Script``), history (``*_HistoryScript``) and help
    (``*_HelpScript``) scripts -- a single ``.asm -> .lua`` path for every
    kind, since the emitter is driven by the rendered opcode *names* and
    needs no per-kind opcode table.  When the same resource appears in
    several pack folders the ``overlay`` copy wins (the built-in master pack).
    """
    seen = {}
    for p in sorted((REPO / "unpacked").glob("**/*Script.script.asm")):
        prev = seen.get(p.name)
        if prev is None or ("overlay" in p.parts and "overlay" not in prev.parts):
            seen[p.name] = p
    return [seen[k] for k in sorted(seen)]


def _resname(asm_path: Path) -> str:
    return asm_path.name[: -len(".script.asm")]


def batch(out_dir: Path) -> int:
    out_dir.mkdir(parents=True, exist_ok=True)
    n = 0
    for path in _iter_asm_files():
        lua = transpile_asm(path.read_text(encoding="utf-8"))
        dst = out_dir / (_resname(path) + ".lua")
        dst.write_text(lua, encoding="utf-8")
        print(f"  {path.name} -> {dst.name}")
        n += 1
    print(f"asm_to_luau: wrote {n} Luau level scripts to {out_dir}")
    return 0


def _self_test():
    nfiles = nfuncs = 0
    gotos = 0
    for path in _iter_asm_files():
        nfiles += 1
        try:
            lua = transpile_asm(path.read_text(encoding="utf-8"))
        except Exception as exc:  # noqa: BLE001
            print(f"  FAIL {path.name}: {exc}")
            continue
        nfuncs += lua.count("\nfunction ") + lua.count("\nlocal function ")
        code_lines = [l for l in lua.splitlines() if not l.lstrip().startswith("--")]
        if any(re.search(r"\bgoto\b", l) or "::" in l for l in code_lines):
            gotos += 1
            print(f"  GOTO present in {path.name}")
    print(f"\ntranspiled {nfiles} scripts, ~{nfuncs} functions, {gotos} with goto")


DEFAULT_OUT = REPO / "open_bulanci" / "assets" / "levels"

if __name__ == "__main__":
    if "--self-test" in sys.argv:
        _self_test()
    elif len(sys.argv) == 1:
        # Drop-in batch mode (invoked by rebuild_assets.py): regenerate every
        # level/history/help script .lua from its curated .asm.
        sys.exit(batch(DEFAULT_OUT))
    elif sys.argv[1] in ("--all", "--batch"):
        out = Path(sys.argv[2]) if len(sys.argv) >= 3 else DEFAULT_OUT
        sys.exit(batch(out))
    else:
        src = Path(sys.argv[1])
        out = transpile_asm(src.read_text(encoding="utf-8"))
        if len(sys.argv) >= 3:
            Path(sys.argv[2]).write_text(out, encoding="utf-8")
            print(f"wrote {sys.argv[2]} ({len(out.splitlines())} lines)")
        else:
            sys.stdout.write(out)
