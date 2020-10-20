
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace AnimationConverter
{
  class GfmotBinaryHeader
  {
    public enum AnimationType
    {
      Top,
      Joint,
      BlendShape,
      Uv,
      TexturePattern,
      Shader,
      Visibility,
      Key,
      EnvLight,
      EnvCamera,
      EnvFog,    //kawa20150514
      EnvGlare,  //kawa20150514
    }

    public GfmotBinaryHeader(AnimationType type, byte[] buffer)
    {
      this.type = type;
      this.buffer = buffer;
    }

    public AnimationType type = AnimationType.Top;
    public byte[] buffer = null;
  }

  class GfmotBinaryExporter : IDisposable
  {
    public const ushort FormatVersionMajor = 0;
    public const ushort FormatVersionMinor = 6;

    private enum SRTIndex
    {
      ScaleX,
      ScaleY,
      ScaleZ,

      RotateX,
      RotateY,
      RotateZ,

      TranslateX,
      TranslateY,
      TranslateZ,

      Count
    }

    private enum UVIndex
    {
      ScaleU,
      ScaleV,

      Rotate,

      TranslateU,
      TranslateV,

      Count
    }

    private enum ColorIndex
    {
      R,
      G,
      B,
      A,

      Count
    }

    private enum LightIndex
    {
      PositionX,
      PositionY,
      PositionZ,

      DirectionX,
      DirectionY,
      DirectionZ,

      ColorR,
      ColorG,
      ColorB,

      ConeAngle,
      Intensity,

      Count
    }

    private enum CameraIndex
    {
      Near,
      Far,
      Fovy,
      AspectRatio,

      RotateX,
      RotateY,
      RotateZ,

      TranslateX,
      TranslateY,
      TranslateZ,

      Count
    }

    //kawa20150514
    private enum FogIndex
    {
      NearLength,
      FarLength,

      ColorR,
      ColorG,
      ColorB,

      Strength,
      
      Count
    }

    private enum GlareIndex
    {
      GlareStrength,
      GlareRange,
      GlareIntensity,

      GlareWeightR,
      GlareWeightG,
      GlareWeightB,

      Count
    }

    public GfmotBinaryExporter(string fileName, string outputPath, Gfmot.GfMotion gfMotion, Gfmot.GfModel gfModel)
    {
      this.fileName = fileName;
      var outputDir = Path.GetDirectoryName(outputPath);
      if (!Directory.Exists(outputDir))
      {
        Directory.CreateDirectory(outputDir);
      }
      this.fs = File.Open(outputPath, FileMode.OpenOrCreate);

      this.bw = new BinaryWriter(this.fs);

      // 上書き用にサイズを０に
      this.bw.BaseStream.SetLength(0);

      this.gfMotion = gfMotion;
      this.gfModel = gfModel;
    }

    public void Dispose()
    {
      this.bw.Dispose();
      this.fs.Dispose();
    }

    // バイナリ出力
    public bool ExportBinary(Mcnf.CompSetting.Levels compLevel, bool isPokeMode)
    {
      this.setting.Level = compLevel;
      this.isPokeMode = isPokeMode;

      // ヘッダー出力
      if (!this.ExportHeader())
      {
        return false;
      }

      // データリスト
      var dataList = new List<GfmotBinaryHeader>();

      // アニメーショントップ出力
      var bufTop = this.ExportAnimationTop();
      if (bufTop != null)
      {
        dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.Top, bufTop));
      }

      // ジョイントアニメーション出力
      var bufJoint = this.ExportJointAnimation();
      if (bufJoint != null)
      {
        dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.Joint, bufJoint));
      }

#if false
      // ブレンドシェイプアニメーション出力
      var bufBlendShape = this.ExportBlendShapeAnimation();
      if (bufBlendShape != null)
      {
        //dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.BlendShape, bufBlendShape));
      }
