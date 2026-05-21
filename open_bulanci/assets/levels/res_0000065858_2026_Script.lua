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
    engine.set_global(1, 4)
    engine.set_global(2, "A0A144D0-26B0-4049-A66F-7AE630965103")
    if get_local(v, args, 0, 0) == 1 then goto lbl_0x007a end
    engine.set_global(0, "Malaria")
    do return 0 end
    ::lbl_0x007a::
    engine.set_global(0, "Malárie")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.registerTimer(0, ((math.random(0, 10) * 1000) + 5000), 6)
    engine.loadPreface(65640)
    engine.setMusic(65869, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 65639))
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(432, 61, 65627))
    engine.insertView(engine.bindToSlot(engine.create_anim(492, 0, 0, {65782}), 6))
    engine.insertView(engine.bindToSlot(engine.create_anim(0, 0, 0, {65787}), 7))
    engine.insertView(engine.bindToSlot(engine.create_anim(280, 0, 0, {65784}), 8))
    engine.insertView(engine.bindToSlot(engine.create_anim(670, 390, 0, {65785}), 9))
    engine.insertView(engine.bindToSlot(engine.create_anim(273, 208, 0, {65788}), 10))
    engine.insertView(engine.bindToSlot(engine.create_anim(235, 88, 0, {65789}), 11))
    engine.insertView(engine.bindToSlot(engine.create_anim(690, 320, 0, {65783}), 12))
    engine.insertView(engine.createObstacle(334, 376, 395, 445))
    engine.insertView(engine.createObstacle(395, 354, 454, 406))
    engine.insertView(engine.createObstacle(120, 0, 312, 77))
    engine.insertView(engine.createObstacle(250, 77, 325, 108))
    engine.insertView(engine.createObstacle(290, 108, 353, 137))
    engine.insertView(engine.createObstacle(586, 237, 651, 284))
    engine.insertView(engine.createObstacle(313, 137, 352, 175))
    engine.insertView(engine.createImage(0, 0, 65628))
    engine.insertView(engine.bindToSlot(engine.create_anim(0, 236, 0, {65786}), 13))
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
    engine.playAnim(engine.getSlot(6), 0)
    engine.playAnim(engine.getSlot(7), 0)
    engine.playAnim(engine.getSlot(8), 0)
    engine.playAnim(engine.getSlot(9), 0)
    engine.playAnim(engine.getSlot(10), 0)
    engine.playAnim(engine.getSlot(11), 0)
    engine.playAnim(engine.getSlot(12), 0)
    engine.playAnim(engine.getSlot(13), 0)
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
