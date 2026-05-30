
fn export#0 @ 0x0064  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0065  SetGlobalVar(1, IntConst(1000))
    @0x006c  SetGlobalVar(2, StrConst("8680C86D-EA25-41AA-8562-04156BF8E4B6"))
    @0x00b9  SetGlobalVar(0, StrConst("Smrtící zájezd"))
    @0x00da  Return(IntConst(0))

fn export#1 @ 0x00e0  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x00e1  SetGlobalVar(3, IntConst(0))
    @0x00e8  IfEqual(IsNet(), IntConst(0), 259)
    @0x00f3  SetGlobalVar(3, StrmCreateMem(IntConst(4096), IntConst(4096)))
    @0x0100  SetCommStrm(GetGlobalVar(3))
    @0x0103  LoadPreface(100001)
    @0x0108  SetMusic(100003, 0)
    @0x0111  SetInsertMode(IntConst(0))
    @0x0117  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x0127  InsertView(CreateImage(IntConst(343), IntConst(98), 100005))
    @0x0137  InsertView(CreateImage(IntConst(414), IntConst(106), 100005))
    @0x0147  InsertView(CreateImage(IntConst(486), IntConst(108), 100005))
    @0x0157  InsertView(CreateImage(IntConst(281), IntConst(27), 100005))
    @0x0167  InsertView(CreateImage(IntConst(274), IntConst(74), 100005))
    @0x0177  InsertView(CreateObstacle(IntConst(405), IntConst(416), IntConst(455), IntConst(431)))
    @0x018d  InsertView(CreateObstacle(IntConst(412), IntConst(274), IntConst(423), IntConst(289)))
    @0x01a3  InsertView(CreateObstacle(IntConst(382), IntConst(279), IntConst(395), IntConst(289)))
    @0x01b9  InsertView(CreateObstacle(IntConst(444), IntConst(221), IntConst(458), IntConst(230)))
    @0x01cf  InsertView(CreateObstacle(IntConst(461), IntConst(238), IntConst(472), IntConst(275)))
    @0x01e5  InsertView(CreateObstacle(IntConst(434), IntConst(230), IntConst(461), IntConst(277)))
    @0x01fb  InsertView(CreateObstacle(IntConst(423), IntConst(239), IntConst(433), IntConst(277)))
    @0x0211  InsertView(CreateObstacle(IntConst(424), IntConst(277), IntConst(460), IntConst(306)))
    @0x0227  InsertView(CreateObstacle(IntConst(389), IntConst(289), IntConst(424), IntConst(322)))
    @0x023d  InsertView(CreateObstacle(IntConst(682), IntConst(239), IntConst(700), IntConst(291)))
    @0x0253  InsertView(CreateObstacle(IntConst(90), IntConst(138), IntConst(99), IntConst(147)))
    @0x0269  InsertView(CreateObstacle(IntConst(82), IntConst(76), IntConst(93), IntConst(90)))
    @0x027f  InsertView(CreateImage(IntConst(555), IntConst(178), 100005))
    @0x028f  InsertView(CreateImage(IntConst(220), IntConst(237), 100005))
    @0x029f  InsertView(EnableFireThrough(CreateObstacle(IntConst(0), IntConst(176), IntConst(19), IntConst(194)), IntConst(1)))
    @0x02bb  InsertView(EnableFireThrough(CreateObstacle(IntConst(30), IntConst(151), IntConst(109), IntConst(166)), IntConst(1)))
    @0x02d7  InsertView(EnableFireThrough(CreateObstacle(IntConst(113), IntConst(137), IntConst(152), IntConst(160)), IntConst(1)))
    @0x02f3  InsertView(EnableFireThrough(CreateObstacle(IntConst(145), IntConst(110), IntConst(181), IntConst(135)), IntConst(1)))
    @0x030f  InsertView(EnableFireThrough(CreateObstacle(IntConst(231), IntConst(0), IntConst(259), IntConst(13)), IntConst(1)))
    @0x032b  InsertView(EnableFireThrough(CreateObstacle(IntConst(185), IntConst(0), IntConst(231), IntConst(42)), IntConst(1)))
    @0x0347  InsertView(EnableFireThrough(CreateObstacle(IntConst(0), IntConst(165), IntConst(114), IntConst(176)), IntConst(1)))
    @0x0363  InsertView(CreateObstacle(IntConst(88), IntConst(91), IntConst(115), IntConst(137)))
    @0x0379  InsertView(CreateObstacle(IntConst(65), IntConst(89), IntConst(88), IntConst(141)))
    @0x038f  InsertView(CreateObstacle(IntConst(653), IntConst(377), IntConst(678), IntConst(383)))
    @0x03a5  InsertView(CreateObstacle(IntConst(466), IntConst(420), IntConst(484), IntConst(437)))
    @0x03bb  InsertView(CreateObstacle(IntConst(493), IntConst(405), IntConst(508), IntConst(421)))
    @0x03d1  InsertView(CreateObstacle(IntConst(523), IntConst(376), IntConst(531), IntConst(387)))
    @0x03e7  InsertView(CreateObstacle(IntConst(486), IntConst(403), IntConst(492), IntConst(440)))
    @0x03fd  InsertView(CreateObstacle(IntConst(650), IntConst(358), IntConst(663), IntConst(368)))
    @0x0413  InsertView(CreateObstacle(IntConst(558), IntConst(343), IntConst(648), IntConst(352)))
    @0x0429  InsertView(CreateObstacle(IntConst(533), IntConst(350), IntConst(649), IntConst(369)))
    @0x043f  InsertView(CreateObstacle(IntConst(531), IntConst(379), IntConst(551), IntConst(401)))
    @0x0455  InsertView(CreateObstacle(IntConst(488), IntConst(381), IntConst(529), IntConst(412)))
    @0x046b  InsertView(CreateObstacle(IntConst(466), IntConst(415), IntConst(514), IntConst(425)))
    @0x0481  InsertView(CreateObstacle(IntConst(453), IntConst(428), IntConst(490), IntConst(450)))
    @0x0497  InsertView(CreateObstacle(IntConst(628), IntConst(379), IntConst(649), IntConst(385)))
    @0x04ad  InsertView(CreateObstacle(IntConst(509), IntConst(368), IntConst(685), IntConst(379)))
    @0x04c3  InsertView(CreateObstacle(IntConst(649), IntConst(381), IntConst(692), IntConst(407)))
    @0x04d9  InsertView(CreateObstacle(IntConst(732), IntConst(33), IntConst(745), IntConst(73)))
    @0x04ef  InsertView(CreateObstacle(IntConst(696), IntConst(25), IntConst(732), IntConst(87)))
    @0x0505  InsertView(CreateObstacle(IntConst(653), IntConst(31), IntConst(693), IntConst(53)))
    @0x051b  InsertView(CreateObstacle(IntConst(52), IntConst(376), IntConst(63), IntConst(416)))
    @0x0531  InsertView(CreateObstacle(IntConst(21), IntConst(360), IntConst(42), IntConst(367)))
    @0x0547  InsertView(CreateObstacle(IntConst(15), IntConst(418), IntConst(45), IntConst(426)))
    @0x055d  InsertView(CreateObstacle(IntConst(0), IntConst(367), IntConst(52), IntConst(418)))
    @0x0573  InsertView(CreateObstacle(IntConst(63), IntConst(359), IntConst(104), IntConst(396)))
    @0x0589  InsertView(CreateObstacle(IntConst(479), IntConst(174), IntConst(492), IntConst(179)))
    @0x059f  InsertView(CreateObstacle(IntConst(390), IntConst(148), IntConst(405), IntConst(155)))
    @0x05b5  InsertView(CreateObstacle(IntConst(356), IntConst(138), IntConst(366), IntConst(143)))
    @0x05cb  InsertView(CreateObstacle(IntConst(310), IntConst(124), IntConst(320), IntConst(130)))
    @0x05e1  InsertView(CreateObstacle(IntConst(423), IntConst(142), IntConst(436), IntConst(147)))
    @0x05f7  InsertView(CreateObstacle(IntConst(456), IntConst(153), IntConst(473), IntConst(161)))
    @0x060d  InsertView(CreateObstacle(IntConst(492), IntConst(158), IntConst(541), IntConst(163)))
    @0x0623  InsertView(CreateObstacle(IntConst(546), IntConst(152), IntConst(552), IntConst(159)))
    @0x0639  InsertView(CreateObstacle(IntConst(529), IntConst(157), IntConst(542), IntConst(167)))
    @0x064f  InsertView(CreateObstacle(IntConst(508), IntConst(161), IntConst(526), IntConst(173)))
    @0x0665  InsertView(CreateObstacle(IntConst(493), IntConst(163), IntConst(506), IntConst(181)))
    @0x067b  InsertView(CreateObstacle(IntConst(436), IntConst(159), IntConst(492), IntConst(174)))
    @0x0691  InsertView(CreateObstacle(IntConst(406), IntConst(147), IntConst(456), IntConst(159)))
    @0x06a7  InsertView(CreateObstacle(IntConst(379), IntConst(129), IntConst(398), IntConst(138)))
    @0x06bd  InsertView(CreateObstacle(IntConst(340), IntConst(117), IntConst(353), IntConst(126)))
    @0x06d3  InsertView(CreateObstacle(IntConst(366), IntConst(138), IntConst(422), IntConst(147)))
    @0x06e9  InsertView(CreateObstacle(IntConst(320), IntConst(125), IntConst(379), IntConst(138)))
    @0x06ff  InsertView(CreateObstacle(IntConst(290), IntConst(110), IntConst(339), IntConst(124)))
    @0x0715  SetInsertMode(IntConst(2))
    @0x071b  InsertView(CreateImage(IntConst(287), IntConst(38), 100004))
    @0x072b  InsertView(CreateImage(IntConst(675), IntConst(113), 100006))
    @0x073b  SetInsertMode(IntConst(1))
    @0x0741  SetInsertMode(IntConst(0))
    @0x0747  IfEqual(IsServer(), IntConst(0), 1936)
    @0x0752  DefineTraceArea(IntConst(0), IntConst(725), IntConst(297), IntConst(805), IntConst(367), IntConst(14))
    @0x0771  DefineTraceArea(IntConst(1), IntConst(-5), IntConst(224), IntConst(75), IntConst(294), IntConst(14))
    @0x0790  InsertBulanci()
    @0x0791  InsertVampires()
    @0x0792  Return(IntConst(0))

