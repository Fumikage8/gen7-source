using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using GFFacePattern;

namespace FaceMaterialNameSerializer
{
  /**
   * @brief 表情のgfmotファイルのパーサクラス
   */
  public class FaceGfmotDocument
  {
    //!< @brief エラーメッセージ
    private string m_ErrorMessage;
    public string ErrorMessage
    {
      get { return m_ErrorMessage; }
    }

    //!< @brief ファイル名
    private readonly string m_FileName;
    public string FileName
    {
      get { return m_FileName; }
    }

    //!< @brief マテリアル名の配列
    private readonly string[] m_MaterialNames;
    public string[] MaterialNames
    {
      get { return m_MaterialNames; }
    }

    /**
     * @brief コンストラクタ
     * @param filePath 表情のgfmotのファイルパス
     * @note 受け取ったファイルパスのパースを行い, ファイル名 : パーツ名の連想配列を作成
     */
    public FaceGfmotDocument(string filePath)
    {
      m_ErrorMessage = "";
      m_FileName = "";
      m_MaterialNames = new string[0];

      if (!PokemonFacePattern.s_FacePatternRegexDictionary.Any(pair => pair.Value.IsMatch(filePath)))
      {// ファイル名が表情の命名規則に則っていなければ終了
        return;
      }

      // ファイル名を取得
      m_FileName = Path.GetFileName(filePath);

      XmlDocument xmlDocument = new XmlDocument();
      try
      {
        xmlDocument.Load(filePath);
      }
      catch (FileNotFoundException e)
      {
        Console.WriteLine("ファイル" + filePath + "が見つかりません.");
        Console.WriteLine(e.StackTrace);
        return;
      }
      catch (Exception e)
      {
        Console.WriteLine(e.StackTrace);
        return;
      }

      // UVが使われているかどうかを判別
      var uvAnimationNodes = xmlDocument.SelectNodes("/GfMotion/UvAnimation");
      if (uvAnimationNodes.Count == 0)
      {
        return;
      }

      // マテリアル名を抽出してマテリアル名を網羅した配列を作成
      // この中で「TextureSlot/TranslateU(もしくはTranslateV)/HermiteKey」を持っているものを探す. それが表情に使われているデータ
      string[] translateNodeNames = new string[] { "TranslateU", "TranslateV" };
      foreach (string translateNodeName in translateNodeNames)
      {
        XmlNodeList hermiteKeyNodes = xmlDocument.SelectNodes("/GfMotion/UvAnimation/Material/TextureSlot/" + translateNodeName + "/HermiteKey");
        string[] materialNames = hermiteKeyNodes.Cast<XmlNode>().Select(node => node.ParentNode.ParentNode.ParentNode.Attributes["Name"].Value).ToArray();
        m_MaterialNames = m_MaterialNames.Concat(materialNames).ToArray();
      }
    }
  }
}
