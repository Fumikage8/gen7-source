using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GFTarga
{
  /// <summary>
  /// 無圧縮Targaデコーダ
  /// </summary>
  public class UmcompressedTargaDecoder : ITargaDecoder
  {
    /// <summary>
    /// コンストラクタ
    /// </summary>
    public UmcompressedTargaDecoder()
    {

    }

    /// <summary>
    /// Targaファイルのデコード
    /// </summary>
    /// <param name="targaHeader">tgaヘッダ情報</param>
    /// <param name="rowData">tgaに含まれるデコード対象のデータ</param>
    /// <returns>PixelDataArray デコード済みのPixelDataArrayオブジェクト</returns>
    public PixelDataArray Decode(TargaHeader targaHeader, byte[] rowData)
    {
      byte colorFormatByteSize = (byte)(targaHeader.bpp / 8);
      int length = targaHeader.width * targaHeader.height * colorFormatByteSize;
      byte[] pixelData = new byte[length];
      Array.Copy(rowData, 0, pixelData, 0, pixelData.Length);

      var pixelDataArray = new PixelDataArray(pixelData, targaHeader.width, targaHeader.height, colorFormatByteSize);

      return pixelDataArray;
    }
  }
}
