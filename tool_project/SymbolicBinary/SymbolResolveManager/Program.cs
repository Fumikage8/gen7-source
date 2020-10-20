using System;
using System.Linq;
using System.Collections.Generic;
using System.Xml.Linq;
using System.Xml.XPath;

namespace SymbolResolveManager
{
    /// <summary>
    /// 終了コードの定義
    /// </summary>
    enum ExitCode
    {
        Success = 0,
        Fail    = 1,
    }

    /// <summary>
    /// SymbolResolveManager自体の設定
    /// </summary>
    class Setting
    {
        public string[] EnvironmentVariables { get; private set; }

        public Setting(string path)
        {
            var xml = XElement.Load(path);

            this.EnvironmentVariables = xml.XPathSelectElements("ReplaceEnvironmentVariables/Variable").Select(node => node.Value.Trim()).ToArray();
        }
    }

    class Program
    {
        /// <summary>
        /// アプリケーションのVersion識別子
        /// GSIにも記録し、一致しなかった場合にはリセット(再生成)を促す
        /// </summary>
        const string Version = "2015_0206";

        const string SettingFilePath = "SymbolResolveManagerSetting.xml";

        /// <summary>
        /// シンボル解決後のバイナリの保存先ディレクトリの辞書
        /// キー : 解決前のファイルのディレクトリ
        /// 値   : 解決後のファイルのディレクトリ
        /// </summary>
        static Dictionary<string, string> outputDirDictionary;

        static int Main(string[] args)
        {
            // 標準出力への出力をUTF8に
            Console.OutputEncoding = System.Text.Encoding.UTF8;

            if (args.Length < 2)
            {
                Console.Error.WriteLine("引数が不正です。");
                Console.Error.WriteLine("SymbolResolveManager.exe [gsiファイルパス] [変更(削除)されたシンボルファイルパスが格納されたファイル]");

                return (int)ExitCode.Fail;
            }

            var gsiFilePath = args[0];

            string[] changeFiles = null;
            using (var fs = new System.IO.StreamReader(args[1]) )
            {
                var list = new List<string>();
                while(fs.EndOfStream == false)
                {
                    var line = fs.ReadLine();
                    if (string.IsNullOrWhiteSpace(line) == false)
                    {
                        list.Add(line);
                    }
                }
                changeFiles = list.ToArray();
            }

            outputDirDictionary = new Dictionary<string, string>();

            try
            {
                // 設定の読み込み
                var setting = new Setting(System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory,SettingFilePath));

                // gsiをファイルから読み込み
                var globalSymbolIndex = new GlobalSymbolIndex(gsiFilePath, Program.Version, false);

                // ファイルパスを環境変数を用いたものに置換
                changeFiles = changeFiles.Select(item => ReplaceWithEnvVariables(item, setting.EnvironmentVariables)).ToArray();

                // 解決が必要なファイルと関連するシンボルファイルを取得する
                var resolvePairList = MainRoutine(globalSymbolIndex, changeFiles);
                if (resolvePairList == null)
                {
                    return (int)ExitCode.Fail;
                }

                // SymbolResolver.exeを並列で実行
                var result = DispatchProcesses(resolvePairList);
                if (result == false)
                {
                    return (int)ExitCode.Fail;
                }

                Console.WriteLine("Resolved {0} Files.", resolvePairList.Count());

                // 失敗していなければ
                // gsiファイルを更新
                globalSymbolIndex.Save();

                return (int)ExitCode.Success;
            }
            catch (Exception e)
            {
                Console.Error.WriteLine(e.Message);
                Console.Error.WriteLine(e.StackTrace);
                Console.Error.WriteLine(e.Data);
                return (int)ExitCode.Fail;
            }
        }

        static string ReplaceWithEnvVariables(string str, string[] envVariables)
        {
            foreach (var variable in envVariables)
            {
                str = str.Replace(Environment.GetEnvironmentVariable(variable), "%" + variable + "%");
            }

            return str;
        }

