-- connecting — the "joining the server" screen (declarative scene).
--
-- The background + the two status lines are declared in connecting.scene.json;
-- this code-behind only horizontally centers each line (its width is
-- font-dependent), mirroring the old hardcoded `draw_connecting` math. The Esc
-- abort + the connect/playing phase transitions stay on the Rust side
-- (`ClientApp::update_connecting_input` / `poll_network`), since they manage
-- the socket + app phase the scene can't own.

local b = require("@bulanci")

local function center_x(name, text, size)
  local tw = b.measure(text, size, 0)
  b.set_pos(b.node(name), (800 - tw) * 0.5, name == "conn_title" and 260 or 300)
end

center_x("conn_title", "Připojování k serveru…", 22)
center_x("conn_sub", "Esc pro zrušení", 14)
