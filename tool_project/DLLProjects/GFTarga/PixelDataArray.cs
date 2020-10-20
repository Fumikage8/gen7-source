using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace GFTarga
{
  /// <summary>
  /// ピクセルデータを扱う配列
  /// </summary>
  public class PixelDataArray
  {
    /// <summary>
    /// 画像の横幅
    /// </summary>
    private readonly ushort m_Width;

    /// <summary>
    /// 画像の縦幅
    /// </summary>
    private readonly ushort m_Height;

    /// <summary>
    /// 画像の1ピクセルあたりのバイト数
    /// </summary>
    private readonly byte m_BytePerPixel;

    /// <summary>
    /// 画像の生データ配列
    /// </summary>
    private readonly byte[] m_RowPixelDataArray;

    /// <summary>
    /// 生データ配列のgetter
    /// </summary>
    public byte[] RowPixelDataArray
    {
      get { return m_RowPixelDataArray; }
    }

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="sourceDataArray">画像の元データ</param>
    /// <param name="width">画像の横幅</param>
    /// <param name="height">画像の縦幅</param>
    /// <param name="bytePerPixel">1ピクセルあたりのバイト数</param>
    public PixelDataArray(byte[] sourceDataArray, ushort width, ushort height, byte bytePerPixel)
    {
      m_Width = width;
      m_Height = height;
      m_BytePerPixel = bytePerPixel;
      m_RowPixelDataArray = new byte[m_Width * m_Height * m_BytePerPixel];

      // 元データと今回メンバで持つ配列が同じかどうかを確認してコピー
      Debug.Assert(m_RowPixelDataArray.Length == sourceDataArray.Length);
      sourceDataArray.CopyTo(m_RowPixelDataArray, 0);
    }

    /// <summary>
    /// 画像データを上下反転させる
    /// </summary>
    /// <returns>bool 成功/失敗</returns>
    public bool InverseVertically()
    {
      for (int y = 0; y < m_Height / 2; ++y)
      {
        for (int x = 0; x < m_Width; ++x)
        {
          for (byte bpp = 0; bpp < m_BytePerPixel; ++bpp)
          {
            int index0 = y * m_Width * m_BytePerPixel + x * m_BytePerPixel + bpp;
            int index1 = (m_Height - y - 1) * m_Width * m_BytePerPixel + x * m_BytePerPixel + bpp;
            byte temp = m_RowPixelDataArray[index0];
            m_RowPixelDataArray[index0] = m_RowPixelDataArray[index1];
            m_RowPixelDataArray[index1] = temp;
          }
        }
      }

      return true;
    }

    /// <summary>
    /// 同一色で塗られているかを判定する
    /// </summary>
    /// <param key="xOrigin">x座標の基準点</param>
    /// <param key="yOrigin">y座標の基準点</param>
    /// <param key="width">横幅</param>
    /// <param key="height">縦幅</param>
    public bool IsFilledSameColor(int xOrigin, int yOrigin, int width, int height)
    {
      // 比較対象となる1ピクセル目のデータを取得
      byte[] colorData = new byte[m_BytePerPixel];
      for (int bpp = 0; bpp < m_BytePerPixel; ++bpp)
      {
        colorData[bpp] = m_RowPixelDataArray[(yOrigin * m_Width + xOrigin) * m_BytePerPixel + bpp];
      }

      for (int y = 0; y < height; ++y)
      {
        for (int x = 0; x < width; ++x)
        {
          for (byte bpp = 0; bpp < m_BytePerPixel; ++bpp)
          {
            int index = ((y + yOrigin) * m_Width + (x + xOrigin)) * m_BytePerPixel + bpp;
            if (m_RowPixelDataArray[index] != colorData[bpp])
            {
              return false;
            }
          }
        }
      }
      return true;
    }
  }
}
