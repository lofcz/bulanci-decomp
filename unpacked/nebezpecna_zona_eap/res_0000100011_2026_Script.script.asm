
fn export#0 @ 0x0064  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0065  SetGlobalVar(1, IntConst(1000))
    @0x006c  SetGlobalVar(2, StrConst("1799E4FE-B5C4-4881-8A1C-4B70E022DCE7"))
    @0x00b9  SetGlobalVar(0, StrConst("Nebezpečná zóna"))
    @0x00dc  Return(IntConst(0))

fn export#1 @ 0x00e2  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x00e3  SetGlobalVar(3, IntConst(0))
    @0x00ea  IfEqual(IsNet(), IntConst(0), 261)
    @0x00f5  SetGlobalVar(3, StrmCreateMem(IntConst(4096), IntConst(4096)))
    @0x0102  SetCommStrm(GetGlobalVar(3))
    @0x0105  LoadPreface(100001)
    @0x010a  SetMusic(100003, 0)
    @0x0113  SetInsertMode(IntConst(0))
    @0x0119  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x0129  InsertView(CreateImage(IntConst(0), IntConst(0), 100010))
    @0x0139  InsertView(CreateImage(IntConst(707), IntConst(409), 100010))
    @0x0149  InsertView(CreateObstacle(IntConst(618), IntConst(85), IntConst(682), IntConst(112)))
    @0x015f  InsertView(CreateObstacle(IntConst(544), IntConst(215), IntConst(611), IntConst(242)))
    @0x0175  InsertView(CreateObstacle(IntConst(658), IntConst(350), IntConst(724), IntConst(378)))
    @0x018b  InsertView(CreateObstacle(IntConst(460), IntConst(423), IntConst(504), IntConst(461)))
    @0x01a1  InsertView(CreateObstacle(IntConst(197), IntConst(474), IntConst(231), IntConst(504)))
    @0x01b7  InsertView(CreateObstacle(IntConst(323), IntConst(344), IntConst(363), IntConst(374)))
    @0x01cd  InsertView(CreateObstacle(IntConst(261), IntConst(332), IntConst(295), IntConst(357)))
    @0x01e3  InsertView(CreateObstacle(IntConst(295), IntConst(315), IntConst(330), IntConst(347)))
    @0x01f9  InsertView(CreateObstacle(IntConst(44), IntConst(291), IntConst(80), IntConst(318)))
    @0x020f  InsertView(CreateObstacle(IntConst(76), IntConst(275), IntConst(112), IntConst(302)))
    @0x0225  InsertView(CreateObstacle(IntConst(114), IntConst(150), IntConst(147), IntConst(167)))
    @0x023b  InsertView(CreateObstacle(IntConst(112), IntConst(115), IntConst(180), IntConst(152)))
    @0x0251  SetInsertMode(IntConst(2))
    @0x0257  SetInsertMode(IntConst(1))
    @0x025d  InsertView(SetOrderAxis(CreateImage(IntConst(312), IntConst(299), 100005), IntConst(0)))
    @0x0273  InsertView(SetOrderAxis(CreateImage(IntConst(25), IntConst(238), 100004), IntConst(0)))
    @0x0289  InsertView(SetOrderAxis(CreateImage(IntConst(179), IntConst(425), 100005), IntConst(0)))
    @0x029f  InsertView(SetOrderAxis(CreateImage(IntConst(617), IntConst(65), 100007), IntConst(0)))
    @0x02b5  InsertView(SetOrderAxis(CreateImage(IntConst(544), IntConst(196), 100008), IntConst(0)))
    @0x02cb  InsertView(SetOrderAxis(CreateImage(IntConst(657), IntConst(330), 100009), IntConst(0)))
    @0x02e1  InsertView(SetOrderAxis(CreateImage(IntConst(241), IntConst(278), 100004), IntConst(0)))
    @0x02f7  InsertView(SetOrderAxis(CreateImage(IntConst(100), IntConst(92), 100004), IntConst(0)))
    @0x030d  InsertView(SetOrderAxis(CreateImage(IntConst(463), IntConst(274), 100006), IntConst(0)))
    @0x0323  InsertView(SetOrderAxis(CreateImage(IntConst(109), IntConst(65), 100005), IntConst(0)))
    @0x0339  SetInsertMode(IntConst(0))
    @0x033f  IfEqual(IsServer(), IntConst(0), 904)
    @0x034a  DefineTraceArea(IntConst(0), IntConst(723), IntConst(446), IntConst(803), IntConst(516), IntConst(14))
    @0x0369  DefineTraceArea(IntConst(1), IntConst(11), IntConst(35), IntConst(91), IntConst(105), IntConst(14))
    @0x0388  InsertBulanci()
    @0x0389  InsertOpponent(IntConst(7), IntConst(1), IntConst(100), IntConst(5))
    @0x039e  InsertOpponent(IntConst(7), IntConst(1), IntConst(100), IntConst(1))
    @0x03b3  InsertOpponent(IntConst(7), IntConst(1), IntConst(100), IntConst(4))
    @0x03c8  Return(IntConst(0))

