use std::cell::RefCell;
use std::rc::Rc;
use mlua::prelude::*;
use crate::engine::EngineSimulation;

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum GlobalVarValue {
    Integer(i32),
    String(String),
}

pub struct LuauScriptEngine {
    lua: Lua,
    _sim: Rc<RefCell<EngineSimulation>>,
    _global_vars: Rc<RefCell<std::collections::HashMap<u8, GlobalVarValue>>>,
}

impl LuauScriptEngine {
    pub fn new(sim: Rc<RefCell<EngineSimulation>>) -> Result<Self, LuaError> {
        // Create Luau VM
        let lua = Lua::new();
        let global_vars = Rc::new(RefCell::new(std::collections::HashMap::new()));

        {
            let sim_clone = Rc::clone(&sim);
            let global_vars_clone = Rc::clone(&global_vars);

            // Define our host bindings in a structured table
            let engine_table = lua.create_table()?;

            // Bind SetGlobalVar & GetGlobalVar
            let gvars = Rc::clone(&global_vars_clone);
            engine_table.set("set_global", lua.create_function(move |_, (id, val): (u8, LuaValue)| {
                match val {
                    LuaValue::Integer(i) => {
                        gvars.borrow_mut().insert(id, GlobalVarValue::Integer(i as i32));
                    }
                    LuaValue::Number(n) => {
                        gvars.borrow_mut().insert(id, GlobalVarValue::Integer(n as i32));
                    }
                    LuaValue::String(s) => {
                        if let Ok(s_str) = s.to_str() {
                            gvars.borrow_mut().insert(id, GlobalVarValue::String(s_str.to_string()));
                        }
                    }
                    _ => {}
                }
                Ok(())
            })?)?;

            let gvars = Rc::clone(&global_vars_clone);
            engine_table.set("get_global", lua.create_function(move |lua, id: u8| {
                let vars = gvars.borrow();
                match vars.get(&id) {
                    Some(GlobalVarValue::Integer(i)) => Ok(LuaValue::Integer(*i as mlua::Integer)),
                    Some(GlobalVarValue::String(s)) => Ok(LuaValue::String(lua.create_string(s)?)),
                    None => Ok(LuaValue::Nil),
                }
            })?)?;

            // Bind CreateObstacle
            let s = Rc::clone(&sim_clone);
            engine_table.set("createObstacle", lua.create_function(move |_, (x1, y1, x2, y2): (i32, i32, i32, i32)| {
                s.borrow_mut().add_obstacle(x1, y1, x2, y2);
                // Returns a dummy object or index representing the obstacle view
                Ok(s.borrow().state.obstacles.len() - 1)
            })?)?;

            // Bind SetObstacleBounds
            let s = Rc::clone(&sim_clone);
            engine_table.set("setObstacleBounds", lua.create_function(move |_, (idx, x1, y1, x2, y2): (usize, i32, i32, i32, i32)| {
                s.borrow_mut().set_obstacle_bounds(idx, x1, y1, x2, y2);
                Ok(())
            })?)?;

            // Bind tunable get/set — the data-driven half of the
            // Rust-primitives / Luau-gamemode split. A deterministic Luau
            // gamemode reads and retunes the sim's primitive parameters
            // (player/bullet speed, hitbox sizes, arena bounds) by name; the
            // values are integer so a live edit stays lockstep-safe.
            let s = Rc::clone(&sim_clone);
            engine_table.set("set_tunable", lua.create_function(move |_, (key, value): (String, i32)| {
                Ok(crate::gameplay::set_tunable_field(&mut s.borrow_mut().tunables, &key, value))
            })?)?;

            let s = Rc::clone(&sim_clone);
            engine_table.set("get_tunable", lua.create_function(move |_, key: String| {
                let t = s.borrow().tunables;
                let v = match key.as_str() {
                    "player_speed" => Some(t.player_speed),
                    "player_w" => Some(t.player_w),
                    "player_h" => Some(t.player_h),
                    "bullet_speed" => Some(t.bullet_speed),
                    "arena_w" => Some(t.arena_w),
                    "arena_h" => Some(t.arena_h),
                    _ => None,
                };
                Ok(v)
            })?)?;

            // Bind RegisterTimer
            let s = Rc::clone(&sim_clone);
            engine_table.set("registerTimer", lua.create_function(move |_, (slot, delay, flags): (i32, i32, i32)| {
                s.borrow_mut().add_timer(slot, delay, flags);
                Ok(())
            })?)?;

            // Bind TimerStop
            let s = Rc::clone(&sim_clone);
            engine_table.set("timerStop", lua.create_function(move |_, slot: i32| {
                s.borrow_mut().stop_timer(slot);
                Ok(())
            })?)?;

            // Placeholders / Logs for other opcodes to support seamless running of level scripts
            engine_table.set("loadPreface", lua.create_function(|_, id: i32| {
                println!("[Lua Script] loadPreface: id={}", id);
                Ok(())
            })?)?;

            engine_table.set("setMusic", lua.create_function(|_, (track, volume): (i32, i32)| {
                println!("[Lua Script] setMusic: track={}, volume={}", track, volume);
                Ok(())
            })?)?;

            engine_table.set("setInsertMode", lua.create_function(|_, mode: i32| {
                println!("[Lua Script] setInsertMode: mode={}", mode);
                Ok(())
            })?)?;

            engine_table.set("insertView", lua.create_function(|_, _view: LuaValue| {
                Ok(())
            })?)?;

            engine_table.set("createImage", lua.create_function(|_, (x, y, id): (i32, i32, i32)| {
                // Placeholder: returns unique image identifier
                Ok(format!("image_{}_{}_{}", x, y, id))
            })?)?;

            engine_table.set("create_anim", lua.create_function(|_, (x, y, delay, frames): (i32, i32, u8, Vec<i32>)| {
                // Placeholder: returns unique anim identifier
                Ok(format!("anim_{}_{}_{}_{:?}", x, y, delay, frames))
            })?)?;

            engine_table.set("insertBulanci", lua.create_function(|_, ()| {
                println!("[Lua Script] insertBulanci invoked");
                Ok(())
            })?)?;

            // Luau has no `package.loaded`; host scripts use `require("engine")`.
            // Register as `@engine` (mlua Luau convention) and expose a global fallback.
            lua.register_module("@engine", engine_table.clone())?;
            lua.globals().set("engine", engine_table)?;
        }

        Ok(LuauScriptEngine {
            lua,
            _sim: sim,
            _global_vars: global_vars,
        })
    }

