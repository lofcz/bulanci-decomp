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

local function fn_0x049c(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 3))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x04ae::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x04d2 end
if _sw == 1 then goto lbl_0x04f3 end
if _sw == 2 then goto lbl_0x0514 end
if _sw == 3 then goto lbl_0x0535 end
    ::lbl_0x04d2::
    if fn_0x0000(get_local(v, args, 2, 2), 9, 466) == 1 then goto lbl_0x0582 end
    engine.goto(1366)
    ::lbl_0x04f3::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x0582 end
    engine.goto(1366)
    ::lbl_0x0514::
    if fn_0x0000(get_local(v, args, 2, 2), 9, 11) == 1 then goto lbl_0x0582 end
    engine.goto(1366)
    ::lbl_0x0535::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 11) == 1 then goto lbl_0x0582 end
    engine.goto(1366)
    ::lbl_0x0556::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 4 then goto lbl_0x0573 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x0573::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x04ae end
    do return 0 end
    ::lbl_0x0582::
    do return 1 end
end

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.set_global(1, 1000)
    engine.set_global(2, "3838938E-851E-4B0F-AF0B-1CB280234F6B")
    engine.set_global(0, "Tutorial 1")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(3, 0)
    if engine.isNet() == 0 then goto lbl_0x00fb end
    engine.set_global(3, engine.strmCreateMem(4096, 4096))
    engine.setCommStrm(engine.get_global(3))
    ::lbl_0x00fb::
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createObstacle(737, 366, 787, 383))
    engine.insertView(engine.createObstacle(425, 398, 474, 415))
    engine.insertView(engine.createObstacle(327, 420, 371, 435))
    engine.insertView(engine.createObstacle(321, 137, 367, 150))
    engine.insertView(engine.createObstacle(247, 133, 294, 148))
    engine.insertView(engine.createObstacle(15, 136, 61, 152))
    engine.insertView(engine.createObstacle(127, 307, 175, 333))
    engine.insertView(engine.createObstacle(655, 302, 730, 359))
    engine.insertView(engine.createObstacle(114, 276, 187, 324))
    engine.insertView(engine.createObstacle(0, 92, 77, 143))
    engine.insertView(engine.createObstacle(724, 325, 800, 373))
    engine.insertView(engine.createObstacle(312, 383, 385, 428))
    engine.insertView(engine.createObstacle(413, 363, 489, 407))
    engine.insertView(engine.createObstacle(235, 90, 383, 140))
    engine.insertView(engine.createObstacle(402, 70, 518, 162))
    engine.insertView(engine.createObstacle(516, 310, 632, 399))
    engine.insertView(engine.createObstacle(197, 424, 310, 515))
    engine.insertView(engine.createImage(360, 230, 100006))
    engine.insertView(engine.createImage(313, 449, 100006))
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(671, 0, 100008))
    engine.insertView(engine.createImage(0, 303, 100007))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(112, 248, 100005), -57))
    engine.insertView(engine.setOrderAxis(engine.createImage(723, 299, 100005), -62))
    engine.insertView(engine.setOrderAxis(engine.createImage(652, 273, 100005), -59))
    engine.insertView(engine.setOrderAxis(engine.createImage(412, 331, 100005), -55))
    engine.insertView(engine.setOrderAxis(engine.createImage(311, 350, 100005), -54))
    engine.insertView(engine.setOrderAxis(engine.createImage(307, 65, 100005), -61))
    engine.insertView(engine.setOrderAxis(engine.createImage(232, 63, 100005), -59))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 67, 100005), -63))
    engine.insertView(engine.setOrderAxis(engine.createImage(194, 395, 100004), -86))
    engine.insertView(engine.setOrderAxis(engine.createImage(514, 282, 100004), -87))
    engine.insertView(engine.setOrderAxis(engine.createImage(400, 44, 100004), -90))
    engine.setInsertMode(0)
    if engine.isServer() == 0 then goto lbl_0x0422 end
    engine.defineTraceArea(0, 312, 444, 392, 514, 14)
    engine.defineTraceArea(1, 359, 225, 439, 295, 14)
    ::lbl_0x0422::
    engine.insertBulanci()
    engine.insertOpponent(7, 3, 120, 5)
    engine.insertOpponent(7, 2, 120, 4)
    engine.insertOpponent(7, 2, 120, 1)
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(3) == 0 then goto lbl_0x047a end
    engine.set_global(3, engine.strmDestroy(engine.get_global(3)))
    ::lbl_0x047a::
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
if _sw == 0 then goto lbl_0x059d end
if _sw == 1 then goto lbl_0x05b4 end
    ::lbl_0x059d::
    fn_0x0000(get_local(v, args, 0, 1), 373, 241)
    engine.goto(1473)
    ::lbl_0x05b4::
    fn_0x049c(get_local(v, args, 0, 1))
    engine.goto(1473)
    ::lbl_0x05c1::
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x05dc end
if _sw == 1 then goto lbl_0x05e1 end
    ::lbl_0x05dc::
    engine.goto(1510)
    ::lbl_0x05e1::
    engine.goto(1510)
    ::lbl_0x05e6::
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    set_local(v, args, 4, 0, engine.strmRead(get_local(v, args, 4, 4), 1))
    if get_local(v, args, 4, 0) ~= 1 then goto lbl_0x062d end
    set_local(v, args, 4, 3, engine.strmRead(get_local(v, args, 4, 4), 1))
    set_local(v, args, 4, 1, engine.strmRead(get_local(v, args, 4, 4), 4))
    set_local(v, args, 4, 2, engine.strmRead(get_local(v, args, 4, 4), 4))
    engine.teleportPlayerTo(get_local(v, args, 4, 3), get_local(v, args, 4, 1), get_local(v, args, 4, 2), 1)
    ::lbl_0x062d::
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    do return 0 end
end
