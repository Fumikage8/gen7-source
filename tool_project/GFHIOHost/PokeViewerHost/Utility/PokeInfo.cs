using System;
using System.Data;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Xml;
using System.Xml.Serialization;

namespace PokeViewerHost.Utility
{
    using PokeInfoColumnPair = KeyValuePair<string, Type>;


    /// <summary>
    ///   
    /// </summary>
    public class PokeInfo
    {
        public int Index { get; set; }
        public int MonsNo { get; set; }
        public string Name { get; set; }
        public int FormNo { get; set; }
        public string GenderString { get; set; }
        public bool IsRare { get; set; }

        public int GenderInt { get { return ParseGenderString(GenderString); } }
        public int IsRareInt { get { return IsRare ? 1 : 0; } }


        public static PokeInfoColumnPair Pair_Index { get { return new PokeInfoColumnPair(@"index", typeof(int)); } }
        public static PokeInfoColumnPair Pair_CaptureState { get { return new PokeInfoColumnPair(@"capture", typeof(string)); } }
        public static PokeInfoColumnPair Pair_MonsNo { get { return new PokeInfoColumnPair(@"monsno", typeof(int)); } }
        public static PokeInfoColumnPair Pair_Name { get{return new PokeInfoColumnPair(@"name", typeof(string));} }
        public static PokeInfoColumnPair Pair_FormNo { get{return new PokeInfoColumnPair(@"formno", typeof(int));} }
        public static PokeInfoColumnPair Pair_Gender { get { return new PokeInfoColumnPair(@"gender", typeof(string)); } }
        public static PokeInfoColumnPair Pair_IsRare { get { return new PokeInfoColumnPair(@"rarity", typeof(bool)); } }

        protected static string GenderLetter_Male { get { return @"m"; } }
        protected static string GenderLetter_Female { get { return @"f"; } }
        protected static string GenderLetter_Same { get { return @"s"; } }
        protected static string GenderLetter_Unknown { get { return @"u"; } }

        protected static string GenderString_Male { get { return @"male"; } }
        protected static string GenderString_Female { get { return @"female"; } }
        protected static string GenderString_Same { get { return @"same"; } }
        protected static string GenderString_Unknown { get { return @"unknown"; } }

        public static string RarityLetter_Rare { get { return @"r"; } }
        public static string RarityLetter_Normal { get { return @"n"; } }

        public static string RarityString_Rare { get { return @"rare"; } }
        public static string RarityString_Normal { get { return @"normal"; } }

        public static int GenderInt_Male { get { return 0; } }
        public static int GenderInt_Female { get { return 1; } }

        public static string DefaultMakeFileNameFormat { get { return @"Capture_{1:000}_{3:00}_{5}_{7}"; } }

        /*   capture state   */
        public static string CaptureStateString_Nothing { get { return @"×"; } }
        public static string CaptureStateString_Captured { get { return @"○"; } }

        /*
         * 
         * */
        public static PokeInfo CreateFromTableRow(DataRow row)
        {
            PokeInfo info = new PokeInfo();

            try
            {
                info.Index = (int)row[Pair_Index.Key];
                info.MonsNo = (int)row[Pair_MonsNo.Key];
                info.Name = (row[Pair_Name.Key].GetType() == typeof(DBNull)) ? @"" : (string)row[Pair_Name.Key];
                info.FormNo = (int)row[Pair_FormNo.Key];
                info.GenderString = (string)row[Pair_Gender.Key];
                info.IsRare = (bool)row[Pair_IsRare.Key];
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
                info = null;
            }

            return info;
        }



        /*
         * 
         * */
        public string MakeFileName()
        {
            return MakeFileName(DefaultMakeFileNameFormat);
        }



        public static string FormatMacroDescription
        {
            get
            {
                return
                    @"{0}:index" + Environment.NewLine +
                    @"{1}:MonsNo" + Environment.NewLine +
                    @"{2}:Name" + Environment.NewLine +
                    @"{3}:FormNo" + Environment.NewLine +
                    @"{4}:GenderLetter" + Environment.NewLine +
                    @"{5}:GenderString" + Environment.NewLine +
                    @"{6}:RarityLetter" + Environment.NewLine +
                    @"{7}:RarityString" + Environment.NewLine
                    ;
            }
        }

