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

local function fn_0x0565(...)
    local args = {...}
    local v = {}
    if engine.isServer() == 0 then goto lbl_0x0594 end
    engine.timerSetData(0, ((math.random(0, 6) * 5000) + 30000))
    engine.timerStop(0)
    ::lbl_0x0594::
    do return 0 end
end

local function fn_0x059a(...)
    local args = {...}
    local v = {}
    if get_local(v, args, 0, 2) ~= get_local(v, args, 0, 3) then goto lbl_0x05d3 end
    engine.spawnAtView(1, get_local(v, args, 0, 4))
    if engine.isServer() == 0 then goto lbl_0x05d3 end
    engine.mapSet(engine.registerTimer(-1, 2000, 2), (get_local(v, args, 0, 0) + bit32.lshift(get_local(v, args, 0, 1), 8)))
    ::lbl_0x05d3::
    do return 0 end
end

local function fn_0x05d9(...)
    local args = {...}
    local v = {}
    set_local(v, args, 1, 0, bit32.rshift(get_local(v, args, 1, 4), 8))
    engine.spawnEnemyAt(get_local(v, args, 1, 1), get_local(v, args, 1, 2), get_local(v, args, 1, 3), get_local(v, args, 1, 0), 1)
    engine.spawnAtView(2, (get_local(v, args, 1, 0) + 10))
    if engine.isServer() == 0 then goto lbl_0x0638 end
    if engine.isNet() == 0 then goto lbl_0x0638 end
    engine.strmSetSize(engine.get_global(3), 0)
    engine.strmWrite(engine.get_global(3), 1, 1)
    engine.strmWrite(engine.get_global(3), get_local(v, args, 1, 4), 2)
    engine.strmSend(engine.get_global(3))
    ::lbl_0x0638::
    do return 0 end
end

local function fn_0x063e(...)
    local args = {...}
    local v = {}
    local _sw = bit32.band(get_local(v, args, 0, 0), 255)
if _sw == 1 then goto lbl_0x0685 end
if _sw == 2 then goto lbl_0x06a1 end
if _sw == 3 then goto lbl_0x06bd end
if _sw == 4 then goto lbl_0x06d9 end
    fn_0x05d9(150, 29, 3, get_local(v, args, 0, 0))
    engine.goto(1776)
    ::lbl_0x0685::
    fn_0x05d9(592, 32, 3, get_local(v, args, 0, 0))
    engine.goto(1776)
    ::lbl_0x06a1::
    fn_0x05d9(461, 440, 2, get_local(v, args, 0, 0))
    engine.goto(1776)
    ::lbl_0x06bd::
    fn_0x05d9(500, 440, 2, get_local(v, args, 0, 0))
    engine.goto(1776)
    ::lbl_0x06d9::
    fn_0x05d9(38, 409, 1, get_local(v, args, 0, 0))
    ::lbl_0x06f0::
    do return 0 end
end

local function fn_0x06f6(...)
    local args = {...}
    local v = {}
    engine.playAnim(engine.getSlot(6), 0)
    engine.spawnAtView(0, 255)
    if engine.isServer() == 0 then goto lbl_0x0742 end
    engine.timerStart(0)
    if engine.isNet() == 0 then goto lbl_0x0742 end
    engine.strmSetSize(engine.get_global(3), 0)
    engine.strmWrite(engine.get_global(3), 0, 1)
    engine.strmSend(engine.get_global(3))
    ::lbl_0x0742::
    do return 0 end
