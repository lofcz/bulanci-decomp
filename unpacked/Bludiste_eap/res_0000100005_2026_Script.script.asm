
fn export#0 @ 0x0064  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0065  SetGlobalVar(1, IntConst(1000))
    @0x006c  SetGlobalVar(2, StrConst("C4F1A65C-5057-4D19-B707-F3FF4A008B74"))
    @0x00b9  SetGlobalVar(0, StrConst("Bludiště"))
    @0x00ce  Return(IntConst(0))

fn export#1 @ 0x00d4  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x00d5  SetGlobalVar(3, IntConst(0))
    @0x00dc  IfEqual(IsNet(), IntConst(0), 247)
    @0x00e7  SetGlobalVar(3, StrmCreateMem(IntConst(4096), IntConst(4096)))
    @0x00f4  SetCommStrm(GetGlobalVar(3))
    @0x00f7  LoadPreface(100001)
    @0x00fc  SetMusic(100003, 0)
    @0x0105  SetInsertMode(IntConst(0))
    @0x010b  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x011b  InsertView(CreateObstacle(IntConst(623), IntConst(207), IntConst(657), IntConst(235)))
    @0x0131  InsertView(CreateObstacle(IntConst(52), IntConst(242), IntConst(91), IntConst(460)))
    @0x0147  InsertView(CreateObstacle(IntConst(56), IntConst(214), IntConst(88), IntConst(246)))
    @0x015d  InsertView(CreateObstacle(IntConst(52), IntConst(0), IntConst(89), IntConst(212)))
    @0x0173  InsertView(CreateObstacle(IntConst(353), IntConst(76), IntConst(385), IntConst(101)))
    @0x0189  InsertView(CreateObstacle(IntConst(148), IntConst(297), IntConst(185), IntConst(515)))
    @0x019f  InsertView(CreateObstacle(IntConst(244), IntConst(419), IntConst(554), IntConst(458)))
    @0x01b5  InsertView(CreateObstacle(IntConst(237), IntConst(265), IntConst(277), IntConst(350)))
    @0x01cb  InsertView(CreateObstacle(IntConst(331), IntConst(270), IntConst(368), IntConst(378)))
    @0x01e1  InsertView(CreateObstacle(IntConst(482), IntConst(267), IntConst(522), IntConst(378)))
    @0x01f7  InsertView(CreateObstacle(IntConst(611), IntConst(233), IntConst(651), IntConst(515)))
    @0x020d  InsertView(CreateObstacle(IntConst(709), IntConst(265), IntConst(747), IntConst(515)))
    @0x0223  InsertView(CreateObstacle(IntConst(613), IntConst(162), IntConst(800), IntConst(204)))
    @0x0239  InsertView(CreateObstacle(IntConst(461), IntConst(60), IntConst(800), IntConst(102)))
    @0x024f  InsertView(CreateObstacle(IntConst(349), IntConst(0), IntConst(396), IntConst(70)))
    @0x0265  InsertView(CreateObstacle(IntConst(238), IntConst(0), IntConst(284), IntConst(69)))
    @0x027b  InsertView(CreateObstacle(IntConst(243), IntConst(165), IntConst(555), IntConst(203)))
    @0x0291  InsertView(CreateObstacle(IntConst(143), IntConst(0), IntConst(182), IntConst(212)))
    @0x02a7  SetInsertMode(IntConst(2))
    @0x02ad  SetInsertMode(IntConst(1))
    @0x02b3  InsertView(SetOrderAxis(CreateImage(IntConst(620), IntConst(206), 100004), IntConst(0)))
    @0x02c9  SetInsertMode(IntConst(0))
    @0x02cf  IfEqual(IsServer(), IntConst(0), 1009)
    @0x02da  DefineTraceArea(IntConst(0), IntConst(177), IntConst(450), IntConst(257), IntConst(520), IntConst(14))
    @0x02f9  DefineTraceArea(IntConst(1), IntConst(77), IntConst(-5), IntConst(157), IntConst(65), IntConst(14))
    @0x0318  DefineTraceArea(IntConst(2), IntConst(387), IntConst(289), IntConst(467), IntConst(359), IntConst(14))
    @0x0337  DefineTraceArea(IntConst(3), IntConst(641), IntConst(450), IntConst(721), IntConst(520), IntConst(14))
    @0x0356  DefineTraceArea(IntConst(4), IntConst(725), IntConst(450), IntConst(805), IntConst(520), IntConst(14))
    @0x0375  DefineTraceArea(IntConst(5), IntConst(725), IntConst(96), IntConst(805), IntConst(166), IntConst(14))
    @0x0394  DefineTraceArea(IntConst(6), IntConst(725), IntConst(-5), IntConst(805), IntConst(65), IntConst(14))
    @0x03b3  DefineTraceArea(IntConst(7), IntConst(174), IntConst(-5), IntConst(254), IntConst(65), IntConst(14))
    @0x03d2  DefineTraceArea(IntConst(8), IntConst(279), IntConst(-5), IntConst(359), IntConst(65), IntConst(14))
    @0x03f1  InsertBulanci()
    @0x03f2  Return(IntConst(0))

