using System;
using System.Drawing;

namespace Editor.ResourceItems;

public class BackgroundResourceItem : BitmapResourceItem
{
	private static readonly Size _size = new Size(800, 515);

	public static Size BitmapSize => _size;

	public override void LoadData()
	{
		if (base.FileName == null || base.FileName.Length == 0)
		{
			throw new Exception(Tools.Localizer.GetString(91));
		}
		Bitmap bitmap = base.Bitmap;
		base.LoadData();
		if (base.Bitmap == null || base.Bitmap.Size != BitmapSize)
		{
			base.Bitmap = bitmap;
			throw new Exception(Tools.Format(107, BitmapSize.Width, BitmapSize.Height));
		}
	}

	public BackgroundResourceItem()
	{
		base.Name = "$$LevelBackground$$";
	}
}
