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

local function fn_0x05b0(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 1))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x05c2::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x05d6 end
if _sw == 1 then goto lbl_0x05f7 end
    ::lbl_0x05d6::
    if fn_0x0000(get_local(v, args, 2, 2), 694, 423) == 1 then goto lbl_0x0644 end
    engine.goto(1560)
    ::lbl_0x05f7::
    if fn_0x0000(get_local(v, args, 2, 2), 404, 258) == 1 then goto lbl_0x0644 end
    engine.goto(1560)
    ::lbl_0x0618::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 2 then goto lbl_0x0635 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x0635::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x05c2 end
    do return 0 end
    ::lbl_0x0644::
    do return 1 end
end

local function fn_0x064a(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 1))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x065c::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x0670 end
if _sw == 1 then goto lbl_0x0691 end
    ::lbl_0x0670::
    if fn_0x0000(get_local(v, args, 2, 2), 22, 25) == 1 then goto lbl_0x06de end
    engine.goto(1714)
    ::lbl_0x0691::
    if fn_0x0000(get_local(v, args, 2, 2), 404, 258) == 1 then goto lbl_0x06de end
    engine.goto(1714)
    ::lbl_0x06b2::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 2 then goto lbl_0x06cf end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x06cf::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x065c end
    do return 0 end
    ::lbl_0x06de::
    do return 1 end
end

local function fn_0x06e4(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 1))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x06f6::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x070a end
if _sw == 1 then goto lbl_0x072b end
    ::lbl_0x070a::
    if fn_0x0000(get_local(v, args, 2, 2), 22, 25) == 1 then goto lbl_0x0778 end
    engine.goto(1868)
    ::lbl_0x072b::
    if fn_0x0000(get_local(v, args, 2, 2), 694, 423) == 1 then goto lbl_0x0778 end
    engine.goto(1868)
    ::lbl_0x074c::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 2 then goto lbl_0x0769 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x0769::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x06f6 end
    do return 0 end
    ::lbl_0x0778::
    do return 1 end
