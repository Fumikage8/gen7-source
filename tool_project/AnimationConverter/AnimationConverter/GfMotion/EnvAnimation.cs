using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class EnvAnimation
  {
    public EnvAnimation_DrawEnv DrawEnv;
  }

  public class EnvAnimation_DrawEnv
  {
    [XmlAttribute()]
    public string Name;

    public EnvAnimation_LightSets LightSets;
    public EnvAnimation_Cameras Cameras;
    public EnvAnimation_EnvParams EnvParams;  //kawa20150514
  }

  public class EnvAnimation_LightSets
  {
    [XmlElement("LightSet")]
    public EnvAnimation_LightSet[] LightSet;
  }

  public class EnvAnimation_LightSet
  {
    [XmlAttribute()]
    public int No;

    [XmlElement("Light")]
    public EnvAnimation_Light[] Light;
  }

  public class EnvAnimation_Light
  {
    public enum LightType
    {
      AmbientLight,
      DirectionalLight,
      PointLight,
      SpotLight,
    }

    [XmlAttribute()]
    public string Name;

    [XmlAttribute()]
    public LightType Type;

    public Common_Keys PositionX;
    public Common_Keys PositionY;
    public Common_Keys PositionZ;
    public Common_Keys DirectionX;
    public Common_Keys DirectionY;
    public Common_Keys DirectionZ;
    public Common_Keys ColorR;
    public Common_Keys ColorG;
    public Common_Keys ColorB;
    public Common_Keys ConeAngle;
    public Common_Keys Intensity;
  }

  public class EnvAnimation_Cameras
  {
    [XmlElement("Camera")]
    public EnvAnimation_Camera[] Camera;
  }

  public class EnvAnimation_Camera
  {
    [XmlAttribute()]
    public string Name;

    public Common_Keys Near;
    public Common_Keys Far;
    public Common_Keys Fovy;
    public Common_Keys AspectRatio;
    public Common_Keys RotateX;
    public Common_Keys RotateY;
    public Common_Keys RotateZ;
    public Common_Keys TranslateX;
    public Common_Keys TranslateY;
    public Common_Keys TranslateZ;
  }

  //kawa20150514
  public class EnvAnimation_EnvParams
  {
    [XmlElement("Fog")]
    public EnvAnimation_Fog[] Fog;

    [XmlElement("Glare")]
    public EnvAnimation_Glare Glare;
  }

  public class EnvAnimation_Fog
  {
    [XmlAttribute()]
    public int No;

    public Common_Keys NearLength;
    public Common_Keys FarLength;
    public Common_Keys ColorR;
    public Common_Keys ColorG;
    public Common_Keys ColorB;
    public Common_Keys Strength;
  }

  public class EnvAnimation_Glare
  {
    public Common_Keys GlareStrength;
    public Common_Keys GlareRange;
    public Common_Keys GlareIntensity;
    public Common_Keys GlareWeightR;
    public Common_Keys GlareWeightG;
    public Common_Keys GlareWeightB;
  }
}
