using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SymbolResolver
{
    /// <summary>
    /// シンボルテーブルファイルへのアクセッサクラス
    /// </summary>
    public class SymbolTable
    {
        SymbolDefinitionTable<OctDecoder> definitionTable;
        SymbolReferenceTable referenceTable;

        public SymbolTable(string filePath, bool expandEnvVariables = false)
        {
            this.FilePath = filePath;

            this.definitionTable = new SymbolResolver.SymbolDefinitionTable<SymbolResolver.OctDecoder>();
            this.referenceTable = new SymbolResolver.SymbolReferenceTable();

            var path = (expandEnvVariables) ? Environment.ExpandEnvironmentVariables(filePath) : filePath;

            using (var file = new System.IO.StreamReader(path))
            {
                string line = "";
                while ((line = file.ReadLine()) != null)
                {
                    this.definitionTable.ParseLineAndAdd(line);
                    this.referenceTable.ParseLineAndAdd(line);
                }
            }
        }

        public Dictionary<string, SymbolResolver.SymbolDefinitionTable<SymbolResolver.OctDecoder>.Record> DefinedSymbols
        {
            get { return this.definitionTable.DefineRecordTable; }
        }

        public IEnumerable<SymbolResolver.SymbolReferenceTable.Record> ReferencedSymbols
        {
            get { return this.referenceTable.RecordTable; }
        }

        public string FilePath { get; private set; }
    }
}
