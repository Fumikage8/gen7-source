using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;

namespace DressUpConverter
{
  class GfMotion
  {
    // コンストラクタ
    public GfMotion(string motionPath)
    {
      this.xmlDocument = new XmlDocument();
      this.xmlDocument.Load(motionPath);

      // GfMotionはよく使うのでキャッシュ
      this.gfMotionNode = this.xmlDocument.SelectSingleNode("GfMotion");

      // スケルトンリスト作成
      this.MakeSkeltonList();
    }

    // 変更内容を保存する
    public void Save(string filename)
    {
      this.xmlDocument.Save(filename);
    }

    // スケルトンリスト作成
    private void MakeSkeltonList()
    {
      var listAndDict = JointNode.CreateJointListAndDict(this.gfMotionNode);
      this.skeltonList = listAndDict.Item1;
      this.skeltonDict = listAndDict.Item2;
    }

    // モーションのマージ
    public void MargeMotion(GfModel baseModel, GfMotion partsMotion)
    {
      var addSkeltonName = baseModel.skeltonList.Last();

      // 拡張スケルトンを追加
      var baseSkelton = this.gfMotionNode.SelectSingleNode("Skelton");
      {
        var partsSkelton = partsMotion.gfMotionNode.SelectSingleNode("Skelton");

        foreach (XmlNode node in partsSkelton.SelectNodes("Node"))
        {
          var nodeName = node.Attributes["Name"].Value;
          if (!nodeName.Contains(addSkeltonName))
          {
            continue;
          }

          // このノードを追加する
          var importedNode = this.xmlDocument.ImportNode(node, true);
          baseSkelton.AppendChild(importedNode);
        }
      }

      // 拡張のアニメーションを追加
      var baseSkeltalAnimation = this.gfMotionNode.SelectSingleNode("SkeltalAnimation");
      {
        var partsSkeltalAnimation = partsMotion.gfMotionNode.SelectSingleNode("SkeltalAnimation");

        foreach (XmlNode node in partsSkeltalAnimation.SelectNodes("Node"))
        {
          var nodeName = node.Attributes["Name"].Value;
          if (!nodeName.Contains(addSkeltonName))
          {
            continue;
          }

          // このノードを追加する
          var importedNode = this.xmlDocument.ImportNode(node, true);
          baseSkeltalAnimation.AppendChild(importedNode);
        }
      }
    }

    // パーツモーションの加工
    public void ConvPartsMotion(GfModel baseModel, GfModel partsModel)
    {
      var baseSkeltonList = baseModel.skeltonList;
      var partsSkeltonList = partsModel.skeltonList;

      var removeNodeList = new List<XmlNode>();
      var separatorList = new char[]
      {
        ':'
      };

      var partsSkeltalAnimation = this.gfMotionNode.SelectSingleNode("SkeltalAnimation");
      if (partsSkeltalAnimation == null)
      {
        return;
      }

      // ベースモデルと骨構造が完全一致するならアニメーション自体を削除
      var subSkeltonList = partsSkeltonList.Except(baseSkeltonList).ToArray();
      if (subSkeltonList.Length == 0)
      {
        this.gfMotionNode.RemoveChild(partsSkeltalAnimation);
        return;
      }

      foreach (XmlNode node in partsSkeltalAnimation.SelectNodes("Node"))
      {
        var nodeNameFull = node.Attributes["Name"].Value;
        var nodeName = nodeNameFull.Split(separatorList).Last();

        // パーツモデルに含まれていないなら削除する
        var hasPartsModelSkelton = partsSkeltonList.Contains(nodeName);
        if (!hasPartsModelSkelton)
        {
          removeNodeList.Add(node);
        }
      }

      foreach (var node in removeNodeList)
      {
        partsSkeltalAnimation.RemoveChild(node);
      }

      // 子供がいなくなったら全て削除
      if (partsSkeltalAnimation.ChildNodes.Count == 0)
      {
        this.gfMotionNode.RemoveChild(partsSkeltalAnimation);
      }
    }

    private XmlDocument xmlDocument = null;
    private XmlNode gfMotionNode = null;

    private List<string> skeltonList = new List<string>();
    private Dictionary<string, int> skeltonDict = new Dictionary<string, int>();
  }
}
