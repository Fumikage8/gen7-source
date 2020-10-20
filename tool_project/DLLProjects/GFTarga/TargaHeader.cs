using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace GFTarga
{
  /// <summary>
  /// カラーフォーマット
  /// </summary>
  public enum ColorFormat
  {
    /// <summary>
    /// X8R8G8B8 各色に 8 ビットを使用する 32 ビットの RGB ピクセル形式。
    /// </summary>
    X8R8G8B8,

    /// <summary>
    /// A8R8G8B8 アルファ付きの、チャネルごとに 8 ビットを使用する 32 ビットの ARGB ピクセル形式。 
    /// </summary>
    A8R8G8B8,
  }

  /// <summary>
  /// 画素データの格納タイプ
  /// </summary>
  public enum ImageType
  {
    /// <summary>
    /// 無圧縮RGB
    /// </summary>
    UncompressedTrueColor = 0x02,
    /// <summary>
    /// run-length圧縮RGB
    /// </summary>
    RunLengthTrueColor = 0x0A,
    /// <summary>
    /// 不定(存在しないフォーマット)
    /// </summary>
    Unknown = 0xFF
  }

  /// <summary>
  /// tgaのデータを読み取る構造体
  /// </summary>
  [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Ansi)]
  public class TargaHeader
  {
    /// <summary>
    /// アトリビュートの情報を識別するマスク
    /// </summary>
    [Flags]
    private enum AttributeMask
    {
      /// <summary>
      /// アトリビュートのデータの格納部分のマスク
      /// </summary>
      NumberOfAttributeBits = 0x0F,
      /// <summary>
      /// U方向の向き(右から左 or 左から右)
      /// </summary>
      TurnOverU = 0x10,
      /// <summary>
      /// U方向の向き(上から下 or 下から上)
      /// </summary>
      TurnOverV = 0x20,
      /// <summary>
      /// インタリーブフラグ
      /// </summary>
      InterreavingFlags = 0xC0
    }

    /// <summary>
    /// tgaのbppとフォーマットを関連付ける連想配列
    /// </summary>
    private static readonly Dictionary<byte, ColorFormat> s_TagBppColorFormatDictionary = new Dictionary<byte, ColorFormat>
    {
      { 24, ColorFormat.X8R8G8B8 },
      { 32, ColorFormat.A8R8G8B8 }
    };

    /// <summary>
    /// 0: 識別子文字列長
    /// </summary>
    public byte idLength;
    /// <summary>
    /// 1: カラーマップタイプ
    /// </summary>
    public byte colorMapType;
    /// <summary>
    /// 2: 0x02 = 無圧縮RGB / 0x0A = 圧縮RGB
    /// </summary>
    public byte imageType;
    /// <summary>
    ///  3: カラーマップタイプオリジン
    /// </summary>
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
    public byte[] colorMapOrigin;
    /// <summary>
    /// 5: カラーマップタイプ長
    /// </summary>
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
    public byte[] colorMapLength;
    /// <summary>
    /// 7: パレットのビット幅
    /// </summary>
    public byte colorMapEntrySize;
    /// <summary>
    /// 8: x原点
    /// </summary>
    public ushort xOrigin;
    /// <summary>
    /// 10: y原点
    /// </summary>
    public ushort yOrigin;
    /// <summary>
    /// 12: 画像の横幅
    /// </summary>
    public ushort width;
    /// <summary>
    /// 14: 画像の縦幅
    /// </summary>
    public ushort height;
    /// <summary>
    /// 16: ビット深度
    /// </summary>
    public byte bpp;
    /// <summary>
    /// 17: アトリビュート(画像の格納方向など)
    /// </summary>
    public byte attribute;

    /// <summary>
    /// UVのU方向が反転しているかの判定取得
    /// </summary>
    public bool TurnOverU
    {
      get { return ((attribute & (byte)AttributeMask.TurnOverU) != 0) ? true : false; }
    }

    /// <summary>
    /// UVのV方向が反転しているかの判定取得
    /// </summary>
    public bool TurnOverV
    {
      get { return ((attribute & (byte)AttributeMask.TurnOverV) != 0) ? true : false; }
    }

    /// <summary>
    /// 画像のピクセル格納タイプ
    /// </summary>
    public ImageType ImageType
    {
      get
      {
        var result = (GFTarga.ImageType)imageType;
        if (!Enum.IsDefined(typeof(GFTarga.ImageType), result))
        {// 存在しないフォーマットであればUnknownを返す
          return GFTarga.ImageType.Unknown;
        }
        return result;
      }
    }

    /// <summary>
    /// 画像フォーマット
    /// </summary>
    public ColorFormat ColorFormat
    {
      get { return s_TagBppColorFormatDictionary[bpp]; }
    }
  }

  /// <summary>
  /// NW4Cの識別子
  /// </summary>
  [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Ansi)]
  public class NW4CIdField
  {
    /// <summary>
    /// " NW4C_Tga Ver1.0" （残りの 1 バイトは 0x00）
    /// </summary>
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
    public char[] identifier;
    /// <summary>
    /// 実データまでのオフセット
    /// </summary>
    public int offset;
  }
}
