using System;
using System.Collections;

namespace Editor.Temp;

public class ViewCollection : IList, ICollection, IEnumerable
{
	private View _parent;

	private ArrayList _list = new ArrayList();

	public View this[int index]
	{
		get
		{
			return _list[index] as View;
		}
		set
		{
			throw new NotSupportedException();
		}
	}

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

	public bool IsReadOnly => false;

	public bool IsFixedSize => false;

	public bool IsSynchronized => _list.IsSynchronized;

	public int Count => _list.Count;

	public object SyncRoot => _parent;

	public void CopyTo(Array array, int index)
	{
		_list.CopyTo(array, index);
	}

	public IEnumerator GetEnumerator()
	{
		return _list.GetEnumerator();
	}

	public int Add(View view)
	{
		Insert(0, view);
		return 0;
	}

	int IList.Add(object value)
	{
		Insert(0, value as View);
		return 0;
	}

	public void Insert(int index, View view)
	{
		if (view != null && view.Parent == null)
		{
			_list.Insert(index, view);
			_parent.ViewInserted(view);
		}
	}

	void IList.Insert(int index, object value)
	{
		Insert(index, value as View);
	}

	public void Remove(View view)
	{
		RemoveAt(IndexOf(view));
	}

	void IList.Remove(object value)
	{
		RemoveAt(IndexOf(value as View));
	}

	public void RemoveAt(int index)
	{
		if (index >= 0 && index < Count)
		{
			View view = _list[index] as View;
			_list.RemoveAt(index);
			_parent.ViewRemoved(view);
		}
	}

	public int IndexOf(View view)
	{
		return _list.IndexOf(view);
	}

	int IList.IndexOf(object value)
	{
		return _list.IndexOf(value);
	}

	public bool Contains(View view)
	{
		return _list.Contains(view);
	}

	bool IList.Contains(object value)
	{
		return _list.Contains(value);
	}

	public void Clear()
	{
		int count = Count;
		while (count-- > 0)
		{
			RemoveAt(count);
		}
	}

	public void Sort(IComparer comparer)
	{
		_list.Sort(comparer);
		_parent.ChildsSorted();
	}

	internal bool MoveTo(View view, int index)
	{
		index = Math.Min(Count - 1, Math.Max(0, index));
		int num = IndexOf(view);
		if (num >= 0 && num != index)
		{
			_list.RemoveAt(num);
			_list.Insert(index, view);
			return true;
		}
		return false;
	}

	internal ViewCollection(View parent)
	{
		_parent = parent;
	}
}
