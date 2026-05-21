
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  SetGlobalVar(1, IntConst(1000))
    @0x0008  SetGlobalVar(2, StrConst("8DCFCC56-35BC-4141-82F9-B46CB402D7C8"))
    @0x0055  SetGlobalVar(0, StrConst("Srdeční záležitost"))
    @0x007e  Return(IntConst(0))

fn export#1 @ 0x0084  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x0085  LoadPreface(100001)
    @0x008a  SetMusic(100003, 0)
    @0x0093  SetInsertMode(IntConst(0))
    @0x0099  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x00a9  InsertView(CreateObstacle(IntConst(749), IntConst(443), IntConst(799), IntConst(468)))
    @0x00bf  InsertView(CreateObstacle(IntConst(0), IntConst(205), IntConst(64), IntConst(220)))
    @0x00d5  InsertView(CreateObstacle(IntConst(64), IntConst(22), IntConst(103), IntConst(56)))
    @0x00eb  InsertView(CreateObstacle(IntConst(100), IntConst(472), IntConst(188), IntConst(515)))
    @0x0101  InsertView(CreateObstacle(IntConst(375), IntConst(469), IntConst(469), IntConst(515)))
    @0x0117  InsertView(CreateObstacle(IntConst(549), IntConst(467), IntConst(635), IntConst(515)))
    @0x012d  InsertView(CreateObstacle(IntConst(708), IntConst(39), IntConst(722), IntConst(68)))
    @0x0143  InsertView(CreateObstacle(IntConst(720), IntConst(0), IntConst(800), IntConst(70)))
    @0x0159  InsertView(CreateObstacle(IntConst(437), IntConst(34), IntConst(526), IntConst(68)))
    @0x016f  InsertView(CreateObstacle(IntConst(451), IntConst(0), IntConst(518), IntConst(33)))
    @0x0185  InsertView(CreateObstacle(IntConst(620), IntConst(0), IntConst(646), IntConst(186)))
    @0x019b  InsertView(CreateObstacle(IntConst(630), IntConst(204), IntConst(727), IntConst(227)))
    @0x01b1  InsertView(CreateObstacle(IntConst(548), IntConst(181), IntConst(646), IntConst(205)))
    @0x01c7  InsertView(CreateObstacle(IntConst(707), IntConst(318), IntConst(800), IntConst(384)))
    @0x01dd  InsertView(CreateObstacle(IntConst(542), IntConst(320), IntConst(635), IntConst(342)))
    @0x01f3  InsertView(CreateObstacle(IntConst(451), IntConst(277), IntConst(544), IntConst(343)))
    @0x0209  InsertView(CreateObstacle(IntConst(352), IntConst(293), IntConst(460), IntConst(315)))
    @0x021f  InsertView(CreateObstacle(IntConst(351), IntConst(291), IntConst(377), IntConst(515)))
    @0x0235  InsertView(CreateObstacle(IntConst(181), IntConst(291), IntConst(280), IntConst(315)))
    @0x024b  InsertView(CreateObstacle(IntConst(370), IntConst(182), IntConst(473), IntConst(205)))
    @0x0261  InsertView(CreateObstacle(IntConst(180), IntConst(142), IntConst(279), IntConst(163)))
    @0x0277  InsertView(CreateObstacle(IntConst(254), IntConst(62), IntConst(278), IntConst(164)))
    @0x028d  InsertView(CreateObstacle(IntConst(280), IntConst(139), IntConst(373), IntConst(205)))
    @0x02a3  InsertView(CreateObstacle(IntConst(0), IntConst(289), IntConst(181), IntConst(354)))
    @0x02b9  InsertView(CreateObstacle(IntConst(0), IntConst(139), IntConst(180), IntConst(205)))
    @0x02cf  SetInsertMode(IntConst(2))
    @0x02d5  InsertView(CreateImage(IntConst(723), IntConst(0), 100013))
    @0x02e5  SetInsertMode(IntConst(1))
    @0x02eb  InsertView(SetOrderAxis(CreateImage(IntConst(746), IntConst(425), 100011), IntConst(0)))
    @0x0301  InsertView(SetOrderAxis(CreateImage(IntConst(545), IntConst(446), 100010), IntConst(0)))
    @0x0317  InsertView(SetOrderAxis(CreateImage(IntConst(436), IntConst(0), 100010), IntConst(0)))
    @0x032d  InsertView(SetOrderAxis(CreateImage(IntConst(66), IntConst(24), 100012), IntConst(0)))
    @0x0343  InsertView(SetOrderAxis(CreateImage(IntConst(97), IntConst(446), 100010), IntConst(0)))
    @0x0359  InsertView(SetOrderAxis(CreateImage(IntConst(708), IntConst(0), 100010), IntConst(0)))
    @0x036f  InsertView(SetOrderAxis(CreateImage(IntConst(377), IntConst(446), 100010), IntConst(0)))
    @0x0385  InsertView(SetOrderAxis(CreateImage(IntConst(645), IntConst(183), 100009), IntConst(0)))
    @0x039b  InsertView(SetOrderAxis(CreateImage(IntConst(182), IntConst(317), 100009), IntConst(0)))
    @0x03b1  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(205), 100008), IntConst(0)))
    @0x03c7  InsertView(SetOrderAxis(CreateImage(IntConst(629), IntConst(203), 100005), IntConst(0)))
    @0x03dd  InsertView(SetOrderAxis(CreateImage(IntConst(707), IntConst(318), 100004), IntConst(0)))
    @0x03f3  InsertView(SetOrderAxis(CreateImage(IntConst(451), IntConst(277), 100004), IntConst(0)))
    @0x0409  InsertView(SetOrderAxis(CreateImage(IntConst(537), IntConst(320), 100005), IntConst(0)))
    @0x041f  InsertView(SetOrderAxis(CreateImage(IntConst(183), IntConst(139), 100005), IntConst(0)))
    @0x0435  InsertView(SetOrderAxis(CreateImage(IntConst(621), IntConst(83), 100007), IntConst(0)))
    @0x044b  InsertView(SetOrderAxis(CreateImage(IntConst(254), IntConst(60), 100007), IntConst(0)))
    @0x0461  InsertView(SetOrderAxis(CreateImage(IntConst(352), IntConst(415), 100007), IntConst(0)))
    @0x0477  InsertView(SetOrderAxis(CreateImage(IntConst(621), IntConst(0), 100007), IntConst(0)))
    @0x048d  InsertView(SetOrderAxis(CreateImage(IntConst(352), IntConst(315), 100007), IntConst(0)))
    @0x04a3  InsertView(SetOrderAxis(CreateImage(IntConst(547), IntConst(181), 100005), IntConst(0)))
    @0x04b9  InsertView(SetOrderAxis(CreateImage(IntConst(375), IntConst(182), 100005), IntConst(0)))
    @0x04cf  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(84), 100006), IntConst(0)))
    @0x04e5  InsertView(SetOrderAxis(CreateImage(IntConst(280), IntConst(139), 100004), IntConst(0)))
    @0x04fb  InsertView(SetOrderAxis(CreateImage(IntConst(351), IntConst(291), 100005), IntConst(0)))
    @0x0511  InsertView(SetOrderAxis(CreateImage(IntConst(181), IntConst(292), 100005), IntConst(0)))
    @0x0527  InsertView(SetOrderAxis(CreateImage(IntConst(88), IntConst(290), 100004), IntConst(0)))
    @0x053d  InsertView(SetOrderAxis(CreateImage(IntConst(88), IntConst(139), 100004), IntConst(0)))
    @0x0553  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(289), 100004), IntConst(0)))
    @0x0569  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(139), 100004), IntConst(0)))
    @0x057f  SetInsertMode(IntConst(0))
    @0x0585  InsertBulanci()
    @0x0586  Return(IntConst(0))

fn export#2 @ 0x058c  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x058d  Return(IntConst(0))

fn export#3 @ 0x0593  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0594  Return(IntConst(0))

fn export#4 @ 0x059a  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x059b  Return(IntConst(0))

fn export#5 @ 0x05a1  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x05a2  Return(IntConst(0))

fn export#6 @ 0x05a8  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x05a9  Return(IntConst(0))

fn export#7 @ 0x05af  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x05b0  Return(IntConst(0))

fn export#8 @ 0x05b6  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x05b7  Return(IntConst(0))

fn export#9 @ 0x05bd  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=0
    @0x05be  Return(IntConst(0))

fn export#10 @ 0x05c4  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x05c5  Return(IntConst(0))
