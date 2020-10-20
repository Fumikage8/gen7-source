using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

using System.Xml;
using System.Xml.Serialization;

#if false

使い方

-i 入力パス
-o 出力パス

-m モード
gfmot gfmotコンバート
mlst パックコンバート
divideTexAnim UVアニメーション分割
mcnf モーションコンフィグコンバート(blend_list含む)

-debug デバッグ用

■ 単体ファイルコンバート
AnimationConverter.exe -i inputPath -o outputPath

■ mlst指定コンバート
AnimationConverter.exe -mi mlstPath -o outputDirectory

■ 部分アニメーション用に不要なキーを削除
AnimationConverter.exe -di inputDirectory -o outputDirectory

#endif

namespace AnimationConverter
{
  class Def
  {
    public static readonly string ExtAnimFile = "gfmot";
    public static readonly string ExtBinaryAnimFile = "gfbmot";

    public static readonly string ExtPackedAnimFile = "gfbmotp";
    public static readonly string ExtPackedAnimHeader = "h";

    public static readonly string ExtBinaryMcnfFile = "bmcnf";

    public static readonly string[] SupportVersionList =
    {
      "0.95",
      "0.96",
      "1.0",
      "1.1",
      "1.2",
    };

    public enum Modes
    {
      Gfmot,
      Mlst,
      DivideTexAnim,
      Mcnf,
      Log,
    }

    public static readonly Dictionary<string, Modes> ModesDict = new Dictionary<string, Modes>()
    {
      {"gfmot", Modes.Gfmot},
      {"mlst", Modes.Mlst},
      {"divideTexAnim", Modes.DivideTexAnim},
      {"mcnf", Modes.Mcnf},
      {"log", Modes.Log},
    };
  }

  class Option
  {
    public Option(string[] args)
    {
      // 引数が無ければ無視
      if (args == null)
      {
        return;
      }
      if (args.Length == 0)
      {
        return;
      }

      this.Parse(args);
    }

    private void Parse(string[] args)
    {
      this.OptionCompLevel = -1;
      this.InputPathList = new List<string>();

      for (var i = 0; i < args.Length; i += 2)
      {
        var key = args[i];
        var value = args[i + 1];

        switch (key)
        {
        case "-m": // モード
          {
            var mode = Def.Modes.Gfmot;
            if (Def.ModesDict.ContainsKey(value))
            {
              mode = Def.ModesDict[value];
            }
            this.Mode = mode;
          } 
          break;

        case "-i": // 入力パス
          this.InputPathList.Add(value);
          break;

        case "-o": // 出力パス
          this.OutputPath = value;
          break;

        case "-comp": // 圧縮レベル
          {
            int result;
            if (int.TryParse(value, out result))
            {
              this.OptionCompLevel = result;
            }
          }
          break;

        case "-poke": // ポケモード
          this.OptionIsPokeMode = (value == "on");
          break;

        case "-debug": // デバッグ
          this.ErrorPopUpEnabled = true;
          break;

        // 旧引数互換
        case "-di":
          this.InputPathList.Add(value);
          this.Mode = Def.Modes.DivideTexAnim;
          break;
        // 旧引数互換
        case "-mi":
          this.InputPathList.Add(value);
          this.Mode = Def.Modes.Mlst;
          break;
        }
      }

      // 単体コンバート用
      if (this.OutputPath == null)
      {
        // 出力パスの指定がない
        switch (this.Mode)
        {
        case Def.Modes.Gfmot:
          this.OutputPath = Path.ChangeExtension(this.InputPathList[0], Def.ExtBinaryAnimFile);
          break;
        case Def.Modes.Mlst:
          this.OutputPath = Path.GetDirectoryName(this.InputPathList[0]);
          break;
        case Def.Modes.DivideTexAnim:
          this.OutputPath = this.InputPathList[0];
          break;
        case Def.Modes.Mcnf:
          this.OutputPath = Path.ChangeExtension(this.InputPathList[0], Def.ExtBinaryMcnfFile);
          break;
        case Def.Modes.Log:
          this.OutputPath = this.InputPathList[0];
          break;
        }
      }
    }

    public Def.Modes Mode { get; set; }

    public List<string> InputPathList { get; set; }
    public string OutputPath { get; set; }

    public int OptionCompLevel { get; set; }
    public bool OptionIsPokeMode { get; set; }

    public bool ErrorPopUpEnabled { get; set; }
  }

  public enum ReturnCode
  {
    Success, // 変換成功

    Error_InvalidArgument, // 無効な引数
    Error_NotFoundInputPath, // 入力ファイルが見つからない
    Error_FaildDeserialize, // ファイルの解析に失敗
    Error_FaildExport,      // ファイルの出力に失敗
    Error_UnsupportedVersion,   // 対応バージョン以外
  }

