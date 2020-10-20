using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Text.RegularExpressions;
using GFFacePattern;
using GFPokemonFile;

namespace FaceMaterialNameSerializer
{
  class FaceMaterialNameSerializer
  {
    //!< @brief 2016/02/25時点のシリーズのディレクトリ(あくまでディレクトリの判定用, 厳密なチェックはしないので, 今後のシリーズが増えても修正は必要ないようにしておく)
    private static readonly string[] s_SeriesNames = new string[] { "rg", "gs", "rs", "dp", "bw", "kujira", "sango", "niji" };

    //!< @brief ポケモンのディレクトリを見分ける正規表現
    private static readonly Regex s_PokemonDirectoryRegex = new Regex(@"pm\d\d\d\d_\d\d");

    static int Main(string[] args)
    {
      string errorMessage = "";
      if (args.Length != 1)
      {
        Console.WriteLine("コマンドライン引数にeye, もしくはmouthを指定してください.");
        return 1;
      }

      if (!PokemonFacePattern.s_FacePartsNameDictionary.Any(pair => pair.Key == args[0]))
      {// 引数が間違っていたら終了
        Console.WriteLine("コマンドライン引数にeye, もしくはmouthを指定してください.");
        return 1;
      }

      // コンバートする表情のタイプを決定
      PokemonFacePattern.FacePattern facePattern = PokemonFacePattern.s_FacePartsNameDictionary[args[0]];

      string pokemonDataPath = Directory.GetCurrentDirectory();
#if DEBUG
//      pokemonDataPath = "C:\\mynote\\201603\\";
      pokemonDataPath = "R:\\home\\niji\\pokemon\\data\\";
#endif
      string[] seriesPaths = Directory.GetDirectories(pokemonDataPath);

      // rg, gsなど, 各種シリーズのディレクトリがあるかを判定
      string[] seriesDirectoryNames = Array.ConvertAll(seriesPaths, path => path = Path.GetFileName(path));
      string[] intersectionNames = s_SeriesNames.Intersect(seriesDirectoryNames).ToArray();
      if (intersectionNames.Length == 0)
      {
        Console.WriteLine("シリーズのポケモンモデルのリソースを管理しているディレクトリ(rg, gsなど)で実行してください.");
        return 1;
      }

      // シリーズの中のディレクトリを全て取得
      var seriesDirectoryCollector = new SeriesDirectoryCollector(pokemonDataPath);

      // ポケモンのディレクトリから表情のモーションのgfmotを収集する
      string[] facePatternGfmotFilePaths = seriesDirectoryCollector.CollectFilePathsWithRegex(PokemonFacePattern.s_FacePatternRegexDictionary[facePattern], "exports");

      // 表情のモーション名(pmNNNN_FF_eye0*など)をキーとした連想配列を作成. これを元にDivideTexAnimに記述されているモーション名を残す.
      Dictionary<string, string[]> allFaceGfmotFileDictionary = new Dictionary<string, string[]>();     // 全てのモーション
      foreach (string facePatternGfmotFilePath in facePatternGfmotFilePaths)
      {
        string key = Path.GetFileNameWithoutExtension(facePatternGfmotFilePath);
        allFaceGfmotFileDictionary[key] = new string[0];
      }

      // ポケモンのディレクトリから例外設定のファイルを収集する
      string[] divideTexAnimFilePaths = seriesDirectoryCollector.CollectFilePathsWithFileName("DivideTexAnim.xml", "exports");
      // DivideTexAnim.xmlから「残しておくマテリアル名」を抽出する
      Dictionary<string, string[]> remainFaceGfmotFileDictionary = new Dictionary<string, string[]>();   // DivideTexAnimに記載されている残すモーション
      foreach (string divideTexAnimFilePath in divideTexAnimFilePaths)
      {
        var divideTexAnimXmlDocument = new DivideTexAnimXmlDocument(divideTexAnimFilePath);
        errorMessage += divideTexAnimXmlDocument.ErrorMessage;
        var filteredDictionary = divideTexAnimXmlDocument.CreateKeyFilteredDictionary(facePattern);   // "eye" or "mouth"でフィルターを掛ける
        remainFaceGfmotFileDictionary = remainFaceGfmotFileDictionary.Concat(filteredDictionary).ToDictionary(x => x.Key, x => x.Value);
      }

      // 収集したgfmotのファイルからUVアニメーションで使用されているマテリアル名を抽出
      Dictionary <string, string[]> facePatternMaterialNameDictionary = new Dictionary<string,string[]>();
      foreach (string gfmotFilePath in facePatternGfmotFilePaths)
      {
        FaceGfmotDocument faceGfmotDocument = new FaceGfmotDocument(gfmotFilePath);
        string key = Path.GetFileNameWithoutExtension(faceGfmotDocument.FileName);
        facePatternMaterialNameDictionary[key] = faceGfmotDocument.MaterialNames;
      }

      // 収集したgfmotのマテリアル名から, DivideTexAnimに記載されているものだけを残す
      foreach (var keyValuePair in remainFaceGfmotFileDictionary)
      {
        facePatternMaterialNameDictionary[keyValuePair.Key] = facePatternMaterialNameDictionary[keyValuePair.Key].Intersect(keyValuePair.Value).ToArray();
      }

      // 残ったキーを配列として保存
      string[] facePartsNames = new string[0];
      foreach (var keyValuePair in facePatternMaterialNameDictionary)
      {
        facePartsNames = facePartsNames.Concat(keyValuePair.Value).ToArray();
      }
      facePartsNames = facePartsNames.Distinct().ToArray();

      // 最終的に残ったパーツ名をソートしてシリアライズ
      Array.Sort(facePartsNames);

      return 0;
    }
  }
}
