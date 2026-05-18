using System.IO;

namespace Editor.Scripts;

public sealed class CreateImage : ParamCommand
{
	private ResourceItem _image;

	public CreateImage(Command x, Command y, ResourceItem image)
		: base(Opcode.CreateImage, x, y)
	{
		_image = image;
	}

	public CreateImage(int x, int y, ResourceItem image)
		: base(Opcode.CreateImage, new IntConst(x), new IntConst(y))
	{
		_image = image;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		writer.Write(_image.ID);
	}
}
