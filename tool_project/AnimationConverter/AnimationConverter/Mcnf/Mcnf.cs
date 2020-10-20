using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

using System.Xml;
using System.Xml.Serialization;

#if !NOUSE_MCNFUTIL 
using gfl2.clr.renderingengine.scenegraph.instance;
#endif

namespace Mcnf
{
  public class INTEREST_NODE
  {
    public enum InterestType
    {
      None,
      X,
      YZ,
      XZ,
      XY,
      ZY,
      Y,
      Z,
      X_M90,
      Z_180,
      Y_90,

      // 実際はここから下は使われない
      YX,
      ZX,
    }

    [XmlAttribute()]
    public string _Name;

    [XmlAttribute()]
    public InterestType _Type;

    [XmlAttribute()]
    public int _Low;

    [XmlAttribute()]
    public int _High;

    [XmlAttribute()]
    public int _Low2;

    [XmlAttribute()]
    public int _High2;

    [XmlAttribute()]
    public int _Speed = 100;
  }

  public class INTEREST
  {
    [XmlElement()]
    public List<INTEREST_NODE> NODE = new List<INTEREST_NODE>();
  }

  public class EXPRESSION
  {
    public enum AimType
    {
      Aim,
      RootAim,
    }

    [XmlAttribute()]
    public AimType _Type;

    [XmlAttribute()]
    public string _Dst;

    [XmlAttribute()]
    public string _Src0;
  }

  public class SPRING
  {
    [XmlAttribute()]
    public string _Name;

    [XmlAttribute()]
    public int _Damping;

    [XmlAttribute()]
    public int _Stiffness;

    [XmlAttribute()]
    public int _Mass;

    [XmlAttribute()]
    public int _Gravity;
  }

	//<IK _Chain="RThigh" _Joint="RLeg" _Effector="RFoot" _Type="reverse" _Tip="RToe" _Land="true" />
  public class IK
  {
    public enum Type
    {
      normal,
      reverse,
      effector_child
    }

    [XmlAttribute()]
    public string _Chain;

    [XmlAttribute()]
    public string _Joint;

    [XmlAttribute()]
    public string _Effector;

    [XmlAttribute()]
    public string _EffectorChild;

    [XmlAttribute()]
    public Type _Type;

    [XmlAttribute()]
    public string _Tip;

    [XmlAttribute()]
    public bool _Hold;

    [XmlAttribute()]
    public bool _Land;
  }
  
  //<MOUTH _Name="UpperJaw"/>
  public class MOUTH
  {
    [XmlAttribute()]
    public string _Name;
  }

  //<EYE _Name="Feeler"/>
  public class EYE
  {
    [XmlAttribute()]
    public string _Name;
  }

  [XmlRoot("root")]
  public class Mcnf
  {
    public INTEREST INTEREST = new INTEREST();

    [XmlElement()]
    public List<EXPRESSION> EXPRESSION = new List<EXPRESSION>();

    [XmlElement()]
    public List<SPRING> SPRING = new List<SPRING>();

    [XmlElement()]
    public List<IK> IK = new List<IK>();

    [XmlElement()]
    public List<MOUTH> MOUTH = new List<MOUTH>();

    [XmlElement()]
    public List<EYE> EYE = new List<EYE>();

    [XmlIgnore()]
    public string FileName;

    // シリアライズ
    public static void Serialize(string outputPath, Mcnf rootNode)
    {
      var impoter = new XmlReflectionImporter();
      var mapping = impoter.ImportTypeMapping(typeof(Mcnf), null, null); 
      var serializer = new XmlSerializer(mapping);

      using (var fs = File.Open(outputPath, FileMode.OpenOrCreate))
      {
        // 上書き用にサイズを０にしておく
        fs.SetLength(0);

        // シリアライズ
        serializer.Serialize(fs, rootNode);
      }
    }

