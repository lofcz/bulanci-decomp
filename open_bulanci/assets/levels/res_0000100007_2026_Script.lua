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
    engine.set_global(2, "8680C86D-EA25-41AA-8562-04156BF8E4B6")
    engine.set_global(0, "Smrtící zájezd")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(3, 0)
    if engine.isNet() == 0 then goto lbl_0x0103 end
    engine.set_global(3, engine.strmCreateMem(4096, 4096))
    engine.setCommStrm(engine.get_global(3))
    ::lbl_0x0103::
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createImage(343, 98, 100005))
    engine.insertView(engine.createImage(414, 106, 100005))
    engine.insertView(engine.createImage(486, 108, 100005))
    engine.insertView(engine.createImage(281, 27, 100005))
    engine.insertView(engine.createImage(274, 74, 100005))
    engine.insertView(engine.createObstacle(405, 416, 455, 431))
    engine.insertView(engine.createObstacle(412, 274, 423, 289))
    engine.insertView(engine.createObstacle(382, 279, 395, 289))
    engine.insertView(engine.createObstacle(444, 221, 458, 230))
    engine.insertView(engine.createObstacle(461, 238, 472, 275))
    engine.insertView(engine.createObstacle(434, 230, 461, 277))
    engine.insertView(engine.createObstacle(423, 239, 433, 277))
    engine.insertView(engine.createObstacle(424, 277, 460, 306))
    engine.insertView(engine.createObstacle(389, 289, 424, 322))
    engine.insertView(engine.createObstacle(682, 239, 700, 291))
    engine.insertView(engine.createObstacle(90, 138, 99, 147))
    engine.insertView(engine.createObstacle(82, 76, 93, 90))
    engine.insertView(engine.createImage(555, 178, 100005))
    engine.insertView(engine.createImage(220, 237, 100005))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(0, 176, 19, 194), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(30, 151, 109, 166), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(113, 137, 152, 160), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(145, 110, 181, 135), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(231, 0, 259, 13), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(185, 0, 231, 42), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(0, 165, 114, 176), 1))
    engine.insertView(engine.createObstacle(88, 91, 115, 137))
    engine.insertView(engine.createObstacle(65, 89, 88, 141))
    engine.insertView(engine.createObstacle(653, 377, 678, 383))
    engine.insertView(engine.createObstacle(466, 420, 484, 437))
    engine.insertView(engine.createObstacle(493, 405, 508, 421))
    engine.insertView(engine.createObstacle(523, 376, 531, 387))
    engine.insertView(engine.createObstacle(486, 403, 492, 440))
    engine.insertView(engine.createObstacle(650, 358, 663, 368))
    engine.insertView(engine.createObstacle(558, 343, 648, 352))
    engine.insertView(engine.createObstacle(533, 350, 649, 369))
    engine.insertView(engine.createObstacle(531, 379, 551, 401))
    engine.insertView(engine.createObstacle(488, 381, 529, 412))
    engine.insertView(engine.createObstacle(466, 415, 514, 425))
    engine.insertView(engine.createObstacle(453, 428, 490, 450))
    engine.insertView(engine.createObstacle(628, 379, 649, 385))
    engine.insertView(engine.createObstacle(509, 368, 685, 379))
    engine.insertView(engine.createObstacle(649, 381, 692, 407))
    engine.insertView(engine.createObstacle(732, 33, 745, 73))
    engine.insertView(engine.createObstacle(696, 25, 732, 87))
    engine.insertView(engine.createObstacle(653, 31, 693, 53))
    engine.insertView(engine.createObstacle(52, 376, 63, 416))
    engine.insertView(engine.createObstacle(21, 360, 42, 367))
    engine.insertView(engine.createObstacle(15, 418, 45, 426))
    engine.insertView(engine.createObstacle(0, 367, 52, 418))
    engine.insertView(engine.createObstacle(63, 359, 104, 396))
    engine.insertView(engine.createObstacle(479, 174, 492, 179))
    engine.insertView(engine.createObstacle(390, 148, 405, 155))
    engine.insertView(engine.createObstacle(356, 138, 366, 143))
    engine.insertView(engine.createObstacle(310, 124, 320, 130))
    engine.insertView(engine.createObstacle(423, 142, 436, 147))
    engine.insertView(engine.createObstacle(456, 153, 473, 161))
    engine.insertView(engine.createObstacle(492, 158, 541, 163))
    engine.insertView(engine.createObstacle(546, 152, 552, 159))
    engine.insertView(engine.createObstacle(529, 157, 542, 167))
    engine.insertView(engine.createObstacle(508, 161, 526, 173))
    engine.insertView(engine.createObstacle(493, 163, 506, 181))
    engine.insertView(engine.createObstacle(436, 159, 492, 174))
    engine.insertView(engine.createObstacle(406, 147, 456, 159))
    engine.insertView(engine.createObstacle(379, 129, 398, 138))
    engine.insertView(engine.createObstacle(340, 117, 353, 126))
    engine.insertView(engine.createObstacle(366, 138, 422, 147))
    engine.insertView(engine.createObstacle(320, 125, 379, 138))
    engine.insertView(engine.createObstacle(290, 110, 339, 124))
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(287, 38, 100004))
    engine.insertView(engine.createImage(675, 113, 100006))
    engine.setInsertMode(1)
    engine.setInsertMode(0)
    if engine.isServer() == 0 then goto lbl_0x0790 end
    engine.defineTraceArea(0, 725, 297, 805, 367, 14)
    engine.defineTraceArea(1, -5, 224, 75, 294, 14)
    ::lbl_0x0790::
    engine.insertBulanci()
    engine.insertVampires()
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(3) == 0 then goto lbl_0x07aa end
    engine.set_global(3, engine.strmDestroy(engine.get_global(3)))
    ::lbl_0x07aa::
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
if _sw == 0 then goto lbl_0x07e1 end
if _sw == 1 then goto lbl_0x07f8 end
    ::lbl_0x07e1::
    fn_0x0000(get_local(v, args, 0, 1), 9, 240)
    engine.goto(2063)
    ::lbl_0x07f8::
    fn_0x0000(get_local(v, args, 0, 1), 739, 313)
    engine.goto(2063)
    ::lbl_0x080f::
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x082a end
if _sw == 1 then goto lbl_0x082f end
    ::lbl_0x082a::
    engine.goto(2100)
    ::lbl_0x082f::
    engine.goto(2100)
    ::lbl_0x0834::
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    set_local(v, args, 4, 0, engine.strmRead(get_local(v, args, 4, 4), 1))
    if get_local(v, args, 4, 0) ~= 1 then goto lbl_0x087b end
    set_local(v, args, 4, 3, engine.strmRead(get_local(v, args, 4, 4), 1))
    set_local(v, args, 4, 1, engine.strmRead(get_local(v, args, 4, 4), 4))
    set_local(v, args, 4, 2, engine.strmRead(get_local(v, args, 4, 4), 4))
    engine.teleportPlayerTo(get_local(v, args, 4, 3), get_local(v, args, 4, 1), get_local(v, args, 4, 2), 1)
    ::lbl_0x087b::
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    do return 0 end
end
