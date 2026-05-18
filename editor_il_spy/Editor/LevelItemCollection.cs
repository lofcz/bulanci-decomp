using System;
using System.Collections;
using Editor.Temp;

namespace Editor;

public class LevelItemCollection : IList, ICollection, IEnumerable
{
	private ArrayList _list = new ArrayList();

	private LayerType _layer;

	private Level _level;

	public bool IsReadOnly => false;

	object IList.this[int index]
	{
		get
		{
			return _list[index];
		}
		set
		{
			throw new NotSupportedException();
		}
	}

	public LevelItem this[int index]
	{
		get
		{
			return _list[index] as LevelItem;
		}
		set
		{
			throw new NotSupportedException();
		}
	}

	public bool IsFixedSize => _list.IsFixedSize;

	public bool IsSynchronized => _list.IsSynchronized;

	public int Count => _list.Count;

	public object SyncRoot => _level;

	public void RemoveAt(int index)
	{
		if (index >= 0 && index < Count)
		{
			LevelItem levelItem = _list[index] as LevelItem;
			_level.OnItemRemoved(levelItem);
			levelItem.SetLevel(null);
			levelItem.SetLayer(LayerType.None);
			_list.RemoveAt(index);
		}
	}

	void IList.Insert(int index, object value)
	{
		Insert(index, value as LevelItem);
	}

	public void Insert(int index, LevelItem item)
	{
		if (item != null)
		{
			if (item.Level != null)
			{
				item.Remove();
			}
			item.SetLayer(_layer);
			item.SetLevel(_level);
			_list.Insert(index, item);
			_level.OnItemInserted(item);
		}
	}

	void IList.Remove(object value)
	{
		RemoveAt(_list.IndexOf(value));
	}

	public void Remove(LevelItem li)
	{
		RemoveAt(IndexOf(li));
	}

	bool IList.Contains(object value)
	{
		return _list.Contains(value);
	}

	public bool Contains(LevelItem item)
	{
		return _list.Contains(item);
	}

	public void Clear()
	{
		int count = Count;
		while (count-- > 0)
		{
			RemoveAt(count);
		}
	}

	int IList.IndexOf(object value)
	{
		return _list.IndexOf(value);
	}

	public int IndexOf(LevelItem item)
	{
		return _list.IndexOf(item);
	}

	int IList.Add(object value)
	{
		return Add(value as LevelItem);
	}

	public int Add(LevelItem item)
	{
		Insert(Count, item);
		return Count - 1;
	}

	public void CopyTo(Array array, int index)
	{
		_list.CopyTo(array, index);
	}

	public IEnumerator GetEnumerator()
	{
		return _list.GetEnumerator();
	}

	public void CopyFrom(LevelItemCollection collection)
	{
		Clear();
		foreach (LevelItem item in collection)
		{
			Add(item.Clone() as LevelItem);
		}
	}

	internal LevelItemCollection(Level level, LayerType layer)
	{
		_level = level;
		_layer = layer;
	}
}
