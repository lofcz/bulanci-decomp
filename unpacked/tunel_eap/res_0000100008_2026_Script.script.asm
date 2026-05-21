
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  SetGlobalVar(1, IntConst(1000))
    @0x0008  SetGlobalVar(2, StrConst("34CC43A7-83E3-4552-8DD8-2FF457F06808"))
    @0x0055  SetGlobalVar(0, StrConst("Tunel"))
    @0x0064  Return(IntConst(0))

fn export#1 @ 0x006a  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x006b  LoadPreface(100001)
    @0x0070  SetMusic(100003, 0)
    @0x0079  SetInsertMode(IntConst(0))
    @0x007f  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x008f  InsertView(CreateObstacle(IntConst(460), IntConst(301), IntConst(797), IntConst(320)))
    @0x00a5  InsertView(CreateObstacle(IntConst(0), IntConst(302), IntConst(289), IntConst(321)))
    @0x00bb  InsertView(CreateObstacle(IntConst(440), IntConst(309), IntConst(466), IntConst(374)))
    @0x00d1  InsertView(CreateObstacle(IntConst(443), IntConst(132), IntConst(456), IntConst(209)))
    @0x00e7  InsertView(CreateObstacle(IntConst(294), IntConst(135), IntConst(312), IntConst(178)))
    @0x00fd  InsertView(CreateObstacle(IntConst(284), IntConst(177), IntConst(313), IntConst(374)))
    @0x0113  InsertView(CreateObstacle(IntConst(443), IntConst(207), IntConst(800), IntConst(309)))
    @0x0129  InsertView(CreateObstacle(IntConst(0), IntConst(207), IntConst(307), IntConst(309)))
    @0x013f  SetInsertMode(IntConst(2))
    @0x0145  InsertView(CreateImage(IntConst(583), IntConst(0), 100007))
    @0x0155  InsertView(CreateImage(IntConst(0), IntConst(219), 100006))
    @0x0165  SetInsertMode(IntConst(1))
    @0x016b  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(88), 100004), IntConst(0)))
    @0x0181  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(262), 100005), IntConst(0)))
    @0x0197  SetInsertMode(IntConst(0))
    @0x019d  InsertBulanci()
    @0x019e  Return(IntConst(0))

fn export#2 @ 0x01a4  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x01a5  Return(IntConst(0))

fn export#3 @ 0x01ab  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x01ac  Return(IntConst(0))

fn export#4 @ 0x01b2  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x01b3  Return(IntConst(0))

fn export#5 @ 0x01b9  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x01ba  Return(IntConst(0))

fn export#6 @ 0x01c0  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x01c1  Return(IntConst(0))

fn export#7 @ 0x01c7  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x01c8  Return(IntConst(0))

fn export#8 @ 0x01ce  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x01cf  Return(IntConst(0))

fn export#9 @ 0x01d5  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=0
    @0x01d6  Return(IntConst(0))

fn export#10 @ 0x01dc  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x01dd  Return(IntConst(0))
