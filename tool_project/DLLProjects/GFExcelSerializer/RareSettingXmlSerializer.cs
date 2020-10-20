using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Serialization;

namespace GFExcelSerializer
{
  /// <summary>
  /// レアカラーのコンスタントカラーのオフセット情報
  /// </summary>
  public class RareColorConstantOffsetInfo
  {
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="materialName">マテリアル名</param>
    /// <param name="constantColorIndex">オフセットを加算するコンスタントカラーのインデックス</param>
    /// <param name="red">オフセットの赤成分</param>
    /// <param name="green">オフセットの緑成分</param>
    /// <param name="blue">オフセットの青成分</param>
    /// <param name="alpha">オフセットのアルファ成分</param>
    public RareColorConstantOffsetInfo(string materialName, uint constantColorIndex, decimal red, decimal green, decimal blue, decimal alpha)
    {
      m_MaterialName = materialName;
      m_ConstantColorIndex = constantColorIndex;
      m_Red = red;
      m_Green = green;
      m_Blue = blue;
      m_Alpha = alpha;
    }

    /// <summary>
    /// マテリアル名
    /// </summary>
    public string MaterialName { get { return m_MaterialName; } }
    /// <summary>
    /// オフセットを加算するコンスタントカラーのインデックス
    /// </summary>
    public uint ConstantColorIndex { get { return m_ConstantColorIndex; } }
    /// <summary>
    /// オフセットの赤成分
    /// </summary>
    public decimal Red { get { return m_Red; } }
    /// <summary>
    /// オフセットの緑成分
    /// </summary>
    public decimal Green { get { return m_Green; } }
    /// <summary>
    /// オフセットの青成分
    /// </summary>
    public decimal Blue { get { return m_Blue; } }
    /// <summary>
    /// オフセットのアルファ成分
    /// </summary>
    public decimal Alpha { get { return m_Alpha; } }

    /// <summary>
    /// マテリアル名
    /// </summary>
    private readonly string m_MaterialName;
    /// <summary>
    /// オフセットを加算するコンスタントカラーのインデックス
    /// </summary>
    private readonly uint m_ConstantColorIndex;
    /// <summary>
    /// オフセットの赤成分
    /// </summary>
    private readonly decimal m_Red;
    /// <summary>
    /// オフセットの緑成分
    /// </summary>
    private readonly decimal m_Green;
    /// <summary>
    /// オフセットの青成分
    /// </summary>
    private readonly decimal m_Blue;
    /// <summary>
    /// オフセットのアルファ成分
    /// </summary>
    private readonly decimal m_Alpha;
  }

  /// <summary>
  /// pmNNNN_FF_rare_setting.xmlファイルを読み取るクラス
  /// </summary>
  public class RareSettingXmlSerializer : IExcelXmlSerializer
  {
    /// <summary>
    /// XmlDocumentのプロパティ
    /// </summary>
    public XmlDocument XmlDocument { get; set; }

    /// <summary>
    /// Excelのセルに格納されているデータのgetter
    /// </summary>
    public string[,] Cell2d { get { return this.cells; } }

    /// <summary>
    /// レアカラーのオフセット情報を格納した配列へのアクセサ
    /// </summary>
    public RareColorConstantOffsetInfo[] RareColorConstantOffsetInfos
    {
      get { return m_RareColorConstantOffsetInfos; }
    }

