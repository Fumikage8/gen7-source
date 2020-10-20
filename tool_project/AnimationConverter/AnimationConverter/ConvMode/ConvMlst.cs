using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace AnimationConverter
{
  class ConvMlst
  {
    public string ErrorMessage { get; set; }
    public string ErrorStackTrance { get; set; }

    // Mlstコンバート
    public ReturnCode Conv(string[] inputPathList, string outputDirectory, bool isPokeMode)
    {
      // 入力ファイルが無ければ無視
      if (inputPathList == null)
      {
        return ReturnCode.Error_InvalidArgument;
      }

      // 入力パスが多過ぎてもエラー
      if (inputPathList.Length > 2)
      {
        return ReturnCode.Error_InvalidArgument;
      }

      var mlstPaths = inputPathList.Where((path) => Path.GetExtension(path) == ".mlst").ToArray();
      if ((mlstPaths == null) || (mlstPaths.Length != 1))
      {
        return ReturnCode.Error_InvalidArgument;
      }
      var mlstPath = mlstPaths[0];

      if (mlstPath == null)
      {
        return ReturnCode.Error_InvalidArgument;
      }
      if (!File.Exists(mlstPath))
      {
        return ReturnCode.Error_NotFoundInputPath;
      }

      var inputPaths = inputPathList.Where((path) => Path.GetExtension(path) != ".mlst").ToArray();
      var inputDir = ((inputPaths == null) || (inputPaths.Length != 1))
        ? Path.GetDirectoryName(mlstPath)
        : inputPaths[0];

      // 出力パス指定無ければ無視
      if (outputDirectory == null)
      {
        return ReturnCode.Error_InvalidArgument;
      }
      // 出力パスが存在しなければ作成
      if (!Directory.Exists(outputDirectory))
      {
        Directory.CreateDirectory(outputDirectory);
      }

      // mlst読み込み
      var outputDir = outputDirectory;

      var tmpFileNames = File.ReadAllLines(mlstPath);
      var fileNameList = new List<string>(tmpFileNames);
      fileNameList.RemoveAll((s) => string.IsNullOrEmpty(s) || string.IsNullOrWhiteSpace(s));

      // 並列でコンバートする
      var resultList = new List<ReturnCode>();
      Parallel.ForEach(fileNameList, (fileName) =>
        {
          var unitInputPath = Path.Combine(inputDir, Path.ChangeExtension(fileName, Def.ExtAnimFile));
          var unitOutputPath = Path.Combine(outputDir, Path.ChangeExtension(fileName, Def.ExtBinaryAnimFile));

          // ファイルが存在しないなら無視
          if (!File.Exists(unitInputPath))
          {
            return;
          }

          // ファイルが存在するならコンバートする
          var converter = new ConvGfmot();
          var unitResult = converter.Conv(unitInputPath, unitOutputPath, -1, isPokeMode);
          if (unitResult != ReturnCode.Success)
          {
            lock (resultList)
            {
              ErrorMessage = unitInputPath + System.Environment.NewLine + converter.ErrorMessage;
              ErrorStackTrance = converter.ErrorStackTrance;
              resultList.Add(unitResult);
            }
          }
        }
      );

      foreach (var result in resultList)
      {
        if (result != ReturnCode.Success)
        {
          return result;
        }
      }

      // 読み込む
      var fileBytesList = new List<byte[]>();
      foreach (var fileName in fileNameList)
      {
          var unitInputPath = Path.Combine(inputDir, Path.ChangeExtension(fileName, Def.ExtAnimFile));
          var unitOutputPath = Path.Combine(outputDir, Path.ChangeExtension(fileName, Def.ExtBinaryAnimFile));

          // ファイルが存在しないなら無視
          if (!File.Exists(unitInputPath))
          {
            // 判別用にわざとnullを追加
            fileBytesList.Add(null);
            continue;
          }

          // 出力されたファイルを読み込む
          fileBytesList.Add(File.ReadAllBytes(unitOutputPath));
      }

      var mlstFileName = Path.GetFileNameWithoutExtension(mlstPath);

      // パック
      var gmpPath = Path.Combine(outputDir, Path.ChangeExtension(mlstFileName, Def.ExtPackedAnimFile));

      using (var bw = new BinaryWriter(File.Open(gmpPath, FileMode.OpenOrCreate)))
      {
        // 上書き用にサイズを０に
        bw.BaseStream.SetLength(0);

        // ファイル数
        bw.Write(fileBytesList.Count);

        // オフセットリスト
        var offset = fileBytesList.Count * 4;
        foreach (var fileBytes in fileBytesList)
        {
          if (fileBytes == null)
          {
            // 空ファイルなら判別用にオフセット位置０に
            bw.Write(0);
          }
          else
          {
            // 通常のオフセット書き込み
            bw.Write(offset);
            offset += fileBytes.Length;
          } 
        }

        // 実際のファイル書き込み
        foreach (var fileBytes in fileBytesList)
        {
          // 空ファイルは無視する
          if (fileBytes != null)
          {
            bw.Write(fileBytes);
          }
        }
      }

      // ヘッダー
      var headerPath = Path.Combine(outputDir, Path.ChangeExtension(mlstFileName, Def.ExtPackedAnimHeader));
      using (var sw = new StreamWriter(headerPath))
      {
        // 上書き用にサイズを０に
        sw.BaseStream.SetLength(0);

#if false
        sw.WriteLine("#ifdef GFL_FILE_LINKED");
        sw.WriteLine();

        foreach (var fileName in fileNameList)
        {
          sw.Write("GFL_FILE_LINKED( ");
          sw.Write(fileName);
          sw.WriteLine(" )");
        }

        sw.WriteLine();
        sw.WriteLine("#undef GFL_FILE_LINKED");
        sw.WriteLine("#endif // GFL_FILE_LINKED");
#else
        var defHeader = Path.GetFileName(headerPath);
        defHeader = defHeader.ToUpper().Replace(".", "_");

        sw.WriteLine("#if !defined(" + defHeader + ")");
        sw.WriteLine("#define " + defHeader);
        sw.WriteLine();

        for (var i = 0; i < fileNameList.Count; ++i)
        {
          sw.Write("#define ");
          sw.Write(fileNameList[i].ToUpper().Replace(".", "_"));
          sw.WriteLine(" " + i);
        }

        sw.WriteLine();
        sw.WriteLine("#endif // " + defHeader);
#endif
      }


      // 成功
      return ReturnCode.Success;
    }

  }
}
