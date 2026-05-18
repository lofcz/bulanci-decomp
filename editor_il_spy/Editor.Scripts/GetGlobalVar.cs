using System;
using System.IO;

namespace Editor.Scripts;

public sealed class GetGlobalVar : Command
{
	private string _variable;

	public GetGlobalVar(string variable)
		: base(Opcode.GetGlobalVar)
	{
		_variable = variable;
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
	}
}
