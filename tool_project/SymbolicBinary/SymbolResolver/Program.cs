using System;
using System.Collections.Generic;

namespace SymbolResolver
{

    enum ErrorCode
    {
        ArgmentError = 1,
        OutputDirNotExist,
        InvalidOutputPath,
        FileReadError,
        LackOfDefinitions,
        MissMatchDataSize,
    };

    /// <summary>
    /// SymbolResolver.exe [モード] [解決対象のシンボリックバイナリ] [出力ディレクトリ] [定義確認に用いるシンボルテーブル...]
    /// </summary>
    class Program
    {
        static int Main(string[] args)
        {
            // 標準出力への出力をUTF8に
            Console.OutputEncoding = System.Text.Encoding.UTF8;

            if (args.Length < 3)
            {
                Console.Error.WriteLine("Usage: SymbolResolver.exe [モード] [解決対象のシンボリックバイナリ] [出力ディレクトリ] [定義確認に用いるシンボルテーブル...]");
                return (int)ErrorCode.ArgmentError;
            }

            var mode = args[0];
            var srcSymBinaryPath = args[1];
            var refSymbolFilePath = srcSymBinaryPath + ".symbol";
            var outputDir = args[2];

            if (System.IO.Directory.Exists(outputDir) == false)
            {
                Console.Error.WriteLine("出力先のディレクトリが存在しません. {0}", outputDir);
                return (int)ErrorCode.OutputDirNotExist;
            }

            {
                var temp1 = System.IO.Path.GetFullPath(srcSymBinaryPath);
                var temp2 = System.IO.Path.Combine(System.IO.Path.GetFullPath(outputDir), System.IO.Path.GetFileName(srcSymBinaryPath));
                if (temp1 == temp2)
                {
                    Console.Error.WriteLine("入力バイナリのパスと出力先のパスが同一です.");
                    return (int)ErrorCode.InvalidOutputPath;
                }
            }

            var isVerbose = (mode == "verbose");

            if (isVerbose)
            {
                Console.WriteLine("Resolve {0}", srcSymBinaryPath);
            }

            // 定義テーブルのロード
            var definitionTable = new SymbolDefinitionTable<OctDecoder>();
            {
                var filePath = "";
                try
                {
                    for (int i = 3; i < args.Length; ++i)
                    {
                        filePath = args[i];
                        definitionTable.Load(filePath);
                    }
                }
                catch (Exception e)
                {
                    Console.Error.WriteLine("\t{0}のロード時にエラーが発生しました", filePath);
                    Console.Error.WriteLine(e.Message);
                    Console.Error.WriteLine(e.Data);
                    return (int)ErrorCode.FileReadError;
                }
            }

            // 参照テーブルファイルから参照情報を取得
            var refTable = new SymbolReferenceTable();
            try
            {
                refTable.Load(refSymbolFilePath);
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("\t{0}のロード時にエラーが発生しました", refSymbolFilePath);
                Console.Error.WriteLine(e.Message);
                Console.Error.WriteLine(e.Data);
                return (int)ErrorCode.FileReadError;
            }

            // シンボリックバイナリの内容を編集用ストリームにコピー
            var distStream = new System.IO.MemoryStream((int)(new System.IO.FileInfo(srcSymBinaryPath)).Length);
            using (var srcStream = new System.IO.FileStream(srcSymBinaryPath, System.IO.FileMode.Open))
            {
                srcStream.CopyTo(distStream);
            }

            // シンボル解決
            int resolveAmount = 0;
            int missAmount = 0;
            int missMatchAmount =  0;
            refTable.Foreach(
                (record) =>
                {
                    try
                    {
                        // 定義テーブル参照
                        var data = definitionTable.Search(record.symbolName);

                        // 定義データのサイズと参照データのサイズに齟齬があった場合はエラー                     
                        if (data.Length != record.size)
                        {
                            Console.Error.WriteLine("\t\t{0}の定義サイズ({1}byte)と参照サイズ({2}byte)に齟齬があります", record.symbolName, data.Length, record.size);
                            ++missMatchAmount;
                            return;
                        }

                        // 置換
                        // Console.WriteLine("\t0x{0:X4}に{1}({2}バイト)をライト", record.start_pos, String.Join(",", data), record.size);
                        distStream.Seek(record.start_pos, System.IO.SeekOrigin.Begin);
                        distStream.Write(data, 0, (int)record.size);

                        ++resolveAmount;
                    }
                    catch (KeyNotFoundException)
                    {
                        Console.Error.WriteLine("\tシンボル {0} の定義が存在しません", record.symbolName);
                        if (record.comment != null)
                        {
                            Console.Error.WriteLine("\t\tコメント:{0}", record.comment);
                        }
                        ++missAmount;
                    }
                }
            );

            if (missMatchAmount > 0)
            {
                Console.Error.WriteLine("\tFailed.");
                return (int)ErrorCode.MissMatchDataSize;
            }

            if (isVerbose)
            {
                Console.WriteLine("\tResolve {0} Symbols (Missing {1} Symbols)", resolveAmount, missAmount);
            }

            // 1つでもシンボル解決に失敗していたら異常終了扱い
            if (missAmount > 0)
            {
                Console.Error.WriteLine("\tFailed. Some Missing Symbol Definitions.");
                return (int)ErrorCode.LackOfDefinitions;
            }

            // 全てのシンボルを解決できたら新しいファイルに書き込み
            var targetBinaryFilePath = System.IO.Path.Combine(outputDir, System.IO.Path.GetFileName(srcSymBinaryPath));
            using (var targetBinaryFile = new System.IO.FileStream(targetBinaryFilePath, System.IO.FileMode.Create))
            {
                distStream.WriteTo(targetBinaryFile);
            }
            if (isVerbose)
            {
                Console.WriteLine("\t -> {0}", targetBinaryFilePath);
            }

            return 0;
        }
    }
}
