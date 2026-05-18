using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public interface IMouseHandler
{
	void OnDoubleClick(Control sender, Point where);

	void OnRightClick(Control sender, Point where);
}
