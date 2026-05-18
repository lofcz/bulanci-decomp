using System.Collections;
using System.IO;

namespace Editor.Scripts;

public class ParamCommand : Command
{
	private Command[] _value;

	public ParamCommand(Opcode opcode, params Command[] val)
		: base(opcode)
	{
		_value = val;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		Command[] value = _value;
		for (int i = 0; i < value.Length; i++)
		{
			value[i].WriteCode(writer);
		}
	}

	protected override ArrayList SubCommands()
	{
		return new ArrayList(_value);
	}
}
