using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

#if false

■引数
-m ターゲットプラットフォーム winglかctrかdepend
-i コンバートするモデルのパス
-o 出力するディレクトリ

#endif

namespace BinaryModelPacker
{
  enum Mode
  {
    wingl,
    ctr,
    depend,
  }

  class Option
  {
    public static uint FormatVersion = 0x00010000;

    private static readonly Dictionary<string, Mode> ModeDictionary = new Dictionary<string, Mode>()
    {
      {Mode.wingl.ToString(), Mode.wingl},
      {Mode.ctr.ToString(), Mode.ctr},
      {Mode.depend.ToString(), Mode.depend},
    };

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
      this.OutputMode = Mode.wingl;

      for (var i = 0; i < args.Length; i++)
      {
        var key = args[i];

        switch (key)
        {
        case "-i": // 入力パス
            this.InputPath = args[i + 1];
            break;

        case "-o": // 出力パス
            this.OutputPath = args[i + 1];
            break;

        case "-m": // 出力モード
            if (ModeDictionary.ContainsKey(args[i + 1]))
            {
                this.OutputMode = ModeDictionary[args[i + 1]];
            }
            break;

        case "-si": // シェーダー入力ディレクトリ
            this.ShaderInputDir = args[i + 1];
            break;

        case "-ti": // テクスチャ入力ディレクトリ
            this.TextureInputDir = args[i + 1];
            break;

        case "--notexture":
            this.ExcludeTexture = true;
            break;

        case "--noshader":
            this.ExcludeShader = true;
            break;
        }
      }

      if (this.OutputPath == null)
      {
        if (this.OutputMode == Mode.depend)
        {
        }
        else
        {
          this.OutputPath = Path.Combine(Path.GetDirectoryName(this.InputPath), "binary_", Path.GetFileNameWithoutExtension(this.InputPath));
          this.OutputPath = Path.Combine(this.OutputPath, this.OutputMode.ToString());
        }
      }

      if (ShaderInputDir == null)
      {
        this.ShaderInputDir = Path.GetDirectoryName(this.InputPath);
      }
      if (TextureInputDir == null)
      {
        this.TextureInputDir = Path.GetDirectoryName(this.InputPath);
      }
    }

    public string InputPath { get; set; }
    public string ShaderInputDir { get; set; }
    public string TextureInputDir { get; set; }
    public string OutputPath { get; set; }
    public bool ExcludeTexture { get; set; }
    public bool ExcludeShader { get; set; }