    /// Load level script into VM.
    pub fn load_level_script(&self, lua_code: &str) -> Result<(), LuaError> {
        // Transpiled scripts use `require("engine")`; mlua Luau registers modules as `@name`.
        // Luau has no `goto`; transpiled bytecode lowers to goto/labels (see desugar_goto_for_luau).
        let code = desugar_goto_for_luau(lua_code).replace("require(\"engine\")", "require(\"@engine\")");
        self.lua.load(&code).exec()
    }

    /// Execute OnInit export function.
    pub fn call_on_init(&self) -> Result<(), LuaError> {
        let on_init: LuaFunction = self.lua.globals().get("OnInit")?;
        on_init.call::<()>(())
    }

    /// Execute OnGameStart export function.
    pub fn call_on_game_start(&self) -> Result<(), LuaError> {
        let on_game_start: LuaFunction = self.lua.globals().get("OnGameStart")?;
        on_game_start.call::<()>(())
    }

    /// Execute OnTimer export function.
    pub fn call_on_timer(&self, slot_id: i32) -> Result<(), LuaError> {
        if let Ok(on_timer) = self.lua.globals().get::<LuaFunction>("OnTimer") {
            on_timer.call::<()>(slot_id)?;
        }
        Ok(())
    }

    /// Execute OnEnter export function.
    pub fn call_on_enter(&self, trace_id: i32, entity_slot: i32) -> Result<(), LuaError> {
        if let Ok(on_enter) = self.lua.globals().get::<LuaFunction>("OnEnter") {
            on_enter.call::<()>((trace_id, entity_slot))?;
        }
        Ok(())
    }

    /// Execute OnLeave export function.
    pub fn call_on_leave(&self, trace_id: i32, entity_slot: i32) -> Result<(), LuaError> {
        if let Ok(on_leave) = self.lua.globals().get::<LuaFunction>("OnLeave") {
            on_leave.call::<()>((trace_id, entity_slot))?;
        }
        Ok(())
    }
}

