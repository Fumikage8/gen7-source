using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;



namespace MakeGUI
{
    enum TargetType { Static, Dynamic };

    /// <summary>
    /// MakeFileの解釈を行うクラス
    /// </summary>
    class MakeFileAnalyser
    {
        /// <summary>
        /// MakeFileに記述されたタイトル
        /// </summary>
        public string Title { get; private set; }

        /// <summary>
        /// MakeFileのフルパス
        /// </summary>
        public string FileFullPath { get; private set; }

        /// <summary>
        /// MakeFileから読み取ったターゲット
        /// </summary>
        public Dictionary<string, TargetInfo> StaticBuildTargets { get; private set; }

        /// <summary>
        /// MakeFileに指定されたプログラムが決定したターゲット
        /// </summary>
        public Dictionary<string, TargetInfo> DynamicBuildTargets { get; private set; }


        Dictionary<string, PlatformParameter> platformPostfixes;

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="path"></param>
        public MakeFileAnalyser(string path, Setting setting)
        {
            string itemListerPath = null;

            this.FileFullPath = System.IO.Path.GetFullPath(path);
            this.platformPostfixes = setting.PlatformSettings;

            using(var file = new System.IO.StreamReader(path, Encoding.UTF8))
            {
                string[] targetList = null;

                // ヘッダ付加情報を取得
                {
                    var headerManipList = new[]{
                        new KeyValuePair<string, Action<string>>(
                            "##Title",
                            value => this.Title = value.Trim()
                        ),
                        new KeyValuePair<string, Action<string>>(
                            "##ItemLister",
                            value => itemListerPath = Environment.ExpandEnvironmentVariables(value.Trim())
                        ),
                        new KeyValuePair<string, Action<string>>(
                            "##List4GUI",
                            value => targetList = value.Trim().Split(' ')
                        ),
                    };
                    var line = "";
                    while ((line = file.ReadLine()) != null)
                    {
                        foreach (var manip in headerManipList)
                        {
                            if (line.StartsWith(manip.Key))
                            {
                                manip.Value(line.Split(':')[1]);
                            }
                        }
                        if (line.StartsWith("##HeaderEnd"))
                        {
                            break;
                        }
                    }
                }

                if (targetList == null || targetList.Length == 0)
                {
                    throw new Exception("静的ターゲットが1つも記述されていません");
                }

                if (this.Title != null)
                {
                    this.Title += System.Environment.NewLine;
                }
                this.Title += this.FileFullPath;

                this.StaticBuildTargets = GetPhonyTargets(file, targetList, this.platformPostfixes);
            }

            // スクリプトから動的ターゲットの取得
            if (itemListerPath != null)
            {
                this.DynamicBuildTargets = DynamicTargetDetector.GetTargetsList(setting.DynamicTargetListerDriverPath, itemListerPath, this.platformPostfixes);
            }
        }

        /// <summary>
        /// 静的ターゲット(MakeFileに記述されたPhonyターゲット)を取得
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="phonyTargets"></param>
        /// <returns></returns>
        private Dictionary<string, TargetInfo> GetPhonyTargets(System.IO.StreamReader stream, string[] phonyTargets, Dictionary<string, PlatformParameter> platformPostfixes)
        {
            var res = new Dictionary<string, TargetInfo>();

            var line = "";
            var preLines = "";
            var regRule = new System.Text.RegularExpressions.Regex(@"([\w_]+)[\s]*:.*");
            while ((line = stream.ReadLine()) != null)
            {
                // コメントの取得
                if (line.StartsWith("#"))
                {
                    preLines += line.TrimStart('#', ' ') + System.Environment.NewLine;
                    continue;
                }

                var match = regRule.Match(line);
                if (match.Success && phonyTargets.Contains(match.Groups[1].Value))
                {
                    var parts = match.Groups[1].Value.Split('_');

                    var target = String.Join("_", parts, 0, parts.Length - 1);
                    var platform = platformPostfixes[parts.Last()];

                    var newInfo = new TargetInfo { Comment = preLines, AvailablePlatform = platform.Flag };

                    TargetInfo prevInfo;                   
                    if (res.TryGetValue(target, out prevInfo))
                    {
                        newInfo.AvailablePlatform |= prevInfo.AvailablePlatform;
                        newInfo.Comment = (newInfo.Comment.Length == 0) ? prevInfo.Comment : newInfo.Comment;
                    }
                    res[target] = newInfo;

                }

                preLines = "";
            }

            return res;
        }

        /// <summary>
        /// ターゲットから選択可能な対象プラットホームの組み合わせ示すフラグを取得する
        /// </summary>
        /// <param name="target"></param>
        /// <param name="targetType"></param>
        /// <returns></returns>
        public uint GetAvailablePlatform(string target, TargetType targetType)
        {
            switch (targetType)
            {
                case TargetType.Dynamic:
                    return this.DynamicBuildTargets[target].AvailablePlatform;

                case TargetType.Static:
                    return this.StaticBuildTargets[target].AvailablePlatform;
            }

            throw new NotImplementedException();
        }

        /// <summary>
        /// ターゲットの指定からMakeに渡すパラメータを生成する
        /// </summary>
        public string ConvertTargetString(string target, TargetType targetType, uint platformFlag)
        {
            switch (targetType)
            {
                case TargetType.Dynamic:
                    var arg = this.DynamicBuildTargets[target].Args;
                    return String.Format("TARGET_4_OMAKE={0} ARG_4_OMAKE='{1}' {2}", target, arg, "DYNAMIC_TARGET_4_OMAKE");
                case TargetType.Static:
                    return target + "_" + this.platformPostfixes.First(item => item.Value.Flag == platformFlag).Key;
            }
            throw new NotImplementedException();
        }


    }

}
