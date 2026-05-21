
fn export#0 @ 0x0064  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0065  SetGlobalVar(1, IntConst(1000))
    @0x006c  SetGlobalVar(2, StrConst("311CAC12-DA5E-4275-A8C5-B9EE0815F835"))
    @0x00b9  SetGlobalVar(0, StrConst("Western"))
    @0x00cc  Return(IntConst(0))

fn export#1 @ 0x00d2  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x00d3  SetGlobalVar(3, IntConst(0))
    @0x00da  IfEqual(IsNet(), IntConst(0), 245)
    @0x00e5  SetGlobalVar(3, StrmCreateMem(IntConst(4096), IntConst(4096)))
    @0x00f2  SetCommStrm(GetGlobalVar(3))
    @0x00f5  LoadPreface(100001)
    @0x00fa  SetMusic(100003, 0)
    @0x0103  SetInsertMode(IntConst(0))
    @0x0109  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x0119  InsertView(CreateObstacle(IntConst(721), IntConst(0), IntConst(732), IntConst(20)))
    @0x012f  InsertView(CreateObstacle(IntConst(434), IntConst(171), IntConst(455), IntConst(183)))
    @0x0145  InsertView(CreateObstacle(IntConst(460), IntConst(170), IntConst(478), IntConst(193)))
    @0x015b  InsertView(CreateObstacle(IntConst(481), IntConst(190), IntConst(500), IntConst(202)))
    @0x0171  InsertView(CreateObstacle(IntConst(500), IntConst(189), IntConst(568), IntConst(214)))
    @0x0187  InsertView(CreateObstacle(IntConst(480), IntConst(163), IntConst(617), IntConst(189)))
    @0x019d  InsertView(CreateObstacle(IntConst(481), IntConst(139), IntConst(637), IntConst(163)))
    @0x01b3  InsertView(CreateObstacle(IntConst(481), IntConst(116), IntConst(658), IntConst(139)))
    @0x01c9  InsertView(CreateObstacle(IntConst(481), IntConst(91), IntConst(674), IntConst(116)))
    @0x01df  InsertView(CreateObstacle(IntConst(370), IntConst(0), IntConst(721), IntConst(47)))
    @0x01f5  InsertView(CreateObstacle(IntConst(424), IntConst(91), IntConst(481), IntConst(170)))
    @0x020b  InsertView(CreateObstacle(IntConst(315), IntConst(149), IntConst(356), IntConst(159)))
    @0x0221  InsertView(CreateObstacle(IntConst(289), IntConst(0), IntConst(304), IntConst(47)))
    @0x0237  InsertView(CreateObstacle(IntConst(261), IntConst(87), IntConst(274), IntConst(111)))
    @0x024d  InsertView(CreateObstacle(IntConst(253), IntConst(112), IntConst(275), IntConst(134)))
    @0x0263  InsertView(CreateObstacle(IntConst(303), IntConst(0), IntConst(370), IntConst(148)))
    @0x0279  InsertView(CreateObstacle(IntConst(275), IntConst(47), IntConst(303), IntConst(148)))
    @0x028f  InsertView(CreateObstacle(IntConst(482), IntConst(463), IntConst(503), IntConst(473)))
    @0x02a5  InsertView(CreateObstacle(IntConst(459), IntConst(453), IntConst(510), IntConst(463)))
    @0x02bb  InsertView(CreateObstacle(IntConst(442), IntConst(432), IntConst(523), IntConst(452)))
    @0x02d1  InsertView(CreateObstacle(IntConst(456), IntConst(412), IntConst(541), IntConst(431)))
    @0x02e7  InsertView(CreateObstacle(IntConst(473), IntConst(393), IntConst(555), IntConst(412)))
    @0x02fd  InsertView(CreateObstacle(IntConst(488), IntConst(374), IntConst(571), IntConst(393)))
    @0x0313  InsertView(CreateObstacle(IntConst(502), IntConst(354), IntConst(587), IntConst(373)))
    @0x0329  InsertView(CreateObstacle(IntConst(503), IntConst(341), IntConst(600), IntConst(354)))
    @0x033f  InsertView(CreateObstacle(IntConst(542), IntConst(303), IntConst(574), IntConst(314)))
    @0x0355  InsertView(CreateObstacle(IntConst(527), IntConst(292), IntConst(541), IntConst(316)))
    @0x036b  InsertView(CreateObstacle(IntConst(516), IntConst(314), IntConst(589), IntConst(325)))
    @0x0381  InsertView(CreateObstacle(IntConst(506), IntConst(325), IntConst(607), IntConst(341)))
    @0x0397  InsertView(CreateObstacle(IntConst(269), IntConst(263), IntConst(321), IntConst(273)))
    @0x03ad  InsertView(CreateObstacle(IntConst(257), IntConst(269), IntConst(335), IntConst(280)))
    @0x03c3  InsertView(CreateObstacle(IntConst(247), IntConst(279), IntConst(343), IntConst(289)))
    @0x03d9  InsertView(CreateObstacle(IntConst(262), IntConst(322), IntConst(321), IntConst(329)))
    @0x03ef  InsertView(CreateObstacle(IntConst(250), IntConst(307), IntConst(337), IntConst(322)))
    @0x0405  InsertView(CreateObstacle(IntConst(244), IntConst(288), IntConst(344), IntConst(307)))
    @0x041b  InsertView(CreateObstacle(IntConst(152), IntConst(191), IntConst(188), IntConst(218)))
    @0x0431  InsertView(CreateObstacle(IntConst(57), IntConst(310), IntConst(95), IntConst(336)))
    @0x0447  SetInsertMode(IntConst(2))
    @0x044d  InsertView(CreateImage(IntConst(437), IntConst(0), 100012))
    @0x045d  InsertView(CreateImage(IntConst(537), IntConst(203), 100009))
    @0x046d  InsertView(CreateImage(IntConst(0), IntConst(348), 100004))
    @0x047d  SetInsertMode(IntConst(1))
    @0x0483  InsertView(SetOrderAxis(CreateImage(IntConst(308), IntConst(0), 100011), IntConst(-18)))
    @0x0499  InsertView(SetOrderAxis(CreateImage(IntConst(240), IntConst(0), 100010), IntConst(0)))
    @0x04af  InsertView(SetOrderAxis(CreateImage(IntConst(433), IntConst(241), 100008), IntConst(0)))
    @0x04c5  InsertView(SetOrderAxis(CreateImage(IntConst(244), IntConst(249), 100007), IntConst(-37)))
    @0x04db  InsertView(SetOrderAxis(CreateImage(IntConst(31), IntConst(17), 100006), IntConst(-8)))
    @0x04f1  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(105), 100005), IntConst(-9)))
    @0x0507  SetInsertMode(IntConst(0))
    @0x050d  IfEqual(IsServer(), IntConst(0), 1397)
    @0x0518  DefineTraceArea(IntConst(0), IntConst(8), IntConst(9), IntConst(88), IntConst(79), IntConst(14))
    @0x0537  DefineTraceArea(IntConst(1), IntConst(680), IntConst(407), IntConst(760), IntConst(477), IntConst(14))
    @0x0556  DefineTraceArea(IntConst(2), IntConst(390), IntConst(242), IntConst(470), IntConst(312), IntConst(14))
    @0x0575  InsertBulanci()
    @0x0576  Return(IntConst(0))

