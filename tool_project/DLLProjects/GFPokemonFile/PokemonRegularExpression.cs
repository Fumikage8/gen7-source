using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace GFPokemonFile
{
  /// <summary>
  /// ポケモンのファイル・ディレクトリ用正規表現クラス
  /// </summary>
  public class PokemonRegularExpression
  {
    /// <summary>
    /// ポケモンのユニーク番号用正規表現("pm0025_00", "pm0006_51"など)
    /// </summary>
    public static readonly Regex UniqueNoRegex = new Regex(@"pm\d\d\d\d_\d\d");

    /// <summary>
    /// ポケモンのディレクトリ用正規表現("pm0025_00_pikachu", "pm0474_00_porygon-z"など)
    /// </summary>
    public static readonly Regex DirectoryRegex = new Regex(@"pm\d\d\d\d_\d\d_\w+[-]*\w+$");

    ///<summary>
    /// setting.xmlのディレクトリ用正規表現("pm0025_00_setting.xml"など)
    ///</summary>
    public static readonly Regex SettingXmlRegex = new Regex(@"pm\d\d\d\d_\d\d_setting.xml");

    ///<summary>
    /// rare_setting.xmlのディレクトリ用正規表現("pm0025_00_rare_setting.xml"など)
    ///</summary>
    public static readonly Regex RareSettingXmlRegex = new Regex(@"pm\d\d\d\d_\d\d_rare_setting.xml");
  }
}
