-- main_menu / history — the paged poem viewer (code-behind #3; CPoemScroller's
-- history mode). The static chrome (backdrop, title, rule, prompt) is declared
-- in the scene under the state-gated "history" group; here we position the
-- asset-sized backdrop, render the current page's lines, and handle paging.

local b = require("@bulanci")
local poem = require("@poem")
local M = _G.MENU

local BX, BY, BW, BH = 250, 60, 510, 500

local hist = b.node("history")
local prompt = b.node("hist_prompt")
local bg = b.node("hist_bg")

-- Position the backdrop bottom-right (its size is asset-dependent).
local hw, hh = b.image_size("menu/backgrounds/bg_history")
b.set_pos(bg, 800 - (hw or 0), 600 - (hh or 0))

local poems = poem.load()
local page = 0
local line_nodes = {}

local function rebuild()
  for _, n in ipairs(line_nodes) do b.remove(n) end
  line_nodes = {}
  if #poems == 0 then return end
  local lines = poems[page + 1]
  local inner_w = BW - 40
  local y = BY + 70
  for _, line in ipairs(lines) do
    local tw = b.measure(line.text, 16, 0)
    local x
    if line.align == "center" then x = BX + 20 + (inner_w - tw) * 0.5
    elseif line.align == "right" then x = BX + BW - 20 - tw
    else x = BX + 20 end
    local n = b.text(hist, line.text, 16, 0, "baseline")
    b.set_pos(n, x, y)
    b.set_tint(n, 0xf2f2f2ff)
    b.set_visible(n, y < BY + BH - 40)
    table.insert(line_nodes, n)
    y = y + 22
  end
end

local function refresh_prompt()
  b.set_text(prompt, "Strana " .. (page + 1) .. " / " .. math.max(#poems, 1) .. "    < — →    Esc pro návrat")
end

rebuild(); refresh_prompt()

b.subscribe("update", function()
  if not b.state.active("history") then return end
  if #poems == 0 then
    if b.key_pressed("escape") then M.close_sub() end
    return
  end
  local changed = false
  if b.key_pressed("right") or b.key_pressed("down") then page = (page + 1) % #poems; changed = true end
  if b.key_pressed("left") or b.key_pressed("up") then page = (page + #poems - 1) % #poems; changed = true end
  if b.key_pressed("home") then page = 0; changed = true end
  if b.key_pressed("end") then page = #poems - 1; changed = true end
  if b.key_pressed("escape") then M.close_sub(); return end
  if changed then rebuild(); refresh_prompt() end
end)