        static IEnumerable<KeyValuePair<string, IEnumerable<string>>> MainRoutine(GlobalSymbolIndex globalSymbolIndex, string[] changedSymbolFilePaths)
        {
            var updatedSymbolFilePaths = new HashSet<string>();
            var deletedSymbolFilePaths = new HashSet<string>();

            // 「削除されたファイル」とそれ以外を仕分ける
            for (int i = 0; i < changedSymbolFilePaths.Length; ++i)
            {
                // 環境によってはパス文字列に\r等の文字が付与されてしまうのでサニタイズしつつ、
                // ファイルパスとステータスを分離
                var info = changedSymbolFilePaths[i].Trim().Split('|');

                if (info[0] == "!")
                {
                    deletedSymbolFilePaths.Add(info[1]);
                }
                else
                {
                    updatedSymbolFilePaths.Add(info[1]);
                }
            }

            #region 修正対象処理
            /*
            // シンボルファイルの削除をgsiに反映
            foreach (var path in deletedSymbolFilePaths)
            {
                globalSymbolIndex.RemoveFilePath(path);
            }

            // 更新されたシンボルファイルを展開
            var symbolTableArray = LoadSymbolFiles(updatedSymbolFilePaths);
            foreach (var symbolTable in symbolTableArray)
            {
                // ファイル情報の更新
                globalSymbolIndex.UpdateFileInfo(symbolTable);
            }

            // 定義更新の整合性チェック
            try
            {
                globalSymbolIndex.ValidateDefinitionChange();
            }
            catch (SymbolDefineException e)
            {
                foreach (var info in e.ErrorInfo)
                {
                    var mes = (info.Type == SymbolDefineException.Info.Types.Duplicate) ? "\tシンボル[{0}]の定義が重複しています" : "\tシンボル[{0}]の定義が見つかりません";

                    if (info.FilePath != null)
                    {
                        Console.Error.WriteLine(info.FilePath);
                    }
                    Console.Error.WriteLine(String.Format(mes, info.Symbol));
                }
                return null;
            }

            // シンボル解決すべきシンボリックバイナリを抽出
            var requireResolveBinary = GetAvailableBinaryPath(updatedSymbolFilePaths);
            requireResolveBinary.UnionWith(GetAvailableBinaryPath(globalSymbolIndex.GetFilesRequireResolve()));

            // 依存ファイルを抽出
            IEnumerable<KeyValuePair<string, IEnumerable<string>>> resolvePairList = null;
            try
            {
                resolvePairList = globalSymbolIndex.GetResolvePair(requireResolveBinary);
            }
            catch (SymbolDefineException e)
            {
                foreach (var info in e.ErrorInfo)
                {
                    Console.Error.WriteLine(info.FilePath);
                    Console.Error.WriteLine(String.Format("\tシンボル[{0}]の定義が見つかりません", info.Symbol));
                }
                return null;
            }
            */
            #endregion

            //@memo 差分処理を働かせず、渡された全てのシンボルのみを基にし、渡された全てのバイナリを解決する

            // 更新されたシンボルファイルを展開
            var symbolTableArray = LoadSymbolFiles(updatedSymbolFilePaths);

            Dictionary<string, string> defSymbolDict = new Dictionary<string, string>();
            var resolvePairDict = new Dictionary<string, IEnumerable<string>>();

            var duplicativeDefineDict = new Dictionary<string, HashSet<string>>();

            foreach (var symbolTable in symbolTableArray)
            {
                foreach (var symbol in symbolTable.DefinedSymbols)
                {
                    string definedFilePath = null;
                    if (defSymbolDict.TryGetValue(symbol.Key, out definedFilePath))
                    {
                        // 重複エラー処理
                        HashSet<string> dupFiles = null;
                        if (duplicativeDefineDict.TryGetValue(symbol.Key, out dupFiles))
                        {
                            dupFiles.Add(symbolTable.FilePath);
                        }
                        else
                        {
                            duplicativeDefineDict.Add(symbol.Key, new HashSet<string>(new[]{symbolTable.FilePath, definedFilePath}));
                        }
                    }
                    else
                    {
                        defSymbolDict.Add(symbol.Key, symbolTable.FilePath);
                    }
                }
            }
            if (duplicativeDefineDict.Count > 0)
            {
                foreach (var dupInfo in duplicativeDefineDict)
                {
                    Console.Error.WriteLine(string.Format("\t定義が重複しています : {0}", dupInfo.Key));
                    foreach(var filePath in dupInfo.Value)
                    {
                        Console.Error.WriteLine(String.Format("\t\t Defined At {0}", filePath));
                    }
                }
                return null; 
            }

            // 解決情報の整理
            var missingSymbolDict = new Dictionary<string, HashSet<string>>();
            foreach (var symbolTable in symbolTableArray)
            {
                // 解決対象で無ければスキップ
                var candidate = symbolTable.FilePath.Replace(".symbol", "");
                if (System.IO.File.Exists(Environment.ExpandEnvironmentVariables(candidate)) == false)
                {
                    continue;
                }

                var depends = new HashSet<string>();

                // 依存ファイルの列挙
                var missingSymbolList = new HashSet<string>();
                foreach (var symbol in symbolTable.ReferencedSymbols)
                {
                    string path = null;
                    if(defSymbolDict.TryGetValue(symbol.symbolName, out path))
                    {
                        depends.Add(defSymbolDict[symbol.symbolName]);
                    }
                    else
                    {
                        missingSymbolList.Add(symbol.symbolName);
                    }
                }
 
                if (missingSymbolList.Count > 0)
                {
                    missingSymbolDict.Add(symbolTable.FilePath, missingSymbolList);
                    continue;
                }
                    
                var tmp = symbolTable.FilePath.Split('.');
                var binaryPath = string.Join("", tmp.Take(tmp.Length - 2)) + "." + tmp[tmp.Length - 2];
                resolvePairDict.Add(binaryPath, depends);
            }
            if (missingSymbolDict.Count > 0)
            {
                foreach (var missingInfo in missingSymbolDict)
                {
                    Console.Error.WriteLine(string.Format("\t定義が見つかりません : Ref At {0}", missingInfo.Key));
                    foreach (var symbol in missingInfo.Value)
                    {
                        Console.Error.WriteLine(string.Format("\t\t : {0}", symbol));
                    }
                }
                return null;
            }

            return resolvePairDict.ToArray();
        }

