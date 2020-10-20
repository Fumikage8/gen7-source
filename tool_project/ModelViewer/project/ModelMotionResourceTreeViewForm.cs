using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Xml;
using System.Security.AccessControl;
using System.Security.Principal;
using gfl2.clr.renderingengine.scenegraph.instance;
using gfl2.clr.renderingengine.scenegraph.resource;
using gfl2.clr.math;
using gfl2.clr.poke_3d_lib;

namespace ModelViewer
{
	public partial class ModelMotionResourceTreeViewForm : Form
	{
		private readonly ApplicationCore m_ApplicationCore;     //!< @brief データロード用
		private readonly CameraControlForm m_CameraControlForm; //!< @brief カメラ切り替え用
    private string m_ImportDirectory;                       //!< @brief ドラッグ&ドロップされたディレクトリ
    private string m_SharedDirectory;                       //!< @brief シリーズで共有するリソースが格納されたディレクトリ
		private string[] m_ReferenceGfmotFilePaths;             //!< @brief 実際に読み込みに行くgfmotファイルパス
    private string m_ReferenceGflocFilePath;                //!< @brief 実際に読み込みに行くgflocファイルパス
    private string m_ReferenceEffectNodeFilePath;           //!< @brief 実際に読み込みに行くEffectNode.xmlのファイルパス
    private bool m_IsNecessaryToConvertResources;           //!< @brief リソースのコンバートの必要/不必要の判定フラグ

    //!< @brief モデルの種類
    private enum ModelType
    {
      MODEL,
      SHADOW_MODEL
    }

    private static readonly Dictionary<ModelType, string> ModelHashFileName =
      new Dictionary<ModelType, string>
      {
        {ModelType.MODEL, "ModelHash.fnv"},
        {ModelType.SHADOW_MODEL, "ShadowModelHash.fnv"}
      };

    //!< @brief リソース共有の種類
    private static readonly string[] s_SharedResourceTypeNames = new string[]
    {
      "ANIME_SHARE",
      "MODEL_SHARE",
      "TEXTURE_SHARE"
    };

    private Dictionary<string, bool> m_NeedToShareResourceFlags;

    private Dictionary<ModelType, string> m_ReferenceGfmdlFilePathDirectory;   //!< @brief 実際に読み込みに行くgfmdlファイルパス

    private static readonly Dictionary<PokemonModelDataManager.ConvertExtensionType, string> ConvertExtensions =
      new Dictionary<PokemonModelDataManager.ConvertExtensionType, string>
      {
        {PokemonModelDataManager.ConvertExtensionType.WinOpenGL, ".wingl"},
        {PokemonModelDataManager.ConvertExtensionType.CTR, ".ctr"}
      };

		/**
		 * @brief コンストラクタ
		 * @param applicationCore データマネージャ
		 */
		public ModelMotionResourceTreeViewForm(
			ApplicationCore applicationCore,
			CameraControlForm cameraControlForm)
		{
			m_ApplicationCore = applicationCore;
			m_CameraControlForm = cameraControlForm;
      m_ImportDirectory = "";
      m_SharedDirectory = "";
      m_ReferenceGfmdlFilePathDirectory = new Dictionary<ModelType, string>();
      m_ReferenceGfmdlFilePathDirectory.Add(ModelType.MODEL, "");
      m_ReferenceGfmdlFilePathDirectory.Add(ModelType.SHADOW_MODEL, "");
      m_NeedToShareResourceFlags = new Dictionary<string, bool>();
      Array.ForEach(s_SharedResourceTypeNames, name => m_NeedToShareResourceFlags.Add(name, false));
			m_ReferenceGfmotFilePaths = null;
      m_ReferenceGflocFilePath = "";
      m_IsNecessaryToConvertResources = false;
			InitializeComponent();
		}

		/**
		 * @brief ウインドウプロシージャ上書き
		 * @param ref Message メッセージ
		 */
		protected override void WndProc(ref Message m)
		{
			const Int32 WM_NCLBUTTONDBLCLK = 0x00A3;

			if (m.Msg == WM_NCLBUTTONDBLCLK)
			{// タイトルバーのダブルクリックをシカト
				return;
			}

			base.WndProc(ref m);
		}

		/**
		 * @brief フォームロード時の処理(OnInitに相当)
		 * @param object
		 * @param EventArgs
		 */
		private void ModelMotionResourceTreeViewForm_Load(object sender, EventArgs e)
		{

		}

		private void modelMotionResourceTreeView_AfterSelect(object sender, TreeViewEventArgs e)
		{
			int selectedIndex = -1;
			selectedIndex = e.Node.Index;

			// 全ノードを通常フォントにしてから, 選択されたノードを太字にする
			m_ApplicationCore.SelectedIndexOfPokemonModelDataManagerList = selectedIndex;
			Font defaultFont = new Font("MS UI Gothic", 9);
			for (Int32 i = 0; i < modelMotionResourceTreeView.Nodes.Count; ++i)
			{
				modelMotionResourceTreeView.Nodes[i].NodeFont = defaultFont;
			}

			Font boldFont = new Font("MS UI Gothic", 9, FontStyle.Bold);
			TreeNode selectedRootNode = modelMotionResourceTreeView.Nodes[selectedIndex];
			selectedRootNode.NodeFont = boldFont;

      m_CameraControlForm.ResetAlignment();
			m_CameraControlForm.ResetCamera();
		}

    /**
     * @brief ドラッグ&ドロップ前の確認処理
     */
		private void modelMotionResourceTreeView_DragEnter(object sender, DragEventArgs e)
		{
      m_ImportDirectory = "";
      m_SharedDirectory = "";
      m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL] = "";
      m_ReferenceGfmdlFilePathDirectory[ModelType.SHADOW_MODEL] = "";
			m_ReferenceGfmotFilePaths = null;
      m_ReferenceGflocFilePath = "";
      m_ReferenceEffectNodeFilePath = "";
			e.Effect = System.Windows.Forms.DragDropEffects.All;
      foreach (string typeName in s_SharedResourceTypeNames)
      {
        m_NeedToShareResourceFlags[typeName] = false;
      }

			// ViewForm内に入ってきたものがファイルなら許可のアイコンに変更
			if (!e.Data.GetDataPresent(System.Windows.Forms.DataFormats.FileDrop))
			{
        e.Effect = System.Windows.Forms.DragDropEffects.None;
        return;
			}

      // ドラッグされたディレクトリを取得
      string[] dropFilePaths = (string[])e.Data.GetData(System.Windows.Forms.DataFormats.FileDrop);
      string dropFilePath = dropFilePaths[0].Replace("\\", "/");
      if (!System.IO.Directory.Exists(dropFilePath))
      {
        e.Effect = System.Windows.Forms.DragDropEffects.None;
        return;
      }

      string[] dropExtDataTxtFilePaths = Directory.GetFiles(dropFilePath, "ext_data.txt");
      if (dropExtDataTxtFilePaths.Length > 0)
      {// "ext_data.txt"を開いて"ANIME_SHARE"の文字列があるかどうかをチェック
        StreamReader streamReader = new StreamReader(dropExtDataTxtFilePaths[0], System.Text.Encoding.Default);
        while (streamReader.Peek() >= 0)
        {
          string readLine = streamReader.ReadLine();
          if (s_SharedResourceTypeNames.Any(name => name == readLine))
          {
            m_NeedToShareResourceFlags[readLine] = true;
          }
        }
        streamReader.Close();
      }

      string[] dropGfmdlFilePaths = System.IO.Directory.GetFiles(dropFilePath, "*.gfmdl", System.IO.SearchOption.TopDirectoryOnly);
      dropGfmdlFilePaths = Array.ConvertAll(dropGfmdlFilePaths, path => path = path.Replace("\\", "/"));

      if ((!m_NeedToShareResourceFlags["MODEL_SHARE"]) && (dropGfmdlFilePaths.Length == 0))
      {
        e.Effect = System.Windows.Forms.DragDropEffects.None;
        return;
      }

      if (dropGfmdlFilePaths.Length > Enum.GetNames(typeof(ModelType)).Length)
      {
        e.Effect = System.Windows.Forms.DragDropEffects.None;
        return;
      }

      e.Effect = System.Windows.Forms.DragDropEffects.All;
    }

    private Dictionary<ModelType, bool> DecideToConvertGfmdlResources(string exportDirectory, Dictionary<ModelType, string> gfmdlFilePathDictionary)
    {
      Dictionary<ModelType, bool> isNecessaryToConvertFlags = new Dictionary<ModelType, bool>();

      foreach (ModelType type in Enum.GetValues(typeof(ModelType)))
      {
        isNecessaryToConvertFlags.Add(type, false);
      }

      foreach (KeyValuePair<ModelType, string> keyValuePair in ModelHashFileName)
      {
        string gfmdlFilePath = gfmdlFilePathDictionary[keyValuePair.Key];
        if (!System.IO.Directory.Exists(exportDirectory))
        {
          isNecessaryToConvertFlags[keyValuePair.Key] = !string.IsNullOrEmpty(gfmdlFilePath);
          continue;
        }

        if (!System.IO.File.Exists(exportDirectory + "/" + keyValuePair.Value))
        {
          isNecessaryToConvertFlags[keyValuePair.Key] = !string.IsNullOrEmpty(gfmdlFilePath);
          continue;
        }

        // ハッシュがあって, gfmdlが存在しているのであれば, ファイル数が揃っていなければいけないので確認
        if (!ExistsConvertedGfmdlData(exportDirectory, gfmdlFilePath))
        {
          isNecessaryToConvertFlags[keyValuePair.Key] = true;
          continue;
        }

        UInt32 fnvModelHash = CreateModelDataHash(m_ReferenceGfmdlFilePathDirectory[keyValuePair.Key]);
        UInt32 fileFnvModelHash = ReadFnvHashFile(exportDirectory + "/" + keyValuePair.Value);
        isNecessaryToConvertFlags[keyValuePair.Key] = (fileFnvModelHash != fnvModelHash);
      }

      return isNecessaryToConvertFlags;
    }

    private bool ExistsConvertedGfmdlData(string exportDirectory, string gfmdlFilePath)
    {
      GfmdlResourcePathSet pathSet = CreateGfmdlResourcePathSetFromGfmdl(gfmdlFilePath);
      foreach (var extension in ConvertExtensions)
      {
        foreach (var shaderTuple in pathSet.shaderFilePathTuples)
        {
          string vertexShaderFileName = System.IO.Path.GetFileNameWithoutExtension(shaderTuple.Item1);
          vertexShaderFileName += extension.Value + ".gfbvsh";
          if (!System.IO.File.Exists(exportDirectory + "Shaders/" + vertexShaderFileName))
          {
            return false;
          }

          string geometryShaderFileName = System.IO.Path.GetFileNameWithoutExtension(shaderTuple.Item2);
          if (!string.IsNullOrEmpty(geometryShaderFileName))
          {
            geometryShaderFileName += extension.Value + ".gfbgsh";
            if (!System.IO.File.Exists(exportDirectory + "Shaders/" + geometryShaderFileName))
            {
              return false;
            }
          }

          string pixelShaderFileName = System.IO.Path.GetFileNameWithoutExtension(shaderTuple.Item3);

          pixelShaderFileName += extension.Value + ".gfbfsh";
          if (!System.IO.File.Exists(exportDirectory + "Shaders/" + pixelShaderFileName))
          {
            return false;
          }
        }

        foreach (var textureTuple in pathSet.textureFilePathTuples)
        {
          if (textureTuple.Item2 == PokemonModelDataManager.GfmdlTextureType.ExtTexture
            && extension.Key == PokemonModelDataManager.ConvertExtensionType.CTR)
          {
            continue;
          }

          string textureFileName = System.IO.Path.GetFileNameWithoutExtension(textureTuple.Item1);
          textureFileName += extension.Value + ".btex";

          if (!System.IO.File.Exists(exportDirectory + "Textures/" + textureFileName))
          {
            return false;
          }
        }

        foreach (var textureTuple in pathSet.textureRareFilePathTuples)
        {
          if (textureTuple.Item2 == PokemonModelDataManager.GfmdlTextureType.ExtTexture
            && extension.Key == PokemonModelDataManager.ConvertExtensionType.CTR)
          {
            continue;
          }

          string textureFileName = System.IO.Path.GetFileNameWithoutExtension(textureTuple.Item1);
          textureFileName += extension.Value + ".btex";

          if (!System.IO.File.Exists(exportDirectory + "Textures_rare/" + textureFileName))
          {
            return false;
          }
        }
      }

      return true;
    }

