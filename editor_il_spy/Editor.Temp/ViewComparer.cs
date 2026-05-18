using System.Collections;

namespace Editor.Temp;

internal sealed class ViewComparer : IComparer
{
	public int Compare(object x, object y)
	{
		if (x == y)
		{
			return 0;
		}
		if (x.GetType() == typeof(BitmapView))
		{
			return 1;
		}
		if (y.GetType() == typeof(BitmapView))
		{
			return -1;
		}
		LevelItem levelItem = (x as View).Tag as LevelItem;
		LevelItem levelItem2 = (y as View).Tag as LevelItem;
		if (levelItem == null)
		{
			return -1;
		}
		if (levelItem2 == null)
		{
			return 1;
		}
		if (levelItem.Layer == levelItem2.Layer)
		{
			return levelItem.ZOrder - levelItem2.ZOrder;
		}
		return levelItem.Layer - levelItem2.Layer;
	}
}
