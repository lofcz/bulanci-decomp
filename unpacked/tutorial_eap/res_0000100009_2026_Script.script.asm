
fn export#0 @ 0x0064  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0065  SetGlobalVar(1, IntConst(1000))
    @0x006c  SetGlobalVar(2, StrConst("3838938E-851E-4B0F-AF0B-1CB280234F6B"))
    @0x00b9  SetGlobalVar(0, StrConst("Tutorial 1"))
    @0x00d2  Return(IntConst(0))

fn export#1 @ 0x00d8  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x00d9  SetGlobalVar(3, IntConst(0))
    @0x00e0  IfEqual(IsNet(), IntConst(0), 251)
    @0x00eb  SetGlobalVar(3, StrmCreateMem(IntConst(4096), IntConst(4096)))
    @0x00f8  SetCommStrm(GetGlobalVar(3))
    @0x00fb  LoadPreface(100001)
    @0x0100  SetMusic(100003, 0)
    @0x0109  SetInsertMode(IntConst(0))
    @0x010f  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x011f  InsertView(CreateObstacle(IntConst(737), IntConst(366), IntConst(787), IntConst(383)))
    @0x0135  InsertView(CreateObstacle(IntConst(425), IntConst(398), IntConst(474), IntConst(415)))
    @0x014b  InsertView(CreateObstacle(IntConst(327), IntConst(420), IntConst(371), IntConst(435)))
    @0x0161  InsertView(CreateObstacle(IntConst(321), IntConst(137), IntConst(367), IntConst(150)))
    @0x0177  InsertView(CreateObstacle(IntConst(247), IntConst(133), IntConst(294), IntConst(148)))
    @0x018d  InsertView(CreateObstacle(IntConst(15), IntConst(136), IntConst(61), IntConst(152)))
    @0x01a3  InsertView(CreateObstacle(IntConst(127), IntConst(307), IntConst(175), IntConst(333)))
    @0x01b9  InsertView(CreateObstacle(IntConst(655), IntConst(302), IntConst(730), IntConst(359)))
    @0x01cf  InsertView(CreateObstacle(IntConst(114), IntConst(276), IntConst(187), IntConst(324)))
    @0x01e5  InsertView(CreateObstacle(IntConst(0), IntConst(92), IntConst(77), IntConst(143)))
    @0x01fb  InsertView(CreateObstacle(IntConst(724), IntConst(325), IntConst(800), IntConst(373)))
    @0x0211  InsertView(CreateObstacle(IntConst(312), IntConst(383), IntConst(385), IntConst(428)))
    @0x0227  InsertView(CreateObstacle(IntConst(413), IntConst(363), IntConst(489), IntConst(407)))
    @0x023d  InsertView(CreateObstacle(IntConst(235), IntConst(90), IntConst(383), IntConst(140)))
    @0x0253  InsertView(CreateObstacle(IntConst(402), IntConst(70), IntConst(518), IntConst(162)))
    @0x0269  InsertView(CreateObstacle(IntConst(516), IntConst(310), IntConst(632), IntConst(399)))
    @0x027f  InsertView(CreateObstacle(IntConst(197), IntConst(424), IntConst(310), IntConst(515)))
    @0x0295  InsertView(CreateImage(IntConst(360), IntConst(230), 100006))
    @0x02a5  InsertView(CreateImage(IntConst(313), IntConst(449), 100006))
    @0x02b5  SetInsertMode(IntConst(2))
    @0x02bb  InsertView(CreateImage(IntConst(671), IntConst(0), 100008))
    @0x02cb  InsertView(CreateImage(IntConst(0), IntConst(303), 100007))
    @0x02db  SetInsertMode(IntConst(1))
    @0x02e1  InsertView(SetOrderAxis(CreateImage(IntConst(112), IntConst(248), 100005), IntConst(-57)))
    @0x02f7  InsertView(SetOrderAxis(CreateImage(IntConst(723), IntConst(299), 100005), IntConst(-62)))
    @0x030d  InsertView(SetOrderAxis(CreateImage(IntConst(652), IntConst(273), 100005), IntConst(-59)))
    @0x0323  InsertView(SetOrderAxis(CreateImage(IntConst(412), IntConst(331), 100005), IntConst(-55)))
    @0x0339  InsertView(SetOrderAxis(CreateImage(IntConst(311), IntConst(350), 100005), IntConst(-54)))
    @0x034f  InsertView(SetOrderAxis(CreateImage(IntConst(307), IntConst(65), 100005), IntConst(-61)))
    @0x0365  InsertView(SetOrderAxis(CreateImage(IntConst(232), IntConst(63), 100005), IntConst(-59)))
    @0x037b  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(67), 100005), IntConst(-63)))
    @0x0391  InsertView(SetOrderAxis(CreateImage(IntConst(194), IntConst(395), 100004), IntConst(-86)))
    @0x03a7  InsertView(SetOrderAxis(CreateImage(IntConst(514), IntConst(282), 100004), IntConst(-87)))
    @0x03bd  InsertView(SetOrderAxis(CreateImage(IntConst(400), IntConst(44), 100004), IntConst(-90)))
    @0x03d3  SetInsertMode(IntConst(0))
    @0x03d9  IfEqual(IsServer(), IntConst(0), 1058)
    @0x03e4  DefineTraceArea(IntConst(0), IntConst(312), IntConst(444), IntConst(392), IntConst(514), IntConst(14))
    @0x0403  DefineTraceArea(IntConst(1), IntConst(359), IntConst(225), IntConst(439), IntConst(295), IntConst(14))
    @0x0422  InsertBulanci()
    @0x0423  InsertOpponent(IntConst(7), IntConst(3), IntConst(120), IntConst(5))
    @0x0438  InsertOpponent(IntConst(7), IntConst(2), IntConst(120), IntConst(4))
    @0x044d  InsertOpponent(IntConst(7), IntConst(2), IntConst(120), IntConst(1))
    @0x0462  Return(IntConst(0))

