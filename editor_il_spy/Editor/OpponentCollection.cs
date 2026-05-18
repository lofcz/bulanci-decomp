using System;
using System.Collections;

namespace Editor;

public class OpponentCollection : IList, ICollection, IEnumerable
{
	private ArrayList _list = new ArrayList();

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

	public Opponent this[int index]
	{
		get
		{
			return _list[index] as Opponent;
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
			Opponent opponent = _list[index] as Opponent;
			_level.OnOpponentRemoved(opponent);
			opponent.Level = null;
			_list.RemoveAt(index);
		}
	}

	void IList.Insert(int index, object value)
	{
		Insert(index, value as Opponent);
	}

	public void Insert(int index, Opponent item)
	{
		if (item != null)
		{
			if (item.Level != null)
			{
				item.Remove();
			}
			item.Level = _level;
			_list.Insert(index, item);
			_level.OnOpponentInserted(item);
		}
	}

	void IList.Remove(object value)
	{
		RemoveAt(_list.IndexOf(value));
	}

	public void Remove(Opponent li)
	{
		RemoveAt(IndexOf(li));
	}

	bool IList.Contains(object value)
	{
		return _list.Contains(value);
	}

	public bool Contains(Opponent item)
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

	public int IndexOf(Opponent item)
	{
		return _list.IndexOf(item);
	}

	int IList.Add(object value)
	{
		return Add(value as Opponent);
	}

	public int Add(Opponent item)
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

	public void CopyFrom(OpponentCollection collection)
	{
		Clear();
		foreach (Opponent item in collection)
		{
			Add(item.Clone() as Opponent);
		}
	}

	internal OpponentCollection(Level level)
	{
		_level = level;
	}
}
