using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace AnimationConverter
{
  class ConvLog
  {
    public string ErrorMessage { get; set; }
    public string ErrorStackTrance { get; set; }

    // logコンバート
    public ReturnCode Conv(string inputPath, string outputPath)
    {
      // 全ての.gfmotファイルを取得
      var files = Directory.GetFiles(inputPath, "*.gfmot");

      var resultList = files.AsParallel().Select((file) =>
        {
          var isSuccess = true;
          var errorMessage = string.Empty;
          var errorStackTrace = string.Empty;

          try
          {
            var gfMotion = Common.DeserializeXml<Gfmot.GfMotion>(file);

            var sb = new StringBuilder();
            sb.AppendLine("[ANIMATION_INFO_START]");
            sb.AppendFormat("TOTAL_FRAME {0}", gfMotion.FrameSize);
            sb.AppendLine();
            sb.AppendLine("[ANIMATION_INFO_END]");
            sb.AppendLine();

            sb.AppendLine("[ATTRIBUTE_KEY_START]");
            if (gfMotion.AttributeKeyAnimation != null && gfMotion.AttributeKeyAnimation.Attribute != null)
            {
              foreach (var key in gfMotion.AttributeKeyAnimation.Attribute)
              {
                sb.AppendFormat("{0} {1}", key.Name, key.AttributeKey.Length);
                sb.AppendLine();
              }
            }
            sb.AppendLine("[ATTRIBUTE_KEY_END]");
            sb.AppendLine();

            File.WriteAllText(Path.ChangeExtension(file, ".log"), sb.ToString());
          }
          catch (Exception e)
          {
            errorMessage = e.Message;
            errorStackTrace = e.StackTrace;
          }

          return Tuple.Create(isSuccess, errorMessage, errorStackTrace);
        }).ToArray();

      foreach (var result in resultList)
      {
        if (result.Item1)
        {
          continue;
        }

        this.ErrorMessage = result.Item2;
        this.ErrorStackTrance = result.Item3;

        return ReturnCode.Error_FaildExport;
      }
      
      return ReturnCode.Success;
    }
  }
}
