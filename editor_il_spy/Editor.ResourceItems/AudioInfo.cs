using System.Xml.Serialization;

namespace Editor.ResourceItems;

public struct AudioInfo
{
	[XmlAttribute]
	public uint Size;

	[XmlAttribute]
	public ushort Channels;

	[XmlAttribute]
	public ushort Bits;

	[XmlAttribute]
	public uint Frequency;
}