fn export#2 @ 0x057c  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x057d  IfEqual(GetGlobalVar(3), IntConst(0), 1422)
    @0x0589  SetGlobalVar(3, StrmDestroy(GetGlobalVar(3)))
    @0x058e  Return(IntConst(0))

fn export#3 @ 0x0594  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0595  Return(IntConst(0))

fn export#4 @ 0x059b  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x059c  Return(IntConst(0))

fn export#5 @ 0x05a2  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x05a3  Return(IntConst(0))

fn export#6 @ 0x05a9  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x05aa  Return(IntConst(0))
    ; (462 byte(s) of unreachable tail/inline helper)

fn export#7 @ 0x077e  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x077f  Switch(GetLocalVar(0); 0=>0x79b, 1=>0x7a8, 2=>0x7b5)
    @0x079b  Call(fn@0x5b0, GetLocalVar(1))
    @0x07a3  Goto(1986)
    @0x07a8  Call(fn@0x64a, GetLocalVar(1))
    @0x07b0  Goto(1986)
    @0x07b5  Call(fn@0x6e4, GetLocalVar(1))
    @0x07bd  Goto(1986)
    @0x07c2  Return(IntConst(0))

fn export#8 @ 0x07c8  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x07c9  Switch(GetLocalVar(0); 0=>0x7e5, 1=>0x7ea, 2=>0x7ef)
    @0x07e5  Goto(2036)
    @0x07ea  Goto(2036)
    @0x07ef  Goto(2036)
    @0x07f4  Return(IntConst(0))

fn export#9 @ 0x07fa  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=4
    @0x07fb  SetLocalVar(0, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x0805  IfNotEqual(GetLocalVar(0), IntConst(1), 2107)
    @0x0811  SetLocalVar(3, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x081b  SetLocalVar(1, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x0825  SetLocalVar(2, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x082f  TeleportPlayerTo(GetLocalVar(3), GetLocalVar(1), GetLocalVar(2), IntConst(1))
    @0x083b  Return(IntConst(0))

fn export#10 @ 0x0841  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0842  Return(IntConst(0))
