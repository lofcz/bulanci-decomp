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

local function fn_0x06c2(...)
    local args = {...}
    local v = {}
    if get_local(v, args, 0, 1) == get_local(v, args, 0, 2) then goto lbl_0x06da end
    engine.spawnAtView(3, get_local(v, args, 0, 0))
    do return 0 end
    ::lbl_0x06da::
    engine.spawnAtView(4, get_local(v, args, 0, 0))
    do return 0 end
end

local function fn_0x06e8(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 2, 2)
if _sw == 11 then goto lbl_0x071b end
if _sw == 13 then goto lbl_0x07bc end
if _sw == 15 then goto lbl_0x081b end
if _sw == 17 then goto lbl_0x077a end
if _sw == 19 then goto lbl_0x087a end
    do return 0 end
    ::lbl_0x071b::
    if engine.get_global(3) > 8 then goto lbl_0x090e end
    set_local(v, args, 2, 0, math.floor(engine.get_global(3) / 3))
    engine.set_global(3, (engine.get_global(3) + 1))
    set_local(v, args, 2, 1, math.floor(engine.get_global(3) / 3))
    fn_0x06c2(10, get_local(v, args, 2, 1), 3)
    if get_local(v, args, 2, 0) == get_local(v, args, 2, 1) then goto lbl_0x08e1 end
    engine.seekAnim(engine.getSlot(10), get_local(v, args, 2, 0))
    engine.playAnim(engine.getSlot(10), 0)
    engine.goto(2273)
    ::lbl_0x077a::
    if engine.get_global(6) > 1 then goto lbl_0x090e end
    fn_0x06c2(16, engine.get_global(6), 1)
    engine.seekAnim(engine.getSlot(16), engine.get_global(6))
    engine.playAnim(engine.getSlot(16), 0)
    engine.set_global(6, (engine.get_global(6) + 1))
    engine.goto(2273)
    ::lbl_0x07bc::
    if engine.get_global(4) > 3 then goto lbl_0x090e end
    set_local(v, args, 2, 0, math.floor(engine.get_global(4) / 2))
    engine.set_global(4, (engine.get_global(4) + 1))
    set_local(v, args, 2, 1, math.floor(engine.get_global(4) / 2))
    fn_0x06c2(12, get_local(v, args, 2, 1), 2)
    if get_local(v, args, 2, 0) == get_local(v, args, 2, 1) then goto lbl_0x08e1 end
    engine.seekAnim(engine.getSlot(12), get_local(v, args, 2, 0))
    engine.playAnim(engine.getSlot(12), 0)
    engine.goto(2273)
    ::lbl_0x081b::
    if engine.get_global(5) > 3 then goto lbl_0x090e end
    set_local(v, args, 2, 0, math.floor(engine.get_global(5) / 2))
    engine.set_global(5, (engine.get_global(5) + 1))
    set_local(v, args, 2, 1, math.floor(engine.get_global(5) / 2))
    fn_0x06c2(14, get_local(v, args, 2, 1), 2)
    if get_local(v, args, 2, 0) == get_local(v, args, 2, 1) then goto lbl_0x08e1 end
    engine.seekAnim(engine.getSlot(14), get_local(v, args, 2, 0))
    engine.playAnim(engine.getSlot(14), 0)
    engine.goto(2273)
    ::lbl_0x087a::
    engine.set_global(13, (engine.get_global(13) + 1))
    if engine.get_global(13) > 7 then goto lbl_0x08a0 end
    engine.spawnAtView(1, 9)
    engine.goto(2273)
    ::lbl_0x08a0::
    engine.spawnAtView(2, 9)
    engine.animResume(engine.getSlot(9))
    engine.showView(engine.getSlot(9))
    engine.showView(engine.getSlot(19))
    engine.hideView(engine.getSlot(18))
    engine.hideView(engine.getSlot(20))
    engine.playAnim(engine.getSlot(20), 0)
    engine.set_global(10, 10)
    ::lbl_0x08e1::
    if engine.isNet() ~= 1 then goto lbl_0x090e end
    engine.strmSetSize(engine.get_global(11), 0)
    engine.strmWrite(engine.get_global(11), 2, 1)
    engine.strmWrite(engine.get_global(11), get_local(v, args, 2, 2), 1)
    engine.strmSend(engine.get_global(11))
    ::lbl_0x090e::
    do return 0 end
