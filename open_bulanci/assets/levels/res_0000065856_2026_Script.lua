-- Transpiled Bulanci Level Script — "Na dobrou noc" / "Bedtime story" (res 65856, classId 2026)
-- Level-specific helper labels come from ghidra_analysis/asset_catalog/registry.json:
--   level_bedtime_butterfly_fly_* (decor), level_bedtime_bunny_clip_* (on log platform),
--   level_bedtime_mouse_* (gun-mouse with danger zones), level_bedtime_bird_nest_*,
--   level_bedtime_bush_* (decor), level_bedtime_mimic_* (bedtime-mimic event).
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

-- place_butterfly_decor(x, y): BUTTERFLY decor (level_bedtime_butterfly_fly_*).
-- 1/8 chance of being absent (Rand 0..7). Inserts butterfly trio anim 65764..65766 on slot 1
-- (32f/31f/32f flying loops), 2 images (65643 body at +8/+141, 65642 base at order axis -39),
-- 1 obstacle at (27,97)-(175,133), obstacle bounds (27,180)-(180,216).
local function fn_0x01e7(...)
    local args = {...}
    local v = {}
    engine.setInsertMode(0)
    engine.insertView(engine.createImage((get_local(v, args, 1, 1) + 8), (get_local(v, args, 1, 2) + 141), 65643))
    engine.setInsertMode(1)
    set_local(v, args, 1, 0, engine.setOrderAxis(engine.createImage(get_local(v, args, 1, 1), get_local(v, args, 1, 2), 65642), -39))
    engine.insertView(engine.setObstacleBounds(get_local(v, args, 1, 0), 27, 180, 180, 216))
    engine.setInsertMode(2)
    engine.insertView(engine.translateTo(engine.createObstacle(27, 97, 175, 133), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.insertView(engine.setAnimFrame(engine.bindToSlot(engine.create_anim((get_local(v, args, 1, 1) + 50), (get_local(v, args, 1, 2) + 50), 1, {65764, 65765, 65766}), 8), 1))
    do return 0 end
end

-- place_bunny_on_log(x, y): BROWN BUNNY on log platform (level_bedtime_bunny_clip_*).
-- 1/10 chance of being absent (Rand 0..9). Inserts 6 obstacles (the log's decorative shape),
-- 1 image 65609 (the LOG), 5-frame bunny anim 65759..65763 on slot 6.
-- Bunny clips: A=8f idle, B=5f static, C=20f idle, D=25f decapitation/death, E=20f idle.
local function fn_0x0286(...)
    local args = {...}
    local v = {}
    if math.random(0, 9) == 0 then goto lbl_0x038e end
    engine.setInsertMode(2)
    engine.insertView(engine.bindToSlot(engine.translateTo(engine.createObstacle(79, 50, 112, 80), get_local(v, args, 1, 1), get_local(v, args, 1, 2)), 7))
    engine.insertView(engine.translateTo(engine.createObstacle(2, 58, 23, 108), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.insertView(engine.translateTo(engine.createObstacle(23, 44, 42, 108), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.insertView(engine.translateTo(engine.createObstacle(42, 28, 66, 91), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.insertView(engine.translateTo(engine.createObstacle(66, 14, 87, 69), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.insertView(engine.translateTo(engine.createObstacle(87, 0, 134, 42), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(get_local(v, args, 1, 1), get_local(v, args, 1, 2), 65609))  -- the LOG
    set_local(v, args, 1, 0, engine.create_anim((get_local(v, args, 1, 1) + 49), (get_local(v, args, 1, 2) + -15), 1, {65759, 65760, 65761, 65763, 65762}))  -- the BUNNY
    engine.bindToSlot(get_local(v, args, 1, 0), 6)
    engine.insertView(get_local(v, args, 1, 0))
    ::lbl_0x038e::
    do return 0 end
end

-- place_gun_mouse(x, y, danger_w): GREY MOUSE WITH RIFLE (level_bedtime_mouse_*).
-- 1/8 chance of being absent (Rand 0..7). Sets g_pos5/6 (mouse position), g_anim_7/9/10/11=0.
-- If server: defines 2 danger zones (left and right of the bush, 30px tall, danger_w apart),
-- registers timer 0. Inserts 3 obstacles (the bush), image 65606 (the cover, order axis -25),
-- 8-frame mouse anim 65767..65774 (clips pop/hide/shoot/death for left+right) on slot 9.
local function fn_0x0394(...)
    local args = {...}
    local v = {}
    if math.random(0, 7) == 0 then goto lbl_0x04db end
    engine.setInsertMode(2)
    engine.insertView(engine.set_global(8, engine.translateTo(engine.createObstacle(36, 0, 45, 14), get_local(v, args, 1, 1), get_local(v, args, 1, 2))))
    engine.insertView(engine.translateTo(engine.createObstacle(5, 0, 36, 58), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.insertView(engine.translateTo(engine.createObstacle(36, 14, 59, 58), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(get_local(v, args, 1, 1), get_local(v, args, 1, 2), 65606), -25))  -- bush cover
    set_local(v, args, 1, 0, engine.create_anim((get_local(v, args, 1, 1) + 17), (get_local(v, args, 1, 2) + -29), 1, {65767, 65768, 65769, 65770, 65771, 65772, 65773, 65774}))  -- MOUSE anim
    engine.bindToSlot(engine.setOrderAxis(get_local(v, args, 1, 0), 35), 9)
    engine.insertView(get_local(v, args, 1, 0))
    ::lbl_0x04db::
    do return 0 end
end

-- place_bird_in_nest(x, y): BIRD IN A NEST (level_bedtime_bird_nest_*).
-- 1/7 chance of being absent (Rand 0..6). Inserts 4 obstacles (the nest), 1 image 65607 (the nest, order axis -25),
-- 5-frame bird anim 65775..65779 on slot 4.
-- Bird clips: A=first idle, B=2nd (9f 66x53), C=3rd, D=12f idle, E=10f death/gore.
local function fn_0x04e1(...)
    local args = {...}
    local v = {}
    if math.random(0, 6) == 0 then goto lbl_0x05c3 end
    engine.setInsertMode(2)
    engine.insertView(engine.bindToSlot(engine.translateTo(engine.createObstacle(106, 20, 122, 36), get_local(v, args, 1, 1), get_local(v, args, 1, 2)), 5))
    engine.insertView(engine.translateTo(engine.createObstacle(7, 35, 63, 83), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.insertView(engine.translateTo(engine.createObstacle(63, 55, 134, 98), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.insertView(engine.translateTo(engine.createObstacle(49, 20, 93, 35), get_local(v, args, 1, 1), get_local(v, args, 1, 2)))
    engine.setInsertMode(1)
    set_local(v, args, 1, 0, engine.setOrderAxis(engine.createImage(get_local(v, args, 1, 1), get_local(v, args, 1, 2), 65607), -25))  -- the NEST
    engine.insertView(get_local(v, args, 1, 0))
    set_local(v, args, 1, 0, engine.create_anim((get_local(v, args, 1, 1) + 82), (get_local(v, args, 1, 2) + 1), 1, {65775, 65776, 65777, 65779, 65778}))  -- the BIRD
    engine.bindToSlot(engine.setOrderAxis(get_local(v, args, 1, 0), 50), 4)
    engine.insertView(get_local(v, args, 1, 0))
    ::lbl_0x05c3::
    do return 0 end
end

-- place_bush_decor(x, y): BERRY BUSH decor (level_bedtime_bush_*).
-- 1/6 chance of being absent (Rand 0..6). Inserts 1 image 65608 with order axis -40,
-- obstacle bounds (9,23)-(90,60). No anim, no danger zone, no timer. Pure decor.
local function fn_0x05c9(...)
    local args = {...}
    local v = {}
    if math.random(0, 6) == 0 then goto lbl_0x060e end
    engine.setInsertMode(1)
    set_local(v, args, 1, 0, engine.setOrderAxis(engine.createImage(get_local(v, args, 1, 1), get_local(v, args, 1, 2), 65608), -40))
    engine.insertView(engine.setObstacleBounds(get_local(v, args, 1, 0), 9, 23, 90, 60))
    ::lbl_0x060e::
    do return 0 end
end

local function fn_0x07b5(...)
    local args = {...}
    local v = {}
    fn_0x08ab(get_local(v, args, 0, 0))
    engine.set_global(7, ((get_local(v, args, 0, 0) * 4) + 2))
    engine.setAnim(engine.getSlot(9), engine.get_global(7), 1)
    if get_local(v, args, 0, 0) == 1 then goto lbl_0x080d end
    engine.spawnEnemyAt((engine.get_global(5) + 65), (engine.get_global(6) + 10), 1, 9, 1)
    engine.goto(2093)
    ::lbl_0x080d::
    engine.spawnEnemyAt((engine.get_global(5) + -10), (engine.get_global(6) + 10), 0, 9, 1)
    ::lbl_0x082d::
    engine.spawnAtView(4, 9)
    if engine.isServer() == 0 then goto lbl_0x0858 end
    engine.bindToSlot(engine.get_global(8), 10)
    fn_0x0978(1, get_local(v, args, 0, 0))
    ::lbl_0x0858::
    do return 0 end
end

local function fn_0x085e(...)
    local args = {...}
    local v = {}
    fn_0x08ab(get_local(v, args, 0, 0))
    engine.set_global(7, ((get_local(v, args, 0, 0) * 4) + 1))
    engine.setAnim(engine.getSlot(9), engine.get_global(7), 1)
    if engine.isServer() == 0 then goto lbl_0x08a5 end
    engine.bindToSlot(engine.get_global(8), 255)
    fn_0x0978(2, get_local(v, args, 0, 0))
    ::lbl_0x08a5::
    do return 0 end
end

local function fn_0x08ab(...)
    local args = {...}
    local v = {}
    if get_local(v, args, 0, 0) == 0 then goto lbl_0x08d5 end
    engine.translateTo(engine.getSlot(9), (engine.get_global(5) + 13), (engine.get_global(6) + -30))
    do return 0 end
    ::lbl_0x08d5::
    engine.translateTo(engine.getSlot(9), (engine.get_global(5) + 13), (engine.get_global(6) + -25))
    do return 0 end
end

local function fn_0x08f2(...)
    local args = {...}
    local v = {}
    if engine.isServer() == 0 then goto lbl_0x090a end
    if engine.get_global(11) == 0 then goto lbl_0x093e end
    ::lbl_0x090a::
    fn_0x08ab(get_local(v, args, 0, 0))
    engine.set_global(7, (get_local(v, args, 0, 0) * 4))
    engine.setAnim(engine.getSlot(9), engine.get_global(7), 1)
    engine.set_global(11, 0)
    fn_0x0978(0, get_local(v, args, 0, 0))
    ::lbl_0x093e::
    do return 0 end
end

local function fn_0x0944(...)
    local args = {...}
    local v = {}
    if engine.isNet() ~= 1 then goto lbl_0x0972 end
    engine.strmSetSize(engine.get_global(12), 0)
    engine.strmWrite(engine.get_global(12), 0, 1)
    engine.strmWrite(engine.get_global(12), get_local(v, args, 0, 0), 1)
    engine.strmSend(engine.get_global(12))
    ::lbl_0x0972::
    do return 0 end
end

local function fn_0x0978(...)
    local args = {...}
    local v = {}
    if engine.isNet() ~= 1 then goto lbl_0x09a9 end
    engine.strmSetSize(engine.get_global(12), 0)
    engine.strmWrite(engine.get_global(12), (get_local(v, args, 0, 0) + 1), 1)
    engine.strmWrite(engine.get_global(12), get_local(v, args, 0, 1), 1)
    engine.strmSend(engine.get_global(12))
    ::lbl_0x09a9::
    do return 0 end
end

local function fn_0x09af(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 5 then goto lbl_0x09d2 end
if _sw == 7 then goto lbl_0x0a15 end
if _sw == 10 then goto lbl_0x0a58 end
    do return 0 end
    ::lbl_0x09d2::
    engine.set_global(4, bit32.bor(engine.get_global(4), 1))
    engine.bindToSlot(engine.getSlot(5), 255)
    engine.setAnim(engine.getSlot(4), 4, 1)
    engine.spawnAtView(2, 4)
    fn_0x0944(5)
    do return 0 end
    ::lbl_0x0a15::
    engine.set_global(4, bit32.bor(engine.get_global(4), 2))
    engine.bindToSlot(engine.getSlot(7), 255)
    engine.setAnim(engine.getSlot(6), 4, 1)
    engine.spawnAtView(1, 6)
    fn_0x0944(7)
    do return 0 end
    ::lbl_0x0a58::
    engine.set_global(4, bit32.bor(engine.get_global(4), 4))
    engine.bindToSlot(engine.get_global(8), 255)
    engine.set_global(7, (engine.get_global(7) + 1))
    engine.setAnim(engine.getSlot(9), engine.get_global(7), 1)
    engine.spawnAtView(3, 9)
    fn_0x0944(10)
    do return 0 end
end

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.set_global(1, 1)
    engine.set_global(2, "0DDB62C1-2E2E-430c-A914-73AD2669796A")
    if get_local(v, args, 0, 0) == 1 then goto lbl_0x0086 end
    engine.set_global(0, "Bedtime story")
    do return 0 end
    ::lbl_0x0086::
    engine.set_global(0, "Na dobrou noc")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.set_global(12, 0)
    if engine.isNet() == 0 then goto lbl_0x00ce end
    engine.set_global(12, engine.strmCreateMem(1024, 1024))
    engine.setCommStrm(engine.get_global(12))
    ::lbl_0x00ce::
    engine.set_global(4, 0)
    engine.loadPreface(65848)
    engine.setMusic(65868, 65537)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 65619))  -- background night scene
    -- g_variant_3 = math.random(0,1): 0=Variant A, 1=Variant B (whole-entity layout)
    if engine.set_global(3, math.random(0, 1)) == 1 then goto lbl_0x016c end
    -- Variant A (right-heavy):
    fn_0x0286(568, 360)  -- place_bunny_on_log  (right side)
    fn_0x0394(227, 103, 800)  -- place_gun_mouse (right bush, wide danger 800px)
    fn_0x01e7(417, 113)  -- place_butterfly_decor (mid)
    fn_0x04e1(154, 306)  -- place_bird_in_nest  (left)
    fn_0x05c9(61, 127)   -- place_bush_decor   (far left)
    engine.insertBulanci()
    do return 0 end
    ::lbl_0x016c::
    -- Variant B (left-heavy):
    fn_0x0286(195, 364)  -- place_bunny_on_log  (left side)
    fn_0x0394(124, 283, 580)  -- place_gun_mouse (left bush, narrow danger 580px)
    fn_0x01e7(309, 50)   -- place_butterfly_decor (mid)
    fn_0x04e1(576, 245)  -- place_bird_in_nest  (right)
    fn_0x05c9(67, 85)    -- place_bush_decor   (far left)
    engine.insertBulanci()
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(12) == 0 then goto lbl_0x01da end
    engine.set_global(12, engine.strmDestroy(engine.get_global(12)))
    ::lbl_0x01da::
    do return 0 end
end

function OnBitmapEvt(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 1, 1)
if _sw == 4 then goto lbl_0x063f end
if _sw == 6 then goto lbl_0x0689 end
if _sw == 8 then goto lbl_0x06b8 end
if _sw == 9 then goto lbl_0x06d5 end
    ::lbl_0x0639::
    do return 0 end
    ::lbl_0x063f::
    if bit32.band(engine.get_global(4), 1) == 1 then goto lbl_0x0639 end
    set_local(v, args, 1, 0, math.random(0, 3))
    if get_local(v, args, 1, 0) ~= 1 then goto lbl_0x0675 end
    engine.spawnAtView(0, 4)
    ::lbl_0x0675::
    engine.setAnim(engine.getSlot(4), get_local(v, args, 1, 0), 1)
    do return 0 end
    ::lbl_0x0689::
    if bit32.band(engine.get_global(4), 2) == 2 then goto lbl_0x0639 end
    engine.setAnim(engine.getSlot(6), math.random(0, 3), 1)
    do return 0 end
    ::lbl_0x06b8::
    engine.setAnim(engine.getSlot(8), math.random(0, 2), 1)
    do return 0 end
    ::lbl_0x06d5::
    if bit32.band(engine.get_global(4), 4) == 4 then goto lbl_0x0639 end
    set_local(v, args, 1, 0, 0)
    if engine.get_global(7) < 4 then goto lbl_0x0701 end
    set_local(v, args, 1, 0, 1)
    ::lbl_0x0701::
    local _sw = engine.get_global(7)
if _sw == 1 then goto lbl_0x0767 end
if _sw == 2 then goto lbl_0x079c end
if _sw == 5 then goto lbl_0x0767 end
if _sw == 6 then goto lbl_0x079c end
    if engine.isServer() == 0 then goto lbl_0x0639 end
    if get_local(v, args, 1, 0) == 0 then goto lbl_0x074d end
    if engine.get_global(9) < 1 then goto lbl_0x07a7 end
    engine.goto(1881)
    ::lbl_0x074d::
    if engine.get_global(10) < 1 then goto lbl_0x07a7 end
    ::lbl_0x0759::
    fn_0x07b5(get_local(v, args, 1, 0))
    do return 0 end
    ::lbl_0x0767::
    engine.set_global(11, 1)
    if engine.isServer() == 0 then goto lbl_0x0639 end
    engine.timerSetData(0, (math.random(3, 15) * 1000))
    engine.timerStop(0)
    do return 0 end
    ::lbl_0x079c::
    if engine.isServer() == 0 then goto lbl_0x0639 end
    ::lbl_0x07a7::
    fn_0x085e(get_local(v, args, 1, 0))
    do return 0 end
end

function OnSlotPlaced(...)
    local args = {...}
    local v = {}
    if engine.isServer() == 0 then goto lbl_0x0ab2 end
    fn_0x09af(get_local(v, args, 0, 0))
    ::lbl_0x0ab2::
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
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x0ad2 end
    do return 0 end
    ::lbl_0x0ad2::
    engine.timerStart(0)
    fn_0x08f2(math.random(0, 1))
    do return 0 end
end

function OnEnter(...)
    local args = {...}
    local v = {}
    if bit32.band(engine.get_global(4), 4) == 4 then goto lbl_0x0b68 end
    if get_local(v, args, 0, 0) == 0 then goto lbl_0x0b3e end
    engine.set_global(10, (engine.get_global(10) + 1))
    if math.random(0, 3) ~= 0 then goto lbl_0x0b68 end
    fn_0x08f2(0)
    do return 0 end
    ::lbl_0x0b3e::
    engine.set_global(9, (engine.get_global(9) + 1))
    if math.random(0, 3) ~= 0 then goto lbl_0x0b68 end
    fn_0x08f2(1)
    ::lbl_0x0b68::
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    if bit32.band(engine.get_global(4), 4) == 4 then goto lbl_0x0ba7 end
    if get_local(v, args, 0, 0) == 0 then goto lbl_0x0b9d end
    engine.set_global(10, (engine.get_global(10) - 1))
    do return 0 end
    ::lbl_0x0b9d::
    engine.set_global(9, (engine.get_global(9) - 1))
    ::lbl_0x0ba7::
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    local _sw = engine.strmRead(get_local(v, args, 0, 0), 1)
if _sw == 0 then goto lbl_0x0bde end
if _sw == 1 then goto lbl_0x0bf2 end
if _sw == 2 then goto lbl_0x0c06 end
if _sw == 3 then goto lbl_0x0c1a end
    do return 0 end
    ::lbl_0x0bde::
    fn_0x09af(engine.strmRead(get_local(v, args, 0, 0), 1))
    do return 0 end
    ::lbl_0x0bf2::
    fn_0x08f2(engine.strmRead(get_local(v, args, 0, 0), 1))
    do return 0 end
    ::lbl_0x0c06::
    fn_0x07b5(engine.strmRead(get_local(v, args, 0, 0), 1))
    do return 0 end
    ::lbl_0x0c1a::
    fn_0x085e(engine.strmRead(get_local(v, args, 0, 0), 1))
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    do return 0 end
end