#endif

      // UVアニメーション出力
      var bufUv = this.ExportUvAnimation();
      if (bufUv != null)
      {
        dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.Uv, bufUv));
      }

      // テクスチャパターンアニメーション出力
      var bufTexturePattern = this.ExportTexturePatternAnimation();
      if (bufTexturePattern != null)
      {
        dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.TexturePattern, bufTexturePattern));
      }

      // シェーダーアニメーション出力
      var bufShader = this.ExportShaderAnimation();
      if (bufShader != null)
      {
        dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.Shader, bufShader));
      }

      // ビジビリティアニメーション出力
      var bufVisible = this.ExportVisibilityAnimation();
      if (bufVisible != null)
      {
        dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.Visibility, bufVisible));
      }

      // キーアニメーション出力
      var bufKey = this.ExportKeyAnimation();
      if (bufKey != null)
      {
        dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.Key, bufKey));
      }

      // 環境アニメーション出力：ライト
      var bufEnvLight = this.ExportEnvAnimationLight();
      if (bufEnvLight != null)
      {
        dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.EnvLight, bufEnvLight));
      }

      // 環境アニメーション出力：カメラ
      var bufEnvCamera = this.ExportEnvAnimationCamera();
      if (bufEnvCamera != null)
      {
        dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.EnvCamera, bufEnvCamera));
      }

      //kawa20150514
      // 環境アニメーション出力：フォグ
      var bufEnvFog = this.ExportEnvAnimationFog();
      if (bufEnvFog != null)
      {
          dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.EnvFog, bufEnvFog));
      }

      // 環境アニメーション出力：グレア(ブルーム)
      var bufEnvGlare = this.ExportEnvAnimationGlare();
      if (bufEnvGlare != null)
      {
          dataList.Add(new GfmotBinaryHeader(GfmotBinaryHeader.AnimationType.EnvGlare, bufEnvGlare));
      }

      // データ数
      this.bw.Write(dataList.Count);

      var offset = (int)this.bw.BaseStream.Position;
      offset += (4 * 3) * dataList.Count;

      // データヘッダー出力
      foreach (var data in dataList)
      {
        // タイプ
        var type = (int)data.type;
        this.bw.Write(type);

        // サイズ
        this.bw.Write(data.buffer.Length);

        // オフセット
        this.bw.Write(offset);
        offset += data.buffer.Length;
      }

      // データ出力
      foreach (var data in dataList)
      {
        this.bw.Write(data.buffer);
      }

      return true;
    }

    // ヘッダ出力
    public bool ExportHeader()
    {
      if (this.gfMotion == null)
      {
        return false;
      }

      // バージョン
      this.bw.Write(FormatVersionMajor);
      this.bw.Write(FormatVersionMinor);

      return true;
    }

    // アニメーショントップ出力
    private byte[] ExportAnimationTop()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        // フレーム数
        msr.Write(this.gfMotion.FrameSize);

        // ループするか
        var loopFlag = (short)(this.gfMotion.IsLoop ? 1 : 0);
        msr.Write(loopFlag);

        // データ的にループしているか
        var dataLoopFlag = (short)(this.IsSkeltalAnimeLoop() ? 1 : 0);
        msr.Write(dataLoopFlag);

        // AABB
        var aabb = Common.GetFloatArray(this.gfMotion.ModelAABB);
        foreach (var f in aabb)
        {
          msr.Write(f);
        }

        // ブレンドリスト用

        // ハッシュ値
        var hash = Common.CreateHash(this.fileName);
        msr.Write(hash);

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    // スケルタルアニメのデータがループしているか？
    bool IsSkeltalAnimeLoop()
    {
      // スケルタルアニメが無ければループフラグをそのまま使う
      var skeltalAnim = this.gfMotion.SkeltalAnimation;
      if (skeltalAnim == null)
      {
        return this.gfMotion.IsLoop;
      }

      if (Common.IsNullOrEmpty(skeltalAnim.Node))
      {
        return this.gfMotion.IsLoop;
      }

      foreach (var node in skeltalAnim.Node)
      {
        // Originは移動値が入るので無視する
        if (node.Name == "Origin")
        {
          // 高さだけはループしているか見る必要がある
          if (!Gfmot.Common_Keys.IsDataLoop(node.TranslateY))
          {
            return false;
          }

          continue;
        }

        // SRTをそれぞれチェック
        var srtkeys = new[]
        {
          node.ScaleX,
          node.ScaleY,
          node.ScaleZ,

          node.RotateX,
          node.RotateY,
          node.RotateZ,

          node.TranslateX,
          node.TranslateY,
          node.TranslateZ,
        };

        foreach (var keys in srtkeys)
        {
          if (!Gfmot.Common_Keys.IsDataLoop(keys))
          {
            // 一つでもループしていなければそこで打ち切れる
            return false;
          }
        }
      }

      // 全データループしている
      return true;
    }

    private static float[] GetQuatLn(Quaternion quat)
    {
      var theta = Math.Acos(quat.w);
      var sinTheta = Math.Sin(theta);
      var s = (float)(theta / sinTheta);
      if (float.IsNaN(s))
      {
        s = 0.0f;
      }

      var result = new float[3];
      result[0] = quat.x * s;
      result[1] = quat.y * s;
      result[2] = quat.z * s;

      return result;
    }

    private static void GetRotate(float[] rotate, float[] rotateOld)
    {
      var rotMatrix = Matrix.GetRotationZYX(rotate[2], rotate[1], rotate[0]);
      var quat = new Quaternion();
      quat.FromMatrix(rotMatrix);

      var quatLn0 = GetQuatLn(quat);
      rotate[0] = quatLn0[0];
      rotate[1] = quatLn0[1];
      rotate[2] = quatLn0[2];

      if (rotateOld == null)
      {
        return;
      }

      quat.x = -quat.x;
      quat.y = -quat.y;
      quat.z = -quat.z;
      quat.w = -quat.w;
      var quatLn1 = GetQuatLn(quat);

      var subX0 = Math.Abs(rotateOld[0] - quatLn0[0]);
      var subY0 = Math.Abs(rotateOld[1] - quatLn0[1]);
      var subZ0 = Math.Abs(rotateOld[2] - quatLn0[2]);
      var sub0 = subX0 + subY0 + subZ0;

      var subX1 = Math.Abs(rotateOld[0] - quatLn1[0]);
      var subY1 = Math.Abs(rotateOld[1] - quatLn1[1]);
      var subZ1 = Math.Abs(rotateOld[2] - quatLn1[2]);
      var sub1 = subX1 + subY1 + subZ1;

      if (sub1 < sub0)
      {
        rotate[0] = quatLn1[0];
        rotate[1] = quatLn1[1];
        rotate[2] = quatLn1[2];
      }
    }


    // ジョイントアニメーション出力
    private byte[] ExportJointAnimation()
    {
      var eulerJointList = new List<string>(); // デバッグ、ログ用

      var skeltalAnimation = this.gfMotion.SkeltalAnimation;
      if (skeltalAnimation == null)
      {
        return null;
      }
      if (Common.IsNullOrEmpty(skeltalAnimation.Node))
      {
        return null;
      }

      var compRengeList = new float[]
      {
        this.setting.GetScaleRange(),
        this.setting.GetScaleRange(),
        this.setting.GetScaleRange(),

        this.setting.GetRotationRange(),
        this.setting.GetRotationRange(),
        this.setting.GetRotationRange(),

        this.setting.GetTranslateRange(),
        this.setting.GetTranslateRange(),
        this.setting.GetTranslateRange(),
      };

      if (gfMotion.Skelton == null)
      {
        return null;
      }

      // モデル側のジョイント辞書作成
      if (this.gfModel != null)
      {
        var modelJointDict = new Dictionary<string, Gfmot.Skelton_Node>();

        foreach (var joint in this.gfModel.Skelton.Node)
        {
          var name = joint.Name;

          if (modelJointDict.ContainsKey(name))
          {
            throw new Exception("ジョイント名が重複しています [ " + name + " ]");
          }

          modelJointDict.Add(name, joint);
        }

        // モデル側ジョイントを見てジョイントアニメーションを整理
        var tmpJointAnimationList = new List<Gfmot.JointAnimation_Node>(skeltalAnimation.Node);
        foreach (var joint in this.gfMotion.Skelton.Node)
        {
          if (!modelJointDict.ContainsKey(joint.Name))
          {
            continue;
          }

          var modelJoint = modelJointDict[joint.Name];
          var modelScale = Common.GetFloatArray(modelJoint.LocalScale);
          var modelRotate = Common.GetFloatArray(modelJoint.LocalRotation);
          var modelTranslate = Common.GetFloatArray(modelJoint.LocalTransform);

          var motionScale = Common.GetFloatArray(joint.LocalScale);
          var motionRotate = Common.GetFloatArray(joint.LocalRotation);
          var motionTranslate = Common.GetFloatArray(joint.LocalTransform);

          var newNode = tmpJointAnimationList.Find((node) => node.Name == joint.Name);
          if (newNode == null)
          {
            newNode = new Gfmot.JointAnimation_Node();
            newNode.Name = joint.Name;

            tmpJointAnimationList.Add(newNode);
          }

          // scale
          newNode.ScaleX = GetNewKey(modelScale[0], motionScale[0], newNode.ScaleX);
          newNode.ScaleY = GetNewKey(modelScale[1], motionScale[1], newNode.ScaleY);
          newNode.ScaleZ = GetNewKey(modelScale[2], motionScale[2], newNode.ScaleZ);

          // rotate
          newNode.RotateX = GetNewKey(modelRotate[0], motionRotate[0], newNode.RotateX);
          newNode.RotateY = GetNewKey(modelRotate[1], motionRotate[1], newNode.RotateY);
          newNode.RotateZ = GetNewKey(modelRotate[2], motionRotate[2], newNode.RotateZ);

          // translate
          newNode.TranslateX = GetNewKey(modelTranslate[0], motionTranslate[0], newNode.TranslateX);
          newNode.TranslateY = GetNewKey(modelTranslate[1], motionTranslate[1], newNode.TranslateY);
          newNode.TranslateZ = GetNewKey(modelTranslate[2], motionTranslate[2], newNode.TranslateZ);
        }

        // 要らないキーを間引く
        var newJointAnimationList = new List<Gfmot.JointAnimation_Node>();
        foreach (var node in tmpJointAnimationList)
        {
          if (!IsAllNullAnimation(node))
          {
            newJointAnimationList.Add(node);
          }
        }

        skeltalAnimation.Node = newJointAnimationList.ToArray();
      }


      var jointDict = new Dictionary<string, Gfmot.Skelton_Node>();

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
          parent.ChildList = new List<Gfmot.Skelton_Node>();
        }
        parent.ChildList.Add(joint);
      }

      // 最大の階層深度を求める
      var maxDepth = -1;
      foreach (var joint in gfMotion.Skelton.Node)
      {
        if (maxDepth < joint.Depth)
        {
          maxDepth = joint.Depth;
        }
      }

      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var nodeList = skeltalAnimation.Node;
        if (nodeList == null)
        {
          return null;
        }

        var rotateDict = new Dictionary<string, float[]>();
        foreach (var skelton in this.gfMotion.Skelton.Node)
        {
          var rotateText = skelton.LocalRotation.Split();
          var rotate = new float[]
          {
            float.Parse(rotateText[0]),
            float.Parse(rotateText[1]),
            float.Parse(rotateText[2]),
          };

          if (rotateDict.ContainsKey(skelton.Name))
          {
            throw new Exception("ジョイント名が重複しています [ " + skelton.Name + " ]");
          }

          rotateDict.Add(skelton.Name, rotate);
        }

        // ポケモンなら歩きモーション用の特殊対処。。
        if (this.isPokeMode)
        {
          var originName = "Origin";
          var _walkName = "_";
          foreach (var node in nodeList)
          {
            if (node.Name.Contains(originName))
            {
              node.Name = _walkName;
              break;
            }
          }
          if (jointDict.ContainsKey(originName))
          {
            var origin = jointDict[originName];
            jointDict.Remove(originName);

            origin.Name = originName;

            if (jointDict.ContainsKey(_walkName))
            {
              throw new Exception("ジョイント名が重複しています [ " + _walkName + " ]");
            }

            jointDict.Add(_walkName, origin);
          }
          if (rotateDict.ContainsKey(originName))
          {
            var origin = rotateDict[originName];
            rotateDict.Remove(originName);

            if (rotateDict.ContainsKey(_walkName))
            {
              throw new Exception("ジョイント名が重複しています [ " + _walkName + " ]");
            }

            rotateDict.Add(_walkName, origin);
          }
        }

        // Endジョイントを間引く
        nodeList = nodeList.Where((node) =>
        {
          if (!node.Name.StartsWith("End"))
          {
            return true;
          }

          var joint = jointDict[node.Name];
          return joint.ChildDepth != 0;
        }).ToArray();

        // ジョイント数
        int jointCount = nodeList.Length;
        msr.Write(jointCount);

        var nodeNameList = new List<char[]>();
        var nameSize = 0;

        foreach (var animNode in nodeList)
        {
          // ノード名
          var nodeName = Common.GetNameBuffer(animNode.Name, animNode.Name.Length);
          nodeNameList.Add(nodeName);

          nameSize += nodeName.Length + 1;
        }

        // 名前サイズ
        msr.Write(Common.GetAlign4ByteSize(nameSize));

        // 名前一覧
        foreach (var name in nodeNameList)
        {
          msr.Write((byte)name.Length);
          msr.Write(name);
        }

        // 4バイトアラインメントにそろえる
        Common.Align4Byte(msr);

