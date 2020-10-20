using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class BlendShapeAnimation
  {
    [XmlElement("TargetMesh")]
    public BlendShapeAnimation_TargetMesh[] TargetMesh;
  }

  public class BlendShapeAnimation_TargetMesh
  {
    [XmlAttribute()]
    public string Name;

    [XmlElement("Frame")]
    public BlendShapeAnimation_Frame[] Frame;
  }

  public class BlendShapeAnimation_Frame
  {
    [XmlAttribute()]
    public int Frame;

    [XmlElement("BlendMesh")]
    public BlendShapeAnimation_BlendMesh[] BlendMesh;
  }

  public class BlendShapeAnimation_BlendMesh
  {
    [XmlAttribute()]
    public string Name;

    [XmlAttribute()]
    public float Weight;
  }
}