    // デシリアライズ
    public static Mcnf Deserialize(string inputPath)
    {
      // ファイルの存在チェック
      if (!File.Exists(inputPath))
      {
        return null;
      }

      var impoter = new XmlReflectionImporter();
      var mapping = impoter.ImportTypeMapping(typeof(Mcnf), null, null); 
      var serializer = new XmlSerializer(mapping);

      using (var fs = File.Open(inputPath, FileMode.Open))
      {
        // デシリアライズ
        return serializer.Deserialize(fs) as Mcnf;
      }
    }
  }

#if !NOUSE_MCNFUTIL 
  class McnfUtil
  {
    static private void SetNode2Axis(INTEREST_NODE node, JointInstanceNode jnt)
    {
      // Y軸方向の座標値のチェック
      var matrix = jnt.GetWorldMatrix();
      var yaxis_x = Math.Abs(matrix.GetElem(1, 0));
      var yaxis_y = Math.Abs(matrix.GetElem(1, 1));
      var yaxis_z = Math.Abs(matrix.GetElem(1, 2));
      var ymax = yaxis_x;
      if (ymax < yaxis_y)
      {
        ymax = yaxis_y;
      }
      if (ymax < yaxis_z)
      {
        ymax = yaxis_z;
      }

      // Z軸方向の座標値のチェック
      var zaxis_x = Math.Abs(matrix.GetElem(2, 0));
      var zaxis_y = Math.Abs(matrix.GetElem(2, 1));
      var zaxis_z = Math.Abs(matrix.GetElem(2, 2));
      node._Name = jnt.GetName();
      if (ymax == yaxis_x)
      {
        node._Type = (zaxis_y < zaxis_z) ? INTEREST_NODE.InterestType.XY : INTEREST_NODE.InterestType.XZ;
      }
      else if (ymax == yaxis_y)
      {
        node._Type = (zaxis_z < zaxis_x) ? INTEREST_NODE.InterestType.YZ : INTEREST_NODE.InterestType.YX;
      }
      else if (ymax == yaxis_z)
      {
        node._Type = (zaxis_y < zaxis_x) ? INTEREST_NODE.InterestType.ZY : INTEREST_NODE.InterestType.ZX;
      }

      node._Low = -30;
      node._High = 30;
      node._Low2 = -20;
      node._High2 = 20;
      node._Speed = 100;
    }

    static private void SetNodeEar(Mcnf root_node, JointInstanceNode jnt)
    {
      // Expression
      var expression = new EXPRESSION();
      root_node.EXPRESSION.Add(expression);
      expression._Type = EXPRESSION.AimType.Aim;
      var parentName = jnt.GetParentName();
      if (parentName != null)
      {
        expression._Dst = parentName;
      }
      expression._Src0 = jnt.GetName();

      // Spring
      var spring = new SPRING();
      root_node.SPRING.Add(spring);
      spring._Name = jnt.GetName();
      spring._Damping = 55;
      spring._Stiffness = 45;
      spring._Mass = 10;
      spring._Gravity = 10;
    }

    static private void SetNodeMouth(Mcnf root_node, JointInstanceNode jnt)
    {
      // Mouth
      var mouth = new MOUTH();
      root_node.MOUTH.Add(mouth);
      mouth._Name = jnt.GetName();
    }

    static public Mcnf CreateMcnf(JointInstanceNode[] jointList)
    {
      //m_Changed = true;
      var root_node = new Mcnf();
      var inode = new INTEREST();
      root_node.INTEREST = inode;
      var node = new INTEREST_NODE();
      inode.NODE.Add(node);

      // 頭を探す
      var head_exist = false;
      foreach (var jnt in jointList)
      {
        if (jnt.GetName() == "Head" )
        {
          node._Name = jnt.GetParentName();
          node._Type = INTEREST_NODE.InterestType.X;
          node._Low = -20;
          node._High = 20;
          node._Speed = 100;

          var node2 = new INTEREST_NODE();
          inode.NODE.Add(node2);
          SetNode2Axis(node2, jnt);
          head_exist = true;
          break;
        }
      }

      // 頭がなかったので腰
      if (!head_exist)
      {
        foreach (var jnt in jointList)
        {
          if (jnt.GetName() == "Waist")
          {
            SetNode2Axis(node, jnt);
            break;
          }
        }
      }

      // 耳があるのか？
      foreach (var jnt in jointList)
      {
        if (jnt.GetName().Contains("Ear2"))
        {
          SetNodeEar(root_node, jnt);
        }
      }

      // Jawを口に設定する
      foreach (var jnt in jointList)
      {
        if (jnt.GetName().Contains("Jaw"))
        {
          SetNodeMouth(root_node, jnt);
        }
      }

      return root_node;
    }
  }
#endif

}
