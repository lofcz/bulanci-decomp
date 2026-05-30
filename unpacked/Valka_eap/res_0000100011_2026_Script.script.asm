
fn export#0 @ 0x0064  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0065  SetGlobalVar(1, IntConst(1000))
    @0x006c  SetGlobalVar(2, StrConst("C05A377E-4B25-4392-8703-80D4D36C1094"))
    @0x00b9  SetGlobalVar(0, StrConst("Válka"))
    @0x00c8  Return(IntConst(0))

fn export#1 @ 0x00ce  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x00cf  SetGlobalVar(3, IntConst(0))
    @0x00d6  IfEqual(IsNet(), IntConst(0), 241)
    @0x00e1  SetGlobalVar(3, StrmCreateMem(IntConst(4096), IntConst(4096)))
    @0x00ee  SetCommStrm(GetGlobalVar(3))
    @0x00f1  LoadPreface(100001)
    @0x00f6  SetMusic(100003, 0)
    @0x00ff  SetInsertMode(IntConst(0))
    @0x0105  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x0115  InsertView(CreateImage(IntConst(18), IntConst(446), 100009))
    @0x0125  InsertView(CreateImage(IntConst(0), IntConst(10), 100009))
    @0x0135  InsertView(CreateImage(IntConst(734), IntConst(466), 100009))
    @0x0145  InsertView(CreateObstacle(IntConst(0), IntConst(252), IntConst(14), IntConst(265)))
    @0x015b  InsertView(CreateObstacle(IntConst(359), IntConst(8), IntConst(373), IntConst(26)))
    @0x0171  InsertView(CreateObstacle(IntConst(353), IntConst(10), IntConst(383), IntConst(34)))
    @0x0187  InsertView(CreateObstacle(IntConst(269), IntConst(105), IntConst(294), IntConst(117)))
    @0x019d  InsertView(CreateObstacle(IntConst(266), IntConst(79), IntConst(278), IntConst(86)))
    @0x01b3  InsertView(CreateObstacle(IntConst(187), IntConst(118), IntConst(203), IntConst(138)))
    @0x01c9  InsertView(CreateObstacle(IntConst(163), IntConst(136), IntConst(176), IntConst(147)))
    @0x01df  InsertView(CreateObstacle(IntConst(87), IntConst(182), IntConst(107), IntConst(200)))
    @0x01f5  InsertView(CreateObstacle(IntConst(7), IntConst(248), IntConst(44), IntConst(276)))
    @0x020b  InsertView(CreateObstacle(IntConst(17), IntConst(230), IntConst(77), IntConst(258)))
    @0x0221  InsertView(CreateObstacle(IntConst(43), IntConst(212), IntConst(97), IntConst(240)))
    @0x0237  InsertView(CreateObstacle(IntConst(64), IntConst(190), IntConst(118), IntConst(223)))
    @0x024d  InsertView(CreateObstacle(IntConst(102), IntConst(160), IntConst(151), IntConst(207)))
    @0x0263  InsertView(CreateObstacle(IntConst(143), IntConst(142), IntConst(186), IntConst(184)))
    @0x0279  InsertView(CreateObstacle(IntConst(172), IntConst(128), IntConst(217), IntConst(165)))
    @0x028f  InsertView(CreateObstacle(IntConst(196), IntConst(108), IntConst(243), IntConst(152)))
    @0x02a5  InsertView(CreateObstacle(IntConst(236), IntConst(83), IntConst(279), IntConst(127)))
    @0x02bb  InsertView(CreateObstacle(IntConst(272), IntConst(67), IntConst(323), IntConst(103)))
    @0x02d1  InsertView(CreateObstacle(IntConst(302), IntConst(37), IntConst(356), IntConst(85)))
    @0x02e7  InsertView(CreateObstacle(IntConst(344), IntConst(25), IntConst(379), IntConst(63)))
    @0x02fd  InsertView(CreateObstacle(IntConst(12), IntConst(367), IntConst(90), IntConst(406)))
    @0x0313  InsertView(CreateObstacle(IntConst(110), IntConst(366), IntConst(168), IntConst(418)))
    @0x0329  InsertView(CreateObstacle(IntConst(181), IntConst(369), IntConst(426), IntConst(412)))
    @0x033f  SetInsertMode(IntConst(2))
    @0x0345  SetInsertMode(IntConst(1))
    @0x034b  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(303), 100004), IntConst(0)))
    @0x0361  InsertView(SetOrderAxis(CreateImage(IntConst(671), IntConst(0), 100005), IntConst(0)))
    @0x0377  InsertView(SetOrderAxis(CreateImage(IntConst(169), IntConst(360), 100010), IntConst(0)))
    @0x038d  InsertView(SetOrderAxis(CreateImage(IntConst(99), IntConst(355), 100008), IntConst(0)))
    @0x03a3  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(354), 100006), IntConst(0)))
    @0x03b9  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(0), 100007), IntConst(0)))
    @0x03cf  SetInsertMode(IntConst(0))
    @0x03d5  IfEqual(IsServer(), IntConst(0), 1085)
    @0x03e0  DefineTraceArea(IntConst(0), IntConst(9), IntConst(437), IntConst(89), IntConst(507), IntConst(14))
    @0x03ff  DefineTraceArea(IntConst(1), IntConst(-5), IntConst(-5), IntConst(75), IntConst(65), IntConst(14))
    @0x041e  DefineTraceArea(IntConst(2), IntConst(725), IntConst(450), IntConst(805), IntConst(520), IntConst(14))
    @0x043d  InsertBulanci()
    @0x043e  InsertVampires()
    @0x043f  InsertOpponent(IntConst(7), IntConst(1), IntConst(50), IntConst(5))
    @0x0454  InsertOpponent(IntConst(7), IntConst(1), IntConst(50), IntConst(5))
    @0x0469  Return(IntConst(0))

