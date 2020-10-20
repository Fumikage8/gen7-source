using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace AnimationConverter
{
  class ConvGfmot
  {
    public string ErrorMessage { get; set; }
    public string ErrorStackTrance { get; set; }

    // 単体コンバート
    public ReturnCode Conv(string inputPath, string outputPath, int optionCompLevel, bool isPokeMode)
    {
      // 入力ファイルが無ければ無視
      if (inputPath == null)
      {
        return ReturnCode.Error_InvalidArgument;
      }
      if (!File.Exists(inputPath))
      {
        return ReturnCode.Error_NotFoundInputPath;
      }

      // 出力ファイルが無ければ無視
      if (outputPath == null)
      {
        return ReturnCode.Error_InvalidArgument;
      }

      // デシリアライズ
      var gfMotion = Common.DeserializeXml<Gfmot.GfMotion>(inputPath);

      // デシリアライズに失敗したか？
      if (gfMotion == null)
      {
        return ReturnCode.Error_FaildDeserialize;
      }

      // 対応するバージョンか？
      bool isSupport = false;
      foreach (var ver in Def.SupportVersionList)
      {
        if (gfMotion.version == ver)
        {
          isSupport = true;
          break;
        }
      }
      if (!isSupport)
      {
        ErrorMessage = "file ver : " + gfMotion.version;
        ErrorMessage += Environment.NewLine;
        ErrorMessage += Environment.NewLine;
        ErrorMessage += "--- supportedVersinList ---";
        ErrorMessage += Environment.NewLine;

        foreach (var ver in Def.SupportVersionList)
        {
          ErrorMessage += ver;
          ErrorMessage += Environment.NewLine;
        }

        return ReturnCode.Error_UnsupportedVersion;
      }

      // フレームサイズが0か？
      if (gfMotion.FrameSize == 0)
      {
        ErrorMessage = "FrameSize is 0 !!";
        return ReturnCode.Error_FaildExport;
      }

      // 圧縮率の指定ファイル読み込み
      var compLevel = Mcnf.CompSetting.Levels.DEFAULT;

      var fileName = Path.GetFileNameWithoutExtension(inputPath);
      // オプションで圧縮率が指定されていたらそちらを使う
      if (optionCompLevel >= 0)
      {
        compLevel = (Mcnf.CompSetting.Levels)optionCompLevel;
      }
      else
      {
        // 同じディレクトリにEachDae.xmlがあるはず
        var dirPath = Path.GetDirectoryName(inputPath);
        var eachDaePath = Path.Combine(dirPath, "EachDae.xml");
        Mcnf.EachDae eachDae = null;
        if (File.Exists(eachDaePath))
        {
          // 読み込み
          eachDae = Mcnf.EachDae.Deserialize(eachDaePath);

          compLevel = eachDae.GetLevel(fileName);
        }
      }

      // 同じディレクトリに.gfmdlがあるはず
      Gfmot.GfModel gfModel = null;
      if (!(inputPath.Contains("_mouth") || inputPath.Contains("_eye") || inputPath.Contains("_loop"))) // 特殊なモーションはノータッチ
      {
        var dirPath = Path.GetDirectoryName(inputPath);
        var gfmdlPathList = Directory.GetFiles(dirPath, "*.gfmdl");
        if (!Common.IsNullOrEmpty(gfmdlPathList))
        {
          // モデルの出力パスが同じかチェック
          foreach (var gfmdlPath in gfmdlPathList)
          {
            var tmpMdl = Common.DeserializeXml<Gfmot.GfModel>(gfmdlPath);

            if (gfMotion.Source == null)
            {
              continue;
            }

            if (gfMotion.Skelton == null)
            {
              continue;
            }

            if (tmpMdl.Source == null)
            {
              continue;
            }

            if (tmpMdl.Skelton == null)
            {
              continue;
            }

            if (tmpMdl.Source.Path != gfMotion.Source.Path)
            {
              continue;
            }

            if (tmpMdl.Source.TargetNode != gfMotion.Source.TargetNode)
            {
              continue;
            }

            gfModel = tmpMdl;
            break;
          }
        }
      }

      // ファイル出力
      bool isExportSuccess = false;
      try
      {
        using (var exporter = new GfmotBinaryExporter(fileName, outputPath, gfMotion, gfModel))
        {
          isExportSuccess = exporter.ExportBinary(compLevel, isPokeMode);
        }
      }
      catch (Exception e)
      {
        ErrorMessage = e.Message;
        ErrorStackTrance = e.StackTrace;

        // 失敗するかもしれない
        isExportSuccess = false;
      }

      // ファイル出力失敗したか？
      if (!isExportSuccess)
      {
        return ReturnCode.Error_FaildExport;
      }

      // 成功
      return ReturnCode.Success;
    }
  }
}
