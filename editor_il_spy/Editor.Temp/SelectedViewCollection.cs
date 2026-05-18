using System;
using System.Collections;

namespace Editor.Temp;

public class SelectedViewCollection : ICollection, IEnumerable
{
	private View _parent;

	private ArrayList _list = new ArrayList();

	public View this[int index] => _list[index] as View;

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

	internal void Add(View view)
	{
		if (view.TopSelect)
		{
			view.ZOrder = 0;
		}
		_list.Add(view);
	}

	internal void Remove(View view)
	{
		_list.Remove(view);
	}

	internal SelectedViewCollection(View parent)
	{
		_parent = parent;
	}
}
