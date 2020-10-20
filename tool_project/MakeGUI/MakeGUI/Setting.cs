using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.Xml.XPath;

namespace MakeGUI
{
    class Setting
    {
        /// <summary>
        /// プラットホームの定義
        /// キー : プラットホーム識別子
        /// </summary>
        public Dictionary<string, PlatformParameter> PlatformSettings { get; private set; }

        /// <summary>
        /// IncrediBuildのプロファイルファイルのパス
        /// </summary>
        public string IBProfileFilePath { get; private set;}

        /// <summary>
        /// 動的ターゲットを決定するスクリプトを実行するアプリケーションのファイルパス
        /// </summary>
        public string DynamicTargetListerDriverPath { get; private set; }

        public Setting(string filePath)
        {
            var xml = XDocument.Load(filePath);

            this.PlatformSettings = new Dictionary<string, PlatformParameter>();

            uint flag = 1;
            foreach (var node in xml.XPathSelectElements("Document/PlatformSettings/Platform"))
            {
                var id = node.Attribute("ID").Value as string;

                var displayName = node.Attribute("DisplayName").Value as string;
                var batPath = (node.Attribute("BatFilePath") != null) ? node.Attribute("BatFilePath").Value as string : "";

                this.PlatformSettings.Add(id, new PlatformParameter(displayName, flag, batPath));
                flag *= 2;
            }

            this.DynamicTargetListerDriverPath = Environment.ExpandEnvironmentVariables(xml.XPathSelectElement("Document/DynamicTargetListerDriver").Attribute("Path").Value);

            this.IBProfileFilePath = Environment.ExpandEnvironmentVariables(xml.XPathSelectElement("Document/IBSetting").Attribute("ProfileFilePath").Value);
        }

    }

}
