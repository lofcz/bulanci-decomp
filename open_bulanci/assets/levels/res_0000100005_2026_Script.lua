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

local function fn_0x042c(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 7))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x043e::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x0482 end
if _sw == 1 then goto lbl_0x04a3 end
if _sw == 2 then goto lbl_0x04c4 end
if _sw == 3 then goto lbl_0x04e5 end
if _sw == 4 then goto lbl_0x0506 end
if _sw == 5 then goto lbl_0x0527 end
if _sw == 6 then goto lbl_0x0548 end
if _sw == 7 then goto lbl_0x0569 end
    ::lbl_0x0482::
    if fn_0x0000(get_local(v, args, 2, 2), 91, 11) == 1 then goto lbl_0x05b6 end
    engine.goto(1418)
    ::lbl_0x04a3::
    if fn_0x0000(get_local(v, args, 2, 2), 401, 305) == 1 then goto lbl_0x05b6 end
    engine.goto(1418)
    ::lbl_0x04c4::
    if fn_0x0000(get_local(v, args, 2, 2), 655, 466) == 1 then goto lbl_0x05b6 end
    engine.goto(1418)
    ::lbl_0x04e5::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x05b6 end
    engine.goto(1418)
    ::lbl_0x0506::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 112) == 1 then goto lbl_0x05b6 end
    engine.goto(1418)
    ::lbl_0x0527::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 11) == 1 then goto lbl_0x05b6 end
    engine.goto(1418)
    ::lbl_0x0548::
    if fn_0x0000(get_local(v, args, 2, 2), 188, 11) == 1 then goto lbl_0x05b6 end
    engine.goto(1418)
    ::lbl_0x0569::
    if fn_0x0000(get_local(v, args, 2, 2), 293, 11) == 1 then goto lbl_0x05b6 end
    engine.goto(1418)
    ::lbl_0x058a::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 8 then goto lbl_0x05a7 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x05a7::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x043e end
    do return 0 end
    ::lbl_0x05b6::
    do return 1 end
end

local function fn_0x05bc(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 7))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x05ce::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x0612 end
if _sw == 1 then goto lbl_0x0633 end
if _sw == 2 then goto lbl_0x0654 end
if _sw == 3 then goto lbl_0x0675 end
if _sw == 4 then goto lbl_0x0696 end
if _sw == 5 then goto lbl_0x06b7 end
if _sw == 6 then goto lbl_0x06d8 end
if _sw == 7 then goto lbl_0x06f9 end
    ::lbl_0x0612::
    if fn_0x0000(get_local(v, args, 2, 2), 191, 466) == 1 then goto lbl_0x0746 end
    engine.goto(1818)
    ::lbl_0x0633::
    if fn_0x0000(get_local(v, args, 2, 2), 401, 305) == 1 then goto lbl_0x0746 end
    engine.goto(1818)
    ::lbl_0x0654::
    if fn_0x0000(get_local(v, args, 2, 2), 655, 466) == 1 then goto lbl_0x0746 end
    engine.goto(1818)
    ::lbl_0x0675::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x0746 end
    engine.goto(1818)
    ::lbl_0x0696::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 112) == 1 then goto lbl_0x0746 end
    engine.goto(1818)
    ::lbl_0x06b7::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 11) == 1 then goto lbl_0x0746 end
    engine.goto(1818)
    ::lbl_0x06d8::
    if fn_0x0000(get_local(v, args, 2, 2), 188, 11) == 1 then goto lbl_0x0746 end
    engine.goto(1818)
    ::lbl_0x06f9::
    if fn_0x0000(get_local(v, args, 2, 2), 293, 11) == 1 then goto lbl_0x0746 end
    engine.goto(1818)
    ::lbl_0x071a::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 8 then goto lbl_0x0737 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x0737::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x05ce end
    do return 0 end
    ::lbl_0x0746::
    do return 1 end
end

