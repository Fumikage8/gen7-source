using System;
using System.Collections.Generic;

namespace SymbolTableMergerLib
{
    /// <summary>
    /// シンボルテーブルファイルをマージし新しいシンボルテーブルファイルを生成するクラス
    /// </summary>
    public class SymbolTableMerger : IDisposable
    {
        System.IO.StreamWriter outFile;

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="outFilePath">生成するシンボルテーブルファイルのパス</param>
        public SymbolTableMerger(string outFilePath)
        {
            this.outFile = new System.IO.StreamWriter(outFilePath);
        }

        /// <summary>
        /// IDisosableインタフェースの実装
        /// </summary>
        public void Dispose()
        {
            outFile.Close();
        }

        /// <summary>
        /// ファイルの追加マージ
        /// scrFilePathの各レコードを追加する。
        /// レコード内のアドレス値にはoffsetが加算される
        /// </summary>
        /// <param name="srcFilePath">入力ファイル</param>
        /// <param name="offset">アドレスのオフセット</param>
        public void MergeSymbolTableFile(string srcFilePath, uint offset)
        {
            using (var srcFile = new System.IO.StreamReader(srcFilePath))
            {
                string line = null;
                while ((line = srcFile.ReadLine()) != null)
                {
                    if (line.Length == 0)
                    {
                        continue;
                    }

                    var columns = line.Split(',');

                    switch (columns[0])
                    {
                        case "D":
                            outFile.WriteLine(line);
                            break;
                        case "R":
                            columns[1] = (uint.Parse(columns[1], System.Globalization.NumberStyles.AllowHexSpecifier) + offset).ToString("x8");
                            this.outFile.WriteLine(String.Join(",", columns));
                            break;
                        default:
                            Console.Error.WriteLine("不正な行です.\nFile={0}\n{1}", srcFilePath, line);
                            throw new Exception();
                    }

                }
            }
        }

        public class MergeResult
        {
            public List<string> srcFiles = new List<string>();
            public string outFile = null;
        };

        /// <summary>
        /// ファイルのマージ
        /// </summary>
        /// <param name="outFilePath">出力パス</param>
        /// <param name="offsetArray">ファイルオフセットの配列</param>
        /// <param name="srcFilePathArray">入力ファイルの配列</param>
        /// <returns></returns>
        public static MergeResult MergeSymbolTableFiles(string outFilePath, uint[] offsetArray, string[] srcFilePathArray)
        {
            // 前提 : オフセット配列とファイルパス配列は長さが同じ
            if (offsetArray.Length != srcFilePathArray.Length)
            {
                throw new ArgumentException();
            }

            var res = new MergeResult();

            using (var outFile = new SymbolTableMerger(outFilePath))
            {
                for (int i = 0; i < offsetArray.Length; ++i)
                {
                    // 対応するシンボルテーブルファイルが存在する場合、オフセットを加味して内容をマージ
                    var symbolFilepath = srcFilePathArray[i] + ".symbol";
                    if (System.IO.File.Exists(symbolFilepath))
                    {                        
                        outFile.MergeSymbolTableFile(symbolFilepath, offsetArray[i]);
                        res.srcFiles.Add(symbolFilepath);
                    }
                }
            }
            res.outFile = outFilePath;
            return res;
        }

        /// <summary>
        /// ファイルのマージ
        /// </summary>
        /// <param name="outFilePath">出力パス</param>
        /// <param name="offsetDictionary">パス -> オフセットのハッシュテーブル</param>
        /// <returns></returns>
        public static MergeResult MergeSymbolTableFiles(string outFilePath, Dictionary<string, uint> offsetDictionary)
        {
            var res = new MergeResult();

            using (var outFile = new SymbolTableMerger(outFilePath))
            {
                foreach (var pair in offsetDictionary)
                {
                    // 対応するシンボルテーブルファイルが存在する場合、オフセットを加味して内容をマージ
                    var symbolFilepath = pair.Key + ".symbol";
                    if (System.IO.File.Exists(symbolFilepath))
                    {
                        outFile.MergeSymbolTableFile(symbolFilepath, pair.Value);
                        res.srcFiles.Add(symbolFilepath);
                    }
                }

            }
            res.outFile = outFilePath;
            return res;
        }


    }
}
