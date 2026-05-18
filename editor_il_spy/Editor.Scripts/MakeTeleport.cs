namespace Editor.Scripts;

public sealed class MakeTeleport : Call
{
	private const string Name = "MakeTeleport";

	public MakeTeleport(Command player, Command x, Command y)
		: base("MakeTeleport", player, x, y)
	{
	}

	public MakeTeleport(Command player, int x, int y)
		: base("MakeTeleport", player, new IntConst(x), new IntConst(y))
	{
	}

	public static Function Generate()
	{
		Function function = new Function("MakeTeleport");
		function.DeclareParameters("p", "x", "y");
		function.InsertCommand(new IfEqual(new TeleportPlayerTo(new GetLocalVar("p"), new GetLocalVar("x"), new GetLocalVar("y"), always: false), 0, "notteleported"));
		function.InsertCommand(new IfEqual(new IsNet(), 0, "notnet"));
		function.InsertCommand(new StrmSetSize(new GetGlobalVar("strm")));
		function.InsertCommand(new StrmWrite(new GetGlobalVar("strm"), new IntConst(1), 1));
		function.InsertCommand(new StrmWrite(new GetGlobalVar("strm"), new GetLocalVar("p"), 1));
		function.InsertCommand(new StrmWrite(new GetGlobalVar("strm"), new GetLocalVar("x"), 4));
		function.InsertCommand(new StrmWrite(new GetGlobalVar("strm"), new GetLocalVar("y"), 4));
		function.InsertCommand(new StrmSend(new GetGlobalVar("strm")));
		function.SetLabel("notnet");
		function.InsertCommand(new Return(new IntConst(1)));
		function.SetLabel("notteleported");
		function.InsertCommand(new Return(new IntConst(0)));
		return function;
	}
}
