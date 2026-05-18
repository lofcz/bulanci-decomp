using System.Drawing;

namespace Editor.Temp;

public class Tools
{
	public static Point AlignSize(Size sz, Rectangle rect, ContentAlignment align)
	{
		return align switch
		{
			ContentAlignment.BottomCenter => new Point(rect.Left + (rect.Size.Width - sz.Width) / 2, rect.Bottom - sz.Height), 
			ContentAlignment.BottomLeft => new Point(rect.Left, rect.Bottom - sz.Height), 
			ContentAlignment.BottomRight => new Point(rect.Right - sz.Width, rect.Bottom - sz.Height), 
			ContentAlignment.MiddleCenter => new Point(rect.Left + (rect.Size.Width - sz.Width) / 2, rect.Top + (rect.Size.Height - sz.Height) / 2), 
			ContentAlignment.MiddleLeft => new Point(rect.Left, rect.Top + (rect.Size.Height - sz.Height) / 2), 
			ContentAlignment.MiddleRight => new Point(rect.Right - sz.Width, rect.Top + (rect.Size.Height - sz.Height) / 2), 
			ContentAlignment.TopCenter => new Point(rect.Left + (rect.Size.Width - sz.Width) / 2, rect.Top), 
			ContentAlignment.TopRight => new Point(rect.Right - sz.Width, rect.Top), 
			_ => new Point(rect.Left, rect.Top), 
		};
	}
}
