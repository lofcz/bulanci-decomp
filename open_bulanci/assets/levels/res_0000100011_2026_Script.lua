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

local function fn_0x0000(...)
    local args = {...}
    local v = {}
    if engine.teleportPlayerTo(get_local(v, args, 0, 0), get_local(v, args, 0, 1), get_local(v, args, 0, 2), 0) == 0 then goto lbl_0x005e end
    if engine.isNet() == 0 then goto lbl_0x0058 end
    engine.strmSetSize(engine.get_global(3), 0)
    engine.strmWrite(engine.get_global(3), 1, 1)
    engine.strmWrite(engine.get_global(3), get_local(v, args, 0, 0), 1)
    engine.strmWrite(engine.get_global(3), get_local(v, args, 0, 1), 4)
    engine.strmWrite(engine.get_global(3), get_local(v, args, 0, 2), 4)
    engine.strmSend(engine.get_global(3))
    ::lbl_0x0058::
    do return 1 end
    ::lbl_0x005e::
    do return 0 end
end

local function fn_0x04a3(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 1))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x04b5::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x04c9 end
if _sw == 1 then goto lbl_0x04ea end
    ::lbl_0x04c9::
    if fn_0x0000(get_local(v, args, 2, 2), 9, 11) == 1 then goto lbl_0x0537 end
    engine.goto(1291)
    ::lbl_0x04ea::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x0537 end
    engine.goto(1291)
    ::lbl_0x050b::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 2 then goto lbl_0x0528 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x0528::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x04b5 end
    do return 0 end
    ::lbl_0x0537::
    do return 1 end
end

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.set_global(1, 1000)
    engine.set_global(2, "C05A377E-4B25-4392-8703-80D4D36C1094")
    engine.set_global(0, "Válka")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(3, 0)
    if engine.isNet() == 0 then goto lbl_0x00f1 end
    engine.set_global(3, engine.strmCreateMem(4096, 4096))
    engine.setCommStrm(engine.get_global(3))
    ::lbl_0x00f1::
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createImage(18, 446, 100009))
    engine.insertView(engine.createImage(0, 10, 100009))
    engine.insertView(engine.createImage(734, 466, 100009))
    engine.insertView(engine.createObstacle(0, 252, 14, 265))
    engine.insertView(engine.createObstacle(359, 8, 373, 26))
    engine.insertView(engine.createObstacle(353, 10, 383, 34))
    engine.insertView(engine.createObstacle(269, 105, 294, 117))
    engine.insertView(engine.createObstacle(266, 79, 278, 86))
    engine.insertView(engine.createObstacle(187, 118, 203, 138))
    engine.insertView(engine.createObstacle(163, 136, 176, 147))
    engine.insertView(engine.createObstacle(87, 182, 107, 200))
    engine.insertView(engine.createObstacle(7, 248, 44, 276))
    engine.insertView(engine.createObstacle(17, 230, 77, 258))
    engine.insertView(engine.createObstacle(43, 212, 97, 240))
    engine.insertView(engine.createObstacle(64, 190, 118, 223))
    engine.insertView(engine.createObstacle(102, 160, 151, 207))
    engine.insertView(engine.createObstacle(143, 142, 186, 184))
    engine.insertView(engine.createObstacle(172, 128, 217, 165))
    engine.insertView(engine.createObstacle(196, 108, 243, 152))
    engine.insertView(engine.createObstacle(236, 83, 279, 127))
    engine.insertView(engine.createObstacle(272, 67, 323, 103))
    engine.insertView(engine.createObstacle(302, 37, 356, 85))
    engine.insertView(engine.createObstacle(344, 25, 379, 63))
    engine.insertView(engine.createObstacle(12, 367, 90, 406))
    engine.insertView(engine.createObstacle(110, 366, 168, 418))
    engine.insertView(engine.createObstacle(181, 369, 426, 412))
    engine.setInsertMode(2)
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 303, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(671, 0, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(169, 360, 100010), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(99, 355, 100008), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 354, 100006), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 0, 100007), 0))
    engine.setInsertMode(0)
    if engine.isServer() == 0 then goto lbl_0x043d end
    engine.defineTraceArea(0, 9, 437, 89, 507, 14)
    engine.defineTraceArea(1, -5, -5, 75, 65, 14)
    engine.defineTraceArea(2, 725, 450, 805, 520, 14)
    ::lbl_0x043d::
    engine.insertBulanci()
    engine.insertVampires()
    engine.insertOpponent(7, 1, 50, 5)
    engine.insertOpponent(7, 1, 50, 5)
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(3) == 0 then goto lbl_0x0481 end
    engine.set_global(3, engine.strmDestroy(engine.get_global(3)))
    ::lbl_0x0481::
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
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x055a end
if _sw == 1 then goto lbl_0x0567 end
if _sw == 2 then goto lbl_0x057e end
    ::lbl_0x055a::
    fn_0x04a3(get_local(v, args, 0, 1))
    engine.goto(1429)
    ::lbl_0x0567::
    fn_0x0000(get_local(v, args, 0, 1), 739, 466)
    engine.goto(1429)
    ::lbl_0x057e::
    fn_0x0000(get_local(v, args, 0, 1), 9, 11)
    engine.goto(1429)
    ::lbl_0x0595::
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x05b8 end
if _sw == 1 then goto lbl_0x05bd end
if _sw == 2 then goto lbl_0x05c2 end
    ::lbl_0x05b8::
    engine.goto(1479)
    ::lbl_0x05bd::
    engine.goto(1479)
    ::lbl_0x05c2::
    engine.goto(1479)
    ::lbl_0x05c7::
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    set_local(v, args, 4, 0, engine.strmRead(get_local(v, args, 4, 4), 1))
    if get_local(v, args, 4, 0) ~= 1 then goto lbl_0x060e end
    set_local(v, args, 4, 3, engine.strmRead(get_local(v, args, 4, 4), 1))
    set_local(v, args, 4, 1, engine.strmRead(get_local(v, args, 4, 4), 4))
    set_local(v, args, 4, 2, engine.strmRead(get_local(v, args, 4, 4), 4))
    engine.teleportPlayerTo(get_local(v, args, 4, 3), get_local(v, args, 4, 1), get_local(v, args, 4, 2), 1)
    ::lbl_0x060e::
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    do return 0 end
end
