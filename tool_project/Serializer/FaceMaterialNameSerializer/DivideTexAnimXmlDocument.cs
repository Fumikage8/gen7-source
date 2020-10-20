using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Text.RegularExpressions;
using GFFacePattern;

namespace FaceMaterialNameSerializer
{
  /**
   * @brief DivideTexAnim.xmlのパーサクラス
   */
  public class DivideTexAnimXmlDocument
  {
    //!< @brief eye01, eye02,...表情データに関連する正規表現
    private static readonly Dictionary<PokemonFacePattern.FacePattern, Regex> s_FacePatternRegexDictionary = new Dictionary<PokemonFacePattern.FacePattern, Regex>
    {
      { PokemonFacePattern.FacePattern.Eye, new Regex(@"pm\d\d\d\d_\d\d_eye0\d") },
      { PokemonFacePattern.FacePattern.Mouth, new Regex(@"pm\d\d\d\d_\d\d_mouth0\d") },
    };

    //!< @brief エラーメッセージ
    private string m_ErrorMessage;
    public string ErrorMessage
    {
      get { return m_ErrorMessage; }
    }

    //!< @brief ファイル名 : パーツ名の連想配列
    private readonly Dictionary<string, string[]> m_RemainPartsNameDictionary;

    /**
     * @brief コンストラクタ
     * @param filePath DivideTexAnim.xmlのファイルパス
     * @note 受け取ったファイルパスのパースを行い, ファイル名 : パーツ名の連想配列を作成
     */
    public DivideTexAnimXmlDocument(string filePath)
    {
      m_ErrorMessage = "";
      m_RemainPartsNameDictionary = new Dictionary<string, string[]>();

      XmlDocument xmlDocument = new XmlDocument();
      try
      {
        xmlDocument.Load(filePath);
      }
      catch (FileNotFoundException e)
      {
        m_ErrorMessage += "ファイル" + filePath + "が見つかりません." + Environment.NewLine;
        m_ErrorMessage += e.StackTrace + Environment.NewLine;
        return;
      }
      catch (Exception e)
      {
        m_ErrorMessage += e.StackTrace + Environment.NewLine;
        return;
      }

      XmlNodeList exceptionNodes = xmlDocument.SelectNodes("/root/EXCEPTION");

      foreach (XmlNode exceptionNode in exceptionNodes)
      {
        string key = exceptionNode.Attributes["_FileName"].Value;
        m_RemainPartsNameDictionary[key] = new string[0];
      }

      foreach (XmlNode exceptionNode in exceptionNodes)
      {
        string key = exceptionNode.Attributes["_FileName"].Value;
        string value = exceptionNode.Attributes["_Parts"].Value;
        m_RemainPartsNameDictionary[key] = m_RemainPartsNameDictionary[key].Concat(new string[] { value }).ToArray();
      }
    }

    /**
     * @brief フィルターを掛けた連想配列を作成
     * @param facePattern 表情のパターン(Eye, Mouth)
     * @return Dictionary<string, string[]> キーの部分一致後の連想配列
     */
    public Dictionary<string, string[]> CreateKeyFilteredDictionary(PokemonFacePattern.FacePattern facePattern)
    {
      var outputDictionary = new Dictionary<string, string[]>();
      foreach (var keyValuePair in m_RemainPartsNameDictionary)
      {
        if (!s_FacePatternRegexDictionary[facePattern].IsMatch(keyValuePair.Key))
        {// 指定した表情の命名規則に則っていなければ無視
          continue;
        }

        //if (!s_FacePatternRegexDictionary[keyFilterName].IsMatch(keyValuePair.Key))
        //{// 命名規則が一致していなければエラーメッセージを追記して飛ばす
        //  m_ErrorMessage += "ファイル : " + keyValuePair.Key + " の命名規則が一致していません" + Environment.NewLine;
        //  continue;
        //}
        outputDictionary[keyValuePair.Key] = keyValuePair.Value;
      }
      return outputDictionary;
    }
  }
}