#if false
        var sw = new StreamWriter(@"C:\home\xy_pokemon\bw\pm0495_00_tsutarja\exports\pm0495_00_ba20_buturi01.txt");
        sw.BaseStream.SetLength(0);
#endif

        foreach (var animNode in nodeList)
        {
          var animKeyList = new AnimKeyList[]
          {
            AnimKeyList.CreateFromGfmot(animNode.ScaleX),
            AnimKeyList.CreateFromGfmot(animNode.ScaleY),
            AnimKeyList.CreateFromGfmot(animNode.ScaleZ),

            AnimKeyList.CreateFromGfmot(animNode.RotateX),
            AnimKeyList.CreateFromGfmot(animNode.RotateY),
            AnimKeyList.CreateFromGfmot(animNode.RotateZ),

            AnimKeyList.CreateFromGfmot(animNode.TranslateX),
            AnimKeyList.CreateFromGfmot(animNode.TranslateY),
            AnimKeyList.CreateFromGfmot(animNode.TranslateZ),
          };
          var rotWSignList = new List<byte>();

          // どれだけ子供がいるかによって精度を調整する
          // 子供が多いと影響が大きいので精度を高くしている
          var joint = jointDict[animNode.Name];
          var perDepth = joint.ChildDepth / (float)maxDepth;
          var levelOffset = 1.0f - perDepth;

          // 圧縮レベルに合わせて際圧縮する
          if (this.setting.Level != Mcnf.CompSetting.Levels.NO_ERROR)
          {
            for (var i = 0; i < animKeyList.Length; ++i)
            {
              if (animKeyList[i] == null)
              {
                continue;
              }

              // エルミート以外の再圧縮には対応しない
              var isHermiteU16 = (animKeyList[i].AnimType == AnimTypes.HermiteU16);
              var isHermiteF32 = (animKeyList[i].AnimType == AnimTypes.HermiteF32);
              if (!(isHermiteU16 || isHermiteF32))
              {
                continue;
              }

              // 再圧縮
              animKeyList[i] = KeyCompress.ReComp(animKeyList[i], compRengeList[i] * levelOffset);
            }
          }

          // 回転をクォータニオンに変換して圧縮
          var rotateBasepose = rotateDict[animNode.Name];
          var useQuat = KeyCompress.ReCompRotateQuat(
            this.gfMotion.FrameSize,
            ref animKeyList[(int)SRTIndex.RotateX],
            ref animKeyList[(int)SRTIndex.RotateY],
            ref animKeyList[(int)SRTIndex.RotateZ],
            rotateBasepose,
            compRengeList[(int)SRTIndex.RotateX] * levelOffset,
            animNode.Name);

          // フラグを出力
          uint flags = 0;

          // オイラー角を使うならフラグを立てる
          if (!useQuat)
          {
            for (var rotIndex = 0; rotIndex < 3; ++rotIndex)
            {
              var animKey = animKeyList[rotIndex + (int)SRTIndex.RotateX];
              if (animKey.AnimType == AnimTypes.Basepose)
              {
                animKey.keys = new AnimKey[] 
                {
                  new AnimKey(0, rotateBasepose[rotIndex], 0),
                };
              }
            }
            flags = 1U << 31;

            eulerJointList.Add(animNode.Name);
          }

          for (var i = 0; i < animKeyList.Length; ++i)
          {
            if (animKeyList[i] == null)
            {
              continue;
            }

            uint animType = (uint)animKeyList[i].AnimType;
            {
              flags |= animType << (i * 3); // 各３bitずつ
            }
          }
          msr.Write(flags);

          // アニメーションを出力
          using (var keyDataMs = new MemoryStream())
          using (var keyDataBw = new BinaryWriter(keyDataMs))
          {
            for (var i = 0; i < animKeyList.Length; ++i)
            {
              if (animKeyList[i] == null)
              {
                continue;
              }

              animKeyList[i].Save(keyDataBw, this.gfMotion.FrameSize);
            }

            var keyData = keyDataMs.ToArray();
            msr.Write(keyData.Length);
            msr.Write(keyData);
          }

#if false
          for (var i = 0; i < animKeyList.Length; ++i)
          {
            var animKey = animKeyList[i];
            var animType = animKey.AnimType;
            sw.WriteLine("name : " + animNode.Name + "_" + ((SRTIndex)i).ToString());
            sw.WriteLine("kind : " + animType.ToString());
            if ((animType == AnimTypes.HermiteU16) || (animType == AnimTypes.HermiteF32))
            {
              sw.WriteLine("  keyCount : " + animKey.keys.Length);
              foreach (var key in animKey.keys)
              {
                sw.WriteLine("  {0} : {1}", key.frame, key.value);
              }
            }
            sw.WriteLine();
          }
#endif

          // デバッグ出力
          // DebugFCurveOutput(animNode.Name, this.gfMotion.FrameSize, fcurveList);
        }

