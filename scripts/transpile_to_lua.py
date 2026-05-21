import os
import sys
import struct
import json
from pathlib import Path

# Add tools/bulanci_unpack to sys.path so we can import bulanci_unpack
sys.path.append(str(Path(__file__).parent.parent / "tools" / "bulanci_unpack"))
import bulanci_unpack

def get_opcodes(kind):
    return bulanci_unpack._SCRIPT_OPCODES.get(kind)

def transpile_script(raw_bytes, kind="level_script"):
    if len(raw_bytes) < 12:
        return "-- Script truncated\n"
    
    code_len, n_exports, n_vars = struct.unpack_from("<iii", raw_bytes, 0)
    if code_len < 0 or 12 + code_len + 4 * n_exports > len(raw_bytes):
        return "-- Script size mismatch\n"
        
    code = raw_bytes[12 : 12 + code_len]
    exports_off = 12 + code_len
    exports = list(
        struct.unpack_from(f"<{n_exports}i", raw_bytes, exports_off)
    ) if n_exports > 0 else []

    opcodes = get_opcodes(kind)
    if opcodes is None:
        return f"-- Unknown script kind {kind}\n"

    # Pass 1: Trace all reachable code to find:
    # 1. All target offsets for Goto / Branch/ Switch / Select (labels)
    # 2. All target offsets of Call (private functions)
    targets = set()
    fn_entries = set(exports)
    
    def trace_sub(off):
        if off >= len(code):
            return off
        op = code[off]
        end = off + 1
        info = opcodes.get(op)
        if info is None:
            return end
        name, spec = info

        if spec == "_call":
            target = struct.unpack_from("<i", code, end)[0]
            fn_entries.add(target)
            end += 4
            param_count = code[end]
            end += 1
            for _ in range(param_count):
                end = trace_sub(end)
            return end

        if spec == "_switch":
            end = trace_sub(end)
            count = code[end]
            end += 1
            for _ in range(count):
                val = struct.unpack_from("<i", code, end)[0]
                end += 4
                lbl = struct.unpack_from("<i", code, end)[0]
                end += 4
                targets.add(lbl)
            return end

        if spec == "_create_anim":
            end = trace_sub(end)
            end = trace_sub(end)
            end += 2 # delay and count
            n_frames = code[end-1]
            end += 4 * n_frames
            return end

        if spec == "_strconst":
            while end + 1 < len(code):
                cu = struct.unpack_from("<H", code, end)[0]
                end += 2
                if cu == 0:
                    break
            return end

        for token in spec:
            if token == "i32":
                end += 4
            elif token == "u8":
                end += 1
            elif token == "sub":
                end = trace_sub(end)
        return end

    def trace_function(start, end_hint):
        off = start + 1
        seen_branch = False
        while off < len(code) and off < end_hint:
            opc = code[off]
            if opc not in opcodes:
                break
            
            # Record branch targets
            if opc == 13: # Goto
                target = struct.unpack_from("<i", code, off + 1)[0]
                targets.add(target)
            elif 18 <= opc <= 23: # If*
                target = struct.unpack_from("<i", code, off + 1 + 1 + 4 + 4)[0] # skip skip subs, read target
                # Wait, the offset of i32 target in If* is actually:
                # off + 1 + parsed sub1 + parsed sub2. Let's trace it precisely!
                pass
            
            # Let's trace the sub-expressions and instruct-advancing
            try:
                # We trace any sub-expressions of this instruction
                new_off = trace_sub(off)
                
                # If it's a branch, find the target
                if 18 <= opc <= 23:
                    # An If* instruction has: u8 op, sub a, sub b, i32 target
                    # We can find target by parsing backwards from new_off
                    target = struct.unpack_from("<i", code, new_off - 4)[0]
                    targets.add(target)
                elif opc == 13: # Goto
                    target = struct.unpack_from("<i", code, off + 1)[0]
                    targets.add(target)
                
                off = new_off
            except Exception:
                break
            
            if not seen_branch and opc == 17: # Return
                break
            if opc == 13 or 18 <= opc <= 25:
                seen_branch = True

    # Run Pass 1
    sorted_exports = sorted(set(exports))
    for i, start in enumerate(sorted_exports):
        end_hint = sorted_exports[i+1] if i+1 < len(sorted_exports) else len(code)
        trace_function(start, end_hint)

    # Re-run Pass 1 recursively for any discovered function entries
    finished_fns = set()
    while fn_entries - finished_fns:
        to_process = list(fn_entries - finished_fns)
        for start in to_process:
            finished_fns.add(start)
            # Find closest next function entry as end_hint
            all_entries = sorted(list(fn_entries | {len(code)}))
            idx = all_entries.index(start)
            end_hint = all_entries[idx+1]
            trace_function(start, end_hint)

    # Pass 2: Generation Pass
    # Transpile commands recursively
    def gen_command(off, var_count):
        op = code[off]
        end = off + 1
        info = opcodes.get(op)
        if info is None:
            return end, f"nil -- UNKNOWN_OP_{op}"
        name, spec = info

        # Translate names to camelCase/snake_case or clean them up
        # We can map opcode names to clean snake_case engine calls
        rust_name = name[0].lower() + name[1:]
        
        # Override some names
        if rust_name == "rand":
            rust_name = "math.random" # we can use native Lua random or engine.rand
        elif rust_name in ["min", "max"]:
            rust_name = "math." + rust_name
        else:
            rust_name = "engine." + rust_name

        if spec == "_call":
            target = struct.unpack_from("<i", code, end)[0]
            end += 4
            param_count = code[end]
            end += 1
            arg_strs = []
            for _ in range(param_count):
                end, s = gen_command(end, var_count)
                arg_strs.append(s)
            return end, f"fn_0x{target:04x}({', '.join(arg_strs)})"

        if spec == "_switch":
            end, expr = gen_command(end, var_count)
            count = code[end]
            end += 1
            cases = []
            for _ in range(count):
                val = struct.unpack_from("<i", code, end)[0]
                end += 4
                lbl = struct.unpack_from("<i", code, end)[0]
                end += 4
                cases.append((val, lbl))
            
            # Formulate inline structure for Switch or Select
            if op == 24: # Switch
                case_lines = [f"local _sw = {expr}"]
                for val, lbl in cases:
                    case_lines.append(f"if _sw == {val} then goto lbl_0x{lbl:04x} end")
                return end, "\n".join(case_lines)
            else: # Select
                case_lines = []
                for val, match_val in cases:
                    case_lines.append(f"if _sel == {val} then return {match_val} end")
                inner = " ".join(case_lines)
                return end, f"(function() local _sel = {expr} {inner} end)()"

        if spec == "_create_anim":
            end, x = gen_command(end, var_count)
            end, y = gen_command(end, var_count)
            delay = code[end]; end += 1
            n_frames = code[end]; end += 1
            frame_ids = []
            for _ in range(n_frames):
                fid = struct.unpack_from("<i", code, end)[0]
                end += 4
                frame_ids.append(fid)
            frames_str = "{" + ", ".join(str(f) for f in frame_ids) + "}"
            return end, f"engine.create_anim({x}, {y}, {delay}, {frames_str})"

        if spec == "_strconst":
            chars = []
            while end + 1 < len(code):
                cu = struct.unpack_from("<H", code, end)[0]
                end += 2
                if cu == 0:
                    break
                chars.append(chr(cu))
            return end, json.dumps("".join(chars), ensure_ascii=False)

        # Base translations
        if op == 0: # IntConst
            val = struct.unpack_from("<i", code, end)[0]
            return end + 4, str(val)
        elif op == 2: # GetGlobalVar
            return end + 1, f"engine.get_global({code[end]})"
        elif op == 3: # GetLocalVar
            return end + 1, f"get_local(v, args, {var_count}, {code[end]})"
        elif op == 4: # Add
            end, a = gen_command(end, var_count)
            end, b = gen_command(end, var_count)
            return end, f"({a} + {b})"
        elif op == 5: # Sub
            end, a = gen_command(end, var_count)
            end, b = gen_command(end, var_count)
            return end, f"({a} - {b})"
        elif op == 6: # Mul
            end, a = gen_command(end, var_count)
            end, b = gen_command(end, var_count)
            return end, f"({a} * {b})"
        elif op == 7: # Div
            end, a = gen_command(end, var_count)
            end, b = gen_command(end, var_count)
            return end, f"math.floor({a} / {b})"
        elif op == 8: # Clamp
            end, min_val = gen_command(end, var_count)
            end, max_val = gen_command(end, var_count)
            end, val = gen_command(end, var_count)
            return end, f"engine.clamp({min_val}, {max_val}, {val})"
        elif op == 11: # SetGlobalVar
            val_id = code[end]
            sub_end, s = gen_command(end + 1, var_count)
            return sub_end, f"engine.set_global({val_id}, {s})"
        elif op == 12: # SetLocalVar
            lid = code[end]
            end, s = gen_command(end + 1, var_count)
            return end, f"set_local(v, args, {var_count}, {lid}, {s})"
        elif op == 15: # ThisId
            return end, "engine.this_id()"
        elif op == 16: # GetLocalVarIdx
            end, s = gen_command(end, var_count)
            return end, f"get_local(v, args, {var_count}, {s})"
        elif op == 17: # Return
            end, s = gen_command(end, var_count)
            return end, f"do return {s} end"
        elif op == 35: # Not
            end, s = gen_command(end, var_count)
            return end, f"bit32.bnot({s})"
        elif op == 36: # Negate
            end, s = gen_command(end, var_count)
            return end, f"(-{s})"
        elif op in [31, 32, 33, 34]: # ShiftRight, ShiftLeft, And, Or
            end, a = gen_command(end, var_count)
            end, b = gen_command(end, var_count)
            op_map = {31: "rshift", 32: "lshift", 33: "band", 34: "bor"}
            return end, f"bit32.{op_map[op]}({a}, {b})"
        elif op in [18, 19, 20, 21, 22, 23]: # If*
            end, a = gen_command(end, var_count)
            end, b = gen_command(end, var_count)
            target = struct.unpack_from("<i", code, end)[0]
            op_map = {18: "==", 19: "<", 20: ">", 21: "<=", 22: ">=", 23: "~="}
            return end + 4, f"if {a} {op_map[op]} {b} then goto lbl_0x{target:04x} end"
        
        # Generic fallback for extension opcodes and others
        arg_strs = []
        for token in spec:
            if token == "i32":
                v = struct.unpack_from("<i", code, end)[0]
                end += 4
                arg_strs.append(str(v))
            elif token == "u8":
                arg_strs.append(str(code[end]))
                end += 1
            elif token == "sub":
                end, s = gen_command(end, var_count)
                arg_strs.append(s)
        return end, f"{rust_name}({', '.join(arg_strs)})"

    # Emit Lua code
    out_lines = [
        "-- Transpiled Bulanci Level Script",
        "local engine = require(\"engine\")",
        "",
        "local function get_local(v, args, var_count, idx)",
        "    if idx < var_count then",
        "        return v[idx + 1] or 0",
        "    else",
        "        return args[idx - var_count + 1] or 0",
        "    end",
        "end",
        "",
        "local function set_local(v, args, var_count, idx, val)",
        "    if idx < var_count then",
        "        v[idx + 1] = val",
        "    else",
        "        args[idx - var_count + 1] = val",
        "    end",
        "end",
        ""
    ]

    # Render each function
    def transpile_function_body(start, end_hint):
        var_count = code[start]
        body_lines = []
        off = start + 1
        seen_branch = False
        while off < len(code) and off < end_hint:
            if off in targets:
                body_lines.append(f"    ::lbl_0x{off:04x}::")
                
            opc = code[off]
            if opc not in opcodes:
                break
            try:
                new_off, s = gen_command(off, var_count)
                body_lines.append(f"    {s}")
                off = new_off
            except Exception as e:
                body_lines.append(f"    -- TRANSPILATION ERROR: {e}")
                break
                
            if not seen_branch and opc == 17: # Return
                break
            if opc == 13 or 18 <= opc <= 25:
                seen_branch = True
        return body_lines

    # First, generate all private subroutine functions
    private_fns = fn_entries - set(exports)
    for start in sorted(list(private_fns)):
        all_entries = sorted(list(fn_entries | {len(code)}))
        idx = all_entries.index(start)
        end_hint = all_entries[idx+1]
        
        out_lines.append(f"local function fn_0x{start:04x}(...)")
        out_lines.append("    local args = {...}")
        out_lines.append("    local v = {}")
        body = transpile_function_body(start, end_hint)
        out_lines.extend(body)
        out_lines.append("end")
        out_lines.append("")

    # Then generate the positional export functions
    export_map = {
        0: "GetInfo",
        1: "OnInit",
        2: "OnDeinit",
        3: "OnBitmapEvt",
        4: "OnSlotPlaced",
        5: "OnSlotDisplaced",
        6: "OnTimer",
        7: "OnEnter",
        8: "OnLeave",
        9: "OnNetCustom",
        10: "OnGameStart"
    }

    for i, start in enumerate(exports):
        all_entries = sorted(list(fn_entries | {len(code)}))
        idx = all_entries.index(start)
        end_hint = all_entries[idx+1]
        
        export_name = export_map.get(i, f"export_{i}")
        
        out_lines.append(f"function {export_name}(...)")
        out_lines.append("    local args = {...}")
        out_lines.append("    local v = {}")
        body = transpile_function_body(start, end_hint)
        out_lines.extend(body)
        out_lines.append("end")
        out_lines.append("")

    return "\n".join(out_lines)

