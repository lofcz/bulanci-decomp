using System;
using System.Collections;
using System.IO;

namespace Editor.Scripts;

public class Function
{
	private bool _exported;

	private int _position;

	private string _name = "";

	private ArrayList _listVars = new ArrayList();

	private ArrayList _listParams = new ArrayList();

	private ArrayList _listCommands = new ArrayList();

	private Hashtable _labels = new Hashtable();

	private ArrayList _calls = new ArrayList();

	private Script _script;

	public Script Script
	{
		get
		{
			return _script;
		}
		set
		{
			_script = value;
		}
	}

	public int Position
	{
		get
		{
			return _position;
		}
		set
		{
			_position = value;
		}
	}

	public string Name
	{
		get
		{
			return _name;
		}
		set
		{
			_name = value;
		}
	}

	public bool Exported
	{
		get
		{
			return _exported;
		}
		set
		{
			_exported = value;
		}
	}

	public void DeclareVariable(string name)
	{
		_listVars.Add(name);
	}

	public void DeclareVariables(params string[] vars)
	{
		foreach (string name in vars)
		{
			DeclareVariable(name);
		}
	}

	public void DeclareParameter(string name)
	{
		_listParams.Add(name);
	}

	public void DeclareParameters(params string[] pars)
	{
		foreach (string name in pars)
		{
			DeclareParameter(name);
		}
	}

	public void InsertCommand(Command cmd)
	{
		cmd.Function = this;
		_listCommands.Add(cmd);
	}

	public void SetLabel(string name)
	{
		ArrayList arrayList = _labels[_listCommands.Count] as ArrayList;
		if (arrayList == null)
		{
			arrayList = (ArrayList)(_labels[_listCommands.Count] = new ArrayList());
		}
		arrayList.Add(new Label(name));
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
		count = _listParams.Count;
		while (count-- > 0)
		{
			if (_listParams[count].ToString() == name)
			{
				return (byte)(count + _listVars.Count);
			}
		}
		return byte.MaxValue;
	}

	public Label FindLabel(string name)
	{
		foreach (ArrayList value in _labels.Values)
		{
			foreach (Label item in value)
			{
				if (item.Name == name)
				{
					return item;
				}
			}
		}
		throw new ArgumentException("Label not found.");
	}

	public void WriteCode(BinaryWriter writer)
	{
		writer.Write((byte)_listVars.Count);
		for (int i = 0; i < _listCommands.Count; i++)
		{
			if (_labels[i] is ArrayList arrayList)
			{
				foreach (Label item in arrayList)
				{
					item.Position = (int)writer.BaseStream.Position;
				}
			}
			(_listCommands[i] as Command).WriteCode(writer);
		}
		foreach (ArrayList value in _labels.Values)
		{
			foreach (Label item2 in value)
			{
				item2.Update(writer);
			}
		}
		writer.BaseStream.Seek(0L, SeekOrigin.End);
	}

	public void WriteUsage(BinaryWriter writer, Call call)
	{
		if (call.Values.Length != _listParams.Count)
		{
			throw new InvalidOperationException("Invalid number of parameters in function call.");
		}
		_calls.Add((int)writer.BaseStream.Position);
		writer.Write(0);
		writer.Write((byte)call.Values.Length);
		Command[] values = call.Values;
		for (int i = 0; i < values.Length; i++)
		{
			values[i].WriteCode(writer);
		}
	}

	public void UpdateUsages(BinaryWriter writer)
	{
		foreach (int call in _calls)
		{
			writer.BaseStream.Seek(call, SeekOrigin.Begin);
			writer.Write(Position);
		}
		writer.BaseStream.Seek(0L, SeekOrigin.End);
	}

	public static Function EmptyFunction(string name)
	{
		Function function = new Function(name, exported: true);
		function.InsertCommand(new Return());
		return function;
	}

	public static Function EmptyFunction(string name, params string[] pars)
	{
		Function function = new Function(name, exported: true);
		function.DeclareParameters(pars);
		function.InsertCommand(new Return());
		return function;
	}

	public Function(string name)
	{
		Name = name;
	}

	public Function(string name, bool exported)
	{
		Name = name;
		Exported = exported;
	}
}
