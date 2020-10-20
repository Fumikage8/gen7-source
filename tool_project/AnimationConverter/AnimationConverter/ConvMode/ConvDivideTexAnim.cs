using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

using Gfmot;

namespace AnimationConverter
{
  class ConvDivideTexAnim
  {
    public string ErrorMessage { get; set; }
    public string ErrorStackTrance { get; set; }

    // マテリアルリストに追加
    static void AddDivideTexAnim(List<string>[] materialList, DivideTexAnim divideTexAnim, string filename)
    {
      if (divideTexAnim == null)
      {
        return;
      }
      if (divideTexAnim.EXCEPTION == null)
      {
        return;
      }

      var modeEye = filename.Contains("eye");

      foreach (var exeption in divideTexAnim.EXCEPTION)
      {
        // 同じファイル名のものを対象に
        if (exeption._FileName != filename)
        {
          continue;
        }

        // ファイルタイプからインデックス取得
        string indexStr = null;
        if (exeption._FileType.Contains("eye"))
        {
          if (!modeEye)
          {
            continue;
          }

          indexStr = exeption._FileType.Substring("eye".Length);
        }
        else
        {
          if (modeEye)
          {
            continue;
          }

          indexStr = exeption._FileType.Substring("mouth".Length);
        }

        var index = int.Parse(indexStr) - 1;

        // 追加する
        materialList[index].Add(exeption._Parts);
      }
    }

    // デフォルト目パチマテリアルリスト作成
    static List<string>[] CreateDefaultEyeList(DivideTexAnim divideTexAnim, GfMotion gfMotion, string filename)
    {
      if (gfMotion == null)
      {
        return null;
      }

      // 残すマテリアル名リスト、このリスト以外のアニメーションは削除
      var eyeMaterialList = new List<string>[4]
      {
        // eye01
        new List<string>()
        {
          "Eye",
          "REye",
          "Eye_L",
          "AEye",
          "EyeA",
        },
        // eye02
        new List<string>()
        {
          "LEye",
          "Eye_R",
          "BEye",
          "EyeB",
        },
        // eye03
        new List<string>()
        {
          "CEye",
          "EyeC",
        },
        // eye04
        new List<string>()
        {
          "DEye",
          "EyeD",
        },
      };

      AddDivideTexAnim(eyeMaterialList, divideTexAnim, filename);

      var exceptionIrisExist = false;
      foreach (var material in eyeMaterialList)
      {
        foreach (var name in material)
        {
          if (name == "Iris")
          {
            exceptionIrisExist = true;
            break;
          }
        }
      }

      // 目が左右に分かれているかどうかで分岐
      if (!exceptionIrisExist)
      {
        var oneEye = false;
        foreach (var material in gfMotion.UvAnimation.Material)
        {
          if (material.Name == "Eye")
          {
            oneEye = true;
            break;
          }
        }

        eyeMaterialList[0].Add("RIris");
        if (oneEye)
        {
          eyeMaterialList[0].Add("LIris");
        }
        else
        {
          eyeMaterialList[1].Add("LIris");
        }
      }

      return eyeMaterialList;
    }

    // デフォルト口パクマテリアルリスト作成
    static List<string>[] CreateDefaultMouthList(DivideTexAnim divideTexAnim, GfMotion gfMotion, string filename)
    {
      if (gfMotion == null)
      {
        return null;
      }

      // 残すマテリアル名リスト、このリスト以外のアニメーションは削除
      var mouthMaterialList = new List<string>[4]
      {
        // mouth01
        new List<string>()
        {
          "Mouth",
          "RMouth",
          "AMouth",
          "MouthA",
        },
        // mouth02
        new List<string>()
        {
          "LMouth",
          "BMouth",
          "MouthB",
        },
        // mouth03
        new List<string>()
        {
          "CMouth",
          "MouthC",
        },
        // mouth04
        new List<string>()
        {
          "DMouth",
          "MouthD",
        },
      };

      AddDivideTexAnim(mouthMaterialList, divideTexAnim, filename);

      return mouthMaterialList;
    }

