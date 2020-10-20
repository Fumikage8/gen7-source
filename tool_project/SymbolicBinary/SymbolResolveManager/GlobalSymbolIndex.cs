using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;

namespace SymbolResolveManager
{
    /// <summary>
    /// 依存関係の存在する範囲内でのファイルとシンボルの関係を記録したファイル
    /// 「グローバルシンボルインデックス」を扱うクラス
    /// </summary>
    class GlobalSymbolIndex
    {
        /// <summary>
        /// 削除された場合など、アクセス不可能なパスを指す文字列
        /// </summary>
        const string InvalidPath = "****";

        /// <summary>
        /// 読み込んでいるgsiファイルのパス
        /// </summary>
        string srcFilePath;

        /// <summary>
        /// ドキュメント操作クラス
        /// </summary>
        XDocument substance;

        // 処理負荷軽減のためにノードをキャッシング
        XElement symbolListNode;
        XElement fileListNode;

        /// <summary>
        /// 更新され、解決が必要なシンボルの集合
        /// </summary>
        HashSet<string> markedSymbolNames;

        /// <summary>
        /// シンボル毎の変更状況を記録する
        /// </summary>
        Dictionary<string, int> defChecker;

        /// <summary>
        /// .gsiファイルを読み込む
        /// </summary>
        /// <param name="filePath"></param>
        /// <param name="version"></param>
        /// <param name="truncate"></param>
        public GlobalSymbolIndex(string filePath, string version, bool truncate = false)
        {
            this.markedSymbolNames = new HashSet<string>();
            this.defChecker = new Dictionary<string, int>();

            this.srcFilePath = filePath;

            // ファイルが存在しなけれ新たに作成
            if (truncate || System.IO.File.Exists(filePath) == false)
            {
                this.substance = XDocument.Parse("<Document><Version>"+version+"</Version><SymbolList></SymbolList><FileList></FileList></Document>");
            }
            else
            {
                this.substance = XDocument.Load(filePath);
            }

            var v_node = this.substance.XPathSelectElement("Document/Version");
            if (v_node ==null || version != v_node.Value.Trim())
            {
                throw new Exception("SymbolResolverManagerとgsiファイルのバージョンが一致していません.");
            }

            this.symbolListNode = this.substance.XPathSelectElement("Document/SymbolList");
            this.fileListNode = this.substance.XPathSelectElement("Document/FileList");
        }

        #region 差分チェック・GSI変更

        /// <summary>
        /// 指定したファイルが「削除」されたものとし、
        /// 関連付けられていたシンボル定義の削除とシンボル参照の除去を実行する
        /// </summary>
        /// <param name="symbolFilePath"></param>
        public void RemoveFilePath(string symbolFilePath)
        {
            var targetNode = this.GetFileNode(symbolFilePath);

            // シンボル定義の削除
            foreach (var definedSymbol in targetNode.XPathSelectElements("DefineSymbolList/Symbol"))
            {
                // SymbolInfoに反映
                this.ChangeDefineAt(
                    definedSymbol.Attribute("name").Value,
                    symbolFilePath,
                    true
                    );
            }

            // シンボル参照からの除去
            foreach (var refedSymbol in targetNode.XPathSelectElements("ReferenceSymbolList/Symbol"))
            {
                this.RemoveReferenceAtNodeFromSymbolInfo(
                    refedSymbol.Attribute("name").Value,
                    symbolFilePath
                    );
            }

            // ファイルノードそのものを削除
            targetNode.Remove();
        }

        /// <summary>
        /// 各シンボルテーブルファイルとGSIの差分比較による更新
        /// </summary>
        /// <param name="symbolTable"></param>
        public void UpdateFileInfo(SymbolResolver.SymbolTable symbolTable)
        {
            var fileNode = this.GetFileNode(symbolTable.FilePath);

            // シンボル定義情報の更新
            this.UpdateDefinition(fileNode, symbolTable.FilePath, symbolTable.DefinedSymbols);

            // シンボル参照情報の更新
            this.UpdateReference(fileNode, symbolTable.FilePath, symbolTable.ReferencedSymbols.Select((item) => item.symbolName));

        }

        /// <summary>
        /// 「定義しているシンボル」を更新する
        /// </summary>
        private void UpdateDefinition(XElement fileNode, string filePath, Dictionary<string, SymbolResolver.SymbolDefinitionTable<SymbolResolver.OctDecoder>.Record> definedSymbolList)
        {
            // GSIの情報との差分を抽出
            var remainSymbol = new HashSet<string>();
            var defineNode = fileNode.XPathSelectElement("DefineSymbolList");
            foreach (var sym in defineNode.XPathSelectElements("Symbol"))
            {
                var symKey = sym.Attribute("name").Value;

                // 削除されているか？
                if (definedSymbolList.ContainsKey(symKey) == false)
                {
                    // 削除をSymbolInfoに反映
                    this.ChangeDefineAt(symKey, filePath, true);

                    continue;
                }

                // 値が変わっているか?
                if (definedSymbolList[symKey].encordedStr != sym.Attribute("value").Value)
                {
                    this.markedSymbolNames.Add(symKey);
                }

                remainSymbol.Add(symKey);
            }

            // 一旦全消し
            defineNode.RemoveNodes();

            // 今回のファイルに含まれている分を新たに設定しなおし
            foreach (var sym in definedSymbolList)
            {
                // 今回追加されるノード
                if (remainSymbol.Contains(sym.Key) == false)
                {
                    this.markedSymbolNames.Add(sym.Key);

                    // Document/SymbolList以下に反映
                    this.ChangeDefineAt(sym.Key, filePath, false);
                }

                // Document/FileList以下に反映
                var node = new XElement("Symbol");
                node.SetAttributeValue("name", sym.Key);
                node.SetAttributeValue("value", sym.Value.encordedStr);


                defineNode.Add(node);
            }
        }

