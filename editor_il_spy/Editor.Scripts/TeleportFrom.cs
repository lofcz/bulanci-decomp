namespace Editor.Scripts;

public sealed class TeleportFrom : Call
{
	private const string Name = "TeleportFrom";

	public TeleportFrom(int id, Command player)
		: base("TeleportFrom" + id, player)
	{
	}

	public static Function Generate(TeleportLevelItem t)
	{
		Function function = new Function("TeleportFrom" + t.TraceAreaID);
		function.DeclareParameter("player");
		function.DeclareVariables("m", "n");
		function.InsertCommand(new SetLocalVar("m", new Rand(0, t.Targets.Count - 1)));
		function.InsertCommand(new SetLocalVar("n", new GetLocalVar("m")));
		function.SetLabel("tryteleport");
		Switch obj = new Switch(new GetLocalVar("n"));
		function.InsertCommand(obj);
		for (int i = 0; i < t.Targets.Count; i++)
		{
			TeleportLevelItem teleportLevelItem = t.Level.GetItemByGuid((string)t.Targets[i]) as TeleportLevelItem;
			string label = "teleportto" + i;
			obj.AddItem(i, label);
			function.SetLabel(label);
			function.InsertCommand(new IfEqual(new MakeTeleport(new GetLocalVar("player"), teleportLevelItem.TeleportTo.X, teleportLevelItem.TeleportTo.Y), 1, "teleported"));
			function.InsertCommand(new Goto("notteleported"));
		}
		function.SetLabel("notteleported");
		function.InsertCommand(new SetLocalVar("n", new Add(new GetLocalVar("n"), 1)));
		function.InsertCommand(new IfNotEqual(new GetLocalVar("n"), t.Targets.Count, "incdone"));
		function.InsertCommand(new SetLocalVar("n", 0));
		function.SetLabel("incdone");
		function.InsertCommand(new IfNotEqual(new GetLocalVar("n"), new GetLocalVar("m"), "tryteleport"));
		function.InsertCommand(new Return(new IntConst(0)));
		function.SetLabel("teleported");
		function.InsertCommand(new Return(new IntConst(1)));
		return function;
	}
}