local function fn_0x074c(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 7))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x075e::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x07a2 end
if _sw == 1 then goto lbl_0x07c3 end
if _sw == 2 then goto lbl_0x07e4 end
if _sw == 3 then goto lbl_0x0805 end
if _sw == 4 then goto lbl_0x0826 end
if _sw == 5 then goto lbl_0x0847 end
if _sw == 6 then goto lbl_0x0868 end
if _sw == 7 then goto lbl_0x0889 end
    ::lbl_0x07a2::
    if fn_0x0000(get_local(v, args, 2, 2), 191, 466) == 1 then goto lbl_0x08d6 end
    engine.goto(2218)
    ::lbl_0x07c3::
    if fn_0x0000(get_local(v, args, 2, 2), 91, 11) == 1 then goto lbl_0x08d6 end
    engine.goto(2218)
    ::lbl_0x07e4::
    if fn_0x0000(get_local(v, args, 2, 2), 655, 466) == 1 then goto lbl_0x08d6 end
    engine.goto(2218)
    ::lbl_0x0805::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x08d6 end
    engine.goto(2218)
    ::lbl_0x0826::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 112) == 1 then goto lbl_0x08d6 end
    engine.goto(2218)
    ::lbl_0x0847::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 11) == 1 then goto lbl_0x08d6 end
    engine.goto(2218)
    ::lbl_0x0868::
    if fn_0x0000(get_local(v, args, 2, 2), 188, 11) == 1 then goto lbl_0x08d6 end
    engine.goto(2218)
    ::lbl_0x0889::
    if fn_0x0000(get_local(v, args, 2, 2), 293, 11) == 1 then goto lbl_0x08d6 end
    engine.goto(2218)
    ::lbl_0x08aa::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 8 then goto lbl_0x08c7 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x08c7::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x075e end
    do return 0 end
    ::lbl_0x08d6::
    do return 1 end
end

local function fn_0x08dc(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 7))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x08ee::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x0932 end
if _sw == 1 then goto lbl_0x0953 end
if _sw == 2 then goto lbl_0x0974 end
if _sw == 3 then goto lbl_0x0995 end
if _sw == 4 then goto lbl_0x09b6 end
if _sw == 5 then goto lbl_0x09d7 end
if _sw == 6 then goto lbl_0x09f8 end
if _sw == 7 then goto lbl_0x0a19 end
    ::lbl_0x0932::
    if fn_0x0000(get_local(v, args, 2, 2), 191, 466) == 1 then goto lbl_0x0a66 end
    engine.goto(2618)
    ::lbl_0x0953::
    if fn_0x0000(get_local(v, args, 2, 2), 91, 11) == 1 then goto lbl_0x0a66 end
    engine.goto(2618)
    ::lbl_0x0974::
    if fn_0x0000(get_local(v, args, 2, 2), 401, 305) == 1 then goto lbl_0x0a66 end
    engine.goto(2618)
    ::lbl_0x0995::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x0a66 end
    engine.goto(2618)
    ::lbl_0x09b6::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 112) == 1 then goto lbl_0x0a66 end
    engine.goto(2618)
    ::lbl_0x09d7::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 11) == 1 then goto lbl_0x0a66 end
    engine.goto(2618)
    ::lbl_0x09f8::
    if fn_0x0000(get_local(v, args, 2, 2), 188, 11) == 1 then goto lbl_0x0a66 end
    engine.goto(2618)
    ::lbl_0x0a19::
    if fn_0x0000(get_local(v, args, 2, 2), 293, 11) == 1 then goto lbl_0x0a66 end
    engine.goto(2618)
    ::lbl_0x0a3a::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 8 then goto lbl_0x0a57 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x0a57::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x08ee end
    do return 0 end
    ::lbl_0x0a66::
    do return 1 end
end

