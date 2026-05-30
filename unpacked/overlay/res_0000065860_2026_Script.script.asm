
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  SetGlobalVar(1, IntConst(6))
    @0x0008  SetGlobalVar(2, StrConst("987EF758-1BD5-4954-B543-767F158B17AF"))
    @0x0055  SetGlobalVar(0, StrConst("Exitus"))
    @0x0066  Return(IntConst(0))

fn export#1 @ 0x006c  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x006d  SetGlobalVar(3, IntConst(0))
    @0x0074  IfEqual(IsNet(), IntConst(0), 143)
    @0x007f  SetGlobalVar(3, StrmCreateMem(IntConst(4096), IntConst(4096)))
    @0x008c  SetCommStrm(GetGlobalVar(3))
    @0x008f  LoadPreface(65589)
    @0x0094  SetMusic(65862, 0)
    @0x009d  SetInsertMode(IntConst(0))
    @0x00a3  InsertView(CreateImage(IntConst(0), IntConst(0), 65638))
    @0x00b3  SetInsertMode(IntConst(2))
    @0x00b9  InsertView(EnableFireThrough(CreateObstacle(IntConst(0), IntConst(140), IntConst(140), IntConst(175)), IntConst(1)))
    @0x00d5  InsertView(EnableFireThrough(CreateObstacle(IntConst(140), IntConst(110), IntConst(180), IntConst(135)), IntConst(1)))
    @0x00f1  InsertView(EnableFireThrough(CreateObstacle(IntConst(185), IntConst(0), IntConst(225), IntConst(50)), IntConst(1)))
    @0x010d  InsertView(EnableFireThrough(CreateObstacle(IntConst(225), IntConst(0), IntConst(259), IntConst(17)), IntConst(1)))
    @0x0129  InsertView(EnableFireThrough(CreateObstacle(IntConst(123), IntConst(129), IntConst(157), IntConst(150)), IntConst(1)))
    @0x0145  InsertView(EnableFireThrough(CreateObstacle(IntConst(0), IntConst(175), IntConst(17), IntConst(200)), IntConst(1)))
    @0x0161  InsertView(CreateObstacle(IntConst(343), IntConst(421), IntConst(386), IntConst(456)))
    @0x0177  InsertView(CreateObstacle(IntConst(370), IntConst(399), IntConst(410), IntConst(430)))
    @0x018d  InsertView(CreateObstacle(IntConst(385), IntConst(368), IntConst(441), IntConst(409)))
    @0x01a3  InsertView(CreateObstacle(IntConst(427), IntConst(349), IntConst(541), IntConst(387)))
    @0x01b9  InsertView(CreateObstacle(IntConst(541), IntConst(373), IntConst(583), IntConst(410)))
    @0x01cf  Call(fn@0x239, )
    @0x01d5  Call(fn@0x296, )
    @0x01db  SetInsertMode(IntConst(1))
    @0x01e1  InsertBulanci()
    @0x01e2  InsertVampires()
    @0x01e3  Return(IntConst(0))

fn export#2 @ 0x01e9  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x01ea  IfEqual(GetGlobalVar(3), IntConst(0), 507)
    @0x01f6  SetGlobalVar(3, StrmDestroy(GetGlobalVar(3)))
    @0x01fb  Return(IntConst(0))

fn export#3 @ 0x0208  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0209  Return(IntConst(0))

fn export#4 @ 0x020f  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x0210  Return(IntConst(0))

fn export#5 @ 0x0216  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x0217  Return(IntConst(0))

fn export#6 @ 0x021d  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x021e  Return(IntConst(0))

fn export#7 @ 0x0224  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x0225  Return(IntConst(0))

fn export#8 @ 0x022b  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x022c  Return(IntConst(0))

fn export#9 @ 0x0232  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=0
    @0x0233  Return(IntConst(0))
    ; (639 byte(s) of unreachable tail/inline helper)

fn export#10 @ 0x0201  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0202  Return(IntConst(0))

fn @ 0x0239  ; helper (call-target)
  ; varCount=0
    @0x023a  IfEqual(Rand(IntConst(0), IntConst(5)), IntConst(0), 613)
    @0x024f  SetInsertMode(IntConst(2))
    @0x0255  InsertView(CreateImage(IntConst(625), IntConst(0), 65583))  ; res 0x1002f exitus_scenery_pine_branch_top
    @0x0265  IfEqual(Rand(IntConst(0), IntConst(5)), IntConst(0), 656)
    @0x027a  SetInsertMode(IntConst(2))
    @0x0280  InsertView(CreateImage(IntConst(678), IntConst(250), 65584))  ; res 0x10030 exitus_scenery_conifer_bush
    @0x0290  Return(IntConst(0))

