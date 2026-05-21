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
    engine.set_global(2, "280A5615-36D3-49B5-B0AE-D7F566F2D00F")
    engine.set_global(0, "svět 3005")
    do return 0 end
end

function OnInit(...)
    local args = {...}
    local v = {}
    engine.loadPreface(100001)
    engine.setMusic(100003, 0)
    engine.setInsertMode(0)
    engine.insertView(engine.createImage(0, 0, 100002))
    engine.insertView(engine.createObstacle(625, 376, 628, 398))
    engine.insertView(engine.createObstacle(613, 380, 623, 410))
    engine.insertView(engine.createObstacle(546, 380, 611, 411))
    engine.insertView(engine.createObstacle(613, 503, 622, 515))
    engine.insertView(engine.createObstacle(535, 398, 613, 514))
    engine.insertView(engine.createObstacle(353, 379, 425, 414))
    engine.insertView(engine.createObstacle(353, 419, 415, 464))
    engine.insertView(engine.createObstacle(332, 457, 407, 499))
    engine.insertView(engine.createObstacle(243, 480, 322, 505))
    engine.insertView(engine.createObstacle(283, 392, 292, 408))
    engine.insertView(engine.createObstacle(148, 374, 171, 406))
    engine.insertView(engine.createObstacle(163, 376, 239, 426))
    engine.insertView(engine.createObstacle(0, 485, 36, 515))
    engine.insertView(engine.createObstacle(64, 467, 95, 490))
    engine.insertView(engine.createObstacle(95, 480, 124, 502))
    engine.insertView(engine.createObstacle(0, 371, 13, 394))
    engine.insertView(engine.createObstacle(650, 153, 666, 161))
    engine.insertView(engine.createObstacle(583, 153, 607, 166))
    engine.insertView(engine.createObstacle(605, 150, 652, 170))
    engine.insertView(engine.createObstacle(576, 100, 673, 151))
    engine.insertView(engine.createObstacle(486, 112, 567, 165))
    engine.insertView(engine.createObstacle(565, 94, 576, 174))
    engine.insertView(engine.createObstacle(477, 90, 491, 171))
    engine.insertView(engine.createObstacle(399, 158, 455, 170))
    engine.insertView(engine.createObstacle(380, 113, 468, 157))
    engine.insertView(engine.createObstacle(189, 109, 205, 159))
    engine.insertView(engine.createObstacle(199, 109, 273, 168))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(145, 26, 153, 52), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(138, 13, 154, 30), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(205, 9, 215, 24), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(130, 0, 152, 13), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(196, 23, 222, 82), 1))
    engine.insertView(engine.enableFireThrough(engine.createObstacle(152, 0, 204, 86), 1))
    engine.insertView(engine.createObstacle(65, 185, 76, 205))
    engine.insertView(engine.createObstacle(67, 167, 84, 183))
    engine.insertView(engine.createObstacle(62, 140, 92, 166))
    engine.insertView(engine.createObstacle(1, 135, 67, 226))
    engine.insertView(engine.createObstacle(551, 284, 568, 301))
    engine.setInsertMode(2)
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(506, 224, 100014), 182))
    engine.insertView(engine.setOrderAxis(engine.createImage(52, 457, 100013), -40))
    engine.insertView(engine.setOrderAxis(engine.createImage(0, 315, 100012), -25))
    engine.insertView(engine.setOrderAxis(engine.createImage(219, 305, 100011), -32))
    engine.insertView(engine.setOrderAxis(engine.createImage(328, 335, 100010), -122))
    engine.insertView(engine.setOrderAxis(engine.createImage(528, 335, 100009), -123))
    engine.insertView(engine.setOrderAxis(engine.createImage(474, 89, 100008), -50))
    engine.insertView(engine.setOrderAxis(engine.createImage(369, 78, 100007), -59))
    engine.insertView(engine.setOrderAxis(engine.createImage(146, 331, 100006), -49))
    engine.insertView(engine.setOrderAxis(engine.createImage(183, 75, 100005), -46))
    engine.insertView(engine.setOrderAxis(engine.createImage(572, 74, 100004), -63))
    engine.setInsertMode(0)
    engine.insertBulanci()
    engine.insertVampires()
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
