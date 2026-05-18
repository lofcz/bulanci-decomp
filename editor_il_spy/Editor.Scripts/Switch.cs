using System.Collections;
using System.IO;

namespace Editor.Scripts;

public class Switch : ParamCommand
{
	public sealed class Item
	{
		private int _value;

		private string _label;

		public int Value => _value;

		public string Label => _label;

		public Item(int v, string label)
		{
			_value = v;
			_label = label;
		}
	}

	private ArrayList _items;

	public void AddItem(Item item)
	{
		_items.Add(item);
	}

	public void AddItem(int val, string label)
	{
		_items.Add(new Item(val, label));
	}

	public Switch(Command val)
		: base(Opcode.Switch, val)
	{
		_items = new ArrayList();
	}

	public Switch(Command val, params Item[] items)
		: base(Opcode.Switch, val)
	{
		_items = new ArrayList(items);
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		writer.Write((byte)_items.Count);
		foreach (Item item in _items)
		{
			writer.Write(item.Value);
			WriteLabel(writer, item.Label);
		}
	}
}
