using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;
using System.Diagnostics;
using System.Windows.Forms;

namespace ModelViewer
{
  /**
   * @brief Gfmdlのエラーチェッククラス
   * @note エラーチェックをしたいgfmdlを読み込み, その後エラーのあるマテリアルを数え上げる
   */
  public class GfmdlErrorChecker
  {
    /**
     * @brief テクスチャの種類
     */
    private enum TextureType
    {
      Texture,      //!< @brief 通常のテクスチャ
      ExtTexture    //!< @brief 参照テーブルなど, 本来の用途とは別のテクスチャ
    }

    /**
     * @brief テクスチャの種類を判別するラベル
     */
    private static Dictionary<string, TextureType> s_TextureTypeLabelDictionary = new Dictionary<string,TextureType>
    {
      {"Texture", TextureType.Texture},       //!< @brief 通常のテクスチャ
      {"ExtTexture", TextureType.ExtTexture}  //!< @brief 参照テーブルなど, 本来の用途とは別のテクスチャ
    };

    /**
     * @brief テクスチャのスロットの種類
     */
    private enum TextureSlotType
    {
      Texture0,
      Texture1,
      Texture2,
      LutTextureD0,
      LutTextureD1,
      LutTextureRR,
      LutTextureRG,
      LutTextureRB,
      LutTextureFR,
      HighLightTexture,
      OutLineColorTexture,
    }

    /**
     * @brief カラーテクスチャのスロットの種類
     */
    private static readonly TextureSlotType[] s_ColorTextureSlotTypes = new TextureSlotType[]
    {
      TextureSlotType.Texture0,
      TextureSlotType.Texture1,
      TextureSlotType.Texture2
    };

    /**
     * @brief カラーテクスチャのスロットのインデックス
     */
    private static readonly Dictionary<uint, TextureSlotType> s_ReferenceTableSlotNo = new Dictionary<uint, TextureSlotType>
    {
      { 0,  TextureSlotType.Texture0 },
      { 1,  TextureSlotType.Texture1 },
      { 2,  TextureSlotType.Texture2 },
      { 9,  TextureSlotType.LutTextureD0 },
      { 10, TextureSlotType.LutTextureD1 },
      { 11, TextureSlotType.LutTextureRR },
      { 12, TextureSlotType.LutTextureRG },
      { 13, TextureSlotType.LutTextureRB },
      { 14, TextureSlotType.LutTextureFR },
      { 8,  TextureSlotType.HighLightTexture },
      { 15, TextureSlotType.OutLineColorTexture },
    };

    /**
     * @brief マテリアルが持つテクスチャのリストクラス(key : TextureSlotType, value: 使用しているテクスチャのインデックス)
     */
    private class MaterialTextureList
    {
      //!< @brief 各種スロットに登録されているテクスチャNoの連想配列
      private Dictionary<TextureSlotType, uint> m_TextureNoDictionary;

      public MaterialTextureList()
      {
        m_TextureNoDictionary = new Dictionary<TextureSlotType, uint>();
        // 各スロットのTextureNoをuintの最大値で初期化
        foreach (TextureSlotType textureSlotType in Enum.GetValues(typeof(TextureSlotType)))
        {
          m_TextureNoDictionary[textureSlotType] = uint.MaxValue;
        }
      }

      public uint this[TextureSlotType slotType]
      {
        get { return m_TextureNoDictionary[slotType]; }
        set { m_TextureNoDictionary[slotType] = value; }
      }
    };

    //!< @brief pmNNNN_FFを判定する正規表現(NNNN : ポケモンNo, FF : フォルムNo)
    public static readonly System.Text.RegularExpressions.Regex s_pmNNNN_FF = new System.Text.RegularExpressions.Regex(@"pm\d\d\d\d_\d\d");

    //!< @brief マテリアルの上限
    private static uint s_MaterialUpperLimit = 16;

    //!< @brief テクスチャの上限(マテリアルの上限 x カラーテクスチャの枚数)
    private static uint s_TextureUpperLimit = s_MaterialUpperLimit * (uint)s_ColorTextureSlotTypes.Length;

    //!< @brief テクスチャのスロットの番号(key : TextureNo(int), value : TextureType)
    private Dictionary<uint, TextureType> m_TextureTypeDictionary;

    //!< @brief テクスチャの名前(key : TextureNo(int), value : TextureName(string))
    private Dictionary<uint, string> m_TextureNameDictionary;

    //!< @brief マテリアルが持つテクスチャ情報(key : materialName(string), value : MaterialTextureList)
    private Dictionary<string, MaterialTextureList> m_MaterialTextureDictionary;

    //!< @brief テクスチャ番号のラベル(バージョン1.92以降は"TextureNo", それ以前は"Usage"
    private string m_TextureNoLabel;

    //!< @brief ポケモンモデルかどうかの判定フラグ
    private bool m_IsPokemonModel;