    public Mode OutputMode { get; set; }
  }

  class Program
  {
    private enum ReturnCode
    {
      Success, // 変換成功

      Error_InvalidArgument,            // 無効な引数
      Error_NotFoundInputPath,          // 入力ファイルが見つからない
      Error_FailedToDesirializeModel,    // モデルファイルのデシリアライズに失敗
      Error_FailedToConvModel,           // モデルファイルのコンバートに失敗
      Error_FailedToConvShader,          // シェーダーファイルのコンバートに失敗
      Error_FailedToConvTexture,         // テクスチャファイルのコンバートに失敗
      Error_Unknown,                    // 不明なエラー
    }

    static List<string> s_ModelFileList = new List<string>();
    static List<string> s_TextureFileList = new List<string>();
    static List<string> s_VshShaderFileList = new List<string>();
    static List<string> s_GshShaderFileList = new List<string>();
    static List<string> s_FshShaderFileList = new List<string>();

    static int Main(string[] args)
    {
      // 引数解析
      var option = new Option(args);

      var result = ReturnCode.Success;
      try
      {
        result = Conv(option.InputPath, option.ShaderInputDir, option.TextureInputDir, option.OutputPath, option.OutputMode, option.ExcludeTexture, option.ExcludeShader);
      }
      catch (Exception e)
      {
        Console.WriteLine(e.Message);
        Console.WriteLine(e.StackTrace);
      }

      if (result != ReturnCode.Success)
      {
        Console.WriteLine("Error: failed to convert");
        Console.WriteLine(result);
      }

      return (int)result;
    }

    static ReturnCode Conv(string inputPath, string shaderInputDir, string textureInputDir, string outputPath, Mode outputMode, bool excludeTexture, bool excludeShader)
    {
      // 入力ファイルが無ければエラー
      if (!File.Exists(inputPath))
      {
        return ReturnCode.Error_NotFoundInputPath;
      }

      var result = ReturnCode.Success;

      var toolDir = System.Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");

      var gfmdl = GfModel.Deserialize(inputPath);

      if (gfmdl == null)
      {
        return ReturnCode.Error_FailedToDesirializeModel;
      }

      // 依存関係が記述されたファイルを出力
      if (outputMode == Mode.depend)
      {
        return OutputDepend(outputPath, gfmdl);
      }

      // 出力フォルダが無ければ作成
      if (!Directory.Exists(outputPath))
      {
        Directory.CreateDirectory(outputPath);
      }

      // モデルのバイナリ化
      result = ConvertModel(inputPath, outputPath, toolDir, outputMode.ToString(), textureInputDir, shaderInputDir);
      if (result != ReturnCode.Success)
      {
        return result;
      }

      // --notextureオプションが指定されたときはTextureListを空に
      if (excludeTexture)
      {
        gfmdl.TextureList = null;
      }

      // ctr版はExtTextureは不要
      if (outputMode == Mode.ctr)
      {
        gfmdl.TextureList = gfmdl.TextureList.Where(texture => texture.Type != "ExtTexture").ToArray();
      }

      // テクスチャのバイナリ化
      result = ConvertTexture(gfmdl, textureInputDir, outputPath, toolDir, outputMode.ToString());

      if (result != ReturnCode.Success)
      {
        return result;
      }

      if (excludeShader)
      {
        gfmdl.ShaderList = null;
      }

      // シェーダーのバイナリ化
      result = ConvertShader(gfmdl, shaderInputDir, outputPath, toolDir, outputMode.ToString());

      if (result != ReturnCode.Success)
      {
        return result;
      }

      // モデルのパック
      PackModel(outputPath, outputMode.ToString());

      // 成功
      return ReturnCode.Success;
    }

    private static ReturnCode OutputDepend(string outputPath, GfModel gfmdl)
    {
      // シェーダー
      var fileList = new List<string>();
      if (gfmdl.ShaderList != null)
      {
        foreach (var shader in gfmdl.ShaderList)
        {
          if (!string.IsNullOrEmpty(shader.VshFilePath))
          {
            if (!fileList.Contains(shader.VshFilePath))
            {
              fileList.Add(shader.VshFilePath);
            }
          }
          if (!string.IsNullOrEmpty(shader.FshFilePath))
          {
            if (!fileList.Contains(shader.FshFilePath))
            {
              fileList.Add(shader.FshFilePath);
            }
          }
          if (!string.IsNullOrEmpty(shader.GshFilePath))
          {
            if (!fileList.Contains(shader.GshFilePath))
            {
              fileList.Add(shader.GshFilePath);
            }
          }
        }
      }
      // テクスチャ
      if (gfmdl.TextureList != null)
      {
        foreach (var texture in gfmdl.TextureList)
        {
          if (!string.IsNullOrEmpty(texture.FilePath))
          {
            if (!fileList.Contains(texture.FilePath))
            {
              fileList.Add(texture.FilePath);
            }
          }
        }
      }

      // 出力パスが空なら標準出力にする
      if (string.IsNullOrEmpty(outputPath))
      {
        foreach (var line in fileList)
        {
          Console.WriteLine(line);
        }
      }
      else
      {
        // 出力パスがあるのでそちらにテキストとして出力する

        // 出力フォルダがなければ作成
        var outputDirectory = Path.GetDirectoryName(outputPath);
        if (!Directory.Exists(outputDirectory))
        {
          Directory.CreateDirectory(outputDirectory);
        }

        // 出力
        File.WriteAllLines(outputPath, fileList.ToArray());
      }

      return ReturnCode.Success;
    }

    // ファイルのパック
    private static void PackFiles(BinaryWriter bw, string[] filePathList)
    {
      var splitChar = new char[] { '.' };

      // オフセット書き込み
      var offset = (int)bw.BaseStream.Position;
      offset += sizeof(int) * filePathList.Length;
      foreach (var filePath in filePathList)
      {
        bw.Write(offset);

        var fileName = Path.GetFileName(filePath).Split(splitChar)[0];

        offset += fileName.Length + 1;  // +1は文字サイズ分
        offset += sizeof(int);          // オフセットサイズ分
      }

      // ファイル名リスト
      foreach (var filePath in filePathList)
      {
        var fileName = Path.GetFileName(filePath).Split(splitChar)[0];
        bw.Write(fileName);

        // 128バイトアラインメントに揃える
        while (offset % 128 != 0)
        {
          ++offset;
        }

        bw.Write(offset);

        var fileInfo = new FileInfo(filePath);
        offset += (int)fileInfo.Length;
      }

      // ファイルのパック
      foreach (var filePath in filePathList)
      {
        // 128バイトアラインメントに揃える
        while (bw.BaseStream.Position % 128 != 0)
        {
          bw.Write((byte)0);
        }

        var fileBytes = File.ReadAllBytes(filePath);
        bw.Write(fileBytes);
      }
    }

    // モデルのパック
    private static void PackModel(string packDirectory, string outputMode)
    {
      var modelPathList = s_ModelFileList.ToArray();
      var texturePathList = s_TextureFileList.ToArray();
      var shaderVertexPathList = s_VshShaderFileList.ToArray();
      var shaderGeometryPathList = s_GshShaderFileList.ToArray();
      var shaderFragmentPathList = s_FshShaderFileList.ToArray();

      var outputFilename = Path.GetFileNameWithoutExtension(modelPathList[0]);

      var modelPackPath = Path.GetFileName(packDirectory);
      modelPackPath = Path.Combine(packDirectory, Path.ChangeExtension(outputFilename, outputMode + ".gfbmdlp"));
      using (var bw = new BinaryWriter(File.OpenWrite(modelPackPath)))
      {
        // 上書き用にサイズを0に
        bw.BaseStream.SetLength(0);

        // フォーマットバージョン
        bw.Write(Option.FormatVersion);

        // ファイル数書き込み
        bw.Write(modelPathList.Length);           // モデル数
        bw.Write(texturePathList.Length);         // テクスチャ数
        bw.Write(shaderVertexPathList.Length);    // バーテクスシェーダ数
        bw.Write(shaderGeometryPathList.Length);  // ジオメトリシェーダ数
        bw.Write(shaderFragmentPathList.Length);  // フラグメントシェーダ数

        // ファイルのパック
        var allFilePathList = new List<string>();
        allFilePathList.AddRange(modelPathList);
        allFilePathList.AddRange(texturePathList);
        allFilePathList.AddRange(shaderVertexPathList);
        allFilePathList.AddRange(shaderFragmentPathList);
        allFilePathList.AddRange(shaderGeometryPathList);
        PackFiles(bw, allFilePathList.ToArray());
      }
    }

    // テクスチャのコンバート
    private static ReturnCode ConvertTexture(GfModel gfmdl, string inputDirectory, string outputDirectory, string toolDir, string outputMode)
    {
      if (gfmdl.TextureList == null)
      {
        return ReturnCode.Success;
      }

      var exePath = Path.Combine(toolDir, "TextureConverter", "TextureConverter.exe");

      // モデルが依存する全テクスチャ
      foreach (var texture in gfmdl.TextureList)
      {
        var inputPath = Path.Combine(inputDirectory, Path.GetDirectoryName(texture.FilePath), Path.GetFileName(texture.FilePath));
        if (!File.Exists(inputPath))
        {
          Console.WriteLine("Error: cannot find texture file: {0}", inputPath);
          Console.WriteLine("Error: failed to convert textures");
          return ReturnCode.Error_FailedToConvTexture;
        }

        // 引数作成
        var outputPath = Path.Combine(outputDirectory, Path.GetDirectoryName(texture.FilePath), Path.GetFileName(texture.FilePath));
        outputPath = Path.ChangeExtension(outputPath, outputMode + ".btex");

        var outputDir = Path.GetDirectoryName(outputPath);
        if (!Directory.Exists(outputDir))
        {
          Directory.CreateDirectory(outputDir);
        }

        var args = "-i \"" + inputPath + "\"" + " -o \"" + outputPath + "\"";

        // コンバート開始
        //Console.WriteLine("ConvertTexture : " + inputPath);
        //Console.WriteLine(" -> " + outputPath);
        var proc = Program.StartProcess(exePath, args);
        proc.WaitForExit();

        // コンバート成功したか？
        if (proc.ExitCode != 0)
        {
          Console.WriteLine("Error: failed to convert textures");
          return ReturnCode.Error_FailedToConvTexture;
        }

        s_TextureFileList.Add(outputPath);
      }
      return ReturnCode.Success;
    }

    // シェーダーのコンバート
    private static ReturnCode ConvertShader(GfModel gfmdl, string inputDirectory, string outputDirectory, string toolDir, string outputMode)
    {
      if (gfmdl.ShaderList == null)
      {
        return ReturnCode.Success;
      }

      var exePath = Path.Combine(toolDir, "ShaderCompiler", "ShaderCompiler.exe");

      Func<string, string, string> getOutputShaderPath = (shaderpath, extension) =>
        {
          if (string.IsNullOrEmpty(shaderpath))
          {
            return null;
          }

          var outputPath = Path.Combine(outputDirectory, Path.GetDirectoryName(shaderpath), Path.GetFileName(shaderpath));
          outputPath = Path.ChangeExtension(outputPath, outputMode + extension);

          return outputPath;
        };

      Func<string, string> getInputShaderPath = (shaderpath) =>
        {
          if (string.IsNullOrEmpty(shaderpath))
          {
            return null;
          }

          return Path.Combine(inputDirectory, Path.GetDirectoryName(shaderpath), Path.GetFileName(shaderpath));
        };

      Func<string, string, string, string, bool> convShader = (vshPath, gshPath, fshPath, outputPath) =>
        {
          if (string.IsNullOrEmpty(outputPath))
          {
            return true;
          }

          var outputDir = Path.GetDirectoryName(outputPath);
          if (!Directory.Exists(outputDir))
          {
            Directory.CreateDirectory(outputDir);
          }

          // 引数作成
          var args = "-vsh \"" + vshPath + "\"";
          if (!string.IsNullOrEmpty(gshPath))
          {
            args += " -gsh \"" + gshPath + "\"";
          }
          
          args += " -fsh \"" + fshPath + "\" -o \"" + outputPath + "\" -def HLSL -def 1";

          // コンバート開始
          //Console.WriteLine("ConvertShaderVsh : " + vshPath);
          //Console.WriteLine("ConvertShaderGsh : " + gshPath);
          //Console.WriteLine("ConvertShaderFsh : " + fshPath);
          //Console.WriteLine(" -> " + outputPath);
          var proc = Program.StartProcess(exePath, args);
          proc.WaitForExit();

          // コンバート成功したか？
          if (proc.ExitCode != 0)
          {
            return false;
          }

          //// NULL文字を終端に追加
          //if (outputMode == "wingl")
          //{
          //  using (var fs = File.OpenWrite(outputPath))
          //  {
          //    fs.Seek(0, SeekOrigin.End);
          //    fs.WriteByte(0);
          //  }
          //}

          return true;
        };

      foreach (var shader in gfmdl.ShaderList)
      {
        // 各シェーダーのパスを取得
        var vshPath = getInputShaderPath(shader.VshFilePath);
        var gshPath = getInputShaderPath(shader.GshFilePath);
        var fshPath = getInputShaderPath(shader.FshFilePath);

        // 各シェーダーの出力パスを取得
        var outputVshPath = getOutputShaderPath(shader.VshFilePath, ".gfbvsh");
        var outputGshPath = getOutputShaderPath(shader.GshFilePath, ".gfbgsh");
        var outputFshPath = getOutputShaderPath(shader.FshFilePath, ".gfbfsh");

        // ファイルが見つかるかチェック
        var fileExist = true;
        if (!File.Exists(vshPath))
        {
          Console.WriteLine("Error: cannot find gfvsh file: {0}", vshPath);
          fileExist = false;
        }
        if (!string.IsNullOrEmpty(gshPath) && !File.Exists(gshPath))
        {
          Console.WriteLine("Error: cannot find gfgsh file: {0}", gshPath);
          fileExist = false;
        }
        if (!File.Exists(fshPath))
        {
          Console.WriteLine("Error: cannot find gffsh file: {0}", gshPath);
          fileExist = false;
        }

        if (!fileExist)
        {
          Console.WriteLine("Error: failed to convert shaders");
          return ReturnCode.Error_FailedToConvShader;
        }

        var resultVsh = convShader(vshPath, gshPath, fshPath, outputVshPath);
        var resultGsh = convShader(vshPath, gshPath, fshPath, outputGshPath);
        var resultFsh = convShader(vshPath, gshPath, fshPath, outputFshPath);

        if (!resultVsh || !resultGsh || !resultFsh)
        {
          Console.WriteLine("Error: failed to convert shaders");
          return ReturnCode.Error_FailedToConvShader;
        }

        s_VshShaderFileList.Add(outputVshPath);
        if (outputGshPath != null)
        {
          s_GshShaderFileList.Add(outputGshPath);
        }
        s_FshShaderFileList.Add(outputFshPath);
      }
      return ReturnCode.Success;
    }

    // モデルのコンバート
    private static ReturnCode ConvertModel(string inputPath, string outputDirectory, string toolDir, string outputMode, string textureInputDir, string shaderInputDir)
    {
      var exePath = Path.Combine(toolDir, "ModelConverter", "ModelConverter.exe");
      var outputModelDirectory = Path.Combine(outputDirectory, "Model");

      if (!Directory.Exists(outputModelDirectory))
      {
        Directory.CreateDirectory(outputModelDirectory);
      }

      // 引数作成
      var modelName = Path.GetFileName(inputPath);
      var outputPath = Path.Combine(outputModelDirectory, modelName);
      outputPath = Path.ChangeExtension(outputPath, outputMode + ".gfbmdl");
      var args = "-i \"" + inputPath + "\"" + " -o \"" + outputPath + "\"" + " -textureDir \"" + textureInputDir + "\"" + " -shaderDir \"" + shaderInputDir;

      // コンバート開始
      //Console.WriteLine("ConvertModel : " + inputPath);
      //Console.WriteLine(" -> " + outputPath);
      var proc = Program.StartProcess(exePath, args);
      proc.WaitForExit();

      // コンバート成功したか？
      if (proc.ExitCode != 0)
      {
        Console.WriteLine("Error: failed to convert model");
        return ReturnCode.Error_FailedToConvModel;
      }

      s_ModelFileList.Add(outputPath);
      return ReturnCode.Success;
    }

    static private System.Diagnostics.Process StartProcess(string exePath, string args)
    {
      var startInfo = new System.Diagnostics.ProcessStartInfo(exePath, args);
      startInfo.CreateNoWindow = false;
      startInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
      var proc = System.Diagnostics.Process.Start(startInfo);

      return proc;
    }

  }
}
