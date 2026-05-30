
fn export#0 @ 0x0064  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0065  SetGlobalVar(1, IntConst(1000))
    @0x006c  SetGlobalVar(2, StrConst("CA434A51-9B74-417C-88B1-2FC1A1643011"))
    @0x00b9  SetGlobalVar(0, StrConst("Autobusy"))
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
    @0x011b  InsertView(CreateObstacle(IntConst(0), IntConst(0), IntConst(800), IntConst(14)))
    @0x0131  InsertView(CreateObstacle(IntConst(569), IntConst(391), IntConst(785), IntConst(410)))
    @0x0147  InsertView(CreateObstacle(IntConst(372), IntConst(391), IntConst(462), IntConst(410)))
    @0x015d  InsertView(CreateObstacle(IntConst(353), IntConst(301), IntConst(372), IntConst(409)))
    @0x0173  InsertView(CreateObstacle(IntConst(785), IntConst(337), IntConst(800), IntConst(499)))
    @0x0189  InsertView(CreateObstacle(IntConst(785), IntConst(14), IntConst(800), IntConst(212)))
    @0x019f  InsertView(CreateObstacle(IntConst(12), IntConst(499), IntConst(800), IntConst(515)))
    @0x01b5  InsertView(CreateObstacle(IntConst(0), IntConst(14), IntConst(12), IntConst(515)))
    @0x01cb  InsertView(CreateObstacle(IntConst(262), IntConst(279), IntConst(304), IntConst(298)))
    @0x01e1  InsertView(CreateObstacle(IntConst(231), IntConst(293), IntConst(259), IntConst(303)))
    @0x01f7  InsertView(CreateObstacle(IntConst(40), IntConst(336), IntConst(71), IntConst(343)))
    @0x020d  InsertView(CreateObstacle(IntConst(195), IntConst(303), IntConst(269), IntConst(311)))
    @0x0223  InsertView(CreateObstacle(IntConst(160), IntConst(311), IntConst(241), IntConst(319)))
    @0x0239  InsertView(CreateObstacle(IntConst(126), IntConst(319), IntConst(201), IntConst(330)))
    @0x024f  InsertView(CreateObstacle(IntConst(93), IntConst(329), IntConst(163), IntConst(341)))
    @0x0265  InsertView(CreateObstacle(IntConst(19), IntConst(341), IntConst(141), IntConst(356)))
    @0x027b  InsertView(CreateObstacle(IntConst(50), IntConst(356), IntConst(88), IntConst(364)))
    @0x0291  InsertView(CreateObstacle(IntConst(483), IntConst(118), IntConst(547), IntConst(128)))
    @0x02a7  InsertView(CreateObstacle(IntConst(491), IntConst(126), IntConst(575), IntConst(131)))
    @0x02bd  InsertView(CreateObstacle(IntConst(516), IntConst(133), IntConst(597), IntConst(136)))
    @0x02d3  InsertView(CreateObstacle(IntConst(538), IntConst(137), IntConst(623), IntConst(143)))
    @0x02e9  InsertView(CreateObstacle(IntConst(560), IntConst(144), IntConst(644), IntConst(150)))
    @0x02ff  InsertView(CreateObstacle(IntConst(584), IntConst(151), IntConst(677), IntConst(159)))
    @0x0315  InsertView(CreateObstacle(IntConst(656), IntConst(168), IntConst(703), IntConst(176)))
    @0x032b  InsertView(CreateObstacle(IntConst(620), IntConst(160), IntConst(733), IntConst(168)))
    @0x0341  InsertView(CreateObstacle(IntConst(266), IntConst(123), IntConst(410), IntConst(127)))
    @0x0357  InsertView(CreateObstacle(IntConst(96), IntConst(142), IntConst(173), IntConst(145)))
    @0x036d  InsertView(CreateObstacle(IntConst(95), IntConst(135), IntConst(246), IntConst(142)))
    @0x0383  InsertView(CreateObstacle(IntConst(93), IntConst(127), IntConst(411), IntConst(135)))
    @0x0399  SetInsertMode(IntConst(2))
    @0x039f  InsertView(CreateImage(IntConst(13), IntConst(206), 100006))
    @0x03af  InsertView(CreateImage(IntConst(478), IntConst(46), 100005))
    @0x03bf  InsertView(CreateImage(IntConst(92), IntConst(39), 100004))
    @0x03cf  SetInsertMode(IntConst(1))
    @0x03d5  SetInsertMode(IntConst(0))
    @0x03db  IfEqual(IsServer(), IntConst(0), 1060)
    @0x03e6  DefineTraceArea(IntConst(0), IntConst(701), IntConst(420), IntConst(781), IntConst(490), IntConst(14))
    @0x0405  DefineTraceArea(IntConst(1), IntConst(17), IntConst(23), IntConst(97), IntConst(93), IntConst(14))
    @0x0424  InsertBulanci()
    @0x0425  Return(IntConst(0))

fn export#2 @ 0x042b  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x042c  IfEqual(GetGlobalVar(3), IntConst(0), 1085)
    @0x0438  SetGlobalVar(3, StrmDestroy(GetGlobalVar(3)))
    @0x043d  Return(IntConst(0))

fn export#3 @ 0x0443  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0444  Return(IntConst(0))

fn export#4 @ 0x044a  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x044b  Return(IntConst(0))

fn export#5 @ 0x0451  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x0452  Return(IntConst(0))

fn export#6 @ 0x0458  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x0459  Return(IntConst(0))

fn export#7 @ 0x045f  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x0460  Switch(GetLocalVar(0); 0=>0x474, 1=>0x48b)
    @0x0474  Call(fn@0x0, GetLocalVar(1), IntConst(31), IntConst(39))
    @0x0486  Goto(1186)
    @0x048b  Call(fn@0x0, GetLocalVar(1), IntConst(715), IntConst(436))
    @0x049d  Goto(1186)
    @0x04a2  Return(IntConst(0))

fn export#8 @ 0x04a8  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x04a9  Switch(GetLocalVar(0); 0=>0x4bd, 1=>0x4c2)
    @0x04bd  Goto(1223)
    @0x04c2  Goto(1223)
    @0x04c7  Return(IntConst(0))

fn export#9 @ 0x04cd  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=4
    @0x04ce  SetLocalVar(0, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x04d8  IfNotEqual(GetLocalVar(0), IntConst(1), 1294)
    @0x04e4  SetLocalVar(3, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x04ee  SetLocalVar(1, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x04f8  SetLocalVar(2, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x0502  TeleportPlayerTo(GetLocalVar(3), GetLocalVar(1), GetLocalVar(2), IntConst(1))
    @0x050e  Return(IntConst(0))

fn export#10 @ 0x0514  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0515  Return(IntConst(0))

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
