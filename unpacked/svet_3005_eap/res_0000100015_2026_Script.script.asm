
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  SetGlobalVar(1, IntConst(1000))
    @0x0008  SetGlobalVar(2, StrConst("280A5615-36D3-49B5-B0AE-D7F566F2D00F"))
    @0x0055  SetGlobalVar(0, StrConst("svět 3005"))
    @0x006c  Return(IntConst(0))

fn export#1 @ 0x0072  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x0073  LoadPreface(100001)
    @0x0078  SetMusic(100003, 0)
    @0x0081  SetInsertMode(IntConst(0))
    @0x0087  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x0097  InsertView(CreateObstacle(IntConst(625), IntConst(376), IntConst(628), IntConst(398)))
    @0x00ad  InsertView(CreateObstacle(IntConst(613), IntConst(380), IntConst(623), IntConst(410)))
    @0x00c3  InsertView(CreateObstacle(IntConst(546), IntConst(380), IntConst(611), IntConst(411)))
    @0x00d9  InsertView(CreateObstacle(IntConst(613), IntConst(503), IntConst(622), IntConst(515)))
    @0x00ef  InsertView(CreateObstacle(IntConst(535), IntConst(398), IntConst(613), IntConst(514)))
    @0x0105  InsertView(CreateObstacle(IntConst(353), IntConst(379), IntConst(425), IntConst(414)))
    @0x011b  InsertView(CreateObstacle(IntConst(353), IntConst(419), IntConst(415), IntConst(464)))
    @0x0131  InsertView(CreateObstacle(IntConst(332), IntConst(457), IntConst(407), IntConst(499)))
    @0x0147  InsertView(CreateObstacle(IntConst(243), IntConst(480), IntConst(322), IntConst(505)))
    @0x015d  InsertView(CreateObstacle(IntConst(283), IntConst(392), IntConst(292), IntConst(408)))
    @0x0173  InsertView(CreateObstacle(IntConst(148), IntConst(374), IntConst(171), IntConst(406)))
    @0x0189  InsertView(CreateObstacle(IntConst(163), IntConst(376), IntConst(239), IntConst(426)))
    @0x019f  InsertView(CreateObstacle(IntConst(0), IntConst(485), IntConst(36), IntConst(515)))
    @0x01b5  InsertView(CreateObstacle(IntConst(64), IntConst(467), IntConst(95), IntConst(490)))
    @0x01cb  InsertView(CreateObstacle(IntConst(95), IntConst(480), IntConst(124), IntConst(502)))
    @0x01e1  InsertView(CreateObstacle(IntConst(0), IntConst(371), IntConst(13), IntConst(394)))
    @0x01f7  InsertView(CreateObstacle(IntConst(650), IntConst(153), IntConst(666), IntConst(161)))
    @0x020d  InsertView(CreateObstacle(IntConst(583), IntConst(153), IntConst(607), IntConst(166)))
    @0x0223  InsertView(CreateObstacle(IntConst(605), IntConst(150), IntConst(652), IntConst(170)))
    @0x0239  InsertView(CreateObstacle(IntConst(576), IntConst(100), IntConst(673), IntConst(151)))
    @0x024f  InsertView(CreateObstacle(IntConst(486), IntConst(112), IntConst(567), IntConst(165)))
    @0x0265  InsertView(CreateObstacle(IntConst(565), IntConst(94), IntConst(576), IntConst(174)))
    @0x027b  InsertView(CreateObstacle(IntConst(477), IntConst(90), IntConst(491), IntConst(171)))
    @0x0291  InsertView(CreateObstacle(IntConst(399), IntConst(158), IntConst(455), IntConst(170)))
    @0x02a7  InsertView(CreateObstacle(IntConst(380), IntConst(113), IntConst(468), IntConst(157)))
    @0x02bd  InsertView(CreateObstacle(IntConst(189), IntConst(109), IntConst(205), IntConst(159)))
    @0x02d3  InsertView(CreateObstacle(IntConst(199), IntConst(109), IntConst(273), IntConst(168)))
    @0x02e9  InsertView(EnableFireThrough(CreateObstacle(IntConst(145), IntConst(26), IntConst(153), IntConst(52)), IntConst(1)))
    @0x0305  InsertView(EnableFireThrough(CreateObstacle(IntConst(138), IntConst(13), IntConst(154), IntConst(30)), IntConst(1)))
    @0x0321  InsertView(EnableFireThrough(CreateObstacle(IntConst(205), IntConst(9), IntConst(215), IntConst(24)), IntConst(1)))
    @0x033d  InsertView(EnableFireThrough(CreateObstacle(IntConst(130), IntConst(0), IntConst(152), IntConst(13)), IntConst(1)))
    @0x0359  InsertView(EnableFireThrough(CreateObstacle(IntConst(196), IntConst(23), IntConst(222), IntConst(82)), IntConst(1)))
    @0x0375  InsertView(EnableFireThrough(CreateObstacle(IntConst(152), IntConst(0), IntConst(204), IntConst(86)), IntConst(1)))
    @0x0391  InsertView(CreateObstacle(IntConst(65), IntConst(185), IntConst(76), IntConst(205)))
    @0x03a7  InsertView(CreateObstacle(IntConst(67), IntConst(167), IntConst(84), IntConst(183)))
    @0x03bd  InsertView(CreateObstacle(IntConst(62), IntConst(140), IntConst(92), IntConst(166)))
    @0x03d3  InsertView(CreateObstacle(IntConst(1), IntConst(135), IntConst(67), IntConst(226)))
    @0x03e9  InsertView(CreateObstacle(IntConst(551), IntConst(284), IntConst(568), IntConst(301)))
    @0x03ff  SetInsertMode(IntConst(2))
    @0x0405  SetInsertMode(IntConst(1))
    @0x040b  InsertView(SetOrderAxis(CreateImage(IntConst(506), IntConst(224), 100014), IntConst(182)))
    @0x0421  InsertView(SetOrderAxis(CreateImage(IntConst(52), IntConst(457), 100013), IntConst(-40)))
    @0x0437  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(315), 100012), IntConst(-25)))
    @0x044d  InsertView(SetOrderAxis(CreateImage(IntConst(219), IntConst(305), 100011), IntConst(-32)))
    @0x0463  InsertView(SetOrderAxis(CreateImage(IntConst(328), IntConst(335), 100010), IntConst(-122)))
    @0x0479  InsertView(SetOrderAxis(CreateImage(IntConst(528), IntConst(335), 100009), IntConst(-123)))
    @0x048f  InsertView(SetOrderAxis(CreateImage(IntConst(474), IntConst(89), 100008), IntConst(-50)))
    @0x04a5  InsertView(SetOrderAxis(CreateImage(IntConst(369), IntConst(78), 100007), IntConst(-59)))
    @0x04bb  InsertView(SetOrderAxis(CreateImage(IntConst(146), IntConst(331), 100006), IntConst(-49)))
    @0x04d1  InsertView(SetOrderAxis(CreateImage(IntConst(183), IntConst(75), 100005), IntConst(-46)))
    @0x04e7  InsertView(SetOrderAxis(CreateImage(IntConst(572), IntConst(74), 100004), IntConst(-63)))
    @0x04fd  SetInsertMode(IntConst(0))
    @0x0503  InsertBulanci()
    @0x0504  InsertVampires()
    @0x0505  Return(IntConst(0))

fn export#2 @ 0x050b  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x050c  Return(IntConst(0))

fn export#3 @ 0x0512  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0513  Return(IntConst(0))

fn export#4 @ 0x0519  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x051a  Return(IntConst(0))

fn export#5 @ 0x0520  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x0521  Return(IntConst(0))

fn export#6 @ 0x0527  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x0528  Return(IntConst(0))

fn export#7 @ 0x052e  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x052f  Return(IntConst(0))

fn export#8 @ 0x0535  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x0536  Return(IntConst(0))

fn export#9 @ 0x053c  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=0
    @0x053d  Return(IntConst(0))

fn export#10 @ 0x0543  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0544  Return(IntConst(0))
