using System;
using System.Collections;
using System.IO;

namespace Editor.Scripts;

public sealed class SetGlobalVar : Command
{
	private Command _value;

	private string _variable;

	public SetGlobalVar(string var, Command v)
		: base(Opcode.SetGlobalVar)
	{
		_variable = var;
		_value = v;
	}

	public SetGlobalVar(string var, string v)
		: base(Opcode.SetGlobalVar)
	{
		_variable = var;
		_value = new StrConst(v);
	}

	public SetGlobalVar(string var, int v)
		: base(Opcode.SetGlobalVar)
	{
		_variable = var;
		_value = new IntConst(v);
	}

	public override void WriteCode(BinaryWriter writer)
	{
		byte b = base.Function.Script.FindVariable(_variable);
		if (b == byte.MaxValue)
		{
			throw new Exception("Global variable '" + _variable + "' not found.");
		}
		base.WriteCode(writer);
		writer.Write(b);
		_value.WriteCode(writer);
	}

	protected override ArrayList SubCommands()
	{
		return new ArrayList(new Command[1] { _value });
	}
}
