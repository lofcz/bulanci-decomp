namespace Editor;

public interface IHasGuid
{
	string Guid { get; set; }

	void AssignNewGuid();
}
