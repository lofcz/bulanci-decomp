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
    engine.set_global(1, 1000)
    engine.set_global(2, "34CC43A7-83E3-4552-8DD8-2FF457F06808")
    engine.set_global(0, "Tunel")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createObstacle(460, 301, 797, 320))
    engine.insertView(engine.createObstacle(0, 302, 289, 321))
    engine.insertView(engine.createObstacle(440, 309, 466, 374))
    engine.insertView(engine.createObstacle(443, 132, 456, 209))
    engine.insertView(engine.createObstacle(294, 135, 312, 178))
    engine.insertView(engine.createObstacle(284, 177, 313, 374))
    engine.insertView(engine.createObstacle(443, 207, 800, 309))
    engine.insertView(engine.createObstacle(0, 207, 307, 309))
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(583, 0, 100007))
    engine.insertView(engine.createImage(0, 219, 100006))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 88, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 262, 100005), 0))
    engine.setInsertMode(0)
    engine.insertBulanci()
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
