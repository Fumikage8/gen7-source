using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using GFExcelSerializer;
using GFPokemonFile;

namespace PokemonExternalDataSerializer
{
  /**
   * @brief ポケモンの外部ファイルを一括でmsgpackでシリアライズするシリアライザ
   */
  class PokemonExternalDataSerializer
  {
    static int Main(string[] args)
    {
      if (args.Length == 0)
      {// コマンドライン引数が空なら終了
        Console.WriteLine("pmNNNN_FF_pokemonname の命名規則に該当するディレクトリを指定してください.");
        return 1;
      }

      if (!PokemonRegularExpression.DirectoryRegex.IsMatch(args[0]))
      {// 命名規則に則っていないディレクトリなら終了
        Console.WriteLine("pmNNNN_FF_pokemonname の命名規則に該当するディレクトリを指定してください. args[0] : " + args[0]);
        return 1;
      }

      // ディレクトリ直下のrare_setting.xmlとsetting.xmlを取得
      string[] settingXmlFilePaths = Directory.GetFiles(args[0], "*_setting.xml");
      string settingXmlFilePath = Array.Find(settingXmlFilePaths, path => PokemonRegularExpression.SettingXmlRegex.IsMatch(path));
      string rareSettingXmlFilePath = Array.Find(settingXmlFilePaths, path => PokemonRegularExpression.RareSettingXmlRegex.IsMatch(path));

      // 各種ファイルの読み取り処理は全てtry-catchで囲い, 例外をキャッチするようにする
      try
      {
        // "pmNNNN_FF_pokemonname"からPokemonDirectoryDataオブジェクトを作成
        var pokemonDirectoryNameSplitter = new PokemonDirectoryNameSplitter();
        var pokemonDirectoryData = pokemonDirectoryNameSplitter.SplitToPokemonDirectoryData(args[0]);

        // pmNNNN_FF_setting.xmlを読み込み, データを抽出
        SettingXmlSerializer settingXmlSerializer = ExcelXml.Deserialize<SettingXmlSerializer>(settingXmlFilePath);

        // pmNNNN_FF_rare_setting.xmlを読み込み, データを抽出
        RareSettingXmlSerializer rareSettingXmlSerializer = ExcelXml.Deserialize<RareSettingXmlSerializer>(rareSettingXmlFilePath);
      }
      catch (Exception e)
      {
        System.Diagnostics.Debug.WriteLine(e.StackTrace);
        Console.WriteLine(e.StackTrace);
      }
      return 0;
    }
  }
}
