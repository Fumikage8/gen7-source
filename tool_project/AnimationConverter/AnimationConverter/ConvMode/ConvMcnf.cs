using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace AnimationConverter
{
  class ConvMcnf
  {
    public string ErrorMessage { get; set; }
    public string ErrorStackTrance { get; set; }

    // mcnfコンバート
    public ReturnCode Conv(string[] inputPathList, string outputPath)
    {
      // 入力ファイルが無ければ無視
      if ((inputPathList == null) || (inputPathList.Length == 0))
      {
        return ReturnCode.Error_InvalidArgument;
      }

      // 出力ファイルが無ければ無視
      if (outputPath == null)
      {
        return ReturnCode.Error_InvalidArgument;
      }

      // 0番目に.mcnfのパスが入っているはず
      var mcnfPath = inputPathList[0];
      if (string.IsNullOrEmpty(mcnfPath))
      {
        return ReturnCode.Error_InvalidArgument;
      }
      if (!File.Exists(mcnfPath))
      {
        return ReturnCode.Error_NotFoundInputPath;
      }

      // デシリアライズ
      var mcnf = Common.DeserializeXml<Mcnf.Mcnf>(mcnfPath);

      // デシリアライズに失敗したか？
      if (mcnf == null)
      {
        return ReturnCode.Error_FaildDeserialize;
      }

      // 1番目にblend_list.xmlのパスが入ってるかも
      var blendListPath = (inputPathList.Length > 1)
        ? inputPathList[1]
        : string.Empty;
      Mcnf.BlendList blendList = null;
      if (!string.IsNullOrEmpty(blendListPath))
      {
        if (!File.Exists(blendListPath))
        {
          return ReturnCode.Error_NotFoundInputPath;
        }

        // デシリアライズ
        blendList = ExcelXml.ExcelXml.Deserialize<Mcnf.BlendList>(blendListPath);

        // デシリアライズに失敗したか？
        if (blendList == null)
        {
          return ReturnCode.Error_FaildDeserialize;
        }
      }

      // 2番目にアニメーションのディレクトリが入ってるかも
      var animationDirectory = (inputPathList.Length > 2)
        ? inputPathList[2]
        : Path.GetDirectoryName(mcnfPath);

      // ファイル出力
      bool isExportSuccess = false;
      try
      {
        using (var exporter = new McnfBinaryExporter(outputPath, mcnf, blendList, animationDirectory))
        {
          isExportSuccess = exporter.ExportBinary();
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
