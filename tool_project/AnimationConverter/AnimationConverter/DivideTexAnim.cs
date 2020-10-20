using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace AnimationConverter
{
  [XmlRoot("root")]
  public class DivideTexAnim
  {
    [XmlElement()]
    public EXCEPTION[] EXCEPTION;
  }

  public class EXCEPTION
  {
    [XmlAttribute()]
    public string _FileName;

    [XmlAttribute()]
    public string _Parts;

    [XmlAttribute()]
    public string _FileType;
  }
}
