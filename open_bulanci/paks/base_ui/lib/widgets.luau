-- @widgets — higher-level UI composed from the scene-engine primitives.
--
-- The engine core ships only primitives (group / sprite / text / quad /
-- input_region). Everything richer is built here in Luau, so the visual
-- look + behavior of buttons, frames, labels and lists is moddable: a
-- higher pak can ship its own "@widgets" and shadow this one.
--
-- Every constructor takes a single options table and returns a small
-- handle table of the node ids it created, so callers can tweak them
-- afterwards (e.g. retint on hover).

local b = require("@bulanci")

local M = {}

-- Default theme; a mod can mutate M.theme before building a scene.
-- `font_size` picks the bitmap font (small <=12, medium 13-15, large >=16).
M.theme = {
  font_size     = 16,
  panel_color   = 0x121821ff,
  button_color  = 0x202c3aff,
  button_hover  = 0x33455aff,
  text_color    = 0xe6edf3ff,
  accent_color  = 0x6fb3ffff,
}

-- A container group at (x, y), optionally bound to a conditional state so
-- it only shows while that state is active.
function M.frame(opts)
  opts = opts or {}
  local g = b.group(opts.parent or b.ROOT)
  b.set_pos(g, opts.x or 0, opts.y or 0)
  if opts.name then b.set_name(g, opts.name) end
  if opts.state then b.set_state_mask(g, b.state_bit(opts.state)) end
  return g
end

-- A text label.
function M.label(opts)
  opts = opts or {}
  local t = b.text(opts.parent or b.ROOT, opts.text or "", opts.size or M.theme.font_size, opts.spacing or 0)
  b.set_pos(t, opts.x or 0, opts.y or 0)
  b.set_tint(t, opts.color or M.theme.text_color)
  if opts.name then b.set_name(t, opts.name) end
  return t
end

-- A clickable button: background quad + centered-ish label + a hit region.
-- opts: { parent, x, y, w, h, label, font, color, hover, text_color,
--         on_click, on_hover, state }
function M.button(opts)
  opts = opts or {}
  local w = opts.w or 160
  local h = opts.h or 30
  local g = b.group(opts.parent or b.ROOT)
  b.set_pos(g, opts.x or 0, opts.y or 0)
  if opts.name then b.set_name(g, opts.name) end
  if opts.state then b.set_state_mask(g, b.state_bit(opts.state)) end

  local bg = b.quad(g, w, h, opts.color or M.theme.button_color)
  local label = b.text(g, opts.label or "", opts.size or M.theme.font_size, 0)
  b.set_pos(label, 10, math.floor((h - (opts.size or M.theme.font_size)) / 2))
  b.set_tint(label, opts.text_color or M.theme.text_color)

  local region = b.input_region(g, w, h)
  local hoverColor = opts.hover or M.theme.button_hover
  b.on_input(region, "hover", function() b.set_tint(bg, hoverColor) end)
  if opts.on_click then b.on_input(region, "click", opts.on_click) end
  if opts.on_hover then b.on_input(region, "hover", opts.on_hover) end

  return { root = g, bg = bg, label = label, region = region }
end

-- A vertical stack of buttons. opts.items = { {label=, on_click=}, ... }.
-- Returns the list of button handles.
function M.button_list(opts)
  opts = opts or {}
  local x = opts.x or 0
  local y = opts.y or 0
  local gap = opts.gap or 8
  local h = opts.h or 30
  local out = {}
  for i, item in ipairs(opts.items or {}) do
    out[i] = M.button({
      parent = opts.parent,
      x = x,
      y = y + (i - 1) * (h + gap),
      w = opts.w, h = h,
      label = item.label,
      on_click = item.on_click,
      state = opts.state,
    })
  end
  return out
end

return M
