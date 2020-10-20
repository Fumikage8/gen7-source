using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;
using System.IO;

namespace PokeViewerHost.Utility
{
    public class PersonalConfig
    {
        public string CompositeBackgoundImagePath { get; set; }

        [XmlIgnore]
        public string SerializePath { get; set; }

        /// <summary>
        ///   
        /// </summary>
        public PersonalConfig()
        {
            SerializePath = @".\PersonalConfig.xml";
        }

        /*
         * 
         * */
        public void Serialize()
        {
            Serialize(SerializePath);
        }

        /*
         * 
         * */
        public void Serialize(string configFilePath)
        {
            XmlSerializer xml = new XmlSerializer(typeof(PersonalConfig));

            string fullPath = System.IO.Path.GetFullPath(configFilePath);
            System.IO.Directory.CreateDirectory(System.IO.Path.GetDirectoryName(fullPath));
            using (FileStream fs = new FileStream(fullPath, FileMode.Create, FileAccess.Write, FileShare.Write))
            {
                xml.Serialize(fs, this);
            }
        }

        /*
         * 
         * */
        public static PersonalConfig Deserialize(string configFilePath)
        {
            PersonalConfig config = null;
            XmlSerializer xml = new XmlSerializer(typeof(PersonalConfig));

            try
            {
                using (StreamReader sr = new StreamReader(configFilePath))
                {
                    config = (PersonalConfig)xml.Deserialize(sr);
                }
            }
            catch (System.Exception ex)
            {
            	
            }

            return config;
        }
    }
}
