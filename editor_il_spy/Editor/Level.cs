using System;
using System.Collections;
using System.IO;
using System.Windows.Forms;
using System.Xml.Serialization;
using Editor.ResourceItems;
using Editor.Scripts;
using Editor.Temp;

namespace Editor;

public sealed class Level
{
	private string _filename = "";

	private bool _modified;

	private bool _vampires;

	private LevelItemCollection _items0;

	private LevelItemCollection _items1;

	private LevelItemCollection _items2;

	private ResourceCollection _resources;

	private OpponentCollection _opponents;

	[XmlIgnore]
	public string Name
	{
		get
		{
			return Sign.Content;
		}
		set
		{
			Sign.Content = value;
		}
	}

	[XmlIgnore]
	public bool Modified
	{
		get
		{
			return _modified;
		}
		set
		{
			_modified = value;
		}
	}

	[XmlIgnore]
	public string CreatedBy
	{
		get
		{
			return Sign.Copyright;
		}
		set
		{
			Sign.Copyright = value;
		}
	}

	[XmlIgnore]
	public string FileName
	{
		get
		{
			return _filename;
		}
		set
		{
			_filename = value;
			Modified = true;
		}
	}

	[XmlIgnore]
	public string LoadBitmapName
	{
		get
		{
			return LoadBitmap.FileName;
		}
		set
		{
			LoadingResourceItem loadBitmap = LoadBitmap;
			loadBitmap.CallLoadData = true;
			loadBitmap.FileName = value;
		}
	}

	[XmlIgnore]
	public string BackgroundName
	{
		get
		{
			return Background.FileName;
		}
		set
		{
			BitmapResourceItem background = Background;
			background.CallLoadData = true;
			background.FileName = value;
		}
	}

	[XmlIgnore]
	public string SoundName
	{
		get
		{
			return Sound.FileName;
		}
		set
		{
			Mp3ResourceItem sound = Sound;
			sound.CallLoadData = true;
			sound.FileName = value;
		}
	}

	[XmlIgnore]
	public LoadingResourceItem LoadBitmap
	{
		get
		{
			LoadingResourceItem loadingResourceItem = FindResource("$$LoadBitmap$$") as LoadingResourceItem;
			if (loadingResourceItem == null)
			{
				Resources.Add(loadingResourceItem = new LoadingResourceItem());
			}
			return loadingResourceItem;
		}
	}

	[XmlIgnore]
	public BitmapResourceItem Background
	{
		get
		{
			BackgroundResourceItem backgroundResourceItem = FindResource("$$LevelBackground$$") as BackgroundResourceItem;
			if (backgroundResourceItem == null)
			{
				Resources.Add(backgroundResourceItem = new BackgroundResourceItem());
			}
			return backgroundResourceItem;
		}
	}

	[XmlIgnore]
	public Mp3ResourceItem Sound
	{
		get
		{
			Mp3ResourceItem mp3ResourceItem = FindResource("$$LevelSound$$") as Mp3ResourceItem;
			if (mp3ResourceItem == null)
			{
				Resources.Add(mp3ResourceItem = new Mp3ResourceItem());
			}
			return mp3ResourceItem;
		}
	}

	[XmlIgnore]
	public ResourceSign Sign
	{
		get
		{
			ResourceSign resourceSign = FindResource("$$LevelSign$$") as ResourceSign;
			if (resourceSign == null)
			{
				Resources.Add(resourceSign = new ResourceSign());
			}
			return resourceSign;
		}
	}

	[XmlArray("Layer0")]
	[XmlArrayItem("Obstacle", typeof(ObstacleLevelItem))]
	[XmlArrayItem("Bitmap", typeof(ImageLevelItem))]
	[XmlArrayItem("Teleport", typeof(TeleportLevelItem))]
	[XmlArrayItem("Mine", typeof(MineLevelItem))]
	public LevelItemCollection ItemsLayer0 => _items0;