end

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.set_global(1, 3)
    engine.set_global(2, "C48AEB13-1BE6-41e1-A50E-7BBA39E43188")
    if get_local(v, args, 0, 0) == 1 then goto lbl_0x0082 end
    engine.set_global(0, "Steel works")
    do return 0 end
    ::lbl_0x0082::
    engine.set_global(0, "Noční směna")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(3, 0)
    if engine.isNet() == 0 then goto lbl_0x00c6 end
    engine.set_global(3, engine.strmCreateMem(1024, 1024))
    engine.setCommStrm(engine.get_global(3))
    ::lbl_0x00c6::
    if engine.isServer() == 0 then goto lbl_0x00e1 end
    engine.registerTimer(0, 0, 7)
    ::lbl_0x00e1::
    engine.loadPreface(65624)
    engine.setMusic(65863, 65538)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 65623))
    engine.insertView(engine.create_anim(701, 180, 3, {65780}))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(129, 99, 65622), -110))
    engine.setInsertMode(2)
    engine.insertView(engine.createImage(0, 0, 65621))
    engine.insertView(engine.createImage(736, 323, 65626))
    engine.insertView(engine.createImage(551, 425, 65620))
    engine.insertView(engine.createImage(713, 0, 65625))
    engine.insertView(engine.bindToSlot(engine.create_anim(538, -347, 0, {65781}), 6))
    engine.insertView(engine.createObstacle(685, 148, 799, 245))
    engine.insertView(engine.createObstacle(695, 254, 799, 362))
    engine.insertView(engine.createObstacle(0, 0, 27, 23))
    engine.insertView(engine.createObstacle(0, 496, 447, 515))
    engine.insertView(engine.createObstacle(524, 507, 799, 515))
    engine.insertView(engine.createObstacle(641, 0, 743, 60))
    engine.insertView(engine.createObstacle(623, 475, 724, 515))
    engine.insertView(engine.bindToSlot(engine.createObstacle(136, 17, 168, 27), 10))
    engine.insertView(engine.bindToSlot(engine.createObstacle(579, 20, 610, 30), 11))
    engine.insertView(engine.bindToSlot(engine.createObstacle(447, 450, 477, 460), 12))
    engine.insertView(engine.bindToSlot(engine.createObstacle(494, 450, 524, 460), 13))
    engine.insertView(engine.bindToSlot(engine.createObstacle(26, 390, 36, 431), 14))
    engine.insertView(engine.createObstacle(136, 0, 168, 17))
    engine.insertView(engine.createObstacle(579, 0, 610, 20))
    engine.insertView(engine.createObstacle(447, 460, 524, 515))
    engine.insertView(engine.createObstacle(0, 390, 26, 431))
    engine.insertView(engine.createObstacle(232, 148, 410, 220))
    engine.insertView(engine.createObstacle(200, 190, 232, 220))
    engine.insertView(engine.createObstacle(127, 220, 367, 296))
    engine.insertView(engine.createObstacle(162, 296, 320, 342))
    engine.insertView(engine.createObstacle(220, 240, 300, 370))
    engine.insertView(engine.createObstacle(320, 296, 342, 315))
    engine.insertView(engine.createObstacle(367, 232, 390, 256))
    engine.insertBulanci()
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(3) == 0 then goto lbl_0x03c2 end
    engine.set_global(3, engine.strmDestroy(engine.get_global(3)))
    ::lbl_0x03c2::
    do return 0 end
end

function OnBitmapEvt(...)
    local args = {...}
    local v = {}
    if get_local(v, args, 0, 0) ~= 6 then goto lbl_0x0417 end
    if get_local(v, args, 0, 1) == 0 then goto lbl_0x03fa end
    fn_0x0565()
    do return 0 end
    ::lbl_0x03fa::
    engine.animResume(engine.getSlot(6))
    if engine.isServer() == 0 then goto lbl_0x0417 end
    engine.timerSetData(0, 10000)
    ::lbl_0x0417::
    do return 0 end
end

function OnSlotPlaced(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 10 then goto lbl_0x0450 end
if _sw == 11 then goto lbl_0x046c end
if _sw == 12 then goto lbl_0x0488 end
if _sw == 13 then goto lbl_0x04a4 end
if _sw == 14 then goto lbl_0x04c0 end
    do return 0 end
    ::lbl_0x0450::
    fn_0x059a(3, get_local(v, args, 0, 1), get_local(v, args, 0, 2), 2, get_local(v, args, 0, 0))
    do return 0 end
    ::lbl_0x046c::
    fn_0x059a(4, get_local(v, args, 0, 1), get_local(v, args, 0, 2), 2, get_local(v, args, 0, 0))
    do return 0 end
    ::lbl_0x0488::
    fn_0x059a(2, get_local(v, args, 0, 1), get_local(v, args, 0, 2), 3, get_local(v, args, 0, 0))
    do return 0 end
    ::lbl_0x04a4::
    fn_0x059a(0, get_local(v, args, 0, 1), get_local(v, args, 0, 2), 3, get_local(v, args, 0, 0))
    do return 0 end
    ::lbl_0x04c0::
    fn_0x059a(1, get_local(v, args, 0, 1), get_local(v, args, 0, 2), 0, get_local(v, args, 0, 0))
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
    if engine.isServer() == 0 then goto lbl_0x0510 end
    if get_local(v, args, 0, 0) ~= 0 then goto lbl_0x0507 end
    fn_0x06f6()
    do return 0 end
    ::lbl_0x0507::
    fn_0x063e(engine.mapGet(get_local(v, args, 0, 0)))
    ::lbl_0x0510::
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
    local _sw = engine.strmRead(engine.get_global(3), 1)
if _sw == 0 then goto lbl_0x0545 end
if _sw == 1 then goto lbl_0x0551 end
    do return 0 end
    ::lbl_0x0545::
    fn_0x06f6()
    do return 0 end
    ::lbl_0x0551::
    fn_0x063e(engine.strmRead(engine.get_global(3), 2))
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    fn_0x0565()
    do return 0 end
end
