using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GFPokemonFile
{
  /// <summary>
  /// ポケモンを一意に識別する番号
  /// </summary>
  public class PokemonUniqueNo
  {
    /// <summary>
    /// コンストラクタ(開発番号, フォルム番号指定)
    /// </summary>
    /// <param name="developmentNo">開発番号</param>
    /// <param name="formNo">フォルム番号</param>
    public PokemonUniqueNo(ushort developmentNo, ushort formNo)
    {
      m_DevelopmentNo = developmentNo;
      m_FormNo = formNo;
    }

    /// <summary>
    /// コンストラクタ(ユニーク番号指定. 開発番号上位16bit, フォルム番号下位16bit)
    /// </summary>
    /// <param name="uniqueNo">ユニーク番号(上位16bit : 開発番号, 下位16bit : フォルム番号)</param>
    public PokemonUniqueNo(uint uniqueNo)
    {
      m_DevelopmentNo = (ushort)((uniqueNo & DevelopmentNoMask) >> UniqueNoShift);
      m_FormNo = (ushort)(uniqueNo & FormNoMask);
    }

    /// <summary>
    /// コンストラクタ(ユニーク番号文字列指定. "pm0025_00"などのフォーマットの文字列を含む文字列)
    /// </summary>
    /// <param name="uniqueNoString">"pm0025_00"などのフォーマットの文字列を含む文字列</param>
    public PokemonUniqueNo(string uniqueNoString)
    {
      if (!PokemonRegularExpression.UniqueNoRegex.IsMatch(uniqueNoString))
      {// "pmNNNN_FF"の表記に則っていない文字列なら例外を投げる
        throw new ArgumentException("pmNNNN_FF の命名規則に則った文字列ではありません.", "pmNNNN_FF");
      }

      // 一致している箇所を抽出
      string pmNNNN_FF = PokemonRegularExpression.UniqueNoRegex.Match(uniqueNoString).Value;

      // 先頭のpmを外し, _で分割, 前方がDevelopmentNo, 後方がFormNoなのでParseして設定.
      string NNNN_FF = pmNNNN_FF.Replace("pm", "");
      string[] uniqueNoStringArray = NNNN_FF.Split('_');
      m_DevelopmentNo = ushort.Parse(uniqueNoStringArray[0]);
      m_FormNo = ushort.Parse(uniqueNoStringArray[1]);
    }

    /// <summary>
    /// ユニーク番号(上位16bit : 開発番号, 下位16bit : フォルム番号)
    /// </summary>
    public uint UniqueNo
    {
      get
      {
        uint uniqueNo = (uint)m_DevelopmentNo;
        uniqueNo <<= UniqueNoShift;
        uniqueNo |= (uint)m_FormNo;
        return uniqueNo;
      }
    }

    /// <summary>
    /// 開発番号
    /// </summary>
    public ushort DevelopmentNo { get { return DevelopmentNo; } }
    /// <summary>
    /// フォルム番号
    /// </summary>
    public ushort FormNo { get { return FormNo; } }

    /// <summary>
    /// UniqueNoを操作する際のシフト数
    /// </summary>
    private static int UniqueNoShift = 16;

    /// <summary>
    /// FormNoのマスク(下位16bit)
    /// </summary>
    private static uint FormNoMask = 0x0000FFFF;

    /// <summary>
    /// DevelopmentNoのマスク(上位16bit)
    /// </summary>
    private static uint DevelopmentNoMask = 0xFFFF0000;

    /// <summary>
    /// 開発番号
    /// </summary>
    private readonly ushort m_DevelopmentNo;

    /// <summary>
    /// フォルム番号
    /// </summary>
    private readonly ushort m_FormNo;
  }
}