fn @ 0x0296  ; helper (call-target)
  ; varCount=0
    @0x0297  IfEqual(Rand(IntConst(0), IntConst(5)), IntConst(6), 784)
    @0x02ac  SetInsertMode(IntConst(0))
    @0x02b2  InsertView(CreateImage(IntConst(404), IntConst(56), 65574))  ; res 0x10026 exitus_scenery_grave_stone_cross_fg
    @0x02c2  SetInsertMode(IntConst(1))
    @0x02c8  InsertView(SetOrderAxis(CreateImage(IntConst(404), IntConst(56), 65573), IntConst(-25)))  ; res 0x10025 exitus_scenery_grave_stone_cross_bg
    @0x02de  SetInsertMode(IntConst(2))
    @0x02e4  InsertView(CreateObstacle(IntConst(413), IntConst(93), IntConst(549), IntConst(138)))
    @0x02fa  InsertView(CreateObstacle(IntConst(428), IntConst(134), IntConst(486), IntConst(167)))
    @0x0310  IfEqual(Rand(IntConst(0), IntConst(5)), IntConst(6), 883)
    @0x0325  SetInsertMode(IntConst(0))
    @0x032b  InsertView(CreateImage(IntConst(664), IntConst(113), 65580))  ; res 0x1002c exitus_scenery_grave_cross_tilted_fg
    @0x033b  SetInsertMode(IntConst(1))
    @0x0341  InsertView(SetOrderAxis(CreateImage(IntConst(664), IntConst(113), 65575), IntConst(-20)))  ; res 0x10027 exitus_scenery_grave_cross_tilted_bg
    @0x0357  SetInsertMode(IntConst(2))
    @0x035d  InsertView(CreateObstacle(IntConst(667), IntConst(259), IntConst(713), IntConst(297)))
    @0x0373  IfEqual(Rand(IntConst(0), IntConst(5)), IntConst(6), 1004)
    @0x0388  SetInsertMode(IntConst(0))
    @0x038e  InsertView(CreateImage(IntConst(82), IntConst(237), 65577))  ; res 0x10029 exitus_scenery_grave_double_fg
    @0x039e  SetInsertMode(IntConst(1))
    @0x03a4  InsertView(SetOrderAxis(CreateImage(IntConst(82), IntConst(237), 65576), IntConst(-20)))  ; res 0x10028 exitus_scenery_grave_double_bg
    @0x03ba  SetInsertMode(IntConst(2))
    @0x03c0  InsertView(CreateObstacle(IntConst(130), IntConst(277), IntConst(178), IntConst(334)))
    @0x03d6  InsertView(CreateObstacle(IntConst(96), IntConst(321), IntConst(138), IntConst(359)))
    @0x03ec  IfEqual(Rand(IntConst(0), IntConst(5)), IntConst(6), 1103)
    @0x0401  SetInsertMode(IntConst(0))
    @0x0407  InsertView(CreateImage(IntConst(294), IntConst(361), 65579))  ; res 0x1002b exitus_scenery_grave_bulanek_fg
    @0x0417  SetInsertMode(IntConst(1))
    @0x041d  InsertView(SetOrderAxis(CreateImage(IntConst(294), IntConst(361), 65578), IntConst(-25)))  ; res 0x1002a exitus_scenery_grave_single_bg
    @0x0433  SetInsertMode(IntConst(2))
    @0x0439  InsertView(CreateObstacle(IntConst(311), IntConst(406), IntConst(346), IntConst(446)))
    @0x044f  IfEqual(Rand(IntConst(0), IntConst(5)), IntConst(6), 1202)
    @0x0464  SetInsertMode(IntConst(0))
    @0x046a  InsertView(CreateImage(IntConst(50), IntConst(71), 65582))  ; res 0x1002e exitus_scenery_tree_fence_fg
    @0x047a  SetInsertMode(IntConst(1))
    @0x0480  InsertView(SetOrderAxis(CreateImage(IntConst(50), IntConst(71), 65581), IntConst(-25)))  ; res 0x1002d exitus_scenery_spruce_bg
    @0x0496  SetInsertMode(IntConst(2))
    @0x049c  InsertView(CreateObstacle(IntConst(67), IntConst(114), IntConst(110), IntConst(155)))
    @0x04b2  Return(IntConst(0))