    // 目パチのマテリアルコンバート
    static void ConvEyeMaterial(DivideTexAnim divideTexAnim, GfMotion gfMotion, string filename, int eyeIndex)
    {
      if (gfMotion == null)
      {
        return;
      }
      if (gfMotion.UvAnimation == null)
      {
        return;
      }
      if (Common.IsNullOrEmpty(gfMotion.UvAnimation.Material))
      {
        return;
      }

      var eyeMaterialList = CreateDefaultEyeList(divideTexAnim, gfMotion, filename);

      // 目パチ用のマテリアルリスト作成
      var newMaterialList = new List<UvAnimation_Material>();

      foreach (var material in gfMotion.UvAnimation.Material)
      {
        if (eyeMaterialList[eyeIndex].Contains(material.Name))
        {
          newMaterialList.Add(material);
        }
      }

      gfMotion.UvAnimation.Material = newMaterialList.ToArray();
    }

    // 口パクのマテリアルコンバート
    static void ConvMouthMaterial(DivideTexAnim divideTexAnim, GfMotion gfMotion, string filename, int mouthIndex)
    {
      if (gfMotion == null)
      {
        return;
      }
      if (gfMotion.UvAnimation == null)
      {
        return;
      }
      if (Common.IsNullOrEmpty(gfMotion.UvAnimation.Material))
      {
        return;
      }

      // 口パク用のマテリアルリスト作成
      var mouthMaterialList = CreateDefaultMouthList(divideTexAnim, gfMotion, filename);

      var newMaterialList = new List<UvAnimation_Material>();

      foreach (var material in gfMotion.UvAnimation.Material)
      {
        if (mouthMaterialList[mouthIndex].Contains(material.Name))
        {
          newMaterialList.Add(material);
        }
      }

      gfMotion.UvAnimation.Material = newMaterialList.ToArray();
    }

    // 動いているビジビリティだけを残す
    static void ConvVisibility_ZeroMotion(GfMotion gfMotion)
    {
      if (gfMotion == null)
      {
        return;
      }
      if (gfMotion.VisibilityAnimation == null)
      {
        return;
      }
      if (Common.IsNullOrEmpty(gfMotion.VisibilityAnimation.Mesh))
      {
        gfMotion.VisibilityAnimation = null;
        return;
      }

      gfMotion.VisibilityAnimation.Mesh = gfMotion.VisibilityAnimation.Mesh.Where((mesh) => mesh.BoolKey.Length > 1).ToArray();
      if (gfMotion.VisibilityAnimation.Mesh.Length == 0)
      {
        gfMotion.VisibilityAnimation = null;
      }
    }

    // 動いているシェーダーアニメだけを残す
    static void ConvShader_ZeroMotion(GfMotion gfMotion)
    {
      if (gfMotion == null)
      {
        return;
      }
      if (gfMotion.ShaderAnimation == null)
      {
        return;
      }
      if (Common.IsNullOrEmpty(gfMotion.ShaderAnimation.Material))
      {
        gfMotion.ShaderAnimation = null;
        return;
      }

      for (var i = 0; i < gfMotion.ShaderAnimation.Material.Length; ++i)
      {
        for (var j = 0; j < gfMotion.ShaderAnimation.Material[i].ConstantColor.Length; ++j)
        {
          var colors = gfMotion.ShaderAnimation.Material[i].ConstantColor[j];

          // RGBA全てのにキーが無いなら削除、どれか一つにでもキーがあれば残す
          if (!Common_Keys.HasHermiteKeys(colors.ColorR) 
            && !Common_Keys.HasHermiteKeys(colors.ColorG)
            && !Common_Keys.HasHermiteKeys(colors.ColorB)
            && !Common_Keys.HasHermiteKeys(colors.ColorA))
          {
            gfMotion.ShaderAnimation.Material[i].ConstantColor[j].ColorR = null;
            gfMotion.ShaderAnimation.Material[i].ConstantColor[j].ColorG = null;
            gfMotion.ShaderAnimation.Material[i].ConstantColor[j].ColorB = null;
            gfMotion.ShaderAnimation.Material[i].ConstantColor[j].ColorA = null;
          }
        }

        gfMotion.ShaderAnimation.Material[i].ConstantColor = 
          gfMotion.ShaderAnimation.Material[i].ConstantColor.Where((color) => (color.ColorR != null) || (color.ColorG != null) || (color.ColorB != null) || (color.ColorA != null)).ToArray();
      }

      gfMotion.ShaderAnimation.Material =
        gfMotion.ShaderAnimation.Material.Where((material) => material.ConstantColor.Length > 0).ToArray();

      if (gfMotion.ShaderAnimation.Material.Length == 0)
      {
        gfMotion.ShaderAnimation = null;
      }
    }

