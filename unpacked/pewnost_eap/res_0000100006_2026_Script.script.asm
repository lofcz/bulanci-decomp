
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  SetGlobalVar(1, IntConst(1000))
    @0x0008  SetGlobalVar(2, StrConst("6A2B7D86-A2AF-4471-8BF6-F68DF5B2624E"))
    @0x0055  SetGlobalVar(0, StrConst("Pewnost"))
    @0x0068  Return(IntConst(0))

fn export#1 @ 0x006e  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x006f  LoadPreface(100001)
    @0x0074  SetMusic(100003, 0)
    @0x007d  SetInsertMode(IntConst(0))
    @0x0083  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x0093  InsertView(CreateObstacle(IntConst(13), IntConst(488), IntConst(50), IntConst(514)))
    @0x00a9  InsertView(CreateObstacle(IntConst(681), IntConst(478), IntConst(717), IntConst(498)))
    @0x00bf  InsertView(CreateObstacle(IntConst(685), IntConst(350), IntConst(800), IntConst(359)))
    @0x00d5  InsertView(EnableFireThrough(CreateObstacle(IntConst(137), IntConst(209), IntConst(178), IntConst(255)), IntConst(1)))
    @0x00f1  InsertView(CreateObstacle(IntConst(133), IntConst(202), IntConst(143), IntConst(234)))
    @0x0107  InsertView(EnableFireThrough(CreateObstacle(IntConst(258), IntConst(58), IntConst(279), IntConst(80)), IntConst(1)))
    @0x0123  InsertView(EnableFireThrough(CreateObstacle(IntConst(725), IntConst(0), IntConst(748), IntConst(35)), IntConst(1)))
    @0x013f  InsertView(EnableFireThrough(CreateObstacle(IntConst(743), IntConst(85), IntConst(768), IntConst(110)), IntConst(1)))
    @0x015b  InsertView(EnableFireThrough(CreateObstacle(IntConst(738), IntConst(59), IntConst(762), IntConst(89)), IntConst(1)))
    @0x0177  InsertView(EnableFireThrough(CreateObstacle(IntConst(735), IntConst(37), IntConst(757), IntConst(60)), IntConst(1)))
    @0x0193  InsertView(EnableFireThrough(CreateObstacle(IntConst(731), IntConst(0), IntConst(761), IntConst(41)), IntConst(1)))
    @0x01af  InsertView(EnableFireThrough(CreateObstacle(IntConst(763), IntConst(129), IntConst(798), IntConst(168)), IntConst(1)))
    @0x01cb  InsertView(EnableFireThrough(CreateObstacle(IntConst(751), IntConst(0), IntConst(800), IntConst(141)), IntConst(1)))
    @0x01e7  InsertView(CreateObstacle(IntConst(659), IntConst(234), IntConst(670), IntConst(263)))
    @0x01fd  InsertView(CreateObstacle(IntConst(658), IntConst(214), IntConst(668), IntConst(242)))
    @0x0213  InsertView(CreateObstacle(IntConst(656), IntConst(183), IntConst(666), IntConst(214)))
    @0x0229  InsertView(CreateObstacle(IntConst(654), IntConst(151), IntConst(666), IntConst(180)))
    @0x023f  InsertView(CreateObstacle(IntConst(636), IntConst(261), IntConst(669), IntConst(271)))
    @0x0255  InsertView(CreateObstacle(IntConst(605), IntConst(270), IntConst(639), IntConst(278)))
    @0x026b  InsertView(CreateObstacle(IntConst(579), IntConst(277), IntConst(609), IntConst(285)))
    @0x0281  InsertView(CreateObstacle(IntConst(551), IntConst(284), IntConst(580), IntConst(294)))
    @0x0297  InsertView(EnableFireThrough(CreateObstacle(IntConst(501), IntConst(231), IntConst(560), IntConst(241)), IntConst(1)))
    @0x02b3  InsertView(EnableFireThrough(CreateObstacle(IntConst(386), IntConst(228), IntConst(498), IntConst(243)), IntConst(1)))
    @0x02cf  InsertView(EnableFireThrough(CreateObstacle(IntConst(304), IntConst(214), IntConst(393), IntConst(245)), IntConst(1)))
    @0x02eb  InsertView(EnableFireThrough(CreateObstacle(IntConst(289), IntConst(191), IntConst(314), IntConst(228)), IntConst(1)))
    @0x0307  InsertView(EnableFireThrough(CreateObstacle(IntConst(277), IntConst(157), IntConst(319), IntConst(198)), IntConst(1)))
    @0x0323  InsertView(EnableFireThrough(CreateObstacle(IntConst(247), IntConst(160), IntConst(275), IntConst(198)), IntConst(1)))
    @0x033f  InsertView(CreateObstacle(IntConst(268), IntConst(148), IntConst(513), IntConst(158)))
    @0x0355  InsertView(CreateObstacle(IntConst(357), IntConst(147), IntConst(661), IntConst(157)))
    @0x036b  InsertView(EnableFireThrough(CreateObstacle(IntConst(308), IntConst(156), IntConst(660), IntConst(235)), IntConst(1)))
    @0x0387  InsertView(CreateObstacle(IntConst(246), IntConst(159), IntConst(277), IntConst(173)))
    @0x039d  InsertView(CreateObstacle(IntConst(666), IntConst(372), IntConst(677), IntConst(385)))
    @0x03b3  InsertView(CreateObstacle(IntConst(650), IntConst(361), IntConst(688), IntConst(372)))
    @0x03c9  InsertView(CreateObstacle(IntConst(555), IntConst(381), IntConst(587), IntConst(387)))
    @0x03df  InsertView(CreateObstacle(IntConst(586), IntConst(373), IntConst(621), IntConst(380)))
    @0x03f5  InsertView(CreateObstacle(IntConst(614), IntConst(366), IntConst(660), IntConst(372)))
    @0x040b  InsertView(EnableFireThrough(CreateObstacle(IntConst(193), IntConst(44), IntConst(244), IntConst(87)), IntConst(1)))
    @0x0427  InsertView(EnableFireThrough(CreateObstacle(IntConst(3), IntConst(78), IntConst(140), IntConst(119)), IntConst(1)))
    @0x0443  InsertView(EnableFireThrough(CreateObstacle(IntConst(0), IntConst(54), IntConst(191), IntConst(110)), IntConst(1)))
    @0x045f  InsertView(EnableFireThrough(CreateObstacle(IntConst(0), IntConst(0), IntConst(278), IntConst(80)), IntConst(1)))
    @0x047b  InsertView(CreateObstacle(IntConst(281), IntConst(0), IntConst(294), IntConst(11)))
    @0x0491  InsertView(CreateObstacle(IntConst(275), IntConst(13), IntConst(290), IntConst(31)))
    @0x04a7  InsertView(CreateObstacle(IntConst(273), IntConst(31), IntConst(287), IntConst(45)))
    @0x04bd  InsertView(CreateObstacle(IntConst(268), IntConst(46), IntConst(282), IntConst(71)))
    @0x04d3  InsertView(CreateObstacle(IntConst(250), IntConst(65), IntConst(276), IntConst(78)))
    @0x04e9  InsertView(CreateObstacle(IntConst(218), IntConst(76), IntConst(244), IntConst(89)))
    @0x04ff  InsertView(CreateObstacle(IntConst(195), IntConst(86), IntConst(219), IntConst(97)))
    @0x0515  InsertView(CreateObstacle(IntConst(167), IntConst(98), IntConst(194), IntConst(108)))
    @0x052b  InsertView(CreateObstacle(IntConst(142), IntConst(108), IntConst(168), IntConst(117)))
    @0x0541  InsertView(CreateObstacle(IntConst(49), IntConst(119), IntConst(83), IntConst(131)))
    @0x0557  InsertView(EnableFireThrough(CreateObstacle(IntConst(775), IntConst(287), IntConst(800), IntConst(306)), IntConst(1)))
    @0x0573  InsertView(EnableFireThrough(CreateObstacle(IntConst(770), IntConst(262), IntConst(800), IntConst(290)), IntConst(1)))
    @0x058f  InsertView(EnableFireThrough(CreateObstacle(IntConst(766), IntConst(248), IntConst(793), IntConst(266)), IntConst(1)))
    @0x05ab  InsertView(EnableFireThrough(CreateObstacle(IntConst(781), IntConst(278), IntConst(800), IntConst(348)), IntConst(1)))
    @0x05c7  InsertView(EnableFireThrough(CreateObstacle(IntConst(765), IntConst(197), IntConst(797), IntConst(256)), IntConst(1)))
    @0x05e3  InsertView(EnableFireThrough(CreateObstacle(IntConst(763), IntConst(170), IntConst(800), IntConst(222)), IntConst(1)))
    @0x05ff  InsertView(EnableFireThrough(CreateObstacle(IntConst(674), IntConst(359), IntConst(800), IntConst(384)), IntConst(1)))
    @0x061b  InsertView(CreateObstacle(IntConst(142), IntConst(285), IntConst(152), IntConst(308)))
    @0x0631  InsertView(CreateObstacle(IntConst(138), IntConst(259), IntConst(151), IntConst(287)))
    @0x0647  InsertView(CreateObstacle(IntConst(135), IntConst(224), IntConst(145), IntConst(257)))
    @0x065d  InsertView(CreateObstacle(IntConst(136), IntConst(197), IntConst(182), IntConst(207)))
    @0x0673  InsertView(CreateObstacle(IntConst(160), IntConst(188), IntConst(194), IntConst(198)))
    @0x0689  InsertView(CreateObstacle(IntConst(190), IntConst(179), IntConst(216), IntConst(189)))
    @0x069f  InsertView(CreateObstacle(IntConst(215), IntConst(169), IntConst(275), IntConst(180)))
    @0x06b5  InsertView(CreateObstacle(IntConst(17), IntConst(292), IntConst(36), IntConst(308)))
    @0x06cb  InsertView(CreateObstacle(IntConst(12), IntConst(246), IntConst(36), IntConst(259)))
    @0x06e1  InsertView(CreateObstacle(IntConst(7), IntConst(277), IntConst(38), IntConst(292)))
    @0x06f7  InsertView(CreateObstacle(IntConst(16), IntConst(308), IntConst(42), IntConst(324)))
    @0x070d  InsertView(CreateObstacle(IntConst(0), IntConst(180), IntConst(31), IntConst(337)))
    @0x0723  InsertView(CreateObstacle(IntConst(0), IntConst(148), IntConst(22), IntConst(222)))
    @0x0739  InsertView(CreateObstacle(IntConst(23), IntConst(129), IntConst(47), IntConst(153)))
    @0x074f  InsertView(CreateObstacle(IntConst(26), IntConst(139), IntConst(43), IntConst(168)))
    @0x0765  InsertView(CreateObstacle(IntConst(31), IntConst(128), IntConst(51), IntConst(142)))
    @0x077b  InsertView(CreateObstacle(IntConst(11), IntConst(185), IntConst(33), IntConst(206)))
    @0x0791  InsertView(CreateObstacle(IntConst(13), IntConst(173), IntConst(35), IntConst(186)))
    @0x07a7  InsertView(CreateObstacle(IntConst(12), IntConst(145), IntConst(38), IntConst(180)))
    @0x07bd  InsertView(CreateObstacle(IntConst(0), IntConst(119), IntConst(37), IntConst(166)))
    @0x07d3  InsertView(CreateObstacle(IntConst(87), IntConst(118), IntConst(139), IntConst(128)))
    @0x07e9  InsertView(EnableFireThrough(CreateObstacle(IntConst(175), IntConst(192), IntConst(314), IntConst(234)), IntConst(1)))
    @0x0805  SetInsertMode(IntConst(2))
    @0x080b  SetInsertMode(IntConst(1))
    @0x0811  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(337), 100005), IntConst(0)))
    @0x0827  InsertView(SetOrderAxis(CreateImage(IntConst(629), IntConst(321), 100004), IntConst(0)))
    @0x083d  SetInsertMode(IntConst(0))
    @0x0843  InsertBulanci()
    @0x0844  Return(IntConst(0))

fn export#2 @ 0x084a  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x084b  Return(IntConst(0))

fn export#3 @ 0x0851  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0852  Return(IntConst(0))

fn export#4 @ 0x0858  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x0859  Return(IntConst(0))

fn export#5 @ 0x085f  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x0860  Return(IntConst(0))

fn export#6 @ 0x0866  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x0867  Return(IntConst(0))

fn export#7 @ 0x086d  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x086e  Return(IntConst(0))

fn export#8 @ 0x0874  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x0875  Return(IntConst(0))

fn export#9 @ 0x087b  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=0
    @0x087c  Return(IntConst(0))

fn export#10 @ 0x0882  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0883  Return(IntConst(0))