	[XmlArray("Layer1")]
	[XmlArrayItem("Obstacle", typeof(ObstacleLevelItem))]
	[XmlArrayItem("Bitmap", typeof(ImageLevelItem))]
	[XmlArrayItem("Teleport", typeof(TeleportLevelItem))]
	[XmlArrayItem("Mine", typeof(MineLevelItem))]
	public LevelItemCollection ItemsLayer1 => _items1;

	[XmlArray("Layer2")]
	[XmlArrayItem("Obstacle", typeof(ObstacleLevelItem))]
	[XmlArrayItem("Bitmap", typeof(ImageLevelItem))]
	[XmlArrayItem("Teleport", typeof(TeleportLevelItem))]
	[XmlArrayItem("Mine", typeof(MineLevelItem))]
	public LevelItemCollection ItemsLayer2 => _items2;

	[XmlIgnore]
	public ArrayList Items
	{
		get
		{
			ArrayList arrayList = new ArrayList();
			arrayList.AddRange(ItemsLayer0);
			arrayList.AddRange(ItemsLayer1);
			arrayList.AddRange(ItemsLayer2);
			return arrayList;
		}
	}

	[XmlArray("Resources")]
	[XmlArrayItem("Background", typeof(BackgroundResourceItem))]
	[XmlArrayItem("LoadBitmap", typeof(LoadingResourceItem))]
	[XmlArrayItem("Sound", typeof(Mp3ResourceItem))]
	[XmlArrayItem("Sign", typeof(ResourceSign))]
	[XmlArrayItem("Bitmap", typeof(BitmapResourceItem))]
	public ResourceCollection Resources => _resources;

	[XmlArray("Opponents")]
	[XmlArrayItem("Opponent", typeof(Opponent))]
	public OpponentCollection Opponents => _opponents;

	public bool Vampires
	{
		get
		{
			return _vampires;
		}
		set
		{
			if (_vampires != value)
			{
				_vampires = value;
				Modified = true;
			}
		}
	}

	public event ItemEventHandler ItemInserted;

	public event ItemEventHandler ItemRemoved;

	public event ItemEventHandler ItemModified;

	public event ResourceEventHandler ResourceInserted;

	public event ResourceEventHandler ResourceRemoved;

	public event ResourceEventHandler ResourceModified;

	public event OpponentEventHandler OpponentInserted;

	public event OpponentEventHandler OpponentRemoved;

	public event OpponentEventHandler OpponentModified;

	private void InitData()
	{
		_items0 = new LevelItemCollection(this, LayerType.Layer0);
		_items1 = new LevelItemCollection(this, LayerType.Layer1);
		_items2 = new LevelItemCollection(this, LayerType.Layer2);
		_resources = new ResourceCollection(this);
		_opponents = new OpponentCollection(this);
	}

	public Level()
	{
		InitData();
	}

	public Level(Level level)
	{
		InitData();
		CopyFrom(level);
	}

	public ResourceItem FindResource(string name)
	{
		foreach (ResourceItem resource in Resources)
		{
			if (resource.Name == name)
			{
				return resource;
			}
		}
		return null;
	}

	public void MarkModified()
	{
		Modified = true;
	}

	public LevelItemCollection Layer(LayerType layer)
	{
		return layer switch
		{
			LayerType.Layer0 => _items0, 
			LayerType.Layer1 => _items1, 
			LayerType.Layer2 => _items2, 
			_ => null, 
		};
	}

	public static Level Open(string name)
	{
		try
		{
			using Stream stream = new FileStream(name, FileMode.Open, FileAccess.Read);
			Stream stream2 = stream;
			uint num = new BinaryReader(stream).ReadUInt32();
			stream.Seek(0L, SeekOrigin.Begin);
			if (num == 1346984519)
			{
				stream2 = new GZipStream(stream, create: false);
			}
			Level obj = new XmlSerializer(typeof(Level)).Deserialize(stream2) as Level;
			obj.FileName = name;
			obj.Modified = false;
			UserSettings.Instance.Settings.LastLevel = name;
			UserSettings.Instance.SaveSettings();
			return obj;
		}
		catch (Exception exc)
		{
			UserSettings.Instance.Settings.LastLevel = string.Empty;
			UserSettings.Instance.SaveSettings();
			Tools.ShowError(exc);
		}
		return null;
	}

