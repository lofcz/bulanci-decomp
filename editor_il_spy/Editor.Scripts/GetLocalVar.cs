using System;
using System.IO;

namespace Editor.Scripts;

public class GetLocalVar : Command
{
	private string _variable;

	public GetLocalVar(string variable)
		: base(Opcode.GetLocalVar)
	{
		_variable = variable;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		byte b = base.Function.FindVariable(_variable);
		if (b == byte.MaxValue)
		{
			throw new Exception("Local variable '" + _variable + "' not found.");
		}
		base.WriteCode(writer);
		writer.Write(b);
	}
}
