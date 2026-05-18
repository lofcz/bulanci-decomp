using System.Collections;
using System.IO;

namespace Editor.Scripts;

public class Call : Command
{
	private string _name;

	private Command[] _values;

	public Command[] Values => _values;

	public Call(string name, params Command[] values)
		: base(Opcode.Call)
	{
		_name = name;
		_values = values;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		base.Function.Script.FindFunction(_name).WriteUsage(writer, this);
	}

	protected override ArrayList SubCommands()
	{
		return new ArrayList(Values);
	}
}