	public static Level OpenLast()
	{
		string lastLevel = UserSettings.Instance.Settings.LastLevel;
		if (string.IsNullOrEmpty(lastLevel))
		{
			return null;
		}
		return Open(lastLevel);
	}

	public static Level Open()
	{
		OpenFileDialog openFileDialog = new OpenFileDialog();
		openFileDialog.DefaultExt = "*.eapres";
		openFileDialog.Filter = Tools.Localizer.GetString(76);
		openFileDialog.Title = Tools.Localizer.GetString(77);
		if (openFileDialog.ShowDialog() == DialogResult.OK)
		{
			return Open(openFileDialog.FileName);
		}
		return null;
	}

	public static Level New()
	{
		Level level = new Level();
		if (new NewLevelWizard(level).ShowDialog() == DialogResult.OK)
		{
			return level;
		}
		return null;
	}

	public bool Save()
	{
		if (FileName.Length == 0)
		{
			return SaveAs();
		}
		try
		{
			using Stream stream = new GZipStream(new FileStream(FileName, FileMode.Create, FileAccess.Write), create: true);
			new XmlSerializer(typeof(Level)).Serialize(stream, this);
		}
		catch (Exception exc)
		{
			Tools.ShowError(exc);
			return false;
		}
		Modified = false;
		Application.CommonAppDataRegistry.SetValue("LastLevel", FileName);
		return true;
	}

	public bool SaveAs()
	{
		SaveFileDialog saveFileDialog = new SaveFileDialog();
		saveFileDialog.DefaultExt = "*.eapres";
		saveFileDialog.Filter = Tools.Localizer.GetString(76);
		saveFileDialog.Title = Tools.Localizer.GetString(78);
		saveFileDialog.FileName = FileName;
		if (saveFileDialog.ShowDialog() == DialogResult.OK)
		{
			FileName = saveFileDialog.FileName;
			return Save();
		}
		return false;
	}

	public string GenerateNewResourceName(string prefix)
	{
		return GenerateNewName(prefix, Resources);
	}

	public string GenerateNewItemName(string prefix)
	{
		return GenerateNewName(prefix, Items);
	}

	public string GenerateNewName(string prefix, IEnumerable enumerable)
	{
		int num = int.MinValue;
		string text = prefix.ToLower();
		int length = text.Length;
		foreach (IHasName item in enumerable)
		{
			if (item.IsReserved || item.Name == null || item.Name.Length < length)
			{
				continue;
			}
			string text2 = item.Name.ToLower();
			if (!(text2.Substring(0, length) == text))
			{
				continue;
			}
			string text3 = text2.Substring(length);
			try
			{
				if (text3 == "" && 1 > num)
				{
					num = 1;
					continue;
				}
				int num2 = Convert.ToInt32(text3);
				if (num2.ToString() == text3 && num2 > num)
				{
					num = num2;
				}
			}
			catch
			{
			}
		}
		if (num == int.MinValue)
		{
			return prefix;
		}
		return prefix + (num + 1);
	}

	public LevelItem GetItemByGuid(string guid)
	{
		foreach (LevelItem item in Items)
		{
			if (item is IHasGuid hasGuid && hasGuid.Guid == guid)
			{
				return item;
			}
		}
		return null;
	}

	public bool IsResourceUsed(ResourceItem item)
	{
		if (item.IsReserved)
		{
			return true;
		}
		if (item is BitmapResourceItem)
		{
			foreach (LevelItem item2 in Items)
			{
				if (item2 is ImageLevelItem && (item2 as ImageLevelItem).BitmapName == item.Name)
				{
					return true;
				}
			}
		}
		return false;
	}

