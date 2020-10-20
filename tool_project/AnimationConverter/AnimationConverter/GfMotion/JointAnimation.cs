using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class JointAnimation
  {
    [XmlElement("Node")]
    public JointAnimation_Node[] Node;
  }

  public class JointAnimation_Node
  {
    [XmlAttribute()]
    public string Name;

    public Common_Keys ScaleX;
    public Common_Keys ScaleY;
    public Common_Keys ScaleZ;

    public Common_Keys RotateX;
    public Common_Keys RotateY;
    public Common_Keys RotateZ;

    public Common_Keys TranslateX;
    public Common_Keys TranslateY;
    public Common_Keys TranslateZ;
  }

}
