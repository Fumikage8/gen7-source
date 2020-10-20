using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

using AnimationConverter;

namespace Gfmot
{
  public class Common_Keys
  {
    [XmlElement("HermiteKey")]
    public Common_HermiteKey[] HermiteKeys;

    public Common_OneFrameKey OneFrameKey;

    public static Common_HermiteKey[] GetHermiteKeys(Common_Keys keys)
    {
      if (!HasHermiteKeys(keys))
      {
        return null;
      }

      return keys.HermiteKeys;
    }

    public static bool HasHermiteKeys(Common_Keys keys)
    {
      if (keys == null)
      {
        return false;
      }

      if (Common.IsNullOrEmpty(keys.HermiteKeys))
      {
        return false;
      }

      return true;
    }

    public static bool IsDataLoop(Common_Keys keys)
    {
      var hermiteKeys = GetHermiteKeys(keys);
      if (hermiteKeys == null)
      {
        return true;
      }

      return hermiteKeys.First().Value == hermiteKeys.Last().Value;
    }

    public static Common_Keys CreateOneFrameKey(float value)
    {
      var key = new Common_Keys();
      key.OneFrameKey = new Common_OneFrameKey();
      key.OneFrameKey.Value = value;

      return key;
    }
  }

  public class Common_HermiteKey
  {
    [XmlAttribute()]
    public int Frame;

    [XmlAttribute()]
    public float Value;

    [XmlAttribute()]
    public float Slope;
  }

  public class Common_OneFrameKey
  {
    [XmlAttribute()]
    public float Value;
  }

  public class Common_AttributeKey
  {
    [XmlAttribute()]
    public int Frame;

    [XmlAttribute()]
    public float Value;
  }

  public class Common_BoolKey
  {
    [XmlAttribute()]
    public int Frame;

    [XmlAttribute()]
    public bool Value;
  }
}