	public bool Compile()
	{
		if (!Save())
		{
			return false;
		}
		try
		{
			ArrayList arrayList = new ArrayList();
			foreach (ResourceItem resource in Resources)
			{
				if (resource.IsUsed)
				{
					arrayList.Add(resource);
				}
			}
			arrayList.Add(CreateLevelScript());
			uint num = 100000u;
			for (int i = 0; i < arrayList.Count; i++)
			{
				(arrayList[i] as ResourceItem).ID = num++;
			}
			MemoryStream memoryStream = new MemoryStream();
			BinaryWriter binaryWriter = new BinaryWriter(memoryStream);
			foreach (ResourceItem item in arrayList)
			{
				item.Position = binaryWriter.BaseStream.Position;
				binaryWriter.Write(item.ClassID);
				item.Write(binaryWriter);
				item.Size = (uint)(binaryWriter.BaseStream.Position - item.Position - 4);
			}
			string text = FileName.Substring(0, FileName.LastIndexOf('.')) + ".eap";
			using (Stream stream = new GZipStream(new FileStream(text, FileMode.Create, FileAccess.Write), create: true))
			{
				binaryWriter = new BinaryWriter(stream);
				binaryWriter.Write(arrayList.Count);
				binaryWriter.Write(8);
				foreach (ResourceItem item2 in arrayList)
				{
					binaryWriter.Write(90);
					item2.WriteHeader(binaryWriter);
				}
				binaryWriter.Flush();
				byte[] array = new byte[65536];
				memoryStream.Seek(0L, SeekOrigin.Begin);
				while (memoryStream.Position < memoryStream.Length)
				{
					int count = Math.Min(array.Length, (int)(memoryStream.Length - memoryStream.Position));
					memoryStream.Read(array, 0, count);
					stream.Write(array, 0, count);
				}
			}
			Tools.ShowInfo(106, text);
		}
		catch (Exception exc)
		{
			Tools.ShowError(exc);
			return false;
		}
		return true;
	}

	private int CountSourceTeleports()
	{
		int num = 0;
		foreach (LevelItem item in Items)
		{
			if (item is TeleportLevelItem { IsSource: not false })
			{
				num++;
			}
		}
		return num;
	}

