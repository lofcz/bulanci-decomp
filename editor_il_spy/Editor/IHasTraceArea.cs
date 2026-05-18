using System.Drawing;
using Editor.Scripts;

namespace Editor;

public interface IHasTraceArea
{
	int TraceAreaID { get; set; }

	bool IsTraceAreaEnabled { get; }

	Rectangle TraceAreaRectangle { get; }

	TraceAreaFlags TraceAreaFlags { get; }

	void GenerateOnEnterTrace(string player, Function func);

	void GenerateOnLeaveTrace(string player, Function func);
}
