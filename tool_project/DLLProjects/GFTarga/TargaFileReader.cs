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
  /// Targaファイル読み取りクラス
  /// </summary>
  /// <remarks>NW4CTarga, 通常のTargaどちらにも対応</remarks>
  public class TargaFileReader
  {
    /// <summary>
    /// Targaのフォーマットに合わせたデコーダの連想配列
    /// </summary>
    private readonly Dictionary<ImageType, ITargaDecoder> m_TargaDecoderDictionary;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    public TargaFileReader()
    {
      // 各種フォーマットのデコーダを登録
      m_TargaDecoderDictionary = new Dictionary<ImageType, ITargaDecoder>();
      m_TargaDecoderDictionary[ImageType.UncompressedTrueColor] = new UmcompressedTargaDecoder();
      m_TargaDecoderDictionary[ImageType.RunLengthTrueColor] = new RunLengthTargaDecoder();
    }

    /// <summary>
    /// tgaファイルからTargaDataを生成
    /// </summary>
    /// <param name="filePath">tgaファイルのパス</param>
    /// <returns>TargaData ファイルから読み取ったTargaの画素データ</returns>
    public TargaData CreateTargaDataFromFile(string filePath)
    {
      byte[] remainingData;           // targaの上記以外の残りのデータを格納するバッファ
      TargaHeader targaHeader;        // targaのヘッダ情報
      byte[] targaIdentifier;         // targaの識別情報
      PixelDataArray pixelDataArray;  // ピクセルデータ

      using (var fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read))
      {
        byte[] targaHeaderBuffer;       // targaのヘッダ部分の領域バッファ

        // TargaHeaderの読み取り
        int targaHeaderSize = Marshal.SizeOf(typeof(TargaHeader));
        targaHeaderBuffer = new byte[targaHeaderSize];
        fileStream.Read(targaHeaderBuffer, 0, targaHeaderSize);
        GCHandle gcHandle = GCHandle.Alloc(targaHeaderBuffer, GCHandleType.Pinned);
        targaHeader = (TargaHeader)Marshal.PtrToStructure(gcHandle.AddrOfPinnedObject(), typeof(TargaHeader));

        // 識別子文字列長ぶんだけバッファを取得して読み取り
        targaIdentifier = new byte[targaHeader.idLength];
        fileStream.Read(targaIdentifier, 0, targaIdentifier.Length);

        // 現在位置からファイルの終端までを読み取る
        int remainingDataLength = (int)fileStream.Length - (targaHeaderSize + targaHeader.idLength);
        remainingData = new byte[remainingDataLength];
        fileStream.Read(remainingData, 0, remainingDataLength);
      }

      pixelDataArray = m_TargaDecoderDictionary[targaHeader.ImageType].Decode(targaHeader, remainingData);

      var targaData = new TargaData(targaHeader, targaIdentifier, pixelDataArray);
      return targaData;
    }
  }
}
