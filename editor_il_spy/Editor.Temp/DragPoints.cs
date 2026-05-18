namespace Editor.Temp;

public class DragPoints
{
	private View _view;

	private DragPointView[] _points = new DragPointView[8];

	public DragPoints(View view)
	{
		_view = view;
		for (int i = 0; i < 8; i++)
		{
			_points[i] = new DragPointView(_view, (DragPointType)i);
			_view.Parent.Childs.Insert(_view.ZOrder, _points[i]);
		}
	}

	public void Clear()
	{
		int num = 8;
		while (num-- > 0)
		{
			_view.Parent.Childs.Remove(_points[num]);
			_points[num] = null;
		}
	}
}