fn export#2 @ 0x0798  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x0799  IfEqual(GetGlobalVar(3), IntConst(0), 1962)
    @0x07a5  SetGlobalVar(3, StrmDestroy(GetGlobalVar(3)))
    @0x07aa  Return(IntConst(0))

fn export#3 @ 0x07b0  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x07b1  Return(IntConst(0))

fn export#4 @ 0x07b7  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x07b8  Return(IntConst(0))

fn export#5 @ 0x07be  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x07bf  Return(IntConst(0))

fn export#6 @ 0x07c5  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x07c6  Return(IntConst(0))

fn export#7 @ 0x07cc  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x07cd  Switch(GetLocalVar(0); 0=>0x7e1, 1=>0x7f8)
    @0x07e1  Call(fn@0x0, GetLocalVar(1), IntConst(9), IntConst(240))
    @0x07f3  Goto(2063)
    @0x07f8  Call(fn@0x0, GetLocalVar(1), IntConst(739), IntConst(313))
    @0x080a  Goto(2063)
    @0x080f  Return(IntConst(0))

fn export#8 @ 0x0815  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x0816  Switch(GetLocalVar(0); 0=>0x82a, 1=>0x82f)
    @0x082a  Goto(2100)
    @0x082f  Goto(2100)
    @0x0834  Return(IntConst(0))

fn export#9 @ 0x083a  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=4
    @0x083b  SetLocalVar(0, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x0845  IfNotEqual(GetLocalVar(0), IntConst(1), 2171)
    @0x0851  SetLocalVar(3, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x085b  SetLocalVar(1, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x0865  SetLocalVar(2, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x086f  TeleportPlayerTo(GetLocalVar(3), GetLocalVar(1), GetLocalVar(2), IntConst(1))
    @0x087b  Return(IntConst(0))

fn export#10 @ 0x0881  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0882  Return(IntConst(0))

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
