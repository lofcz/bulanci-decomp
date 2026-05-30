-- @poem — the CPoemScroller, as a scene behavior.
--
-- Faithful Luau port of client/src/poem.rs + render.rs::render_poem_text
-- (CPoemScroller::Render FUN_004240f0, OnScheduledTick FUN_00425df0):
--   * poems live as VFS text "poems/poem_<N>.txt" (read via b.read_text),
--   * each line may carry a [CENTER] / [RIGHT] prefix, else left-aligns,
--   * a 205x166 rect at (10,384), medium font (13 px line height),
--     char-spacing 1, color 0xB0B0B0,
--   * scrolls up 1 px / 120 ms after a 3000 ms initial delay, starting at
--     y=550, looping to a fresh random poem once fully off the top,
--   * the original's two 26 px shader fade bands are reproduced here as a
--     per-line tint alpha (the agreed "minimal raylib draws" boundary).

local b = require("@bulanci")

local M = {}

local RECT_X = 10
local RECT_Y = 384
local RECT_W = 205
local RECT_H = 166
local LINE_H = 13
local SPACING = 1
local FADE = 26          -- top/bottom fade band height
local BASE_RGB = 0xb0b0b0

local function trim(s)
  return (s:gsub("^%s+", ""):gsub("%s+$", ""))
end

local function parse_line(raw)
  local s = trim(raw)
  local rest = s:match("^%[CENTER%](.*)$")
  if rest then return { text = trim(rest), align = "center" } end
  rest = s:match("^%[RIGHT%](.*)$")
  if rest then return { text = trim(rest), align = "right" } end
  return { text = s, align = "left" }
end

-- Split on "\n" with Rust `str::lines()` semantics (a single trailing
-- newline does not yield a final empty line; blank stanza lines are kept).
local function split_lines(s)
  local out = {}
  local start = 1
  while true do
    local nl = s:find("\n", start, true)
    if not nl then
      table.insert(out, s:sub(start))
      break
    end
    table.insert(out, s:sub(start, nl - 1))
    start = nl + 1
  end
  for i, l in ipairs(out) do out[i] = (l:gsub("\r$", "")) end
  if #out > 0 and out[#out] == "" and s:sub(-1) == "\n" then
    out[#out] = nil
  end
  return out
end

-- Load up to 13 poems ("poems/poem_<N>.txt"), each as a list of parsed
-- {text, align} lines. Shared by the scroller and the History page.
function M.load()
  local poems = {}
  for i = 0, 12 do
    local txt = b.read_text("poems/poem_" .. i .. ".txt")
    if txt then
      local lines = {}
      for _, raw in ipairs(split_lines(txt)) do
        table.insert(lines, parse_line(raw))
      end
      table.insert(poems, lines)
    end
  end
  return poems
end

-- opts: { parent, state }
function M.mount(opts)
  opts = opts or {}
  local parent = b.group(opts.parent or b.ROOT)
  if opts.state then b.set_state_mask(parent, b.state_bit(opts.state)) end

  local poems = M.load()
  if #poems == 0 then return parent end

  local idx = b.rand(0, #poems - 1) + 1
  local scroll_y = 550
  local nodes = {}     -- one text node per line of the current poem

  local function clear_nodes()
    for _, n in ipairs(nodes) do b.remove(n) end
    nodes = {}
  end

  local function build()
    clear_nodes()
    for i, line in ipairs(poems[idx]) do
      local n = b.text(parent, line.text, LINE_H, SPACING, "top")
      nodes[i] = n
    end
  end

  local function reposition()
    local lines = poems[idx]
    for i, line in ipairs(lines) do
      local n = nodes[i]
      if n then
        local screen_y = scroll_y + (i - 1) * LINE_H
        local y_rt = screen_y - RECT_Y
        if y_rt + LINE_H >= 0 and y_rt <= RECT_H then
          local w = b.measure(line.text, LINE_H, SPACING)
          local x
          if line.align == "center" then
            x = RECT_X + math.floor((RECT_W - w) * 0.5)
          elseif line.align == "right" then
            x = RECT_X + math.floor(RECT_W - w - 2)
          else
            x = RECT_X + 2
          end
          b.set_pos(n, x, math.floor(screen_y))
          -- Two-band edge fade reproduced as per-line alpha.
          local yc = y_rt + LINE_H * 0.5
          local a_top = math.clamp(yc / FADE, 0, 1)
          local a_bot = math.clamp((RECT_H - yc) / FADE, 0, 1)
          local alpha = math.floor(math.min(a_top, a_bot) * 255 + 0.5)
          b.set_tint(n, BASE_RGB * 256 + alpha)
          b.set_visible(n, true)
        else
          b.set_visible(n, false)
        end
      end
    end
  end

  local function scroll()
    scroll_y = scroll_y - 1
    local height = #poems[idx] * LINE_H
    if scroll_y + height < RECT_Y then
      idx = b.rand(0, #poems - 1) + 1
      scroll_y = 550
      build()
    end
    reposition()
  end

  build()
  reposition()
  -- 3000 ms initial delay, then strict 1 px / 120 ms (first scroll at 3120).
  b.after(3000, function()
    b.every(120, scroll)
  end)

  return parent
end

return M
