using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GFTarga
{
  /// <summary>
  /// Targaデコーダインタフェース
  /// </summary>
  public interface ITargaDecoder
  {
    /// <summary>
    /// Targaデコーダインタフェース
    /// </summary>
    /// <param name="targaHeader">tgaヘッダ情報</param>
    /// <param name="rowData">tgaに含まれるデコード対象のデータ</param>
    /// <returns>PixelDataArray デコード済みのPixelDataArrayオブジェクト</returns>
    PixelDataArray Decode(TargaHeader targaHeader, byte[] rowData);
  }
}