  class Program
  {
    static int Main(string[] args)
    {
#if false
      var jointText =
        "Origine Waist FeelerA FeelerA1 FeelerA10 FeelerA11 FeelerA12 FeelerA13 FeelerA14 FeelerA15 FeelerA16 FeelerA17 FeelerA18 FeelerA19 FeelerA2 FeelerA20 FeelerA3 FeelerA4 FeelerA5 FeelerA6 FeelerA7 FeelerA8 FeelerA9 FeelerB FeelerB1 FeelerB2 FeelerB3 FeelerB4 FeelerB5 FeelerB6 FeelerB7 FeelerB8 FeelerC FeelerC1 FeelerC3 FeelerC5 FeelerC7 Head Hips";

      var jointList = jointText.Split();

      //var mot = Mot.Mot.LoadMot(@"C:\home\xy_pokemon\kujira\pm0707_00_pupa\exports\LittleEndian_pm0707_00_ba\pm0707_00_ba10_waitB01.mot", jointList);
      //mot.DumpText(@"C:\home\xy_pokemon\kujira\pm0707_00_pupa\exports\LittleEndian_pm0707_00_ba\dump.txt");
#endif

      // 引数があるか？
      if ((args == null) || (args.Length == 0))
      {
        System.Windows.Forms.MessageBox.Show("引数がありません");
        return (int)ReturnCode.Error_InvalidArgument;
      }

      // 引数解析
      var option = new Option(args);

      // 単体コンバート
      var result = ReturnCode.Success;
      var errorMessage = string.Empty;
      var errorStackTrance = string.Empty;

      switch (option.Mode)
      {
      case Def.Modes.Gfmot:
        {
          var converter = new ConvGfmot();
          result = converter.Conv(option.InputPathList[0], option.OutputPath, option.OptionCompLevel, option.OptionIsPokeMode);
          errorMessage = converter.ErrorMessage;
          errorStackTrance = converter.ErrorStackTrance;
        }
        break;

      case Def.Modes.Mlst:
        {
          var converter = new ConvMlst();
          result = converter.Conv(option.InputPathList.ToArray(), option.OutputPath, option.OptionIsPokeMode);
          errorMessage = converter.ErrorMessage;
          errorStackTrance = converter.ErrorStackTrance;
        }
        break;

      case Def.Modes.DivideTexAnim:
        {
          var converter = new ConvDivideTexAnim();
          result = converter.Conv(option.InputPathList[0], option.OutputPath);
          errorMessage = converter.ErrorMessage;
          errorStackTrance = converter.ErrorStackTrance;
        }
        break;

      case Def.Modes.Mcnf:
        {
          var converter = new ConvMcnf();
          result = converter.Conv(option.InputPathList.ToArray(), option.OutputPath);
          errorMessage = converter.ErrorMessage;
          errorStackTrance = converter.ErrorStackTrance;
        }
        break;

      case Def.Modes.Log:
        {
          var converter = new ConvLog();
          result = converter.Conv(option.InputPathList[0], option.OutputPath);
          errorMessage = converter.ErrorMessage;
          errorStackTrance = converter.ErrorStackTrance;
        }
        break;
      }

      // エラー出力
      if (result != ReturnCode.Success)
      {
        // 実行中のアセンブリからビルド日時を取得
        var asm = System.Reflection.Assembly.GetExecutingAssembly();
        var version = asm.GetName().Version;
        var days = version.Build;
        var seconds = version.Revision * 2;

        var buildDate = new DateTime(2000, 1, 1);
        buildDate = buildDate.AddDays(days);
        buildDate = buildDate.AddSeconds(seconds);

        var errorTitle = string.Format("AnimationConverter {0}", buildDate);

        var sb = new StringBuilder();
        sb.AppendFormat("ConvertMode : {0}", option.Mode);
        sb.AppendLine();
        foreach (var inputPath in option.InputPathList)
        {
          sb.AppendFormat("InputPath : {0}", inputPath);
        }
        sb.AppendLine();
        sb.AppendLine();
        sb.AppendFormat("ErrorType : {0}", result.ToString());
        sb.AppendLine();
        if (errorMessage != null)
        {
          sb.AppendLine();
          sb.AppendLine(errorMessage);
        }
        if (errorStackTrance != null)
        {
          sb.AppendLine();
          sb.AppendLine(errorStackTrance);
        }

        var outputErrorMessage = sb.ToString();

        // 標準エラー出力
        Console.Error.WriteLine(errorTitle);
        Console.Error.WriteLine(outputErrorMessage);

        // エラーポップアップ
        if (option.ErrorPopUpEnabled)
        {
          System.Windows.Forms.MessageBox.Show(
            outputErrorMessage,
            errorTitle,
            System.Windows.Forms.MessageBoxButtons.OK,
            System.Windows.Forms.MessageBoxIcon.Error);
        }
      }

      return (int)result;
    }
  }
}
