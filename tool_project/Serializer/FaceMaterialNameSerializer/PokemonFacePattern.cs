using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace GFFacePattern
{
  public class PokemonFacePattern
  {
    //!< @brief 表情のパーツの種類
    public enum FacePattern
    {
      Eye,
      Mouth
    }

    //!< @brief 実行時の引数と表情のパーツの種類を判別する連想配列
    public static readonly Dictionary<string, FacePattern> s_FacePartsNameDictionary = new Dictionary<string, FacePattern>
    {
      { "eye", FacePattern.Eye },
      { "mouth", FacePattern.Mouth }
    };

    //!< @brief eye01, eye02,...表情データに関連する正規表現
    public static readonly Dictionary<FacePattern, Regex> s_FacePatternRegexDictionary = new Dictionary<FacePattern, Regex>
    {
      { FacePattern.Eye, new Regex(@"pm\d\d\d\d_\d\d_eye0\d.gfmot") },
      { FacePattern.Mouth, new Regex(@"pm\d\d\d\d_\d\d_mouth0\d.gfmot") },
    };
  }
}