    private bool DecideToConvertGfmotResources(string exportDirectory, string[] gfmotFilePaths)
    {
      if (!System.IO.Directory.Exists(exportDirectory))
      {
        return true;
      }

      if (!System.IO.File.Exists(exportDirectory + "MotionHash.fnv"))
      {
        return true;
      }

      string[] gfbmotFilePath = Array.ConvertAll(gfmotFilePaths, path => path.Replace(".gfmot", ".gfbmot"));
      if (!Array.TrueForAll(gfbmotFilePath, path => System.IO.File.Exists(path)))
      {
        return true;
      }

      UInt32 fnvMotionHash = CreateMotionDataHash(m_ReferenceGfmotFilePaths);
      UInt32 fileFnvMotionHash = ReadFnvHashFile(exportDirectory + "MotionHash.fnv");
      return (fileFnvMotionHash != fnvMotionHash);
    }

    /**
     * @brief ドラッグ&ドロップ後の処理
     */
		private void modelMotionResourceTreeView_DragDrop(object sender, DragEventArgs e)
		{
      // ドラッグされたディレクトリを取得
      string[] m_ImportDirectorys = (string[])e.Data.GetData(System.Windows.Forms.DataFormats.FileDrop);
      string dropFilePath = m_ImportDirectorys[0].Replace("\\", "/");
      m_ImportDirectory = dropFilePath + "/";

      m_SharedDirectory = FindSharedResourceFilePath(dropFilePath) + "/";
      if (!Directory.Exists(m_SharedDirectory))
      {
        m_SharedDirectory = m_ImportDirectory;
      }

      // 通常モデルと影モデルのパスの読み込み
      string[] dropGfmdlFilePaths = System.IO.Directory.GetFiles(dropFilePath, "*.gfmdl", System.IO.SearchOption.TopDirectoryOnly);
      dropGfmdlFilePaths = Array.ConvertAll(dropGfmdlFilePaths, path => path = path.Replace("\\", "/"));
      m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL] = Array.Find(dropGfmdlFilePaths, path => !path.Contains("_shadow.gfmdl"));
      m_ReferenceGfmdlFilePathDirectory[ModelType.SHADOW_MODEL] = Array.Find(dropGfmdlFilePaths, path => path.Contains("_shadow.gfmdl"));

