using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace ModelViewer
{
  static class Program
  {
    private const double waitTime = 1000.0f / 60.0f;

    /// <summary>
    /// アプリケーションのメイン エントリ ポイントです。
    /// </summary>
    [STAThread]
    static int Main()
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);

      var args = Environment.GetCommandLineArgs();
      if (args.Contains("-c"))
      {
        Console.WriteLine("ModelViewer Command Line Mode");

        var opts = new Options();
        bool isSuccess = CommandLine.Parser.Default.ParseArguments(args, opts);

        if (!isSuccess)
        {
          return 1;
        }

        var path = opts.ExportsDirectory.Replace("\\", "/");

        ApplicationCore applicationCore = new ApplicationCore();
        applicationCore.Show();

        var sw = new System.Diagnostics.Stopwatch();
        sw.Start();

        applicationCore.LoadModel(path);

        foreach (var motion in opts.Motions)
        {
          applicationCore.SimpleExec(path, motion, 0);
        }

        applicationCore.Close();

        return 0;
      }
      else
      {

        ApplicationCore applicationCore = new ApplicationCore();
        applicationCore.Show();

        var sw = new System.Diagnostics.Stopwatch();
        var frameCount = 0;
        sw.Start();
        while (applicationCore.Created)
        {
          //メインの処理(常に例外はキャッチ)
          applicationCore.MainLoop();

          // 60フレーム回したらその時点でのFPSを表示
          if (++frameCount >= 60)
          {
            applicationCore.SetFPS(frameCount / sw.Elapsed.TotalSeconds);

            frameCount = 0;
            sw.Restart();
          }

          Application.DoEvents();
        }
        return 0;
      }
    }
  }
}
