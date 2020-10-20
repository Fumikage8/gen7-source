using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GFTarga
{
  /// <summary>
  /// RunLength圧縮Targaデコーダ
  /// </summary>
  public class RunLengthTargaDecoder : ITargaDecoder
  {
    /// <summary>
    /// コンストラクタ
    /// </summary>
    public RunLengthTargaDecoder()
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
      // フォーマットに応じたピクセルデータを取得
      byte colorFormatByteSize = (byte)(targaHeader.bpp / 8);
      int length = targaHeader.width * targaHeader.height * colorFormatByteSize;
      byte[] pixelData = new byte[length];

      // まずiバイト目を読み込む
      int pixelDataIndex = 0;       // ピクセルデータのインデックス
      int readBytesIndex = 0;       // 読み取ったデータのインデックス
      while (pixelDataIndex < pixelData.Length)
      {
        //int controlPacket = fileStream.ReadByte();
        int controlPacket = rowData[readBytesIndex];
        ++readBytesIndex;
        int colorDataCount = (controlPacket & 0x7F) + 1;
        byte[] colorDataArray = new byte[colorFormatByteSize * colorDataCount];

        if ((controlPacket & 0x80) == 0)
        {// run-length圧縮されていないのでそのまま読み取る
          //fileStream.Read(colorDataArray, 0, colorDataArray.Length);
          Array.Copy(rowData, readBytesIndex, colorDataArray, 0, colorDataArray.Length);
          readBytesIndex += colorDataArray.Length;
          colorDataArray.CopyTo(pixelData, pixelDataIndex);
        }
        else
        {// run-length圧縮されているのでデコード
          byte[] colorData = new byte[colorFormatByteSize];
          //fileStream.Read(colorData, 0, colorData.Length);
          Array.Copy(rowData, readBytesIndex, colorData, 0, colorData.Length);
          readBytesIndex += colorData.Length;
          for (int i = 0; i < colorDataCount; ++i)
          {
            colorData.CopyTo(colorDataArray, i * colorData.Length);
          }
          colorDataArray.CopyTo(pixelData, pixelDataIndex);
        }
        pixelDataIndex += colorDataArray.Length;
      }

      var pixelDataArray = new PixelDataArray(pixelData, targaHeader.width, targaHeader.height, colorFormatByteSize);
      
      return pixelDataArray;
    }
  }
}
