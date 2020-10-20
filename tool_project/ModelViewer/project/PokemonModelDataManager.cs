using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;
using System.Text.RegularExpressions;
using gfl2.clr.math;
using gfl2.clr.renderingengine;
using gfl2.clr.animation;
using gfl2.clr.renderingengine.scenegraph.resource;
using gfl2.clr.renderingengine.scenegraph.instance;
using gfl2.clr.poke_3d_lib;
using GFTarga;

namespace ModelViewer
{
  //!< @brief 各参照テクスチャのパスを記載するDictionaryを内包したクラス
  public class ReferenceTableTexturePaths
  {
    //!< @brief 参照テクスチャの種類
    public enum ReferenceTable
    {
      RR,
      RG,
      RB
    }

    //!< @brief 各参照テクスチャのインデックス
    private static readonly Dictionary<Int32, ReferenceTable> s_ReferenceTableSlotNo =
      new Dictionary<Int32, ReferenceTable>
      {
        { 11, ReferenceTable.RR },
        { 12, ReferenceTable.RG },
        { 13, ReferenceTable.RB }
      };

    //!< @brief 各三章テクスチャのファイルパス
    private Dictionary<ReferenceTable, string> m_ReferenceTextureFilePathDictionary;

    //!< @brief 添字でアクセスできるようにするためのインデクサ
    public string this[ReferenceTable referenceTable]
    {
      get { return m_ReferenceTextureFilePathDictionary[referenceTable]; }
      set { m_ReferenceTextureFilePathDictionary[referenceTable] = value; }
    }

    //!< @brief 該当のスロット番号が存在するかどうかの判定
    public static bool ContainsSlotNo(Int32 slotNo)
    {
      return s_ReferenceTableSlotNo.ContainsKey(slotNo);
    }

    //!< @brief 該当のスロット番号から各参照テクスチャの種類を選択
    public static ReferenceTable SelectReferenceTableFromSlotNo(Int32 slotNo)
    {
      Debug.Assert(ContainsSlotNo(slotNo));
      return s_ReferenceTableSlotNo[slotNo];
    }

    public ReferenceTableTexturePaths()
    {
      m_ReferenceTextureFilePathDictionary = new Dictionary<ReferenceTable, string>();
      foreach (ReferenceTable type in Enum.GetValues(typeof(ReferenceTable)))
      {
        m_ReferenceTextureFilePathDictionary.Add(type, "");
      }
    }
  }

	public class PokemonModelDataManager : IDisposable, IModelDataManager
	{
		private PokemonModel.PokemonModelInfo m_PokemonModelInfo;
		public PokemonModel.PokemonModelInfo PokemonModelInfo
		{
			get { return m_PokemonModelInfo; }
		}

    private GfMdlResourcePathSetDictionary m_GfMdlResourcePathSetDictionary;
    public GfMdlResourcePathSetDictionary GfMdlResourcePathSetDictionary
    {
      get { return m_GfMdlResourcePathSetDictionary; }
    }
    private GfMdlResourcePathSetDictionary m_ShadowGfMdlResourcePathSetDictionary;
    public GfMdlResourcePathSetDictionary ShadowGfMdlResourcePathSetDictionary
    {
      get { return m_ShadowGfMdlResourcePathSetDictionary; }
    }

    private GfmotPathSet m_GfmotResourcePathSet;
    public GfmotPathSet GfmotPathSet
    {
      get { return m_GfmotResourcePathSet; }
    }

    private ExtraFilePathSet m_ExtraFilePathSet;
    public ExtraFilePathSet ExtraFilePathSet
    {
      get { return m_ExtraFilePathSet; }
    }

    public static readonly System.Text.RegularExpressions.Regex s_pmXXXX =
      new System.Text.RegularExpressions.Regex(@"pm\d\d\d\d");  //!< @brief pmXXXXを判定する正規表現

    public static readonly System.Text.RegularExpressions.Regex s_pmXXXX_YY =
      new System.Text.RegularExpressions.Regex(@"pm\d\d\d\d_\d\d");  //!< @brief pmXXXX_YYを判定する正規表現

		//!< @brief 隣のモデルとの間隔
		public static readonly float PositionInterval = 100.0f;

		//<! @brief 大きさを示す文字列と大きさを対応させた連想配列
		public static readonly Dictionary<string, PokemonSize.Enum> PokemonSizeLabel =
			new Dictionary<string, PokemonSize.Enum>
			{
				{ "S", PokemonSize.Enum.S },
				{ "M", PokemonSize.Enum.M },
				{ "L", PokemonSize.Enum.L },
			};

    //!< @brief コンバート時の拡張子の種類
    public enum ConvertExtensionType
    {
      WinOpenGL,
      CTR,
      NumberOf
    }

    private static string[] ConvertExtensions =
      new string[(Int32)ConvertExtensionType.NumberOf]
      {
        ".wingl",
        ".ctr"
      };

		//<! @brief 部分モーションが必要なパーツ名示す文字列とパーツの番号を対応させた連想配列
		public static readonly string[] OverridePartsTypeLabel =
			new string[(Int32)gfl2.clr.animation.OverridePartsType.Enum.NumberOf]
			{
				"eye01",
				"eye02",
				"eye03",
				"mouth01",
				"mouth02",
				"mouth03",
				"loop01",
				"loop02",
				"loop03",
				"loop04",
			};

    //!< @brief 表情切り替えパターンの総数
    private static readonly int NUMBER_OF_FACE_PATTERN = 8;

    //!< @brief 顔の表情切り替えパターンの種類
    public enum FacePatternType
    {
      Eye,    // 目
      Mouth   // 口
    }

    //!< @brief 表情モーションの判定正規表現
    private static readonly Dictionary<FacePatternType, Regex> s_FaceMotionRegexDictionary = new Dictionary<FacePatternType, Regex>
    {
      { FacePatternType.Eye, new Regex(@"pm\d\d\d\d_\d\d_eye\d\d.gfmot") },
      { FacePatternType.Mouth, new Regex(@"pm\d\d\d\d_\d\d_mouth\d\d.gfmot") },
    };

    //!< @brief 表情テクスチャの判定正規表現
    private static readonly Dictionary<FacePatternType, Regex> s_FaceTextureRegexDictionary = new Dictionary<FacePatternType, Regex>
    {
      { FacePatternType.Eye, new Regex(@"pm\d\d\d\d_\d\d_Eye\d.tga") },
      { FacePatternType.Mouth, new Regex(@"pm\d\d\d\d_\d\d_Mouth\d.tga") },
    };

    //!< @brief 表情の使用/未使用チェックフラグ
    private readonly Dictionary<FacePatternType, bool[]> m_FacePatternUsedFlags;
    public Dictionary<FacePatternType, bool[]> FacePatternUsedFlags
    {
      get { return m_FacePatternUsedFlags; }
    }

		//!< @brief gfmdlに格納されているテクスチャの種類(通常テクスチャ, 参照テーブルなど)
		public enum GfmdlTextureType
		{
			Texture,	// 通常テクスチャ
			ExtTexture,	// 拡張テクスチャ(参照テーブルなど)
			NumberOf
		}

		//!< @brief 通常のテクスチャかそれ以外(参照テーブルなど)を見分けるための辞書配列
		public static readonly Dictionary<string, GfmdlTextureType> GfmdlTextureTypeLabel =
			new Dictionary<string, GfmdlTextureType>
			{
				{"Texture", GfmdlTextureType.Texture},
				{"ExtTexture", GfmdlTextureType.ExtTexture}
			};