        /// <summary>
        ///   formatに従いファイル名を生成する
        ///   展開パラメータは次の通り
        ///     {0}:index
        ///     {1}:MonsNo
        ///     {2}:Name
        ///     {3}:FormNo
        ///     {4}:GenderLetter
        ///     {5}:GenderString
        ///     {6}:RarityLetter
        ///     {7}:RarityString
        /// </summary>
        /// 
        public string MakeFileName(string format)
        {
            /*
            string genderLetter = genderString.Equals(@"male") ? @"m" : (genderString.Equals(@"female") ? @"f" : (genderString.Equals(@"same") ? @"s" : @"n"));
            string bmpName = string.Format(@"zukan_poke_pic_{0:000}_{1:00}_{2}_{3}_front.bmp", monsNo, formNo, genderLetter, isRare ? @"r" : @"n");
             * */

            return string.Format(
                format,
                Index,
                MonsNo,
                Name,
                FormNo,
                GetGenderLetter(GenderString),
                GenderString,
                GetRarityLetter(IsRare),
                GetRarityString(IsRare)
                );
        }


        public static string GetRarityLetter(bool isRare)
        {
            return isRare ? RarityLetter_Rare : RarityLetter_Normal;
        }

        public static string GetRarityString(bool isRare)
        {
            return isRare ? RarityString_Rare : RarityString_Normal;
        }

        public static string GetGenderString(string genderLetter)
        {
            if (genderLetter.Equals(GenderLetter_Male)) return GenderString_Male;
            else if (genderLetter.Equals(GenderLetter_Female)) return GenderString_Female;
            else if (genderLetter.Equals(GenderLetter_Same)) return GenderString_Same;
            else return GenderString_Unknown;
        }

        public static string GetGenderLetter(string genderString)
        {
            if (genderString.Equals(GenderString_Male)) return GenderLetter_Male;
            else if (genderString.Equals(GenderString_Female)) return GenderLetter_Female;
            else if (genderString.Equals(GenderString_Same)) return GenderLetter_Same;
            else return GenderLetter_Unknown;
        }

        /// <summary>
        ///   genderStringを数値化
        /// </summary>
        public static int ParseGenderString(string genderString)
        {
            return (genderString.Equals(GenderString_Female) ? GenderInt_Female : GenderInt_Male);
        }
    }


    /// <summary>
    ///   PokeInfo用DataTable
    /// </summary>
    public class PokeInfoTable
    {
        public DataTable DataTable { get; set; }
        public Hashtable NameList { get; set; }

        /// <summary>
        ///   pokemonの名前辞書をxmlから生成する
        /// </summary>
        public static Hashtable CreateNameListFromXml(string path)
        {
            Hashtable hashTable = null;
            XmlSerializer xml = new XmlSerializer(typeof(PokeNameInfoList));

            using (StreamReader reader = new StreamReader(path))
            {
                PokeNameInfoList nameInfoList = xml.Deserialize(reader) as PokeNameInfoList;

                hashTable = new Hashtable();

                foreach (PokeNameInfo nameInfo in nameInfoList.NameInfos)
                {
                    hashTable[nameInfo.MonsNo] = nameInfo.Name;
                }
            }

            return hashTable;
        }

        /// <summary>
        ///   pokemonの名前辞書をxmlから生成、設定する
        /// </summary>
        public void SetupNameListFromXml(string path)
        {
            NameList = CreateNameListFromXml(path);
        }


        /// <summary>
        ///   pokemonのデータテーブルをxmlから生成する
        /// </summary>
        public static DataTable CreateDataTableFromXml(string path)
        {
            DataTable table = CreateEmptyTable();

            table.ReadXml(path);

            return table;
        }