/// Prepare transpiled level Lua for Luau: normalize labels, then remove unsupported `goto`.
fn desugar_goto_for_luau(code: &str) -> String {
    let mut lines: Vec<String> = fix_luau_goto_labels(code).lines().map(str::to_string).collect();
    loop {
        if let Some((next, replacement)) = try_desugar_switch_block(&lines) {
            lines.splice(next.0..next.1, replacement);
            continue;
        }
        if let Some((next, replacement)) = try_desugar_forward_gotos(&lines) {
            lines.splice(next.0..next.1, replacement);
            continue;
        }
        break;
    }
    lines.join("\n")
}

fn line_indent(line: &str) -> String {
    line.chars()
        .take_while(|c| c.is_whitespace())
        .collect()
}

fn is_label_line(line: &str, label: &str) -> bool {
    line.trim() == format!("::{label}::")
}

/// `if <cond> then goto lbl_N end` (not `engine.goto`).
fn parse_switch_arm(line: &str) -> Option<(String, String, String)> {
    let (cond, label) = parse_forward_goto(line)?;
    let (lhs, val) = cond.split_once(" == ")?;
    Some((lhs.trim().to_string(), val.trim().to_string(), label))
}

fn parse_forward_goto(line: &str) -> Option<(String, String)> {
    let trimmed = line.trim();
    if !trimmed.starts_with("if ") || !trimmed.ends_with(" end") {
        return None;
    }
    let inner = trimmed.strip_prefix("if ")?.strip_suffix(" end")?;
    let (cond, label) = inner.rsplit_once(" then goto ")?;
    if !label.starts_with("lbl_") {
        return None;
    }
    Some((cond.to_string(), label.to_string()))
}

fn try_desugar_switch_block(lines: &[String]) -> Option<((usize, usize), Vec<String>)> {
    let start = lines.iter().position(|l| parse_switch_arm(l).is_some())?;
    let first = parse_switch_arm(&lines[start])?;
    let lhs = first.0;
    let indent = line_indent(&lines[start]);

    let mut arms: Vec<(String, String)> = Vec::new(); // (val, label)
    let mut i = start;
    while i < lines.len() {
        if line_indent(&lines[i]) != indent {
            break;
        }
        match parse_switch_arm(&lines[i]) {
            Some((arm_lhs, val, label)) if arm_lhs == lhs => {
                arms.push((val, label));
                i += 1;
            }
            _ => break,
        }
    }
    if arms.len() < 2 {
        return None;
    }

    let default_start = i;
    let first_label = &arms[0].1;
    let first_label_idx = (default_start..lines.len()).find(|&j| is_label_line(&lines[j], first_label))?;
    let default_body: Vec<String> = lines[default_start..first_label_idx].to_vec();

    let mut case_bodies: Vec<Vec<String>> = Vec::new();
    let mut label_positions: Vec<usize> = Vec::new();
    for (_, label) in &arms {
        let pos = lines[default_start..]
            .iter()
            .position(|l| is_label_line(l, label))?;
        label_positions.push(default_start + pos);
    }
    label_positions.push(lines.len());

    for (idx, &pos) in label_positions[..arms.len()].iter().enumerate() {
        let next = label_positions[idx + 1];
        let body_start = pos + 1;
        case_bodies.push(lines[body_start..next].to_vec());
    }

    let end = label_positions[arms.len()];
    let mut out = Vec::new();
    for (idx, (val, _)) in arms.iter().enumerate() {
        let kw = if idx == 0 { "if" } else { "elseif" };
        out.push(format!("{indent}{kw} {lhs} == {val} then"));
        for line in &case_bodies[idx] {
            out.push(line.clone());
        }
    }
    out.push(format!("{indent}else"));
    for line in &default_body {
        out.push(line.clone());
    }
    out.push(format!("{indent}end"));

    Some(((start, end), out))
}