    // 動いているUVアニメだけを残す
    static void ConvUV_ZeroMotion(GfMotion gfMotion)
    {
      if (gfMotion == null)
      {
        return;
      }
      if (gfMotion.UvAnimation == null)
      {
        return;
      }
      if (Common.IsNullOrEmpty(gfMotion.UvAnimation.Material))
      {
        gfMotion.UvAnimation = null;
        return;
      }

      for (var i = 0; i < gfMotion.UvAnimation.Material.Length; ++i)
      {
        // nullなら無視
        if (gfMotion.UvAnimation.Material[i].TextureSlot == null)
        {
          continue;
        }

        for (var j = 0; j < gfMotion.UvAnimation.Material[i].TextureSlot.Length; ++j)
        {
          var textureSlot = gfMotion.UvAnimation.Material[i].TextureSlot[j];

          // scale
          // UVどちらもキーが無いなら削除、どちらか一つにでもキーがあれば残す
          if (!Common_Keys.HasHermiteKeys(textureSlot.ScaleU) && !Common_Keys.HasHermiteKeys(textureSlot.ScaleV))
          {
            gfMotion.UvAnimation.Material[i].TextureSlot[j].ScaleU = null;
            gfMotion.UvAnimation.Material[i].TextureSlot[j].ScaleV = null;
          }
          // rotate
          if (!Common_Keys.HasHermiteKeys(textureSlot.Rotate))
          {
            gfMotion.UvAnimation.Material[i].TextureSlot[j].Rotate = null;
          }
          // translate
          // UVどちらもキーが無いなら削除、どちらか一つにでもキーがあれば残す
          if (!Common_Keys.HasHermiteKeys(textureSlot.TranslateU) && !Common_Keys.HasHermiteKeys(textureSlot.TranslateV)) 
          {
            gfMotion.UvAnimation.Material[i].TextureSlot[j].TranslateU = null;
            gfMotion.UvAnimation.Material[i].TextureSlot[j].TranslateV = null;
          }
        }

        gfMotion.UvAnimation.Material[i].TextureSlot =
          gfMotion.UvAnimation.Material[i].TextureSlot.Where((slot) => (slot.ScaleU != null) || (slot.ScaleV != null) || (slot.Rotate != null) || (slot.TranslateU != null) || (slot.TranslateV != null)).ToArray();
      }

      gfMotion.UvAnimation.Material =
        gfMotion.UvAnimation.Material.Where((material) => 
          {
            if (material.TextureSlot == null)
            {
              return false;
            }
            return (material.TextureSlot.Length > 0);
          }).ToArray();

      if (gfMotion.UvAnimation.Material.Length == 0)
      {
        gfMotion.UvAnimation.Material = null;
      }
    }

    // 動いているテクスチャパターンアニメだけを残す
    static void ConvTexturePattern_ZeroMotion(GfMotion gfMotion)
    {
      if (gfMotion == null)
      {
        return;
      }
      if (gfMotion.TexturePatternAnimation == null)
      {
        return;
      }
      if (Common.IsNullOrEmpty(gfMotion.TexturePatternAnimation.Material))
      {
        gfMotion.TexturePatternAnimation = null;
        return;
      }

      for (var i = 0; i < gfMotion.TexturePatternAnimation.Material.Length; ++i)
      {
        gfMotion.TexturePatternAnimation.Material[i].TextureSlot = gfMotion.TexturePatternAnimation.Material[i].TextureSlot.Where((textureSlot) => textureSlot.Attribute.Length > 0).ToArray();
      }

      gfMotion.TexturePatternAnimation.Material =
        gfMotion.TexturePatternAnimation.Material.Where((material) => material.TextureSlot.Length > 0).ToArray();

      if (gfMotion.TexturePatternAnimation.Material.Length == 0)
      {
        gfMotion.TexturePatternAnimation.Material = null;
      }
    }

