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

local function fn_0x0239(...)
    local args = {...}
    local v = {}
    if math.random(0, 5) == 0 then goto lbl_0x0265 end
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(625, 0, 65583))
    ::lbl_0x0265::
    if math.random(0, 5) == 0 then goto lbl_0x0290 end
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(678, 250, 65584))
    ::lbl_0x0290::
    do return 0 end
end

local function fn_0x0296(...)
    local args = {...}
    local v = {}
    if math.random(0, 5) == 6 then goto lbl_0x0310 end
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(404, 56, 65574))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(404, 56, 65573), -25))
    engine.setInsertMode(2)
    engine.insertView(engine.createObstacle(413, 93, 549, 138))
    engine.insertView(engine.createObstacle(428, 134, 486, 167))
    ::lbl_0x0310::
    if math.random(0, 5) == 6 then goto lbl_0x0373 end
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(664, 113, 65580))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(664, 113, 65575), -20))
    engine.setInsertMode(2)
    engine.insertView(engine.createObstacle(667, 259, 713, 297))
    ::lbl_0x0373::
    if math.random(0, 5) == 6 then goto lbl_0x03ec end
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(82, 237, 65577))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(82, 237, 65576), -20))
    engine.setInsertMode(2)
    engine.insertView(engine.createObstacle(130, 277, 178, 334))
    engine.insertView(engine.createObstacle(96, 321, 138, 359))
    ::lbl_0x03ec::
    if math.random(0, 5) == 6 then goto lbl_0x044f end
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(294, 361, 65579))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(294, 361, 65578), -25))
    engine.setInsertMode(2)
    engine.insertView(engine.createObstacle(311, 406, 346, 446))
    ::lbl_0x044f::
    if math.random(0, 5) == 6 then goto lbl_0x04b2 end
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(50, 71, 65582))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(50, 71, 65581), -25))
    engine.setInsertMode(2)
    engine.insertView(engine.createObstacle(67, 114, 110, 155))
    ::lbl_0x04b2::
    do return 0 end
end

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.set_global(1, 6)
    engine.set_global(2, "987EF758-1BD5-4954-B543-767F158B17AF")
    engine.set_global(0, "Exitus")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(3, 0)
    if engine.isNet() == 0 then goto lbl_0x008f end
    engine.set_global(3, engine.strmCreateMem(4096, 4096))
    engine.setCommStrm(engine.get_global(3))
    ::lbl_0x008f::
    engine.loadPreface(65589)
    engine.setMusic(65862, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 65638))
    engine.setInsertMode(2)
    engine.insertView(engine.enableFireThrough(engine.createObstacle(0, 140, 140, 175), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(140, 110, 180, 135), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(185, 0, 225, 50), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(225, 0, 259, 17), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(123, 129, 157, 150), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(0, 175, 17, 200), 1))
    engine.insertView(engine.createObstacle(343, 421, 386, 456))
    engine.insertView(engine.createObstacle(370, 399, 410, 430))
    engine.insertView(engine.createObstacle(385, 368, 441, 409))
    engine.insertView(engine.createObstacle(427, 349, 541, 387))
    engine.insertView(engine.createObstacle(541, 373, 583, 410))
    fn_0x0239()
    fn_0x0296()
    engine.setInsertMode(1)
    engine.insertBulanci()
    engine.insertVampires()
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(3) == 0 then goto lbl_0x01fb end
    engine.set_global(3, engine.strmDestroy(engine.get_global(3)))
    ::lbl_0x01fb::
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