fn export#2 @ 0x03ce  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x03cf  IfEqual(GetGlobalVar(3), IntConst(0), 992)
    @0x03db  SetGlobalVar(3, StrmDestroy(GetGlobalVar(3)))
    @0x03e0  Return(IntConst(0))

fn export#3 @ 0x03e6  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x03e7  Return(IntConst(0))

fn export#4 @ 0x03ed  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x03ee  Return(IntConst(0))

fn export#5 @ 0x03f4  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x03f5  Return(IntConst(0))

fn export#6 @ 0x03fb  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x03fc  Return(IntConst(0))

fn export#7 @ 0x0402  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x0403  Switch(GetLocalVar(0); 0=>0x417, 1=>0x42e)
    @0x0417  Call(fn@0x0, GetLocalVar(1), IntConst(25), IntConst(51))
    @0x0429  Goto(1093)
    @0x042e  Call(fn@0x0, GetLocalVar(1), IntConst(737), IntConst(462))
    @0x0440  Goto(1093)
    @0x0445  Return(IntConst(0))

fn export#8 @ 0x044b  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x044c  Switch(GetLocalVar(0); 0=>0x460, 1=>0x465)
    @0x0460  Goto(1130)
    @0x0465  Goto(1130)
    @0x046a  Return(IntConst(0))

fn export#9 @ 0x0470  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=4
    @0x0471  SetLocalVar(0, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x047b  IfNotEqual(GetLocalVar(0), IntConst(1), 1201)
    @0x0487  SetLocalVar(3, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x0491  SetLocalVar(1, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x049b  SetLocalVar(2, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x04a5  TeleportPlayerTo(GetLocalVar(3), GetLocalVar(1), GetLocalVar(2), IntConst(1))
    @0x04b1  Return(IntConst(0))

fn export#10 @ 0x04b7  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x04b8  Return(IntConst(0))

fn @ 0x0000  ; helper (call-target)
  ; varCount=0
    @0x0001  IfEqual(TeleportPlayerTo(GetLocalVar(0), GetLocalVar(1), GetLocalVar(2), IntConst(0)), IntConst(0), 94)
    @0x0017  IfEqual(IsNet(), IntConst(0), 88)
    @0x0022  StrmSetSize(GetGlobalVar(3), IntConst(0))
    @0x002a  StrmWrite(GetGlobalVar(3), IntConst(1), IntConst(1))
    @0x0037  StrmWrite(GetGlobalVar(3), GetLocalVar(0), IntConst(1))
    @0x0041  StrmWrite(GetGlobalVar(3), GetLocalVar(1), IntConst(4))
    @0x004b  StrmWrite(GetGlobalVar(3), GetLocalVar(2), IntConst(4))
    @0x0055  StrmSend(GetGlobalVar(3))
    @0x0058  Return(IntConst(1))
    @0x005e  Return(IntConst(0))
