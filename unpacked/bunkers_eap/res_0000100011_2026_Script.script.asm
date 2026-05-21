
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  SetGlobalVar(1, IntConst(1000))
    @0x0008  SetGlobalVar(2, StrConst("271BBE43-1D32-4AF3-8A2C-F24B08C71831"))
    @0x0055  SetGlobalVar(0, StrConst("Bunkers"))
    @0x0068  Return(IntConst(0))

fn export#1 @ 0x006e  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x006f  LoadPreface(100001)
    @0x0074  SetMusic(100003, 0)
    @0x007d  SetInsertMode(IntConst(0))
    @0x0083  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x0093  InsertView(CreateImage(IntConst(207), IntConst(0), 100006))
    @0x00a3  InsertView(CreateObstacle(IntConst(18), IntConst(493), IntConst(47), IntConst(513)))
    @0x00b9  InsertView(CreateObstacle(IntConst(201), IntConst(491), IntConst(230), IntConst(515)))
    @0x00cf  InsertView(CreateObstacle(IntConst(133), IntConst(416), IntConst(162), IntConst(435)))
    @0x00e5  InsertView(CreateObstacle(IntConst(59), IntConst(235), IntConst(72), IntConst(247)))
    @0x00fb  InsertView(CreateObstacle(IntConst(403), IntConst(66), IntConst(427), IntConst(91)))
    @0x0111  InsertView(CreateObstacle(IntConst(391), IntConst(83), IntConst(435), IntConst(95)))
    @0x0127  InsertView(CreateObstacle(IntConst(421), IntConst(5), IntConst(446), IntConst(20)))
    @0x013d  InsertView(CreateObstacle(IntConst(416), IntConst(14), IntConst(445), IntConst(35)))
    @0x0153  InsertView(CreateObstacle(IntConst(411), IntConst(33), IntConst(443), IntConst(56)))
    @0x0169  InsertView(CreateObstacle(IntConst(410), IntConst(48), IntConst(439), IntConst(88)))
    @0x017f  InsertView(CreateObstacle(IntConst(589), IntConst(135), IntConst(800), IntConst(167)))
    @0x0195  InsertView(CreateObstacle(IntConst(68), IntConst(235), IntConst(86), IntConst(254)))
    @0x01ab  InsertView(EnableFireThrough(CreateObstacle(IntConst(275), IntConst(77), IntConst(302), IntConst(109)), IntConst(1)))
    @0x01c7  InsertView(EnableFireThrough(CreateObstacle(IntConst(292), IntConst(133), IntConst(322), IntConst(164)), IntConst(1)))
    @0x01e3  InsertView(EnableFireThrough(CreateObstacle(IntConst(281), IntConst(104), IntConst(319), IntConst(142)), IntConst(1)))
    @0x01ff  InsertView(EnableFireThrough(CreateObstacle(IntConst(776), IntConst(219), IntConst(800), IntConst(269)), IntConst(1)))
    @0x021b  InsertView(EnableFireThrough(CreateObstacle(IntConst(742), IntConst(221), IntConst(787), IntConst(269)), IntConst(1)))
    @0x0237  InsertView(EnableFireThrough(CreateObstacle(IntConst(614), IntConst(222), IntConst(740), IntConst(267)), IntConst(1)))
    @0x0253  InsertView(EnableFireThrough(CreateObstacle(IntConst(512), IntConst(232), IntConst(533), IntConst(261)), IntConst(1)))
    @0x026f  InsertView(EnableFireThrough(CreateObstacle(IntConst(534), IntConst(222), IntConst(615), IntConst(260)), IntConst(1)))
    @0x028b  InsertView(EnableFireThrough(CreateObstacle(IntConst(478), IntConst(241), IntConst(516), IntConst(266)), IntConst(1)))
    @0x02a7  InsertView(EnableFireThrough(CreateObstacle(IntConst(450), IntConst(258), IntConst(486), IntConst(293)), IntConst(1)))
    @0x02c3  InsertView(EnableFireThrough(CreateObstacle(IntConst(437), IntConst(274), IntConst(456), IntConst(313)), IntConst(1)))
    @0x02df  InsertView(EnableFireThrough(CreateObstacle(IntConst(380), IntConst(277), IntConst(435), IntConst(303)), IntConst(1)))
    @0x02fb  InsertView(EnableFireThrough(CreateObstacle(IntConst(295), IntConst(181), IntConst(335), IntConst(227)), IntConst(1)))
    @0x0317  InsertView(EnableFireThrough(CreateObstacle(IntConst(286), IntConst(150), IntConst(332), IntConst(192)), IntConst(1)))
    @0x0333  InsertView(EnableFireThrough(CreateObstacle(IntConst(263), IntConst(54), IntConst(299), IntConst(87)), IntConst(1)))
    @0x034f  InsertView(EnableFireThrough(CreateObstacle(IntConst(247), IntConst(14), IntConst(276), IntConst(42)), IntConst(1)))
    @0x036b  InsertView(EnableFireThrough(CreateObstacle(IntConst(256), IntConst(41), IntConst(293), IntConst(55)), IntConst(1)))
    @0x0387  InsertView(EnableFireThrough(CreateObstacle(IntConst(231), IntConst(0), IntConst(264), IntConst(40)), IntConst(1)))
    @0x03a3  InsertView(CreateObstacle(IntConst(175), IntConst(137), IntConst(198), IntConst(161)))
    @0x03b9  InsertView(CreateObstacle(IntConst(160), IntConst(149), IntConst(180), IntConst(170)))
    @0x03cf  InsertView(CreateObstacle(IntConst(144), IntConst(163), IntConst(168), IntConst(187)))
    @0x03e5  InsertView(CreateObstacle(IntConst(128), IntConst(184), IntConst(150), IntConst(209)))
    @0x03fb  InsertView(CreateObstacle(IntConst(111), IntConst(197), IntConst(133), IntConst(223)))
    @0x0411  InsertView(CreateObstacle(IntConst(97), IntConst(216), IntConst(120), IntConst(242)))
    @0x0427  InsertView(CreateObstacle(IntConst(81), IntConst(234), IntConst(106), IntConst(260)))
    @0x043d  InsertView(CreateObstacle(IntConst(593), IntConst(382), IntConst(613), IntConst(407)))
    @0x0453  InsertView(CreateObstacle(IntConst(578), IntConst(403), IntConst(594), IntConst(427)))
    @0x0469  InsertView(CreateObstacle(IntConst(491), IntConst(492), IntConst(513), IntConst(515)))
    @0x047f  InsertView(CreateObstacle(IntConst(511), IntConst(475), IntConst(530), IntConst(496)))
    @0x0495  InsertView(CreateObstacle(IntConst(528), IntConst(458), IntConst(548), IntConst(478)))
    @0x04ab  InsertView(CreateObstacle(IntConst(543), IntConst(438), IntConst(562), IntConst(461)))
    @0x04c1  InsertView(CreateObstacle(IntConst(556), IntConst(421), IntConst(576), IntConst(444)))
    @0x04d7  InsertView(CreateObstacle(IntConst(570), IntConst(404), IntConst(589), IntConst(424)))
    @0x04ed  InsertView(CreateImage(IntConst(246), IntConst(191), 100005))
    @0x04fd  SetInsertMode(IntConst(2))
    @0x0503  SetInsertMode(IntConst(1))
    @0x0509  InsertView(SetOrderAxis(CreateImage(IntConst(143), IntConst(337), 100009), IntConst(0)))
    @0x051f  InsertView(SetOrderAxis(CreateImage(IntConst(382), IntConst(0), 100008), IntConst(0)))
    @0x0535  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(337), 100010), IntConst(0)))
    @0x054b  InsertView(SetOrderAxis(CreateImage(IntConst(59), IntConst(117), 100004), IntConst(0)))
    @0x0561  InsertView(SetOrderAxis(CreateImage(IntConst(477), IntConst(366), 100004), IntConst(0)))
    @0x0577  InsertView(SetOrderAxis(CreateImage(IntConst(580), IntConst(81), 100007), IntConst(0)))
    @0x058d  InsertView(SetOrderAxis(CreateImage(IntConst(78), IntConst(259), 100009), IntConst(0)))
    @0x05a3  SetInsertMode(IntConst(0))
    @0x05a9  InsertBulanci()
    @0x05aa  Return(IntConst(0))

fn export#2 @ 0x05b0  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x05b1  Return(IntConst(0))

fn export#3 @ 0x05b7  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x05b8  Return(IntConst(0))

fn export#4 @ 0x05be  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x05bf  Return(IntConst(0))

fn export#5 @ 0x05c5  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x05c6  Return(IntConst(0))

fn export#6 @ 0x05cc  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x05cd  Return(IntConst(0))

fn export#7 @ 0x05d3  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x05d4  Return(IntConst(0))

fn export#8 @ 0x05da  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x05db  Return(IntConst(0))

fn export#9 @ 0x05e1  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=0
    @0x05e2  Return(IntConst(0))

fn export#10 @ 0x05e8  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x05e9  Return(IntConst(0))
