using System.Collections;
using System.IO;

namespace Editor.Scripts;

public class Command
{
	private Opcode _opcode;

	private Function _function;

	public Function Function
	{
		get
		{
			return _function;
		}
		set
		{
			_function = value;
			ArrayList arrayList = SubCommands();
			if (arrayList == null)
			{
				return;
			}
			foreach (Command item in arrayList)
			{
				item.Function = value;
			}
		}
	}

	public Opcode Opcode => _opcode;

	public virtual void WriteCode(BinaryWriter writer)
	{
		writer.Write((byte)Opcode);
	}

	protected void WriteLabel(BinaryWriter writer, string name)
	{
		Function.FindLabel(name).WriteCode(writer);
	}

	protected virtual ArrayList SubCommands()
	{
		return null;
	}

	protected Command(Opcode opcode)
	{
		_opcode = opcode;
	}
}