fn export#2 @ 0x0468  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x0469  IfEqual(GetGlobalVar(3), IntConst(0), 1146)
    @0x0475  SetGlobalVar(3, StrmDestroy(GetGlobalVar(3)))
    @0x047a  Return(IntConst(0))

fn export#3 @ 0x0480  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0481  Return(IntConst(0))

fn export#4 @ 0x0487  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x0488  Return(IntConst(0))

fn export#5 @ 0x048e  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x048f  Return(IntConst(0))

fn export#6 @ 0x0495  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x0496  Return(IntConst(0))
    ; (236 byte(s) of unreachable tail/inline helper)

fn export#7 @ 0x0588  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x0589  Switch(GetLocalVar(0); 0=>0x59d, 1=>0x5b4)
    @0x059d  Call(fn@0x0, GetLocalVar(1), IntConst(373), IntConst(241))
    @0x05af  Goto(1473)
    @0x05b4  Call(fn@0x49c, GetLocalVar(1))
    @0x05bc  Goto(1473)
    @0x05c1  Return(IntConst(0))

fn export#8 @ 0x05c7  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x05c8  Switch(GetLocalVar(0); 0=>0x5dc, 1=>0x5e1)
    @0x05dc  Goto(1510)
    @0x05e1  Goto(1510)
    @0x05e6  Return(IntConst(0))

fn export#9 @ 0x05ec  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=4
    @0x05ed  SetLocalVar(0, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x05f7  IfNotEqual(GetLocalVar(0), IntConst(1), 1581)
    @0x0603  SetLocalVar(3, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x060d  SetLocalVar(1, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x0617  SetLocalVar(2, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x0621  TeleportPlayerTo(GetLocalVar(3), GetLocalVar(1), GetLocalVar(2), IntConst(1))
    @0x062d  Return(IntConst(0))

fn export#10 @ 0x0633  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0634  Return(IntConst(0))

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

fn @ 0x049c  ; helper (call-target)
  ; varCount=2
    @0x049d  SetLocalVar(0, Rand(IntConst(0), IntConst(3)))
    @0x04aa  SetLocalVar(1, GetLocalVar(0))
    @0x04ae  Switch(GetLocalVar(1); 0=>0x4d2, 1=>0x4f3, 2=>0x514, 3=>0x535)
    @0x04d2  IfEqual(Call(fn@0x0, GetLocalVar(2), IntConst(9), IntConst(466)), IntConst(1), 1410)
    @0x04ee  Goto(1366)
    @0x04f3  IfEqual(Call(fn@0x0, GetLocalVar(2), IntConst(739), IntConst(466)), IntConst(1), 1410)
    @0x050f  Goto(1366)
    @0x0514  IfEqual(Call(fn@0x0, GetLocalVar(2), IntConst(9), IntConst(11)), IntConst(1), 1410)
    @0x0530  Goto(1366)
    @0x0535  IfEqual(Call(fn@0x0, GetLocalVar(2), IntConst(739), IntConst(11)), IntConst(1), 1410)
    @0x0551  Goto(1366)
    @0x0556  SetLocalVar(1, Add(GetLocalVar(1), IntConst(1)))
    @0x0560  IfNotEqual(GetLocalVar(1), IntConst(4), 1395)
    @0x056c  SetLocalVar(1, IntConst(0))
    @0x0573  IfNotEqual(GetLocalVar(1), GetLocalVar(0), 1198)
    @0x057c  Return(IntConst(0))
    @0x0582  Return(IntConst(1))
