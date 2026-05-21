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
    engine.set_global(1, 2)
    engine.set_global(2, "6A080067-362A-478d-B9EC-68F54424001B")
    if get_local(v, args, 0, 0) == 1 then goto lbl_0x0084 end
    engine.set_global(0, "Dj Slaughter")
    do return 0 end
    ::lbl_0x0084::
    engine.set_global(0, "Vybíjená")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.loadPreface(65847)
    engine.setMusic(65861, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 65846))
    engine.insertView(engine.create_anim(448, 81, 3, {65752}))
    engine.insertView(engine.create_anim(463, 63, 3, {65753}))
    engine.setInsertMode(2)
    engine.insertView(engine.createObstacle(0, 0, 800, 160))
    engine.insertView(engine.createObstacle(0, 160, 40, 233))
    engine.insertView(engine.createObstacle(0, 233, 20, 310))
    engine.insertView(engine.createObstacle(670, 160, 800, 180))
    engine.insertView(engine.createObstacle(760, 180, 800, 300))
    engine.insertView(engine.createObstacle(775, 300, 800, 365))
    engine.setInsertMode(1)
    engine.insertBulanci()
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(0, 0, 65557))
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    do return 0 end
end

function OnBitmapEvt(...)
    local args = {...}
    local v = {}
    do return 0 end
end

function OnSlotPlaced(...)
    local args = {...}
    local v = {}
    do return 0 end
end

function OnSlotDisplaced(...)
    local args = {...}
    local v = {}
    do return 0 end
end

function OnTimer(...)
    local args = {...}
    local v = {}
    do return 0 end
end

function OnEnter(...)
    local args = {...}
    local v = {}
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    do return 0 end
end
