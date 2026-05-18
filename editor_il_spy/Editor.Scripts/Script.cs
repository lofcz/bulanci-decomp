using System;
using System.Collections;
using System.IO;

namespace Editor.Scripts;

public class Script : ResourceItem
{
	private ArrayList _listVars = new ArrayList();

	private ArrayList _listFunctions = new ArrayList();

	public override uint ClassID => 2026u;

	public override void Write(BinaryWriter writer)
	{
		MemoryStream memoryStream = new MemoryStream(65536);
		BinaryWriter binaryWriter = new BinaryWriter(memoryStream);
		int[] array = new int[_listFunctions.Count];
		int num = 0;
		int num2 = 0;
		foreach (Function listFunction in _listFunctions)
		{
			if (listFunction.Exported)
			{
				array[num++] = num2;
			}
			listFunction.Position = num2;
			listFunction.WriteCode(binaryWriter);
			num2 = (int)binaryWriter.BaseStream.Position;
		}
		foreach (Function listFunction2 in _listFunctions)
		{
			listFunction2.UpdateUsages(binaryWriter);
		}
		int num3 = (int)memoryStream.Length;
		writer.Write(num3);
		writer.Write(num);
		writer.Write(_listVars.Count);
		writer.Write(memoryStream.GetBuffer(), 0, num3);
		for (int i = 0; i < num; i++)
		{
			writer.Write(array[i]);
		}
	}

	public void DeclareVariable(string name)
	{
		_listVars.Add(name);
	}

	public void DeclareVariables(params string[] names)
	{
		foreach (string name in names)
		{
			DeclareVariable(name);
		}
	}

	public byte FindVariable(string name)
	{
		int count = _listVars.Count;
		while (count-- > 0)
		{
			if (_listVars[count].ToString() == name)
			{
				return (byte)count;
			}
		}
		return byte.MaxValue;
	}

	public Function FindFunction(string name)
	{
		foreach (Function listFunction in _listFunctions)
		{
			if (listFunction.Name == name)
			{
				return listFunction;
			}
		}
		throw new ArgumentException("Function '" + name + "' not found.");
	}

	public void InsertFunction(Function func)
	{
		func.Script = this;
		_listFunctions.Add(func);
	}
}
