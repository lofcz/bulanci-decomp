using System;
using System.Collections;

namespace Editor.ResourceItems;

public class ResourceCollection : IList, ICollection, IEnumerable
{
	private Level _level;

	private ArrayList _list = new ArrayList();

	private Hashtable _hash = new Hashtable();

	public bool IsReadOnly => _list.IsReadOnly;

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

	public ResourceItem this[int index]
	{
		get
		{
			return _list[index] as ResourceItem;
		}
		set
		{
			throw new NotSupportedException();
		}
	}

	public ResourceItem this[string name] => _hash[name] as ResourceItem;

	public bool IsFixedSize => _list.IsFixedSize;

	public bool IsSynchronized => _list.IsSynchronized;

	public int Count => _list.Count;

	public object SyncRoot => _level;

	public void RemoveAt(int index)
	{
		if (index >= 0 && index < Count)
		{
			ResourceItem resourceItem = this[index];
			if (resourceItem.IsUsed)
			{
				throw new Exception(Tools.Localizer.GetString(96));
			}
			_level.OnResourceRemoved(resourceItem);
			_hash.Remove(resourceItem.Name);
			_list.RemoveAt(index);
			resourceItem.SetLevel(null);
		}
	}

	void IList.Insert(int index, object value)
	{
		Insert(index, value as ResourceItem);
	}

	public void Insert(int index, ResourceItem item)
	{
		if (item != null)
		{
			if (_hash.Contains(item.Name))
			{
				throw new Exception(Tools.Localizer.GetString(97));
			}
			if (item.Level != null)
			{
				item.Level.Resources.Remove(item);
			}
			_list.Insert(index, item);
			_hash[item.Name] = item;
			item.SetLevel(_level);
			_level.OnResourceInserted(item);
		}
	}

	void IList.Remove(object value)
	{
		RemoveAt(IndexOf(value as ResourceItem));
	}

	public void Remove(ResourceItem item)
	{
		RemoveAt(IndexOf(item));
	}

	bool IList.Contains(object value)
	{
		return _list.Contains(value);
	}

	public bool Contains(ResourceItem item)
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

	public int IndexOf(ResourceItem item)
	{
		return _list.IndexOf(item);
	}

	public int IndexOf(string name)
	{
		return IndexOf(this[name]);
	}

	int IList.Add(object value)
	{
		Insert(Count, value as ResourceItem);
		return Count - 1;
	}

	public int Add(ResourceItem item)
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

	internal void Rename(ResourceItem item, string name)
	{
		if (IndexOf(item) >= 0)
		{
			if (this[name] != null)
			{
				throw new ArgumentException("Cannot rename resource to '" + name + "'. It is already in collection.");
			}
			_hash.Remove(item.Name);
			_hash[name] = item;
		}
	}

	public void CopyFrom(ResourceCollection collection)
	{
		ArrayList arrayList = new ArrayList(_list);
		foreach (ResourceItem item2 in collection)
		{
			int num = IndexOf(item2.Name);
			if (num < 0)
			{
				Add(item2.Clone() as ResourceItem);
				continue;
			}
			this[num].CopyFrom(item2);
			arrayList.Remove(this[num]);
		}
		foreach (ResourceItem item3 in arrayList)
		{
			Remove(item3);
		}
	}

	internal ResourceCollection(Level level)
	{
		_level = level;
	}
}
