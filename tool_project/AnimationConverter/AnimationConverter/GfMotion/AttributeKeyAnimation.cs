using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class AttributeKeyAnimation
  {
    [XmlElement("Attribute")]
    public AttributeKeyAnimation_Attribute[] Attribute;
  }

  public class AttributeKeyAnimation_Attribute
  {
    [XmlAttribute()]
    public string Name;

    [XmlElement("AttributeKey")]
    public Common_AttributeKey[] AttributeKey;
  }
}