    // 動いているアニメーションだけを残す
    static void ConvZeroMotion(GfMotion gfMotion)
    {
      ConvVisibility_ZeroMotion(gfMotion);
      ConvShader_ZeroMotion(gfMotion);
      ConvUV_ZeroMotion(gfMotion);
      ConvTexturePattern_ZeroMotion(gfMotion);

      if (gfMotion != null)
      {
        gfMotion.BlendShapeAnimation = null;
        gfMotion.AttributeKeyAnimation = null;
      }
    }

    // ループジョイント、目パチ口パク用の共通コンバート：コア
    static JointAnimation_Node[] ConvLoopJoint_EyeMouthCore(GfMotion gfMotion, string[] animJointList, bool addBindposeFlag)
    {
      if (gfMotion == null)
      {
        return null;
      }
      if (gfMotion.SkeltalAnimation == null)
      {
        return null;
      }
      if (Common.IsNullOrEmpty(gfMotion.SkeltalAnimation.Node))
      {
        return null;
      }

      if (gfMotion.Skelton == null)
      {
        return null;
      }
      if (Common.IsNullOrEmpty(gfMotion.Skelton.Node))
      {
        return null;
      }

      if (Common.IsNullOrEmpty(animJointList))
      {
        return null;
      }

      var jointDict = new Dictionary<string, Skelton_Node>();

      // 階層構造構築
      foreach (var joint in gfMotion.Skelton.Node)
      {
        var name = joint.Name;

        if (jointDict.ContainsKey(name))
        {
          throw new Exception("ジョイント名が重複しています [ " + name + " ]");
        }

        jointDict.Add(name, joint);
      }
      foreach (var joint in gfMotion.Skelton.Node)
      {
        if (string.IsNullOrEmpty(joint.ParentName))
        {
          continue;
        }
        var parentName = joint.ParentName;
        if (!jointDict.ContainsKey(parentName))
        {
          continue;
        }
        var parent = jointDict[parentName];
        joint.Parent = parent;
        if (parent.ChildList == null)
        {
          parent.ChildList = new List<Skelton_Node>();
        }
        parent.ChildList.Add(joint);
      }

      var newAnimNodeList = new List<JointAnimation_Node>();
      foreach (var animNode in gfMotion.SkeltalAnimation.Node)
      {
        var name = animNode.Name;

        // 含まれているか？
        if (animJointList.Contains(name))
        {
          newAnimNodeList.Add(animNode);
          continue;
        }

        // 親が含まれているかもチェック
        var parent = jointDict[name].Parent;
        while (parent != null)
        {
          var parentName = parent.Name;

          if (animJointList.Contains(parentName))
          {
            newAnimNodeList.Add(animNode);
            break;
          }

          parent = parent.Parent;
        }
      }

      if (addBindposeFlag)
      {
        // 上書きするジョイントを全て列挙
        var addJointList = new List<string>();
        foreach (var animJoint in animJointList)
        {
          addJointList.AddRange(Gfmot.Skelton_Node.GetAllJoint(jointDict, animJoint));
          addJointList.Add(animJoint);
        }

        foreach (var addJoint in addJointList)
        {
          // Endジョイントは無視
          if (addJoint.StartsWith("End"))
          {
            continue;
          }

          // 追加済みなら無視
          if (newAnimNodeList.Where((node) => node.Name == addJoint).Count() != 0)
          {
            continue;
          }

          // 追加
          var addNode = new JointAnimation_Node();
          addNode.Name = addJoint;

          newAnimNodeList.Add(addNode);
        }

        // アニメーションしていない要素は全てバインドポーズで埋める
        for (var i = 0; i < newAnimNodeList.Count; ++i)
        {
          var addNode = newAnimNodeList[i];

          var joint = jointDict[addNode.Name];
          var localScale = joint.LocalScale.Split();
          var localRotate = joint.LocalRotation.Split();
          var localTrans = joint.LocalTransform.Split();

          if (addNode.ScaleX == null)
          {
            addNode.ScaleX = Common_Keys.CreateOneFrameKey(float.Parse(localScale[0]));
          }
          if (addNode.ScaleY == null)
          {
            addNode.ScaleY = Common_Keys.CreateOneFrameKey(float.Parse(localScale[1]));
          }
          if (addNode.ScaleZ == null)
          {
            addNode.ScaleZ = Common_Keys.CreateOneFrameKey(float.Parse(localScale[2]));
          }

          if (addNode.RotateX == null)
          {
            addNode.RotateX = Common_Keys.CreateOneFrameKey(float.Parse(localRotate[0]));
          }
          if (addNode.RotateY == null)
          {
            addNode.RotateY = Common_Keys.CreateOneFrameKey(float.Parse(localRotate[1]));
          }
          if (addNode.RotateZ == null)
          {
            addNode.RotateZ = Common_Keys.CreateOneFrameKey(float.Parse(localRotate[2]));
          }

          if (addNode.TranslateX == null)
          {
            addNode.TranslateX = Common_Keys.CreateOneFrameKey(float.Parse(localTrans[0]));
          }
          if (addNode.TranslateY == null)
          {
            addNode.TranslateY = Common_Keys.CreateOneFrameKey(float.Parse(localTrans[1]));
          }
          if (addNode.TranslateZ == null)
          {
            addNode.TranslateZ = Common_Keys.CreateOneFrameKey(float.Parse(localTrans[2]));
          }
        }
      }

      return newAnimNodeList.ToArray();
    }


