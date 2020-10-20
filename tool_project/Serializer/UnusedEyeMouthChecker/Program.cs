using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;
using System.Threading.Tasks;
using System.Xml;
using System.Runtime.InteropServices;
using GFPokemonFile;
using GFTarga;

namespace UnusedEyeMouthChecker
{
  class Program
  {
    static int Main(string[] args)
    {
      if (args.Length == 0)
      {
        Console.WriteLine("pmNNNN_FF_pokemonname の命名規則に則ったディレクトリをドラッグ&ドロップしてください.");
        return 1;
      }

      // 受け取ったディレクトリ以下に"exports"があるかを検索
      string exportPath = Path.Combine(args[0], "exports");
      if (!Directory.Exists(exportPath))
      {// 該当のディレクトリがなければ終了
        Console.WriteLine(exportPath + "is not found.");
        return 1;
      }

      // 共有設定のファイルがあるかどうかを確認. "ANIME_SHARE"の項目があったらそのリソースの確認の必要はないので終了
      // @todo : ext_data.txtの読み取り処理をGFPokemonFile.dllの中に含めるべき.
      string extDataTxtFilePath = Path.Combine(exportPath, "ext_data.txt");
      if (!File.Exists(extDataTxtFilePath))
      {// ファイルが見つからなかった場合は終了
        Console.WriteLine(extDataTxtFilePath + " is not found.");
        return 1;
      }
      string extDataDetail = "";
      using (StreamReader streamReader = new StreamReader(extDataTxtFilePath))
      {// ext_data.txtの中身を一括で読み取り
        extDataDetail = streamReader.ReadToEnd();
      }
      // 改行コードで分割
      string[] shareTypeStrings = Regex.Split(extDataDetail, Environment.NewLine);
      if (shareTypeStrings.Any(name => name == "ANIME_SHARE"))
      {// "ANIME_SHARE"の項目が存在していたら終了. 異常ではないので正常終了の0を返す.
        return 0;
      }

      // 表情のgfmotファイルを収集して, GfmotFaceDataを作成
      var gfmotFaceDataReader = new GfmotFaceDataReader();
      string[] gfmotFilePaths = System.IO.Directory.GetFiles(exportPath, "*.gfmot");
      var gfmotFaceDataDictionary = new Dictionary<string, GfmotFaceData>();
      foreach (string gfmotFilePath in gfmotFilePaths)
      {
        var faceData = gfmotFaceDataReader.CreateFaceDataFromGfmot(gfmotFilePath);
        gfmotFaceDataDictionary.Add(gfmotFilePath, faceData);
      }
      gfmotFaceDataDictionary = gfmotFaceDataDictionary.Where(data => data.Value != null).ToDictionary(data => data.Key, data => data.Value);

      // ジョイントとテクスチャ制御の表情データのファイルパスを表示
      foreach (var faceData in gfmotFaceDataDictionary)
      {
        if (faceData.Value.HasJoint && faceData.Value.HasUv)
        {
          Console.WriteLine(faceData.Key);
        }
      }

      return 0;
    }
  }
}
