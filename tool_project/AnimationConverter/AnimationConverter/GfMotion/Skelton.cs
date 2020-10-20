using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Gfmot
{
  public class Skelton
  {
    [XmlElement("Node")]
    public Skelton_Node[] Node;
  }

  public class Skelton_Node
  {
    [XmlAttribute()]
    public string Name;

    [XmlAttribute()]
    public string ParentName;

    [XmlAttribute()]
    public string NeededRendering;

    [XmlAttribute()]
    public string IsJoint;

    public string LocalScale;
    public string LocalRotation;
    public string LocalTransform;

    [XmlIgnore()]
    public Skelton_Node Parent = null;

    [XmlIgnore()]
    public List<Skelton_Node> ChildList = null;

    [XmlIgnore()]
    public int Depth
    {
      get
      {
        var depth = 0;
        var joint = this;
        while (joint.Parent != null)
        {
          joint = joint.Parent;
          ++depth;
        }

        return depth;
      } 
    }

    [XmlIgnore()]
    public int ChildDepth
    {
      get
      {
        return GetMaxChildDepth(this) - this.Depth;
      } 
    }
    private int GetMaxChildDepth(Skelton_Node joint)
    {
      var depth = joint.Depth;
      if (joint.ChildList != null)
      {
        foreach (var child in joint.ChildList)
        {
          var childDepth = child.GetMaxChildDepth(child);
          if (depth < childDepth)
          {
            depth = childDepth;
          }
        }
      }
      return depth;
    }

    public static List<string> GetAllJoint(Dictionary<string, Skelton_Node> jointDict, string jointName)
    {
      var jointList = new List<string>();

      var joint = jointDict[jointName];
      if (joint.IsJoint != "TRUE")
      {
        return jointList;
      }

      jointList.Add(jointName);

      var childList = joint.ChildList;
      if (childList == null)
      {
        return jointList;
      }

      foreach (var child in childList)
      {
        jointList.AddRange(GetAllJoint(jointDict, child.Name));
      }

      return jointList;
    }
  }
}