fn export#2 @ 0x03f8  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x03f9  IfEqual(GetGlobalVar(3), IntConst(0), 1034)
    @0x0405  SetGlobalVar(3, StrmDestroy(GetGlobalVar(3)))
    @0x040a  Return(IntConst(0))

fn export#3 @ 0x0410  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0411  Return(IntConst(0))

fn export#4 @ 0x0417  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x0418  Return(IntConst(0))

fn export#5 @ 0x041e  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x041f  Return(IntConst(0))

fn export#6 @ 0x0425  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x0426  Return(IntConst(0))
    ; (3600 byte(s) of unreachable tail/inline helper)

fn export#7 @ 0x123c  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x123d  Switch(GetLocalVar(0); 0=>0x1289, 1=>0x1296, 2=>0x12a3, 3=>0x12b0, 4=>0x12bd, 5=>0x12ca, 6=>0x12d7, 7=>0x12e4, 8=>0x12f1)
    @0x1289  Call(fn@0x42c, GetLocalVar(1))
    @0x1291  Goto(4862)
    @0x1296  Call(fn@0x5bc, GetLocalVar(1))
    @0x129e  Goto(4862)
    @0x12a3  Call(fn@0x74c, GetLocalVar(1))
    @0x12ab  Goto(4862)
    @0x12b0  Call(fn@0x8dc, GetLocalVar(1))
    @0x12b8  Goto(4862)
    @0x12bd  Call(fn@0xa6c, GetLocalVar(1))
    @0x12c5  Goto(4862)
    @0x12ca  Call(fn@0xbfc, GetLocalVar(1))
    @0x12d2  Goto(4862)
    @0x12d7  Call(fn@0xd8c, GetLocalVar(1))
    @0x12df  Goto(4862)
    @0x12e4  Call(fn@0xf1c, GetLocalVar(1))
    @0x12ec  Goto(4862)
    @0x12f1  Call(fn@0x10ac, GetLocalVar(1))
    @0x12f9  Goto(4862)
    @0x12fe  Return(IntConst(0))

fn export#8 @ 0x1304  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x1305  Switch(GetLocalVar(0); 0=>0x1351, 1=>0x1356, 2=>0x135b, 3=>0x1360, 4=>0x1365, 5=>0x136a, 6=>0x136f, 7=>0x1374, 8=>0x1379)
    @0x1351  Goto(4990)
    @0x1356  Goto(4990)
    @0x135b  Goto(4990)
    @0x1360  Goto(4990)
    @0x1365  Goto(4990)
    @0x136a  Goto(4990)
    @0x136f  Goto(4990)
    @0x1374  Goto(4990)
    @0x1379  Goto(4990)
    @0x137e  Return(IntConst(0))

fn export#9 @ 0x1384  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=4
    @0x1385  SetLocalVar(0, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x138f  IfNotEqual(GetLocalVar(0), IntConst(1), 5061)
    @0x139b  SetLocalVar(3, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x13a5  SetLocalVar(1, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x13af  SetLocalVar(2, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x13b9  TeleportPlayerTo(GetLocalVar(3), GetLocalVar(1), GetLocalVar(2), IntConst(1))
    @0x13c5  Return(IntConst(0))

fn export#10 @ 0x13cb  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x13cc  Return(IntConst(0))
