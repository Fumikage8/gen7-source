using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GFPokemonFile
{
  /// <summary>
  /// ポケモンのディレクトリから抽出したデータ
  /// </summary>
  public struct PokemonDirectoryData
  {
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="developmentNo">開発番号</param>
    /// <param name="formNo">フォルム番号</param>
    /// <param name="developmentName">開発名</param>
    /// <param name="seriesName">シリーズ名</param>
    public PokemonDirectoryData(ushort developmentNo, ushort formNo, string developmentName, string seriesName)
    {
      m_PokemonUniqueNo = new PokemonUniqueNo(developmentNo, formNo);
      m_DevelopmentName = developmentName;
      m_SeriesName = seriesName;
    }

    /// <summary>
    /// ユニーク番号(上位16bit : 開発番号, 下位16bit : フォルム番号)
    /// </summary>
    public uint UniqueNo { get { return m_PokemonUniqueNo.UniqueNo; } }

    /// <summary>
    /// 開発番号
    /// </summary>
    public ushort DevelopmentNo { get { return m_PokemonUniqueNo.DevelopmentNo; } }

    /// <summary>
    /// フォルム番号
    /// </summary>
    public ushort FormNo { get { return m_PokemonUniqueNo.FormNo; } }

    /// <summary>
    /// 開発名
    /// </summary>
    public string DevelopmentName { get { return m_DevelopmentName; } }

    /// <summary>
    /// シリーズ名
    /// </summary>
    public string SeriesName { get { return m_SeriesName; } }

    /// <summary>
    /// ポケモンを一意に識別するユニーク番号
    /// </summary>
    private PokemonUniqueNo m_PokemonUniqueNo;

    /// <summary>
    /// 開発名
    /// </summary>
    private string m_DevelopmentName;
    /// <summary>
    /// シリーズ名
    /// </summary>
    private string m_SeriesName;
  }
}