        /// <summary>
        /// シンボルファイルからシンボルファイルクラスのインスタンスを生成する
        /// </summary>
        /// <param name="symbolFiles"></param>
        /// <returns></returns>
        static SymbolResolver.SymbolTable[] LoadSymbolFiles(IEnumerable<string> symbolFiles)
        {
            var symbolTableArray = new SymbolResolver.SymbolTable[symbolFiles.Count()];

            int i = 0;
            foreach(var filePath  in symbolFiles)
            {
                symbolTableArray[i] = new SymbolResolver.SymbolTable(filePath, true);
                ++i;
            }

            return symbolTableArray;
        }

        /// <summary>
        /// 指定したシンボルファイルに対応するファイルを取得する
        /// </summary>
        /// <param name="pathList"></param>
        /// <returns></returns>
        static HashSet<string> GetAvailableBinaryPath(IEnumerable<string> pathList)
        {
            var res = new HashSet<string>();

            foreach (var path in pathList)
            {
                var candidate = path.Replace(".symbol", "");
                if (System.IO.File.Exists( Environment.ExpandEnvironmentVariables(candidate)))
                {
                    res.Add(candidate);
                }
            }

            return res;
        }

        /// <summary>
        /// SymbolResolverを並列実行する
        /// </summary>
        /// <param name="resolvePairList"></param>
        /// <returns></returns>
        static bool DispatchProcesses(IEnumerable<KeyValuePair<string, IEnumerable<string>>> resolvePairList)
        {
            ProcessManager procManager = new ProcessManager(true);

            foreach (var resolvePair in resolvePairList)
            {
                var binary      = Environment.ExpandEnvironmentVariables(resolvePair.Key);
                var symbolFiles = resolvePair.Value.Select(item => Environment.ExpandEnvironmentVariables(item));

                var mode = "verbose";
                var outputDir = GetOutputDir(System.IO.Path.GetDirectoryName(binary));
                var arg = String.Join(" ", mode, binary, outputDir, String.Join(" ", symbolFiles));

                Console.WriteLine("SymbolResolver.exe " + arg);

                procManager.DispatchProcess(new System.Diagnostics.ProcessStartInfo("SymbolResolver.exe", arg));
            }

            procManager.WaitAll();

            // 1つでも失敗していれば失敗したプロセスを表示して「失敗」を返す
            if (procManager.FailProcessInfo.Count > 0)
            {
                foreach (var failInfo in procManager.FailProcessInfo)
                {
                    Console.Error.WriteLine(failInfo);
                }
                return false;
            }
            return true;
        }


        /// <summary>
        /// シンボリックバイナリファイルの格納元ディレクトリから出力先ディレクトリを導出する
        /// </summary>
        /// <param name="srcDirPath"></param>
        /// <returns></returns>
        static string GetOutputDir(string srcDirPath)
        {
            if (outputDirDictionary.ContainsKey(srcDirPath))
            {
                return outputDirDictionary[srcDirPath];
            }

            var result = "";

            // [入力ディレクトリ]/SymbolicSetting.xmlを設定ファイルとする
            var settingFilePath = System.IO.Path.Combine(srcDirPath, "SymbolicSetting.xml");
            if (System.IO.File.Exists(settingFilePath))
            {
                var setting =  System.Xml.Linq.XDocument.Load(settingFilePath);
 
                var param = setting.XPathSelectElement("Dir4Resolved").Value.Trim();

                result = new Uri(new Uri(srcDirPath), Environment.ExpandEnvironmentVariables(param)).LocalPath;
            }

            // 出力ディレクトリが設定されていなかった場合、[入力ディレクトリ]/resolved/に出力する
            if (result.Length == 0)
            {
                result = System.IO.Path.Combine(srcDirPath, "resolved");

                Console.WriteLine("[SymbolResolveManager]");
                Console.WriteLine("Notice : [{0}]に対する出力ディレクトリが設定されていないため、[{1}]に出力します. ", srcDirPath, result);
            }

            outputDirDictionary.Add(srcDirPath, result);

            return result;
        }
    }

}