    /// <summary>
    /// rare_setting.xmlのデシリアライズ処理. どのようにデシリアライズされるかを記述.
    /// </summary>
    /// <param name="cells">Excelシートのセル</param>
    public void DeserializeExcel(string[,] cells)
    {
      this.cells = cells;
      var rareColorConstantOffsetInfoList = new List<RareColorConstantOffsetInfo>();

      for (var i = StartLineIndex; i < cells.GetLength(0); ++i)
      {
        if (!IsRowAvailable(cells, i))
        {// 行が有効でなければ無視して次へ
          continue;
        }

        string materialName = cells[i, 1];
        uint constantColorIndex;
        decimal red;
        decimal green;
        decimal blue;
        decimal alpha;

        // 数値は「変換できないもの」が紛れている可能性があるのでTryParseで逐一確認を行う
        if (!uint.TryParse(cells[i, 2], NumberStyles.Number | NumberStyles.AllowExponent, CultureInfo.CurrentCulture, out constantColorIndex))
        {
          throw new FormatException("rare_setting.xmlのコンスタントカラーのインデックスの項目に整数以外のものが含まれています");
        }

        if (!decimal.TryParse(cells[i, 3], NumberStyles.Number | NumberStyles.AllowExponent, CultureInfo.CurrentCulture, out red))
        {
          throw new FormatException("rare_setting.xmlのコンスタントカラーの赤の項目に数値以外のものが含まれています");
        }

        if (!decimal.TryParse(cells[i, 4], NumberStyles.Number | NumberStyles.AllowExponent, CultureInfo.CurrentCulture, out green))
        {
          throw new FormatException("rare_setting.xmlのコンスタントカラーの緑の項目に数値以外のものが含まれています");
        }

        if (!decimal.TryParse(cells[i, 5], NumberStyles.Number | NumberStyles.AllowExponent, CultureInfo.CurrentCulture, out blue))
        {
          throw new FormatException("rare_setting.xmlのコンスタントカラーの青の項目に数値以外のものが含まれています");
        }

        if (!decimal.TryParse(cells[i, 6], NumberStyles.Number | NumberStyles.AllowExponent, CultureInfo.CurrentCulture, out alpha))
        {
          throw new FormatException("rare_setting.xmlのコンスタントカラーのアルファの項目に数値以外のものが含まれています");
        }

        RareColorConstantOffsetInfo info = new RareColorConstantOffsetInfo(materialName, constantColorIndex, red, green, blue, alpha);
        rareColorConstantOffsetInfoList.Add(info);
      }

      // リストを配列に変換して保存
      m_RareColorConstantOffsetInfos = rareColorConstantOffsetInfoList.ToArray();
    }

    /// <summary>
    /// 読み取りの必要がある行かどうかの確認
    /// </summary>
    /// <param name="cells">読み取り対象のセルの配列</param>
    /// <param name="row">行番号</param>
    /// <returns>bool 読み取りの必要あり/なし</returns>
    private bool IsRowAvailable(string[,] cells, uint row)
    {
      // 要素を全て取り出す
      string[] rowValues = new string[LineValueNum];
      for (var i = 0; i < LineValueNum; ++i)
      {
        rowValues[i] = cells[row, i + 1];
      }

      // nullを全て空文字列に置き換える
      rowValues = Array.ConvertAll(rowValues, value => value = string.IsNullOrEmpty(value) ? "" : value);

      // 全角の文字が含まれていたら全て空文字列に置換(全角文字で格納されているデータは読み取りの必要がない)
      rowValues = Array.ConvertAll(rowValues, value => value = IsOneByteString(value) ? value : "");
      
      if (rowValues.All(value => string.IsNullOrEmpty(value)))
      {// 全て空なら未使用なのでfalse
        return false;
      }

      // 部分的に空の部分がある場合は不具合なので例外を投げる
      if (rowValues.Any(value => string.IsNullOrEmpty(value)))
      {
        throw new NullReferenceException("rare_setting.xmlの項目に空の項目が含まれています。");
      }

      // 定数カラーがひとつでも欠けていたらfalse
      for (var i = 1; i <= LineValueNum; ++i)
      {
        if (string.IsNullOrEmpty(cells[row, i]))
        {
          return false;
        }
      }

      return true;
    }

    /// <summary>
    /// 1バイト文字で構成された文字列であるか判定
    /// </summary>
    /// <param name="str">判定する文字列</param>
    /// <returns>bool 1バイト文字のみ/1バイト文字以外のものが含まれている</returns>
    private bool IsOneByteString(string str)
    {
      // SJISに変換した後, 元の文字列とバイト数が同じかどうかをチェック.
      byte[] byte_data = System.Text.Encoding.GetEncoding("Shift_JIS").GetBytes(str);
      return (byte_data.Length == str.Length);
    }

    /// <summary>
    /// Excelのセルに格納されているデータ
    /// </summary>
    private string[,] cells = null;

    /// <summary>
    /// 読み取り開始する行数
    /// </summary>
    private static readonly uint StartLineIndex = 6;

    /// <summary>
    /// 行から読み取る要素数
    /// </summary>
    private static readonly uint LineValueNum = 6;

    /// <summary>
    /// レアカラーのコンスタントカラーのオフセット情報
    /// </summary>
    private RareColorConstantOffsetInfo[] m_RareColorConstantOffsetInfos;
  }
}
