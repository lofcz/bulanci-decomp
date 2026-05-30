-- Transpiled Bulanci Level Script
local engine = require("engine")

local function get_local(v, args, var_count, idx)
    if idx < var_count then
        return v[idx + 1] or 0
    else
        return args[idx - var_count + 1] or 0
    end
end

local function set_local(v, args, var_count, idx, val)
    if idx < var_count then
        v[idx + 1] = val
    else
        args[idx - var_count + 1] = val
    end
end

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.hhAddChild(engine.hhSetStaticTextStyle())
    engine.hhBuildStaticTextAuto(30, 28, "Bulánek", 2, 65710)
    (0 + 16777215)
    14540253
    14540253
    engine.hhAddChild(engine.hhBuildStaticTextAuto(100, 30, "- vyšší forma života, někdy označována zkratkou E.A.P. (extrémně\n  agresivní polštář) - vybráno z naučného slovníku.", 2, 65711))
    engine.hhAddChild(engine.hhSetStaticTextStyle())
    engine.hhBuildStaticTextAuto(30, 83, "Důležitá data:", 2, 65711)
    engine.get_global(0)
    0
end
