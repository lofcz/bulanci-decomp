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
                    Some(GlobalVarValue::Integer(i)) => Ok(LuaValue::Integer(*i as i64)),
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

            // Setup package.loaded["engine"] so that `local engine = require("engine")` works instantly without closures
            let loaded: LuaTable = lua.load("package.loaded").eval()?;
            loaded.set("engine", engine_table)?;
        }

        Ok(LuauScriptEngine {
            lua,
            _sim: sim,
            _global_vars: global_vars,
        })
    }

    /// Load level script into VM.
    pub fn load_level_script(&self, lua_code: &str) -> Result<(), LuaError> {
        self.lua.load(lua_code).exec()
    }

    /// Execute OnInit export function.
    pub fn call_on_init(&self) -> Result<(), LuaError> {
        let on_init: LuaFunction = self.lua.globals().get("OnInit")?;
        on_init.call::<_, ()>(())
    }

    /// Execute OnGameStart export function.
    pub fn call_on_game_start(&self) -> Result<(), LuaError> {
        let on_game_start: LuaFunction = self.lua.globals().get("OnGameStart")?;
        on_game_start.call::<_, ()>(())
    }

    /// Execute OnTimer export function.
    pub fn call_on_timer(&self, slot_id: i32) -> Result<(), LuaError> {
        if let Ok(on_timer) = self.lua.globals().get::<_, LuaFunction>("OnTimer") {
            on_timer.call::<_, ()>(slot_id)?;
        }
        Ok(())
    }

    /// Execute OnEnter export function.
    pub fn call_on_enter(&self, trace_id: i32, entity_slot: i32) -> Result<(), LuaError> {
        if let Ok(on_enter) = self.lua.globals().get::<_, LuaFunction>("OnEnter") {
            on_enter.call::<_, ()>((trace_id, entity_slot))?;
        }
        Ok(())
    }

    /// Execute OnLeave export function.
    pub fn call_on_leave(&self, trace_id: i32, entity_slot: i32) -> Result<(), LuaError> {
        if let Ok(on_leave) = self.lua.globals().get::<_, LuaFunction>("OnLeave") {
            on_leave.call::<_, ()>((trace_id, entity_slot))?;
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::assets::AssetFileSystem;

    #[test]
    fn test_lua_bindings_and_init() {
        let sim = Rc::new(RefCell::new(EngineSimulation::new()));
        let script_engine = LuauScriptEngine::new(Rc::clone(&sim)).unwrap();

        let vfs = AssetFileSystem::load().unwrap();
        let test_script_code = vfs.read_to_string("levels/res_0000065855_2026_Script.lua")
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
