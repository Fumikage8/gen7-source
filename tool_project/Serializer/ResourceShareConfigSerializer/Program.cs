using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using MsgPack.Serialization;
using GFMsgPack;
using GFPokemonFile;

namespace ResourceShareConfigSerializer
{
  class Program
  {
    /// <summary>
    /// 終了コード
    /// </summary>
    enum ReturnCode
    {
      SUCCESS,            // 成功
      NO_ARGUMENTS,       // 引数が指定されていない
      NO_EXTDATATXT,      // ext_data.txtファイルが見つからない
    }

    /// <summary>
    /// リソースの共有設定の種類(ビットフィールド)
    /// </summary>
    [Flags]
    enum ResourceShareType
    {
      NONE = 0,                   // 無し
      ANIMATION_SHARE = 1 << 0,   // アニメーション共有
      MODEL_SHARE = 1 << 1,       // モデル共有
      TEXTURE_SHARE = 1 << 2      // テクスチャ共有
    }

    /// <summary>
    /// ext_data.txtのファイル名
    /// </summary>
    private static readonly string ExtDataFileName = "ext_data.txt";

    /// <summary>
    /// ext_data.txtに記述されているテキストとリソースの共有設定を対応させた連想配列
    /// </summary>
    private static readonly Dictionary<string, ResourceShareType> ResourceShareTypeDictionary = new Dictionary<string, ResourceShareType>
    {
      {"", ResourceShareType.NONE},
      {"ANIME_SHARE", ResourceShareType.ANIMATION_SHARE},
      {"MODEL_SHARE", ResourceShareType.MODEL_SHARE},
      {"TEXTURE_SHARE", ResourceShareType.TEXTURE_SHARE},
    };

    /// <summary>
    /// ext_data.txtからリソースの共有設定の読み取り
    /// </summary>
    /// <param name="extDataFilePath">ext_data.txtのファイルパス</param>
    /// <returns>ResourceShareType リソースの共有設定(ビットフィールド)</returns>
    private static ResourceShareType ReadResourceShareTypeFromExtData(string extDataFilePath)
    {
      ResourceShareType resourceSharetype = ResourceShareType.NONE;

      // ext_data.txtを読み取り専用で開く
      using (var fileStream = new FileStream(extDataFilePath, FileMode.Open, FileAccess.Read))
      {
        using (var streamReader = new StreamReader(fileStream))
        {
          while (!streamReader.EndOfStream)
          {
            string resourceShareTypeName = streamReader.ReadLine();
            resourceSharetype |= ResourceShareTypeDictionary[resourceShareTypeName];
          }
        }
      }

      return resourceSharetype;
    }

    /// <summary>
    /// ext_data.txtを読み取って共有設定をシリアライズ
    /// </summary>
    /// <param name="args">コマンドライン引数. 第1引数にシリーズのディレクトリを指定</param>
    /// <returns>ReturnCode. 正常終了/異常終了の判別</returns>
    static int Main(string[] args)
    {
      if (args.Length == 0)
      {
        Console.WriteLine("各シリーズのポケモンのリソースを格納したディレクトリ(rg, gsなど)が存在するパスを指定してください.");
        return (int)ReturnCode.NO_ARGUMENTS;
      }

      // 全シリーズのディレクトリから, ext_data.txtを収集
      var seriesDirectoryCollector = new GFPokemonFile.SeriesDirectoryCollector(args[0]);
      var extDataFilePaths = seriesDirectoryCollector.CollectFilePathsWithFileName(ExtDataFileName, "exports");
      if (extDataFilePaths.Length == 0)
      {// ext_data.txtファイルが存在しない
        Console.WriteLine("指定したディレクトリ : " + args[0] + "に, ファイル " + ExtDataFileName + "が存在しません.");
        return (int)ReturnCode.NO_EXTDATATXT;
      }

      var pokemonDirectoryNameSplitter = new PokemonDirectoryNameSplitter();

      // ext_data.txtを読み込み, リソースの共有があるかどうかの確認
      var resourceShareTypeDictionary = new Dictionary<PokemonDirectoryData, ResourceShareType>();
      foreach (var extDataFilePath in extDataFilePaths)
      {
        var pokemonDirectoryData = pokemonDirectoryNameSplitter.SplitToPokemonDirectoryData(extDataFilePath);
        ResourceShareType resourceShareType = ReadResourceShareTypeFromExtData(extDataFilePath);
        resourceShareTypeDictionary.Add(pokemonDirectoryData, resourceShareType);
      }

      return (int)ReturnCode.SUCCESS;
    }
  }
}