local function fn_0x0a6c(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 7))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x0a7e::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x0ac2 end
if _sw == 1 then goto lbl_0x0ae3 end
if _sw == 2 then goto lbl_0x0b04 end
if _sw == 3 then goto lbl_0x0b25 end
if _sw == 4 then goto lbl_0x0b46 end
if _sw == 5 then goto lbl_0x0b67 end
if _sw == 6 then goto lbl_0x0b88 end
if _sw == 7 then goto lbl_0x0ba9 end
    ::lbl_0x0ac2::
    if fn_0x0000(get_local(v, args, 2, 2), 191, 466) == 1 then goto lbl_0x0bf6 end
    engine.goto(3018)
    ::lbl_0x0ae3::
    if fn_0x0000(get_local(v, args, 2, 2), 91, 11) == 1 then goto lbl_0x0bf6 end
    engine.goto(3018)
    ::lbl_0x0b04::
    if fn_0x0000(get_local(v, args, 2, 2), 401, 305) == 1 then goto lbl_0x0bf6 end
    engine.goto(3018)
    ::lbl_0x0b25::
    if fn_0x0000(get_local(v, args, 2, 2), 655, 466) == 1 then goto lbl_0x0bf6 end
    engine.goto(3018)
    ::lbl_0x0b46::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 112) == 1 then goto lbl_0x0bf6 end
    engine.goto(3018)
    ::lbl_0x0b67::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 11) == 1 then goto lbl_0x0bf6 end
    engine.goto(3018)
    ::lbl_0x0b88::
    if fn_0x0000(get_local(v, args, 2, 2), 188, 11) == 1 then goto lbl_0x0bf6 end
    engine.goto(3018)
    ::lbl_0x0ba9::
    if fn_0x0000(get_local(v, args, 2, 2), 293, 11) == 1 then goto lbl_0x0bf6 end
    engine.goto(3018)
    ::lbl_0x0bca::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 8 then goto lbl_0x0be7 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x0be7::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x0a7e end
    do return 0 end
    ::lbl_0x0bf6::
    do return 1 end
end

local function fn_0x0bfc(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 7))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x0c0e::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x0c52 end
if _sw == 1 then goto lbl_0x0c73 end
if _sw == 2 then goto lbl_0x0c94 end
if _sw == 3 then goto lbl_0x0cb5 end
if _sw == 4 then goto lbl_0x0cd6 end
if _sw == 5 then goto lbl_0x0cf7 end
if _sw == 6 then goto lbl_0x0d18 end
if _sw == 7 then goto lbl_0x0d39 end
    ::lbl_0x0c52::
    if fn_0x0000(get_local(v, args, 2, 2), 191, 466) == 1 then goto lbl_0x0d86 end
    engine.goto(3418)
    ::lbl_0x0c73::
    if fn_0x0000(get_local(v, args, 2, 2), 91, 11) == 1 then goto lbl_0x0d86 end
    engine.goto(3418)
    ::lbl_0x0c94::
    if fn_0x0000(get_local(v, args, 2, 2), 401, 305) == 1 then goto lbl_0x0d86 end
    engine.goto(3418)
    ::lbl_0x0cb5::
    if fn_0x0000(get_local(v, args, 2, 2), 655, 466) == 1 then goto lbl_0x0d86 end
    engine.goto(3418)
    ::lbl_0x0cd6::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x0d86 end
    engine.goto(3418)
    ::lbl_0x0cf7::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 11) == 1 then goto lbl_0x0d86 end
    engine.goto(3418)
    ::lbl_0x0d18::
    if fn_0x0000(get_local(v, args, 2, 2), 188, 11) == 1 then goto lbl_0x0d86 end
    engine.goto(3418)
    ::lbl_0x0d39::
    if fn_0x0000(get_local(v, args, 2, 2), 293, 11) == 1 then goto lbl_0x0d86 end
    engine.goto(3418)
    ::lbl_0x0d5a::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 8 then goto lbl_0x0d77 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x0d77::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x0c0e end
    do return 0 end
    ::lbl_0x0d86::
    do return 1 end
end

