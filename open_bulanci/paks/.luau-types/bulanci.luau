--!nonstrict
-- Type stub for the engine-provided `@bulanci` module.
--
-- At runtime there is NO such file: the Rust SceneHost injects the real
-- table via `lua.register_module("@bulanci", ...)` (see
-- open_bulanci/core/src/scene/host.rs :: install_api). This stub exists
-- ONLY so the Luau language server can resolve `require("@bulanci")` and
-- type-check / autocomplete the `bulanci.*` surface. It is never packed
-- into a pak (it lives outside any pak folder) and is never executed.
--
-- Keep this in sync with `install_api` — it is the single source of truth
-- for the scripting API the language server sees. `API_VERSION` mirrors the
-- Rust constant so scenes can feature-gate.

-- A scene-graph node id. `ROOT` is the implicit root every node attaches
-- under. Returned by the constructors (group/sprite/text/quad/...).
export type Node = number

-- Scheduler phase a behavior can `subscribe` to, in tick order.
export type Phase = "preupdate" | "update" | "postupdate" | "draw"

-- Pointer event an `input_region` can listen for via `on_input`.
export type InputEvent = "hover" | "leave" | "press" | "release" | "click"

-- Text vertical anchor: "baseline" (default, draw_t semantics) or "top"
-- (cell-top, what the poem / radio rows use).
export type TextAnchor = "baseline" | "top"

-- Atlas frame motion delta (cursor idle-twitch path step).
export type AtlasMotion = { dx: number, dy: number }

-- Atlas metadata returned by `bulanci.atlas(path)` (nil if not an atlas).
export type AtlasInfo = {
	count: number,
	ox: number,
	oy: number,
	motion: { AtlasMotion },
}

-- Conditional-state controls. Nodes are gated by a state mask; toggling a
-- state shows/hides every node tagged with its bit.
export type StateApi = {
	set: (name: string, on: boolean) -> (),
	active: (name: string) -> boolean,
}

-- Queued audio (by canonical asset path). The client drains the queue
-- each frame and plays the sfx.
export type AudioApi = {
	play: (path: string) -> (),
}

-- Scene transitions + the overlay stack. `goto` swaps the active scene (the
-- host honors the last `goto` of the frame); `push` overlays a scene on top of
-- the current one and `pop` retracts the top overlay (drained as the
-- PushScene / PopScene host commands).
export type SceneApi = {
	["goto"]: (name: string) -> (),
	push: (name: string) -> (),
	pop: () -> (),
}

-- Lifecycle: join/host a match at `addr` ("ip:port"). Drained by the
-- client as a HostCommand::Connect.
export type NetApi = {
	connect: (addr: string) -> (),
}

-- Lifecycle: begin the retail exit sequence. Drained as HostCommand::Exit.
export type AppApi = {
	exit: () -> (),
}

