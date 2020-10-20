using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace MinusScaleChecker
{
  class Program
  {
    static void Main(string[] args)
    {
      if (args.Length != 2)
      {
        ConsoleWriteLineColor("引数がおかしいです！", ConsoleColor.Red);
        return;
      }

      var outputPath = args[0];
      var checkDirectory = args[1];

      var checkFiles = Directory.GetFiles(checkDirectory, "*.gfmot", SearchOption.AllDirectories);
      var result = CheckAllFiles(checkFiles);

      File.WriteAllText(outputPath, result);
    }

    static void ConsoleWriteLineColor(string line, ConsoleColor color)
    {
      var oldColor = Console.ForegroundColor;
      Console.ForegroundColor = color;
      Console.WriteLine(line);
      Console.ForegroundColor = oldColor;
    }

    static string CheckAllFiles(string[] checkFiles)
    {
      var sb = new StringBuilder();
      sb.AppendLine();
      sb.AppendLine();

      var errorNo = 0;
      for (int i = 0; i < checkFiles.Length; ++i)
      {
        var checkFile = checkFiles[i];

        Console.WriteLine("{0}/{1} : {2}", i, checkFiles.Length, checkFile);
        var errorNodeList = CheckFile(checkFile);

        if (errorNodeList.Count() == 0)
        {
          continue;
        }

        sb.AppendLine(string.Format("[{0}] {1}", errorNo, checkFile));
        foreach (var nodeName in errorNodeList)
        {
          sb.AppendLine(nodeName);
        }
        sb.AppendLine();

        ++errorNo;
      }

      // 結果を表示
      var resultString = string.Format("エラーファイル数 {0} / チェックファイル数 {1}", errorNo, checkFiles.Length);
      var color = (errorNo == 0) ? ConsoleColor.White : ConsoleColor.Red;
      ConsoleWriteLineColor(resultString, color);

      sb.Insert(0, resultString);

      return sb.ToString();
    }

    static List<string> CheckFile(string checkFile)
    {
      var gfMotion = AnimationConverter.Common.DeserializeXml<Gfmot.GfMotion>(checkFile);

      var errorNodeList = new List<string>();
      if (gfMotion == null)
      {
        return errorNodeList;
      }

      if (gfMotion.SkeltalAnimation == null)
      {
        return errorNodeList;
      }

      if (gfMotion.SkeltalAnimation.Node == null)
      {
        return errorNodeList;
      }

      var nodes = gfMotion.SkeltalAnimation.Node;
      if (nodes.Length == 0)
      {
        return errorNodeList;
      }

      foreach (var node in nodes)
      {
        if (!CheckNode(node))
        {
          // 赤文字で警告
          ConsoleWriteLineColor(string.Format("NG!マイナススケール {0}", node.Name), ConsoleColor.Red);

          // エラーリストに追加
          errorNodeList.Add(node.Name);
        }
      }

      return errorNodeList;
    }

    static bool CheckNode(Gfmot.JointAnimation_Node node)
    {
      // スケールをXYZ全てチェック
      if (!CheckScale(node.ScaleX))
      {
        return false;
      }
      if (!CheckScale(node.ScaleY))
      {
        return false;
      }
      if (!CheckScale(node.ScaleZ))
      {
        return false;
      }

      // 正常
      return true;
    }

    static bool CheckScale(Gfmot.Common_Keys keys)
    {
      // キーがないなら無視
      if (keys == null)
      {
        return true;
      }

      // 1フレームキーのチェック
      if (keys.OneFrameKey != null)
      {
        if (keys.OneFrameKey.Value < 0)
        {
          // マイナススケール
          return false;
        }

        // 正常
        return true;
      }

      // エルミートキーのチェック
      foreach (var hermiteKey in keys.HermiteKeys)
      {
        if (hermiteKey.Value < 0)
        {
          // マイナススケール
          return false;
        }
      }

      // 正常
      return true;
    }
  }
}
