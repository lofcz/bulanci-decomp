
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  SetGlobalVar(1, IntConst(2))
    @0x0008  SetGlobalVar(2, StrConst("6A080067-362A-478d-B9EC-68F54424001B"))
    @0x0055  IfEqual(GetLocalVar(0), IntConst(1), 132)
    @0x0061  SetGlobalVar(0, StrConst("Dj Slaughter"))
    @0x007e  Return(IntConst(0))
    @0x0084  SetGlobalVar(0, StrConst("Vybíjená"))
    @0x0099  Return(IntConst(0))

fn export#1 @ 0x009f  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x00a0  LoadPreface(65847)
    @0x00a5  SetMusic(65861, 0)
    @0x00ae  SetInsertMode(IntConst(0))
    @0x00b4  InsertView(CreateImage(IntConst(0), IntConst(0), 65846))
    @0x00c4  InsertView(CreateAnim(IntConst(448), IntConst(81), delay=3, frames=[65752]))
    @0x00d6  InsertView(CreateAnim(IntConst(463), IntConst(63), delay=3, frames=[65753]))
    @0x00e8  SetInsertMode(IntConst(2))
    @0x00ee  InsertView(CreateObstacle(IntConst(0), IntConst(0), IntConst(800), IntConst(160)))
    @0x0104  InsertView(CreateObstacle(IntConst(0), IntConst(160), IntConst(40), IntConst(233)))
    @0x011a  InsertView(CreateObstacle(IntConst(0), IntConst(233), IntConst(20), IntConst(310)))
    @0x0130  InsertView(CreateObstacle(IntConst(670), IntConst(160), IntConst(800), IntConst(180)))
    @0x0146  InsertView(CreateObstacle(IntConst(760), IntConst(180), IntConst(800), IntConst(300)))
    @0x015c  InsertView(CreateObstacle(IntConst(775), IntConst(300), IntConst(800), IntConst(365)))
    @0x0172  SetInsertMode(IntConst(1))
    @0x0178  InsertBulanci()
    @0x0179  SetInsertMode(IntConst(2))
    @0x017f  InsertView(CreateImage(IntConst(0), IntConst(0), 65557))
    @0x018f  Return(IntConst(0))

fn export#2 @ 0x0195  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x0196  Return(IntConst(0))

fn export#3 @ 0x01a3  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x01a4  Return(IntConst(0))

fn export#4 @ 0x01aa  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x01ab  Return(IntConst(0))

fn export#5 @ 0x01b1  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x01b2  Return(IntConst(0))

fn export#6 @ 0x01b8  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x01b9  Return(IntConst(0))

fn export#7 @ 0x01bf  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x01c0  Return(IntConst(0))

fn export#8 @ 0x01c6  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x01c7  Return(IntConst(0))

fn export#9 @ 0x01cd  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=0
    @0x01ce  Return(IntConst(0))

fn export#10 @ 0x019c  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x019d  Return(IntConst(0))
