using System;
using System.Drawing;
using System.IO;

namespace Editor.ResourceItems;

public class LoadingResourceItem : BitmapResourceItem
{
	public override void Write(BinaryWriter writer)
	{
		Bitmap bitmap = new Bitmap(base.Bitmap);
		Bitmap bitmap2 = Tools.Texts.GetObject("createdby") as Bitmap;
		using (Graphics graphics = Graphics.FromImage(bitmap))
		{
			graphics.DrawImage(bitmap2, new Rectangle(20, bitmap.Height - bitmap2.Height - 20, bitmap2.Size.Width, bitmap2.Size.Height));
		}
		WriteBitmap(writer, bitmap);
	}

	public override void LoadData()
	{
		if (base.FileName == null || base.FileName.Length == 0)
		{
			throw new Exception(Tools.Localizer.GetString(92));
		}
		Bitmap bitmap = base.Bitmap;
		base.LoadData();
		if (base.Bitmap == null || base.Bitmap.Size != new Size(800, 600))
		{
			base.Bitmap = bitmap;
			throw new Exception(Tools.Localizer.GetString(93));
		}
	}

	public LoadingResourceItem()
	{
		base.Name = "$$LoadBitmap$$";
	}
}
