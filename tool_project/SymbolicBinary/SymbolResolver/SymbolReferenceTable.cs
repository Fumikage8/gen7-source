using System;
using System.Collections.Generic;

namespace SymbolResolver
{
    /// <summary>
    /// シンボル参照テーブル
    /// </summary>
    public class SymbolReferenceTable
    {
        /// <summary>
        /// シンボル参照レコード
        /// </summary>
        public struct Record
        {
            /// <summary>置換開始アドレス</summary>
            public uint start_pos;
            /// <summary>置換サイズ</summary>
            public uint size;
            /// <summary>参照シンボル名</summary>
            public string symbolName;

            /// <summary>参照記録時に付記されたコメント</summary>
            public string comment;
        }

        /// <summary>参照テーブル本体</summary>
        public List<Record> RecordTable { get; private set; }

        /// <summary>
        /// コンストラクタ
        /// </summary>
        public SymbolReferenceTable()
        {
            this.RecordTable = new List<Record>();
        }

        /// <summary>
        /// ファイルから参照レコードを読み込む
        /// </summary>
        /// <param name="filePath">読み込むファイルのパス</param>
        public void Load(string filePath)
        {
            using (var file = new System.IO.StreamReader(filePath))
            {
                string line = "";
                while ((line = file.ReadLine()) != null)
                {
                    this.ParseLineAndAdd(line);
                }
            }
        }

        public void ParseLineAndAdd(string line)
        {
            var columns = line.Split(',');
            if (columns[0] != SymbolRecordType.Reference)
            {
                return;
            }

            this.RecordTable.Add(
                new Record
                {
                    start_pos = uint.Parse(columns[1], System.Globalization.NumberStyles.AllowHexSpecifier),
                    size = uint.Parse(columns[2], System.Globalization.NumberStyles.AllowHexSpecifier),
                    symbolName = columns[3],
                    comment = (columns.Length == 5) ? columns[4] : null
                }
            );
        }

        /// <summary>
        /// テーブル内の全てのレコードに対して指定の処理を実行する
        /// </summary>
        /// <param name="action"></param>
        public void Foreach(Action<Record> action)
        {
            foreach (var record in this.RecordTable)
            {
                action(record);
            }
        }

        /// <summary>
        /// 参照情報の出力(デバッグ用)
        /// </summary>
        public void DumpRecord()
        {
            foreach (var record in this.RecordTable) {
                Console.WriteLine("0x{0:X4}\t0x{1:X4}\t{2}", record.start_pos, record.size, record.symbolName);
            }
        }

    }
}
