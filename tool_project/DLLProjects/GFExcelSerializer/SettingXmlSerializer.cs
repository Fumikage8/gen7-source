using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Xml;

namespace GFExcelSerializer
{
  /// <summary>
  /// pmNNNN_FF_setting.xmlファイルを読み取るクラス
  /// </summary>
  public class SettingXmlSerializer : IExcelXmlSerializer
  {
    /// <summary>
    /// XmlDocumentのプロパティ
    /// </summary>
    public XmlDocument XmlDocument { get; set; }

    /// <summary>
    /// Excelのセルに格納されているデータのgetter
    /// </summary>
    public string[,] Cell2d { get { return this.m_Cells; } }

    /// <summary>
    /// 展開後の数値以外のデータのgetter
    /// </summary>
    public Dictionary<string, string> NonNumericValueDictionary { get { return m_NonNumericValueDictionary; } }

    /// <summary>
    /// 展開後の数値データのgetter
    /// </summary>
    public Dictionary<string, decimal> NumericValueDictionary { get { return m_NumericValueDictionary; } }

    /// <summary>
    /// ポケモン名のgetter
    /// </summary>
    public string PokemonName { get { return m_PokemonName; } }

    /// <summary>
    /// フォルムNoのgetter
    /// </summary>
    public uint FormNo { get { return m_FormNo; } }

    /// <summary>
    /// デシリアライズ
    /// </summary>
    /// <param name="cells">Excelファイルのセル</param>
    public void DeserializeExcel(string[,] cells)
    {
      m_Cells = cells;
      m_NumericValueDictionary = new Dictionary<string, decimal>();

      // ポケモン名とフォルム番号を抽出
      m_PokemonName = (cells[2, 1] != null) ? cells[2, 1] : string.Empty; // ポケモン名は確定するまでは空欄のため対処
      m_FormNo = uint.Parse(cells[2, 3]);

      // 残りのフォームの設定項目を抽出
      var nonNumericValueDictionary = new Dictionary<string, string>();
      for (var i = 0; i < cells.GetLength(0); ++i)
      {
        string key = string.IsNullOrEmpty(cells[i, 0]) ? string.Empty : cells[i, 0];
        string strValue = string.IsNullOrEmpty(cells[i, 1]) ? string.Empty : cells[i, 1];
        decimal value = 0;

        bool succeeded = ConvertToDecimal(key, strValue, out value);
        if (!succeeded)
        {// 変換に失敗した場合は失敗したデータ群に登録
          nonNumericValueDictionary[key] = strValue;    // 頻繁にnull, 空文字列が来るのでインデクサで追加
          continue;
        }

        // 数値データはキーが重複してはならないのでAddで要素を追加(キーが重複していた場合例外が投げられる)
        m_NumericValueDictionary.Add(key, value);
      }

      // NonNumericValueDictionaryに紛れている不要なデータを除外
      var trimmedDict = nonNumericValueDictionary.Where(data => !string.IsNullOrEmpty(data.Key));    // keyが空文字列を除外
      trimmedDict = trimmedDict.Where(data => !string.IsNullOrEmpty(data.Value));                    // valueが空文字列を除外
      // 半角英数字以外を除外
      Regex alphanumericCharacterRegex = new Regex("^[a-zA-Z0-9]+$");
      trimmedDict = trimmedDict.Where(data => alphanumericCharacterRegex.IsMatch(data.Key));     // keyが英数字以外の要素を除外
      trimmedDict = trimmedDict.Where(data => alphanumericCharacterRegex.IsMatch(data.Value));   // valueが英数字以外の要素を除外

      // 除外したデータをDictionary<string, string>として保存
      m_NonNumericValueDictionary = trimmedDict.ToDictionary(data => data.Key, data => data.Value);
    }

    /// <summary>
    /// 文字列を数値に変換
    /// </summary>
    /// <param name="key">連想配列のキーとなる文字列</param>
    /// <param name="strValue">文字列型で格納された数値</param>
    /// <param name="value">変換後の数値(out)</param>
    /// <returns>bool 成功/失敗</returns>
    private bool ConvertToDecimal(string key, string strValue, out decimal value)
    {
      value = 0;

      if (string.IsNullOrEmpty(key) || string.IsNullOrEmpty(strValue))
      {
        return false;
      }

      double doubleValue = 0.0;
      bool succeeded = double.TryParse(strValue, out doubleValue);

      if (succeeded)
      {
        value = (decimal)doubleValue;
      }

      return succeeded;
    }

    /// <summary>
    /// Excelのセルに格納されているデータ
    /// </summary>
    private string[,] m_Cells = null;

    /// <summary>
    /// ポケモン名
    /// </summary>
    private string m_PokemonName;

    /// <summary>
    /// フォルム番号
    /// </summary>
    private uint m_FormNo;

    /// <summary>
    /// setting.xmlの項目名と設定された数値の連想配列
    /// </summary>
    private Dictionary<string, decimal> m_NumericValueDictionary = null;

    /// <summary>
    /// setting.xmlの項目名と設定された数値以外の連想配列
    /// </summary>
    private Dictionary<string, string> m_NonNumericValueDictionary = null;
  }
}
