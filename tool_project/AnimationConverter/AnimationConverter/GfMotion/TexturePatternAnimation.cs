using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class TexturePatternAnimation
  {
    [XmlElement("Material")]
    public TexturePatternAnimation_Material[] Material;
  }

  public class TexturePatternAnimation_Material
  {
    [XmlAttribute()]
    public string Name;

    [XmlElement("TextureSlot")]
    public TexturePatternAnimation_TextureSlot[] TextureSlot;
  }

  public class TexturePatternAnimation_TextureSlot
  {
    [XmlAttribute()]
    public int No;

    [XmlElement("Attribute")]
    public TexturePatternAnimation_Attribute[] Attribute;
  }

  public class TexturePatternAnimation_Attribute
  {
    [XmlAttribute()]
    public int Frame;

    [XmlAttribute()]
    public string TextureName;
  }
}