end

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.set_global(1, 1000)
    engine.set_global(2, "311CAC12-DA5E-4275-A8C5-B9EE0815F835")
    engine.set_global(0, "Western")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(3, 0)
    if engine.isNet() == 0 then goto lbl_0x00f5 end
    engine.set_global(3, engine.strmCreateMem(4096, 4096))
    engine.setCommStrm(engine.get_global(3))
    ::lbl_0x00f5::
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createObstacle(721, 0, 732, 20))
    engine.insertView(engine.createObstacle(434, 171, 455, 183))
    engine.insertView(engine.createObstacle(460, 170, 478, 193))
    engine.insertView(engine.createObstacle(481, 190, 500, 202))
    engine.insertView(engine.createObstacle(500, 189, 568, 214))
    engine.insertView(engine.createObstacle(480, 163, 617, 189))
    engine.insertView(engine.createObstacle(481, 139, 637, 163))
    engine.insertView(engine.createObstacle(481, 116, 658, 139))
    engine.insertView(engine.createObstacle(481, 91, 674, 116))
    engine.insertView(engine.createObstacle(370, 0, 721, 47))
    engine.insertView(engine.createObstacle(424, 91, 481, 170))
    engine.insertView(engine.createObstacle(315, 149, 356, 159))
    engine.insertView(engine.createObstacle(289, 0, 304, 47))
    engine.insertView(engine.createObstacle(261, 87, 274, 111))
    engine.insertView(engine.createObstacle(253, 112, 275, 134))
    engine.insertView(engine.createObstacle(303, 0, 370, 148))
    engine.insertView(engine.createObstacle(275, 47, 303, 148))
    engine.insertView(engine.createObstacle(482, 463, 503, 473))
    engine.insertView(engine.createObstacle(459, 453, 510, 463))
    engine.insertView(engine.createObstacle(442, 432, 523, 452))
    engine.insertView(engine.createObstacle(456, 412, 541, 431))
    engine.insertView(engine.createObstacle(473, 393, 555, 412))
    engine.insertView(engine.createObstacle(488, 374, 571, 393))
    engine.insertView(engine.createObstacle(502, 354, 587, 373))
    engine.insertView(engine.createObstacle(503, 341, 600, 354))
    engine.insertView(engine.createObstacle(542, 303, 574, 314))
    engine.insertView(engine.createObstacle(527, 292, 541, 316))
    engine.insertView(engine.createObstacle(516, 314, 589, 325))
    engine.insertView(engine.createObstacle(506, 325, 607, 341))
    engine.insertView(engine.createObstacle(269, 263, 321, 273))
    engine.insertView(engine.createObstacle(257, 269, 335, 280))
    engine.insertView(engine.createObstacle(247, 279, 343, 289))
    engine.insertView(engine.createObstacle(262, 322, 321, 329))
    engine.insertView(engine.createObstacle(250, 307, 337, 322))
    engine.insertView(engine.createObstacle(244, 288, 344, 307))
    engine.insertView(engine.createObstacle(152, 191, 188, 218))
    engine.insertView(engine.createObstacle(57, 310, 95, 336))
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(437, 0, 100012))
    engine.insertView(engine.createImage(537, 203, 100009))
    engine.insertView(engine.createImage(0, 348, 100004))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(308, 0, 100011), -18))
    engine.insertView(engine.setOrderAxis(engine.createImage(240, 0, 100010), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(433, 241, 100008), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(244, 249, 100007), -37))
    engine.insertView(engine.setOrderAxis(engine.createImage(31, 17, 100006), -8))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 105, 100005), -9))
    engine.setInsertMode(0)
    if engine.isServer() == 0 then goto lbl_0x0575 end
    engine.defineTraceArea(0, 8, 9, 88, 79, 14)
    engine.defineTraceArea(1, 680, 407, 760, 477, 14)
    engine.defineTraceArea(2, 390, 242, 470, 312, 14)
    ::lbl_0x0575::
    engine.insertBulanci()
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(3) == 0 then goto lbl_0x058e end
    engine.set_global(3, engine.strmDestroy(engine.get_global(3)))
    ::lbl_0x058e::
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
if _sw == 0 then goto lbl_0x079b end
if _sw == 1 then goto lbl_0x07a8 end
if _sw == 2 then goto lbl_0x07b5 end
    ::lbl_0x079b::
    fn_0x05b0(get_local(v, args, 0, 1))
    engine.goto(1986)
    ::lbl_0x07a8::
    fn_0x064a(get_local(v, args, 0, 1))
    engine.goto(1986)
    ::lbl_0x07b5::
    fn_0x06e4(get_local(v, args, 0, 1))
    engine.goto(1986)
    ::lbl_0x07c2::
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x07e5 end
if _sw == 1 then goto lbl_0x07ea end
if _sw == 2 then goto lbl_0x07ef end
    ::lbl_0x07e5::
    engine.goto(2036)
    ::lbl_0x07ea::
    engine.goto(2036)
    ::lbl_0x07ef::
    engine.goto(2036)
    ::lbl_0x07f4::
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    set_local(v, args, 4, 0, engine.strmRead(get_local(v, args, 4, 4), 1))
    if get_local(v, args, 4, 0) ~= 1 then goto lbl_0x083b end
    set_local(v, args, 4, 3, engine.strmRead(get_local(v, args, 4, 4), 1))
    set_local(v, args, 4, 1, engine.strmRead(get_local(v, args, 4, 4), 4))
    set_local(v, args, 4, 2, engine.strmRead(get_local(v, args, 4, 4), 4))
    engine.teleportPlayerTo(get_local(v, args, 4, 3), get_local(v, args, 4, 1), get_local(v, args, 4, 2), 1)
    ::lbl_0x083b::
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    do return 0 end
end
