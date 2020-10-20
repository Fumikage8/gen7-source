using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;
using System.Xml;
using System.Xml.Serialization;

namespace Cskla
{
  public class NintendoWareIntermediateFile
  {
    public GraphicsContentCtr GraphicsContentCtr;
  }

	public class GraphicsContentCtr
  {
    [XmlAttribute()]
    public string Version;

    public SkeletalAnimationData[] Animations;
  }

  public class SkeletalAnimationData
  {
    [XmlAttribute()]
    public int FrameSize;

    [XmlAttribute()]
    public string LoopMode;

    public MemberAnimationData[] MemberAnimationDataSet;
    public Skeleton Skeleton;
  }

  public class MemberAnimationData
  {
    public string Path;

    public TransformAnimation TransformAnimation;
  }

  public class TransformAnimation
  {
    public Vector3Animation ScaleVector3Animation;
    public Vector3Animation RotateVector3Animation;
    public Vector3Animation TranslateVector3Animation;
  }

  public class Vector3Animation
  {
    public SegmentsFloatCurve XSegmentsFloatCurve;
    public SegmentsFloatCurve YSegmentsFloatCurve;
    public SegmentsFloatCurve ZSegmentsFloatCurve;
  }

  public class SegmentsFloatCurve
  {
    [XmlAttribute()]
    public int StartFrame;

    [XmlAttribute()]
    public int EndFrame;

    //[XmlArrayItem(typeof(HermiteFloatSegment)),
      //XmlArrayItem(typeof(LinearFloatSegment))]
    //public FloatSegmentBase[] Segments;
    public Segments Segments;
  }

  public class Segments
  {
    public HermiteFloatSegment HermiteFloatSegment;
    public LinearFloatSegment LinearFloatSegment;
  }

  public class HermiteFloatSegment
  {
    public HermiteFloatKey[] Keys;
  }

  public class LinearFloatSegment
  {
    public LinearFloatKey[] Keys;
  }

  public class HermiteFloatKey
  {
    [XmlAttribute()]
    public int Frame;

    [XmlAttribute()]
    public float Value;

    [XmlAttribute()]
    public float InSlope;
  }

  public class LinearFloatKey
  {
    [XmlAttribute()]
    public int Frame;

    [XmlAttribute()]
    public float Value;
  }

  public class Skeleton
  {
    [XmlAttribute()]
    public string RootBoneName;

    public Bone[] Bones;
  }

  public class Bone
  {
    [XmlAttribute()]
    public string Name;

    [XmlAttribute()]
    public string ParentBoneName;

    public Transform Transform;
  }

  public class Transform
  {
    public Vector3 Scale;
    public Vector3 Rotate;
    public Vector3 Translate;
  }

  public class Vector3
  {
    [XmlAttribute()]
    public float X;

    [XmlAttribute()]
    public float Y;

    [XmlAttribute()]
    public float Z;
  }
}
