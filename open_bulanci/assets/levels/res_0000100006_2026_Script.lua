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
    engine.set_global(1, 1000)
    engine.set_global(2, "6A2B7D86-A2AF-4471-8BF6-F68DF5B2624E")
    engine.set_global(0, "Pewnost")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createObstacle(13, 488, 50, 514))
    engine.insertView(engine.createObstacle(681, 478, 717, 498))
    engine.insertView(engine.createObstacle(685, 350, 800, 359))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(137, 209, 178, 255), 1))
    engine.insertView(engine.createObstacle(133, 202, 143, 234))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(258, 58, 279, 80), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(725, 0, 748, 35), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(743, 85, 768, 110), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(738, 59, 762, 89), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(735, 37, 757, 60), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(731, 0, 761, 41), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(763, 129, 798, 168), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(751, 0, 800, 141), 1))
    engine.insertView(engine.createObstacle(659, 234, 670, 263))
    engine.insertView(engine.createObstacle(658, 214, 668, 242))
    engine.insertView(engine.createObstacle(656, 183, 666, 214))
    engine.insertView(engine.createObstacle(654, 151, 666, 180))
    engine.insertView(engine.createObstacle(636, 261, 669, 271))
    engine.insertView(engine.createObstacle(605, 270, 639, 278))
    engine.insertView(engine.createObstacle(579, 277, 609, 285))
    engine.insertView(engine.createObstacle(551, 284, 580, 294))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(501, 231, 560, 241), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(386, 228, 498, 243), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(304, 214, 393, 245), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(289, 191, 314, 228), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(277, 157, 319, 198), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(247, 160, 275, 198), 1))
    engine.insertView(engine.createObstacle(268, 148, 513, 158))
    engine.insertView(engine.createObstacle(357, 147, 661, 157))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(308, 156, 660, 235), 1))
    engine.insertView(engine.createObstacle(246, 159, 277, 173))
    engine.insertView(engine.createObstacle(666, 372, 677, 385))
    engine.insertView(engine.createObstacle(650, 361, 688, 372))
    engine.insertView(engine.createObstacle(555, 381, 587, 387))
    engine.insertView(engine.createObstacle(586, 373, 621, 380))
    engine.insertView(engine.createObstacle(614, 366, 660, 372))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(193, 44, 244, 87), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(3, 78, 140, 119), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(0, 54, 191, 110), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(0, 0, 278, 80), 1))
    engine.insertView(engine.createObstacle(281, 0, 294, 11))
    engine.insertView(engine.createObstacle(275, 13, 290, 31))
    engine.insertView(engine.createObstacle(273, 31, 287, 45))
    engine.insertView(engine.createObstacle(268, 46, 282, 71))
    engine.insertView(engine.createObstacle(250, 65, 276, 78))
    engine.insertView(engine.createObstacle(218, 76, 244, 89))
    engine.insertView(engine.createObstacle(195, 86, 219, 97))
    engine.insertView(engine.createObstacle(167, 98, 194, 108))
    engine.insertView(engine.createObstacle(142, 108, 168, 117))
    engine.insertView(engine.createObstacle(49, 119, 83, 131))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(775, 287, 800, 306), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(770, 262, 800, 290), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(766, 248, 793, 266), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(781, 278, 800, 348), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(765, 197, 797, 256), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(763, 170, 800, 222), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(674, 359, 800, 384), 1))
    engine.insertView(engine.createObstacle(142, 285, 152, 308))
    engine.insertView(engine.createObstacle(138, 259, 151, 287))
    engine.insertView(engine.createObstacle(135, 224, 145, 257))
    engine.insertView(engine.createObstacle(136, 197, 182, 207))
    engine.insertView(engine.createObstacle(160, 188, 194, 198))
    engine.insertView(engine.createObstacle(190, 179, 216, 189))
    engine.insertView(engine.createObstacle(215, 169, 275, 180))
    engine.insertView(engine.createObstacle(17, 292, 36, 308))
    engine.insertView(engine.createObstacle(12, 246, 36, 259))
    engine.insertView(engine.createObstacle(7, 277, 38, 292))
    engine.insertView(engine.createObstacle(16, 308, 42, 324))
    engine.insertView(engine.createObstacle(0, 180, 31, 337))
    engine.insertView(engine.createObstacle(0, 148, 22, 222))
    engine.insertView(engine.createObstacle(23, 129, 47, 153))
    engine.insertView(engine.createObstacle(26, 139, 43, 168))
    engine.insertView(engine.createObstacle(31, 128, 51, 142))
    engine.insertView(engine.createObstacle(11, 185, 33, 206))
    engine.insertView(engine.createObstacle(13, 173, 35, 186))
    engine.insertView(engine.createObstacle(12, 145, 38, 180))
    engine.insertView(engine.createObstacle(0, 119, 37, 166))
    engine.insertView(engine.createObstacle(87, 118, 139, 128))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(175, 192, 314, 234), 1))
    engine.setInsertMode(2)
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 337, 100005), 0))
    engine.insertView(engine.setOrderAxis(engine.createImage(629, 321, 100004), 0))
    engine.setInsertMode(0)
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
