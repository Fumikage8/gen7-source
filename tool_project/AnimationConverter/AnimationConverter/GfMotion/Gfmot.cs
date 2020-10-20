using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class GfMotion
  {
    [XmlAttribute()]
    public string version;

    [XmlAttribute()]
    public int FrameSize;

    [XmlAttribute()]
    public bool IsLoop;

    [XmlAttribute()]
    public string CurrrentUnit;

    public Source Source;

    public string ModelAABB;

    public UvAnimation UvAnimation;
    public BlendShapeAnimation BlendShapeAnimation;
    public TexturePatternAnimation TexturePatternAnimation;
    public VisibilityAnimation VisibilityAnimation;
    public AttributeKeyAnimation AttributeKeyAnimation;
    public ShaderAnimation ShaderAnimation;
    public JointAnimation SkeltalAnimation;
    public Skelton Skelton;

    public EnvAnimation EnvAnimation;
  }

  // これだけなのでとりあえずここに
  public class GfModel
  {
    public Source Source;
    public Skelton Skelton;
  }

  public class Source
  {
    [XmlAttribute()]
    public string Path;

    [XmlAttribute()]
    public string TargetNode;
  }
}
