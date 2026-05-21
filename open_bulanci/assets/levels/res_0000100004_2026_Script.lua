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

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.set_global(1, 1000)
    engine.set_global(2, "9B98D1B9-3B41-4772-9FEB-943FBEC4FD66")
    engine.set_global(0, "No jo no")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(3, 0)
    if engine.isNet() == 0 then goto lbl_0x00f7 end
    engine.set_global(3, engine.strmCreateMem(4096, 4096))
    engine.setCommStrm(engine.get_global(3))
    ::lbl_0x00f7::
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createObstacle(461, 409, 480, 499))
    engine.insertView(engine.createObstacle(461, 391, 675, 409))
    engine.insertView(engine.createObstacle(696, 337, 784, 355))
    engine.insertView(engine.createObstacle(677, 337, 696, 410))
    engine.insertView(engine.createObstacle(353, 248, 372, 428))
    engine.insertView(engine.createObstacle(695, 121, 785, 140))
    engine.insertView(engine.createObstacle(245, 229, 785, 248))
    engine.insertView(engine.createObstacle(785, 14, 800, 499))
    engine.insertView(engine.createObstacle(245, 319, 264, 499))
    engine.insertView(engine.createObstacle(137, 140, 156, 410))
    engine.insertView(engine.createObstacle(137, 121, 588, 140))
    engine.insertView(engine.createObstacle(12, 0, 800, 14))
    engine.insertView(engine.createObstacle(12, 499, 800, 515))
    engine.insertView(engine.createObstacle(0, 0, 12, 515))
    engine.setInsertMode(2)
    engine.setInsertMode(1)
    engine.setInsertMode(0)
    if engine.isServer() == 0 then goto lbl_0x02e8 end
    engine.defineTraceArea(0, 484, 419, 564, 489, 14)
    engine.defineTraceArea(1, 701, 257, 781, 327, 14)
    engine.defineTraceArea(2, 701, 149, 781, 219, 14)
    engine.defineTraceArea(3, 17, 23, 97, 93, 14)
    ::lbl_0x02e8::
    engine.insertBulanci()
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(3) == 0 then goto lbl_0x0301 end
    engine.set_global(3, engine.strmDestroy(engine.get_global(3)))
    ::lbl_0x0301::
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
if _sw == 0 then goto lbl_0x0348 end
if _sw == 1 then goto lbl_0x035f end
if _sw == 2 then goto lbl_0x0376 end
if _sw == 3 then goto lbl_0x038d end
    ::lbl_0x0348::
    fn_0x0000(get_local(v, args, 0, 1), 31, 39)
    engine.goto(932)
    ::lbl_0x035f::
    fn_0x0000(get_local(v, args, 0, 1), 498, 435)
    engine.goto(932)
    ::lbl_0x0376::
    fn_0x0000(get_local(v, args, 0, 1), 715, 273)
    engine.goto(932)
    ::lbl_0x038d::
    fn_0x0000(get_local(v, args, 0, 1), 715, 165)
    engine.goto(932)
    ::lbl_0x03a4::
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x03cf end
if _sw == 1 then goto lbl_0x03d4 end
if _sw == 2 then goto lbl_0x03d9 end
if _sw == 3 then goto lbl_0x03de end
    ::lbl_0x03cf::
    engine.goto(995)
    ::lbl_0x03d4::
    engine.goto(995)
    ::lbl_0x03d9::
    engine.goto(995)
    ::lbl_0x03de::
    engine.goto(995)
    ::lbl_0x03e3::
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    set_local(v, args, 4, 0, engine.strmRead(get_local(v, args, 4, 4), 1))
    if get_local(v, args, 4, 0) ~= 1 then goto lbl_0x042a end
    set_local(v, args, 4, 3, engine.strmRead(get_local(v, args, 4, 4), 1))
    set_local(v, args, 4, 1, engine.strmRead(get_local(v, args, 4, 4), 4))
    set_local(v, args, 4, 2, engine.strmRead(get_local(v, args, 4, 4), 4))
    engine.teleportPlayerTo(get_local(v, args, 4, 3), get_local(v, args, 4, 1), get_local(v, args, 4, 2), 1)
    ::lbl_0x042a::
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    do return 0 end
end
