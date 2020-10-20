using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.XPath;
using System.Runtime.Serialization.Json;

namespace MakeGUI
{
    class DynamicTargetDetector
    {
        /// <summary>
        /// 指定したRubyスクリプトからコンバートターゲットのリストを取得する
        /// RubyスクリプトはJSONフォーマットの文字列を出力するものとする
        /// </summary>
        /// <param name="rubyExePath"></param>
        /// <param name="scriptPath"></param>
        /// <param name="platformPostfixes"></param>
        /// <returns></returns>
        static public Dictionary<string, TargetInfo> GetTargetsList(string rubyExePath, string scriptPath, Dictionary<string, PlatformParameter> platformPostfixes)
        {

            if (System.IO.File.Exists(scriptPath) == false)
            {
                throw new Exception(String.Format("{0} \nが存在しません", scriptPath));
            }

            var res = new Dictionary<string, TargetInfo>();

            var ps = Util.CreateCmdProcessStartInfo(string.Format("{0} {1}", rubyExePath, scriptPath));

            System.Xml.Linq.XElement info_list;
            using (var stream = Util.GetProcessStream(ps, System.Text.Encoding.UTF8))
            {
                var reader = JsonReaderWriterFactory.CreateJsonReader(stream.BaseStream, new System.Xml.XmlDictionaryReaderQuotas());
                info_list = System.Xml.Linq.XElement.Load(reader, System.Xml.Linq.LoadOptions.PreserveWhitespace);
            }
            foreach (var info in info_list.XPathSelectElements("item"))
            {
                var target = info.Element("target").Value;

                var targetInfo = new TargetInfo
                {
                   AvailablePlatform = (info.Element("platform") == null) ? 0 : info.XPathSelectElements("platform/item").Aggregate((uint)0, (o, item) => o | platformPostfixes[item.Value].Flag),
                   Comment = (info.Element("comment") == null) ? "(No Comment)" : info.Element("comment").Value,
                   Args = (info.Element("args") == null) ? "" : info.XPathSelectElements("args/item").Aggregate("", (o, item) => o + " " + item.Value),
                };

                res.Add(target, targetInfo);
            }

            return res;
        }

    }
}
