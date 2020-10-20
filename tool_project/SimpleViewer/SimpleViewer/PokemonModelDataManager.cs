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
using gfl2.clr.math;
using gfl2.clr.renderingengine;
using gfl2.clr.animation;
using gfl2.clr.renderingengine.scenegraph.resource;
using gfl2.clr.renderingengine.scenegraph.instance;
using gfl2.clr.poke_3d_lib;

namespace SimpleViewer
{
  public class PokemonModelDataManager : IDisposable
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
        {"bb_chest", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_CHEST} },
        {"bb_badge", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_BADGE} },
        {"all_head_parts", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_HAT} },
        {"all_chest_parts", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_RIBBON, gfl2.clr.renderingengine.PokedecoType.Enum.BB_CHEST, gfl2.clr.renderingengine.PokedecoType.Enum.BB_BADGE} },
        {"all_face_parts", new gfl2.clr.renderingengine.PokedecoType.Enum[]{gfl2.clr.renderingengine.PokedecoType.Enum.BB_GLASSES} },
      };

    //!< @brief ポケデコのバウンディングボックスの種類のインデックス
    public string[] PokedecoLocatorBoundingBoxNames { get; set; }

    public PokemonSize.Enum Size { get; set; }
    public bool IsInnerCylinderVisible { get; set; }
    public bool IsOuterCylinderVisible { get; set; }
    public bool IsModelAABBVisible { get; set; }
    public Vector InnerCylinderColor { get; set; }
    public Vector OuterCylinderColor { get; set; }
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

    private string m_InputDirectory;
    public string InputDirectory
    {
      get { return m_InputDirectory; }
    }

    public string GfmdlFilePath
    {
      get { return m_GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].gfmdlFilePath; }
    }
    public Int32 GfmdlBinarySize
    {
      get
      {
        FileInfo fileInfo = new FileInfo(m_GfMdlResourcePathSetDictionary[ConvertExtensionType.WinOpenGL].gfbmdlFilePath);
        return (Int32)fileInfo.Length;
      }
    }
    private ResourceNode m_motionConfigResourceRootNode;
    ResourceNode m_pModelResourceRootNode;
    GfBinaryMdlData m_pModelResourceBinaryData;

    private ResourceNode m_pShadowModelResourceRootNode;
    private GfBinaryMdlData m_pShadowModelResourceBinaryData;

    private string m_McnfFilePath;
    public string McnfFilePath
    {
      get { return m_McnfFilePath; }
      set { m_McnfFilePath = value; }
    }
    private string m_BlendListFilePath;
    public string BlendListPath
    {
      get { return m_BlendListFilePath; }
      set { m_BlendListFilePath = value; }
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
      set { m_Scale = value; }
    }
    private Vector m_Position;
    public Vector Position
    {
      get { return m_Position; }
      set { m_Position = value; }
    }
    private Vector m_WalkPosition;
    public Vector WalkPosition
    {
      get { return m_WalkPosition; }
      set { m_WalkPosition = value; }
    }
    public bool WalkEnabled { get; set; }
    public float WalkSpeedOriginal { get; set; }
    public bool m_ResetLoopFlag = false;
    private Vector m_EulerRotation;	//!< @brief オイラー角(度数法なので注意)
    public Vector EulerRotation
    {
      get { return m_EulerRotation; }
      set { m_EulerRotation = value; }
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
      set
      {
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

    public Vector InterestPosition { get; set; }			//!< @brief 振り向き時の注視点

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
     * @brief IDモデルの作成
     * @param motionNum モーションの総数
     * @param idTextureDirectory IDテクスチャのパス
     * @return PokemonModel IDモデル
     */
    public PokemonModel CreateIdModel(Int32 motionNum, string idTexturesDirectory)
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
      idModel.SetVisible(false);

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
     * @param gfmdlFilePath gfbmdl形式のファイルパス
     */
    public PokemonModelDataManager(string gfbmdlFile, string shaderDirectory, string textureDirectory)
    {
      m_PokemonModelInfo = new gfl2.clr.poke_3d_lib.PokemonModel.PokemonModelInfo();

      m_FrameResolution = 1.0m;
      m_LoopEnable = false;
      m_IsRare = false;

      m_Scale = new Vector(1.0f, 1.0f, 1.0f);
      m_Position = new Vector(0.0f, 0.0f, 0.0f);
      m_WalkPosition = new Vector(0.0f, 0.0f, 0.0f);
      m_EulerRotation = new Vector(0.0f, 0.0f, 0.0f);

      // get paths to resource directories
      string binaryFileDirectory = System.IO.Path.GetDirectoryName(gfbmdlFile);

      // create resource node from gfbmdl file
      m_pModelResourceBinaryData = new GfBinaryMdlData(File.ReadAllBytes(gfbmdlFile));
      m_pModelResourceRootNode = ResourceManager.CreateResourceNode(m_pModelResourceBinaryData);

      // get shader file list
      string[] shaderResourcePaths = System.IO.Directory.GetFiles(shaderDirectory, "*.wingl.*", SearchOption.TopDirectoryOnly);
      m_pModelResourceRootNode.AddShaderResources(shaderResourcePaths);

      // get texture file list
      string[] textureResourcePaths = System.IO.Directory.GetFiles(textureDirectory, "*.wingl.*", SearchOption.TopDirectoryOnly);
      m_pModelResourceRootNode.AddTextureResources(textureResourcePaths);

      // PokemonModelクラスにリソースを渡す
      m_PokemonModel = new PokemonModel();
      m_PokemonModel.Create(m_pModelResourceRootNode, m_motionConfigResourceRootNode, null);
      m_PokemonModel.CreateColorShaderTextures();
      m_PokemonModel.SetVisible(true);

      // ポケビューア上では開始インデックスが1なのでそれに合わせる
      m_EyeIndex = 1;
      m_MouthIndex = 1;
      m_LoopIndex = 1;

      LightRotationX = 0.0f;
      LightRotationY = 0.0f;

      // 視線制御の設定
      InterestPosition = new Vector(0.0f, 0.0f, 250.0f);
      IsInterestPositionControllable = false;

      IsInnerCylinderVisible = false;
      IsOuterCylinderVisible = false;
      IsModelAABBVisible = false;
      InnerCylinderColor = new gfl2.clr.math.Vector(1.0f, 1.0f, 0.5f, 0.5f);
      OuterCylinderColor = new gfl2.clr.math.Vector(1.0f, 1.0f, 0.5f, 0.5f);
      ModelAABBColor = new gfl2.clr.math.Vector(1.0f, 1.0f, 0.5f, 0.5f);

      DefaultModelAABB = null;

      DefaultCmHeight = 100.0f;
      DefaultAdjustHeight = 100.0f;

      m_PokemonModel.SetHeight(DefaultCmHeight, DefaultAdjustHeight);

      m_Scale.x = DefaultScale;
      m_Scale.y = DefaultScale;
      m_Scale.z = DefaultScale;

      // UI上で表示するノードの階層構造を作成
      m_PokemonModel.CreateModelNameTreeNode();
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
  }

}