fn try_desugar_forward_gotos(lines: &[String]) -> Option<((usize, usize), Vec<String>)> {
    let goto_idx = lines.iter().position(|l| parse_forward_goto(l).is_some())?;
    let (_, label) = parse_forward_goto(&lines[goto_idx])?;
    let indent = line_indent(&lines[goto_idx]);

    let mut conds: Vec<String> = Vec::new();
    let mut i = goto_idx;
    while i < lines.len() {
        if line_indent(&lines[i]) != indent {
            break;
        }
        if let Some((cond, lbl)) = parse_forward_goto(&lines[i]) {
            if lbl == label {
                conds.push(cond);
                i += 1;
                continue;
            }
        }
        break;
    }

    let label_idx = (i..lines.len()).find(|&j| is_label_line(&lines[j], &label))?;
    let region: Vec<String> = lines[i..label_idx].to_vec();
    let after_label: Vec<String> = lines[label_idx + 1..].to_vec();

    let mut nested = region;
    for cond in conds.iter().rev() {
        let mut wrapped = Vec::with_capacity(nested.len() + 2);
        wrapped.push(format!("{indent}if not ({cond}) then"));
        wrapped.extend(nested);
        wrapped.push(format!("{indent}end"));
        nested = wrapped;
    }

    let consumed_end = label_idx + 1 + after_label.len();
    let mut out = nested;
    out.extend(after_label);
    Some(((goto_idx, consumed_end), out))
}

/// Transpiler labels were `lbl_0x{off:04x}`; Luau rejects `goto lbl_0084` because `0084` lexes as a number.
fn fix_luau_goto_labels(code: &str) -> String {
    let mut out = String::with_capacity(code.len());
    let bytes = code.as_bytes();
    let mut i = 0;
    while i < bytes.len() {
        if bytes[i..].starts_with(b"lbl_0x") {
            i += 6;
            let start = i;
            while i < bytes.len() && bytes[i].is_ascii_hexdigit() {
                i += 1;
            }
            let hex = std::str::from_utf8(&bytes[start..i]).unwrap_or("0");
            let n = u32::from_str_radix(hex, 16).unwrap_or(0);
            out.push_str("lbl_");
            out.push_str(&n.to_string());
        } else {
            out.push(char::from(bytes[i]));
            i += 1;
        }
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::assets::AssetFileSystem;

    #[test]
    fn fix_goto_labels_rewrites_hex_suffix() {
        assert_eq!(
            fix_luau_goto_labels("goto lbl_0x0084\n::lbl_0x0084::"),
            "goto lbl_132\n::lbl_132::",
        );
    }

    #[test]
    fn desugar_getinfo_goto() {
        let raw = r#"
function GetInfo(...)
    if get_local(v, args, 0, 0) == 1 then goto lbl_0x0084 end
    engine.set_global(0, "A")
    do return 0 end
    ::lbl_0x0084::
    engine.set_global(0, "B")
end
"#;
        let out = desugar_goto_for_luau(raw);
        assert!(!out.contains("goto "));
        assert!(out.contains("if not (get_local"));
    }

    #[test]
    fn lua_gamemode_can_get_and_set_sim_tunables() {
        let sim = Rc::new(RefCell::new(EngineSimulation::new()));
        let eng = LuauScriptEngine::new(Rc::clone(&sim)).unwrap();
        // A gamemode reads a primitive parameter and retunes another.
        eng.load_level_script(
            r#"
            local base = engine.get_tunable("bullet_speed")
            engine.set_tunable("player_speed", base + 3)
            unknown = engine.set_tunable("nope", 1)
            "#,
        )
        .unwrap();
        // bullet_speed default 8 -> player_speed becomes 11; unknown key noop.
        assert_eq!(sim.borrow().tunables.player_speed, 11);
        assert_eq!(sim.borrow().tunables.bullet_speed, 8);
    }

    #[test]
    fn test_lua_bindings_and_init() {
        let sim = Rc::new(RefCell::new(EngineSimulation::new()));
        let script_engine = LuauScriptEngine::new(Rc::clone(&sim)).unwrap();

        let vfs = AssetFileSystem::load().unwrap();
        let test_script_code = vfs
            .read_to_string("levels/res_0000065855_2026_Script.lua")
            .unwrap()
            .unwrap();

        script_engine.load_level_script(&test_script_code).unwrap();

        // Let's call OnInit which populates obstacles dynamically!
        script_engine.call_on_init().unwrap();

        let sim_borrow = sim.borrow();
        // The original script has 6 `createObstacle` calls in its OnInit function.
        assert_eq!(sim_borrow.state.obstacles.len(), 6, "OnInit should have loaded 6 obstacles dynamically!");

        // Assert values of some obstacles
        assert_eq!(sim_borrow.state.obstacles[0].x1, 0);
        assert_eq!(sim_borrow.state.obstacles[0].x2, 800);
        assert_eq!(sim_borrow.state.obstacles[0].y2, 160);
    }
}