# Main batch transpiler
if __name__ == "__main__":
    import glob
    
    unpacked_dir = Path(__file__).parent.parent / "unpacked"
    out_dir = Path(__file__).parent.parent / "open_bulanci" / "assets" / "levels"
    out_dir.mkdir(parents=True, exist_ok=True)
    
    bin_files = list(unpacked_dir.glob("**/*_Script.bin")) + list(unpacked_dir.glob("**/*_Script.script.bin"))
    bin_files += list(unpacked_dir.glob("**/*_HistoryScript.bin")) + list(unpacked_dir.glob("**/*_HistoryScript.script.bin"))
    bin_files += list(unpacked_dir.glob("**/*_HelpScript.bin")) + list(unpacked_dir.glob("**/*_HelpScript.script.bin"))
    
    # Deduplicate bin files by resolving absolute paths
    unique_files = {}
    for bf in bin_files:
        unique_files[bf.resolve()] = bf
        
    print(f"Found {len(unique_files)} script files to transpile.")
    
    for abs_path, bf in unique_files.items():
        kind = "level_script"
        if "HistoryScript" in bf.name:
            kind = "history_script"
        elif "HelpScript" in bf.name:
            kind = "help_script"
            
        print(f"Transpiling {bf.name} ({kind})...")
        try:
            raw_bytes = bf.read_bytes()
            # If the file is just the raw script payload (.script.bin), wait:
            # Let's check if the file starts with header bytes code_len, n_exports, n_vars
            # In bulanci_unpack.py, both the full raw and .script.bin are generated.
            # Let's check which file format we have.
            # If the file name is .script.bin, let's look for the matching full resource .bin file which has the exports at the end!
            # Let's see: full .bin contains the exports. .script.bin only has the bytecode!
            # Therefore, we should ALWAYS transpile from the FULL resource .bin file, not the .script.bin!
            # The full resource .bin is named like: res_0000065855_2026_Script.bin
            # Let's find the full resource .bin for this script!
            full_bin_path = bf.parent / bf.name.replace(".script.bin", ".bin")
            if not full_bin_path.exists():
                full_bin_path = bf
                
            raw_bytes = full_bin_path.read_bytes()
            
            lua_code = transpile_script(raw_bytes, kind=kind)
            
            # Clean up the output name, e.g. res_0000065855_2026_Script.lua
            lua_name = full_bin_path.name.replace(".bin", ".lua")
            out_file = out_dir / lua_name
            out_file.write_text(lua_code, encoding="utf-8")
            print(f"  Saved to {out_file}")
        except Exception as e:
            print(f"  Failed: {e}")
