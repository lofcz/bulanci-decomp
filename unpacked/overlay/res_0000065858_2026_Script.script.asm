
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  SetGlobalVar(1, IntConst(4))
    @0x0008  SetGlobalVar(2, StrConst("A0A144D0-26B0-4049-A66F-7AE630965103"))
    @0x0055  IfEqual(GetLocalVar(0), IntConst(1), 122)
    @0x0061  SetGlobalVar(0, StrConst("Malaria"))
    @0x0074  Return(IntConst(0))
    @0x007a  SetGlobalVar(0, StrConst("Malárie"))
    @0x008d  Return(IntConst(0))

fn export#1 @ 0x0093  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x0094  RegisterTimer(IntConst(0), Add(Mul(Rand(IntConst(0), IntConst(10)), IntConst(1000)), IntConst(5000)), IntConst(6))
    @0x00b6  LoadPreface(65640)
    @0x00bb  SetMusic(65869, 0)
    @0x00c4  SetInsertMode(IntConst(0))
    @0x00ca  InsertView(CreateImage(IntConst(0), IntConst(0), 65639))
    @0x00da  SetInsertMode(IntConst(2))
    @0x00e0  InsertView(CreateImage(IntConst(432), IntConst(61), 65627))
    @0x00f0  InsertView(BindToSlot(CreateAnim(IntConst(492), IntConst(0), delay=0, frames=[65782]), IntConst(6)))
    @0x0108  InsertView(BindToSlot(CreateAnim(IntConst(0), IntConst(0), delay=0, frames=[65787]), IntConst(7)))
    @0x0120  InsertView(BindToSlot(CreateAnim(IntConst(280), IntConst(0), delay=0, frames=[65784]), IntConst(8)))
    @0x0138  InsertView(BindToSlot(CreateAnim(IntConst(670), IntConst(390), delay=0, frames=[65785]), IntConst(9)))
    @0x0150  InsertView(BindToSlot(CreateAnim(IntConst(273), IntConst(208), delay=0, frames=[65788]), IntConst(10)))
    @0x0168  InsertView(BindToSlot(CreateAnim(IntConst(235), IntConst(88), delay=0, frames=[65789]), IntConst(11)))
    @0x0180  InsertView(BindToSlot(CreateAnim(IntConst(690), IntConst(320), delay=0, frames=[65783]), IntConst(12)))
    @0x0198  InsertView(CreateObstacle(IntConst(334), IntConst(376), IntConst(395), IntConst(445)))
    @0x01ae  InsertView(CreateObstacle(IntConst(395), IntConst(354), IntConst(454), IntConst(406)))
    @0x01c4  InsertView(CreateObstacle(IntConst(120), IntConst(0), IntConst(312), IntConst(77)))
    @0x01da  InsertView(CreateObstacle(IntConst(250), IntConst(77), IntConst(325), IntConst(108)))
    @0x01f0  InsertView(CreateObstacle(IntConst(290), IntConst(108), IntConst(353), IntConst(137)))
    @0x0206  InsertView(CreateObstacle(IntConst(586), IntConst(237), IntConst(651), IntConst(284)))
    @0x021c  InsertView(CreateObstacle(IntConst(313), IntConst(137), IntConst(352), IntConst(175)))
    @0x0232  InsertView(CreateImage(IntConst(0), IntConst(0), 65628))
    @0x0242  InsertView(BindToSlot(CreateAnim(IntConst(0), IntConst(236), delay=0, frames=[65786]), IntConst(13)))
    @0x025a  InsertBulanci()
    @0x025b  Return(IntConst(0))

fn export#2 @ 0x0261  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x0262  Return(IntConst(0))

fn export#3 @ 0x026f  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0270  Return(IntConst(0))

fn export#4 @ 0x0276  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x0277  Return(IntConst(0))

fn export#5 @ 0x027d  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x027e  Return(IntConst(0))

fn export#6 @ 0x0284  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x0285  PlayAnim(GetSlot(IntConst(6)), IntConst(0))
    @0x0291  PlayAnim(GetSlot(IntConst(7)), IntConst(0))
    @0x029d  PlayAnim(GetSlot(IntConst(8)), IntConst(0))
    @0x02a9  PlayAnim(GetSlot(IntConst(9)), IntConst(0))
    @0x02b5  PlayAnim(GetSlot(IntConst(10)), IntConst(0))
    @0x02c1  PlayAnim(GetSlot(IntConst(11)), IntConst(0))
    @0x02cd  PlayAnim(GetSlot(IntConst(12)), IntConst(0))
    @0x02d9  PlayAnim(GetSlot(IntConst(13)), IntConst(0))
    @0x02e5  Return(IntConst(0))

fn export#7 @ 0x02eb  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x02ec  Return(IntConst(0))

fn export#8 @ 0x02f2  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x02f3  Return(IntConst(0))

fn export#9 @ 0x02f9  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=0
    @0x02fa  Return(IntConst(0))

fn export#10 @ 0x0268  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0269  Return(IntConst(0))