end

local function fn_0x0a44(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 1)
if _sw == 0 then goto lbl_0x0a79 end
if _sw == 1 then goto lbl_0x0a69 end
    do return engine.teleportPlayerTo(get_local(v, args, 0, 0), 704, 387, get_local(v, args, 0, 2)) end
    ::lbl_0x0a69::
    do return engine.teleportPlayerTo(get_local(v, args, 0, 0), 452, 77, get_local(v, args, 0, 2)) end
    ::lbl_0x0a79::
    do return engine.teleportPlayerTo(get_local(v, args, 0, 0), 40, 195, get_local(v, args, 0, 2)) end
end

local function fn_0x0a89(...)
    local args = {...}
    local v = {}
    if math.random(0, 10) < 5 then goto lbl_0x0adf end
    set_local(v, args, 1, 0, get_local(v, args, 1, 3))
    if fn_0x0a44(get_local(v, args, 1, 1), get_local(v, args, 1, 3), 0) == 1 then goto lbl_0x0b1f end
    set_local(v, args, 1, 0, get_local(v, args, 1, 2))
    if fn_0x0a44(get_local(v, args, 1, 1), get_local(v, args, 1, 2), 0) == 1 then goto lbl_0x0b1f end
    do return 0 end
    ::lbl_0x0adf::
    set_local(v, args, 1, 0, get_local(v, args, 1, 2))
    if fn_0x0a44(get_local(v, args, 1, 1), get_local(v, args, 1, 2), 0) == 1 then goto lbl_0x0b1f end
    set_local(v, args, 1, 0, get_local(v, args, 1, 3))
    if fn_0x0a44(get_local(v, args, 1, 1), get_local(v, args, 1, 3), 0) == 1 then goto lbl_0x0b1f end
    do return 0 end
    ::lbl_0x0b1f::
    if engine.isNet() ~= 1 then goto lbl_0x0b4f end
    engine.strmSetSize(engine.get_global(11), 0)
    engine.strmWrite(engine.get_global(11), (3 + get_local(v, args, 1, 1)), 1)
    engine.strmWrite(engine.get_global(11), get_local(v, args, 1, 0), 1)
    engine.strmSend(engine.get_global(11))
    ::lbl_0x0b4f::
    do return 1 end
end

local function fn_0x0b55(...)
    local args = {...}
    local v = {}
    engine.set_global(10, get_local(v, args, 0, 0))
    engine.setAnim(engine.getSlot(9), get_local(v, args, 0, 0), 1)
    engine.playAnim(engine.getSlot(9), 0)
    engine.hideView(engine.getSlot(18))
    if engine.isNet() ~= 1 then goto lbl_0x0ba8 end
    engine.strmSetSize(engine.get_global(11), 0)
    engine.strmWrite(engine.get_global(11), 1, 1)
    engine.strmWrite(engine.get_global(11), get_local(v, args, 0, 0), 1)
    engine.strmSend(engine.get_global(11))
    ::lbl_0x0ba8::
    do return 0 end
end

local function fn_0x0bae(...)
    local args = {...}
    local v = {}
    if engine.get_global(14) > 0 then goto lbl_0x0bca end
    if (engine.get_global(8) * engine.get_global(9)) == 0 then goto lbl_0x0bd0 end
    ::lbl_0x0bca::
    do return 0 end
    ::lbl_0x0bd0::
    do return 1 end
end

local function fn_0x0bd6(...)
    local args = {...}
    local v = {}
    if math.random(0, 1) ~= 0 then goto lbl_0x0c59 end
    if engine.get_global(10) == 10 then goto lbl_0x0c59 end
    if engine.isViewKind(engine.getSlot(9)) == 1 then goto lbl_0x0c59 end
    if engine.get_global(7) ~= 0 then goto lbl_0x0c59 end
    if engine.get_global(6) < 2 then goto lbl_0x0c3d end
    if get_local(v, args, 0, 1) == 0 then goto lbl_0x0c3d end
    if fn_0x0bae() == 1 then goto lbl_0x0c4b end
    ::lbl_0x0c3d::
    fn_0x0b55(get_local(v, args, 0, 0))
    do return 0 end
    ::lbl_0x0c4b::
    fn_0x0b55((4 + get_local(v, args, 0, 0)))
    ::lbl_0x0c59::
    do return 0 end
