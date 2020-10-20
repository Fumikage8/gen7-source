using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TableMerger4Binlinker
{
    /// <summary>
    /// TableMerger4Binlinker.exe [バイナリ.pack] [Binlinkerに渡したファイル...(順一致)]
    /// </summary>
    class Program
    {
        static int Main(string[] args)
        {
            try
            {
                var targetFilePath = args[0];
                string[] srcFilePathArray = new string[args.Length - 1];
                Array.Copy(args, 1, srcFilePathArray, 0, srcFilePathArray.Length);

                // アーカイブされたバイナリから
                var offsetArray = GetOffsetArray(targetFilePath);

                // シンボルテーブルファイルのマージ処理
                var res = SymbolTableMergerLib.SymbolTableMerger.MergeSymbolTableFiles(
                    targetFilePath + ".symbol",
                    offsetArray,
                    srcFilePathArray
                );

                Console.WriteLine("== Merge Symbols ===========================================");
                foreach (var src in res.srcFiles)
                {
                    Console.WriteLine(" {0}", src);
                }
                Console.WriteLine(" ->{0}", res.outFile);
                Console.WriteLine("============================================================");
            }
            catch (Exception e)
            {
                Console.Error.WriteLine(e);
                Console.Error.WriteLine(e.StackTrace);
                return -1;
            }

            return 0;
        }

        /// <summary>
        /// Binlinkerでアーカイブされたバイナリファイルのヘッダを解析し、
        /// </summary>
        /// <param name="packFilePath">対象のバイナリファイルのパス</param>
        /// <returns>収録されている各ファイルのバイナリ上での開始位置の配列(収録順)</returns>
        static public uint[] GetOffsetArray(string packFilePath)
        {
            uint[] offsetArray = null;

            using (var file = new System.IO.FileStream(packFilePath, System.IO.FileMode.Open, System.IO.FileAccess.Read))
            {
                var bytes = new byte[4];

                file.Read(bytes, 0, 4);
                var fileAmount = BitConverter.ToUInt16(bytes, 2);

                bytes = new byte[4 * fileAmount];

                file.Read(bytes, 0, 4 * fileAmount);

                offsetArray = new uint[fileAmount];

                for (int i = 0; i < fileAmount; ++i)
                {
                    offsetArray[i] = BitConverter.ToUInt32(bytes, i * 4);
                }
            }

            return offsetArray;
        }

    }

}
