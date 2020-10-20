using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.IO;
using GFExcelSerializer;
using GFPokemonFile;

namespace AllSettingXmlCollector
{
  class Program
  {
    //!< @brief サイズのカテゴリ分類
    //!< @todo サイズカテゴリが増えた場合を考慮すること(別DLLに定義を設けるなど)
    private static readonly string[] SizeCategories = new string[] { "S", "M", "L" };

    static int Main(string[] args)
    {
      if (args.Length != 1)
      {
        Console.WriteLine("各シリーズのディレクトリ(rg, gsなど)が存在するパスを指定してください.");
        return 1;
      }

      // 全てのディレクトリのsetting.xmlを収集
      var seriesDirectoryCollector = new GFPokemonFile.SeriesDirectoryCollector(args[0]);
      string[] allSettingXmlFilePaths = seriesDirectoryCollector.CollectFilePathsWithRegex(PokemonRegularExpression.SettingXmlRegex, "");

      if (allSettingXmlFilePaths.Length == 0)
      {
        Console.WriteLine("指定パス : " + args[0] + "以下にpmFFFF_NN_setting.xmlが1つも見つかりませんでした.");
        return 1;
      }

      // ディレクトリ名から必要な情報を抽出
      var pokemonDirectorySplitter = new PokemonDirectoryNameSplitter();

      // PokemonDirectoryData と SettingXmlSerializerで連想配列を作成
      var settingXmlDictionary = new Dictionary<PokemonDirectoryData, SettingXmlSerializer>();
      foreach (string pokemonDirectory in allSettingXmlFilePaths)
      {// ディレクトリ名から必要な情報を抽出し, シリアライズしたデータと1:1対応を作る
        var pokemonDirectoryData = pokemonDirectorySplitter.SplitToPokemonDirectoryData(pokemonDirectory);
        settingXmlDictionary[pokemonDirectoryData] = ExcelXml.Deserialize<SettingXmlSerializer>(pokemonDirectory);
      }

      using (var fileStream = new FileStream("AllSettingXmlBreakDownData.csv", FileMode.OpenOrCreate, FileAccess.Write))
      {
        using (var streamWriter = new StreamWriter(fileStream, System.Text.Encoding.GetEncoding("shift_jis")))
        {
          // 各列にDictionaryから抽出したデータとsetting.xmlの内訳の項目名を記述
          string[] pokemonDirectoryDataLabels = new string[] { "開発番号", "開発名", "シリーズ名" };
          Array.ForEach(pokemonDirectoryDataLabels, data => streamWriter.Write(data + ","));

          // setting.xmlの項目を記述
          string[] settingXmlTopDataLabels = new string[] { "ポケモン名", "フォルム番号" };
          Array.ForEach(settingXmlTopDataLabels, data => streamWriter.Write(data + ","));

          var settingXmlFirstNonNumericValues = settingXmlDictionary.First().Value.NonNumericValueDictionary;
          string[] settingXmlNonNumericKeys = settingXmlFirstNonNumericValues.Keys.ToArray();
          Array.ForEach(settingXmlNonNumericKeys, data => streamWriter.Write(data + ","));

          var settingXmlFirstNumericValues = settingXmlDictionary.First().Value.NumericValueDictionary;
          string[] settingXmlNumericKeys = settingXmlFirstNumericValues.Keys.ToArray();
          Array.ForEach(settingXmlNumericKeys, data => streamWriter.Write(data + ","));
          streamWriter.WriteLine();

          // setting.xml内のデータを記述
          foreach (var keyValuePair in settingXmlDictionary)
          {
            string[] pokemonDirectoryData = new string[] { keyValuePair.Key.DevelopmentNo.ToString(), keyValuePair.Key.DevelopmentName, keyValuePair.Key.SeriesName };
            Array.ForEach(pokemonDirectoryData, data => streamWriter.Write(data + ","));
            string[] settingXmlTopData = new string[] { keyValuePair.Value.PokemonName.ToString(), keyValuePair.Value.FormNo.ToString() };
            Array.ForEach(settingXmlTopData, data => streamWriter.Write(data + ","));

            // 数値以外のデータを全て出力
            string[] nonNumericValues = keyValuePair.Value.NonNumericValueDictionary.Values.ToArray();
            Array.ForEach(nonNumericValues, data => streamWriter.Write(data.ToString() + ","));

            // 数値データを全て出力
            decimal[] numericValues = keyValuePair.Value.NumericValueDictionary.Values.ToArray();
            Array.ForEach(numericValues, data => streamWriter.Write(data.ToString() + ","));
            streamWriter.WriteLine();
          }
        }
      }

      return 0;
    }
  }
}