	private Script CreateLevelScript()
	{
		Function function = null;
		int num = CountSourceTeleports();
		bool flag = num > 0;
		Script script = new Script();
		script.DeclareVariables("name", "version", "guid");
		if (flag)
		{
			script.DeclareVariable("strm");
		}
		if (num > 0)
		{
			script.InsertFunction(MakeTeleport.Generate());
		}
		function = new Function("info", exported: true);
		function.DeclareParameter("lang");
		function.InsertCommand(new SetGlobalVar("version", 1000));
		function.InsertCommand(new SetGlobalVar("guid", Tools.GenerateGuid()));
		function.InsertCommand(new SetGlobalVar("name", Name));
		function.InsertCommand(new Return());
		script.InsertFunction(function);
		function = new Function("init", exported: true);
		if (flag)
		{
			function.InsertCommand(new SetGlobalVar("strm", 0));
			function.InsertCommand(new IfEqual(new IsNet(), 0, "withoutnet"));
			function.InsertCommand(new SetGlobalVar("strm", new StrmCreateMem(4096, 4096)));
			function.InsertCommand(new SetCommStrm(new GetGlobalVar("strm")));
			function.SetLabel("withoutnet");
		}
		function.InsertCommand(new LoadPreface(LoadBitmap));
		function.InsertCommand(new SetMusic(Sound));
		function.InsertCommand(new SetInsertMode(0));
		function.InsertCommand(new InsertView(new CreateImage(0, 0, Background)));
		foreach (LevelItem item in ItemsLayer2)
		{
			item.GenerateScript(function);
		}
		function.InsertCommand(new SetInsertMode(2));
		foreach (LevelItem item2 in ItemsLayer0)
		{
			item2.GenerateScript(function);
		}
		function.InsertCommand(new SetInsertMode(1));
		foreach (LevelItem item3 in ItemsLayer1)
		{
			item3.GenerateScript(function);
		}
		function.InsertCommand(new SetInsertMode(0));
		int num2 = 0;
		foreach (LevelItem item4 in Items)
		{
			if (item4 is IHasTraceArea { IsTraceAreaEnabled: not false } hasTraceArea)
			{
				hasTraceArea.TraceAreaID = num2++;
			}
		}
		if (num2 > 0)
		{
			function.InsertCommand(new IfEqual(new IsServer(), 0, "skiptraces"));
			foreach (LevelItem item5 in Items)
			{
				if (item5 is IHasTraceArea { IsTraceAreaEnabled: not false } hasTraceArea2)
				{
					function.InsertCommand(new DefineTraceArea(hasTraceArea2.TraceAreaID, hasTraceArea2.TraceAreaRectangle, (int)hasTraceArea2.TraceAreaFlags));
				}
			}
			function.SetLabel("skiptraces");
		}
		function.InsertCommand(new InsertBulanci());
		if (Vampires)
		{
			function.InsertCommand(new InsertVampires());
		}
		for (int i = 0; i < Opponents.Count; i++)
		{
			function.InsertCommand(new InsertOpponent(Opponents[i]));
		}
		function.InsertCommand(new Return());
		script.InsertFunction(function);
		function = new Function("done", exported: true);
		if (flag)
		{
			function.InsertCommand(new IfEqual(new GetGlobalVar("strm"), 0, "nothing"));
			function.InsertCommand(new SetGlobalVar("strm", new StrmDestroy(new GetGlobalVar("strm"))));
			function.SetLabel("nothing");
		}
		function.InsertCommand(new Return());
		script.InsertFunction(function);
		script.InsertFunction(Function.EmptyFunction("onimagenotify", "lc0", "lc1"));
		script.InsertFunction(Function.EmptyFunction("onhit", "lc0", "lc1", "lc2"));
		script.InsertFunction(Function.EmptyFunction("onhitsucc", "lc0", "lc1"));
		script.InsertFunction(Function.EmptyFunction("ontimer", "lc0"));
		foreach (LevelItem item6 in Items)
		{
			if (item6 is TeleportLevelItem teleportLevelItem && teleportLevelItem.Targets.Count > 1)
			{
				script.InsertFunction(TeleportFrom.Generate(teleportLevelItem));
			}
		}
		function = new Function("onenter", exported: true);
		function.DeclareParameters("id", "player");
		if (num2 > 0)
		{
			Switch obj = new Switch(new GetLocalVar("id"));
			function.InsertCommand(obj);
			foreach (LevelItem item7 in Items)
			{
				if (item7 is IHasTraceArea { IsTraceAreaEnabled: not false } hasTraceArea3)
				{
					obj.AddItem(hasTraceArea3.TraceAreaID, "trace" + hasTraceArea3.TraceAreaID);
					function.SetLabel("trace" + hasTraceArea3.TraceAreaID);
					hasTraceArea3.GenerateOnEnterTrace("player", function);
					function.InsertCommand(new Goto("finish"));
				}
			}
			function.SetLabel("finish");
		}
		function.InsertCommand(new Return());
		script.InsertFunction(function);
		function = new Function("onleave", exported: true);
		function.DeclareParameters("id", "player");
		if (num2 > 0)
		{
			Switch obj2 = new Switch(new GetLocalVar("id"));
			function.InsertCommand(obj2);
			foreach (LevelItem item8 in Items)
			{
				if (item8 is IHasTraceArea { IsTraceAreaEnabled: not false } hasTraceArea4)
				{
					obj2.AddItem(hasTraceArea4.TraceAreaID, "trace" + hasTraceArea4.TraceAreaID);
					function.SetLabel("trace" + hasTraceArea4.TraceAreaID);
					hasTraceArea4.GenerateOnLeaveTrace("player", function);
					function.InsertCommand(new Goto("finish"));
				}
			}
			function.SetLabel("finish");
		}
		function.InsertCommand(new Return());
		script.InsertFunction(function);
		function = new Function("onreceive", exported: true);
		function.DeclareParameter("s");
		if (flag)
		{
			function.DeclareVariable("cmd");
			if (num > 0)
			{
				function.DeclareVariables("x", "y", "p");
			}
			function.InsertCommand(new SetLocalVar("cmd", new StrmRead(new GetLocalVar("s"), 1)));
			if (num > 0)
			{
				function.InsertCommand(new IfNotEqual(new GetLocalVar("cmd"), 1, "notateleport"));
				function.InsertCommand(new SetLocalVar("p", new StrmRead(new GetLocalVar("s"), 1)));
				function.InsertCommand(new SetLocalVar("x", new StrmRead(new GetLocalVar("s"), 4)));
				function.InsertCommand(new SetLocalVar("y", new StrmRead(new GetLocalVar("s"), 4)));
				function.InsertCommand(new TeleportPlayerTo(new GetLocalVar("p"), new GetLocalVar("x"), new GetLocalVar("y"), always: true));
				function.SetLabel("notateleport");
			}
		}
		function.InsertCommand(new Return());
		script.InsertFunction(function);
		script.InsertFunction(Function.EmptyFunction("levelstart"));
		return script;
	}