end

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.set_global(1, 5)
    engine.set_global(2, "C7B4C92B-5196-4b24-A090-0BF090A95DB3")
    if get_local(v, args, 0, 0) == 1 then goto lbl_0x0076 end
    engine.set_global(0, "8-bit")
    do return 0 end
    ::lbl_0x0076::
    engine.set_global(0, "V zajetí 8-bitu")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(3, 0)
    engine.set_global(4, 0)
    engine.set_global(5, 0)
    engine.set_global(6, 0)
    engine.set_global(7, 0)
    engine.set_global(8, 0)
    engine.set_global(9, 0)
    engine.set_global(14, 0)
    engine.set_global(13, 0)
    engine.set_global(10, 0)
    engine.set_global(11, 0)
    engine.set_global(12, engine.newCollection())
    engine.collInsert(engine.get_global(12), -1, engine.getImage(65633))
    engine.collInsert(engine.get_global(12), -1, engine.getImage(65634))
    engine.collInsert(engine.get_global(12), -1, engine.getImage(65635))
    engine.collInsert(engine.get_global(12), -1, engine.getImage(65632))
    if engine.isNet() == 0 then goto lbl_0x013f end
    engine.set_global(11, engine.strmCreateMem(4096, 4096))
    engine.setCommStrm(engine.get_global(11))
    ::lbl_0x013f::
    engine.loadPreface(65641)
    engine.setMusic(65864, 65536)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 65636))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(58, 250, 65631), -25))
    engine.insertView(engine.setOrderAxis(engine.createImage(576, 80, 65637), -25))
    engine.insertView(engine.setOrderAxis(engine.createImage(300, 106, 65629), -67))
    engine.insertView(engine.setOrderAxis(engine.createImage(133, 55, 65630), -30))
    engine.insertView(engine.setOrderAxis(engine.bindToSlot(engine.create_anim(199, 77, 0, {65795, 65798, 65796, 65799, 65794, 65797}), 9), -17))
    engine.insertView(engine.showView(engine.setOrderAxis(engine.bindToSlot(engine.create_anim(175, 22, 0, {65800}), 20), -17)))
    engine.insertView(engine.setOrderAxis(engine.bindToSlot(engine.create_anim(54, 49, 0, {65790}), 10), -35))
    engine.insertView(engine.setOrderAxis(engine.bindToSlot(engine.create_anim(506, 256, 0, {65792}), 12), -17))
    engine.insertView(engine.setOrderAxis(engine.bindToSlot(engine.create_anim(507, 376, 0, {65793}), 14), -17))
    engine.insertView(engine.setOrderAxis(engine.bindToSlot(engine.create_anim(175, 55, 0, {65791}), 16), -25))
    engine.setInsertMode(2)
    engine.insertView(engine.createObstacle(95, 320, 245, 402))
    engine.insertView(engine.createObstacle(577, 109, 702, 166))
    engine.insertView(engine.createObstacle(303, 150, 422, 257))
    engine.insertView(engine.createObstacle(126, 73, 174, 123))
    engine.insertView(engine.bindToSlot(engine.createObstacle(58, 73, 110, 123), 11))
    engine.insertView(engine.bindToSlot(engine.createObstacle(522, 262, 598, 305), 13))
    engine.insertView(engine.bindToSlot(engine.createObstacle(533, 390, 609, 429), 15))
    engine.insertView(engine.bindToSlot(engine.createObstacle(191, 73, 251, 123), 17))
    engine.insertView(engine.showView(engine.bindToSlot(engine.createObstacle(191, 73, 251, 123), 18)))
    engine.insertView(engine.showView(engine.bindToSlot(engine.createObstacle(191, 73, 251, 123), 19)))
    if engine.isServer() == 0 then goto lbl_0x0476 end
    engine.defineTraceArea(0, 30, 185, 105, 243, 6)
    engine.defineTraceArea(1, 442, 67, 515, 131, 6)
    engine.defineTraceArea(2, 694, 377, 770, 440, 6)
    engine.defineTraceArea(3, 191, 73, 251, 123, 10)
    engine.defineTraceArea(4, 200, 128, 243, 322, 8)
    engine.defineTraceArea(5, 257, 80, 785, 116, 8)
    engine.defineTraceArea(6, 200, 9, 243, 50, 8)
    ::lbl_0x0476::
    engine.insertBulanci()
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    engine.set_global(12, engine.freeObject(engine.get_global(12)))
    if engine.get_global(11) == 0 then goto lbl_0x0494 end
    engine.set_global(11, engine.strmDestroy(engine.get_global(11)))
    ::lbl_0x0494::
    do return 0 end