        /// <summary>
        /// 「参照しているシンボル」を更新する
        /// </summary>
        private void UpdateReference(XElement fileNode, string filePath, IEnumerable<string> referencedSymbolList)
        {
            // GSIの情報との差分を抽出
            var existInPreb = new HashSet<string>();
            var removeSymbol = new HashSet<XElement>();
            foreach (var sym in fileNode.XPathSelectElements("ReferenceSymbolList/Symbol"))
            {
                if (referencedSymbolList.Contains(sym.Attribute("name").Value))
                {
                    existInPreb.Add(sym.Attribute("name").Value);
                }
                else
                {
                    removeSymbol.Add(sym);
                }
            }
            var addSymbol = referencedSymbolList.Except(existInPreb);

            // 参照の削除を反映
            foreach (var sym in removeSymbol)
            {
                // Document/SymbolList以下に反映
                this.RemoveReferenceAtNodeFromSymbolInfo(sym.Attribute("name").Value, filePath);
                // Document/FileList以下に反映
                sym.Remove();
            }

            // 参照の追加を反映
            foreach (var sym in addSymbol)
            {
                // Document/SymbolList以下に反映
                var node = new XElement("File");
                node.SetAttributeValue("path", filePath);
                this.GetSymbolInfoNode(sym).XPathSelectElement("ReferenceAt").Add(node);

                // Document/FileList以下に反映
                node = new XElement("Symbol");
                node.SetAttributeValue("name", sym);
                fileNode.XPathSelectElement("ReferenceSymbolList").Add(node);
            }
        }

        /// <summary>
        /// シンボル定義の変更の整合性チェック
        /// </summary>
        public void ValidateDefinitionChange()
        {
            var errorInfos = new HashSet<SymbolDefineException.Info>();

            // 2以上はNG
            // 0は参照が存在しないことを確認
            foreach (var pair in defChecker)
            {
                if (pair.Value > 1)
                {
                    // 重複
                    errorInfos.Add(new SymbolDefineException.Info(null, pair.Key, SymbolDefineException.Info.Types.Duplicate));
                }
                else if (pair.Value == 0)
                {
                    var symNode = this.GetSymbolInfoNode(pair.Key);

                    // 参照が消えてなければexception

                    var refNodes = symNode.XPathSelectElements("ReferenceAt/File");
                    if (refNodes.Count() > 0)
                    {
                        var refFilePaths = refNodes.Select(node => node.Attribute(XName.Get("path")).Value).ToArray();
                        errorInfos.Add(new SymbolDefineException.Info(String.Join(System.Environment.NewLine, refFilePaths), pair.Key, SymbolDefineException.Info.Types.Removed));
                        continue;
                    }

                    // 消えていればシンボルノードそのものを消す
                    symNode.Remove();
                }
            }

            if (errorInfos.Count() > 0)
            {
                throw new SymbolDefineException(errorInfos);
            }
        }

        #endregion

        #region シンボル解決の依存関係出力

        /// <summary>
        /// markedSymbolListの内容を基にシンボル解決が必要なファイルを返す
        /// </summary>
        /// <returns></returns>
        public IEnumerable<string> GetFilesRequireResolve()
        {
            var binaryFileList = new HashSet<string>();

            foreach(var sym_name in this.markedSymbolNames)
            {            
                foreach (var item in this.GetSymbolInfoNode(sym_name).XPathSelectElements("ReferenceAt/File").Attributes("path"))
                {
                    binaryFileList.Add(item.Value);
                }
            }

            return binaryFileList;
        }

