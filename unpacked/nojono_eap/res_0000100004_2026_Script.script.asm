
fn export#0 @ 0x0064  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0065  SetGlobalVar(1, IntConst(1000))
    @0x006c  SetGlobalVar(2, StrConst("9B98D1B9-3B41-4772-9FEB-943FBEC4FD66"))
    @0x00b9  SetGlobalVar(0, StrConst("No jo no"))
    @0x00ce  Return(IntConst(0))

fn export#1 @ 0x00d4  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x00d5  SetGlobalVar(3, IntConst(0))
    @0x00dc  IfEqual(IsNet(), IntConst(0), 247)
    @0x00e7  SetGlobalVar(3, StrmCreateMem(IntConst(4096), IntConst(4096)))
    @0x00f4  SetCommStrm(GetGlobalVar(3))
    @0x00f7  LoadPreface(100001)
    @0x00fc  SetMusic(100003, 0)
    @0x0105  SetInsertMode(IntConst(0))
    @0x010b  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
    @0x011b  InsertView(CreateObstacle(IntConst(461), IntConst(409), IntConst(480), IntConst(499)))
    @0x0131  InsertView(CreateObstacle(IntConst(461), IntConst(391), IntConst(675), IntConst(409)))
    @0x0147  InsertView(CreateObstacle(IntConst(696), IntConst(337), IntConst(784), IntConst(355)))
    @0x015d  InsertView(CreateObstacle(IntConst(677), IntConst(337), IntConst(696), IntConst(410)))
    @0x0173  InsertView(CreateObstacle(IntConst(353), IntConst(248), IntConst(372), IntConst(428)))
    @0x0189  InsertView(CreateObstacle(IntConst(695), IntConst(121), IntConst(785), IntConst(140)))
    @0x019f  InsertView(CreateObstacle(IntConst(245), IntConst(229), IntConst(785), IntConst(248)))
    @0x01b5  InsertView(CreateObstacle(IntConst(785), IntConst(14), IntConst(800), IntConst(499)))
    @0x01cb  InsertView(CreateObstacle(IntConst(245), IntConst(319), IntConst(264), IntConst(499)))
    @0x01e1  InsertView(CreateObstacle(IntConst(137), IntConst(140), IntConst(156), IntConst(410)))
    @0x01f7  InsertView(CreateObstacle(IntConst(137), IntConst(121), IntConst(588), IntConst(140)))
    @0x020d  InsertView(CreateObstacle(IntConst(12), IntConst(0), IntConst(800), IntConst(14)))
    @0x0223  InsertView(CreateObstacle(IntConst(12), IntConst(499), IntConst(800), IntConst(515)))
    @0x0239  InsertView(CreateObstacle(IntConst(0), IntConst(0), IntConst(12), IntConst(515)))
    @0x024f  SetInsertMode(IntConst(2))
    @0x0255  SetInsertMode(IntConst(1))
    @0x025b  SetInsertMode(IntConst(0))
    @0x0261  IfEqual(IsServer(), IntConst(0), 744)
    @0x026c  DefineTraceArea(IntConst(0), IntConst(484), IntConst(419), IntConst(564), IntConst(489), IntConst(14))
    @0x028b  DefineTraceArea(IntConst(1), IntConst(701), IntConst(257), IntConst(781), IntConst(327), IntConst(14))
    @0x02aa  DefineTraceArea(IntConst(2), IntConst(701), IntConst(149), IntConst(781), IntConst(219), IntConst(14))
    @0x02c9  DefineTraceArea(IntConst(3), IntConst(17), IntConst(23), IntConst(97), IntConst(93), IntConst(14))
    @0x02e8  InsertBulanci()
    @0x02e9  Return(IntConst(0))

fn export#2 @ 0x02ef  ; OnDeinit()  -- fires once during CGaming destruction; resource teardown
  ; varCount=0
    @0x02f0  IfEqual(GetGlobalVar(3), IntConst(0), 769)
    @0x02fc  SetGlobalVar(3, StrmDestroy(GetGlobalVar(3)))
    @0x0301  Return(IntConst(0))