    // ループジョイント、目パチ口パク用の共通コンバート
    static void ConvLoopJoint_EyeMouth(GfMotion gfMotion, string[] animJointList, bool addBindposeFlag)
    {
      if (gfMotion == null)
      {
        return;
      }
      if (gfMotion.SkeltalAnimation == null)
      {
        return;
      }
      if (Common.IsNullOrEmpty(gfMotion.SkeltalAnimation.Node))
      {
        return;
      }

      gfMotion.SkeltalAnimation.Node = ConvLoopJoint_EyeMouthCore(gfMotion, animJointList, addBindposeFlag);
    }

    // 部分アニメ用コンバート
    public ReturnCode Conv(string inputDirectory, string outputDirectory)
    {
      // 入力ディレクトリが無ければ無視
      if (inputDirectory == null)
      {
        return ReturnCode.Error_InvalidArgument;
      }
      if (!Directory.Exists(inputDirectory))
      {
        return ReturnCode.Error_NotFoundInputPath;
      }

      // 出力ディレクトリが無ければ作成
      if (outputDirectory == null)
      {
        return ReturnCode.Error_InvalidArgument;
      }
      if (!Directory.Exists(outputDirectory))
      {
        Directory.CreateDirectory(outputDirectory);
      }

      bool isExportSuccess = true;
      try
      {
        // 例外ファイルがあるか検索
        DivideTexAnim divideTexAnim = null;
        var divideTexAnimPath = Directory.GetFiles(inputDirectory, "DivideTexAnim.xml");
        if ((divideTexAnimPath != null) && (divideTexAnimPath.Length > 0))
        {
          divideTexAnim = Common.DeserializeXml<DivideTexAnim>(divideTexAnimPath[0]);

          Console.WriteLine("DivideTexAnim.xml : " + divideTexAnimPath[0]);
        }

        // .mcnfがあるか検索
        Mcnf.Mcnf mcnf = null;
        var mcnfPath = Directory.GetFiles(inputDirectory, "*.mcnf");
        if ((mcnfPath != null) && (mcnfPath.Length > 0))
        {
          mcnf = Common.DeserializeXml<Mcnf.Mcnf>(mcnfPath[0]);

          Console.WriteLine("*.mcnf : " + mcnfPath[0]);
        }
        string[] eyeJointList = null;
        string[] mouthJointList = null;
        if (mcnf != null)
        {
          if (mcnf.EYE != null)
          {
            eyeJointList = mcnf.EYE.Select(eye => eye._Name).ToArray();
          }
          if (mcnf.MOUTH != null)
          {
            mouthJointList = mcnf.MOUTH.Select(mouth => mouth._Name).ToArray();
          }
        }

        // 目パチ口パクのコンバートは.mcnfがある場合のみに実行
        if (mcnf != null)
        {
          // 目パチ用ファイルのコンバート
          ConvEyeDivide(inputDirectory, outputDirectory, divideTexAnim, eyeJointList);

          // 口パク用ファイルのコンバート
          ConvMouthDivide(inputDirectory, outputDirectory, divideTexAnim, mouthJointList);
        }

        // ループジョイントアニメ用ファイルのコンバート
        ConvLoopDivide(inputDirectory, outputDirectory);
      }
      catch (Exception e)
      {
        ErrorMessage = e.Message;
        ErrorStackTrance = e.StackTrace;

        // 失敗するかもしれない
        isExportSuccess = false;
      }

      // ファイル出力失敗したか？
      if (!isExportSuccess)
      {
        return ReturnCode.Error_FaildExport;
      }

      return ReturnCode.Success;
    }