local function fn_0x0d8c(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 7))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x0d9e::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x0de2 end
if _sw == 1 then goto lbl_0x0e03 end
if _sw == 2 then goto lbl_0x0e24 end
if _sw == 3 then goto lbl_0x0e45 end
if _sw == 4 then goto lbl_0x0e66 end
if _sw == 5 then goto lbl_0x0e87 end
if _sw == 6 then goto lbl_0x0ea8 end
if _sw == 7 then goto lbl_0x0ec9 end
    ::lbl_0x0de2::
    if fn_0x0000(get_local(v, args, 2, 2), 191, 466) == 1 then goto lbl_0x0f16 end
    engine.goto(3818)
    ::lbl_0x0e03::
    if fn_0x0000(get_local(v, args, 2, 2), 91, 11) == 1 then goto lbl_0x0f16 end
    engine.goto(3818)
    ::lbl_0x0e24::
    if fn_0x0000(get_local(v, args, 2, 2), 401, 305) == 1 then goto lbl_0x0f16 end
    engine.goto(3818)
    ::lbl_0x0e45::
    if fn_0x0000(get_local(v, args, 2, 2), 655, 466) == 1 then goto lbl_0x0f16 end
    engine.goto(3818)
    ::lbl_0x0e66::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x0f16 end
    engine.goto(3818)
    ::lbl_0x0e87::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 112) == 1 then goto lbl_0x0f16 end
    engine.goto(3818)
    ::lbl_0x0ea8::
    if fn_0x0000(get_local(v, args, 2, 2), 188, 11) == 1 then goto lbl_0x0f16 end
    engine.goto(3818)
    ::lbl_0x0ec9::
    if fn_0x0000(get_local(v, args, 2, 2), 293, 11) == 1 then goto lbl_0x0f16 end
    engine.goto(3818)
    ::lbl_0x0eea::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 8 then goto lbl_0x0f07 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x0f07::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x0d9e end
    do return 0 end
    ::lbl_0x0f16::
    do return 1 end
end

local function fn_0x0f1c(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 7))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x0f2e::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x0f72 end
if _sw == 1 then goto lbl_0x0f93 end
if _sw == 2 then goto lbl_0x0fb4 end
if _sw == 3 then goto lbl_0x0fd5 end
if _sw == 4 then goto lbl_0x0ff6 end
if _sw == 5 then goto lbl_0x1017 end
if _sw == 6 then goto lbl_0x1038 end
if _sw == 7 then goto lbl_0x1059 end
    ::lbl_0x0f72::
    if fn_0x0000(get_local(v, args, 2, 2), 191, 466) == 1 then goto lbl_0x10a6 end
    engine.goto(4218)
    ::lbl_0x0f93::
    if fn_0x0000(get_local(v, args, 2, 2), 91, 11) == 1 then goto lbl_0x10a6 end
    engine.goto(4218)
    ::lbl_0x0fb4::
    if fn_0x0000(get_local(v, args, 2, 2), 401, 305) == 1 then goto lbl_0x10a6 end
    engine.goto(4218)
    ::lbl_0x0fd5::
    if fn_0x0000(get_local(v, args, 2, 2), 655, 466) == 1 then goto lbl_0x10a6 end
    engine.goto(4218)
    ::lbl_0x0ff6::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x10a6 end
    engine.goto(4218)
    ::lbl_0x1017::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 112) == 1 then goto lbl_0x10a6 end
    engine.goto(4218)
    ::lbl_0x1038::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 11) == 1 then goto lbl_0x10a6 end
    engine.goto(4218)
    ::lbl_0x1059::
    if fn_0x0000(get_local(v, args, 2, 2), 293, 11) == 1 then goto lbl_0x10a6 end
    engine.goto(4218)
    ::lbl_0x107a::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 8 then goto lbl_0x1097 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x1097::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x0f2e end
    do return 0 end
    ::lbl_0x10a6::
    do return 1 end
end

local function fn_0x10ac(...)
    local args = {...}
    local v = {}
    set_local(v, args, 2, 0, math.random(0, 7))
    set_local(v, args, 2, 1, get_local(v, args, 2, 0))
    ::lbl_0x10be::
    local _sw = get_local(v, args, 2, 1)
