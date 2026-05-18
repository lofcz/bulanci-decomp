using System.Windows.Forms;

namespace Editor;

public interface IKeybHandler
{
	bool OnKeyDown(Control sender, Keys key);
}