    //!< @brief ポケデコロケータのバウンディングボックスに対応した辞書配列
    public static readonly Dictionary<string, gfl2.clr.renderingengine.PokedecoType.Enum[]> PokedecoTypeDictionary =
      new Dictionary<string, gfl2.clr.renderingengine.PokedecoType.Enum[]>
      {
        {"none", new gfl2.clr.renderingengine.PokedecoType.Enum[]{} },
        {"bb_hat", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_HAT} },
        {"bb_ribbon", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_RIBBON} },
        {"bb_glasses", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_GLASSES} },
        {"bb_badge", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_BADGE} },
        {"all_head_parts", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_HAT} },
        {"all_chest_parts", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_RIBBON, gfl2.clr.renderingengine.PokedecoType.Enum.BB_BADGE} },
        {"all_face_parts", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_GLASSES} },
      };

    //!< @brief マテリアル名に対応したプリセットの連想配列
    private readonly Dictionary<string, string> m_PresetNameDictionary;
    public Dictionary<string, string> PresetnameDictionary
    {
      get { return m_PresetNameDictionary; }
    }

    //!< @brief マテリアル名に対応した参照テクスチャの連想配列
    private readonly Dictionary<string, ReferenceTableTexturePaths> m_ReferenceTableDictionary;
    public Dictionary<string, ReferenceTableTexturePaths> ReferenceTableDictionary
    {
      get { return m_ReferenceTableDictionary; }
    }

    //!< @brief ポケデコのバウンディングボックスの種類のインデックス
    public string[] PokedecoLocatorBoundingBoxNames { get; set; }

		public PokemonSize.Enum Size { get; set; }
		public bool IsInnerTubeVisible { get; set; }
		public bool IsOuterTubeVisible { get; set; }
		public bool IsModelAABBVisible { get; set; }
		public Vector InnerTubeColor { get; set; }
		public Vector OuterTubeColor { get; set; }
		public Vector ModelAABBColor { get; set; }

		public AABB DefaultModelAABB { get; set; }
		public void ResetModelAABB()
		{
			AABB aabb = new AABB(DefaultModelAABB);
			m_PokemonModel.SetModelAABB(aabb);
		}
		public float DefaultCmHeight { get; set; }
		public float DefaultAdjustHeight { get; set; }
    public float DefaultFieldAdjust { get; set; }
		public float DefaultScale
		{
			get { return DefaultAdjustHeight / DefaultCmHeight; }
		}
    public float FieldAdjust { get; set; }

		private PokemonModel m_PokemonModel;
		public PokemonModel PokemonModel
		{
			get { return m_PokemonModel; }
		}
		private PokemonModel m_IdModel;
		public PokemonModel IdModel
		{
			get { return m_IdModel; }
		}
		private PokemonModel m_ShadowModel;
		public PokemonModel ShadowModel
		{
			get { return m_ShadowModel; }
		}
    private int[] m_ShadowMaterialIndexList = null;

		private AnimationTimeLine m_AnimationTimeLine;
		public AnimationTimeLine AnimationTimeLine
		{
			get { return m_AnimationTimeLine; }
		}

		private string m_ImportDirectory;
		public string ImportDirectory
		{
			get { return m_ImportDirectory; }
		}

		public string GfmdlFilePath
		{
      get { return m_GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].gfmdlFilePath; }
		}
		public Int32 GfmdlBinarySize
		{
			get {
        FileInfo fileInfo = new FileInfo(m_GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].gfbmdlFilePath);
        return (Int32)fileInfo.Length;
      }
		}
		private ResourceNode m_motionConfigResourceRootNode;
		ResourceNode m_pModelResourceRootNode;
    GfBinaryMdlData m_pModelResourceBinaryData;
    public string[] GfmotFilePaths
    {
      get { return m_GfmotResourcePathSet.GfmotFilePaths; }
    }

    private ResourceNode m_pShadowModelResourceRootNode;
    private GfBinaryMdlData m_pShadowModelResourceBinaryData;

    public string GfmotFilePath(Int32 index)
    {
      return m_GfmotResourcePathSet.GfmotFilePaths[index];
    }
    public Int32 GfmotBinarySize(Int32 index)
    {
      FileInfo fileInfo = new FileInfo(m_GfmotResourcePathSet.GfmotFilePaths[index]);
      return (Int32)fileInfo.Length;
    }

		private decimal m_FrameResolution;
		public decimal FrameResolution
		{
			get { return m_FrameResolution; }
		}
		private bool m_LoopEnable;
		public bool LoopEnable
		{
			get { return m_LoopEnable; }
		}
		private bool m_IsRare;
		public bool IsRare
		{
			get { return m_IsRare; }
		}

		private Vector m_Scale;
		public Vector Scale
		{
			get { return m_Scale; }
			set {	m_Scale = value; }
		}
		private Vector m_Position;
		public Vector Position
		{
			get { return m_Position; }
			set {	m_Position = value; }
		}
		private Vector m_WalkPosition;
		public Vector WalkPosition
		{
			get { return m_WalkPosition; }
			set {	m_WalkPosition = value; }
		}
    public bool WalkEnabled { get; set; }
    public float WalkSpeedOriginal { get; set; }
    public bool m_ResetLoopFlag = false;
		private Vector m_EulerRotation;	//!< @brief オイラー角(度数法なので注意)
		public Vector EulerRotation
		{
			get { return m_EulerRotation; }
			set	{ m_EulerRotation = value; }
		}
		public float LightRotationX { set; get; }
		public float LightRotationY { set; get; }

		private bool m_Overrided;
		public bool Overrided
		{
			get { return m_Overrided; }
		}
		public void ResetOverrided()
		{
			m_Overrided = false;
		}
		private bool m_IsFaceControlEnable;
		public bool IsFaceControlEnable
		{
			set { m_IsFaceControlEnable = value; }
		}

		//!< @brief 注視点操作可能フラグ
		private bool m_IsInterestPositionControllable;
		public bool IsInterestPositionControllable
		{
			get { return m_IsInterestPositionControllable; }
			set {
        if (m_motionConfigResourceRootNode == null)
        {
          return;
        }
				m_IsInterestPositionControllable = value;
				Vector position = null;
				if (value)
				{
					position = InterestPosition;
				}
        m_PokemonModel.SetInterestPosition(position);
			}
		}

		//!< @brief バイナリサイズ取得
		public Int32 BinarySize
		{
			get 
			{
				Int32 binarySize = 0;
        binarySize += m_GfMdlResourcePathSetDictionary.CalculateGfmdlFileSize(ConvertExtensionType.CTR);
        binarySize += m_GfMdlResourcePathSetDictionary.CalculateShaderFileSize(ConvertExtensionType.CTR);
        binarySize += m_GfMdlResourcePathSetDictionary.CalculateTextureFileSize(ConvertExtensionType.CTR);
        binarySize += m_GfmotResourcePathSet.CalculateBinarySize();
				return binarySize;
			}
		}

    public bool IsHeightControlEnable { get; set; }

    public bool IsSpringEnable { get; set; }

		/**
		 * @brief モデルのバイナリサイズを取得
		 * @param MotionType モーションの種類
		 * @return Int32 バイナリサイズ
		 */
    public Int32 CalculateBinarySize(GfmotPathSet.MotionType type)
		{
      Int32 binarySize = 0;
      binarySize += m_GfMdlResourcePathSetDictionary.CalculateGfmdlFileSize(ConvertExtensionType.CTR);
      binarySize += m_GfMdlResourcePathSetDictionary.CalculateShaderFileSize(ConvertExtensionType.CTR);
      binarySize += m_GfMdlResourcePathSetDictionary.CalculateTextureFileSize(ConvertExtensionType.CTR);
      binarySize += m_GfmotResourcePathSet.CalculateBinarySize(type);

      return binarySize;
		}

		/**
		 * @brief 各リソースのバイナリサイズをファイルに出力
		 * @param outputPath 出力先のパス
		 * @return bool 成功/失敗
		 */
		public bool OutputBinarySize()
		{
      //string outputPath = System.IO.Path.GetFileNameWithoutExtension(GfmdlFilePath) + "_binary_size.txt";
      //StreamWriter streamWriter = new StreamWriter(
      //  outputPath,
      //  false,
      //  System.Text.Encoding.UTF8);

      //Int32 binarySize = 0;
      //Int32 battleBinarySize = 0;
      //Int32 kwBinarySize = 0;
      //streamWriter.WriteLine(m_gfmdlFileData.Item1 + " : " + m_gfmdlFileData.Item2.ToString());
      //foreach (var data in m_gfmotFileDataArray)
      //{
      //  streamWriter.WriteLine(data.Item1 + " : " + data.Item2.ToString());
      //  if (data.Item1.Contains("_ba"))
      //  {
      //    battleBinarySize += data.Item2;
      //  }
      //  else if (data.Item1.Contains("_kw"))
      //  {
      //    kwBinarySize += data.Item2;
      //  }
      //  binarySize += data.Item2;
      //}
      //streamWriter.WriteLine("モーション合計 : " + binarySize);
      //streamWriter.WriteLine("baモーション合計 : " + battleBinarySize);
      //streamWriter.WriteLine("kwモーション合計 : " + kwBinarySize);

      //binarySize = 0;
      //foreach (var data in m_shaderFileDataArray)
      //{
      //  streamWriter.WriteLine(data.Item1 + " : " + data.Item4.ToString());
      //  binarySize += data.Item4;
      //}
      //streamWriter.WriteLine("シェーダ合計 : " + binarySize);

      //binarySize = 0;
      //foreach (var data in m_textureFileDataArray)
      //{
      //  if (data.Item2 != GfmdlTextureType.Texture)
      //  {
      //    continue;
      //  }
      //  streamWriter.WriteLine(data.Item1 + " : " + data.Item2.ToString());
      //  binarySize += data.Item3;
      //}
      //streamWriter.WriteLine("テクスチャ合計 : " + binarySize);

      //binarySize = 0;
      //binarySize += m_gfmdlFileData.Item2;
      //Array.ForEach(m_shaderFileDataArray, data => binarySize += data.Item4);
      //Array.ForEach(m_textureFileDataArray, data => binarySize += (data.Item2 == GfmdlTextureType.Texture) ? data.Item3 : 0);
      //Array.ForEach(m_gfmotFileDataArray, data => binarySize += data.Item2);
      //streamWriter.WriteLine("全合計 : " + binarySize);
      //streamWriter.Close();

			return true;
		}

		public Vector InterestPosition { get; set; }			//!< @brief 振り向き時の注視点

		private ResourceNode[] m_pMotionResourceRootNodes;
    private GfBinaryMotData[] m_pMotionResourceBinaryDatas;
		private Int32 m_motionNum;
		private List<IModelMotionResourceChangeListener> m_ModelMotionResourceChangeListenerList;

		private ResourceNode[] m_pFaceMotionResourceRootNodes;
		private Int32 m_EyeIndex;
		public Int32 EyeIndex
		{
			get { return m_EyeIndex; }
			set { m_EyeIndex = value; }
		}
		private Int32 m_MouthIndex;
		public Int32 MouthIndex
		{
			get { return m_MouthIndex; }
			set { m_MouthIndex = value; }
		}
		private Int32 m_LoopIndex;
		public Int32 LoopIndex
		{
			get { return m_LoopIndex; }
			set { m_LoopIndex = value; }
		}

    public List<Mcnf.Mcnf> McnfRootNodeList
    {
      get { return mcnfRootNodeList; }
    }
    private List<Mcnf.Mcnf> mcnfRootNodeList = new List<Mcnf.Mcnf>();

    private ResourceNode m_OldResourceNode = null;

		public void SetVisible(bool visible)
		{
			m_PokemonModel.SetVisible(visible);
		}

		public void SetIdModelVisible(bool visible)
		{
			if (m_IdModel != null)
			{
				m_IdModel.SetVisible(visible);
			}
		}

		// @fixme : これは2/13までに必ず消すこと！
		public void ReplaceWithIdTexture()
		{
			m_PokemonModel.ReplaceWithIdTexture();
		}

    private GfblocPathSet m_GfblocPathSet;
    public GfblocPathSet GfblocPathSet
    {
      get { return m_GfblocPathSet; }
    }

    private ViewerLocatorInstanceNode[] m_EffectLocatorInstanceNodes;
		public ViewerLocatorInstanceNode[] EffectLocatorInstanceNodes
		{
      get { return m_EffectLocatorInstanceNodes; }
		}
		public void SetFocusedToEffectLocators(bool focused)
		{
      if (m_EffectLocatorInstanceNodes != null)
      {
        Array.ForEach(m_EffectLocatorInstanceNodes, data => data.SetFocused(focused));
      }
		}

    private ViewerLocatorInstanceNode[] m_PokedecoLocatorInstanceNodes;
    public ViewerLocatorInstanceNode[] PokedecoLocatorInstanceNodes
    {
      get { return m_PokedecoLocatorInstanceNodes; }
    }
    public void SetFocusedToPokedecoLocators(bool focused)
    {
      if (m_PokedecoLocatorInstanceNodes != null)
      {
        Array.ForEach(m_PokedecoLocatorInstanceNodes, data => data.SetFocused(focused));
      }
    }

		/**
		 * @brief レアカラーの設定を行う
		 * @param gfmdlFilePath gfmdlファイルのパス
		 */
		private void ChangeToRareColor(string gfmdlFileDirectory, string outputFileDirectory)
		{
      var rareTextureTuples = m_GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].textureRareFilePathTuples;
      // @todo : 見つからないならそもそも「レアカラー読み込み」のボタンを押せないようにしておくべき
      if (rareTextureTuples == null)
      {
        MessageBox.Show("レアカラー用のテクスチャが見つかりませんでした。");
        return;
      }

      var textureTuples = m_GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].textureFilePathTuples;
      string settingFilePath = System.IO.Directory.GetParent(m_ImportDirectory.TrimEnd('/')).ToString();
      string[] xmlFiles = System.IO.Directory.GetFiles(settingFilePath, "*_rare_setting.xml");

      foreach (var rareTextureTuple in rareTextureTuples)
      {
        string tgaFilePath = rareTextureTuple.Item1.Replace(".wingl.btex", ".tga");
        m_PokemonModel.CreateRareColorTexture(tgaFilePath);
      }
      m_PokemonModel.ReplaceWithRareColorTexture();

      // レアカラーのコンスタントカラー変更処理を行う
      // exportsの一つ上のディレクトリにあるpmXXXX_YY_rare_setting.xmlを読み込み
      if (xmlFiles.Length == 0)
      {
        MessageBox.Show("レアカラーセッティング用のxmlファイルが見つかりませんでした。\n参照ディレクトリ : " + settingFilePath);
        return;
      }

      Int32 startIndex = settingFilePath.LastIndexOf('\\');
      Int32 endIndex = settingFilePath.LastIndexOf('_');
      Int32 subStringLength = endIndex - startIndex - 1;
      if (startIndex <= 0 || endIndex <= 0)
      {
        MessageBox.Show("レアカラーセッティング用のxmlファイルが命名規則と一致していません。pmXXXX_YY_NAMEの形式になっているか確認してください。");
        return;
      }

      string pmXXXX_YY = settingFilePath.Substring(startIndex + 1, subStringLength);
      string settingFileName = settingFilePath + "\\" + pmXXXX_YY + "_rare_setting.xml";
      if (!System.IO.File.Exists(settingFileName))
      {
        MessageBox.Show("レアカラーセッティング用のxmlファイルが見つかりませんでした。\n参照ディレクトリ : " + settingFilePath);
        return;
      }

      // Excelファイルを開いてデータ取得
      var rareSettingList = ExcelXml.ExcelXml.Deserialize<RareSettingXml.RareSettingList>(settingFileName);
      foreach (var info in rareSettingList.rareColorConstantSettingInfoList)
      {
        m_PokemonModel.AddRareConstantColor(info);
      }
		}

		/**
		 * @brief エフェクト発生ロケータのファイル読み込み
		 * @param gfmdlFilePath gfmdlファイルのパス
		 */
		private void LoadEffectLocatorNode(string effectPath)
		{
      if (string.IsNullOrEmpty(effectPath))
      {
        return;
      }

			if (!System.IO.File.Exists(effectPath))
			{
				return;
			}

      m_EffectLocatorInstanceNodes = m_PokemonModel.LoadEffectLocatorInstanceNodes(effectPath);
		}

    /**
     * @brief エフェクト発生ロケータのファイル読み込み
     * @param gfmdlFilePath gfmdlファイルのパス
     */
    private void LoadPokedecoLocatorNode(string pokedecoGfblocPath)
    {
      if (string.IsNullOrEmpty(pokedecoGfblocPath))
      {
        return;
      }

      if (!System.IO.File.Exists(pokedecoGfblocPath))
      {
        return;
      }

      m_PokedecoLocatorInstanceNodes = m_PokemonModel.LoadEffectLocatorInstanceNodes(pokedecoGfblocPath);

      // ポケデコの命名規則に則った名前を取得
      PokedecoLocatorBoundingBoxNames = new string[m_PokedecoLocatorInstanceNodes.Length];
      foreach (string key in PokedecoTypeDictionary.Keys)
      {
        PokedecoLocatorBoundingBoxNames = Array.ConvertAll(PokedecoLocatorBoundingBoxNames, name => name = key);
        break;
      }
    }

		/**
		 * @brief ポケモンのセッティング情報読み込み(体長, AABBの設定値など)
		 * @param gfmdlFilePath gfmdlファイルのパス
		 * @return bool 成功/失敗
		 */
		private bool LoadSettingFile(string importDirectory)
		{
      string settingFilePath = System.IO.Directory.GetParent(importDirectory.TrimEnd('/')).ToString();
			string[] xmlFiles = System.IO.Directory.GetFiles(settingFilePath, "*_setting.xml");

			// exportsの一つ上のディレクトリにあるpmXXXX_YY_setting.xmlを読み込み
			if (xmlFiles.Length <= 0)
			{
				MessageBox.Show("pmXXXX_YY_setting.xmlファイル(体長, AABB管理ファイル)が見つかりませんでした。\n参照ディレクトリ : " + settingFilePath,
                        null,
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Exclamation,
                        MessageBoxDefaultButton.Button1,
                        MessageBoxOptions.DefaultDesktopOnly);
				return false;
			}

			Int32 startIndex = settingFilePath.LastIndexOf('\\');
			Int32 endIndex = settingFilePath.LastIndexOf('_');
			Int32 subStringLength = endIndex - startIndex - 1;
			if (startIndex <= 0 || endIndex <= 0)
			{
				return false;
			}

			string pmXXXX_YY = settingFilePath.Substring(startIndex + 1, subStringLength);
			string settingFileName = settingFilePath + "\\" + pmXXXX_YY + "_setting.xml";
			if (!System.IO.File.Exists(settingFileName))
			{
				MessageBox.Show("pmXXXX_YY_setting.xmlファイル(体長, AABB管理ファイル)が見つかりませんでした。\n参照ディレクトリ : " + settingFilePath,
                        null,
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Exclamation,
                        MessageBoxDefaultButton.Button1,
                        MessageBoxOptions.DefaultDesktopOnly);
				return false;
			}

			Setting.SettingList settingList = ExcelXml.ExcelXml.Deserialize<Setting.SettingList>(settingFileName);

			// excel上のデータからCmHeightとAdjustHeightの取得
			float cmHeight = (float)settingList.dictionary["CmHeight"];
			DefaultCmHeight = cmHeight;

			float adjustHeight = (float)settingList.dictionary["AdjustHeight"];
			if (adjustHeight == 0.0f)
			{// 0に設定されていたらCmHeightから算出
				adjustHeight = (float)Math.Pow(1.0f / (0.01 * cmHeight), 0.43) * cmHeight;
				adjustHeight = MathUtil.Truncate(adjustHeight, 0);
			}
			DefaultAdjustHeight = adjustHeight;

      float fieldAdjust = 0.0f;
      if (settingList.dictionary.Any(data => data.Key == "FieldAdjust"))
      {// 2015/07以前のsetting.xmlにはFieldAdjustが存在していないので別途対応
        fieldAdjust = (float)settingList.dictionary["FieldAdjust"];
        fieldAdjust = MathUtil.Truncate(fieldAdjust, 0);
      }
      if (fieldAdjust == 0.0f)
      {
        fieldAdjust = adjustHeight;
      }
      DefaultFieldAdjust = fieldAdjust;
      FieldAdjust = fieldAdjust;

      PokemonModel.SetHeight(DefaultCmHeight, DefaultAdjustHeight);
      Size = (PokemonSize.Enum)settingList.dictionary["Size"];

			// excel上のデータからAABBの情報を取得
			float aabbMinX = (float)settingList.dictionary["MinX"];
			float aabbMinY = (float)settingList.dictionary["MinY"];
			float aabbMinZ = (float)settingList.dictionary["MinZ"];
			float aabbMaxX = (float)settingList.dictionary["MaxX"];
			float aabbMaxY = (float)settingList.dictionary["MaxY"];
			float aabbMaxZ = (float)settingList.dictionary["MaxZ"];
			AABB aabb = new AABB(new Vector(aabbMinX, aabbMinY, aabbMinZ), new Vector(aabbMaxX, aabbMaxY, aabbMaxZ));
			PokemonModel.SetModelAABB(aabb);
			DefaultModelAABB = aabb;

			return true;
		}

    private void LoadSettingDataFromGfmdl(string gfmdlFilePath)
    {
      XmlDocument gfmdlDocument = new XmlDocument();
      gfmdlDocument.Load(gfmdlFilePath);
      XmlElement rootElement = gfmdlDocument.DocumentElement;
      XmlNodeList nodeUserData = rootElement.GetElementsByTagName("UserData");
      var strCmHeight = nodeUserData[0].Attributes[0].Value.ToString();
      DefaultCmHeight = float.Parse(strCmHeight);
      if (DefaultCmHeight <= 0.0f)
      {
        DefaultCmHeight = 100.0f;		// デフォルトは1mにしておく
      }
      DefaultAdjustHeight = (float)Math.Pow(1.0f / (0.01 * DefaultCmHeight), 0.43) * DefaultCmHeight;
      DefaultAdjustHeight = MathUtil.Truncate(DefaultAdjustHeight, 0);

      if (DefaultCmHeight < 80.0f)
      {
        Size = PokemonSize.Enum.S;
      }
      else if (DefaultCmHeight < 180.0f)
      {
        Size = PokemonSize.Enum.M;
      }
      else
      {
        Size = PokemonSize.Enum.L;
      }
    }

    /**
     * @brief モーションのリソースのロード
     * @param gfmdlFileDirectory gfmdlを読み込んだディレクトリ
     */
    private void LoadMotionConfigResources(string[] mcnfFilePathList)
    {
      var hasData = mcnfFilePathList.Where((path) => !string.IsNullOrEmpty(path));
      if (hasData.Count() == 0)
      {
        return;
      }

      // かわいがり用を先頭に
      var kwList = mcnfFilePathList.Where(path => path.Contains("_kw.mcnf"));
      var otherList = mcnfFilePathList.Where(path => !path.Contains("_kw.mcnf"));

      var newList = kwList.Concat(otherList).ToArray();

      foreach (var mcnfFilePath in newList)
      {
        // Mcnf読み込み
        var rootNode = Mcnf.Mcnf.Deserialize(mcnfFilePath);
        rootNode.FileName = mcnfFilePath;
        McnfRootNodeList.Add(rootNode);
      }

      // 先頭のデータを読み込む
      AddMotionConfigResource(newList[0]);
    }

		/**
		 * @brief モーションのリソースの追加
		 * @param mcnfFilePaths モーションコンフィグのファイルパス
		 */
		public void AddMotionConfigResource(string filePath)
		{
      if (m_motionConfigResourceRootNode != null)
      {
        m_motionConfigResourceRootNode.Dispose();
        m_motionConfigResourceRootNode = null;
      }

			McnfData mcnfData = new McnfData(filePath: filePath);
			m_motionConfigResourceRootNode = gfl2.clr.renderingengine.scenegraph.resource.ResourceManager.CreateResourceNode(mcnfData);
			mcnfData.Dispose();

      if (m_PokemonModel != null)
      {
        m_PokemonModel.ResetMotionConfig(m_motionConfigResourceRootNode);
      }
		}

		/**
		 * @brief IDモデルの作成
		 * @param motionNum モーションの総数
		 * @param idTextureDirectory IDテクスチャのパス
		 * @return PokemonModel IDモデル
		 */
		public PokemonModel CreateIdModel(string idTexturesDirectory)
		{
			if (!System.IO.Directory.Exists(idTexturesDirectory))
			{
				//MessageBox.Show(
				//	"IDテクスチャ(かわいがりで使用するタッチ判定用のテクスチャ)の配置されているディレクトリが見つかりませんでした。\n	参照ディレクトリ : " + idTexturesDirectory);
				return null;
			}

			string[] idTexturePaths = System.IO.Directory.GetFiles(idTexturesDirectory, "*Id.tga");
			if (idTexturePaths.Length == 0)
			{
				//MessageBox.Show(
				//	"IDテクスチャ(かわいがりで使用するタッチ判定用のテクスチャ)が見つかりませんでした。\n参照ディレクトリ : " + idTexturesDirectory);
				return null;
			}

			PokemonModel idModel = new PokemonModel();
      idModel.Create(m_pModelResourceRootNode, null, m_PokemonModel);
			foreach (var path in idTexturePaths)
			{
        idModel.CreateIdTexture(path);
			}
      idModel.ReplaceWithIdTexture();
      idModel.SetVisible(false);    // 最初は見えないようにしておく

      return idModel;
		}

    public PokemonModel CreateShadowModel(
      GfMdlResourcePathSetDictionary gfmdlResourcePathSetDirectory,
      string outputFileDirectory,
      PokemonModel refModel)
    {
      if (gfmdlResourcePathSetDirectory == null)
      {
        return null;
      }

      // コンバート後のリソースを使用してリソースデータを作成
      m_pShadowModelResourceBinaryData = new GfBinaryMdlData(File.ReadAllBytes(gfmdlResourcePathSetDirectory[ConvertExtensionType.WinOpenGL].gfbmdlFilePath));
      m_pShadowModelResourceRootNode = ResourceManager.CreateResourceNode(m_pShadowModelResourceBinaryData);

      string[] shaderResourcePaths = gfmdlResourcePathSetDirectory.OutputShaderResourcePaths(ConvertExtensionType.WinOpenGL);
      m_pShadowModelResourceRootNode.AddShaderResources(shaderResourcePaths);
      string[] textureResourcePaths = gfmdlResourcePathSetDirectory.OutputTextureResourcePaths(ConvertExtensionType.WinOpenGL);
      m_pShadowModelResourceRootNode.AddTextureResources(textureResourcePaths);

      // PokemonShadowModelクラスにリソースを渡す
      PokemonModel model = new PokemonModel();
      model.Create(m_pShadowModelResourceRootNode, null, refModel);
      return model;
    }

		/**
		 * @brief コンストラクタ(gfmdlファイルのパスを受け取る)
		 * @param gfmdlFilePath gfmdl形式のファイルパス
		 * @param タイムラインデータの総数
		 * @param 扱えるモーションの総数
		 * @param 再生時のフレーム分解能
		 * @param ループON/OFF
		 * @param レアカラーの有効/無効
		 */
		public PokemonModelDataManager(
      string importDirectory,
      GfMdlResourcePathSetDictionary gfmdlResourcePathSetDirectory,
      GfMdlResourcePathSetDictionary shadowGfmdlResourcePathSetDirectory,
      GfmotPathSet gfmotPathSet,
      GfblocPathSet gfblocPathSet,
      ExtraFilePathSet extraFilePathSet,
			Int32 timeLineLength,
			Int32 motionNum,
			decimal frameResolution,
			bool loopEnable,
			bool isRare)
		{
      m_ImportDirectory = importDirectory;
      m_GfMdlResourcePathSetDictionary = gfmdlResourcePathSetDirectory;
      m_ShadowGfMdlResourcePathSetDictionary = shadowGfmdlResourcePathSetDirectory;
			m_PokemonModelInfo = new gfl2.clr.poke_3d_lib.PokemonModel.PokemonModelInfo();
      m_GfmotResourcePathSet = gfmotPathSet;
      m_GfblocPathSet = gfblocPathSet;
      m_ExtraFilePathSet = extraFilePathSet;

			m_FrameResolution = frameResolution;
			m_LoopEnable = loopEnable;

			m_IsRare = isRare;

			m_Scale = new Vector(1.0f, 1.0f, 1.0f);
			m_Position = new Vector(0.0f, 0.0f, 0.0f);
      m_WalkPosition = new Vector(0.0f, 0.0f, 0.0f);
			m_EulerRotation = new Vector(0.0f, 0.0f, 0.0f);

      m_motionNum = 0;

			// gfmdlのあるディレクトリ名を取得(何かとこの後使う)
      string gfbmdlFilePath = GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].gfbmdlFilePath;
      string outputFileDirectory = System.IO.Path.GetDirectoryName(gfbmdlFilePath);
      string gfmdlFileDirectory = m_GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].gfmdlFilePath;

      // gfmdlファイルからプリセットの配列を作成
      m_PresetNameDictionary = CreateMaterialPresetNameDictionary(gfmdlFileDirectory);

      // gfmdlファイルから参照テクスチャの情報を取得
      m_ReferenceTableDictionary = CreateReferenceTableTexturePathsDictionary(gfmdlFileDirectory);

      // コンバート後のリソースを使用してリソースデータを作成
      m_pModelResourceBinaryData = new GfBinaryMdlData(File.ReadAllBytes(gfbmdlFilePath));
      m_pModelResourceRootNode = ResourceManager.CreateResourceNode(m_pModelResourceBinaryData);

      string[] shaderResourcePaths = GfMdlResourcePathSetDictionary.OutputShaderResourcePaths(ConvertExtensionType.WinOpenGL);
      m_pModelResourceRootNode.AddShaderResources(shaderResourcePaths);
      string[] textureResourcePaths = GfMdlResourcePathSetDictionary.OutputTextureResourcePaths(ConvertExtensionType.WinOpenGL);
      m_pModelResourceRootNode.AddTextureResources(textureResourcePaths);

      // モーションコンフィグの読み込み
      LoadMotionConfigResources(m_ExtraFilePathSet.McnfPathList);

      // PokemonModelクラスにリソースを渡す
      m_PokemonModel = new PokemonModel();
      m_PokemonModel.Create(m_pModelResourceRootNode, m_motionConfigResourceRootNode, null);
      if (isRare)
      {
        ChangeToRareColor(gfmdlFileDirectory, outputFileDirectory);
      }
      m_PokemonModel.CreateColorShaderTextures();
			m_PokemonModel.SetVisible(true);

			// カラーシェーダ用の色を設定(毒食らってほんのり紫色になるとかはこの設定)
			m_PokemonModel.SetEnabledColorShaderTexture(false);
			m_PokemonModel.SetInterpColor(new gfl2.clr.math.Vector(0.0f, 0.0f, 0.0f, 0.0f));
			// 環境光用の色を指定(洞窟に入ったときに暗くなるとかはこの設定)
			m_PokemonModel.SetEnabledAmbientColor(false);
			m_PokemonModel.SetAmbientColor(new gfl2.clr.math.Vector(0.5f, 0.5f, 0.5f, 0.5f));

      // IDモデルの作成もここで行う
      // 基本的にPokemonModelと同じリソースで,
      // ID用のテクスチャに差し替える処理を追加.
      string idTexturesDirectory = System.IO.Path.GetDirectoryName(m_GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].gfbmdlFilePath);
      idTexturesDirectory += "\\Textures";
      m_IdModel = CreateIdModel(idTexturesDirectory);

      // 影用のモデルを作成
      m_ShadowModel = CreateShadowModel(shadowGfmdlResourcePathSetDirectory, outputFileDirectory, m_PokemonModel);

      // 本体マテリアルとの対応テーブルを作成
      m_ShadowMaterialIndexList = null;
      if (m_ShadowModel != null)
      {
        if (m_ShadowModel.GetMaterialCount() > 0)
        {
          m_ShadowMaterialIndexList = new int[m_ShadowModel.GetMaterialCount()];

          for (var i = 0; i < m_ShadowModel.GetMaterialCount(); ++i)
          {
            var materialName = m_ShadowModel.GetMaterialName(i);
            m_ShadowMaterialIndexList[i] = m_PokemonModel.GetMaterialIndex(materialName);
          }
        }
      }

			// 表情のモーション上書き用の配列を用意
			m_pFaceMotionResourceRootNodes = new ResourceNode[(Int32)OverridePartsType.Enum.NumberOf];

			// ポケビューア上では開始インデックスが1なのでそれに合わせる
			m_EyeIndex = 1;
			m_MouthIndex = 1;
			m_LoopIndex = 1;

			if (timeLineLength > 0)
			{
				m_AnimationTimeLine = new AnimationTimeLine(timeLineLength, frameResolution, loopEnable);
				m_AnimationTimeLine.Model = m_PokemonModel;		// サウンドキーのラベルの色を切り替えるのに必要
			}
			if (motionNum > 0)
			{
				//m_gfmotFilePaths = new string[motionNum];
				m_pMotionResourceRootNodes = new ResourceNode[motionNum];
				m_pMotionResourceBinaryDatas = new GfBinaryMotData[motionNum];
			}

      LoadGfmotResources(gfmotPathSet);

			m_ModelMotionResourceChangeListenerList = new List<IModelMotionResourceChangeListener>();

			LightRotationX = 0.0f;
			LightRotationY = 0.0f;

			// 視線制御の設定
			InterestPosition = new Vector(0.0f, 0.0f, 250.0f);
			IsInterestPositionControllable = false;

			IsInnerTubeVisible = false;
			IsOuterTubeVisible = false;
			IsModelAABBVisible = false;
			InnerTubeColor = new gfl2.clr.math.Vector(1.0f, 1.0f, 0.5f, 0.5f);
			OuterTubeColor = new gfl2.clr.math.Vector(1.0f, 1.0f, 0.5f, 0.5f);
			ModelAABBColor = new gfl2.clr.math.Vector(1.0f, 1.0f, 0.5f, 0.5f);

			DefaultModelAABB = null;

      // ロケータの情報読み込み(ココから火を吐きますとかそういう情報)
      LoadEffectLocatorNode(gfblocPathSet[ModelViewer.GfblocPathSet.LocatorType.EFFECT]);

      // ポケデコのロケータ情報の読み込み
      LoadPokedecoLocatorNode(gfblocPathSet[ModelViewer.GfblocPathSet.LocatorType.POKEDECO]);

      DefaultCmHeight = 100.0f;
      DefaultAdjustHeight = 100.0f;
      // セッティングファイルを読み込み(体長とかAABBとか)
      bool isSucceeded = LoadSettingFile(importDirectory);
      if (!isSucceeded)
      {
        LoadSettingDataFromGfmdl(GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].gfmdlFilePath);
      }

      m_PokemonModel.SetHeight(DefaultCmHeight, DefaultAdjustHeight);

			m_Scale.x = DefaultScale;
			m_Scale.y = DefaultScale;
			m_Scale.z = DefaultScale;

      IsHeightControlEnable = false;

      // 表情の使用/未使用の判定
      GFPokemonFile.GfmotFaceDataReader gfmotFaceDataReader = new GFPokemonFile.GfmotFaceDataReader();
      m_FacePatternUsedFlags = new Dictionary<FacePatternType, bool[]>();
      foreach (FacePatternType type in Enum.GetValues(typeof(FacePatternType)))
      {
        m_FacePatternUsedFlags[type] = new bool[NUMBER_OF_FACE_PATTERN];
        // 取得したモーションファイルの総フレーム数を確認し、最小値を取得
        string[] gfmotPaths = Array.ConvertAll(gfmotPathSet.GfmotFilePaths, path => path.Replace(".gfbmot", ".gfmot"));
        gfmotPaths = Array.FindAll(gfmotPaths, path => s_FaceMotionRegexDictionary[type].IsMatch(path));
        uint[] frames = new uint[gfmotPaths.Length];
        for (int index = 0; index < gfmotPaths.Length; ++index)
        {
          var faceData = gfmotFaceDataReader.CreateFaceDataFromGfmot(gfmotPaths[index]);
          frames[index] = faceData.FrameSize;
        }
        uint usedGfmotIndex = 0;
        // データが存在していたらインデックスを割り出す
        if (frames.Length > 0)
        {// フレームのデータがあるなら, 今度はフレームが0より大きい値が入っているかをチェック
          if (!frames.All(frame => frame == 0))
          {
            usedGfmotIndex = Math.Min((frames.Min() + 10) / 10, 8);
          }
        }

        bool[] usedGfmotFlags = new bool[NUMBER_OF_FACE_PATTERN];
        for (int i = 0; i < usedGfmotIndex; ++i)
        {
          usedGfmotFlags[i] = true;
        }
        string[] texturePaths = Array.ConvertAll(textureResourcePaths, path => path.Replace(".wingl.btex", ".tga"));
        texturePaths = Array.FindAll(texturePaths, path => s_FaceTextureRegexDictionary[type].IsMatch(path));
        texturePaths = Array.FindAll(texturePaths, path => path.Contains("1.tga"));    // Eye1.tga, Mouth1.tga以外はカラーテクスチャではないので除外

        // tgaファイルをバイナリで開いてチェック
        TargaFileReader targaReader = new TargaFileReader();
        bool[] usedTexFlags = new bool[NUMBER_OF_FACE_PATTERN];
        usedTexFlags = Array.ConvertAll(usedTexFlags, data => data = true);
        foreach (string filePath in texturePaths)
        {
          var targaData = targaReader.CreateTargaDataFromFile(filePath);
          var targaHeader = targaData.TargaHeader;
          if (!targaHeader.TurnOverU)
          {
            targaData.PixelDataArray.InverseVertically();
          }

          int index = 0;
          int blockWidth = targaHeader.width / 2;
          int blockHeight = targaHeader.height / 4;
          for (int w = 0; w < 2; ++w)
          {
            for (int h = 0; h < 4; ++h)
            {
              usedTexFlags[index] &= !targaData.PixelDataArray.IsFilledSameColor(w * blockWidth, h * blockHeight, blockWidth, blockHeight);
              ++index;
            }
          }
        }

        for (int i = 0; i < NUMBER_OF_FACE_PATTERN; ++i)
        {
          m_FacePatternUsedFlags[type][i] = usedGfmotFlags[i] & usedTexFlags[i];
        }
      }

      // UI上で表示するノードの階層構造を作成
      m_PokemonModel.CreateModelNameTreeNode();
    }

    int ExtractMotionFrameFromFaceGfmot(string gfmotFilePath)
    {
      int frameSize = 0;
      // gfmdlをパースしてテクスチャのリストを作成
      XmlDocument xmlDocument = new XmlDocument();
      try
      {
        xmlDocument.Load(gfmotFilePath);

        // gfmdlのバージョンをチェック
        var gfmdlNode = xmlDocument.SelectSingleNode("/GfMotion");

        // ジョイントが使われているかどうかを判別
        var skeltalAnimationNodes = xmlDocument.SelectNodes("/GfMotion/SkeltalAnimation");
        if (skeltalAnimationNodes.Count > 0)
        {
          frameSize = int.Parse(gfmdlNode.Attributes["FrameSize"].Value);
          return frameSize;
        }

        // UVが使われているかどうかを判別
        var uvAnimationNodes = xmlDocument.SelectNodes("/GfMotion/UvAnimation");
        if (uvAnimationNodes.Count > 0)
        {
          frameSize = int.Parse(gfmdlNode.Attributes["FrameSize"].Value);
          return frameSize;
        }

      }
      catch (Exception e)
      {
        Console.WriteLine(e.StackTrace);
      }

      return frameSize;
    }

		/**
		 * @brief 破棄
		 */
		public void Dispose()
		{
			if (m_motionConfigResourceRootNode != null)
			{
				m_motionConfigResourceRootNode.Dispose();
				m_motionConfigResourceRootNode = null;
			}

			if (m_pModelResourceRootNode != null)
			{
				m_pModelResourceRootNode.Dispose();
				m_pModelResourceRootNode = null;

        m_pModelResourceBinaryData.Dispose();
        m_pModelResourceBinaryData = null;
			}

			if (m_pMotionResourceRootNodes != null)
			{
				for (Int32 i = 0; i < m_pMotionResourceRootNodes.Length; ++i)
				{
					if (m_pMotionResourceRootNodes[i] != null)
					{
						m_pMotionResourceRootNodes[i].Dispose();
						m_pMotionResourceRootNodes[i] = null;

            m_pMotionResourceBinaryDatas[i].Dispose();
            m_pMotionResourceBinaryDatas[i] = null;
					}
				}
			}

			if (m_PokemonModel != null)
			{
				m_PokemonModel.Destroy();
				m_PokemonModel.Dispose();
				m_PokemonModel = null;
			}

			if (m_IdModel != null)
			{
				m_IdModel.Destroy();
				m_IdModel.Dispose();
				m_IdModel = null;
			}

      if (m_pShadowModelResourceRootNode != null)
      {
        m_pShadowModelResourceRootNode.Dispose();
        m_pShadowModelResourceRootNode = null;
        m_pShadowModelResourceBinaryData.Dispose();
        m_pShadowModelResourceBinaryData = null;
      }

			if (m_ShadowModel != null)
			{
				m_ShadowModel.Destroy();
				m_ShadowModel.Dispose();
				m_ShadowModel = null;
			}
		}

		/**
		 * @brief 変更通知
		 */
		public void NotifyToListeners()
		{
			m_ModelMotionResourceChangeListenerList.ForEach(listener => listener.ReceiveNotification(this));
		}

		/**
		 * @brief 変更通知相手を追加
		 * @param listener 変更通知相手
		 */
		public void AddListener(IModelMotionResourceChangeListener listener)
		{
			m_ModelMotionResourceChangeListenerList.Add(listener);
		}

		/**
		 * @brief 位置, 拡大率, 回転量を設定
		 * @param position 位置
		 * @param scale 拡大率
		 * @param eulerRotation 回転量(オイラー角指定)
		 */
		private void SetModelTransform(
			PokemonModel pokemonModel,
			gfl2.clr.math.Vector position,
			gfl2.clr.math.Vector scale,
			gfl2.clr.math.Vector eulerRotation)
		{
			if (pokemonModel == null)
			{
				return;
			}
			pokemonModel.SetTranslate(position);
			pokemonModel.SetScale(scale);
			pokemonModel.SetRotateZYX(eulerRotation * gfl2.clr.math.Util.Pai / 180.0f);
		}

		/**
		 * @brief info.dat出力
		 * @param 出力先のディレクトリ
		 */
		public bool OutputInfoDat(
			string outputDirectory,
			Matrix viewMatrix,
			float fieldOfView,
			float nearClip,
			float farClip,
      bool isBackgroundVisible,
      Byte markFrame1,
      Byte markFrame2,
      Byte markFrame3,
      Byte markFrame4
      )
		{
      GfmdlResourcePathSet pathSet = m_GfMdlResourcePathSetDictionary[ConvertExtensionType.CTR];
      m_PokemonModelInfo.m_GfmdlFilePath = pathSet.gfbmdlFilePath;

      string[] allShaderPath = m_GfMdlResourcePathSetDictionary.OutputShaderResourcePaths(ConvertExtensionType.CTR);
      m_PokemonModelInfo.m_GfvshFilePaths = Array.FindAll(allShaderPath, path => path.Contains(".gfbvsh"));
      m_PokemonModelInfo.m_GfgshFilePaths = Array.FindAll(allShaderPath, path => path.Contains(".gfbgsh"));
      m_PokemonModelInfo.m_GffshFilePaths = Array.FindAll(allShaderPath, path => path.Contains(".gfbfsh"));

      string[] allTexturePath = m_GfMdlResourcePathSetDictionary.OutputTextureResourcePaths(ConvertExtensionType.CTR);
      if (m_IsRare)
      {
        foreach (var rareTexturePath in m_GfMdlResourcePathSetDictionary.OutputRareTextureResourcePaths(ConvertExtensionType.CTR))
        {
          string rareTextureFileName = System.IO.Path.GetFileName(rareTexturePath);
          Int32 index = Array.FindIndex(allTexturePath, path => path.Contains(rareTextureFileName));
          if (index < 0)
          {
            continue;
          }
          allTexturePath[index] = rareTexturePath;
        }
      }
      m_PokemonModelInfo.m_TextureFilePaths = allTexturePath;

      m_PokemonModelInfo.m_GfmotFilePaths = m_GfmotResourcePathSet.GfmotFilePaths;

			// 出力前にm_PokemonModelInfoにフレームを設定
			m_PokemonModelInfo.m_ColorShaderColor = m_PokemonModel.GetInterpColor();
			m_PokemonModelInfo.m_AmbientColor = m_PokemonModel.GetAmbientColor();
			m_PokemonModelInfo.m_Scale = m_Scale;
			m_PokemonModelInfo.m_Rotation = m_PokemonModel.GetRotateZYX();
			m_PokemonModelInfo.m_Translate = m_Position;
			m_PokemonModelInfo.m_LightVector = m_PokemonModel.GetLightDirection();
			m_PokemonModelInfo.m_IsColorShaderEnabled = m_PokemonModel.IsEnabledColorShaderTexture() ? 1 : 0;
			m_PokemonModelInfo.m_IsAmbientColorEnabled = m_PokemonModel.IsEnabledAmbientColor() ? 1 : 0;
			m_PokemonModelInfo.m_CameraMatrix = viewMatrix;
			m_PokemonModelInfo.m_FieldOfView = fieldOfView;
			m_PokemonModelInfo.m_NearClip = nearClip;
			m_PokemonModelInfo.m_FarClip = farClip;
			for (Int32 i = 0; i < m_AnimationTimeLine.TimeLineDataArray.Length; ++i)
			{
				m_PokemonModelInfo.m_MotionIndices[i] = -1;
				if (m_AnimationTimeLine.TimeLineDataArray[i].IsAvailable)
				{
					m_PokemonModelInfo.m_MotionIndices[i] = m_AnimationTimeLine.TimeLineDataArray[i].MotionIndex;
				}
				m_PokemonModelInfo.m_MotionStartFrames[i] = (float)m_AnimationTimeLine.TimeLineDataArray[i].StartFrame;
				m_PokemonModelInfo.m_MotionEndFrames[i] = (float)m_AnimationTimeLine.TimeLineDataArray[i].EndFrame;
			}

			for (Int32 i = 0; i < m_AnimationTimeLine.TimeLineDataArray.Length - 1; ++i)
			{
				m_PokemonModelInfo.m_MotionInterpFrames[i] = m_AnimationTimeLine.TimeLineDataArray[i].InterpSize;
			}

			for (Int32 i = 0; i < (Int32)gfl2.clr.animation.OverridePartsType.Enum.NumberOf; ++i)
			{
				m_PokemonModelInfo.m_OverrideMotionIndices[i] = m_PokemonModel.GetOverridePartsMotionIndex(i);
			}

			m_PokemonModelInfo.m_IsPlayingAnimation = m_AnimationTimeLine.IsPlayingAnimation() ? 1 : 0;
			m_PokemonModelInfo.m_PlayingFrame = (Int32)m_AnimationTimeLine.CurrentFrame;

			Int32 stopMotionIndex;
			decimal stopMotionFrame;
			Int32 stopPrevMotionIndex;
			decimal stopPrevMotionFrame;
			decimal stopInterpWeight;

			m_AnimationTimeLine.GetCurrentMotion(
				out stopMotionIndex, out stopMotionFrame);
			m_AnimationTimeLine.GetCurrentBlendMotion(
				out stopPrevMotionIndex, out stopPrevMotionFrame, out stopInterpWeight);

			m_PokemonModelInfo.m_StopMotionIndex = stopMotionIndex;
			m_PokemonModelInfo.m_StopMotionFrame = (float)stopMotionFrame;
			m_PokemonModelInfo.m_StopPrevMotionIndex = stopPrevMotionIndex;
			m_PokemonModelInfo.m_StopPrevMotionFrame = (float)stopPrevMotionFrame;
			m_PokemonModelInfo.m_StopInterpWeight = (float)stopInterpWeight;

      for (Int32 i = 0; i < AnimationTimeLine.LoopAnimationBit.Length; ++i)
      {
        // GFModelViewerはnijiと異なり,
        // 0 : 通常のモーション
        // 1～10 : eye01～eye03, mouth01～mouth03, loop01～loop04
        // となっているため, インデックスを + 1している
        // (nijiのPokeModelクラスでは,
        // 0 : 通常のモーション
        // 1 : バトル時の登場のOrigin移動モーション
        // 2～11 : eye01～eye03, mouth01～mouth03, loop01～loop04
        // となっている)
        Int32 slotIndex = (Int32)gfl2.clr.animation.OverridePartsType.Enum.Loop01 + 1;
        m_PokemonModelInfo.m_LoopPlayingFrame[i] = 0;
        decimal endFrame = (decimal)m_PokemonModel.GetAnimationEndFrame(slotIndex, AnimationTimeLine.LoopAnimationBit[i]);
        if (endFrame > 0.0m)
        {
          m_PokemonModelInfo.m_LoopPlayingFrame[i] = (Int32)m_AnimationTimeLine.GetLoopPlayFrame(AnimationTimeLine.LoopAnimationBit[i]);
          m_PokemonModelInfo.m_LoopPlayingFrame[i] %= (Int32)endFrame;
        }
      }

      m_PokemonModelInfo.m_IsBackgroundVisible = isBackgroundVisible ? 1: 0;

      m_PokemonModelInfo.m_MarkFrame1 = markFrame1;
      m_PokemonModelInfo.m_MarkFrame2 = markFrame2;
      m_PokemonModelInfo.m_MarkFrame3 = markFrame3;
      m_PokemonModelInfo.m_MarkFrame4 = markFrame4;

			m_PokemonModel.OutputInfoDat(
				outputDirectory,
				m_PokemonModelInfo);

			return true;
		}

		/**
		 * @brief アニメーションタイムラインの更新
		 */
		public void Update()
		{
			// モデルのモーションを更新
			UpdateModelAnimation(m_PokemonModel);

      var position = m_Position + m_WalkPosition;
			SetModelTransform(m_PokemonModel, position, m_Scale, m_EulerRotation);
			SetModelTransform(m_IdModel, position, m_Scale, m_EulerRotation);
      SetModelTransform(m_ShadowModel, position, m_Scale, m_EulerRotation);

      // 振り向きモード有効なら注視位置を更新する
      if (IsInterestPositionControllable)
      {
				m_PokemonModel.SetInterestPosition(InterestPosition);
      }

      // ライトを更新
      float radianRotateX = gfl2.clr.math.Util.ConvDegToRad(LightRotationX + m_EulerRotation.y);
      float radianRotateY = gfl2.clr.math.Util.ConvDegToRad(LightRotationY);
      gfl2.clr.math.Vector unitZ = gfl2.clr.math.Vector.GetZUnit();
      gfl2.clr.math.Matrix mtxRotX = gfl2.clr.math.Matrix.GetRotationY(radianRotateX);
      gfl2.clr.math.Matrix mtxRotY = gfl2.clr.math.Matrix.GetRotationX(-radianRotateY);
      gfl2.clr.math.Vector lightDirection = -(mtxRotX * mtxRotY * unitZ);
      // PokemonModelにライトのベクトルを渡す
      m_PokemonModel.SetLightDirection(lightDirection);
      if (m_ShadowModel != null)
      {
        m_ShadowModel.SetLightDirection(lightDirection);
      }

      // 影モデルに本体のマテリアルUV座標をコピー
      if (m_ShadowModel != null)
      {
        for (var shadowMaterialIndex = 0; shadowMaterialIndex < m_ShadowModel.GetMaterialCount(); ++shadowMaterialIndex)
        {
          var materialIndex = m_ShadowMaterialIndexList[shadowMaterialIndex];
          if (materialIndex < 0)
          {
            continue;
          }

          for (var slotNo = 0; slotNo < 3; ++slotNo)
          {
            var scale = m_PokemonModel.GetMaterialTextureCoordinatorScaleUV(materialIndex, slotNo);
            var rotate = m_PokemonModel.GetMaterialTextureCoordinatorRotateUV(materialIndex, slotNo);
            var translate = m_PokemonModel.GetMaterialTextureCoordinatorTranslateUV(materialIndex, slotNo);

            m_ShadowModel.SetMaterialTextureCoordinatorScaleUV(shadowMaterialIndex, slotNo, scale);
            m_ShadowModel.SetMaterialTextureCoordinatorRotateUV(shadowMaterialIndex, slotNo, rotate);
            m_ShadowModel.SetMaterialTextureCoordinatorTranslateUV(shadowMaterialIndex, slotNo, translate);
          }
        }
      }

		}

    /**
     * @brief すべてのモーション(通常/補間, ループ, 表情)の一括設定
     * @param model モデルデータ
     */
    private void UpdateModelAnimation(PokemonModel model)
		{
			if (model == null)
			{
				return;	// IDモデルや影モデルなどがきたときを想定(これらの持っていないモデルもある)
			}

			// タイムラインを更新し, 現在のフレームを取得
			Int32 motionIndex = -1;
			decimal motionFrame = 0;
			Int32 prevMotionIndex = -1;

			decimal prevMotionFrame = 0;
			decimal interpRatio = 0.0m;

      // 前回のフレームのモーションインデックスを取得
      Int32 oldMotionIndex = -1;
      decimal oldMotionFrame = 0;
			m_AnimationTimeLine.GetCurrentMotion(out oldMotionIndex, out oldMotionFrame);
      Int32 oldPrevMotionIndex = -1;
      decimal oldPrevMotionFrame = 0;
			m_AnimationTimeLine.GetCurrentBlendMotion(out oldPrevMotionIndex, out oldPrevMotionFrame, out interpRatio);

      // コマ送り判定用
      var isOneFrame =
        (m_AnimationTimeLine.State == ModelViewer.AnimationTimeLine.STATE.BACKWARD_FRAME)
        || (m_AnimationTimeLine.State == ModelViewer.AnimationTimeLine.STATE.FORWARD_FRAME);

      // フレームを進める
			var isFinished_addFrame = m_AnimationTimeLine.Update(model);
      var isFinished = isFinished_addFrame.Item1;
      var addFrame = isFinished_addFrame.Item2;

			m_AnimationTimeLine.GetCurrentMotion(out motionIndex, out motionFrame);
			m_AnimationTimeLine.GetCurrentBlendMotion(out prevMotionIndex, out prevMotionFrame, out interpRatio);

      // モーションが切り替わったらループアニメーションをリセットする
      bool isMotionChanged = false;
      if (motionIndex >= 0)
      {
        ResourceNode currentResourceNode = m_PokemonModel.GetMotionResourceNode(motionIndex);
        isMotionChanged = (currentResourceNode != m_OldResourceNode);
        m_OldResourceNode = currentResourceNode;
      }
      if (isMotionChanged && !m_ResetLoopFlag)
      {
        ResetLoopMotion(model);
      }
      m_ResetLoopFlag = false;

			if (interpRatio >= 0.0m)
			{// 補間モーションがあったときの設定
				SetUpInterpMotion(model, prevMotionIndex, prevMotionFrame, motionIndex, motionFrame);
			}
			else
			{// 補間モーションがなかったときの設定
				SetUpMotion(model, motionIndex, motionFrame);
        interpRatio = -1.0m;
			}

      // 歩き用移動速度更新
      decimal walkFrame0 = motionFrame;
      decimal walkFrame1 = motionFrame;
      if (interpRatio >= 0.0m)
      {
        // 補間用
        walkFrame0 = prevMotionFrame;
      }

      // モーションが一巡していたらフレームの加算を0にする
      float walkSpeedAddFrame = (float)addFrame;
      if (isFinished)
      {
        walkSpeedAddFrame = 0.0f;
      }

      Vector walkSpeed = model.GetWalkSpeedManual(walkSpeedAddFrame, (float)walkFrame0, (float)walkFrame1, (float)interpRatio);
      m_WalkPosition.x = walkSpeed.x;
      m_WalkPosition.y = walkSpeed.y * m_Scale.y;

      // コマ送り時に一瞬高さがぶれる問題の対処。他に影響させないためにチェックをきつくして影響範囲を狭める
      // 補間無し＆モーションが切り替わったら高さは０フレーム目を取得させる
      if ((!isFinished) && (interpRatio < 0.0m) && (motionIndex != oldMotionIndex))
      {
        walkSpeed = model.GetWalkSpeedManual(0, (float)walkFrame0, (float)walkFrame1, (float)interpRatio);
        m_WalkPosition.y = walkSpeed.y * m_Scale.y;
      }

      if (WalkEnabled)
      {
        m_WalkPosition.z += walkSpeed.z * m_Scale.z;
      }
      else
      {
        m_WalkPosition.z = 0.0f;
      }
      if (Math.Abs(addFrame) > 0.0m)
      {
        WalkSpeedOriginal = walkSpeed.z;
      }

      // リストの再生が終わったら位置を戻す
      if (isFinished || (m_AnimationTimeLine.State == ModelViewer.AnimationTimeLine.STATE.STOP))
      {
        m_WalkPosition.z = 0;
      }

      // 前回のループアニメーションフラグ取得
      bool[] oldLoopFlags = new bool[]
      {
        model.GetLoopAnimationFlag((int)gfl2.clr.animation.OverridePartsType.Enum.Loop01 - (int)gfl2.clr.animation.OverridePartsType.Enum.Loop01),
        model.GetLoopAnimationFlag((int)gfl2.clr.animation.OverridePartsType.Enum.Loop02 - (int)gfl2.clr.animation.OverridePartsType.Enum.Loop01),
        model.GetLoopAnimationFlag((int)gfl2.clr.animation.OverridePartsType.Enum.Loop03 - (int)gfl2.clr.animation.OverridePartsType.Enum.Loop01),
        model.GetLoopAnimationFlag((int)gfl2.clr.animation.OverridePartsType.Enum.Loop04 - (int)gfl2.clr.animation.OverridePartsType.Enum.Loop01),
      };

      // キーアニメーションの更新
      model.UpdateKeyAnimation();

			// ループがOFFからONになったらループをリセット
			for (uint loopIndex = 0; loopIndex < oldLoopFlags.Length; ++loopIndex)
			{
				if (oldLoopFlags[loopIndex])
				{
					continue;
				}
				if (!model.GetLoopAnimationFlag(loopIndex))
				{
					continue;
				}

        foreach (var loopAnimationBit in AnimationTimeLine.LoopAnimationBit)
        {
          m_AnimationTimeLine.SetLoopPlayFrame(loopAnimationBit, 0.0m);
        }
				break;
			}

			// 表情アニメーションの設定
			SetUpFaceMotion(model);

			// ループアニメーション(ヤンヤンマの羽, メガハガネールの輪っかなど)の設定
			SetUpLoopMotion(model);

			// 補間フレームに応じてアニメーションを更新(0.0fなら補間なしで再生)
			model.UpdateAnimation((float)interpRatio);
		}

		/**
		 * @brief 補間モーションの設定
		 * @param model モデルデータ
		 * @param prevMotionIndex 補間前モーションのインデックス
		 * @param prevMotionFrame 補間前モーションのフレーム
		 * @param motionIndex 補間後モーションのインデックス
		 * @param motionFrame 補間後モーションのフレーム
		 */
		private void SetUpInterpMotion(PokemonModel model, Int32 prevMotionIndex, decimal prevMotionFrame, Int32 motionIndex, decimal motionFrame)
		{
			if (prevMotionIndex < 0 || motionIndex < 0)
			{
				return;
			}

			ResourceNode pMotionResourceNode = m_PokemonModel.GetMotionResourceNode(prevMotionIndex);
			ResourceNode pInterpMotionResourceNode = m_PokemonModel.GetMotionResourceNode(motionIndex);

      // GFModelViewerはnijiと異なり,
      // 0 : 通常のモーション
      // (nijiのPokeModelクラスでは,
      // 0 : 通常のモーション
      // 1 : バトル時の登場のOrigin移動モーション
      // となっている)
      int slotIndex = 0;
      PokemonModel.AnimationBit animationBit = PokemonModel.AnimationBit.ANIMATION_BIT_ALL;

			// 補間前モーションの設定
      m_PokemonModel.ChangeAnimationSmoothManual(
        pMotionResourceNode,
        pInterpMotionResourceNode,
        slotIndex,
        animationBit);
      m_PokemonModel.SetAnimationControllTargetInterp(false);
      m_PokemonModel.SetAnimationFrame((float)prevMotionFrame, slotIndex, animationBit);
      m_PokemonModel.SetAnimationStepFrame(0.0f, slotIndex, animationBit);

			// 補間後のモーションの設定
      m_PokemonModel.SetAnimationControllTargetInterp(true);
			m_PokemonModel.SetAnimationFrame((float)motionFrame, slotIndex, animationBit);
      m_PokemonModel.SetAnimationStepFrame(0.0f, slotIndex, animationBit);

      m_PokemonModel.SetAnimationControllTargetInterp(false);
		}

		/**
		 * @brief 通常モーション(補間なし)の設定
		 * @param model モデルデータ
		 * @param motionIndex モーションのインデックス
		 * @param motionFrame モーションのフレーム
		 */
		private void SetUpMotion(PokemonModel model, Int32 motionIndex, decimal motionFrame)
		{
			ResourceNode pMotionResourceNode = null;
			if (motionIndex >= 0)
			{
				pMotionResourceNode = m_PokemonModel.GetMotionResourceNode(motionIndex);
			}

      // GFModelViewerはnijiと異なり,
      // 0 : 通常のモーション
      // (nijiのPokeModelクラスでは,
      // 0 : 通常のモーション
      // 1 : バトル時の登場のOrigin移動モーション
      // となっている)
      int slotIndex = 0;
      PokemonModel.AnimationBit animationBit = PokemonModel.AnimationBit.ANIMATION_BIT_ALL;

      model.ChangeAnimation(pMotionResourceNode, slotIndex, animationBit);
			model.SetAnimationFrame((float)motionFrame, slotIndex, animationBit);
		}

		/**
		 * @brief ループアニメーション(ヤンヤンマの羽, メガハガネールの輪っかなど)の設定
		 * @param model 設定するモデル
		 */
		private void SetUpLoopMotion(PokemonModel model)
		{
      // 15フレーム再生時のループモーション(ヤンヤンマの羽など)の設定
      for (Int32 loopMotionType = 0; loopMotionType < AnimationTimeLine.LoopAnimationBit.Length; ++loopMotionType)
      {
        // GFModelViewerはnijiと異なり,
        // 0 : 通常のモーション
        // 1～10 : eye01～eye03, mouth01～mouth03, loop01～loop04
        // となっているため, インデックスを + 1している
        // (nijiのPokeModelクラスでは,
        // 0 : 通常のモーション
        // 1 : バトル時の登場のOrigin移動モーション
        // 2～11 : eye01～eye03, mouth01～mouth03, loop01～loop04
        // となっている)
        int slotIndex = (int)gfl2.clr.animation.OverridePartsType.Enum.Loop01 + 1;
        PokemonModel.AnimationBit animationBit = AnimationTimeLine.LoopAnimationBit[loopMotionType];

        ResourceNode loopMotionResourceNode = null;
        // コールバックで設定されたフラグを取得
        bool changeAnimationLoopFlag = model.GetLoopAnimationFlag((uint)loopMotionType);
        if (changeAnimationLoopFlag)
        {
          loopMotionResourceNode = m_pFaceMotionResourceRootNodes[slotIndex - 1];
        }

        model.ChangeAnimationSmoothManual(loopMotionResourceNode, loopMotionResourceNode, slotIndex, animationBit);

        decimal loopMotionFrame = m_AnimationTimeLine.GetLoopPlayFrame(animationBit);
        decimal endFrame = (decimal)model.GetAnimationEndFrame(slotIndex, animationBit);
        if (loopMotionFrame < 0.0m)
        {// 逆再生時の対処
          // @todo: LoopPlayFrameをここで設定しているが, ループ再生以外でTotalPlayTimeを使用する場合はここを見直す必要がある
          loopMotionFrame += endFrame + 1.0m;
          m_AnimationTimeLine.SetLoopPlayFrame(animationBit, loopMotionFrame);
        }

        if (endFrame > 0.0m)
        {
          loopMotionFrame %= endFrame;
        }

        // 補間用スロットの設定
        model.SetAnimationControllTargetInterp(true);
        model.SetAnimationFrame((float)loopMotionFrame, slotIndex, animationBit);

        // 通常用スロットの設定
        model.SetAnimationControllTargetInterp(false);
        model.SetAnimationFrame((float)loopMotionFrame, slotIndex, animationBit);
      }
		}

		/**
		 * @brief ループアニメーション(ヤンヤンマの羽, メガハガネールの輪っかなど)をリセットする
		 * @param model 設定するモデル
		 */
		private void ResetLoopMotion(PokemonModel model)
		{
			// 15フレーム再生時のループモーション(ヤンヤンマの羽など)の設定
			for (Int32 loopMotionIndex = (Int32)gfl2.clr.animation.OverridePartsType.Enum.Loop01;
				loopMotionIndex <= (Int32)gfl2.clr.animation.OverridePartsType.Enum.Loop04;
				++loopMotionIndex)
			{
				UInt32 loopIndex = (UInt32)loopMotionIndex - (UInt32)gfl2.clr.animation.OverridePartsType.Enum.Loop01;

				// コールバックで設定されたフラグを取得
				bool oldLoopFlag = model.GetLoopAnimationFlag(loopIndex);

        // コールバックで設定されたフラグを立てた状態にする
        model.ResetLoopAnimationFlag(loopIndex);

        // ＯＦＦからＯＮになった場合にはフレームを０に戻す
        if (!oldLoopFlag)
        {
          for (Int32 i = 0; i < AnimationTimeLine.LoopAnimationBit.Length; ++i)
          {
            m_AnimationTimeLine.SetLoopPlayFrame(AnimationTimeLine.LoopAnimationBit[i], 0.0m);
          }
        }
			}
		}
    public void ResetLoopMotion()
    {
      ResetLoopMotion(m_PokemonModel);
      m_ResetLoopFlag = true;
    }

		/**
		 * @brief 表情のアニメーションの設定
		 */
		private void SetUpFaceMotion(PokemonModel model)
		{
			if (!m_IsFaceControlEnable)
			{
				// 表情の上書きが無ければnullを設定
				for (Int32 i = 0; i < m_pFaceMotionResourceRootNodes.Length; ++i)
				{
          // GFModelViewerはnijiと異なり,
          // 0 : 通常のモーション
          // 1～10 : eye01～eye03, mouth01～mouth03, loop01～loop04
          // となっているため, インデックスを + 1している
          // (nijiのPokeModelクラスでは,
          // 0 : 通常のモーション
          // 1 : バトル時の登場のOrigin移動モーション
          // 2～11 : eye01～eye03, mouth01～mouth03, loop01～loop04
          // となっている)
          int slotIndex = i + 1;    // 通常のモーションが0番のスロットを使っているため+1
          PokemonModel.AnimationBit animationBit =
            PokemonModel.AnimationBit.ANIMATION_BIT_JOINT |
            PokemonModel.AnimationBit.ANIMATION_BIT_MATERIAL |
            PokemonModel.AnimationBit.ANIMATION_BIT_VISIBILITY;

          model.ChangeAnimation(null, slotIndex, animationBit);
				}
				return;
			}

			for (Int32 i = 0; i < m_pFaceMotionResourceRootNodes.Length; ++i)
			{
				if (m_pFaceMotionResourceRootNodes[i] == null)
				{
					continue;
				}

				float frame = 0.0f;
				if (i <= (Int32)OverridePartsType.Enum.Eye03)
				{
					frame = (float)(m_EyeIndex - 1) * 10.0f;
				}
				else if (i <= (Int32)OverridePartsType.Enum.Mouth03)
				{
					frame = (float)(m_MouthIndex - 1) * 10.0f;
				}
				else if (i <= (Int32)OverridePartsType.Enum.Loop04)
				{
					frame = (float)(m_LoopIndex - 1) * 10.0f;
				}

        // GFModelViewerはnijiと異なり,
        // 0 : 通常のモーション
        // 1～10 : eye01～eye03, mouth01～mouth03, loop01～loop04
        // となっているため, インデックスを + 1している
        // (nijiのPokeModelクラスでは,
        // 0 : 通常のモーション
        // 1 : バトル時の登場のOrigin移動モーション
        // 2～11 : eye01～eye03, mouth01～mouth03, loop01～loop04
        // となっている)
        int slotIndex = i + 1;    // 通常のモーションが0番のスロットを使っているため+1
        PokemonModel.AnimationBit animationBit =
					PokemonModel.AnimationBit.ANIMATION_BIT_JOINT |
					PokemonModel.AnimationBit.ANIMATION_BIT_MATERIAL |
					PokemonModel.AnimationBit.ANIMATION_BIT_VISIBILITY;

				m_PokemonModel.ChangeAnimation(m_pFaceMotionResourceRootNodes[i], slotIndex, animationBit);
				m_PokemonModel.SetAnimationFrame(frame, slotIndex, animationBit);
			}
		}

		/**
		 * @brief ジョイントコントローラの更新
		 */
		public void UpdateJointController()
		{
      m_PokemonModel.UpdateInterest();

      if (IsSpringEnable)
      {
        m_PokemonModel.UpdateSpring();
      }
		}

		/**
		 * @brief ジョイントのバネのリセット
		 */
		public void SpringResetRequest()
		{
			//m_JointController.SpringResetRequest();
		}

		/**
		 * @brief アニメーションリソースの追加
		 * @param string gfmotFilePath gfmot形式のファイルパス
		 */
		public void LoadGfmotResources(GfmotPathSet gfmotPathSet)
		{
      foreach (var gfmotFilePath in gfmotPathSet.GfmotFilePaths)
			{
        LoadGfmotResource(gfmotFilePath, m_motionNum);
				++m_motionNum;
			}
		}

    public void LoadGfmotResource(string gfmotFilePath, int loadMotionIndex)
    {
      m_pMotionResourceBinaryDatas[loadMotionIndex] = new GfBinaryMotData(File.ReadAllBytes(gfmotFilePath));
      m_pMotionResourceRootNodes[loadMotionIndex] = ResourceManager.CreateResourceNode(m_pMotionResourceBinaryDatas[loadMotionIndex]);

      m_PokemonModel.SetMotionResourceNode(loadMotionIndex, m_pMotionResourceRootNodes[loadMotionIndex]);

      // 目のアニメーション, 口のアニメーション, ループアニメーション(パッチールのブチ模様など)
      // のモーションリソースノードを抜き出して設定
      for (Int32 partsType = 0; partsType < (Int32)OverridePartsType.Enum.NumberOf; ++partsType)
      {
        var match = s_pmXXXX_YY.Match(System.IO.Path.GetFileNameWithoutExtension(gfmotFilePath));
        string pmXXXX_YY = match.ToString();
        if (System.IO.Path.GetFileNameWithoutExtension(gfmotFilePath) == pmXXXX_YY + "_" + OverridePartsTypeLabel[partsType])
        {
          m_pFaceMotionResourceRootNodes[partsType] = m_pMotionResourceRootNodes[loadMotionIndex];

          // GFModelViewerではスロット番号は通常のモーションが0番を使用しているため, 
          // 各種パーツの種類 + 1 となっている.
          uint slotIndex = (uint)(partsType + 1);

          m_PokemonModel.SetOverridePartsMotionIndex(
            partsType,            // パーツの種類(目, 口, ループ. 0から始まるインデックス)
            slotIndex,            // GFModelViewerで使用する目, 口, ループのスロット
            loadMotionIndex);     // 目, 口, ループで使用するモーションのインデックス
          break;
        }
      }
    }

		/**
		 * @brief アニメーションリソースの有効化
		 * @param index タイムライン上のインデックス
		 * @param motionIndex 有効化するPokemonModelに格納されているインデックス
		 * @param enabled 有効/無効
		 */
		public void SetMotion(Int32 index, Int32 motionIndex)
		{
			m_AnimationTimeLine.SetMotion(m_PokemonModel, index, motionIndex);
		}

    /**
     * @brief マテリアル名と対応したプリセット名の連想配列作成
     * @param gfmdlFilePath gfmdlのファイルパス
     * @return Dictionary<string, string> Key : マテリアル名, Value : プリセット名
     */
    private Dictionary<string, string> CreateMaterialPresetNameDictionary(string gfmdlFilePath)
    {
      if (!File.Exists(gfmdlFilePath))
      {
        return null;
      }

      Dictionary<string, string> materialPresetNameDictionary = new Dictionary<string, string>();

      try
      {
        XmlDocument xmlDocument = new XmlDocument();
        xmlDocument.Load(gfmdlFilePath);

        var presetNodes = xmlDocument.SelectNodes("/GfModel/Materials/Material/Combiner");

        foreach (XmlNode presetNode in presetNodes)
        {
          string presetName = presetNode.Attributes["PresetName"].Value;

          XmlNode parentNode = presetNode.ParentNode;
          string materialName = parentNode.Attributes["Name"].Value;

          materialPresetNameDictionary.Add(materialName, presetName);
        }
      }
      catch (Exception e)
      {
        MessageBox.Show(e.StackTrace);
        Environment.Exit(0);
      }


      return materialPresetNameDictionary;
    }

    /**
     * @brief 模様の設定
     * @param Byte markingFrame1 Loop01の固定フレーム
     * @param Byte markingFrame2 Loop02の固定フレーム
     * @param Byte markingFrame3 Loop03の固定フレーム
     * @param Byte markingFrame4 Loop04の固定フレーム
     */
    public void SetMarkingFrames(Byte markingFrame1, Byte markingFrame2, Byte markingFrame3, Byte markingFrame4)
    {
      m_PokemonModel.SetMarkingFrames(markingFrame1, markingFrame2, markingFrame3, markingFrame4);
    }

    /**
     * @brief マテリアルと対応した参照テーブルテクスチャの連想配列作成
     * @param gfmdlFilePath gfmdlのファイルパス
     * @return Dictionary<string, ReferenceTableTexturePaths> Key : マテリアル名, Value : 参照テクスチャのファイルパス群
     */
    private Dictionary<string, ReferenceTableTexturePaths> CreateReferenceTableTexturePathsDictionary(string gfmdlFilePath)
    {
      if (!File.Exists(gfmdlFilePath))
      {
        return null;
      }

      Dictionary<string, ReferenceTableTexturePaths> referenceTableTexturesDictionary = new Dictionary<string, ReferenceTableTexturePaths>();

      try
      {
        XmlDocument xmlDocument = new XmlDocument();
        xmlDocument.Load(gfmdlFilePath);

        // まず使用しているテクスチャとその番号を抽出
        Dictionary<Int32, string> textureFilePathDictionary = new Dictionary<int, string>();
        var texturePathNodes = xmlDocument.SelectNodes("/GfModel/TextureList/Texture");
        foreach (XmlNode texturePathNode in texturePathNodes)
        {
          Int32 key = Int32.Parse(texturePathNode.Attributes["No"].Value);
          string value = Path.GetFileName(texturePathNode.FirstChild.InnerText);
          textureFilePathDictionary.Add(key, value);
        }

        // 参照テーブルの項目を参照し, 各テクスチャの番号のパスをreferenceの連想配列に記述
        var materialNodes = xmlDocument.SelectNodes("/GfModel/Materials/Material");
        foreach (XmlNode materialNode in materialNodes)
        {
          string materialName = materialNode.Attributes["Name"].Value;
          var referenceTableTextures = new ReferenceTableTexturePaths();
          var textureSettingNodes = materialNode.SelectNodes("Textures/Texture");
          if (textureSettingNodes == null)
          {
            referenceTableTexturesDictionary.Add(materialName, referenceTableTextures);
            continue;
          }

          if (textureSettingNodes.Count == 0)
          {
            referenceTableTexturesDictionary.Add(materialName, referenceTableTextures);
            continue;
          }

          // スロット番号から参照テーブルの名前を取得し, パスの情報を書き込む
          foreach (XmlNode textureSettingNode in textureSettingNodes)
          {
            Int32 slotNo = Int32.Parse(textureSettingNode.Attributes["SlotNo"].Value);

            if (!ReferenceTableTexturePaths.ContainsSlotNo(slotNo))
            {// そもそも参照テーブルのインデックスでなければ無視
              continue;
            }

            var table = ReferenceTableTexturePaths.SelectReferenceTableFromSlotNo(slotNo);
            Int32 textureNo = -1;
            var textureAttributes = textureSettingNode.Attributes;
            if (textureAttributes["TextureNo"] != null)
            {
              textureNo = Int32.Parse(textureSettingNode.Attributes["TextureNo"].Value);
            }
            else if (textureAttributes["Usage"] != null)
            {// gfmdlのバージョン1.91以前は"Usage"という名前で登録されている, 後方互換を保つためのコード
              textureNo = Int32.Parse(textureSettingNode.Attributes["Usage"].Value);
            }

            if (textureNo < 0)
            {
              continue;
            }

            string filePath = textureFilePathDictionary[textureNo];
            referenceTableTextures[table] = filePath;
          }

          referenceTableTexturesDictionary.Add(materialName, referenceTableTextures);
        }
      }
      catch (Exception e)
      {
        MessageBox.Show(e.StackTrace);
        Environment.Exit(0);
      }

      return referenceTableTexturesDictionary;
    }
	}
}
