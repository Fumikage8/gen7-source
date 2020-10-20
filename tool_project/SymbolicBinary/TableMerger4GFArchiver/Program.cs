using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TableMerger4GFArchiver
{
    /// <summary>
    /// TableMerger4BGFArchiver.exe [バイナリ.garc]
    /// </summary>
    class Program
    {
        static int Main(string[] args)
        {
            var targetFilePath = args[0];

            // GASCからオフセット情報取得
            var offsetDictionary = GetOffsetDictionary(System.IO.Path.ChangeExtension(targetFilePath, "gasc"));

            var res = SymbolTableMergerLib.SymbolTableMerger.MergeSymbolTableFiles(
                System.IO.Path.ChangeExtension(targetFilePath, ".garc.symbol"),
                offsetDictionary
            );

            Console.WriteLine("== Merge Symbols ===========================================");
            foreach (var src in res.srcFiles)
            {
                Console.WriteLine(" {0}", src);
            }
            Console.WriteLine(" -> {0}", System.IO.Path.GetFullPath(res.outFile));
            Console.WriteLine("============================================================");

            return 0;
        }


        static Dictionary<string, uint> GetOffsetDictionary(string gascPath)
        {
            var res = new Dictionary<string, uint>();

            using (var file = new System.IO.FileStream(gascPath, System.IO.FileMode.Open, System.IO.FileAccess.Read))
            {
                var bytes = new byte[256];

                file.Read(bytes, 0, 4);
                if (CheckMagicNumber(bytes, 0) == false)
                {
                    throw new ArgumentException(gascPath+" is not GASCFile.");
                }

                file.Seek(8, System.IO.SeekOrigin.Begin);
                file.Read(bytes, 0, 4);
                var dat_amount = BitConverter.ToUInt32(bytes, 0);

                var dat_offset = new uint[dat_amount];

                // (言語差分を除く)ファイル単位のデータのオフセットを取得する
                for (int i = 0; i < dat_amount; ++i)
                {
                    file.Read(bytes, 0, 4);
                    dat_offset[i] = BitConverter.ToUInt32(bytes, 0);
                }

                // 実行パスの取得
                file.Read(bytes, 0, (int)(dat_offset[0] - file.Position));
                var rootPath = new Uri(Encoding.UTF8.GetString(bytes).Split('\0')[0]+System.IO.Path.DirectorySeparatorChar);

                var file_data_start_offset = new List<int>();

                for (int i = 0; i < dat_amount; ++i)
                {
                    file.Seek(dat_offset[i], System.IO.SeekOrigin.Begin);
                    file.Read(bytes, 0, 4);
                    var lang_amount = CountTrueBit(bytes, 0, 4);

                    file.Read(bytes, 0, 4 * lang_amount);
                    for (int j = 0; j < lang_amount; ++j)
                    {
                        file_data_start_offset.Add((int)BitConverter.ToUInt32(bytes, j * 4));
                    }
                }

                // ファイル末尾をダミーとして挿入
                file_data_start_offset.Add((int)file.Length);

                // ファイルパス -> オフセットの組を取得する
                for (int i = 0; i < file_data_start_offset.Count - 1; ++i)
                {
                    file.Seek(file_data_start_offset[i], System.IO.SeekOrigin.Begin);
                    file.Read(bytes, 0, 4);
                    var offset = BitConverter.ToUInt32(bytes, 0);
                    file.Read(bytes, 0, file_data_start_offset[i + 1] - file_data_start_offset[i] - 4);
                    var path = Encoding.UTF8.GetString(bytes).Split('\0')[0];

                    res.Add(new Uri(rootPath, path).LocalPath, offset);
                }
            }
            
            return res;
        }


        static bool CheckMagicNumber(byte[] bytes, uint offset)
        {
            return bytes[0 + offset] == 'G' && bytes[1 + offset] == 'A' && bytes[2 + offset] == 'S' && bytes[3 + offset] == 'C';
        }

        static int CountTrueBit(byte[] bitfield, int offset, int size)
        {
            int res = 0;

            for (int i = offset; i < offset + size; ++i)
            {
                for (int x = 1; x < byte.MaxValue; x *= 2)
                {
                    if ((bitfield[i] & x) > 0)
                    {
                        ++res;
                    }
                }
            }
            return res;
        }

    }
}