fn export#3 @ 0x0307  ; OnBitmapEvt(slot, evt)  -- fires when a CBitmap sub-view emits an event (animation frame end, click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code
  ; varCount=0
    @0x0308  Return(IntConst(0))

fn export#4 @ 0x030e  ; OnSlotPlaced(slot, msgHi, msgLoBits)  -- fires when CExplosion / net-msg-0x0f spawns or moves a slot via `FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, msgHi/msgLo = the upper/lower halves of the spawn parameter
  ; varCount=0
    @0x030f  Return(IntConst(0))

fn export#5 @ 0x0315  ; OnSlotDisplaced(slot, byParam)  -- companion to OnSlotPlaced: fires on a *second* slot when its occupant gets displaced by a OnSlotPlaced event (msg 0xd8). slot = the displaced slot, byParam = the slot that displaced it
  ; varCount=0
    @0x0316  Return(IntConst(0))

fn export#6 @ 0x031c  ; OnTimer(slotId)  -- fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes 68..72; timer table at `this+0x440`) expires; the slot id passed back is the same slotId originally registered
  ; varCount=0
    @0x031d  Return(IntConst(0))

fn export#7 @ 0x0323  ; OnEnter(traceId, entitySlot)  -- fires when a player/entity *enters* a `DefineTraceArea` rectangle. traceId = the first arg passed to DefineTraceArea, entitySlot = the 0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary
  ; varCount=0
    @0x0324  Switch(GetLocalVar(0); 0=>0x348, 1=>0x35f, 2=>0x376, 3=>0x38d)
    @0x0348  Call(fn@0x0, GetLocalVar(1), IntConst(31), IntConst(39))
    @0x035a  Goto(932)
    @0x035f  Call(fn@0x0, GetLocalVar(1), IntConst(498), IntConst(435))
    @0x0371  Goto(932)
    @0x0376  Call(fn@0x0, GetLocalVar(1), IntConst(715), IntConst(273))
    @0x0388  Goto(932)
    @0x038d  Call(fn@0x0, GetLocalVar(1), IntConst(715), IntConst(165))
    @0x039f  Goto(932)
    @0x03a4  Return(IntConst(0))

fn export#8 @ 0x03aa  ; OnLeave(traceId, entitySlot)  -- fires when a player/entity *leaves* a `DefineTraceArea` rectangle (symmetric to OnEnter, same argument shape)
  ; varCount=0
    @0x03ab  Switch(GetLocalVar(0); 0=>0x3cf, 1=>0x3d4, 2=>0x3d9, 3=>0x3de)
    @0x03cf  Goto(995)
    @0x03d4  Goto(995)
    @0x03d9  Goto(995)
    @0x03de  Goto(995)
    @0x03e3  Return(IntConst(0))

fn export#9 @ 0x03e9  ; OnNetCustom(streamHandle)  -- fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. Scripts typically `StrmRead(handle, 1)` the leading opcode byte and switch on it to dispatch their own RPC sub-protocol
  ; varCount=4
    @0x03ea  SetLocalVar(0, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x03f4  IfNotEqual(GetLocalVar(0), IntConst(1), 1066)
    @0x0400  SetLocalVar(3, StrmRead(GetLocalVar(4), IntConst(1)))
    @0x040a  SetLocalVar(1, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x0414  SetLocalVar(2, StrmRead(GetLocalVar(4), IntConst(4)))
    @0x041e  TeleportPlayerTo(GetLocalVar(3), GetLocalVar(1), GetLocalVar(2), IntConst(1))
    @0x042a  Return(IntConst(0))

fn export#10 @ 0x0430  ; OnGameStart()  -- fires from `CGaming::FUN_0041c140(true)` whenever the level transitions from paused/loaded to running (level start, post-pause resume). Music is started and engine-side timer slots 1/2 are armed right after this returns
  ; varCount=0
    @0x0431  Return(IntConst(0))

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