if _sw == 0 then goto lbl_0x1102 end
if _sw == 1 then goto lbl_0x1123 end
if _sw == 2 then goto lbl_0x1144 end
if _sw == 3 then goto lbl_0x1165 end
if _sw == 4 then goto lbl_0x1186 end
if _sw == 5 then goto lbl_0x11a7 end
if _sw == 6 then goto lbl_0x11c8 end
if _sw == 7 then goto lbl_0x11e9 end
    ::lbl_0x1102::
    if fn_0x0000(get_local(v, args, 2, 2), 191, 466) == 1 then goto lbl_0x1236 end
    engine.goto(4618)
    ::lbl_0x1123::
    if fn_0x0000(get_local(v, args, 2, 2), 91, 11) == 1 then goto lbl_0x1236 end
    engine.goto(4618)
    ::lbl_0x1144::
    if fn_0x0000(get_local(v, args, 2, 2), 401, 305) == 1 then goto lbl_0x1236 end
    engine.goto(4618)
    ::lbl_0x1165::
    if fn_0x0000(get_local(v, args, 2, 2), 655, 466) == 1 then goto lbl_0x1236 end
    engine.goto(4618)
    ::lbl_0x1186::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 466) == 1 then goto lbl_0x1236 end
    engine.goto(4618)
    ::lbl_0x11a7::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 112) == 1 then goto lbl_0x1236 end
    engine.goto(4618)
    ::lbl_0x11c8::
    if fn_0x0000(get_local(v, args, 2, 2), 739, 11) == 1 then goto lbl_0x1236 end
    engine.goto(4618)
    ::lbl_0x11e9::
    if fn_0x0000(get_local(v, args, 2, 2), 188, 11) == 1 then goto lbl_0x1236 end
    engine.goto(4618)
    ::lbl_0x120a::
    set_local(v, args, 2, 1, (get_local(v, args, 2, 1) + 1))
    if get_local(v, args, 2, 1) ~= 8 then goto lbl_0x1227 end
    set_local(v, args, 2, 1, 0)
    ::lbl_0x1227::
    if get_local(v, args, 2, 1) ~= get_local(v, args, 2, 0) then goto lbl_0x10be end
    do return 0 end
    ::lbl_0x1236::
    do return 1 end
end

function GetInfo(...)
    local args = {...}
    local v = {}
    engine.set_global(1, 1000)
    engine.set_global(2, "C4F1A65C-5057-4D19-B707-F3FF4A008B74")
    engine.set_global(0, "Bludiště")
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
    engine.insertView(engine.createObstacle(623, 207, 657, 235))
    engine.insertView(engine.createObstacle(52, 242, 91, 460))
    engine.insertView(engine.createObstacle(56, 214, 88, 246))
    engine.insertView(engine.createObstacle(52, 0, 89, 212))
    engine.insertView(engine.createObstacle(353, 76, 385, 101))
    engine.insertView(engine.createObstacle(148, 297, 185, 515))
    engine.insertView(engine.createObstacle(244, 419, 554, 458))
    engine.insertView(engine.createObstacle(237, 265, 277, 350))
    engine.insertView(engine.createObstacle(331, 270, 368, 378))
    engine.insertView(engine.createObstacle(482, 267, 522, 378))
    engine.insertView(engine.createObstacle(611, 233, 651, 515))
    engine.insertView(engine.createObstacle(709, 265, 747, 515))
    engine.insertView(engine.createObstacle(613, 162, 800, 204))
    engine.insertView(engine.createObstacle(461, 60, 800, 102))
    engine.insertView(engine.createObstacle(349, 0, 396, 70))
    engine.insertView(engine.createObstacle(238, 0, 284, 69))
    engine.insertView(engine.createObstacle(243, 165, 555, 203))
    engine.insertView(engine.createObstacle(143, 0, 182, 212))
    engine.setInsertMode(2)
    engine.setInsertMode(1)
    engine.insertView(engine.setOrderAxis(engine.createImage(620, 206, 100004), 0))
    engine.setInsertMode(0)
    if engine.isServer() == 0 then goto lbl_0x03f1 end
    engine.defineTraceArea(0, 177, 450, 257, 520, 14)
    engine.defineTraceArea(1, 77, -5, 157, 65, 14)
    engine.defineTraceArea(2, 387, 289, 467, 359, 14)
    engine.defineTraceArea(3, 641, 450, 721, 520, 14)
    engine.defineTraceArea(4, 725, 450, 805, 520, 14)
    engine.defineTraceArea(5, 725, 96, 805, 166, 14)
    engine.defineTraceArea(6, 725, -5, 805, 65, 14)
    engine.defineTraceArea(7, 174, -5, 254, 65, 14)
    engine.defineTraceArea(8, 279, -5, 359, 65, 14)
    ::lbl_0x03f1::
    engine.insertBulanci()
    do return 0 end
