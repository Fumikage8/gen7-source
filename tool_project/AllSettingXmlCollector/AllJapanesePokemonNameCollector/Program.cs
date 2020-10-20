using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.IO;
using GFExcelSerializer;
using GFMsgPack;
using GFPokemonFile;

namespace AllSettingXmlCollector
{
  class Program
  {
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
      var settingXmlDictionary = new Dictionary<uint, SettingXmlSerializer>();
      foreach (string settingXmlFilePath in allSettingXmlFilePaths)
      {// ディレクトリ名から必要な情報を抽出し, シリアライズしたデータと1:1対応を作る
        var pokemonDirectoryData = pokemonDirectorySplitter.SplitToPokemonDirectoryData(settingXmlFilePath);
        settingXmlDictionary[pokemonDirectoryData.UniqueNo] = ExcelXml.Deserialize<SettingXmlSerializer>(settingXmlFilePath);
      }

      // UniqueNoと日本語名の文字列をmsgpackでシリアライズ
      var japaneseNameDictionary = new Dictionary<uint, string>();
      foreach (var settingXml in settingXmlDictionary)
      {
        japaneseNameDictionary[settingXml.Key] = settingXml.Value.PokemonName;
      }
      byte[] msgpackSerializedData = GFMsgPackFunction.ConvertObjectToMessagePack<Dictionary<uint, string>>(japaneseNameDictionary);

      // シリアライズ結果をファイルに保存
      GFFileIO.BinaryFile.Write("AllPokemonJapaneseNameDictionary.msgpack", msgpackSerializedData);

      return 0;
    }
  }
}
