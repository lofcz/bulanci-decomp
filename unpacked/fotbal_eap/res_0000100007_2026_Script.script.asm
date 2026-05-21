
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  SetGlobalVar(1, IntConst(1000))
    @0x0008  SetGlobalVar(2, StrConst("C53FADA7-B403-4BEE-9E34-7A3DCA3D078A"))
    @0x0055  SetGlobalVar(0, StrConst("Fotbal"))
    @0x0066  Return(IntConst(0))

fn export#1 @ 0x006c  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x006d  LoadPreface(100001)
    @0x0072  SetMusic(100003, 0)
    @0x007b  SetInsertMode(IntConst(0))
    @0x0081  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x0091  InsertView(CreateObstacle(IntConst(556), IntConst(0), IntConst(800), IntConst(93)))
    @0x00a7  InsertView(CreateObstacle(IntConst(0), IntConst(0), IntConst(478), IntConst(93)))
    @0x00bd  InsertView(EnableFireThrough(CreateObstacle(IntConst(701), IntConst(254), IntConst(739), IntConst(312)), IntConst(1)))
    @0x00d9  InsertView(EnableFireThrough(CreateObstacle(IntConst(55), IntConst(247), IntConst(93), IntConst(304)), IntConst(1)))
    @0x00f5  SetInsertMode(IntConst(2))
    @0x00fb  SetInsertMode(IntConst(1))
    @0x0101  InsertView(SetOrderAxis(CreateImage(IntConst(557), IntConst(0), 100006), IntConst(0)))
    @0x0117  InsertView(SetOrderAxis(CreateImage(IntConst(0), IntConst(0), 100006), IntConst(0)))
    @0x012d  InsertView(SetOrderAxis(CreateImage(IntConst(235), IntConst(0), 100006), IntConst(0)))
    @0x0143  InsertView(SetOrderAxis(CreateImage(IntConst(701), IntConst(254), 100005), IntConst(0)))
    @0x0159  InsertView(SetOrderAxis(CreateImage(IntConst(55), IntConst(246), 100004), IntConst(0)))
    @0x016f  SetInsertMode(IntConst(0))
    @0x0175  InsertBulanci()
    @0x0176  Return(IntConst(0))

fn export#2 @ 0x017c  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x017d  Return(IntConst(0))

fn export#3 @ 0x0183  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0184  Return(IntConst(0))

fn export#4 @ 0x018a  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x018b  Return(IntConst(0))

fn export#5 @ 0x0191  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x0192  Return(IntConst(0))

fn export#6 @ 0x0198  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x0199  Return(IntConst(0))

fn export#7 @ 0x019f  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x01a0  Return(IntConst(0))

fn export#8 @ 0x01a6  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x01a7  Return(IntConst(0))

fn export#9 @ 0x01ad  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=0
    @0x01ae  Return(IntConst(0))

fn export#10 @ 0x01b4  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x01b5  Return(IntConst(0))
