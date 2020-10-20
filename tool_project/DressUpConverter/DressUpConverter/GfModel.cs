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
  class GfModel
  {
    // コンストラクタ
    public GfModel(string modelPath)
    {
      this.xmlDocument = new XmlDocument();
      this.xmlDocument.Load(modelPath);

      // GfModelはよく使うのでキャッシュ
      this.gfModelNode = this.xmlDocument.SelectSingleNode("GfModel");

      // スケルトンリスト作成
      this.MakeSkeltonList();

      // スキニングに使われているスケルトンのリストを作成
      this.MakeUsedSkeltonList();
    }

    // 変更内容を保存する
    public void Save(string filename)
    {
      this.xmlDocument.Save(filename);
    }

    // スケルトンリスト作成
    private void MakeSkeltonList()
    {
      var listAndDict = JointNode.CreateJointListAndDict(this.gfModelNode);
      this.skeltonList = listAndDict.Item1;
      this.skeltonDict = listAndDict.Item2;
    }

    // 全ての頂点ノードを巡回
    private void EachVertexData(Action<XmlNode> action, int elemCount, string usage)
    {
      foreach (XmlNode mesh in this.gfModelNode.SelectNodes("Meshes/Mesh"))
      {
        foreach (XmlNode vector in mesh.SelectNodes("VertexData/Vector" + elemCount))
        {
          if (vector.Attributes["Usage"].Value != usage)
          {
            continue;
          }

          action(vector);
        }
      }
    }

    // 全てのメッシュからジョイントの情報を削除
    private void RemoveAllJointFromMesh()
    {
      foreach (XmlNode mesh in this.gfModelNode.SelectNodes("Meshes/Mesh"))
      {
        var vertexData = mesh.SelectSingleNode("VertexData");
        foreach (XmlNode vector in vertexData.SelectNodes("Vector4"))
        {
          var usage = vector.Attributes["Usage"].Value;
          if ((usage == "JointNo") || (usage == "JointWeight"))
          {
            vertexData.RemoveChild(vector);
          }
        }
      }
    }

    // 全てのジョイント番号ノードを巡回
    private void EachJointNo(Action<XmlNode> action)
    {
      this.EachVertexData(action, 4, "JointNo");
    }

    // 全てのポジションノードを巡回
    private void EachPosition(Action<XmlNode> action)
    {
      this.EachVertexData(action, 3, "Position");
    }

    // 全ての法線ノードを巡回
    private void EachNormal(Action<XmlNode> action)
    {
      this.EachVertexData(action, 3, "Normal");
    }

    // スキニングに使われているスケルトンのリストを作成
    private void MakeUsedSkeltonList()
    {
      var usedSkeltonIndexList = new List<int>();

      // 全てのジョイント番号ノードを巡回
      this.EachJointNo((vector4) => {

        var jointNoList = vector4.InnerText.Split().Select((jointNo) => (int)float.Parse(jointNo));
        foreach (var jointNo in jointNoList)
        {
          // ジョイント番号が０より小さいものは無効
          if (jointNo < 0)
          {
            continue;
          }

          // すでに含まれているものは要らない
          if (usedSkeltonIndexList.Contains(jointNo))
          {
            continue;
          }

          usedSkeltonIndexList.Add(jointNo);
        }
      });

      usedSkeltonIndexList.Sort();
      this.usedSkeltonList = usedSkeltonIndexList.Select((index) => this.skeltonList[index]).ToList();
    }

    // 着せ替え用にスケルトンをコンバート
    public bool ConvertSkelton(Def.ConvertType convType, GfModel baseModel)
    {
      foreach (var skeltonName in this.usedSkeltonList)
      {
        if (skeltonName.Contains("_attach"))
        {
          // 既にコンバート済み
          return true;
        }
      }

      var subSkeltonList = this.usedSkeltonList.Except(baseModel.skeltonList).ToArray();

      // ０なら普通のスケルトン共有
      if (subSkeltonList.Length == 0)
      {
        this.ConvertSkeltonShared(baseModel);
        return true;
      }

      // 拡張スケルトンがある

      // ルートになる可能性のボーンを探す
      var rootBoneNameList = this.usedSkeltonList.Except(subSkeltonList).ToArray();

      if (rootBoneNameList.Length == 0)
      {
        // 既にコンバート済み
        return true;
      }

      var rootBoneName = this.GetRootBoneName(rootBoneNameList);
      if (string.IsNullOrEmpty(rootBoneName))
      {
        return false;
      }

      this.ConvertSkeltonNoShared(baseModel, rootBoneName);
      return true;
    }

    public bool ConvertAcc(Def.ConvertType convType)
    {
      // ピボット取得
      var pivotNode = this.gfModelNode.SelectSingleNode("Skelton/Node/LocalScalePivot");
      if (pivotNode == null)
      {
        // 見つからない
        return true;
      }

      // ピボット位置取得
      var pivotPosition = pivotNode.InnerText.Split().Select((value) => float.Parse(value)).ToArray();
      var pivotMatrix = Matrix<float>.Build.DenseIdentity(4);
      pivotMatrix[0, 3] = pivotPosition[0];
      pivotMatrix[1, 3] = pivotPosition[1];
      pivotMatrix[2, 3] = pivotPosition[2];

      // 原点に移動させる
      var invMatrix = pivotMatrix.Inverse();
      this.TransformPosition(invMatrix);

      // Skeltonを削除
      var skeltonNode = this.gfModelNode.SelectSingleNode("Skelton");
      this.gfModelNode.RemoveChild(skeltonNode);

      // MeshからJointを削除
      this.RemoveAllJointFromMesh();

      return true;
    }

    // ベースジョイントのコンバート
    public void ConvertBaseJoint(GfModel partsModel)
    {
      var subSkeltonList = partsModel.usedSkeltonList.Except(this.skeltonList).ToArray();

      // ０なら拡張スケルトンなし
      if (subSkeltonList.Length == 0)
      {
        return;
      }

      // 拡張スケルトンを追加
      var baseSkelton = this.gfModelNode.SelectSingleNode("Skelton");
      foreach (var skeltonName in subSkeltonList)
      {
        var partsSkelton = partsModel.gfModelNode.SelectSingleNode("Skelton");

        foreach (XmlNode node in partsSkelton.SelectNodes("Node"))
        {
          var nodeName = node.Attributes["Name"].Value;
          if (nodeName != skeltonName)
          {
            continue;
          }

          // このノードを追加する
          var importedNode = this.xmlDocument.ImportNode(node, true);
          baseSkelton.AppendChild(importedNode);
          break;
        }
      }
    }

    // 頂点バッファのジョイント番号を現在のジョイントに合わせる
    private void UpdateVertexJointNo(Dictionary<string, int> skeltonDict, List<string> skeltonList)
    {
      this.EachJointNo((jointNoNode) => {

        var jointNoList = jointNoNode.InnerText.Split().Select((jointNo) => (int)float.Parse(jointNo));
        var newJointNoList = jointNoList.Select((jointNo) => {

          if (jointNo < 0)
          {
            return jointNo;
          }

          var skeltonName = skeltonList[jointNo];
          return skeltonDict[skeltonName];
        });

        jointNoNode.InnerText = string.Join(" ", newJointNoList);
      });
    }

    // 頂点バッファのポジションを変換する
    private void TransformPosition(Matrix<float> matrix)
    {
      this.EachPosition((positionNode) => {

        var posList = positionNode.InnerText.Split().Select((pos) => float.Parse(pos)).ToArray();

        for (var i = 0; i < posList.Length; i += 3)
        {
          var vec4Pos = matrix * Vector<float>.Build.Dense(new float[] {
            posList[i], posList[i + 1], posList[i + 2], 1.0f
          });

          posList[i] = vec4Pos[0];
          posList[i + 1] = vec4Pos[1];
          posList[i + 2] = vec4Pos[2];
        }

        positionNode.InnerText = string.Join(" ", posList.Select((f) => (decimal)f)); // decimalにしないと指数表記にされることがある
      });
    }

    // 頂点バッファの法線を変換する
    private void TransformNormal(Matrix<float> matrix)
    {
      this.EachNormal((normalNode) => {

        var norList = normalNode.InnerText.Split().Select((nor) => float.Parse(nor)).ToArray();

        for (var i = 0; i < norList.Length; i += 3)
        {
          var vec4Nor = matrix * Vector<float>.Build.Dense(new float[] {
            norList[i], norList[i + 1], norList[i + 2], 1.0f
          });

          norList[i] = vec4Nor[0];
          norList[i + 1] = vec4Nor[1];
          norList[i + 2] = vec4Nor[2];
        }

        normalNode.InnerText = string.Join(" ", norList.Select((f) => (decimal)f)); // decimalにしないと指数表記にされることがある
      });
    }

    // スケルトン共有用にコンバート
    private void ConvertSkeltonShared(GfModel baseModel)
    {
      // ベースモデルのスケルトンをそのままコピーする
      var oldSkelton = this.gfModelNode.SelectSingleNode("Skelton");
      var newSkelton = this.xmlDocument.ImportNode(baseModel.gfModelNode.SelectSingleNode("Skelton"), true);

      this.gfModelNode.ReplaceChild(newSkelton, oldSkelton);

      // 新しいジョイント番号に合わせる
      this.UpdateVertexJointNo(baseModel.skeltonDict, this.skeltonList);
    }

    // ルートジョイント名を取得
    private string GetRootBoneName(string[] rootBoneNameList)
    {
      if (rootBoneNameList.Length == 1)
      {
        return rootBoneNameList.First();
      }

      // ジョイントノード構築
      var jointNodeDict = new Dictionary<string, JointNode>();

      var skeltonNode = this.gfModelNode.SelectSingleNode("Skelton");
      foreach (XmlNode node in skeltonNode.SelectNodes("Node"))
      {
        var nameAttr = node.Attributes["Name"];
        var parentNameAttr = node.Attributes["ParentName"];

        var skeltonName = nameAttr.Value;
        var parentName = (parentNameAttr == null) ? string.Empty : parentNameAttr.Value;

        var jointNode = new JointNode();
        jointNode.name = skeltonName;
        jointNode.parentName = parentName;

        jointNodeDict.Add(skeltonName, jointNode);
      }

      // 親を解決
      foreach (var jointNode in jointNodeDict)
      {
        jointNodeDict.TryGetValue(jointNode.Value.parentName, out jointNode.Value.parent);
        if (jointNode.Value.parent != null)
        {
          jointNode.Value.parent.childList.Add(jointNode.Value);
        }
      }

      var rootFlagList = new bool[rootBoneNameList.Length];
      for (int i = 0; i < rootBoneNameList.Length; ++i)
      {
        rootFlagList[i] = true;
      }

      for (int i = 0; i < rootBoneNameList.Length; ++i)
      {
        var name0 = rootBoneNameList[i];
        if (!rootFlagList[i])
        {
          continue;
        }

        for (int j = 0; j < rootBoneNameList.Length; ++j)
        {
          if (i == j)
          {
            continue;
          }

          var name1 = rootBoneNameList[j];
          var child = jointNodeDict[name0].FindChild(name1);
          if (child != null)
          {
            rootFlagList[j] = false;
          }
        }
      }

      var newRootBoneNameList = new List<string>();
      for (int i = 0; i < rootBoneNameList.Length; ++i)
      {
        if (rootFlagList[i])
        {
          newRootBoneNameList.Add(rootBoneNameList[i]);
        }
      }

      if (newRootBoneNameList.Count == 0)
      {
        // 何かおかしい
        return string.Empty;
      }

      if (newRootBoneNameList.Count == 1)
      {
        // 見つかった
        return newRootBoneNameList.First();
      }

      // おそらく共通の親を持つ
      var rootName = jointNodeDict[newRootBoneNameList.First()].parentName;
      for (int i = 1; i < newRootBoneNameList.Count; ++i)
      {
        if (rootName != jointNodeDict[newRootBoneNameList[i]].parentName)
        {
          return string.Empty;
        }
      }

      return rootName;
    }

    // スケルトン共有しない用にコンバート
    private void ConvertSkeltonNoShared(GfModel baseModel, string rootBoneName)
    {
      // 使われていない骨は削除する

      var skeltonNode = this.gfModelNode.SelectSingleNode("Skelton");
      var removeList = new List<XmlNode>();
      foreach (XmlNode node in skeltonNode.SelectNodes("Node"))
      {
        var skeltonName = node.Attributes["Name"].Value;
        if (skeltonName == rootBoneName)
        {
          continue;
        }
        if (!this.usedSkeltonList.Contains(skeltonName))
        {
          removeList.Add(node);
        }
      }
      foreach (var node in removeList)
      {
        skeltonNode.RemoveChild(node);
      }

      // 新しいスケルトンリストを作成
      var oldSkeltonList = new List<string>(this.skeltonList); // 古いスケルトンリストをディープコピー
      this.MakeSkeltonList();

      // 新しいジョイント番号に合わせる
      this.UpdateVertexJointNo(this.skeltonDict, oldSkeltonList);

      // ルートジョイントの名前変更
      var newRootBoneName = rootBoneName + "_attach";
      foreach (XmlNode node in skeltonNode.SelectNodes("Node"))
      {
        var skeltonNameAttr = node.Attributes["Name"];
        var parentNameAttr = node.Attributes["ParentName"];

        // rootBoneNameならParentNameを削除する
        if (skeltonNameAttr.Value == rootBoneName)
        {
          node.Attributes.Remove(parentNameAttr);
          skeltonNameAttr.Value = newRootBoneName;

          // SRTを単位行列になるように調整
          node.SelectSingleNode("LocalScale").InnerText = "1 1 1";
          node.SelectSingleNode("LocalRotation").InnerText = "0 0 0";
          node.SelectSingleNode("LocalTransform").InnerText = "0 0 0";
        }
        // ParentNameがrootBoneNameなら名前変更
        else if (parentNameAttr != null)
        {
          if (parentNameAttr.Value == rootBoneName)
          {
            parentNameAttr.Value = newRootBoneName;
          }
        }
      }

      // ジョイントノード構築
      var jointNodeList = new List<JointNode>();

      foreach (XmlNode node in skeltonNode.SelectNodes("Node"))
      {
        var nameAttr = node.Attributes["Name"];
        var parentNameAttr = node.Attributes["ParentName"];

        var skeltonName = nameAttr.Value;
        var parentName = (parentNameAttr == null) ? string.Empty : parentNameAttr.Value;

        var localMatrixNode = node.SelectSingleNode("LocalMatrix");
        var globalMatrixNode = node.SelectSingleNode("GlobalMatrix");

        var jointNode = new JointNode();
        jointNode.xmlLocalMatrixNode = localMatrixNode;
        jointNode.xmlGlobalMatrixNode = globalMatrixNode;
        jointNode.name = skeltonName;
        jointNode.parentName = parentName;
        jointNode.localMatrix = Matrix<float>.Build.DenseOfColumnMajor(4, 4, localMatrixNode.InnerText.Split().Select((val) => float.Parse(val)));
        jointNode.globalMatrix = Matrix<float>.Build.DenseOfColumnMajor(4, 4, globalMatrixNode.InnerText.Split().Select((val) => float.Parse(val)));

        jointNodeList.Add(jointNode);
      }

      // 親を探す
      foreach (var jointNode in jointNodeList)
      {
        if (string.IsNullOrEmpty(jointNode.parentName))
        {
          continue;
        }

        jointNode.parent = jointNodeList.Find((node) => (node.name == jointNode.parentName));
      }
      // 子を探す
      foreach (var jointNode in jointNodeList)
      {
        jointNode.childList = jointNodeList.FindAll((node) => (node.parent == jointNode));
      }

      // グローバル行列計算
      var topNode = jointNodeList.Find((node) => (node.parent == null));
      var originamGlobalMatrix = topNode.globalMatrix;
      topNode.localMatrix = Matrix<float>.Build.DenseIdentity(4);
      topNode.CalcMatrix(topNode.localMatrix);

      // Positionをローカル座標系に変換
      var invPositionMatrix = originamGlobalMatrix.Inverse();
      this.TransformPosition(invPositionMatrix);

      // Normalをローカル座標系に変換
      originamGlobalMatrix[0, 3] = 0.0f;
      originamGlobalMatrix[1, 3] = 0.0f;
      originamGlobalMatrix[2, 3] = 0.0f;
      var invNormalMatrix = originamGlobalMatrix.Inverse();
      this.TransformNormal(invNormalMatrix);
    }

    private XmlDocument xmlDocument = null;
    private XmlNode gfModelNode = null;

    public List<string> skeltonList = new List<string>();
    private Dictionary<string, int> skeltonDict = new Dictionary<string, int>();

    public List<string> usedSkeltonList = new List<string>();
  }
}
