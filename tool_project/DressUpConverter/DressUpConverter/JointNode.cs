using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;

using MathNet.Numerics.LinearAlgebra;

namespace DressUpConverter
{
  class JointNode
  {
    public XmlNode xmlLocalMatrixNode = null;
    public XmlNode xmlGlobalMatrixNode = null;

    public string name = string.Empty;
    public string parentName = string.Empty;

    public JointNode parent = null;
    public List<JointNode> childList = new List<JointNode>();

    public Matrix<float> localMatrix = Matrix<float>.Build.DenseIdentity(4);
    public Matrix<float> globalMatrix = Matrix<float>.Build.DenseIdentity(4);

    public JointNode FindChild(string targetName)
    {
      if (this.name == targetName)
      {
        return this;
      }

      foreach (var child in this.childList)
      {
        var targetChild = child.FindChild(targetName);
        if (targetChild != null)
        {
          return targetChild;
        }
      }

      return null;
    }

    public void CalcMatrix(Matrix<float> parentMatrix)
    {
      //this.globalMatrix = this.localMatrix * parentMatrix;
      this.globalMatrix = parentMatrix * this.localMatrix;

      this.xmlLocalMatrixNode.InnerText = string.Join(" ", this.localMatrix.ToColumnWiseArray().Select((f) => (decimal)f)); // decimalにしないと指数表記にされることがある
      this.xmlGlobalMatrixNode.InnerText = string.Join(" ", this.globalMatrix.ToColumnWiseArray().Select((f) => (decimal)f)); // decimalにしないと指数表記にされることがある

      foreach (var child in this.childList)
      {
        child.CalcMatrix(this.globalMatrix);
      }
    }

    static public Tuple<List<string>, Dictionary<string, int>> CreateJointListAndDict(XmlNode rootNode)
    {
      if (rootNode == null)
      {
        return null;
      }

      var skeltonList = new List<string>();
      var skeltonDict = new Dictionary<string, int>();

      foreach (XmlNode node in rootNode.SelectNodes("Skelton/Node"))
      {
        var skeltonName = node.Attributes["Name"].Value;
        skeltonDict.Add(skeltonName, skeltonList.Count);
        skeltonList.Add(skeltonName);
      }

      return Tuple.Create(skeltonList, skeltonDict);
    }

  }
}
