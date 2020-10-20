using System;
using System.Xml;

namespace GFExcelSerializer
{
  /// <summary>
  /// Excelシリアライズ用インタフェース
  /// どのようにシリアライズしたいかを実装先のクラスで記述し, Serialize/Deserializeメソッドに渡す
  /// </summary>
  public interface IExcelXmlSerializer
  {
    /// <summary>
    /// XmlDocumentのプロパティ
    /// </summary>
    XmlDocument XmlDocument { get; set; }

    /// <summary>
    /// 各種cell
    /// </summary>
    string[,] Cell2d { get; }

    /// <summary>
    /// Excelファイルのデシリアライズ処理. どのようにデシリアライズされるかを記述.
    /// </summary>
    /// <param name="cells">Excelシートのセル</param>
    void DeserializeExcel(string[,] cells);
  }
}
