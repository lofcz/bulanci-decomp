-- @startgame1 — the CStartGame1 sub-screen, as a layered scene behavior.
--
-- Faithful Luau port of client/src/startgame1.rs + widget.rs::RadioRowWidget.
-- The retail screen is a vertically-collapsing chain (CDSChained) of four
-- radio groups + a "Dále" continue button, with two-level focus (focused
-- group + active row), mouse + keyboard navigation, and an IP entry box that
-- only appears for a remote join.
--
-- The control tree is now *declarative*: every header/row/button/IP node is
-- authored in `scenes/main_menu.scene.json` under the `sg1` state group with a
-- stable `x:Name` (so it shows up in the editor and is editable there). This
-- module no longer *creates* nodes — `attach` resolves the fixed set by name
-- via `b.node(...)`, then each frame recomputes the collapsing layout and just
-- repositions + shows/hides + retints them. The layout math is a 1:1 port of
-- `get_sg1_layout`.

local b = require("@bulanci")

local M = {}

-- Colors (widget.rs): disabled 0x595959, unfocused-group 0x858585,
-- primary 0xCECECE, static 0x909090.
local C_DISABLED = 0x595959ff
local C_UNFOCUSED = 0x858585ff
local C_PRIMARY = 0xcececeff
local C_STATIC = 0x909090ff
local WHITE = 0xffffffff
local BTN_NORMAL_ALPHA = 0xffffff85   -- non-highlighted CButton bitmap alpha 0x85

local WINDOW_X, WINDOW_Y = 250, 60
local CONTENT_ROOT_Y = 20
local HEADER_TO_OPTION = 25
local OPTION_TO_OPTION = 17
local GROUP_TO_HEADER = 43
local CONT_Y = 456
local BUTTON_W, BUTTON_H = 82, 25

local RADIO = {
  selected = "menu/widgets/radio/radio_selected",
  unselected = "menu/widgets/radio/radio_unselected",
  selected_hover = "menu/widgets/radio/radio_selected_hover",
  unselected_hover = "menu/widgets/radio/radio_unselected_hover",
}
local DLG_NORMAL = "menu/widgets/dialog_button/btn_dialog_normal"
local DLG_HOVER = "menu/widgets/dialog_button/btn_dialog_hover"

-- Canonical items in fixed order; `g` = group (0..3), `v` = value index.
local ITEMS = {
  { g = 0, v = 0, label = "Vytvořit" },
  { g = 0, v = 1, label = "Připojit se" },
  { g = 1, v = 0, label = "1 hráč (trénink)" },
  { g = 1, v = 1, label = "2 hráči" },
  { g = 1, v = 2, label = "3 hráči" },
  { g = 1, v = 3, label = "4 hráči" },
  { g = 2, v = 0, label = "1 hráč" },
  { g = 2, v = 1, label = "2 hráči" },
  { g = 2, v = 2, label = "3 hráči" },
  { g = 3, v = 0, label = "TCP/IP protokol" },
  { g = 3, v = 1, label = "IPX protokol" },
}