    // ループジョイントアニメ用ファイルのコンバート
    private static void ConvLoopDivide(string inputDirectory, string outputDirectory)
    {
      var optionPath = Path.Combine(inputDirectory, "LoopSkelData.txt");
      var loopJointList = (File.Exists(optionPath))
        ? File.ReadAllLines(optionPath)
        : null;

      var loopFiles = Directory.GetFiles(inputDirectory, "*loop*.gfmot");

      foreach (var loopFile in loopFiles)
      {
        var gfMotion = Common.DeserializeXml<Gfmot.GfMotion>(loopFile);

        Console.WriteLine("Convert : " + loopFile);

        // リスト内の必要なジョイントだけ残す
        ConvLoopJoint_EyeMouth(gfMotion, loopJointList, false);

        // 動いていないアニメーションは削除
        ConvZeroMotion(gfMotion);

        var outputPath = Path.Combine(outputDirectory, Path.GetFileName(loopFile));
        Common.SerializeXml<Gfmot.GfMotion>(outputPath, gfMotion);
      }
    }

    // 口パク用ファイルのコンバート
    private static void ConvMouthDivide(string inputDirectory, string outputDirectory, DivideTexAnim divideTexAnim, string[] mouthJointList)
    {
      var mouthFiles = Directory.GetFiles(inputDirectory, "*mouth*.gfmot");
      var addBindpose = (mouthFiles.Length == 1); // 複数口があるポケモン用

      for (var i = 0; i < mouthFiles.Length; ++i)
      {
        var mouthFile = mouthFiles[i];
        var gfMotion = Common.DeserializeXml<Gfmot.GfMotion>(mouthFile);

        Console.WriteLine("Convert : " + mouthFile);

        // リスト内の必要なマテリアルもしくはジョイントだけ残す
        ConvMouthMaterial(divideTexAnim, gfMotion, Path.GetFileNameWithoutExtension(mouthFile), i);
        ConvLoopJoint_EyeMouth(gfMotion, mouthJointList, addBindpose);

        // 動いていないアニメーションは削除
        ConvZeroMotion(gfMotion);

        var outputPath = Path.Combine(outputDirectory, Path.GetFileName(mouthFile));
        Common.SerializeXml<Gfmot.GfMotion>(outputPath, gfMotion);
      }
    }

    // 目パチ用ファイルのコンバート
    private static void ConvEyeDivide(string inputDirectory, string outputDirectory, DivideTexAnim divideTexAnim, string[] eyeJointList)
    {
      var eyeFiles = Directory.GetFiles(inputDirectory, "*eye*.gfmot");
      var addBindpose = (eyeFiles.Length == 1); // 複数目があるポケモン用

      for (var i = 0; i < eyeFiles.Length; ++i)
      {
        var eyeFile = eyeFiles[i];
        var gfMotion = Common.DeserializeXml<Gfmot.GfMotion>(eyeFile);

        Console.WriteLine("Convert : " + eyeFile);

        // リスト内の必要なマテリアルもしくはジョイントだけ残す
        ConvEyeMaterial(divideTexAnim, gfMotion, Path.GetFileNameWithoutExtension(eyeFile), i);
        ConvLoopJoint_EyeMouth(gfMotion, eyeJointList, addBindpose);

        // 動いていないアニメーションは削除
        ConvZeroMotion(gfMotion);

        var outputPath = Path.Combine(outputDirectory, Path.GetFileName(eyeFile));
        Common.SerializeXml<Gfmot.GfMotion>(outputPath, gfMotion);
      }
    }
  }
}
