namespace Editor.Temp;

public enum EventType
{
	Nothing = 0,
	MouseDown = 1,
	MouseUp = 2,
	MouseMove = 4,
	KeyDown = 128,
	KeyUp = 256,
	Command = 1024,
	Broadcast = 2048,
	Mouse = 7,
	Keyboard = 384,
	Positional = 7,
	Focused = 1408
}
