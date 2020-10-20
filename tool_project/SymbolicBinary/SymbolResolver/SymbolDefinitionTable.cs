using System;
using System.Collections.Generic;

namespace SymbolResolver
{
    /// <remarks>
    //  エンコードキャッシュをシンボルテーブルに持たせるか、デコーダに持たせるか、悩ましい
    //
    // ・「複数シンボルが同一の値を持つ」
    // ・「同一シンボルにヒットする回数が少ない」
    // 以上の条件をみたす場合にのみ、デコーダに持たせたほうが良いはず
    // </remarks>

    /// <summary>
    /// シンボル名 -> 値(バイト列)のハッシュテーブル
    /// </summary>
    public class SymbolDefinitionTable<TDecoder> 
        where TDecoder : IBinaryDecoder, new()
    {
        /// <summary>
        /// 定義に対する値を格納する
        /// デコード処理回数を減らすため、一度デコードした値をキャッシュするための領域付き
        /// </summary>
        public class Record
        {
            /// <summary>エンコード済み文字列</summary>
            public string encordedStr;
            /// <summary>デコード済みバイナリ</summary>
            public byte[] bytes;

            public Record(string str)
            {
                this.encordedStr = str;
                this.bytes = null;
            }
        }

        /// <summary>
        /// 定義テーブル本体
        /// </summary>
        public Dictionary<string, Record> DefineRecordTable{get; private set;}

        /// <summary>
        /// 「文字列 -> バイト列」のデコーダインスタンス
        /// </summary>
        IBinaryDecoder decoder;

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="decoder">「シンボル名 -> バイト列」のデコーダ</param>
        public SymbolDefinitionTable()
        {
            this.decoder = new TDecoder();
            this.DefineRecordTable = new Dictionary<string, Record>();
        }

        /// <summary>
        /// ファイルからシンボル定義を読み込む
        /// </summary>
        /// <param name="file_path">読み込むファイルのパス</param>
        /// <exception cref="System.ArgumentNullException"></exception>
        /// <exception cref="System.ArgumentException"></exception>
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
            var colums = line.Split(',');
            if (colums[0] != SymbolRecordType.Define)
            {
                return;
            }

            try
            {
                this.DefineRecordTable.Add(colums[1], new Record(colums[2]));
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("シンボル[" + colums[1] + "]が重複しています.");
                throw e;
            }
        }


        /// <summary>
        /// シンボルの値を取得する
        /// </summary>
        /// <param name="symbolName">シンボル名</param>
        /// <returns>シンボルに対応する値(バイト列)</returns>
        /// <exception cref="System.ArgumentException"></exception>
        /// <exception cref="System.Collections.Generic.KeyNotFoundException">レコードが存在しなかった場合</exception>
        public byte[] Search(string symbolName)
        {
            var record = this.DefineRecordTable[symbolName];

            // デコードされていなければデコードしキャッシュ
            if (record.bytes == null)
            {
                record.bytes = this.decoder.Decode(record.encordedStr);                
            }

            return record.bytes;
        }

        /// <summary>
        /// 定義情報を出力する
        /// </summary>
        public void DumpRecord()
        {
            foreach (var pair in this.DefineRecordTable)
            {
                Console.WriteLine(pair.Key + "\t=>\t" + pair.Value.encordedStr);
            }
        }

    }

}