	public void CopyFrom(Level level)
	{
		_items0.Clear();
		_items1.Clear();
		_items2.Clear();
		_resources.CopyFrom(level._resources);
		_items0.CopyFrom(level._items0);
		_items1.CopyFrom(level._items1);
		_items2.CopyFrom(level._items2);
		_opponents.CopyFrom(level._opponents);
		_filename = level._filename;
		_vampires = level._vampires;
		_modified = level._modified;
	}

	internal void OnItemInserted(LevelItem item)
	{
		Modified = true;
		if (this.ItemInserted != null)
		{
			this.ItemInserted(this, new ItemEventArgs(item));
		}
	}

	internal void OnItemRemoved(LevelItem item)
	{
		if (item is TeleportLevelItem)
		{
			TeleportLevelItem item2 = item as TeleportLevelItem;
			foreach (LevelItem item3 in Items)
			{
				if (item3 is TeleportLevelItem)
				{
					(item3 as TeleportLevelItem).RemoveTarget(item2);
				}
			}
		}
		Modified = true;
		if (this.ItemRemoved != null)
		{
			this.ItemRemoved(this, new ItemEventArgs(item));
		}
	}

	internal void OnItemModified(LevelItem item)
	{
		Modified = true;
		if (this.ItemModified != null)
		{
			this.ItemModified(this, new ItemEventArgs(item));
		}
	}

	internal void OnResourceModified(ResourceItem item)
	{
		Modified = true;
		if (this.ResourceModified != null)
		{
			this.ResourceModified(this, new ResourceEventArgs(item));
		}
	}

	internal void OnResourceInserted(ResourceItem item)
	{
		Modified = true;
		if (this.ResourceInserted != null)
		{
			this.ResourceInserted(this, new ResourceEventArgs(item));
		}
	}

	internal void OnResourceRemoved(ResourceItem item)
	{
		Modified = true;
		if (this.ResourceRemoved != null)
		{
			this.ResourceRemoved(this, new ResourceEventArgs(item));
		}
	}

	internal void ResourceRenamed(ResourceItem item, string originalname)
	{
		if (!(item is BitmapResourceItem))
		{
			return;
		}
		foreach (LevelItem item2 in Items)
		{
			if (item2 is ImageLevelItem imageLevelItem && imageLevelItem.BitmapName == originalname)
			{
				imageLevelItem.BitmapName = item.Name;
			}
		}
	}

	internal void OnOpponentModified(Opponent opponent)
	{
		Modified = true;
		if (this.OpponentModified != null)
		{
			this.OpponentModified(this, new OpponentEventArgs(opponent));
		}
	}

	internal void OnOpponentInserted(Opponent opponent)
	{
		Modified = true;
		if (this.OpponentInserted != null)
		{
			this.OpponentInserted(this, new OpponentEventArgs(opponent));
		}
	}

	internal void OnOpponentRemoved(Opponent opponent)
	{
		Modified = true;
		if (this.OpponentRemoved != null)
		{
			this.OpponentRemoved(this, new OpponentEventArgs(opponent));
		}
	}
}