    /**
     * @brief コンストラクタ
     * @param gfmdlFilePath gfmdlのファイルパス
     * @note gfmdlが持つマテリアルとテクスチャの情報をまとめる
     */
    public GfmdlErrorChecker(string gfmdlFilePath)
    {
      // 命名規則が"pmNNNN_FF"に則っているかをチェック
      m_IsPokemonModel = s_pmNNNN_FF.IsMatch(Path.GetFileNameWithoutExtension(gfmdlFilePath));
      if (!m_IsPokemonModel)
      {// 人物モデルなどには関係ないチェックのためここで弾く
        return;
      }

      m_TextureTypeDictionary = new Dictionary<uint, TextureType>();
      m_TextureNameDictionary = new Dictionary<uint, string>();
      m_MaterialTextureDictionary = new Dictionary<string, MaterialTextureList>();
      m_TextureNoLabel = "TextureNo";

      // gfmdlをパースしてテクスチャのリストを作成
      XmlDocument xmlDocument = new XmlDocument();
      try
      {
        xmlDocument.Load(gfmdlFilePath);

        // gfmdlのバージョンをチェック
        var gfmdlNode = xmlDocument.SelectSingleNode("/GfModel");
        decimal versionNo = decimal.Parse(gfmdlNode.Attributes["version"].Value);
        if (versionNo <= 1.91m)
        {// gfmdlのバージョンが1.91以前は"TextureNo"ではなく"Usage"のため
          m_TextureNoLabel = "Usage";
        }

        // 使用されているテクスチャの種類を判別する連想配列を作成
        var texturePathNodes = xmlDocument.SelectNodes("/GfModel/TextureList/Texture");
        foreach (XmlNode texturePathNode in texturePathNodes)
        {
          uint key;
          bool isSucceeded = uint.TryParse(texturePathNode.Attributes["No"].Value, out key);
          Debug.Assert(isSucceeded);
          string textureTypeLabel = texturePathNode.Attributes["Type"].Value;
          m_TextureTypeDictionary[key] = s_TextureTypeLabelDictionary[textureTypeLabel];
          m_TextureNameDictionary[key] = Path.GetFileName(texturePathNode.FirstChild.InnerText);
        }

        // マテリアルの項目を参照し, 使用しているテクスチャのインデックス
        var materialNodes = xmlDocument.SelectNodes("/GfModel/Materials/Material");
        foreach (XmlNode materialNode in materialNodes)
        {
          string materialName = materialNode.Attributes["Name"].Value;
          var textureSettingNodes = materialNode.SelectNodes("Textures/Texture");
          MaterialTextureList materialTextureList = new MaterialTextureList();
          foreach (XmlNode textureSettingNode in textureSettingNodes)
          {
            uint slotNo;
            uint textureNo;
            bool isSucceeded;
            isSucceeded = uint.TryParse(textureSettingNode.Attributes["SlotNo"].Value, out slotNo);
            Debug.Assert(isSucceeded);
            isSucceeded = uint.TryParse(textureSettingNode.Attributes[m_TextureNoLabel].Value, out textureNo);
            Debug.Assert(isSucceeded);
            var textureSlotType = s_ReferenceTableSlotNo[slotNo];
            materialTextureList[textureSlotType] = textureNo;
          }
          m_MaterialTextureDictionary[materialName] = materialTextureList;
        }
      }
      catch (Exception e)
      {
        MessageBox.Show(e.StackTrace);
        Environment.Exit(0);
      }
    }

    /**
     * @brief gfmdlの不正なテクスチャフォーマットのマテリアルを数え上げる
     * @return Tuple<string, string>[] フォーマットが間違っているマテリアルとテクスチャの組の配列
     */
    public Tuple<string, string>[] CountUpIncorrectTextureFormatMaterials()
    {
      List<Tuple<string, string>> incorrectTextureFormatMaterialList = new List<Tuple<string, string>>();

      if (!m_IsPokemonModel)
      {// ポケモンモデル以外であれば関係ないチェックなので「正しい」と判断して終了
        return incorrectTextureFormatMaterialList.ToArray();
      }

      // 出来上がったマテリアルの持つカラーテクスチャの中にExtTextureのものがないかチェック
      foreach (var materialTextureData in m_MaterialTextureDictionary)
      {
        var materialTextureList = materialTextureData.Value;
        foreach (TextureSlotType textureSlotType in s_ColorTextureSlotTypes)
        {
          uint usedTextureNo = materialTextureList[textureSlotType];

          if (usedTextureNo >= s_TextureUpperLimit)
          {// そもそもテクスチャが使用されていなければ次へ
            continue;
          }

          if (m_TextureTypeDictionary[usedTextureNo] != TextureType.ExtTexture)
          {// "ExtTexture"でなければ問題ないので次へ
            continue;
          }

          // カラーテクスチャに"ExtTexture"の項目があるのはルール違反.
          Tuple<string, string> incorrectMaterialTextureTuple = new Tuple<string, string>(materialTextureData.Key, m_TextureNameDictionary[usedTextureNo]);
          incorrectTextureFormatMaterialList.Add(incorrectMaterialTextureTuple);
        }
      }

      return incorrectTextureFormatMaterialList.ToArray();
    }
  }
}
