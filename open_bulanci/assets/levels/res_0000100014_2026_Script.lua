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
    engine.set_global(2, "8DCFCC56-35BC-4141-82F9-B46CB402D7C8")
    engine.set_global(0, "Srdeční záležitost")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createObstacle(749, 443, 799, 468))
    engine.insertView(engine.createObstacle(0, 205, 64, 220))
    engine.insertView(engine.createObstacle(64, 22, 103, 56))
    engine.insertView(engine.createObstacle(100, 472, 188, 515))
    engine.insertView(engine.createObstacle(375, 469, 469, 515))
    engine.insertView(engine.createObstacle(549, 467, 635, 515))
    engine.insertView(engine.createObstacle(708, 39, 722, 68))
    engine.insertView(engine.createObstacle(720, 0, 800, 70))
    engine.insertView(engine.createObstacle(437, 34, 526, 68))
    engine.insertView(engine.createObstacle(451, 0, 518, 33))
    engine.insertView(engine.createObstacle(620, 0, 646, 186))
    engine.insertView(engine.createObstacle(630, 204, 727, 227))
    engine.insertView(engine.createObstacle(548, 181, 646, 205))
    engine.insertView(engine.createObstacle(707, 318, 800, 384))
    engine.insertView(engine.createObstacle(542, 320, 635, 342))
    engine.insertView(engine.createObstacle(451, 277, 544, 343))
    engine.insertView(engine.createObstacle(352, 293, 460, 315))
    engine.insertView(engine.createObstacle(351, 291, 377, 515))
    engine.insertView(engine.createObstacle(181, 291, 280, 315))
    engine.insertView(engine.createObstacle(370, 182, 473, 205))
    engine.insertView(engine.createObstacle(180, 142, 279, 163))
    engine.insertView(engine.createObstacle(254, 62, 278, 164))
    engine.insertView(engine.createObstacle(280, 139, 373, 205))
    engine.insertView(engine.createObstacle(0, 289, 181, 354))
    engine.insertView(engine.createObstacle(0, 139, 180, 205))
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(723, 0, 100013))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(746, 425, 100011), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(545, 446, 100010), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(436, 0, 100010), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(66, 24, 100012), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(97, 446, 100010), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(708, 0, 100010), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(377, 446, 100010), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(645, 183, 100009), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(182, 317, 100009), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 205, 100008), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(629, 203, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(707, 318, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(451, 277, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(537, 320, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(183, 139, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(621, 83, 100007), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(254, 60, 100007), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(352, 415, 100007), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(621, 0, 100007), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(352, 315, 100007), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(547, 181, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(375, 182, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 84, 100006), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(280, 139, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(351, 291, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(181, 292, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(88, 290, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(88, 139, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 289, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 139, 100004), 0))
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
