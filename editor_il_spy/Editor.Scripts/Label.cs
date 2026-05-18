using System.Collections;
using System.IO;

namespace Editor.Scripts;

public sealed class Label
{
	private string _name;

	private int _position;

	private ArrayList _refs = new ArrayList();

	public string Name => _name;

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

	public Label(string name)
	{
		_name = name;
	}

	public void WriteCode(BinaryWriter writer)
	{
		_refs.Add((int)writer.BaseStream.Position);
		writer.Write(0);
	}

	public void Update(BinaryWriter writer)
	{
		foreach (int @ref in _refs)
		{
			writer.BaseStream.Seek(@ref, SeekOrigin.Begin);
			writer.Write(Position);
		}
	}
}
