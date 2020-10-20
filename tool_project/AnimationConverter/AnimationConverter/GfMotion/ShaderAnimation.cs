using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class ShaderAnimation
  {
    [XmlElement("Material")]
    public ShaderAnimation_Material[] Material;
  }

  public class ShaderAnimation_Material
  {
    [XmlAttribute()]
    public string Name;

    [XmlElement("ConstantColor")]
    public ShaderAnimation_ConstantColor[] ConstantColor;
  }

  public class ShaderAnimation_ConstantColor
  {
    [XmlAttribute()]
    public int No;

    public Common_Keys ColorR;
    public Common_Keys ColorG;
    public Common_Keys ColorB;
    public Common_Keys ColorA;
  }
}
