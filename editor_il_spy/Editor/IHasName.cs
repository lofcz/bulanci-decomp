namespace Editor;

public interface IHasName
{
	bool IsReserved { get; }

	string Name { get; set; }
}
