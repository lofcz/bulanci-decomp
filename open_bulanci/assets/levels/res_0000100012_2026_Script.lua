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
    engine.set_global(2, "40DB7B80-6F82-47CA-BC44-081F407013DE")
    engine.set_global(0, "Ať žijou Bulánci!")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(3, 0)
    if engine.isNet() == 0 then goto lbl_0x0109 end
    engine.set_global(3, engine.strmCreateMem(4096, 4096))
    engine.setCommStrm(engine.get_global(3))
    ::lbl_0x0109::
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createObstacle(74, 0, 85, 29))
    engine.insertView(engine.createImage(7, 46, 100011))
    engine.insertView(engine.createImage(51, 8, 100011))
    engine.insertView(engine.createImage(49, 44, 100011))
    engine.insertView(engine.createImage(4, 8, 100011))
    engine.insertView(engine.createImage(0, 29, 100011))
    engine.insertView(engine.createImage(58, 26, 100011))
    engine.insertView(engine.createImage(28, 51, 100011))
    engine.insertView(engine.createImage(28, 0, 100011))
    engine.insertView(engine.createObstacle(165, 347, 196, 355))
    engine.insertView(engine.createObstacle(126, 346, 166, 371))
    engine.insertView(engine.createObstacle(66, 485, 104, 515))
    engine.insertView(engine.createObstacle(161, 308, 181, 317))
    engine.insertView(engine.createObstacle(134, 302, 164, 316))
    engine.insertView(engine.createObstacle(106, 299, 137, 316))
    engine.insertView(engine.createObstacle(100, 336, 111, 345))
    engine.insertView(engine.createObstacle(93, 316, 116, 337))
    engine.insertView(engine.createObstacle(110, 315, 201, 349))
    engine.insertView(engine.createImage(125, 337, 100005))
    engine.insertView(engine.createObstacle(680, 174, 715, 198))
    engine.insertView(engine.createObstacle(668, 188, 701, 198))
    engine.insertView(engine.createObstacle(654, 202, 671, 237))
    engine.insertView(engine.createObstacle(720, 230, 738, 267))
    engine.insertView(engine.createObstacle(669, 194, 720, 277))
    engine.insertView(engine.createObstacle(735, 140, 752, 170))
    engine.insertView(engine.createObstacle(736, 129, 769, 143))
    engine.insertView(engine.createObstacle(689, 77, 736, 177))
    engine.insertView(engine.createObstacle(735, 60, 800, 133))
    engine.insertView(engine.createObstacle(359, 273, 376, 289))
    engine.insertView(engine.createObstacle(408, 273, 417, 282))
    engine.insertView(engine.createObstacle(374, 271, 409, 290))
    engine.insertView(engine.createObstacle(397, 210, 413, 223))
    engine.insertView(engine.createObstacle(356, 211, 369, 221))
    engine.insertView(engine.createObstacle(367, 206, 401, 222))
    engine.insertView(engine.createObstacle(323, 225, 342, 230))
    engine.insertView(engine.createObstacle(320, 251, 345, 266))
    engine.insertView(engine.createObstacle(309, 234, 317, 253))
    engine.insertView(engine.createObstacle(316, 230, 341, 258))
    engine.insertView(engine.createObstacle(338, 221, 419, 273))
    engine.insertView(engine.createObstacle(238, 138, 269, 145))
    engine.insertView(engine.createObstacle(221, 163, 229, 170))
    engine.insertView(engine.createObstacle(220, 147, 228, 161))
    engine.insertView(engine.createObstacle(277, 147, 287, 169))
    engine.insertView(engine.createObstacle(228, 143, 279, 180))
    engine.insertView(engine.createObstacle(394, 420, 408, 430))
    engine.insertView(engine.createObstacle(363, 421, 382, 436))
    engine.insertView(engine.createObstacle(457, 368, 470, 384))
    engine.insertView(engine.createObstacle(418, 355, 454, 361))
    engine.insertView(engine.createObstacle(416, 360, 462, 371))
    engine.insertView(engine.createObstacle(408, 370, 460, 392))
    engine.insertView(engine.createObstacle(351, 403, 361, 412))
    engine.insertView(engine.createObstacle(344, 386, 360, 404))
    engine.insertView(engine.createObstacle(359, 378, 408, 425))
    engine.insertView(engine.createObstacle(600, 140, 608, 147))
    engine.insertView(engine.createObstacle(497, 102, 502, 135))
    engine.insertView(engine.createObstacle(600, 104, 609, 114))
    engine.insertView(engine.createObstacle(601, 113, 611, 141))
    engine.insertView(engine.createObstacle(588, 145, 599, 155))
    engine.insertView(engine.createObstacle(537, 154, 580, 164))
    engine.insertView(engine.createObstacle(573, 157, 586, 166))
    engine.insertView(engine.createObstacle(505, 87, 513, 97))
    engine.insertView(engine.createObstacle(542, 161, 575, 171))
    engine.insertView(engine.createObstacle(588, 98, 602, 147))
    engine.insertView(engine.createObstacle(501, 94, 514, 143))
    engine.insertView(engine.createObstacle(513, 85, 591, 159))
    engine.insertView(engine.createObstacle(152, 44, 182, 52))
    engine.insertView(engine.createObstacle(109, 42, 150, 80))
    engine.insertView(engine.createObstacle(81, 0, 191, 46))
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(553, 240, 100010))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(108, 45, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(76, 0, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(495, 74, 100006), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(668, 206, 100009), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(650, 170, 100009), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(730, 69, 100009), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(680, 110, 100009), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(685, 53, 100009), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(730, 33, 100009), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(341, 343, 100007), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(64, 479, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(132, 294, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(91, 303, 100004), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(103, 287, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(348, 237, 100008), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(304, 214, 100008), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(348, 194, 100008), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(216, 126, 100008), 0))
    engine.setInsertMode(0)
    if engine.isServer() == 0 then goto lbl_0x08c6 end
    engine.defineTraceArea(0, 725, 450, 805, 520, 14)
    engine.defineTraceArea(1, -5, -5, 75, 65, 14)
    ::lbl_0x08c6::
    engine.insertBulanci()
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(3) == 0 then goto lbl_0x08df end
    engine.set_global(3, engine.strmDestroy(engine.get_global(3)))
    ::lbl_0x08df::
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
if _sw == 0 then goto lbl_0x0916 end
if _sw == 1 then goto lbl_0x092d end
    ::lbl_0x0916::
    fn_0x0000(get_local(v, args, 0, 1), 9, 11)
    engine.goto(2372)
    ::lbl_0x092d::
    fn_0x0000(get_local(v, args, 0, 1), 739, 466)
    engine.goto(2372)
    ::lbl_0x0944::
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x095f end
if _sw == 1 then goto lbl_0x0964 end
    ::lbl_0x095f::
    engine.goto(2409)
    ::lbl_0x0964::
    engine.goto(2409)
    ::lbl_0x0969::
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    set_local(v, args, 4, 0, engine.strmRead(get_local(v, args, 4, 4), 1))
    if get_local(v, args, 4, 0) ~= 1 then goto lbl_0x09b0 end
    set_local(v, args, 4, 3, engine.strmRead(get_local(v, args, 4, 4), 1))
    set_local(v, args, 4, 1, engine.strmRead(get_local(v, args, 4, 4), 4))
    set_local(v, args, 4, 2, engine.strmRead(get_local(v, args, 4, 4), 4))
    engine.teleportPlayerTo(get_local(v, args, 4, 3), get_local(v, args, 4, 1), get_local(v, args, 4, 2), 1)
    ::lbl_0x09b0::
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    do return 0 end
end