#if false
        sw.Close();
#endif

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

#if false
        if (eulerJointList.Count > 0)
        {
          File.WriteAllLines(this.fileName + "_conv_log", eulerJointList);
        }
#endif

        return ms.ToArray();
      }
    }

    private static Gfmot.Common_Keys GetNewKey(float modelValue, float motionValue, Gfmot.Common_Keys keys)
    {
      if (Common.IsFloatEqual(modelValue, motionValue))
      {
        if (keys != null)
        {
          if (keys.OneFrameKey != null)
          {
            if (Common.IsFloatEqual(modelValue, keys.OneFrameKey.Value))
            {
              keys = null;
            }
          }
        }
      }
      else
      {
        if (keys == null)
        {
          keys = new Gfmot.Common_Keys();
          keys.OneFrameKey = new Gfmot.Common_OneFrameKey();
          keys.OneFrameKey.Value = motionValue;
        }
      }

      return keys;
    }

    private static bool IsAllNullAnimation(Gfmot.JointAnimation_Node node)
    {
      if (node.ScaleX != null)
      {
        return false;
      }
      if (node.ScaleY != null)
      {
        return false;
      }
      if (node.ScaleZ != null)
      {
        return false;
      }

      if (node.RotateX != null)
      {
        return false;
      }
      if (node.RotateY != null)
      {
        return false;
      }
      if (node.RotateZ != null)
      {
        return false;
      }

      if (node.TranslateX != null)
      {
        return false;
      }
      if (node.TranslateY != null)
      {
        return false;
      }
      if (node.TranslateZ != null)
      {
        return false;
      }

      return true;
    }

    private static void DebugFCurveOutput(string name, int frameSize, FunctionCurve[] fcurveList)
    {
      var width = 2048;
      var height = 1024;

      for (var srtIndex = (int)SRTIndex.RotateX; srtIndex <= (int)SRTIndex.RotateZ; ++srtIndex)
      {
        if (!fcurveList[srtIndex].HasAnimation())
        {
          continue;
        }

        var outputFileName = name + "_" + srtIndex.ToString() + ".png";
        outputFileName = outputFileName.Replace(':', '_');

        using (var bitmap = new System.Drawing.Bitmap(width, height))
        using (var graphics = System.Drawing.Graphics.FromImage(bitmap))
        {
          // クリア
          graphics.Clear(System.Drawing.Color.White);

          var pathList = new List<System.Drawing.Point>();
          if (true)
          {
            var minValue = -Math.PI - 0.01;
            var maxValue = Math.PI;
            for (var i = 0; i < frameSize; ++i)
            {
              var value = fcurveList[srtIndex].GetFrameValue(i);
              if (value < minValue) minValue = value;
              if (value > maxValue) maxValue = value;
            }
            for (var i = 0; i < frameSize * 4; ++i)
            {
              var value = fcurveList[srtIndex].GetFrameValue(i / 4.0f);

              var xPoint = (width / frameSize) * (i / 4.0f);
              var yPoint = height - ((value - minValue) / (maxValue - minValue)) * height;
              pathList.Add(new System.Drawing.Point((int)xPoint, (int)yPoint));
            }

            graphics.DrawLines(System.Drawing.Pens.Black, pathList.ToArray());

            {
              var yPoint = height - ((Math.PI - minValue) / (maxValue - minValue)) * height;
              pathList.Clear();
              pathList.Add(new System.Drawing.Point(0, (int)yPoint));
              pathList.Add(new System.Drawing.Point(width, (int)yPoint));
              graphics.DrawLines(System.Drawing.Pens.Blue, pathList.ToArray());
            }
            {
              var yPoint = height - ((0.0f - minValue) / (maxValue - minValue)) * height;
              pathList.Clear();
              pathList.Add(new System.Drawing.Point(0, (int)yPoint));
              pathList.Add(new System.Drawing.Point(width, (int)yPoint));
              graphics.DrawLines(System.Drawing.Pens.Blue, pathList.ToArray());
            }
            {
              var yPoint = height - ((-Math.PI - minValue) / (maxValue - minValue)) * height;
              pathList.Clear();
              pathList.Add(new System.Drawing.Point(0, (int)yPoint));
              pathList.Add(new System.Drawing.Point(width, (int)yPoint));
              graphics.DrawLines(System.Drawing.Pens.Blue, pathList.ToArray());
            }
          }

          // 保存
          bitmap.Save(outputFileName, System.Drawing.Imaging.ImageFormat.Png);
        }
      }
    }

    // ブレンドシェイプアニメーション出力
    private byte[] ExportBlendShapeAnimation()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var blendShapeAnimation = this.gfMotion.BlendShapeAnimation;
        if (blendShapeAnimation == null)
        {
          return null;
        }

        var targetMeshList = blendShapeAnimation.TargetMesh;
        if (targetMeshList == null)
        {
          return null;
        }

        // フレーム数
        msr.Write(this.gfMotion.FrameSize + 1);

        // ループするか
        int loopFlag = this.gfMotion.IsLoop ? 1 : 0;
        msr.Write(loopFlag);

        // ターゲットメッシュ数
        int targetMeshCount = targetMeshList.Length;
        msr.Write(targetMeshCount);

        foreach (var targetMesh in targetMeshList)
        {
          // ターゲットメッシュ名
          var targetMeshName = Common.GetNameBuffer(targetMesh.Name, 64);
          msr.Write(targetMeshName);

          // ハッシュ（ダミー）
          msr.Write(0);

          foreach (var frame in targetMesh.Frame)
          {
            foreach (var blendMesh in frame.BlendMesh)
            {
              // ブレンドメッシュ名
              var blendMeshName = Common.GetNameBuffer(blendMesh.Name, 64);
              msr.Write(blendMeshName);
            }
            foreach (var blendMesh in frame.BlendMesh)
            {
              // ハッシュ（ダミー）
              msr.Write(0);
            }
            foreach (var blendMesh in frame.BlendMesh)
            {
              // ウェイト
              msr.Write(blendMesh.Weight);
            }
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    // UVアニメーション出力
    private byte[] ExportUvAnimation()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var uvAnimation = this.gfMotion.UvAnimation;
        if (uvAnimation == null)
        {
          return null;
        }

        var materialList = uvAnimation.Material;
        if (materialList == null)
        {
          return null;
        }

        var newMaterialList = new List<Gfmot.UvAnimation_Material>();
        foreach (var material in materialList)
        {
          if (material.TextureSlot != null)
          {
            newMaterialList.Add(material);
          }
        }
        materialList = newMaterialList.ToArray();
        if (materialList.Length == 0)
        {
          return null;
        }

        // マテリアル数
        int materialCount = materialList.Length;
        msr.Write(materialCount);

        var materialNameList = new List<char[]>();
        var nameSize = 0;
        foreach (var material in materialList)
        {
          // マテリアル名
          var materialName = Common.GetNameBuffer(material.Name, material.Name.Length);
          materialNameList.Add(materialName);

          nameSize += materialName.Length + 1;
        }

        msr.Write(Common.GetAlign4ByteSize(nameSize));

        foreach (var material in materialList)
        {
          // テクスチャスロット数
          msr.Write(material.TextureSlot.Length);
        }

        foreach (var name in materialNameList)
        {
          msr.Write((byte)name.Length);
          msr.Write(name);
        }

        // 4バイトアラインメントにそろえる
        Common.Align4Byte(msr);

        foreach (var material in materialList)
        {
          foreach (var textureSlot in material.TextureSlot)
          {
            // スロット番号
            msr.Write(textureSlot.No);

            var animKeyList = new AnimKeyList[]
            {
              AnimKeyList.CreateFromGfmot(textureSlot.ScaleU),
              AnimKeyList.CreateFromGfmot(textureSlot.ScaleV),

              AnimKeyList.CreateFromGfmot(textureSlot.Rotate),

              AnimKeyList.CreateFromGfmot(textureSlot.TranslateU),
              AnimKeyList.CreateFromGfmot(textureSlot.TranslateV),
            };

            // フラグを出力
            uint flags = 0;
            for (var i = 0; i < animKeyList.Length; ++i)
            {
              uint animType = (uint)animKeyList[i].AnimType;
              {
                flags |= animType << (i * 3); // 各３bitずつ
              }
            }
            msr.Write(flags);

            // アニメーションを出力
            using (var keyDataMs = new MemoryStream())
            using (var keyDataBw = new BinaryWriter(keyDataMs))
            {
              for (var i = 0; i < animKeyList.Length; ++i)
              {
                animKeyList[i].Save(keyDataBw, this.gfMotion.FrameSize);
              }

              var keyData = keyDataMs.ToArray();
              msr.Write(keyData.Length);
              msr.Write(keyData);
            }
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    // テクスチャパターンアニメーション出力
    private byte[] ExportTexturePatternAnimation()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var texturePatternAnimation = this.gfMotion.TexturePatternAnimation;
        if (texturePatternAnimation == null)
        {
          return null;
        }

        var materialList = texturePatternAnimation.Material;
        if (materialList == null)
        {
          return null;
        }

        // マテリアル数
        int materialCount = materialList.Length;
        msr.Write(materialCount);

        var materialNameList = new List<char[]>();
        var nameSize = 0;
        foreach (var material in materialList)
        {
          // マテリアル名
          var materialName = Common.GetNameBuffer(material.Name, material.Name.Length);
          materialNameList.Add(materialName);

          nameSize += materialName.Length + 1;
        }

        msr.Write(Common.GetAlign4ByteSize(nameSize));

        // テクスチャ辞書作成
        var texIdList = new List<string>();
        foreach (var material in materialList)
        {
          foreach (var textureSlot in material.TextureSlot)
          {
            foreach (var attribute in textureSlot.Attribute)
            {
              // まだ追加されていないなら追加
              if (!texIdList.Contains(attribute.TextureName))
              {
                texIdList.Add(attribute.TextureName);
              }
            }
          }
        }
        texIdList.Sort();

        var texIdDict = new Dictionary<string, int>();
        for (var texId = 0; texId < texIdList.Count; ++texId)
        {
          texIdDict.Add(texIdList[texId], texId);
        }

        // テクスチャ辞書出力
        msr.Write(texIdDict.Count);

        var texIdDictSize = 0;
        var texIdDictList = new List<char[]>();
        foreach (var texName in texIdDict.Keys)
        {
          var name = Common.GetNameBuffer(texName, texName.Length);
          texIdDictList.Add(name);

          texIdDictSize += name.Length + 1;
        }
        msr.Write(Common.GetAlign4ByteSize(texIdDictSize));

        foreach (var material in materialList)
        {
          // テクスチャスロット数
          msr.Write(material.TextureSlot.Length);
        }

        foreach (var name in materialNameList)
        {
          msr.Write((byte)name.Length);
          msr.Write(name);
        }
        // 4バイトアラインメントにそろえる
        Common.Align4Byte(msr);
        foreach (var name in texIdDictList)
        {
          msr.Write((byte)name.Length);
          msr.Write(name);
        }
        // 4バイトアラインメントにそろえる
        Common.Align4Byte(msr);

        foreach (var material in materialList)
        {
          foreach (var textureSlot in material.TextureSlot)
          {
            // テクスチャスロット番号
            msr.Write(textureSlot.No);

            // キー数
            msr.Write(textureSlot.Attribute.Length);

            foreach (var attribute in textureSlot.Attribute)
            {
              var frame = attribute.Frame;
              var texId = texIdDict[attribute.TextureName];

              msr.Write((ushort)frame);
              msr.Write((ushort)texId);
            }
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    // シェーダーアニメーション出力
    private byte[] ExportShaderAnimation()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var shaderAnimation = this.gfMotion.ShaderAnimation;
        if (shaderAnimation == null)
        {
          return null;
        }

        var materialList = shaderAnimation.Material;
        if (materialList == null)
        {
          return null;
        }

        // マテリアル数
        int materialCount = materialList.Length;
        if (materialCount == 0)
        {
          return null;
        }
        msr.Write(materialCount);

        var materialNameList = new List<char[]>();
        var nameSize = 0;
        foreach (var material in materialList)
        {
          // マテリアル名
          var materialName = Common.GetNameBuffer(material.Name, material.Name.Length);
          materialNameList.Add(materialName);

          nameSize += materialName.Length + 1;
        }

        msr.Write(Common.GetAlign4ByteSize(nameSize));

        foreach (var material in materialList)
        {
          // コンスタントカラー数
          msr.Write(material.ConstantColor.Length);
        }

        foreach (var name in materialNameList)
        {
          msr.Write((byte)name.Length);
          msr.Write(name);
        }
        // 4バイトアラインメントにそろえる
        Common.Align4Byte(msr);

        foreach (var material in materialList)
        {
          foreach (var constantColor in material.ConstantColor)
          {
            // コンスタントカラー番号
            msr.Write(constantColor.No);

            var animKeyList = new AnimKeyList[]
            {
              AnimKeyList.CreateFromGfmot(constantColor.ColorR),
              AnimKeyList.CreateFromGfmot(constantColor.ColorG),
              AnimKeyList.CreateFromGfmot(constantColor.ColorB),
              AnimKeyList.CreateFromGfmot(constantColor.ColorA),
            };

            // フラグを出力
            uint flags = 0;
            for (var i = 0; i < animKeyList.Length; ++i)
            {
              uint animType = (uint)animKeyList[i].AnimType;
              {
                flags |= animType << (i * 3); // 各３bitずつ
              }
            }
            msr.Write(flags);

            // アニメーションを出力
            using (var keyDataMs = new MemoryStream())
            using (var keyDataBw = new BinaryWriter(keyDataMs))
            {
              for (var i = 0; i < animKeyList.Length; ++i)
              {
                animKeyList[i].Save(keyDataBw, this.gfMotion.FrameSize);
              }

              var keyData = keyDataMs.ToArray();
              msr.Write(keyData.Length);
              msr.Write(keyData);
            }
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    // ビジビリティアニメーション出力
    private byte[] ExportVisibilityAnimation()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var visibilityAnimation = this.gfMotion.VisibilityAnimation;
        if (visibilityAnimation == null)
        {
          return null;
        }

        var meshList = visibilityAnimation.Mesh;
        if (meshList == null)
        {
          return null;
        }

        // メッシュ数
        int meshCount = meshList.Length;
        msr.Write(meshCount);

        var nodeNameList = new List<char[]>();
        var nameSize = 0;
        foreach (var animMesh in meshList)
        {
          // ノード名
          var nodeName = Common.GetNameBuffer(animMesh.Name, animMesh.Name.Length);
          nodeNameList.Add(nodeName);

          nameSize += nodeName.Length + 1;
        }

        // 名前サイズ
        msr.Write(Common.GetAlign4ByteSize(nameSize));

        // 名前一覧
        foreach (var name in nodeNameList)
        {
          msr.Write((byte)name.Length);
          msr.Write(name);
        }
        // 4バイトアラインメントにそろえる
        Common.Align4Byte(msr);

        foreach (var mesh in meshList)
        {
          var boolKeyList = new List<bool>();

          var keyIndex = 0;
          var key = mesh.BoolKey[keyIndex];
          var nextFrame = int.MaxValue;
          if (mesh.BoolKey.Length > keyIndex + 1)
          {
            nextFrame = mesh.BoolKey[keyIndex + 1].Frame;
          }
          for (var frame = 0; frame <= this.gfMotion.FrameSize; ++frame)
          {
            if (nextFrame <= frame)
            {
              ++keyIndex;
              key = mesh.BoolKey[keyIndex];
              nextFrame = int.MaxValue;
              if (mesh.BoolKey.Length > keyIndex + 1)
              {
                nextFrame = mesh.BoolKey[keyIndex + 1].Frame;
              }
            }
            boolKeyList.Add(key.Value);
          }

          var flags = (byte)0U;
          var hasData = false;
          for (var i = 0; i < boolKeyList.Count; ++i)
          {
            hasData = true;
            var isVisible = boolKeyList[i];
            var bit = i % 8;
            if (isVisible)
            {
              flags |= (byte)(1U << bit);
            }

            if (bit == 7)
            {
              msr.Write(flags);
              flags = (byte)0U;
              hasData = false;
            }
          }

          if (hasData)
          {
            msr.Write(flags);
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    // キーアニメーション出力
    private byte[] ExportKeyAnimation()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var attributeKeyAnimation = this.gfMotion.AttributeKeyAnimation;
        if (attributeKeyAnimation == null)
        {
          return null;
        }

        var attributeList = attributeKeyAnimation.Attribute;
        if (attributeList == null)
        {
          return null;
        }

        // アトリビュート数
        int attributeCount = attributeList.Length;
        msr.Write(attributeCount);

        foreach (var attribute in attributeList)
        {
          // アトリビュート名
          var attributeName = Common.GetNameBuffer(attribute.Name, attribute.Name.Length + 1); // null文字
          msr.Write((byte)(Common.GetAlign4ByteSize(attributeName.Length) - 1)); 
          msr.Write(attributeName);

          // 4バイトアラインメントに揃える
          Common.Align4Byte(msr);

          // キーフレーム数
          msr.Write(attribute.AttributeKey.Length);

          foreach (var key in attribute.AttributeKey)
          {
            // フレーム
            msr.Write(key.Frame);

            // 値
            msr.Write(key.Value);
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    // 環境アニメーション出力：ライト
    private byte[] ExportEnvAnimationLight()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var envAnimation = this.gfMotion.EnvAnimation;
        if (envAnimation == null)
        {
          return null;
        }

        var drawEnv = envAnimation.DrawEnv;
        if (drawEnv == null)
        {
          return null;
        }

        // ライトセット数
        var lightSetCount = 0;
        var lightCount = 0;
        if (drawEnv.LightSets != null)
        {
          if (drawEnv.LightSets.LightSet != null)
          {
            lightSetCount = drawEnv.LightSets.LightSet.Length;

            foreach (var lightSet in drawEnv.LightSets.LightSet)
            {
              if (lightSet.Light == null)
              {
                continue;
              }
              lightCount += lightSet.Light.Length;
            }
          }
        }
        if (lightSetCount == 0)
        {
          return null;
        }
        if (lightCount == 0)
        {
          return null;
        }
        msr.Write(lightCount);

        var lightNameList = new List<char[]>();
        var nameSize = 0;
        foreach (var lightSet in drawEnv.LightSets.LightSet)
        {
          if (lightSet.Light == null)
          {
            continue;
          }

          foreach (var light in lightSet.Light)
          {
            if (lightSet.Light == null)
            {
              continue;
            }

            // ライト名
            var lightName = Common.GetNameBuffer(light.Name, light.Name.Length);
            lightNameList.Add(lightName);

            nameSize += lightName.Length + 1;
          }
        }
        msr.Write(Common.GetAlign4ByteSize(nameSize));

        foreach (var name in lightNameList)
        {
          msr.Write((byte)name.Length);
          msr.Write(name);
        }
        // 4バイトアラインメントにそろえる
        Common.Align4Byte(msr);

        foreach (var lightSet in drawEnv.LightSets.LightSet)
        {
          if (lightSet.Light == null)
          {
            continue;
          }

          foreach (var light in lightSet.Light)
          {
            // ライトセット番号
            msr.Write((int)lightSet.No);

            // ライトタイプ
            var lightType = (int)light.Type;
            msr.Write(lightType);

            var animKeyList = new AnimKeyList[]
            {
              AnimKeyList.CreateFromGfmot(light.PositionX),
              AnimKeyList.CreateFromGfmot(light.PositionY),
              AnimKeyList.CreateFromGfmot(light.PositionZ),

              AnimKeyList.CreateFromGfmot(light.DirectionX),
              AnimKeyList.CreateFromGfmot(light.DirectionY),
              AnimKeyList.CreateFromGfmot(light.DirectionZ),

              AnimKeyList.CreateFromGfmot(light.ColorR),
              AnimKeyList.CreateFromGfmot(light.ColorG),
              AnimKeyList.CreateFromGfmot(light.ColorB),

              AnimKeyList.CreateFromGfmot(light.ConeAngle),
              AnimKeyList.CreateFromGfmot(light.Intensity),
            };

            // フラグを出力
            UInt64 flags = 0;
            for (var i = 0; i < animKeyList.Length; ++i)
            {
              uint animType = (uint)animKeyList[i].AnimType;
              {
                flags |= animType << (i * 3); // 各３bitずつ
              }
            }
            msr.Write(flags);

            // アニメーションを出力
            using (var keyDataMs = new MemoryStream())
            using (var keyDataBw = new BinaryWriter(keyDataMs))
            {
              for (var i = 0; i < animKeyList.Length; ++i)
              {
                animKeyList[i].Save(keyDataBw, this.gfMotion.FrameSize);
              }

              var keyData = keyDataMs.ToArray();
              msr.Write(keyData.Length);
              msr.Write(0xFFFFFFFF); // パディング
              msr.Write(keyData);
            }
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    // 環境アニメーション：カメラ
    private byte[] ExportEnvAnimationCamera()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var envAnimation = this.gfMotion.EnvAnimation;
        if (envAnimation == null)
        {
          return null;
        }

        var drawEnv = envAnimation.DrawEnv;
        if (drawEnv == null)
        {
          return null;
        }

        // カメラ数
        int cameraCount = 0;
        if (drawEnv.Cameras != null)
        {
          if (drawEnv.Cameras.Camera != null)
          {
            cameraCount = drawEnv.Cameras.Camera.Length;
          }
        }
        if (cameraCount == 0)
        {
          return null;
        }
        msr.Write(cameraCount);

        var cameraNameList = new List<char[]>();
        var nameSize = 0;
        foreach (var camera in drawEnv.Cameras.Camera)
        {
          // カメラ名
          var cameraName = Common.GetNameBuffer(camera.Name, camera.Name.Length);
          cameraNameList.Add(cameraName);

          nameSize += cameraName.Length + 1;
        }

        msr.Write(Common.GetAlign4ByteSize(nameSize));

        foreach (var name in cameraNameList)
        {
          msr.Write((byte)name.Length);
          msr.Write(name);
        }
        // 4バイトアラインメントにそろえる
        Common.Align4Byte(msr);

        foreach (var camera in drawEnv.Cameras.Camera)
        {
          var animKeyList = new AnimKeyList[]
          {
            AnimKeyList.CreateFromGfmot(camera.Near),
            AnimKeyList.CreateFromGfmot(camera.Far),
            AnimKeyList.CreateFromGfmot(camera.Fovy),
            AnimKeyList.CreateFromGfmot(camera.AspectRatio),
            AnimKeyList.CreateFromGfmot(camera.RotateX),
            AnimKeyList.CreateFromGfmot(camera.RotateY),
            AnimKeyList.CreateFromGfmot(camera.RotateZ),
            AnimKeyList.CreateFromGfmot(camera.TranslateX),
            AnimKeyList.CreateFromGfmot(camera.TranslateY),
            AnimKeyList.CreateFromGfmot(camera.TranslateZ),
          };

          // フラグを出力
          UInt64 flags = 0;
          for (var i = 0; i < animKeyList.Length; ++i)
          {
            uint animType = (uint)animKeyList[i].AnimType;
            {
              flags |= animType << (i * 3); // 各３bitずつ
            }
          }
          msr.Write(flags);

          // アニメーションを出力
          using (var keyDataMs = new MemoryStream())
          using (var keyDataBw = new BinaryWriter(keyDataMs))
          {
            for (var i = 0; i < animKeyList.Length; ++i)
            {
              animKeyList[i].Save(keyDataBw, this.gfMotion.FrameSize);
            }

            var keyData = keyDataMs.ToArray();
            msr.Write(keyData.Length);
            msr.Write(keyData);
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    //kawa20150514
    // 環境アニメーション出力：フォグ
    private byte[] ExportEnvAnimationFog()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var envAnimation = this.gfMotion.EnvAnimation;
        if (envAnimation == null)
        {
          return null;
        }

        var drawEnv = envAnimation.DrawEnv;
        if (drawEnv == null)
        {
          return null;
        }

        // フォグ数
        int fogCount = 0;
        if (drawEnv.EnvParams != null)
        {
          if (drawEnv.EnvParams.Fog != null)
          {
            fogCount = drawEnv.EnvParams.Fog.Length;
          }
        }
        if (fogCount == 0)
        {
          return null;
        }
        msr.Write(fogCount);
        msr.Write(0xFFFFFFFF); // パディング

        foreach (var fog in drawEnv.EnvParams.Fog)
        {
          // フォグ番号
          msr.Write((int)fog.No);
          msr.Write(0xFFFFFFFF); // パディング

          var animKeyList = new AnimKeyList[]
          {
            AnimKeyList.CreateFromGfmot(fog.NearLength),
            AnimKeyList.CreateFromGfmot(fog.FarLength),
            AnimKeyList.CreateFromGfmot(fog.ColorR),
            AnimKeyList.CreateFromGfmot(fog.ColorG),
            AnimKeyList.CreateFromGfmot(fog.ColorB),
            AnimKeyList.CreateFromGfmot(fog.Strength),
          };

          // フラグを出力
          UInt64 flags = 0;
          for (var i = 0; i < animKeyList.Length; ++i)
          {
            uint animType = (uint)animKeyList[i].AnimType;
            {
              flags |= animType << (i * 3); // 各３bitずつ
            }
          }
          msr.Write(flags);

          // アニメーションを出力
          using (var keyDataMs = new MemoryStream())
          using (var keyDataBw = new BinaryWriter(keyDataMs))
          {
            for (var i = 0; i < animKeyList.Length; ++i)
            {
              animKeyList[i].Save(keyDataBw, this.gfMotion.FrameSize);
            }

            var keyData = keyDataMs.ToArray();
            msr.Write(keyData.Length);
            msr.Write(keyData);
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    // 環境アニメーション出力：グレア(ブルーム)
    private byte[] ExportEnvAnimationGlare()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        var envAnimation = this.gfMotion.EnvAnimation;
        if (envAnimation == null)
        {
          return null;
        }

        var drawEnv = envAnimation.DrawEnv;
        if (drawEnv == null)
        {
          return null;
        }

        // グレア(ブルーム)の有無
        int glareCount = 0;
        if (drawEnv.EnvParams != null)
        {
          if (drawEnv.EnvParams.Glare != null)
          {
            glareCount = 1;
          }
        }
        if (glareCount == 0)
        {
          return null;
        }

        var glare = drawEnv.EnvParams.Glare;
        {
          var animKeyList = new AnimKeyList[]
          {
            AnimKeyList.CreateFromGfmot(glare.GlareStrength),
            AnimKeyList.CreateFromGfmot(glare.GlareRange),
            AnimKeyList.CreateFromGfmot(glare.GlareIntensity),
            AnimKeyList.CreateFromGfmot(glare.GlareWeightR),
            AnimKeyList.CreateFromGfmot(glare.GlareWeightG),
            AnimKeyList.CreateFromGfmot(glare.GlareWeightB),
          };

          // フラグを出力
          UInt64 flags = 0;
          for (var i = 0; i < animKeyList.Length; ++i)
          {
            uint animType = (uint)animKeyList[i].AnimType;
            {
              flags |= animType << (i * 3); // 各３bitずつ
            }
          }
          msr.Write(flags);

          // アニメーションを出力
          using (var keyDataMs = new MemoryStream())
          using (var keyDataBw = new BinaryWriter(keyDataMs))
          {
            for (var i = 0; i < animKeyList.Length; ++i)
            {
              animKeyList[i].Save(keyDataBw, this.gfMotion.FrameSize);
            }

            var keyData = keyDataMs.ToArray();
            msr.Write(keyData.Length);
            msr.Write(keyData);
          }
        }

        // 4バイトアライメントに揃える
        Common.Align4Byte(msr);

        return ms.ToArray();
      }
    }

    private FileStream fs = null;
    private BinaryWriter bw = null;
    private Gfmot.GfMotion gfMotion = null;
    private Gfmot.GfModel gfModel = null;
    private string fileName = null;

    private Mcnf.CompSetting setting = new Mcnf.CompSetting();
    private bool isPokeMode = false;
  }

}
