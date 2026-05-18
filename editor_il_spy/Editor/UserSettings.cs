using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using System.Xml.Serialization;

namespace Editor;

public class UserSettings
{
	public static readonly UserSettings Instance = new UserSettings();

	private const string FileName = "settings.xml";

	private static readonly XmlSerializer serializer = new XmlSerializer(typeof(Settings));

	public Settings Settings { get; set; } = CreateDefault();

	public void LoadSettings()
	{
		try
		{
			using (FileStream stream = File.OpenRead(Path.Combine(GetPath(), "settings.xml")))
			{
				Settings = (Settings)serializer.Deserialize(stream);
			}
			if (Settings == null)
			{
				throw new Exception("Failed to load settings");
			}
		}
		catch (Exception)
		{
			Settings = CreateDefault();
		}
	}

	public void SaveSettings()
	{
		using FileStream stream = File.Create(Path.Combine(GetPath(), "settings.xml"));
		serializer.Serialize(stream, Settings);
	}

	private static string GetPath()
	{
		string text = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "CombatPillows Editor");
		if (!Directory.Exists(text))
		{
			Directory.CreateDirectory(text);
		}
		return text;
	}

	private static Settings CreateDefault()
	{
		Rectangle bounds = Screen.PrimaryScreen.Bounds;
		return new Settings
		{
			X = (bounds.Width - 1200) / 2,
			Y = (bounds.Height - 800) / 2,
			Width = 1200,
			Height = 800,
			TreeHeight = 350,
			TreeWidth = 200,
			LastLevel = string.Empty
		};
	}

	private UserSettings()
	{
	}
}
