using System;
using System.IO;
using System.Xml.Serialization;
using NAudio.Wave;

namespace Editor.ResourceItems;

[XmlRoot("Mp3")]
public class Mp3ResourceItem : FileResourceItem
{
	private AudioInfo _info;

	private byte[] _data;

	public AudioInfo AudioInfo
	{
		get
		{
			return _info;
		}
		set
		{
			_info = value;
			ItemModified();
		}
	}

	public byte[] Data
	{
		get
		{
			return _data;
		}
		set
		{
			_data = value;
			ItemModified();
		}
	}

	public override uint ClassID => 48u;

	public override void Write(BinaryWriter writer)
	{
		writer.Write(_data.Length);
		writer.Write(_info.Size);
		writer.Write(_info.Channels);
		writer.Write(_info.Bits);
		writer.Write(_info.Frequency);
		writer.Write(_data, 0, _data.Length);
	}

	public override void LoadData()
	{
		if (base.FileName == null || base.FileName.Length == 0)
		{
			throw new Exception(Tools.Localizer.GetString(94));
		}
		byte[] array;
		using (Stream stream = new FileStream(base.FileName, FileMode.Open, FileAccess.Read))
		{
			array = new byte[stream.Length];
			stream.Read(array, 0, array.Length);
		}
		try
		{
			using MemoryStream inputStream = new MemoryStream(array, writable: false);
			using Mp3FileReader mp3FileReader = new Mp3FileReader(inputStream);
			_info = new AudioInfo
			{
				Frequency = (uint)mp3FileReader.WaveFormat.SampleRate,
				Bits = (ushort)mp3FileReader.WaveFormat.BitsPerSample,
				Channels = (ushort)mp3FileReader.WaveFormat.Channels,
				Size = (uint)mp3FileReader.Length
			};
		}
		catch (Exception innerException)
		{
			throw new Exception(Tools.Localizer.GetString(95), innerException);
		}
		_data = array;
	}

	public override void CopyFrom(ResourceItem item)
	{
		Lock();
		base.CopyFrom(item);
		if (item is Mp3ResourceItem mp3ResourceItem)
		{
			AudioInfo = mp3ResourceItem.AudioInfo;
			Data = mp3ResourceItem.Data;
		}
		Unlock();
	}

	public Mp3ResourceItem()
	{
		base.Name = "$$LevelSound$$";
	}
}
