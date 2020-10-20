using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;



namespace PokeViewerHost.Utility
{
    

    public class Config
    {
        /*
         * properties
         * */
        public static string DefaultConfigFileName { get { return @"config.xml"; } }
        public string WorkspacePath { get; set; }
        public string EnvironmentDir { get; set; }
        public string ConfigFilePath { get { return string.Format(@"{0}\{1}\{2}", WorkspacePath, EnvironmentDir, ConfigFileName); } }
        public string ConfigFileName { get; set; }

        /*      */
        public string ImageDir_Color { get; set; }
        public string ImageDir_Alpha { get; set; }
        public string ImageDir_Composite { get; set; }
        public string BluePrintDir { get; set; }
        public string FileNamingRule { get; set; }
        public int GuideGridSize_1 { get; set; }
        public int GuideGridSize_2 { get; set; }
        public int GuideGridSize_3 { get; set; }


        /*   pokeInfoList   */
        public string PokeInfoDefinitionFileName { get; set; }
        public string PokeInfoDefinitionFilePath { get { return string.Format(@"{0}\{1}\{2}", WorkspacePath, EnvironmentDir, PokeInfoDefinitionFileName); } }
        public string PokeNameListFileName { get { return @"PokeNameList.xml"; } }
        public string PokeNameListFilePath { get { return string.Format(@"{0}\{1}\{2}", WorkspacePath, EnvironmentDir, PokeNameListFileName); } }
        


        /*
         * members
         * */
        protected string mConfigFileName = DefaultConfigFileName;

        /*  pokeInfoListDef */



        /// <summary>
        ///   
        /// </summary>
        public Config()
        {
            WorkspacePath = @".\workspace";
            EnvironmentDir = @"_env";
            ConfigFileName = DefaultConfigFileName;

            ImageDir_Color = @"capture\color";
            ImageDir_Alpha = @"capture\alpha";
            ImageDir_Composite = @"capture\composite";
            BluePrintDir = @"capture\blueprint";
            GuideGridSize_1 = 128;
            GuideGridSize_2 = 160;
            GuideGridSize_3 = 192;

            PokeInfoDefinitionFileName = @"PokeInfoDef.xml";

            FileNamingRule = PokeInfo.DefaultMakeFileNameFormat;
        }




        /*
         * 
         * */
        public void Serialize()
        {
            Serialize(DefaultConfigFileName);
        }

        /*
         * 
         * */
        public void Serialize(string configFileName)
        {
            XmlSerializer xml = new XmlSerializer(typeof(Config));

//            using (StreamWriter sw = new StreamWriter(ConfigFilePath))
            System.IO.Directory.CreateDirectory(System.IO.Path.GetDirectoryName(ConfigFilePath));
            using (FileStream fs = new FileStream(ConfigFilePath, FileMode.Create, FileAccess.Write, FileShare.Write))
            {
                xml.Serialize(fs, this);
            }
        }

        /*
         * 
         * */
        public static Config Deserialize()
        {
            return Deserialize(DefaultConfigFileName);
        }

        /*
         * 
         * */
        public static Config Deserialize(string configFilePath)
        {
            Config config = null;
            XmlSerializer xml = new XmlSerializer(typeof(Config));

            using (StreamReader sr = new StreamReader(configFilePath))
            {
                config = (Config)xml.Deserialize(sr);
            }

            return config;
        }
    }
}