fn export#2 @ 0x046f  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x0470  IfEqual(GetGlobalVar(3), IntConst(0), 1153)
    @0x047c  SetGlobalVar(3, StrmDestroy(GetGlobalVar(3)))
    @0x0481  Return(IntConst(0))

fn export#3 @ 0x0487  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0488  Return(IntConst(0))

fn export#4 @ 0x048e  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x048f  Return(IntConst(0))

fn export#5 @ 0x0495  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x0496  Return(IntConst(0))

fn export#6 @ 0x049c  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x049d  Return(IntConst(0))
    ; (154 byte(s) of unreachable tail/inline helper)

fn export#7 @ 0x053d  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x053e  Switch(GetLocalVar(0); 0=>0x55a, 1=>0x567, 2=>0x57e)
    @0x055a  Call(fn@0x4a3, GetLocalVar(1))
    @0x0562  Goto(1429)
    @0x0567  Call(fn@0x0, GetLocalVar(1), IntConst(739), IntConst(466))
    @0x0579  Goto(1429)
    @0x057e  Call(fn@0x0, GetLocalVar(1), IntConst(9), IntConst(11))
    @0x0590  Goto(1429)
    @0x0595  Return(IntConst(0))

fn export#8 @ 0x059b  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x059c  Switch(GetLocalVar(0); 0=>0x5b8, 1=>0x5bd, 2=>0x5c2)
    @0x05b8  Goto(1479)
    @0x05bd  Goto(1479)
    @0x05c2  Goto(1479)
    @0x05c7  Return(IntConst(0))

fn export#9 @ 0x05cd  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=4
    @0x05ce  SetLocalVar(0, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x05d8  IfNotEqual(GetLocalVar(0), IntConst(1), 1550)
    @0x05e4  SetLocalVar(3, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x05ee  SetLocalVar(1, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x05f8  SetLocalVar(2, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x0602  TeleportPlayerTo(GetLocalVar(3), GetLocalVar(1), GetLocalVar(2), IntConst(1))
    @0x060e  Return(IntConst(0))

fn export#10 @ 0x0614  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0615  Return(IntConst(0))

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

fn @ 0x04a3  ; helper (call-target)
  ; varCount=2
    @0x04a4  SetLocalVar(0, Rand(IntConst(0), IntConst(1)))
    @0x04b1  SetLocalVar(1, GetLocalVar(0))
    @0x04b5  Switch(GetLocalVar(1); 0=>0x4c9, 1=>0x4ea)
    @0x04c9  IfEqual(Call(fn@0x0, GetLocalVar(2), IntConst(9), IntConst(11)), IntConst(1), 1335)
    @0x04e5  Goto(1291)
    @0x04ea  IfEqual(Call(fn@0x0, GetLocalVar(2), IntConst(739), IntConst(466)), IntConst(1), 1335)
    @0x0506  Goto(1291)
    @0x050b  SetLocalVar(1, Add(GetLocalVar(1), IntConst(1)))
    @0x0515  IfNotEqual(GetLocalVar(1), IntConst(2), 1320)
    @0x0521  SetLocalVar(1, IntConst(0))
    @0x0528  IfNotEqual(GetLocalVar(1), GetLocalVar(0), 1205)
    @0x0531  Return(IntConst(0))
    @0x0537  Return(IntConst(1))
