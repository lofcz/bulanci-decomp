using System;
using System.Reflection;
using System.Resources;
using System.Windows.Forms;
using Editor.Localize;

namespace Editor;

public class Tools
{
	private static ResourceManager _resources;

	private static LocalizerManager _localizer;

	public static ResourceManager Texts
	{
		get
		{
			if (_resources == null)
			{
				_resources = new ResourceManager("Editor.Texts", Assembly.GetExecutingAssembly());
			}
			return _resources;
		}
	}

	public static LocalizerManager Localizer
	{
		get
		{
			if (_localizer == null)
			{
				_localizer = new LocalizerManager();
				_localizer.ResourceSet = new ResXResourceSet(Assembly.GetExecutingAssembly().GetManifestResourceStream("Editor.Data.lang.xml"));
			}
			return _localizer;
		}
	}

	public static string GenerateGuid()
	{
		return Guid.NewGuid().ToString().ToUpper();
	}

	public static void ShowError(Exception exc)
	{
		MessageBox.Show(exc.Message, Localizer.GetString(104), MessageBoxButtons.OK, MessageBoxIcon.Hand);
	}

	public static string Format(int key, params object[] pars)
	{
		return string.Format(Localizer.GetString(key), pars);
	}

	public static void ShowInfo(int info, params object[] pars)
	{
		MessageBox.Show(string.Format(Localizer.GetString(info), pars), Localizer.GetString(105), MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
	}

	public static DialogResult Question(MessageBoxButtons buttons, int info, params object[] pars)
	{
		return MessageBox.Show(string.Format(Localizer.GetString(info), pars), Localizer.GetString(109), buttons, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1);
	}
}
