using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class UvAnimation
  {
    [XmlElement("Material")]
    public UvAnimation_Material[] Material;
  }

  public class UvAnimation_Material
  {
    [XmlAttribute()]
    public string Name;

    [XmlElement("TextureSlot")]
    public UvAnimation_TextureSlot[] TextureSlot;
  }

  public class UvAnimation_TextureSlot
  {
    [XmlAttribute()]
    public int No;

    public Common_Keys ScaleU;
    public Common_Keys ScaleV;
    public Common_Keys Rotate;
    public Common_Keys TranslateU;
    public Common_Keys TranslateV;
  }
}
