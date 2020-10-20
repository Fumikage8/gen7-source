using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class VisibilityAnimation
  {
    [XmlElement("Mesh")]
    public VisibilityAnimation_Mesh[] Mesh;
  }

  public class VisibilityAnimation_Mesh
  {
    [XmlAttribute()]
    public string Name;

    [XmlElement("BoolKey")]
    public Common_BoolKey[] BoolKey;
  }
}