        /// <summary>
        /// 依存するシンボルファイルの取得
        /// </summary>
        /// <param name="binaryFileList"></param>
        /// <returns>
        /// キー : シンボリックファイルのパス
        /// 値   : 依存するシンボルテーブルの配列
        /// </returns>
        public IEnumerable<KeyValuePair<string, IEnumerable<string>>> GetResolvePair(IEnumerable<string> binaryFileList)
        {
            var result = new HashSet<KeyValuePair<string, IEnumerable<string>>>();

            var missingInfoSet = new HashSet<SymbolDefineException.Info>();

            foreach (var filePath in binaryFileList)
            {
                var node = this.GetFileNode(filePath+".symbol");

                var depends = new HashSet<string>();

                bool isOk = true;

                foreach (var symbol in node.XPathSelectElements("ReferenceSymbolList/Symbol"))
                {
                    var defineAtAttr = this.GetSymbolInfoNode(symbol.Attribute("name").Value).Attribute("defineAt");
                    if (defineAtAttr == null || defineAtAttr.Value == InvalidPath)
                    {
                        missingInfoSet.Add(new SymbolDefineException.Info(filePath, symbol.Attribute("name").Value, SymbolDefineException.Info.Types.Missing));
                        isOk = false;
                        continue;
                    }
                    depends.Add(defineAtAttr.Value);
                }

                if (isOk)
                {
                    result.Add(new KeyValuePair<string, IEnumerable<string>>(filePath, depends));
                }
            }

            if (missingInfoSet.Count > 0)
            {
                throw new SymbolDefineException(missingInfoSet);
            }

            return result;
        }

        #endregion

        #region ノード操作ヘルパ

        /// <summary>
        /// Document/SymbolList以下の対応するシンボルの情報を取得する
        /// 存在しなければ新たに生成する
        /// </summary>
        /// <param name="symbolName"></param>
        /// <returns></returns>
        private XElement GetSymbolInfoNode(string symbolName)
        {
            var res = this.symbolListNode.XPathSelectElement("Symbol[@name=\"" + symbolName + "\"]");

            if (res == null)
            {
                res = XElement.Parse("<Symbol><ReferenceAt/></Symbol>");
                res.SetAttributeValue("name", symbolName);
                this.symbolListNode.Add(res);
            }

            return res;
        }

        /// <summary>
        /// Document/FileList以下の対応するファイルの情報を取得する
        /// 存在しなければ新たに生成する
        /// </summary>
        /// <param name="filePath"></param>
        /// <returns></returns>
        private XElement GetFileNode(string filePath)
        {
            var res = this.fileListNode.XPathSelectElement("File[@path=\"" + filePath + "\"]");

            if (res == null)
            {
                res = XElement.Parse("<File><ReferenceSymbolList/><DefineSymbolList/></File>");
                res.SetAttributeValue("path", filePath);
                this.fileListNode.Add(res);
            }

            return res;
        }

        /// <summary>
        /// SymbolInfoノードの定義元情報を変更する
        /// </summary>
        /// <param name="key"></param>
        /// <param name="newPath"></param>
        /// <param name="isRemove"></param>
        private void ChangeDefineAt(string key, string newPath, bool isRemove)
        {
            var node = this.GetSymbolInfoNode(key);

            var prevPath = node.Attribute("defineAt");

            // defCheckerにkeyが存在しない = 初めての変更操作
            // prevの値がファイルに記述されている内容なので、
            // その値を元にdefChecker[key]の初期値を決定する
            int prevChecker = 0;
            if (this.defChecker.TryGetValue(key, out prevChecker) == false)
            {
                prevChecker = (prevPath == null || prevPath.Value == InvalidPath) ? 0 : 1;
            }

            if (isRemove)
            {
                // 削除の場合
                // 常に-1
                // 以前のパスと一致していればパスをInvalidに
                this.defChecker[key] = prevChecker - 1;
                if (prevPath.Value == newPath)
                {
                    node.SetAttributeValue("defineAt", InvalidPath);
                }
            }
            else
            {
                // 追加の場合
                // newPathとprevPathの値が違えば +1
                // 常にパスを書き換える
                if (prevPath == null || prevPath.Value != newPath)
                {
                    this.defChecker[key] = prevChecker + 1;
                }
                node.SetAttributeValue("defineAt", newPath);
            }
        }

        /// <summary>
        /// SymbolInfo以下の特定のシンボル情報の参照先から指定のファイルパスを取り除く
        /// </summary>
        /// <param name="symbolName"></param>
        /// <param name="filePath"></param>
        private void RemoveReferenceAtNodeFromSymbolInfo(string symbolName, string filePath)
        {
            this.GetSymbolInfoNode(symbolName).XPathSelectElement("ReferenceAt/File[@path=\"" + filePath + "\"]").Remove();
        }

        #endregion

        /// <summary>
        /// gsiファイルを現在の内容で更新する
        /// </summary>
        public void Save()
        {
            this.substance.Save(this.srcFilePath);
        }

        /// <summary>
        /// 現在のGSIの状態をダンプした文字列を取得する
        /// 主にデバッグ用
        /// </summary>
        /// <returns></returns>
        public string Dump()
        {
            return this.substance.ToString();
        }

    }


    class SymbolDefineException : Exception
    {
        public class Info
        {
            public enum Types { Duplicate, Removed, Missing }

            public string FilePath { get; private set; }
            public string Symbol { get; private set; }

            public Types Type {get; private set;}


            public Info(string filePath, string symbol, Types type)
            {
                this.FilePath = filePath;
                this.Symbol = symbol;

                this.Type = type;
            }
        }

        public HashSet<Info> ErrorInfo { get; private set; }

        public SymbolDefineException(HashSet<Info> infoSet)
        {
            this.ErrorInfo = infoSet;
        }

    }


}
