using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices;

namespace GFTarga
{
  /// <summary>
  /// Targaファイルを読み取ったデータ
  /// </summary>
  public class TargaData
  {
    /// <summary>
    /// 読み取ったTargaのヘッダ
    /// </summary>
    private readonly TargaHeader m_TargaHeader;

    /// <summary>
    /// 読み取ったTargaの識別情報
    /// </summary>
    private readonly byte[] m_TargaIdentifier;

    /// <summary>
    /// 読み取ったピクセルデータ
    /// </summary>
    private readonly PixelDataArray m_PixelDataArray;

    /// <summary>
    /// TargaHeaderのプロパティ
    /// </summary>
    public TargaHeader TargaHeader
    {
      get { return m_TargaHeader; }
    }

    /// <summary>
    /// PixelDataArrayのプロパティ
    /// </summary>
    public PixelDataArray PixelDataArray
    {
      get { return m_PixelDataArray; }
    }

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="targaHeader">Targaのヘッダ情報</param>
    /// <param name="targaIdentifier">Targaの識別情報</param>
    /// <param name="pixelDataArray">格納されているピクセル情報</param>
    public TargaData(TargaHeader targaHeader, byte[] targaIdentifier, PixelDataArray pixelDataArray)
    {
      m_TargaHeader = targaHeader;
      m_TargaIdentifier = targaIdentifier;
      m_PixelDataArray = pixelDataArray;
    }
  }
}
