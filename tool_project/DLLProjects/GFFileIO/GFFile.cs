using System;
using System.IO;

namespace GFFileIO
{
  /// <summary>
  /// バイナリファイルの読み書きを扱うクラス
  /// </summary>
  public class BinaryFile
  {
    /// <summary>
    /// ファイルを読み取った内容をbyte[]にして返す
    /// </summary>
    /// <param name="filePath">ファイルパス</param>
    /// <returns>byte[] 読み取ったデータのバイト配列</returns>
    public static byte[] Read(string filePath)
    {
      using (var fileStream = File.OpenRead(filePath))
      {
        BinaryReader binaryReader = new BinaryReader(fileStream);
        byte[] readBytes = BinaryFile.ReadAllBytes(binaryReader);
        return readBytes;
      }
    }

    /// <summary>
    /// バイト配列をファイルに出力
    /// </summary>
    /// <param name="filePath">出力するファイルパス</param>
    /// <param name="sourceBytes">書き込み対象となるバイト配列</param>
    public static void Write(string filePath, byte[] sourceBytes)
    {
      using (var fileStream = File.OpenWrite(filePath))
      {
        // シリアライズしたデータをファイルに書き込んで出力
        BinaryWriter binaryWriter = new BinaryWriter(fileStream);
        binaryWriter.Write(sourceBytes, 0, sourceBytes.Length);
      }
    }

    /// <summary>
    /// BinaryReaderからバイナリデータを読み取る
    /// </summary>
    /// <param name="binaryReader">バイナリリーダオブジェクト</param>
    /// <returns>byte[] 読み取ったデータのバイト配列</returns>
    private static byte[] ReadAllBytes(BinaryReader binaryReader)
    {
      const int bufferSize = 4096;
      using (var memoryStream = new MemoryStream())
      {
        byte[] buffer = new byte[bufferSize];
        int count;
        while ((count = binaryReader.Read(buffer, 0, buffer.Length)) != 0)
        {
          memoryStream.Write(buffer, 0, count);
        }
        return memoryStream.ToArray();
      }
    }
  }
}