end

function OnBitmapEvt(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 10 then goto lbl_0x04f7 end
if _sw == 12 then goto lbl_0x0559 end
if _sw == 14 then goto lbl_0x058a end
if _sw == 16 then goto lbl_0x0528 end
if _sw == 9 then goto lbl_0x05bb end
if _sw == 20 then goto lbl_0x061c end
    do return 0 end
    ::lbl_0x04f7::
    engine.animResume(engine.getSlot(10))
    if engine.get_global(3) ~= 9 then goto lbl_0x06bc end
    engine.removeView(engine.getSlot(11), 1)
    engine.setOrderAxis(engine.getSlot(10), -120)
    do return 0 end
    ::lbl_0x0528::
    engine.animResume(engine.getSlot(16))
    if engine.get_global(6) ~= 2 then goto lbl_0x06bc end
    engine.removeView(engine.getSlot(17), 1)
    engine.setOrderAxis(engine.getSlot(16), -120)
    do return 0 end
    ::lbl_0x0559::
    engine.animResume(engine.getSlot(12))
    if engine.get_global(4) ~= 4 then goto lbl_0x06bc end
    engine.removeView(engine.getSlot(13), 1)
    engine.setOrderAxis(engine.getSlot(12), -120)
    do return 0 end
    ::lbl_0x058a::
    engine.animResume(engine.getSlot(14))
    if engine.get_global(5) ~= 4 then goto lbl_0x06bc end
    engine.removeView(engine.getSlot(15), 1)
    engine.setOrderAxis(engine.getSlot(14), -120)
    do return 0 end
    ::lbl_0x05bb::
    if engine.get_global(10) < 2 then goto lbl_0x05e7 end
    if engine.get_global(10) > 3 then goto lbl_0x0635 end
    local _sw = get_local(v, args, 0, 1)
if _sw == 0 then goto lbl_0x05f4 end
if _sw == 1 then goto lbl_0x0608 end
    ::lbl_0x05e7::
    engine.showView(engine.getSlot(18))
    do return 0 end
    ::lbl_0x05f4::
    engine.hideView(engine.getSlot(19))
    engine.showView(engine.getSlot(18))
    do return 0 end
    ::lbl_0x0608::
    engine.showView(engine.getSlot(19))
    engine.hideView(engine.getSlot(18))
    do return 0 end
    ::lbl_0x061c::
    engine.showView(engine.getSlot(18))
    engine.setOrderAxis(engine.getSlot(20), -120)
    do return 0 end
    ::lbl_0x0635::
    local _sw = get_local(v, args, 0, 1)
if _sw == 0 then goto lbl_0x05f4 end
if _sw == 1 then goto lbl_0x0656 end
if _sw == 2 then goto lbl_0x0608 end
    engine.goto(1511)
    ::lbl_0x0656::
    if engine.get_global(10) == 4 then goto lbl_0x068f end
    engine.spawnOpponentEx(221, 136, 3, 9, 2, engine.get_global(12))
    engine.spawnAtView(0, 9)
    do return 0 end
    ::lbl_0x068f::
    engine.spawnOpponentEx(260, 96, 1, 9, 2, engine.get_global(12))
    engine.spawnAtView(0, 9)
    do return 0 end
    ::lbl_0x06bc::
    do return 0 end
end

function OnSlotPlaced(...)
    local args = {...}
    local v = {}
    if engine.isServer() == 0 then goto lbl_0x0928 end
    fn_0x06e8(get_local(v, args, 0, 0))
    ::lbl_0x0928::
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
    if math.random(0, 2) ~= 0 then goto lbl_0x0a3e end
    if engine.get_global(10) == 10 then goto lbl_0x0a3e end
    if engine.get_global(6) < 2 then goto lbl_0x0a3e end
    if engine.isViewKind(engine.getSlot(9)) == 1 then goto lbl_0x0a3e end
    if engine.get_global(7) ~= 0 then goto lbl_0x0a3e end
    if fn_0x0bae() == 0 then goto lbl_0x0a2d end
    if engine.get_global(9) == 0 then goto lbl_0x09d3 end
    if math.random(0, 3) ~= 0 then goto lbl_0x09c2 end
    fn_0x0b55(4)
    do return 0 end
    ::lbl_0x09c2::
    fn_0x0b55(0)
    do return 0 end
    ::lbl_0x09d3::
    if engine.get_global(8) == 0 then goto lbl_0x0a16 end
    if math.random(0, 3) ~= 0 then goto lbl_0x0a05 end
    fn_0x0b55(5)
    do return 0 end
    ::lbl_0x0a05::
    fn_0x0b55(1)
    do return 0 end
    ::lbl_0x0a16::
    fn_0x0b55(math.random(2, 3))
    do return 0 end
    ::lbl_0x0a2d::
    fn_0x0b55(math.random(0, 1))
    ::lbl_0x0a3e::
    do return 0 end
end

function OnEnter(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x0cd2 end
if _sw == 1 then goto lbl_0x0cba end
if _sw == 2 then goto lbl_0x0ca2 end
if _sw == 3 then goto lbl_0x0cea end
if _sw == 4 then goto lbl_0x0cfa end
if _sw == 5 then goto lbl_0x0d1a end
if _sw == 6 then goto lbl_0x0d3a end
    do return 0 end
    ::lbl_0x0ca2::
    fn_0x0a89(get_local(v, args, 0, 1), 0, 1)
    do return 0 end
    ::lbl_0x0cba::
    fn_0x0a89(get_local(v, args, 0, 1), 0, 2)
    do return 0 end
    ::lbl_0x0cd2::
    fn_0x0a89(get_local(v, args, 0, 1), 1, 2)
    do return 0 end
    ::lbl_0x0cea::
    engine.set_global(7, (engine.get_global(7) + 1))
    do return 0 end
    ::lbl_0x0cfa::
    engine.set_global(8, (engine.get_global(8) + 1))
    fn_0x0bd6(1, 1)
    do return 0 end
    ::lbl_0x0d1a::
    engine.set_global(9, (engine.get_global(9) + 1))
    fn_0x0bd6(0, 1)
    do return 0 end
    ::lbl_0x0d3a::
    engine.set_global(14, (engine.get_global(14) + 1))
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 3 then goto lbl_0x0d75 end
if _sw == 4 then goto lbl_0x0d85 end
if _sw == 5 then goto lbl_0x0d95 end
if _sw == 6 then goto lbl_0x0da5 end
    do return 0 end
    ::lbl_0x0d75::
    engine.set_global(7, (engine.get_global(7) - 1))
    do return 0 end
    ::lbl_0x0d85::
    engine.set_global(8, (engine.get_global(8) - 1))
    do return 0 end
    ::lbl_0x0d95::
    engine.set_global(9, (engine.get_global(9) - 1))
    do return 0 end
    ::lbl_0x0da5::
    engine.set_global(14, (engine.get_global(14) - 1))
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    local _sw = engine.strmRead(get_local(v, args, 0, 0), 1)
if _sw == 1 then goto lbl_0x0e6e end
if _sw == 2 then goto lbl_0x0e82 end
if _sw == 3 then goto lbl_0x0df6 end
if _sw == 4 then goto lbl_0x0e14 end
if _sw == 5 then goto lbl_0x0e32 end
if _sw == 6 then goto lbl_0x0e50 end
    do return 0 end
    ::lbl_0x0df6::
    fn_0x0a44(0, engine.strmRead(get_local(v, args, 0, 0), 1), 1)
    do return 0 end
    ::lbl_0x0e14::
    fn_0x0a44(1, engine.strmRead(get_local(v, args, 0, 0), 1), 1)
    do return 0 end
    ::lbl_0x0e32::
    fn_0x0a44(2, engine.strmRead(get_local(v, args, 0, 0), 1), 1)
    do return 0 end
    ::lbl_0x0e50::
    fn_0x0a44(3, engine.strmRead(get_local(v, args, 0, 0), 1), 1)
    do return 0 end
    ::lbl_0x0e6e::
    fn_0x0b55(engine.strmRead(get_local(v, args, 0, 0), 1))
    do return 0 end
    ::lbl_0x0e82::
    fn_0x06e8(engine.strmRead(get_local(v, args, 0, 0), 1))
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    if engine.isServer() == 0 then goto lbl_0x04b6 end
    engine.registerTimer(0, 3000, 6)
    ::lbl_0x04b6::
    do return 0 end
end