export type Bulanci = {
	-- The `bulanci.*` API contract version (bumped on breaking changes).
	API_VERSION: number,
	-- The implicit root node; pass as `parent` to attach at the top level.
	ROOT: Node,

	-- Resolve a VFS path (e.g. "menu/backgrounds/bg_menu") to its content
	-- handle as an integer. Rarely needed now that primitives take paths.
	handle: (name: string) -> number,

	-- ---- node construction (returns the new node id) ----
	group: (parent: Node) -> Node,
	-- A textured sprite by canonical asset path; `frame` indexes into an
	-- atlas (0 for a plain image).
	sprite: (parent: Node, path: string, frame: number?) -> Node,
	-- Bitmap text. The font is picked by `size` exactly like draw_t.
	-- `spacing` is extra px between glyphs; `anchor` is "baseline"|"top".
	text: (parent: Node, text: string, size: number?, spacing: number?, anchor: TextAnchor?) -> Node,
	quad: (parent: Node, w: number, h: number, color: number) -> Node,
	input_region: (parent: Node, w: number, h: number) -> Node,

	-- ---- node mutation ----
	set_pos: (id: Node, x: number, y: number) -> (),
	set_scale: (id: Node, sx: number, sy: number) -> (),
	set_rot: (id: Node, rot: number) -> (),
	set_visible: (id: Node, vis: boolean) -> (),
	set_text: (id: Node, text: string) -> (),
	-- Swap a sprite's texture (by path) and optionally its frame.
	set_sprite: (id: Node, path: string, frame: number?) -> (),
	-- Swap a sprite's frame only.
	set_frame: (id: Node, frame: number) -> (),
	set_tint: (id: Node, rgba: number) -> (),
	set_state_mask: (id: Node, mask: number) -> (),
	set_name: (id: Node, name: string) -> (),
	reparent: (id: Node, parent: Node) -> (),
	remove: (id: Node) -> (),

	-- Allocate (or look up) the bit for a named conditional state, for use
	-- with `set_state_mask`.
	state_bit: (name: string) -> number,

	-- ---- declarative controls (SceneDoc code-behind) ----
	-- Resolve an `x:Name`d control from the scene document to its node id
	-- (nil if the scene was built imperatively or the name is unknown).
	-- This is how an attached script reaches the controls authored in the
	-- visual designer / `.scene.json`.
	node: (name: string) -> Node?,
	-- Publish a value a declarative binding copies into a control property
	-- each frame (the doc's `bindings` map a control prop -> a binding key;
	-- the script just sets the key). Accepts number | boolean | string.
	bind: (key: string, value: number | boolean | string) -> (),

	-- ---- deterministic RNG (SplitMix64, seedable) ----
	srand: (seed: number) -> (),
	-- Inclusive integer in [lo, hi].
	rand: (lo: number, hi: number) -> number,

	-- ---- behaviors: phase subscriptions + timers ----
	subscribe: (phase: Phase, fn: () -> ()) -> (),
	-- One-shot timer; fires `ms` after registration. Returns its id.
	after: (ms: number, fn: () -> ()) -> number,
	-- Recurring timer; fires every `ms` (one call per fired step). Returns
	-- its id. For HOT, fixed-cadence work prefer `on_fixed`.
	every: (ms: number, fn: () -> ()) -> number,
	-- Cancel a timer returned by `after`/`every`.
	cancel: (id: number) -> (),
	-- Bind a fixed-timestep tick. The host coalesces all due steps for the
	-- frame into a SINGLE call `fn(steps)` (one boundary crossing/frame, so
	-- the body runs `for _ = 1, steps do ... end`). Drift-free, anchored on
	-- the first tick (no cold-start catch-up burst), clamped on a huge gap.
	-- This is the preferred primitive for high-frequency tick actions
	-- (cursor ring, smoothing, integrators). Lives for the scene's lifetime.
	on_fixed: (ms: number, fn: (steps: number) -> ()) -> (),

	-- Register a pointer handler on an `input_region` node. The handler
	-- receives the node id.
	on_input: (id: Node, event: InputEvent, fn: (id: Node) -> ()) -> (),

	-- ---- queries ----
	-- Current pointer position in 800x600 logical space.
	pointer: () -> (number, number),
	-- Monotonic time + frame delta, both in ms.
	now: () -> number,
	dt: () -> number,
	-- Is the pointer currently over this input_region?
	hovered: (id: Node) -> boolean,

	-- ---- keyboard ----
	-- Fire `fn(name)` when a named key transitions to down this frame.
	on_key: (name: string, fn: (name: string) -> ()) -> (),
	-- Fire `fn(ch)` for each character typed this frame.
	on_char: (fn: (ch: string) -> ()) -> (),
	key_pressed: (name: string) -> boolean,
	key_down: (name: string) -> boolean,

	-- ---- client services ----
	-- Pixel width of `text` in the font picked by `size` with `spacing`.
	measure: (text: string, size: number, spacing: number?) -> number,
	-- Read a VFS text asset (poems). nil if absent.
	read_text: (path: string) -> string?,
	-- (line_height, baseline) of the bitmap font picked by `size`.
	font_metrics: (size: number) -> (number?, number?),
	-- Pixel size of an image asset, or (nil, nil) if not warmed.
	image_size: (path: string) -> (number?, number?),
	-- Atlas metadata for a sprite-strip asset, or nil.
	atlas: (path: string) -> AtlasInfo?,

	-- ---- sub-tables ----
	state: StateApi,
	audio: AudioApi,
	scene: SceneApi,
	net: NetApi,
	app: AppApi,

	-- Print a line through the host's scene log.
	log: (msg: string) -> (),
}

-- Never executed at runtime; the cast just gives the module its type so
-- `local b = require("@bulanci")` resolves to `Bulanci`.
return (nil :: any) :: Bulanci