-- opts: { state, on_close }. The nodes are declared in the scene doc; this
-- attaches behavior to the existing `sg1_*` controls (no creation).
function M.attach(opts)
  opts = opts or {}

  -- ---- form state (defaults match ClientApp::new). ----
  local sm = 0           -- server mode: 0 host, 1 join
  local tp = 3           -- total players
  local lp = 3           -- local players
  local ct = 0           -- connection type: 0 TCP/IP, 1 IPX
  local active = 1       -- canonical item index of the focused row
  local focused_group = 0
  local ip_input = "127.0.0.1"

  -- ---- bottom-right background art (anchored to the window corner). ----
  local bg = b.node("sg1_bg")
  local bw, bh = b.image_size("menu/backgrounds/bg_start")
  b.set_pos(bg, 800 - (bw or 0), 600 - (bh or 0))

  -- ---- 1:1 port of get_sg1_layout -> per-item {y,visible,checked,disabled}. ----
  local function layout()
    local is_host = sm == 0
    local g2 = is_host
    local g3 = not (is_host and tp == 1)
    local g4 = (not is_host) or (tp > 1 and lp < tp)
    local rows = {}
    local headers = {}
    local y = CONTENT_ROOT_Y
    headers[0] = { y = y, visible = true }; y = y + HEADER_TO_OPTION
    rows[1] = { y = y, visible = true, checked = sm == 0, disabled = false }; y = y + OPTION_TO_OPTION
    rows[2] = { y = y, visible = true, checked = sm == 1, disabled = false }
    if g2 then
      y = y + GROUP_TO_HEADER; headers[1] = { y = y, visible = true }; y = y + HEADER_TO_OPTION
      rows[3] = { y = y, visible = true, checked = tp == 1, disabled = false }; y = y + OPTION_TO_OPTION
      rows[4] = { y = y, visible = true, checked = tp == 2, disabled = false }; y = y + OPTION_TO_OPTION
      rows[5] = { y = y, visible = true, checked = tp == 3, disabled = false }; y = y + OPTION_TO_OPTION
      rows[6] = { y = y, visible = true, checked = tp == 4, disabled = false }
    end
    if g3 then
      y = y + GROUP_TO_HEADER; headers[2] = { y = y, visible = true }; y = y + HEADER_TO_OPTION
      rows[7] = { y = y, visible = true, checked = lp == 1, disabled = false }; y = y + OPTION_TO_OPTION
      rows[8] = { y = y, visible = true, checked = lp == 2, disabled = is_host and tp < 2 }; y = y + OPTION_TO_OPTION
      rows[9] = { y = y, visible = true, checked = lp == 3, disabled = is_host and tp < 3 }
    end
    if g4 then
      y = y + GROUP_TO_HEADER; headers[3] = { y = y, visible = true }; y = y + HEADER_TO_OPTION
      rows[10] = { y = y, visible = true, checked = ct == 0, disabled = false }; y = y + OPTION_TO_OPTION
      rows[11] = { y = y, visible = true, checked = ct == 1, disabled = false }
    end
    return rows, headers
  end

  -- ---- persistent nodes (declared in the scene doc, toggled per frame). ----
  local header_nodes = {}
  for g = 0, 3 do
    header_nodes[g] = b.node("sg1_h" .. g)
  end

  local row_nodes = {}
  for i = 1, #ITEMS do
    row_nodes[i] = {
      cb = b.node("sg1_r" .. i .. "_cb"),
      lbl = b.node("sg1_r" .. i .. "_lbl"),
      reg = b.node("sg1_r" .. i .. "_rg"),
    }
  end

  -- continue button.
  local cont_sprite = b.node("sg1_cont")
  local cont_lbl = b.node("sg1_cont_lbl")
  local cont_reg = b.node("sg1_cont_rg")

  -- IP entry box (shown only for remote join).
  local ip_label = b.node("sg1_ip_label")
  local ip_border = b.node("sg1_ip_border")
  local ip_bg = b.node("sg1_ip_bg")
  local ip_value = b.node("sg1_ip_value")

  -- ---- focus helpers (port of sg1_step_in_focused_group / ensure_valid_focus). ----
  local function group_list(rows, group)
    local list = {}
    for i, item in ipairs(ITEMS) do
      local r = rows[i]
      if r and r.visible and not r.disabled and item.g == group then
        table.insert(list, i)
      end
    end
    return list
  end

  local function step(rows, dir)
    local list = group_list(rows, focused_group)
    if #list == 0 then return active end
    local pos = 1
    for k, i in ipairs(list) do if i == active then pos = k; break end end
    local n = #list
    local np = ((pos - 1 + dir) % n) + 1
    return list[np]
  end

  local function ensure(rows)
    local r = rows[active]
    if (not r) or (not r.visible) or r.disabled then
      for i, _ in ipairs(ITEMS) do
        local rr = rows[i]
        if rr and rr.visible and not rr.disabled then active = i; break end
      end
    end
    local has = false
    for i, item in ipairs(ITEMS) do
      local rr = rows[i]
      if rr and rr.visible and item.g == focused_group then has = true; break end
    end
    if not has then
      for i, item in ipairs(ITEMS) do
        local rr = rows[i]
        if rr and rr.visible then focused_group = item.g; break end
      end
    end
  end

  -- ---- activation (port of activate_sg1_item_kind). ----
  local function activate(item)
    local g, v = item.g, item.v
    if g == 0 then
      if v == 0 then sm = 0; lp = math.min(lp, tp) else sm = 1 end
    elseif g == 1 then
      if v == 0 then tp = 1; lp = 1; sm = 0
      elseif v == 1 then tp = 2; lp = math.min(lp, 2)
      elseif v == 2 then tp = 3; lp = math.min(lp, 3)
      elseif v == 3 then tp = 4 end
    elseif g == 2 then
      lp = v + 1
    elseif g == 3 then
      ct = v
    end
    local rows = layout()
    ensure(rows)
  end

  local function kick_off()
    local addr
    if tp > 1 and sm == 1 then
      if ip_input:find(":", 1, true) then addr = ip_input else addr = ip_input .. ":34568" end
    else
      addr = "127.0.0.1:34568"
    end
    b.net.connect(addr)
  end

  -- ---- mouse handlers (engine edge events). ----
  for i, item in ipairs(ITEMS) do
    local rec = row_nodes[i]
    b.on_input(rec.reg, "press", function()
      if not b.state.active(opts.state) then return end
      local rows = layout()
      local r = rows[i]
      if r and r.visible and not r.disabled then
        focused_group = item.g
        active = i
        b.audio.play("menu/sfx/sfx_radio_click")
      end
    end)
    b.on_input(rec.reg, "click", function()
      if not b.state.active(opts.state) then return end
      local rows = layout()
      local r = rows[i]
      if r and r.visible and not r.disabled then
        focused_group = item.g
        active = i
        activate(item)
      end
    end)
  end
  b.on_input(cont_reg, "click", function()
    if not b.state.active(opts.state) then return end
    kick_off()
  end)

  -- ---- per-frame: keyboard, then visual refresh. ----
  b.subscribe("update", function()
    if not b.state.active(opts.state) then return end
    local rows = layout()

    if b.key_pressed("down") then active = step(rows, 1) end
    if b.key_pressed("up") then active = step(rows, -1) end
    if b.key_pressed("escape") then
      if opts.on_close then opts.on_close() end
      return
    end
    if b.key_pressed("enter") then
      local r = rows[active]
      if r and r.visible and not r.disabled then activate(ITEMS[active]) end
    end
    if tp > 1 and sm == 1 and b.key_pressed("backspace") then
      ip_input = ip_input:sub(1, -2)
    end
    ensure(rows)

    -- recompute after any state change, then lay out the fixed nodes.
    local headers
    rows, headers = layout()
    for g = 0, 3 do
      local h = headers[g]
      local node = header_nodes[g]
      if h and h.visible then
        b.set_pos(node, WINDOW_X + 20, WINDOW_Y + h.y + 13)
        b.set_visible(node, true)
      else
        b.set_visible(node, false)
      end
    end

    for i, item in ipairs(ITEMS) do
      local r = rows[i]
      local rec = row_nodes[i]
      if r and r.visible then
        local abs_y = WINDOW_Y + r.y
        b.set_pos(rec.cb, 280, abs_y)
        b.set_pos(rec.lbl, 306, abs_y + 13)
        b.set_pos(rec.reg, 280, abs_y)
        b.set_visible(rec.cb, true); b.set_visible(rec.lbl, true); b.set_visible(rec.reg, true)
        local group_focused = item.g == focused_group
        local hovered = group_focused and b.hovered(rec.reg)
        local tex, text_color
        if r.disabled then
          tex = r.checked and RADIO.selected or RADIO.unselected
          text_color = C_DISABLED
        elseif r.checked then
          tex = hovered and RADIO.selected_hover or RADIO.selected
          text_color = C_PRIMARY
        elseif hovered then
          tex = RADIO.unselected_hover
          text_color = C_PRIMARY
        else
          tex = RADIO.unselected
          text_color = C_STATIC
        end
        if (not r.disabled) and (not group_focused) then text_color = C_UNFOCUSED end
        local tint
        if r.disabled then tint = C_DISABLED
        elseif group_focused then tint = WHITE
        else tint = C_UNFOCUSED end
        b.set_sprite(rec.cb, tex)
        b.set_tint(rec.cb, tint)
        b.set_tint(rec.lbl, text_color)
      else
        b.set_visible(rec.cb, false); b.set_visible(rec.lbl, false); b.set_visible(rec.reg, false)
      end
    end

    -- continue button.
    local cy = WINDOW_Y + CONT_Y
    b.set_pos(cont_sprite, WINDOW_X + 20, cy)
    b.set_pos(cont_reg, WINDOW_X + 20, cy)
    local hov = b.hovered(cont_reg)
    b.set_sprite(cont_sprite, hov and DLG_HOVER or DLG_NORMAL)
    local bitmap_tint, text_color
    if hov then bitmap_tint = WHITE; text_color = C_PRIMARY
    else bitmap_tint = BTN_NORMAL_ALPHA; text_color = C_UNFOCUSED end
    b.set_tint(cont_sprite, bitmap_tint)
    local lh = b.font_metrics(16) or 16
    local _, base = b.font_metrics(16)
    base = base or 12
    local dw = b.measure("Dále", 16, 1)
    b.set_pos(cont_lbl, (WINDOW_X + 20) + math.floor((BUTTON_W - dw) * 0.5),
      cy + math.floor((BUTTON_H - lh) * 0.5) + base)
    b.set_tint(cont_lbl, text_color)

    -- IP box.
    local show_ip = tp > 1 and sm == 1
    b.set_visible(ip_label, show_ip)
    b.set_visible(ip_border, show_ip)
    b.set_visible(ip_bg, show_ip)
    b.set_visible(ip_value, show_ip)
    if show_ip then b.set_text(ip_value, ip_input) end
  end)

  b.on_char(function(ch)
    if not b.state.active(opts.state) then return end
    if tp > 1 and sm == 1 and #ip_input < 21 and ch:match("^[0-9.:]$") then
      ip_input = ip_input .. ch
    end
  end)

  return {
    reset = function()
      active = 1
      focused_group = 0
    end,
  }
end

return M