end

function OnDeinit(...)
    local args = {...}
    local v = {}
    if engine.get_global(3) == 0 then goto lbl_0x040a end
    engine.set_global(3, engine.strmDestroy(engine.get_global(3)))
    ::lbl_0x040a::
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
if _sw == 0 then goto lbl_0x1289 end
if _sw == 1 then goto lbl_0x1296 end
if _sw == 2 then goto lbl_0x12a3 end
if _sw == 3 then goto lbl_0x12b0 end
if _sw == 4 then goto lbl_0x12bd end
if _sw == 5 then goto lbl_0x12ca end
if _sw == 6 then goto lbl_0x12d7 end
if _sw == 7 then goto lbl_0x12e4 end
if _sw == 8 then goto lbl_0x12f1 end
    ::lbl_0x1289::
    fn_0x042c(get_local(v, args, 0, 1))
    engine.goto(4862)
    ::lbl_0x1296::
    fn_0x05bc(get_local(v, args, 0, 1))
    engine.goto(4862)
    ::lbl_0x12a3::
    fn_0x074c(get_local(v, args, 0, 1))
    engine.goto(4862)
    ::lbl_0x12b0::
    fn_0x08dc(get_local(v, args, 0, 1))
    engine.goto(4862)
    ::lbl_0x12bd::
    fn_0x0a6c(get_local(v, args, 0, 1))
    engine.goto(4862)
    ::lbl_0x12ca::
    fn_0x0bfc(get_local(v, args, 0, 1))
    engine.goto(4862)
    ::lbl_0x12d7::
    fn_0x0d8c(get_local(v, args, 0, 1))
    engine.goto(4862)
    ::lbl_0x12e4::
    fn_0x0f1c(get_local(v, args, 0, 1))
    engine.goto(4862)
    ::lbl_0x12f1::
    fn_0x10ac(get_local(v, args, 0, 1))
    engine.goto(4862)
    ::lbl_0x12fe::
    do return 0 end
end

function OnLeave(...)
    local args = {...}
    local v = {}
    local _sw = get_local(v, args, 0, 0)
if _sw == 0 then goto lbl_0x1351 end
if _sw == 1 then goto lbl_0x1356 end
if _sw == 2 then goto lbl_0x135b end
if _sw == 3 then goto lbl_0x1360 end
if _sw == 4 then goto lbl_0x1365 end
if _sw == 5 then goto lbl_0x136a end
if _sw == 6 then goto lbl_0x136f end
if _sw == 7 then goto lbl_0x1374 end
if _sw == 8 then goto lbl_0x1379 end
    ::lbl_0x1351::
    engine.goto(4990)
    ::lbl_0x1356::
    engine.goto(4990)
    ::lbl_0x135b::
    engine.goto(4990)
    ::lbl_0x1360::
    engine.goto(4990)
    ::lbl_0x1365::
    engine.goto(4990)
    ::lbl_0x136a::
    engine.goto(4990)
    ::lbl_0x136f::
    engine.goto(4990)
    ::lbl_0x1374::
    engine.goto(4990)
    ::lbl_0x1379::
    engine.goto(4990)
    ::lbl_0x137e::
    do return 0 end
end

function OnNetCustom(...)
    local args = {...}
    local v = {}
    set_local(v, args, 4, 0, engine.strmRead(get_local(v, args, 4, 4), 1))
    if get_local(v, args, 4, 0) ~= 1 then goto lbl_0x13c5 end
    set_local(v, args, 4, 3, engine.strmRead(get_local(v, args, 4, 4), 1))
    set_local(v, args, 4, 1, engine.strmRead(get_local(v, args, 4, 4), 4))
    set_local(v, args, 4, 2, engine.strmRead(get_local(v, args, 4, 4), 4))
    engine.teleportPlayerTo(get_local(v, args, 4, 3), get_local(v, args, 4, 1), get_local(v, args, 4, 2), 1)
    ::lbl_0x13c5::
    do return 0 end
end

function OnGameStart(...)
    local args = {...}
    local v = {}
    do return 0 end
end