      // 読み込まれたgfmdlが正しいテクスチャの構造になっているかをチェック
      GfmdlErrorChecker gfmdlErrorChecker = new GfmdlErrorChecker(m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL]);
      var incorrectTextureFormatMaterials = gfmdlErrorChecker.CountUpIncorrectTextureFormatMaterials();
      foreach (var materialTextureTuple in incorrectTextureFormatMaterials)
      {
        string errorMessage = "Material : " + materialTextureTuple.Item1 + " にカラーテクスチャ以外のテクスチャが入っています." + Environment.NewLine;
        errorMessage += "Texture : " + materialTextureTuple.Item2 + " のフォーマットを確認してください.";
        MessageBox.Show(errorMessage, "Material Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
      if (incorrectTextureFormatMaterials.Length > 0)
      {// 間違っている設定のものが一つでもあったらここで終了
        return;
      }

      if (m_NeedToShareResourceFlags["MODEL_SHARE"])
      {
        string[] sharedGfmdlResourcePaths = Directory.GetFiles(m_SharedDirectory, "*gfmdl");
        m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL] = Array.Find(sharedGfmdlResourcePaths, path => !path.Contains("_shadow.gfmdl"));
        m_ReferenceGfmdlFilePathDirectory[ModelType.SHADOW_MODEL] = Array.Find(sharedGfmdlResourcePaths, path => path.Contains("_shadow.gfmdl"));
      }

      m_ReferenceGfmotFilePaths = System.IO.Directory.GetFiles(dropFilePath, "*.gfmot", System.IO.SearchOption.TopDirectoryOnly);

      if (m_NeedToShareResourceFlags["ANIME_SHARE"])
      {
        m_ReferenceGfmotFilePaths = System.IO.Directory.GetFiles(m_SharedDirectory, "*.gfmot", System.IO.SearchOption.TopDirectoryOnly);
      }
      Array.ForEach(m_ReferenceGfmotFilePaths, path => path.Replace("\\", "/"));

      string[] dropGflocFilePaths = System.IO.Directory.GetFiles(dropFilePath, "*.gfloc", System.IO.SearchOption.TopDirectoryOnly);
      if (dropGflocFilePaths.Length == 0)
      {
        dropGflocFilePaths = System.IO.Directory.GetFiles(m_SharedDirectory, "*.gfloc", System.IO.SearchOption.TopDirectoryOnly);
      }
      if (dropGflocFilePaths.Length > 0)
      {
        Array.ForEach(dropGflocFilePaths, path => path.Replace("\\", "/"));
        m_ReferenceGflocFilePath = dropGflocFilePaths[0];
      }

      string[] dropEffectNodeFilePaths = System.IO.Directory.GetFiles(dropFilePath, "EffectNode.xml", System.IO.SearchOption.TopDirectoryOnly);
      if ((dropEffectNodeFilePaths.Length == 0) || m_NeedToShareResourceFlags["ANIME_SHARE"])
      {// 見つからなかったら他のシリーズの関連ディレクトリを探しに行く
        m_ReferenceEffectNodeFilePath = m_SharedDirectory + "EffectNode.xml";
      }
      else
      {
        m_ReferenceEffectNodeFilePath = dropEffectNodeFilePaths[0];
      }

      m_IsNecessaryToConvertResources = DecideToConvertResources();

      // gfmdlのリソースをコンバート
      GfMdlResourcePathSetDictionary[] pathSetDictionaries = ProceedGfmdlConvert();

      // レアカラーのテクスチャのサイズの比較調査
      Array.ForEach(pathSetDictionaries, pathSet => CheckDifferenceWithNormalTextureAndRareTexture(pathSet));

      // EffectNode.xmlをEffectNode.gfblocにコンバート
      GfblocPathSet gfblocPathSet = ProceedGfblocConvert();

      // その他のファイルのコンバート
      ExtraFilePathSet extraFilePathSet = ProceedExtraFileConvert();

      // gfmotのリソースをコンバート
      GfmotPathSet gfmotPathSet = ProceedGfmotConvert();

      LoadGfResources(
        m_ImportDirectory,
        pathSetDictionaries[0],
        pathSetDictionaries[1],
        gfmotPathSet,
        gfblocPathSet,
        extraFilePathSet,
        2.0m,
        true,
        false);
    }

    private void CheckDifferenceWithNormalTextureAndRareTexture(GfMdlResourcePathSetDictionary pathSetDictionary)
    {
      // レアのテクスチャと通常のテクスチャでサイズに差がないかどうかをチェック
      // (CTRのテクスチャのみをチェック. winglは無視)
      if (pathSetDictionary == null)
      {
        return;
      }

      var diffTexturePathTuples = pathSetDictionary.CheckConvertedTextureFileSizeDiff(PokemonModelDataManager.ConvertExtensionType.CTR);
      foreach (var diffTexturePathTuple in diffTexturePathTuples)
      {
        FileInfo textureFileInfo = new FileInfo(diffTexturePathTuple.Item1);
        FileInfo rareTextureFileInfo = new FileInfo(diffTexturePathTuple.Item2);
        string textureFileName = Path.GetFileName(diffTexturePathTuple.Item1);
        string message =
          "テクスチャ名 : " + textureFileName + Environment.NewLine +
          "通常 : " + textureFileInfo.Length + " [Byte]" + Environment.NewLine +
          "レア : " + rareTextureFileInfo.Length + " [Byte]";
        MessageBox.Show(message,
          "通常テクスチャとレアテクスチャのサイズが異なります",
          MessageBoxButtons.OK,
          MessageBoxIcon.Exclamation,
          MessageBoxDefaultButton.Button1,
          MessageBoxOptions.DefaultDesktopOnly);
      }
    }

    private bool DecideToConvertResources()
    {
      string exportDirectory = Path.GetDirectoryName(m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL]).Replace("\\", "/") + "/";
      exportDirectory += "converted_data/";

      Dictionary<ModelType, bool> isNecessaryToConvertFlags = DecideToConvertGfmdlResources(exportDirectory, m_ReferenceGfmdlFilePathDirectory);

      // コンバートの必要があるならディレクトリを削除
      foreach (var keyValuePair in isNecessaryToConvertFlags)
      {
        if (keyValuePair.Value == true)
        {
          return true;
        }
      }

      if (m_ReferenceGfmotFilePaths == null)
      {
        return false;
      }

      if (m_ReferenceGfmotFilePaths.Length == 0)
      {
        return false;
      }

      string sharedGfmotDirectory = Path.GetDirectoryName(m_ReferenceGfmotFilePaths[0]).Replace("\\", "/") + "/";
      string exportGfmotDirectory = sharedGfmotDirectory + "converted_data/";
      if (!Directory.Exists(exportGfmotDirectory))
      {
        return false;
      }

      // gfmotファイル群のリソースのコンバート
      string[] gfmotFilePaths = Array.ConvertAll(m_ReferenceGfmotFilePaths, path => exportGfmotDirectory + System.IO.Path.GetFileName(path));
      bool isNecessaryToConvert = DecideToConvertGfmotResources(exportGfmotDirectory, gfmotFilePaths);

      return isNecessaryToConvert;
    }

    private GfMdlResourcePathSetDictionary[] ProceedGfmdlConvert()
    {
      string exportDirectory = Path.GetDirectoryName(m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL]).Replace("\\", "/") + "/";
      exportDirectory += "converted_data/";

      Dictionary<ModelType, bool> isNecessaryToConvertFlags = DecideToConvertGfmdlResources(exportDirectory, m_ReferenceGfmdlFilePathDirectory);

      if (m_IsNecessaryToConvertResources)
      {
        if (System.IO.Directory.Exists(exportDirectory))
        {
          Util.DeleteDirectory(exportDirectory);
        }
        System.IO.Directory.CreateDirectory(exportDirectory);
        // 作成したディレクトリにリソースをコピー
        foreach (KeyValuePair<ModelType, string> keyValuePair in m_ReferenceGfmdlFilePathDirectory)
        {
          if (!System.IO.File.Exists(keyValuePair.Value))
          {
            continue;
          }

          CopyPokemonModelGfmdlResourceFiles(keyValuePair.Value, exportDirectory);
          CreateModelDataHashFile(exportDirectory, keyValuePair.Value, keyValuePair.Key);
        }
      }

      GfMdlResourcePathSetDictionary[] pathSetDictionaries = new GfMdlResourcePathSetDictionary[Enum.GetNames(typeof(ModelType)).Length];
      Int32 dictionaryCount = 0;
      foreach (KeyValuePair<ModelType, string> keyValuePair in ModelHashFileName)
      {
        // DivideTexAnim起動後, リソースのあるファイルパスを生成
        string gfmdlFilePath = exportDirectory + System.IO.Path.GetFileName(m_ReferenceGfmdlFilePathDirectory[keyValuePair.Key]);

        if (!System.IO.File.Exists(gfmdlFilePath))
        {
          ++dictionaryCount;
          continue;
        }

        // gfmdlファイルのリソースを収集
        GfmdlResourcePathSet originalPathSet = CreateGfmdlResourcePathSetFromGfmdl(gfmdlFilePath);

        // リソースをコンバート
        var pathSetDictionary = CreateGfmdlResourcePathSetDictionary(originalPathSet, exportDirectory, isNecessaryToConvertFlags[keyValuePair.Key]);
        pathSetDictionaries[dictionaryCount] = pathSetDictionary;
        ++dictionaryCount;
      }

      return pathSetDictionaries;
    }

    private ExtraFilePathSet ProceedExtraFileConvert()
    {
      string exportDirectory = Path.GetDirectoryName(m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL]).Replace("\\", "/") + "/";
      exportDirectory += "converted_data/";

      // モーションに関連する各種リソースは"ANIME_SHARE"の設定になっていたら共有する
      string[] mcnfPathList = FindFilePathList(exportDirectory, "*.mcnf");
      string blendListXmlPath = FindBlendListXmlFilePath(exportDirectory);
      string eachDaeXmlPath = FindFilePathFirst(exportDirectory, "EachDae.xml");
      if (m_NeedToShareResourceFlags["ANIME_SHARE"])
      {
        mcnfPathList = FindFilePathList(m_SharedDirectory, "*.mcnf");
        blendListXmlPath = FindBlendListXmlFilePath(m_SharedDirectory);
        eachDaeXmlPath = FindFilePathFirst(m_SharedDirectory, "EachDae.xml");
      }

      ExtraFilePathSet extraFilePathSet = new ExtraFilePathSet(mcnfPathList, blendListXmlPath, eachDaeXmlPath);
      return extraFilePathSet;
    }

    private GfmotPathSet ProceedGfmotConvert()
    {
      if (m_ReferenceGfmotFilePaths == null)
      {
        return new GfmotPathSet();
      }

      if (m_ReferenceGfmotFilePaths.Length == 0)
      {
        return new GfmotPathSet();
      }

      string sharedGfmotDirectory = Path.GetDirectoryName(m_ReferenceGfmotFilePaths[0]).Replace("\\", "/") + "/";
      string exportGfmotDirectory = sharedGfmotDirectory + "converted_data/";
      if (!Directory.Exists(exportGfmotDirectory))
      {
        Directory.CreateDirectory(exportGfmotDirectory);
      }

      // gfmotファイル群のリソースのコンバート
      string[] gfmotFilePaths = Array.ConvertAll(m_ReferenceGfmotFilePaths, path => exportGfmotDirectory + System.IO.Path.GetFileName(path));
      bool isNecessaryToConvertGfmot = DecideToConvertGfmotResources(exportGfmotDirectory, gfmotFilePaths);

      string[] gfbmotFilePaths;
      if (m_IsNecessaryToConvertResources)
      {
        // ディレクトリ以下にgfmotをコピーしてハッシュを作る
        CopyResourceFiles(m_ReferenceGfmotFilePaths, exportGfmotDirectory);
        CreateMotionDataHashFile(exportGfmotDirectory, m_ReferenceGfmotFilePaths);

        // DivideTexAnimBatchを起動
        string gfmdlFileNameWithoutExtension = Path.GetFileNameWithoutExtension(m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL]);
        var match_pmXXXX_YY = PokemonModelDataManager.s_pmXXXX_YY.Match(gfmdlFileNameWithoutExtension);
        string pmXXXX_YY = match_pmXXXX_YY.ToString();
        if (gfmdlFileNameWithoutExtension == pmXXXX_YY)
        {
          ExecuteDivideTexAnimBatch(exportGfmotDirectory);
        }

        // DivideTexAnimを起動した後でコンバート
        gfbmotFilePaths = ConvertGfmotResources(gfmotFilePaths, exportGfmotDirectory);
      }
      else
      {
        gfbmotFilePaths = Array.ConvertAll(gfmotFilePaths, path => path.Replace(".gfmot", ".gfbmot"));
      }
      GfmotPathSet gfmotPathSet = new GfmotPathSet(gfbmotFilePaths);
      return gfmotPathSet;
    }

    private GfblocPathSet ProceedGfblocConvert()
    {
      // EffectNode.xmlをEffectNode.gfblocにコンバート
      GfblocPathSet gfblocPathSet = new GfblocPathSet();
      string sharedEffectNodeResourceFilePath = Path.GetDirectoryName(m_ReferenceEffectNodeFilePath).Replace("\\", "/") + "/";
      string sharedConvertedEffectNodeResourceFilePath = sharedEffectNodeResourceFilePath + "converted_data/";
      CopyPokemonModelExtraResourceFiles(sharedEffectNodeResourceFilePath, sharedConvertedEffectNodeResourceFilePath);
      if (System.IO.File.Exists(sharedConvertedEffectNodeResourceFilePath + "EffectNode.xml"))
      {
        gfblocPathSet[GfblocPathSet.LocatorType.EFFECT] = ConvertEffectNodeXml(
          sharedConvertedEffectNodeResourceFilePath + "EffectNode.xml",
          sharedConvertedEffectNodeResourceFilePath);
      }

      // gflocファイルがあれば一括でコンバート
      if (!string.IsNullOrEmpty(m_ReferenceGflocFilePath))
      {
        string sharedPokedecoLocatorResourceFilePath = Path.GetDirectoryName(m_ReferenceGflocFilePath).Replace("\\", "/") + "/";
        string sharedConvertedPokedecoLocatorResourceFilePath = sharedPokedecoLocatorResourceFilePath + "converted_data/";
        CopyPokemonModelExtraResourceFiles(sharedPokedecoLocatorResourceFilePath, sharedConvertedPokedecoLocatorResourceFilePath);
        if (System.IO.File.Exists(sharedConvertedPokedecoLocatorResourceFilePath + System.IO.Path.GetFileName(m_ReferenceGflocFilePath)))
        {
          gfblocPathSet[GfblocPathSet.LocatorType.POKEDECO] = ConvertGflocResource(
            sharedConvertedPokedecoLocatorResourceFilePath + System.IO.Path.GetFileName(m_ReferenceGflocFilePath),
            sharedConvertedPokedecoLocatorResourceFilePath);
        }
      }

      return gfblocPathSet;
    }

    private string FindSharedResourceFilePath(string exportDirectory)
    {
      // 検索対象のファイルがなかったら, 名前が一致しているディレクトリを検索
      string sharedResourceFilePath = "";
      try
      {
        var pmXXXX_YYDirectoryInfo = Directory.GetParent(exportDirectory);
        if (pmXXXX_YYDirectoryInfo == null)
        {
          return exportDirectory;
        }
        var pmSeriesDirectoryInfo = Directory.GetParent(pmXXXX_YYDirectoryInfo.FullName);
        if (pmSeriesDirectoryInfo == null)
        {
          return exportDirectory;
        }
        var pmAllSeriesDirectoryInfo = Directory.GetParent(pmSeriesDirectoryInfo.FullName);
        if (pmAllSeriesDirectoryInfo == null)
        {
          return exportDirectory;
        }

        var pmXXXX_YY = PokemonModelDataManager.s_pmXXXX.Match(pmXXXX_YYDirectoryInfo.FullName);
        string pmXXXX_YY_string = pmXXXX_YY.Value;
        if (string.IsNullOrEmpty(pmXXXX_YY_string))
        {
          return exportDirectory;
        }
        string[] allDirectories = Directory.GetDirectories(pmAllSeriesDirectoryInfo.FullName, pmXXXX_YY_string + "*", SearchOption.AllDirectories);
        string[] trimmedAllDirectories = Array.ConvertAll(allDirectories, path => path.Replace(pmAllSeriesDirectoryInfo.FullName, ""));
        trimmedAllDirectories = Array.FindAll(trimmedAllDirectories, directory => directory.Where(c => c == '\\').Count() <= 2);
        string[] sortedAllDirectories = PokemonUtility.SortBySeriesName(trimmedAllDirectories);
        if (sortedAllDirectories.Length == 0)
        {// シリーズの命名規則に則っていないディレクトリの場合, 見つからないのでexportsをそのまま返す
          return exportDirectory;
        }
        sortedAllDirectories = Array.ConvertAll(sortedAllDirectories, path => path = (pmAllSeriesDirectoryInfo.FullName + path).Replace("\\", "/"));

        string restPath = exportDirectory.Replace(pmXXXX_YYDirectoryInfo.FullName.Replace("\\", "/"), "");
        sharedResourceFilePath = sortedAllDirectories[0] + restPath;
      }
      catch (DirectoryNotFoundException e)
      {
        return exportDirectory;
      }

      return sharedResourceFilePath;
    }

    private string FindFilePathFirst(string searchDirectory, string searchPattern)
    {
      return FindFilePathList(searchDirectory, searchPattern).First();
    }

    private string[] FindFilePathList(string searchDirectory, string searchPattern)
    {
      var filePaths = Directory.GetFiles(searchDirectory, searchPattern);
      if ((filePaths == null) || (filePaths.Length == 0) || (string.IsNullOrEmpty(filePaths[0])))
      {
        return new string[] { string.Empty };
      }

      return filePaths;
    }

    private string FindBlendListXmlFilePath(string searchDirectory)
    {
      // gfbmdl, gfbmot, gfblocファイル以外に必要なファイルをまとめる
      var blendListPaths = Directory.GetFiles(searchDirectory, "blend_list.xml");
      if ((blendListPaths == null) || (blendListPaths.Length == 0) || (string.IsNullOrEmpty(blendListPaths[0])))
      {
        // デフォルトのブレンドリストを読み込む
        var default_list_path = "data/BlendList/pm_default_blend_list.xml";
#if DEBUG
        default_list_path = "../../../" + default_list_path;
#endif
        return default_list_path;
      }
      
      return blendListPaths[0];
    }

    private GfMdlResourcePathSetDictionary CreateGfmdlResourcePathSetDictionary(GfmdlResourcePathSet pathSet, string exportDirectory, bool isNecessaryToConvert)
    {
      GfMdlResourcePathSetDictionary pathSetDictionary = new GfMdlResourcePathSetDictionary();
      foreach (var convertExtension in ConvertExtensions)
      {
        GfmdlResourcePathSet originalPathSet = pathSet;
        if (isNecessaryToConvert)
        {
          originalPathSet = ConvertGfmdlResourceFromPathSet(originalPathSet, exportDirectory, convertExtension.Key);
        }
        else
        {
          originalPathSet = ReplaceWithConvertedGfmdlResourcePathSet(originalPathSet, exportDirectory, convertExtension.Key);
        }
        pathSetDictionary.Add(convertExtension.Key, originalPathSet);
      }
      return pathSetDictionary;
    }

    private GfmdlResourcePathSet CreateGfmdlResourcePathSetFromGfmdl(string gfmdlFilePath)
    {
      GfmdlResourcePathSet pathSet;
      pathSet.gfmdlFilePath = gfmdlFilePath;
      pathSet.gfbmdlFilePath = "";
      pathSet.shaderFilePathTuples = CreateUsedShaderTuples(gfmdlFilePath);
      pathSet.textureFilePathTuples = CreateUsedTextureTuples(gfmdlFilePath);
      pathSet.textureRareFilePathTuples = null;
      var textureRareFilePathTupleList = new List<Tuple<string, PokemonModelDataManager.GfmdlTextureType>>();
      for (var i = 0; i < pathSet.textureFilePathTuples.Length; ++i)
      {
        string rareTextureFilePath = pathSet.textureFilePathTuples[i].Item1.Replace("/Textures/", "/Textures_rare/");
        if (!System.IO.File.Exists(rareTextureFilePath))
        {
          if (!m_NeedToShareResourceFlags["TEXTURE_SHARE"])
          {// ext_data.txtにテクスチャ共有設定がなければ無視
            continue;
          }

          if (!System.IO.Directory.Exists(m_SharedDirectory + "converted_data/Textures_rare/"))
          {
            continue;
          }

          string sharedRareTextureFilePath = m_SharedDirectory + "converted_data/Textures_rare/" + Path.GetFileName(rareTextureFilePath);
          if (!System.IO.File.Exists(sharedRareTextureFilePath))
          {
            continue;
          }
          rareTextureFilePath = sharedRareTextureFilePath;
        }

        textureRareFilePathTupleList.Add(new Tuple<string, PokemonModelDataManager.GfmdlTextureType>(rareTextureFilePath, pathSet.textureFilePathTuples[i].Item2));
      }

      pathSet.textureRareFilePathTuples = textureRareFilePathTupleList.ToArray();

      return pathSet;
    }

    private GfmdlResourcePathSet ConvertGfmdlResourceFromPathSet(GfmdlResourcePathSet pathSet, string exportDirectory, PokemonModelDataManager.ConvertExtensionType convertType)
    {
      GfmdlResourcePathSet outputPathSet;
      outputPathSet.gfmdlFilePath = pathSet.gfmdlFilePath;
      outputPathSet.gfbmdlFilePath = ConvertGfmdlResource(pathSet.gfmdlFilePath, exportDirectory, convertType);
      outputPathSet.shaderFilePathTuples = ConvertShaderResources(pathSet.shaderFilePathTuples, exportDirectory + "Shaders/", convertType);
      outputPathSet.textureFilePathTuples = ConvertTextureResources(pathSet.textureFilePathTuples, exportDirectory + "Textures/", convertType);
      if (pathSet.textureRareFilePathTuples == null)
      {
        outputPathSet.textureRareFilePathTuples = null;
        return outputPathSet;
      }

      outputPathSet.textureRareFilePathTuples = null; 
      if (pathSet.textureRareFilePathTuples != null)
      {
        if (pathSet.textureRareFilePathTuples.Length > 0)
        {
          string rareColorExportDirectory = Path.GetDirectoryName(pathSet.textureRareFilePathTuples[0].Item1).Replace("\\", "/") + "/";
          outputPathSet.textureRareFilePathTuples = ConvertTextureResources(pathSet.textureRareFilePathTuples, rareColorExportDirectory, convertType);
        }
      }

      return outputPathSet;
    }

    private GfmdlResourcePathSet ReplaceWithConvertedGfmdlResourcePathSet(GfmdlResourcePathSet pathSet, string exportDirectory, PokemonModelDataManager.ConvertExtensionType convertType)
    {
      GfmdlResourcePathSet outputPathSet;
      outputPathSet.gfmdlFilePath = pathSet.gfmdlFilePath;
      outputPathSet.gfbmdlFilePath = pathSet.gfmdlFilePath.Replace(".gfmdl", ConvertExtensions[convertType] + ".gfbmdl");
      outputPathSet.shaderFilePathTuples = ReplaceShaderFileTuples(pathSet.shaderFilePathTuples, convertType);
      outputPathSet.textureFilePathTuples = ReplaceTextureFileTuples(pathSet.textureFilePathTuples, convertType);
      if (pathSet.textureRareFilePathTuples == null)
      {
        outputPathSet.textureRareFilePathTuples = null;
        return outputPathSet;
      }

      outputPathSet.textureRareFilePathTuples = null;
      if (pathSet.textureRareFilePathTuples != null)
      {
        if (pathSet.textureRareFilePathTuples.Length > 0)
        {
          outputPathSet.textureRareFilePathTuples = ReplaceTextureFileTuples(pathSet.textureRareFilePathTuples, convertType);
        }
      }
      return outputPathSet;
    }

    /**
     * @brief gfmdlファイルのコンバート
     * @param gfmdlFilePath gfmdlファイルへのパス
     * @return string コンバート後のファイルパス
     */
    private string ConvertGfmdlResource(
      string gfmdlFilePath,
      string exportDirectory,
      PokemonModelDataManager.ConvertExtensionType convertExtensionType)
    {
      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");

      // モデルのバイナリサイズをコンバートして計測
      string gfmdlConverterFilePath =
#if DEBUG
      Path.Combine(nijiToolDir, "ModelConverter", "ModelConverter.exe");
#else
      Path.GetFullPath("../ModelConverter/ModelConverter.exe");
      if (!File.Exists(gfmdlConverterFilePath))
      {
        gfmdlConverterFilePath = Path.Combine(nijiToolDir, "ModelConverter", "ModelConverter.exe");
      }
#endif
      if (!File.Exists(gfmdlConverterFilePath))
      {
        MessageBox.Show("モデルコンバータ : " + gfmdlConverterFilePath + " が見つかりません。");
        Application.Exit();
        return "";
      }

      ProcessStartInfo startInfo = new ProcessStartInfo(gfmdlConverterFilePath);
      startInfo.WindowStyle = ProcessWindowStyle.Hidden;
      startInfo.RedirectStandardError = true;
      startInfo.RedirectStandardOutput = true;
      startInfo.CreateNoWindow = true;
      startInfo.UseShellExecute = false;

      string bgfmdlFileName = gfmdlFilePath.Replace(".gfmdl", ConvertExtensions[convertExtensionType] + ".gfbmdl");
      bgfmdlFileName = System.IO.Path.GetFileName(bgfmdlFileName);
      string bgfmdlFilePath = exportDirectory + bgfmdlFileName;

      startInfo.Arguments =
        "-i \"" + gfmdlFilePath + "\"" +
        " -o \"" + bgfmdlFilePath + "\"";
      Process process = Process.Start(startInfo);
      process.WaitForExit();
      if (process.ExitCode != 0)
      {
        MessageBox.Show(
          "ModelConverter.exe doesn't work correctly." + Environment.NewLine
          + "Error Code = " + process.ExitCode + Environment.NewLine
          + "StandardOutput : " + process.StandardOutput.ReadToEnd() + Environment.NewLine
          + "StandardError : " + process.StandardError.ReadToEnd() + Environment.NewLine
          + "Input Path = " + gfmdlFilePath + Environment.NewLine
          + "Output Path = " + bgfmdlFilePath,
          "ModelConverter Error!",
          MessageBoxButtons.OK,
          MessageBoxIcon.Stop);
      }

      return bgfmdlFilePath;
    }

    /**
     * @brief gfvsh, gfgsh, gffshのリソースのタプルを作成
     * @param gfmdlFilePath gfmdlファイルパス
     */
    private Tuple<string, string, string>[] CreateUsedShaderTuples(string gfmdlFilePath)
    {
      // 頂点シェーダとピクセルシェーダの対になっているパスをgfmdlから取得
      XmlDocument gfmdlDocument = new XmlDocument();
      gfmdlDocument.Load(gfmdlFilePath);
      XmlElement rootElement = gfmdlDocument.DocumentElement;
      XmlNodeList nodeShaderListList = rootElement.GetElementsByTagName("ShaderList");
      XmlNodeList nodeShaderList = nodeShaderListList[0].ChildNodes;

      string gfmdlDirectory = System.IO.Path.GetDirectoryName(gfmdlFilePath).Replace("\\", "/") + "/";
      var shaderTupleList = new List<Tuple<string, string, string>>();
      for (Int32 i = 0; i < nodeShaderList.Count; ++i)
      {
        string vertexShaderPath = "";
        string geometryShaderPath = "";
        string pixelShaderPath = "";
        for (Int32 j = 0; j < nodeShaderList[i].ChildNodes.Count; ++j)
        {
          var childNode = nodeShaderList[i].ChildNodes[j];
          if (childNode.Name == "VshFilePath")
          {
            vertexShaderPath = gfmdlDirectory + childNode.InnerText;
          }
          else if (childNode.Name == "GshFilePath")
          {
            geometryShaderPath = gfmdlDirectory + childNode.InnerText;
          }
          else if (childNode.Name == "FshFilePath")
          {
            pixelShaderPath = gfmdlDirectory + childNode.InnerText;
          }
        }
        var tuple = new Tuple<string, string, string>(vertexShaderPath, geometryShaderPath, pixelShaderPath);
        shaderTupleList.Add(tuple);
      }

      return shaderTupleList.ToArray();
    }

    /**
     * @brief シェーダのコンバート
     * @param shaderFilePathTuples シェーダのタプル配列(gfvsh, gfgsh, gffsh)
     * @param exportDirectory 出力先のディレクトリ
     * @return Tuple<string, string, string> 各種シェーダのタプル
     */
    private Tuple<string, string, string>[] ConvertShaderResources(
      Tuple<string, string, string>[] shaderFilePathTuples,
      string exportDirectory,
      PokemonModelDataManager.ConvertExtensionType convertExtensionType)
    {
      List<Tuple<string, string, string>> exportShaderResourceList = new List<Tuple<string, string, string>>();
      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");

      // シェーダのバイナリサイズをコンバートして計測
      string shaderConverterFilePath =
#if DEBUG
      Path.Combine(nijiToolDir, "ShaderCompiler", "ShaderCompiler.exe");
#else
        Path.GetFullPath("../ShaderCompiler/ShaderCompiler.exe");
#endif
      if (!File.Exists(shaderConverterFilePath))
      {
        MessageBox.Show("シェーダコンバータ : " + shaderConverterFilePath + " が見つかりません。");
        Application.Exit();
        return exportShaderResourceList.ToArray();
      }

      Process process;
      ProcessStartInfo startInfo = new ProcessStartInfo(shaderConverterFilePath);
      startInfo.WindowStyle = ProcessWindowStyle.Hidden;
      startInfo.RedirectStandardError = true;
      startInfo.RedirectStandardOutput = true;
      startInfo.CreateNoWindow = true;
      startInfo.UseShellExecute = false;

      foreach (var shaderFilePathTuple in shaderFilePathTuples)
      {
        string extension = ConvertExtensions[convertExtensionType];
        string outputVertexShaderName = System.IO.Path.GetFileName(shaderFilePathTuple.Item1.Replace(".gfvsh", extension + ".gfbvsh"));
        string outputGeometryShaderName = System.IO.Path.GetFileName(shaderFilePathTuple.Item2.Replace(".gfgsh", extension + ".gfbgsh"));
        string outputPixelShaderName = System.IO.Path.GetFileName(shaderFilePathTuple.Item3.Replace(".gffsh", extension + ".gfbfsh"));
        string outputVertexShaderPath = exportDirectory + outputVertexShaderName;
        string outputGeometryShaderPath = "";
        if (!string.IsNullOrEmpty(outputGeometryShaderName))
        {
          outputGeometryShaderPath = exportDirectory + outputGeometryShaderName;
        }
        string outputPixelShaderPath = exportDirectory + outputPixelShaderName;

        // まずは頂点シェーダのコンバート
        startInfo.Arguments = " -vsh \"" + shaderFilePathTuple.Item1 + "\"";
        if (!string.IsNullOrEmpty(outputGeometryShaderName))
        {
          startInfo.Arguments += " -gsh \"" + shaderFilePathTuple.Item2 + "\"";
        }
        startInfo.Arguments += " -fsh \"" + shaderFilePathTuple.Item3 + "\"";
        startInfo.Arguments += " -o \"" + outputVertexShaderPath + "\" ";
        startInfo.Arguments += " -def HLSL -def 1";

        process = Process.Start(startInfo);
        process.WaitForExit();
        if (process.ExitCode != 0)
        {
          MessageBox.Show(
            "ShaderCompiler.exe doesn't work correctly. Could not compile a vertex shader." + Environment.NewLine
            + "Error Code = " + process.ExitCode + Environment.NewLine
            + "StandardOutput : " + process.StandardOutput.ReadToEnd() + Environment.NewLine
            + "StandardError : " + process.StandardError.ReadToEnd() + Environment.NewLine
            + "Input Path = " + shaderFilePathTuple.Item1 + Environment.NewLine
            + "Output Path = " + outputVertexShaderPath,
            "VertexShader Compile Error!",
            MessageBoxButtons.OK,
            MessageBoxIcon.Stop);
        }

        if (!string.IsNullOrEmpty(outputGeometryShaderName))
        {// ジオメトリシェーダが存在していたらジオメトリシェーダのコンバート
          startInfo.Arguments =
          " -vsh \"" + shaderFilePathTuple.Item1 + "\"" +
          " -gsh \"" + shaderFilePathTuple.Item2 + "\"" +
          " -fsh \"" + shaderFilePathTuple.Item3 + "\"" +
          " -o \"" + outputGeometryShaderPath + "\" " +
          " -def HLSL -def 1";
          process = Process.Start(startInfo);
          process.WaitForExit();
          if (process.ExitCode != 0)
          {
            MessageBox.Show(
              "ShaderCompiler.exe doesn't work correctly. Could not compile a geometry shader." + Environment.NewLine
              + "Error Code = " + process.ExitCode + Environment.NewLine
              + "StandardOutput : " + process.StandardOutput.ReadToEnd() + Environment.NewLine
              + "StandardError : " + process.StandardError.ReadToEnd() + Environment.NewLine
              + "Input Path = " + shaderFilePathTuple.Item2 + Environment.NewLine
              + "Output Path = " + outputGeometryShaderPath,
              "GeometryShader Compile Error!",
              MessageBoxButtons.OK,
              MessageBoxIcon.Stop);
          }
        }

        // 続いてピクセルシェーダのコンバート
        startInfo.Arguments = " -vsh \"" + shaderFilePathTuple.Item1 + "\"";
        if (!string.IsNullOrEmpty(outputGeometryShaderName))
        {
          startInfo.Arguments += " -gsh \"" + shaderFilePathTuple.Item2 + "\"";
        }
        startInfo.Arguments += " -fsh \"" + shaderFilePathTuple.Item3 + "\"";
        startInfo.Arguments += " -o \"" + outputPixelShaderPath + "\" ";
        startInfo.Arguments += " -def HLSL -def 1";
        process = Process.Start(startInfo);
        process.WaitForExit();
        if (process.ExitCode != 0)
        {
          MessageBox.Show(
            "ShaderCompiler.exe doesn't work correctly. Could not compile a pixel shader." + Environment.NewLine
            + "Error Code = " + process.ExitCode + Environment.NewLine
            + "StandardOutput : " + process.StandardOutput.ReadToEnd() + Environment.NewLine
            + "StandardError : " + process.StandardError.ReadToEnd() + Environment.NewLine
            + "Input Path = " + shaderFilePathTuple.Item3 + Environment.NewLine
            + "Output Path = " + outputPixelShaderPath,
            "PixelShader Compile Error!",
            MessageBoxButtons.OK,
            MessageBoxIcon.Stop);
        }

        exportShaderResourceList.Add(
          new Tuple<string, string, string>(
            outputVertexShaderPath,
            outputGeometryShaderPath,
            outputPixelShaderPath));
      }

      return exportShaderResourceList.ToArray();
    }

    /**
     * @brief gfvsh, gfgsh, テクスチャのリソースのタプルを作成
     * @param gfmdlFilePath gfmdlファイルパス
     * @return Tuple<string, PokemonModelDataManager.GfmdlTextureType> 使用テクスチャ
     */
    private Tuple<string, PokemonModelDataManager.GfmdlTextureType>[] CreateUsedTextureTuples(string gfmdlFilePath)
    {
      XmlDocument gfmdlDocument = new XmlDocument();
      gfmdlDocument.Load(gfmdlFilePath);
      XmlElement rootElement = gfmdlDocument.DocumentElement;
      XmlNodeList nodeTextureListList = rootElement.GetElementsByTagName("TextureList");
      XmlNodeList nodeTextureList = nodeTextureListList[0].ChildNodes;

      string gfmdlDirectory = System.IO.Path.GetDirectoryName(gfmdlFilePath).Replace("\\", "/");
      var textureFileDataList = new List<Tuple<string, PokemonModelDataManager.GfmdlTextureType>>();
      for (Int32 i = 0; i < nodeTextureList.Count; ++i)
      {
        XmlAttributeCollection attributeCollection = nodeTextureList[i].Attributes;

        string path = gfmdlDirectory + "/" + nodeTextureList[i].ChildNodes[0].InnerText;
        PokemonModelDataManager.GfmdlTextureType type = 
          PokemonModelDataManager.GfmdlTextureTypeLabel[attributeCollection[1].Value];
        textureFileDataList.Add(new Tuple<string, PokemonModelDataManager.GfmdlTextureType>(path, type));
      }

      return textureFileDataList.ToArray();
    }

    /**
     * @brief テクスチャのコンバート
     * @param shaderFilePathTuples テクスチャのタプル配列(tga, GfmdlTextureType)
     * @param exportDirectory 出力先のディレクトリ
     * @return Tuple<string, GfmdlTextureType>[] 各種テクスチャのタプル
     */
    private Tuple<string, PokemonModelDataManager.GfmdlTextureType>[] ConvertTextureResources(
      Tuple<string, PokemonModelDataManager.GfmdlTextureType>[] textureFilePathTuples,
      string exportDirectory,
      PokemonModelDataManager.ConvertExtensionType convertExtensionType)
    {
      List<Tuple<string, PokemonModelDataManager.GfmdlTextureType>> exportTextureResourceList =
        new List<Tuple<string, PokemonModelDataManager.GfmdlTextureType>>();
      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");

      // テクスチャのバイナリサイズをコンバートして計測
      string textureConverterFilePath =
#if DEBUG
 Path.Combine(nijiToolDir, "TextureConverter", "TextureConverter.exe");
#else
				Path.GetFullPath("../TextureConverter/TextureConverter.exe");
#endif
      if (!File.Exists(textureConverterFilePath))
      {
        MessageBox.Show("テクスチャコンバータ : " + textureConverterFilePath + " が見つかりません。");
        Application.Exit();
      }

      foreach (var textureFilePathTuple in textureFilePathTuples)
      {
        if ((convertExtensionType == PokemonModelDataManager.ConvertExtensionType.CTR) && 
          (textureFilePathTuple.Item2 != PokemonModelDataManager.GfmdlTextureType.Texture))
        {// CTRで参照テーブルだったらコンバートしない
          continue;
        }

        string outputTextureName = System.IO.Path.GetFileName(textureFilePathTuple.Item1.Replace(".tga", ConvertExtensions[convertExtensionType] + ".btex"));
        string outputTexturePath = exportDirectory + outputTextureName;
        ProcessStartInfo startInfo = new ProcessStartInfo(textureConverterFilePath);
        startInfo.WindowStyle = ProcessWindowStyle.Hidden;
        startInfo.RedirectStandardError = true;
        startInfo.RedirectStandardOutput = true;
        startInfo.CreateNoWindow = true;
        startInfo.UseShellExecute = false;
        startInfo.Arguments =
          " -i \"" + textureFilePathTuple.Item1 + "\"" +
          " -o \"" + outputTexturePath + "\"" +
          " -notopt";
        Process process = Process.Start(startInfo);
        process.WaitForExit();
        if (process.ExitCode != 0)
        {
          MessageBox.Show(
            "TextureConverter.exe doesn't work correctly." + Environment.NewLine
            + "Error Code = " + process.ExitCode + Environment.NewLine
            + "StandardOutput" + process.StandardOutput.ReadToEnd() + Environment.NewLine
            + "StandardError" + process.StandardError.ReadToEnd() + Environment.NewLine
            + "Input Path = " + textureFilePathTuple.Item1 + Environment.NewLine
            + "Output Path = " + outputTexturePath,
            "TextureConverter Error!",
            MessageBoxButtons.OK,
            MessageBoxIcon.Stop);
        }

        exportTextureResourceList.Add(
          new Tuple<string, PokemonModelDataManager.GfmdlTextureType>(
            outputTexturePath,
            textureFilePathTuple.Item2));
      }

      return exportTextureResourceList.ToArray();
    }

    /**
     * @brief gfmotのコンバート
     * @param gfmotFilePaths 変換前のファイル
     * @param exportDirectory 出力先ディレクトリ
     * @return string[] 変換後のファイルのパス
     */
    private string[] ConvertGfmotResources(
      string[] gfmotFilePaths,
      string exportDirectory)
    {
      List<string> gfbmotFilePathList = new List<string>();

      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");

      string animationConverterFilePath =
#if DEBUG
 Path.Combine(nijiToolDir, "AnimationConverter", "AnimationConverter.exe");
#else
			Path.GetFullPath("../AnimationConverter/AnimationConverter.exe");
			if (!File.Exists(animationConverterFilePath))
			{
				animationConverterFilePath = Path.Combine(nijiToolDir, "AnimationConverter", "AnimationConverter.exe");
			}
#endif
      if (!File.Exists(animationConverterFilePath))
      {
        MessageBox.Show("モーションコンバータ : " + animationConverterFilePath + " が見つかりません。");
        Application.Exit();
        return null;
      }

      foreach (string gfmotFilePath in gfmotFilePaths)
      {
        // モーションのバイナリコンバータを起動してバイナリサイズを計測
        string outputFileName = System.IO.Path.GetFileName(gfmotFilePath.Replace(".gfmot", ".gfbmot"));
        string outputFilePath = exportDirectory + outputFileName;
        ProcessStartInfo startInfo = new ProcessStartInfo(animationConverterFilePath);
        startInfo.WindowStyle = ProcessWindowStyle.Hidden;
        startInfo.Arguments =
          " -i \"" + gfmotFilePath + "\"" +
          " -o \"" + outputFilePath + "\"" +
          " -poke on";
        startInfo.RedirectStandardError = true;
        startInfo.RedirectStandardOutput = true;
        startInfo.CreateNoWindow = true;
        startInfo.UseShellExecute = false;
        Process animationConverterProcess = Process.Start(startInfo);
        animationConverterProcess.WaitForExit();
        if (animationConverterProcess.ExitCode != 0)
        {
          MessageBox.Show(
            "AnimationConverter.exe doesn't work correctly." + Environment.NewLine
            +"Error Code = " + animationConverterProcess.ExitCode + Environment.NewLine
            + "StandardOutput : " + animationConverterProcess.StandardOutput.ReadToEnd() + Environment.NewLine
            + "StandardError : " + animationConverterProcess.StandardError.ReadToEnd() + Environment.NewLine
            + "Input Path = " + gfmotFilePath + Environment.NewLine
            + "Output Path = " + outputFilePath,
            "AnimationConverter Error!",
            MessageBoxButtons.OK,
            MessageBoxIcon.Stop
          );
        }
        gfbmotFilePathList.Add(outputFilePath);
      }

      return gfbmotFilePathList.ToArray();
    }

    /**
     * @brief gflocファイルのコンバート
     * @param gflocFilePath gflocファイルへのパス
     * @param exportDirectory 出力先のディレクトリ
     * @return string コンバート後のファイルパス群
     */
    private string ConvertGflocResource(
      string gflocFilePath,
      string exportDirectory)
    {
      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");

      // モデルのバイナリサイズをコンバートして計測
      string gfmdlConverterFilePath =
#if DEBUG
 Path.Combine(nijiToolDir, "LocConverter", "LocConverter.exe");
#else
      Path.GetFullPath("../LocConverter/LocConverter.exe");
      if (!File.Exists(gfmdlConverterFilePath))
      {
        gfmdlConverterFilePath = Path.Combine(nijiToolDir, "LocConverter", "LocConverter.exe");
      }
#endif
      if (!File.Exists(gfmdlConverterFilePath))
      {
        MessageBox.Show("ロケータノードコンバータ : " + gfmdlConverterFilePath + " が見つかりません。");
        Application.Exit();
        return "";
      }

      ProcessStartInfo startInfo = new ProcessStartInfo(gfmdlConverterFilePath);
      startInfo.WindowStyle = ProcessWindowStyle.Hidden;
      startInfo.RedirectStandardError = true;
      startInfo.RedirectStandardOutput = true;
      startInfo.CreateNoWindow = true;
      startInfo.UseShellExecute = false;
      string gfblocFileName = gflocFilePath.Replace(".gfloc", ".gfbloc");
      gfblocFileName = System.IO.Path.GetFileName(gfblocFileName);
      string gfblocFilePath = exportDirectory + gfblocFileName;
      if (System.IO.File.Exists(gfblocFilePath))
      {// 既にコンバート後のファイルがある場合はコンバートしない
        return gfblocFilePath;
      }

      startInfo.Arguments =
        "-i \"" + gflocFilePath + "\"" +
        " -o \"" + gfblocFilePath + "\"";
      Process process = Process.Start(startInfo);
      process.WaitForExit();
      if (process.ExitCode != 0)
      {
        MessageBox.Show(
          "LocConverter.exe doesn't work correctly." + Environment.NewLine
          + "Error Code = " + process.ExitCode + Environment.NewLine
          + "StandardOutput : " + process.StandardOutput.ReadToEnd() + Environment.NewLine
          + "StandardError : " + process.StandardError.ReadToEnd() + Environment.NewLine
          + "Input Path = " + gflocFilePath + Environment.NewLine
          + "Output Path = " + gfblocFilePath,
          "Locator Convert Error!",
          MessageBoxButtons.OK,
          MessageBoxIcon.Stop);
      }
      return gfblocFilePath;
    }

    /**
     * @brief gflocファイルのコンバート
     * @param gflocFilePath EffectNode.xmlへのパス
     * @param exportDirectory 出力先のディレクトリ
     * @return string コンバート後のファイルパス
     */
    private string ConvertEffectNodeXml(
      string effectNodeXmlFilePath,
      string exportDirectory)
    {
      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");

      // モデルのバイナリサイズをコンバートして計測
      string gflocConverterFilePath =
#if DEBUG
 Path.Combine(nijiToolDir, "EffectNodeConverter", "EffectNodeConverter.exe");
#else
      Path.GetFullPath("../EffectNodeConverter/EffectNodeConverter.exe");
      if (!File.Exists(gflocConverterFilePath))
      {
        gflocConverterFilePath = Path.Combine(nijiToolDir, "EffectNodeConverter", "EffectNodeConverter.exe");
      }
#endif
      if (!File.Exists(gflocConverterFilePath))
      {
        MessageBox.Show("エフェクトノードコンバータ : " + gflocConverterFilePath + " が見つかりません。");
        Application.Exit();
        return "";
      }

      string gfblocFileName = effectNodeXmlFilePath.Replace(".xml", ".gfbloc");
      gfblocFileName = System.IO.Path.GetFileName(gfblocFileName);
      string gfblocFilePath = exportDirectory + gfblocFileName;
      if (System.IO.File.Exists(gfblocFilePath))
      {
        return gfblocFilePath;
      }

      ProcessStartInfo startInfo = new ProcessStartInfo(gflocConverterFilePath);
      startInfo.WindowStyle = ProcessWindowStyle.Hidden;
      startInfo.RedirectStandardError = true;
      startInfo.RedirectStandardOutput = true;
      startInfo.CreateNoWindow = true;
      startInfo.UseShellExecute = false;
      startInfo.Arguments =
        "-i \"" + effectNodeXmlFilePath + "\"" +
        " -o \"" + gfblocFilePath + "\"";
      Process process = Process.Start(startInfo);
      process.WaitForExit();
      if (process.ExitCode != 0)
      {
        MessageBox.Show(
          "EffectNodeConverter.exe doesn't work correctly." + Environment.NewLine
          + "Error Code = " + process.ExitCode + Environment.NewLine
          + "StandardOutput : " + process.StandardOutput.ReadToEnd() + Environment.NewLine
          + "StandardError : " + process.StandardError.ReadToEnd() + Environment.NewLine
          + "Input Path = " + effectNodeXmlFilePath + Environment.NewLine
          + "Output Path = " + gfblocFilePath,
          "EffectNodeConverter Error!",
          MessageBoxButtons.OK,
          MessageBoxIcon.Stop);

      }

      return gfblocFilePath;
    }

    /**
     * @brief 表情モーションの変換
     * @param exportPath 出力ディレクトリ
     * @return bool 成功/失敗
     */
    private bool ExecuteDivideTexAnimBatch(string exportPath)
    {
      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");

      string divideTexAnimBatchFilePath =
#if DEBUG
 Path.Combine(nijiToolDir, "AnimationConverter", "DivideTexAnim.bat");
#else
			Path.GetFullPath("../AnimationConverter/DivideTexAnim.bat");
			if (!File.Exists(divideTexAnimBatchFilePath))
			{
				divideTexAnimBatchFilePath = Path.Combine(nijiToolDir, "AnimationConverter", "DivideTexAnim.bat");
			}
#endif
      if (!File.Exists(divideTexAnimBatchFilePath))
      {
        MessageBox.Show("表情モーション変換バッチファイル : " + divideTexAnimBatchFilePath + " が見つかりません。");
        return false;
      }

      ProcessStartInfo startInfo = new ProcessStartInfo(divideTexAnimBatchFilePath);
      startInfo.WindowStyle = ProcessWindowStyle.Hidden;
      startInfo.RedirectStandardError = true;
      startInfo.RedirectStandardOutput = true;
      startInfo.CreateNoWindow = true;
      startInfo.UseShellExecute = false;
      startInfo.Arguments = " \"" + exportPath + "\"";
      Process divideTexAnimBatchProcess = Process.Start(startInfo);
      divideTexAnimBatchProcess.WaitForExit();

      if (divideTexAnimBatchProcess.ExitCode != 0)
      {
        MessageBox.Show(
          "DivideTexAnimBatch doesn't work correctly." + Environment.NewLine
          + "Error Code = " + divideTexAnimBatchProcess.ExitCode + Environment.NewLine
          + "StandardOutput : " + divideTexAnimBatchProcess.StandardOutput.ReadToEnd() + Environment.NewLine
          + "StandardError : " + divideTexAnimBatchProcess.StandardError.ReadToEnd() + Environment.NewLine
          + "Input Path = " + exportPath + Environment.NewLine
          + "Output Path = " + exportPath,
          "DivideTexAnimBatch Error!",
          MessageBoxButtons.OK,
          MessageBoxIcon.Stop);
      }

      return true;
    }

    /**
     * @brief シェーダの拡張子を変更
     * @param Tuple<string, string, string>[] シェーダのタプル
     * @param PokemonModelDataManager.ConvertExtensionType 拡張子の種類
     * @return Tuple<string, string, string>[] シェーダのタプル
     */
    private Tuple<string, string, string>[] ReplaceShaderFileTuples(
      Tuple<string, string, string>[] shaderTuples,
      PokemonModelDataManager.ConvertExtensionType extensionType
      )
    {
      var tupleList = new List<Tuple<string, string, string>>();
      foreach (var tuple in shaderTuples)
      {
        tupleList.Add(
          new Tuple<string, string, string>(
            tuple.Item1.Replace(".gfvsh", ConvertExtensions[extensionType] + ".gfbvsh"),
            tuple.Item2.Replace(".gfgsh", ConvertExtensions[extensionType] + ".gfbgsh"),
            tuple.Item3.Replace(".gffsh", ConvertExtensions[extensionType] + ".gfbfsh")
          )
        );
      }

      return tupleList.ToArray();
    }

    /**
     * @brief テクスチャの拡張子を変更
     * @param Tuple<string, PokemonModelDataManager.GfmdlTextureType>[] テクスチャのタプル
     * @param PokemonModelDataManager.ConvertExtensionType 拡張子の種類
     * @return Tuple<string, PokemonModelDataManager.GfmdlTextureType>[] テクスチャのタプル
     */
    private Tuple<string, PokemonModelDataManager.GfmdlTextureType>[] ReplaceTextureFileTuples(
      Tuple<string, PokemonModelDataManager.GfmdlTextureType>[] textureTuples,
      PokemonModelDataManager.ConvertExtensionType extensionType
      )
    {
      var tupleList = new List<Tuple<string, PokemonModelDataManager.GfmdlTextureType>>();
      foreach (var tuple in textureTuples)
      {
        if ((extensionType == PokemonModelDataManager.ConvertExtensionType.CTR) &&
          (tuple.Item2 != PokemonModelDataManager.GfmdlTextureType.Texture))
        {
          continue;
        }

        tupleList.Add(
          new Tuple<string, PokemonModelDataManager.GfmdlTextureType>(
            tuple.Item1.Replace(".tga", ConvertExtensions[extensionType] + ".btex"),
            tuple.Item2)
            );
      }
      return tupleList.ToArray();
    }

    /**
     * @brief gfmdl, gfmot以外の外部ファイルをコピー
     * @param sourceDirectory コピー元のディレクトリ
     * @param destinationDirectory コピー先のディレクトリ
     */
    private void CopyPokemonModelExtraResourceFiles(string sourceDirectory, string destinationDirectory)
    {
      if (!Directory.Exists(destinationDirectory))
      {
        Directory.CreateDirectory(destinationDirectory);
      }

      byte[] readBytes;
      // LoopSkelData.txt, EffectNode.xml, *.mcnfをコピー
      if (!File.Exists(destinationDirectory + "LoopSkelData.txt"))
      {
        if (System.IO.File.Exists(sourceDirectory + "LoopSkelData.txt"))
        {
          readBytes = System.IO.File.ReadAllBytes(sourceDirectory + "LoopSkelData.txt");
          System.IO.File.WriteAllBytes(destinationDirectory + "LoopSkelData.txt", readBytes);
        }
      }

      string[] xmlFilePaths = System.IO.Directory.GetFiles(sourceDirectory, "*.xml");
      foreach (string path in xmlFilePaths)
      {
        string fileName = System.IO.Path.GetFileName(path);
        string destinationFileName = destinationDirectory + fileName;
        if (File.Exists(destinationFileName))
        {
          continue;
        }
        readBytes = System.IO.File.ReadAllBytes(path);
        System.IO.File.WriteAllBytes(destinationFileName, readBytes);
      }

      string[] mcnfFilePaths = System.IO.Directory.GetFiles(sourceDirectory, "*.mcnf");
      foreach (string path in mcnfFilePaths)
      {
        string fileName = System.IO.Path.GetFileName(path);
        string destinationFileName = destinationDirectory + fileName;
        if (File.Exists(destinationFileName))
        {
          continue;
        }
        readBytes = System.IO.File.ReadAllBytes(path);
        System.IO.File.WriteAllBytes(destinationFileName, readBytes);
      }

      string[] gflocFilePaths = System.IO.Directory.GetFiles(sourceDirectory, "*.gfloc");
      foreach (string path in gflocFilePaths)
      {
        string fileName = System.IO.Path.GetFileName(path);
        string destinationFileName = destinationDirectory + fileName;
        if (File.Exists(destinationFileName))
        {
          continue;
        }
        readBytes = System.IO.File.ReadAllBytes(path);
        System.IO.File.WriteAllBytes(destinationFileName, readBytes);
      }
    }

    /**
     * @brief PoekmonModel作成に必要なリソースをコピー
     * @param sourceGfmdlFilePath コピー元のディレクトリ
     * @param destinationDirectory コピー先のディレクトリ
     */
    private void CopyPokemonModelGfmdlResourceFiles(string sourceGfmdlFilePath, string destinationDirectory)
    {
      if (string.IsNullOrEmpty(sourceGfmdlFilePath))
      {
        return;
      }

      // gfmdlファイルをまずコピー
      byte[] readBytes;
      readBytes = System.IO.File.ReadAllBytes(sourceGfmdlFilePath);
      System.IO.File.WriteAllBytes(destinationDirectory + System.IO.Path.GetFileName(sourceGfmdlFilePath), readBytes);

      // Shaders, Textures, Textures_rareをコピー
      string shadersPath = System.IO.Path.GetDirectoryName(sourceGfmdlFilePath).Replace("\\", "/") + "/Shaders";
      if (Directory.Exists(shadersPath))
      {
        Util.CopyDirectory(shadersPath, destinationDirectory + "Shaders", false);
      }
      string texturesPath = System.IO.Path.GetDirectoryName(sourceGfmdlFilePath).Replace("\\", "/") + "/Textures";
      if (Directory.Exists(texturesPath))
      {
        Util.CopyDirectory(texturesPath, destinationDirectory + "Textures", false);
      }
      string texturesRarePath = System.IO.Path.GetDirectoryName(sourceGfmdlFilePath).Replace("\\", "/") + "/Textures_rare";
      if (Directory.Exists(texturesRarePath))
      {
        Util.CopyDirectory(texturesRarePath, destinationDirectory + "Textures_rare", false);
      }
      else
      {// テクスチャの共有設定があって, 該当のディレクトリにTextures_rareがなかったらシリーズの別のディレクトリを探しにいく
        if (m_NeedToShareResourceFlags["TEXTURE_SHARE"] && Directory.Exists(m_SharedDirectory + "Textures_rare"))
        {
          Util.CopyDirectory(m_SharedDirectory + "Textures_rare", m_SharedDirectory + "converted_data/Textures_rare", false);
        }
      }
    }

    /**
     * @brief PoekmonModel作成に必要なgfmotリソースをコピー
     * @param sourceFilePath コピー元のディレクトリ
     * @param destinationDirectory コピー先のディレクトリ
     */
    private void CopyResourceFiles(string[] sourceFilePaths, string destinationDirectory)
    {
      if (sourceFilePaths.Length == 0)
      {
        return;
      }

      foreach (string path in sourceFilePaths)
      {
        byte[] readBytes = System.IO.File.ReadAllBytes(path);
        System.IO.File.WriteAllBytes(destinationDirectory + System.IO.Path.GetFileName(path), readBytes);
      }
    }

    /**
     * @brief gfmdlファイルからハッシュを作成
     * @param gfmdlFilePath gfmdlファイルへのパス
     * @return UInt32 ハッシュ値
     */
    private UInt32 CreateModelDataHash(string gfmdlFilePath)
    {
      string[] gfmdlResourcePaths = CollectModelResourcePaths(gfmdlFilePath);

      // 各種コンバータもハッシュの作成時に用いる
      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");
      string gfmdlConverterFilePath =
#if DEBUG
        Path.Combine(nijiToolDir, "ModelConverter", "ModelConverter.exe");
#else
        Path.GetFullPath("../ModelConverter/ModelConverter.exe");
        if (!File.Exists(gfmdlConverterFilePath))
        {
          gfmdlConverterFilePath = Path.Combine(nijiToolDir, "ModelConverter", "ModelConverter.exe");
        }
#endif

      string shaderConverterFilePath =
#if DEBUG
        Path.Combine(nijiToolDir, "ShaderCompiler", "ShaderCompiler.exe");
#else
        Path.GetFullPath("../ShaderCompiler/ShaderCompiler.exe");
#endif

      // テクスチャのバイナリサイズをコンバートして計測
      string textureConverterFilePath =
#if DEBUG
        Path.Combine(nijiToolDir, "TextureConverter", "TextureConverter.exe");
#else
        Path.GetFullPath("../TextureConverter/TextureConverter.exe");
#endif

      var hashFilePathList = gfmdlResourcePaths.ToList();
      hashFilePathList.Add(gfmdlConverterFilePath);
      hashFilePathList.Add(shaderConverterFilePath);
      hashFilePathList.Add(textureConverterFilePath);

      // xmlファイル, txtファイル, mcnfファイル, gflocファイルなどもキャッシュに含める
      string directoryName = Path.GetDirectoryName(gfmdlFilePath);
      var xmlFilePaths = Directory.GetFiles(directoryName, "*.xml");
      Array.ForEach(xmlFilePaths, path => hashFilePathList.Add(path));
      var txtFilePaths = Directory.GetFiles(directoryName, "*.txt");
      Array.ForEach(txtFilePaths, path => hashFilePathList.Add(path));
      var mcnfFilePaths = Directory.GetFiles(directoryName, "*.mcnf");
      Array.ForEach(mcnfFilePaths, path => hashFilePathList.Add(path));
      var gfblocFilePaths = Directory.GetFiles(directoryName, "*.gfbloc");
      Array.ForEach(gfblocFilePaths, path => hashFilePathList.Add(path));

      UInt32 fnvHash = PokemonModel.CreateHashFromResources(hashFilePathList.ToArray());
      return fnvHash;
    }

    /**
     * @brief gfmdlデータのハッシュ値の作成
     * @param exportDirectory 出力先のディレクトリ
     * @param gfmdlFilePath gfmdlのファイルパス
     * @return bool 成功/失敗(失敗時は「ファイルが既にある」)
     */
    private bool CreateModelDataHashFile(string exportDirectory, string gfmdlFilePath, ModelType modelType)
    {
      if (string.IsNullOrEmpty(gfmdlFilePath))
      {
        return false;
      }

      string hashFilePath = exportDirectory + "/" + ModelHashFileName[modelType];
      UInt32 fnvHash = CreateModelDataHash(gfmdlFilePath);
      CreateHashFile(hashFilePath, fnvHash);
      return true;
    }

    /**
     * @brief gfmotからハッシュを作成
     * @param string[] gfmotFilePaths ファイルパス
     * @return UInt32 ハッシュ値
     */
    private UInt32 CreateMotionDataHash(string[] gfmotFilePaths)
    {
      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");
      string animationConverterFilePath =
#if DEBUG
        Path.Combine(nijiToolDir, "AnimationConverter", "AnimationConverter.exe");
#else
        Path.GetFullPath("../AnimationConverter/AnimationConverter.exe");
        if (!File.Exists(animationConverterFilePath))
        {
          animationConverterFilePath = Path.Combine(nijiToolDir, "AnimationConverter", "AnimationConverter.exe");
        }
#endif
      var hashDataFileList = gfmotFilePaths.ToList();
      hashDataFileList.Add(animationConverterFilePath);
      UInt32 fnvHash = PokemonModel.CreateHashFromResources(hashDataFileList.ToArray());
      return fnvHash;
    }

    /**
     * @brief gfmotデータのハッシュ値の作成
     * @param exportDirectory 出力先のディレクトリ
     * @param gfmotFilePaths gfmdlのファイルパス群
     * @return bool 成功/失敗
     */
    private bool CreateMotionDataHashFile(string exportDirectory, string[] gfmotFilePaths)
    {
      if (gfmotFilePaths == null)
      {
        return false;
      }

      if (gfmotFilePaths.Length == 0)
      {
        return false;
      }

      string hashFilePath = exportDirectory + "/MotionHash.fnv";
      UInt32 fnvHash = CreateMotionDataHash(gfmotFilePaths);
      CreateHashFile(hashFilePath, fnvHash);
      return true;
    }

    /**
     * @brief ファイルを開いてハッシュ値を読み取り
     * @param fnvHashFilePath ファイルパス
     * @return UInt32 ハッシュ値
     */
    private UInt32 ReadFnvHashFile(string fnvHashFilePath)
    {
      if (Path.GetExtension(fnvHashFilePath) != ".fnv")
      {
        return 0;
      }

      var fileStream = new FileStream(fnvHashFilePath, FileMode.Open, FileAccess.Read);
      var binaryReader = new BinaryReader(fileStream);
      UInt32 fileFnvHash = (UInt32)binaryReader.ReadInt32();
      binaryReader.Close();
      fileStream.Close();
      return fileFnvHash;
    }

    /**
     * @brief 受け取った4バイトの数値を書き込んだハッシュファイルの作成
     * @param exportFilePath 出力するファイルパス
     * @param value 4バイトの数値
     * @return bool 成功/失敗
     */
    private void CreateHashFile(string exportFilePath, UInt32 value)
    {
      System.IO.FileStream fs = new System.IO.FileStream(
        exportFilePath,
        FileMode.Create,
        FileAccess.Write);
      System.IO.BinaryWriter bw = new BinaryWriter(fs);

      // gfmdlのバージョン番号とハッシュ値を書き込む.
      byte[] bytes = BitConverter.GetBytes(value);
      bw.Write(bytes, 0, bytes.Length);

      fs.Close();
    }

    /**
     * @brief gfmdlファイルから使用されているリソースのパスを収集
     * @param gfmdlFIlePath gfmdlファイルのパス
     * @return string[] 収集したリソースのパス
     */
    private string[] CollectModelResourcePaths(string gfmdlFilePath)
    {
      List<string> resourcePathList = new List<string>();

      string gfmdlFileDirectory = System.IO.Path.GetDirectoryName(gfmdlFilePath).Replace("\\", "/");

      // まずはgfmdlのファイルパスを追加
      resourcePathList.Add(gfmdlFilePath);

      // 頂点シェーダとピクセルシェーダの対になっているパスをgfmdlから取得
      XmlDocument gfmdlDocument = new XmlDocument();
      gfmdlDocument.Load(gfmdlFilePath);

      XmlElement rootElement = gfmdlDocument.DocumentElement;
      XmlNodeList nodeShaderListList = rootElement.GetElementsByTagName("ShaderList");
      XmlNodeList nodeShaderList = nodeShaderListList[0].ChildNodes;
      for (Int32 i = 0; i < nodeShaderList.Count; ++i)
      {
        for (Int32 j = 0; j < nodeShaderList[i].ChildNodes.Count; ++j)
        {
          var childNode = nodeShaderList[i].ChildNodes[j];
          string resourcePath = gfmdlFileDirectory + "/" + childNode.InnerText;
          if (!resourcePathList.Contains(resourcePath))
          {
            resourcePathList.Add(resourcePath);
          }
        }
      }

      // テクスチャファイルのコンバート
      XmlNodeList nodeTextureListList = rootElement.GetElementsByTagName("TextureList");
      XmlNodeList nodeTextureList = nodeTextureListList[0].ChildNodes;
      for (Int32 i = 0; i < nodeTextureList.Count; ++i)
      {
        XmlAttributeCollection attributeCollection = nodeTextureList[i].Attributes;

        string resourcePath = gfmdlFileDirectory + "/" + nodeTextureList[i].ChildNodes[0].InnerText;
        PokemonModelDataManager.GfmdlTextureType type = PokemonModelDataManager.GfmdlTextureType.Texture;
        if (type == PokemonModelDataManager.GfmdlTextureType.Texture)
        {
          resourcePathList.Add(resourcePath);
        }
      }

      return resourcePathList.ToArray();
    }

		/**
		 * @brief リソース読み込み後、ツリービューに追加
		 * @param string gfmdlFilePath gfmdl形式のファイルパス
		 * @param string gfmotFilePaths gfmot形式のファイルパス一式
		 * @param PokemonModelDataManager モデルデータマネージャ
		 * @param frameResolution フレームの分解能
		 * @param loopEnable ループON/OFF
		 * @param isRare レア/通常
		 */
		public PokemonModelDataManager LoadGfResources(
      string importDirectory,
      GfMdlResourcePathSetDictionary gfmdlResourcePathSetDirectory,
      GfMdlResourcePathSetDictionary shadowGfmdlResourcePathSetDirectory,
			GfmotPathSet gfmotPathSet,
      GfblocPathSet gfblocPathSet,
      ExtraFilePathSet extraFilePathSet,
			decimal frameResolution,
			bool loopEnable,
			bool isRare)
		{
			PokemonModelDataManager pokemonModelDataManager = m_ApplicationCore.LoadGfResources(
        importDirectory,
        gfmdlResourcePathSetDirectory,
        shadowGfmdlResourcePathSetDirectory,
        gfmotPathSet,
        gfblocPathSet,
        extraFilePathSet,
				frameResolution,
				loopEnable,
				isRare);

      TreeNode gfmdlTreeNode = new TreeNode(Directory.GetParent(pokemonModelDataManager.ImportDirectory.TrimEnd('/')).Name);
			TreeNode rootTreeNode = gfmdlTreeNode;

			Int32 index = modelMotionResourceTreeView.Nodes.Count;
			modelMotionResourceTreeView.Nodes.Add(rootTreeNode);
			modelMotionResourceTreeView.SelectedNode = rootTreeNode;

			// 読み込んだノードを選択状態にして, UIに変更通知
			pokemonModelDataManager.NotifyToListeners();
			modelMotionResourceTreeView.Focus();
			return pokemonModelDataManager;
		}

		/**
		 * @brief gfmdl, gfmot形式のリソースを再読み込み
		 * @param modelDataManager 読み込むモデルの情報
		 * @return bool 成功/失敗
		 */
		public PokemonModelDataManager ReloadGfResources(
			PokemonModelDataManager modelDataManager,
			bool isRare)
		{
			// 選択されていたノードのインデックスを保存
			TreeNode selectedNode = modelMotionResourceTreeView.SelectedNode;
			Int32 index = selectedNode.Index;

      string importDirectory = modelDataManager.ImportDirectory;
      GfMdlResourcePathSetDictionary gfmdlResourcePathSetDirectory =
        modelDataManager.GfMdlResourcePathSetDictionary;
      GfMdlResourcePathSetDictionary shadowGfmdlResourcePathSetDirectory =
        modelDataManager.ShadowGfMdlResourcePathSetDictionary;
			GfmotPathSet gfmotFilePathSet = new GfmotPathSet(modelDataManager.GfmotFilePaths);
      GfblocPathSet gfblocPathSet = modelDataManager.GfblocPathSet;
      ExtraFilePathSet extraFilePathSet = modelDataManager.ExtraFilePathSet;
			decimal frameResolution = modelDataManager.FrameResolution;
			bool loopEnable = modelDataManager.LoopEnable;

			// 選択されていたノードを削除
			DeleteSelectedNode();

			// 受け取ったデータからレアカラー/通常カラーのモデルを生成
			PokemonModelDataManager pokemonModelDataManager = m_ApplicationCore.ReloadGfResources(
        importDirectory,
        gfmdlResourcePathSetDirectory,
        shadowGfmdlResourcePathSetDirectory,
        gfmotFilePathSet,
        gfblocPathSet,
        extraFilePathSet,
				frameResolution,
				loopEnable,
				isRare,
				index);

			TreeNode gfmdlTreeNode = new TreeNode(
				System.IO.Path.GetFileName(pokemonModelDataManager.GfmdlFilePath));
			TreeNode rootTreeNode = gfmdlTreeNode;
			modelMotionResourceTreeView.Nodes.Insert(index, rootTreeNode);
			modelMotionResourceTreeView.SelectedNode = rootTreeNode;

			// 読み込んだノードを選択状態にして, UIに変更通知
			pokemonModelDataManager.NotifyToListeners();
			modelMotionResourceTreeView.Focus();
			return pokemonModelDataManager;
		}

		/**
		 * @brief ノード選択中のキー入力処理
		 * @param sender
		 * @param KeyEventArgs キー入力情報
		 */
		private void modelMotionResourceTreeView_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyData == Keys.Delete)
			{
				DeleteSelectedNode();
			}
		}

		private void DeleteSelectedNode()
		{
			if (m_ApplicationCore.IsEmptyPokemonModelDataManagerList())
			{
				return;
			}
			TreeNode selectedNode = modelMotionResourceTreeView.SelectedNode;
			Int32 index = selectedNode.Index;

			if (index < 0)
			{
				return;
			}
			bool isSucceeded = m_ApplicationCore.RemoveFromPokemonModelDataManagerList(index);
			if (!isSucceeded)
			{// 削除失敗ならここで終わり. ツリービューからノードは外さない
				return;
			}
			selectedNode.Remove();
		}

    public void EmulateDragDrop(string exportsDirectoryPath)
    {
      string dropFilePath = exportsDirectoryPath.Replace("\\", "/");
      m_ImportDirectory = dropFilePath + "/";

      m_SharedDirectory = FindSharedResourceFilePath(dropFilePath) + "/";
      if (!Directory.Exists(m_SharedDirectory))
      {
        m_SharedDirectory = m_ImportDirectory;
      }

      // 通常モデルと影モデルのパスの読み込み
      string[] dropGfmdlFilePaths = System.IO.Directory.GetFiles(dropFilePath, "*.gfmdl", System.IO.SearchOption.TopDirectoryOnly);
      dropGfmdlFilePaths = Array.ConvertAll(dropGfmdlFilePaths, path => path = path.Replace("\\", "/"));
      m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL] = Array.Find(dropGfmdlFilePaths, path => !path.Contains("_shadow.gfmdl"));
      m_ReferenceGfmdlFilePathDirectory[ModelType.SHADOW_MODEL] = Array.Find(dropGfmdlFilePaths, path => path.Contains("_shadow.gfmdl"));

      // 読み込まれたgfmdlが正しいテクスチャの構造になっているかをチェック
      GfmdlErrorChecker gfmdlErrorChecker = new GfmdlErrorChecker(m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL]);
      var incorrectTextureFormatMaterials = gfmdlErrorChecker.CountUpIncorrectTextureFormatMaterials();
      foreach (var materialTextureTuple in incorrectTextureFormatMaterials)
      {
        string errorMessage = "Material : " + materialTextureTuple.Item1 + " にカラーテクスチャ以外のテクスチャが入っています." + Environment.NewLine;
        errorMessage += "Texture : " + materialTextureTuple.Item2 + " のフォーマットを確認してください.";
        MessageBox.Show(errorMessage, "Material Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
      if (incorrectTextureFormatMaterials.Length > 0)
      {// 間違っている設定のものが一つでもあったらここで終了
        return;
      }

      if (m_NeedToShareResourceFlags["MODEL_SHARE"])
      {
        string[] sharedGfmdlResourcePaths = Directory.GetFiles(m_SharedDirectory, "*gfmdl");
        m_ReferenceGfmdlFilePathDirectory[ModelType.MODEL] = Array.Find(sharedGfmdlResourcePaths, path => !path.Contains("_shadow.gfmdl"));
        m_ReferenceGfmdlFilePathDirectory[ModelType.SHADOW_MODEL] = Array.Find(sharedGfmdlResourcePaths, path => path.Contains("_shadow.gfmdl"));
      }

      m_ReferenceGfmotFilePaths = System.IO.Directory.GetFiles(dropFilePath, "*.gfmot", System.IO.SearchOption.TopDirectoryOnly);

      if (m_NeedToShareResourceFlags["ANIME_SHARE"])
      {
        m_ReferenceGfmotFilePaths = System.IO.Directory.GetFiles(m_SharedDirectory, "*.gfmot", System.IO.SearchOption.TopDirectoryOnly);
      }
      Array.ForEach(m_ReferenceGfmotFilePaths, path => path.Replace("\\", "/"));

      string[] dropGflocFilePaths = System.IO.Directory.GetFiles(dropFilePath, "*.gfloc", System.IO.SearchOption.TopDirectoryOnly);
      if (dropGflocFilePaths.Length == 0)
      {
        dropGflocFilePaths = System.IO.Directory.GetFiles(m_SharedDirectory, "*.gfloc", System.IO.SearchOption.TopDirectoryOnly);
      }
      if (dropGflocFilePaths.Length > 0)
      {
        Array.ForEach(dropGflocFilePaths, path => path.Replace("\\", "/"));
        m_ReferenceGflocFilePath = dropGflocFilePaths[0];
      }

      string[] dropEffectNodeFilePaths = System.IO.Directory.GetFiles(dropFilePath, "EffectNode.xml", System.IO.SearchOption.TopDirectoryOnly);
      if ((dropEffectNodeFilePaths.Length == 0) || m_NeedToShareResourceFlags["ANIME_SHARE"])
      {// 見つからなかったら他のシリーズの関連ディレクトリを探しに行く
        m_ReferenceEffectNodeFilePath = m_SharedDirectory + "EffectNode.xml";
      }
      else
      {
        m_ReferenceEffectNodeFilePath = dropEffectNodeFilePaths[0];
      }

      m_IsNecessaryToConvertResources = DecideToConvertResources();

      // gfmdlのリソースをコンバート
      GfMdlResourcePathSetDictionary[] pathSetDictionaries = ProceedGfmdlConvert();

      // レアカラーのテクスチャのサイズの比較調査
      Array.ForEach(pathSetDictionaries, pathSet => CheckDifferenceWithNormalTextureAndRareTexture(pathSet));

      // EffectNode.xmlをEffectNode.gfblocにコンバート
      GfblocPathSet gfblocPathSet = ProceedGfblocConvert();

      // その他のファイルのコンバート
      ExtraFilePathSet extraFilePathSet = ProceedExtraFileConvert();

      // gfmotのリソースをコンバート
      GfmotPathSet gfmotPathSet = ProceedGfmotConvert();

      LoadGfResources(
        m_ImportDirectory,
        pathSetDictionaries[0],
        pathSetDictionaries[1],
        gfmotPathSet,
        gfblocPathSet,
        extraFilePathSet,
        2.0m,
        true,
        false);
    }
  }
}