        /// <summary>
        ///   pokemonのデータテーブルをxmlから生成、設定する
        /// </summary>
        public void SetupDataTableFromXml(string path)
        {
            string nameColumnKey = PokeInfo.Pair_Name.Key;
            DataTable = CreateDataTableFromXml(path);
            foreach(DataRow row in DataTable.Rows)
            {
                PokeInfo info = PokeInfo.CreateFromTableRow(row);

                row[nameColumnKey] = ((NameList != null) && (NameList.ContainsKey(info.MonsNo))) ? NameList[info.MonsNo] : @"-unknown-";
            }
        }

        /// <summary>
        ///   
        /// </summary>
        public class PokeNameInfo
        {
            [XmlAttribute("MonsNo")]
            public int MonsNo { get; set; }

            [XmlAttribute("Name")]
            public string Name { get; set; }
        }

        /// <summary>
        ///   
        /// </summary>
        [XmlRoot("PokeNameInfoList")]
        public class PokeNameInfoList
        {
            [XmlElement("PokeNameInfo")]
            public List<PokeNameInfo> NameInfos { get; set; }
        }


        /// <summary>
        ///   空のPokeInfo用DataTableを作成する
        /// </summary>
        public static DataTable CreateEmptyTable()
        {
            DataTable table = new DataTable();

            table.TableName = @"PokeInfo";

            table.Columns.Add(PokeInfo.Pair_Index.Key, PokeInfo.Pair_Index.Value).ColumnMapping = MappingType.Attribute;
            table.Columns.Add(PokeInfo.Pair_CaptureState.Key, PokeInfo.Pair_CaptureState.Value).ColumnMapping = MappingType.Attribute;
            table.Columns.Add(PokeInfo.Pair_MonsNo.Key, PokeInfo.Pair_MonsNo.Value).ColumnMapping = MappingType.Attribute;
            table.Columns.Add(PokeInfo.Pair_Name.Key, PokeInfo.Pair_Name.Value).ColumnMapping = MappingType.Attribute;
            table.Columns.Add(PokeInfo.Pair_FormNo.Key, PokeInfo.Pair_FormNo.Value).ColumnMapping = MappingType.Attribute;
            table.Columns.Add(PokeInfo.Pair_Gender.Key, PokeInfo.Pair_Gender.Value).ColumnMapping = MappingType.Attribute;
            table.Columns.Add(PokeInfo.Pair_IsRare.Key, PokeInfo.Pair_IsRare.Value).ColumnMapping = MappingType.Attribute;


            table.Columns[@"index"].AutoIncrement = true;
            table.Columns[@"index"].AutoIncrementSeed = 0;
            table.Columns[@"index"].AutoIncrementStep = 1;


            return table;
        }

#if false
        public static DataTable CreateXmlFromText(string dstPath, string srcPath)
        {
            DataTable table = null;

            using (StreamReader reader = new StreamReader(srcPath))
            {
                table = CreateEmptyTable();
                string line;

                while ((line = reader.ReadLine()) != null)
                {
                    if(string.IsNullOrWhiteSpace(line)) continue;

                    string[] tokens = line.Split(',');

                    if(tokens.Length < 3)   continue;

                    /*   rows   */
                    {
                        DataRow row = table.Rows.Add();

                        row[@"monsno"] = int.Parse(tokens[0]);
                        row[@"name"] =  @"-unknown-";
                        row[@"formno"] = int.Parse(tokens[1]);
                        row[@"gender"]  = tokens[2];
                        row[@"rarity"]  = tokens[3].Equals(@"r");
                    }
                }
            }

            if (table == null) return null;



            {
                string[] ignoreColumns = new string[]
                                        {
                                            @"index",
                                            @"name",
                                        };

                MappingType[] mappingTypeCache = new MappingType[ignoreColumns.Length];


                /*   push   */
                for (int index = 0; index < ignoreColumns.Length; ++index )
                {
                    DataColumn  column  = table.Columns[ignoreColumns[index]];
                    mappingTypeCache[index] = column.ColumnMapping;
                    column.ColumnMapping = MappingType.Hidden;
                }


                table.WriteXml(dstPath);

                /*   pop   */
                for (int index = 0; index < ignoreColumns.Length; ++index)
                {
                    DataColumn column = table.Columns[ignoreColumns[index]];
                    column.ColumnMapping = mappingTypeCache[index];
                }
            }

            return table;
        }
#endif

    }
}
