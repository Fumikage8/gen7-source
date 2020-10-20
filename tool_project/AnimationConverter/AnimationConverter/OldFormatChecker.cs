using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace AnimationConverter
{
  class OldFormatChecker
  {
    const float EqualRangeDefault = 0.001f;

    public static void CheckMotTest()
    {
      var currentPath = Directory.GetCurrentDirectory();
      //var cskla = Common.DeserializeXml<Cskla.NintendoWareIntermediateFile>(@"R:\home\niji\git_program\tool_project\AnimationConverter\test_file\new_format\pm0025_00_ba20_buturi01.cskla");

      var jointNames = new string[]
      {
        "Origin",
        "Waist",
        "Hips",
        "LHips",
        "LFoot",
        "LToe",
        "RHips",
        "RFoot",
        "RToe",
        "Tail1",
        "Tail2",
        "Tail3",
        "Tail4",
        "TailA",
        "TailB",
        "TailC",
        "Spine1",
        "Spine2",
        "LArm",
        "LForeArm",
        "LHand",
        "LFeelerA1",
        "LFeelerA2",
        "LFeelerC01",
        "LFeelerC02",
        "LFeelerC03",
        "LFeelerC04",
        "LFeelerC05",
        "LFeelerC06",
        "LFeelerC07",
        "LFeelerC08",
        "LFeelerC09",
        "LFeelerC10",
        "Neck",
        "Head",
        "Nose",
        "RArm",
        "RForeArm",
        "RHand",
        "RFeelerA1",
        "RFeelerA2",
        "RFeelerC01",
        "RFeelerC02",
        "RFeelerC03",
        "RFeelerC04",
        "RFeelerC05",
        "RFeelerC06",
        "RFeelerC07",
        "RFeelerC08",
        "RFeelerC09",
        "RFeelerC10",
      };

      var motFileName = @"C:\Users\takebe_makoto\Desktop\testdata\pm0495_00\anime_ba\dae_temp\LittleEndian_pm0495_00_motion_ba\pm0495_00_ba20_buturi01.mot";
      var textFileName = Path.ChangeExtension(motFileName, "txt");

      var mot = Mot.Mot.LoadMot(motFileName, jointNames);
      mot.DumpText(textFileName);
    }

    public static bool CheckAll(string checkDirectory, string logOutputDirectory)
    {
      // 全ファイル削除
      {
        var gfmots = Directory.GetFiles(checkDirectory, "*.gfmot", SearchOption.AllDirectories);
        foreach (var gfmot in gfmots)
        {
          File.Delete(gfmot);
        }
      }

      // 全ファイルコンバート
      var errorFileList = new List<string>();
      var totalGfmotCount = 0;
      var errorPokeCount = 0;
      var totalPokeCount = 0;
      {
        var dirs = Directory.GetDirectories(checkDirectory, "scenes", SearchOption.AllDirectories);

        var rand = new Random();
        var dirList = new List<string>();
        dirList.AddRange(dirs);

        // ランダムに何匹かテスト
        while (dirList.Count > 20)
        {
          dirList.RemoveAt(rand.Next() % dirList.Count);
        }

#if true
        // よく使うテスト用ポケモン
        dirList.Clear();
        dirList.Add(@"C:\home\xy_pokemon\rg\pm0001_00_fushigidane\scenes");
        dirList.Add(@"C:\home\xy_pokemon\rg\pm0025_00_pikachu\scenes");
        dirList.Add(@"C:\home\xy_pokemon\rg\pm0026_00_raichu\scenes");
        dirList.Add(@"C:\home\xy_pokemon\rg\pm0051_00_dugtrio\scenes");
        dirList.Add(@"C:\home\xy_pokemon\rg\pm0067_00_goriky\scenes");
        dirList.Add(@"C:\home\xy_pokemon\rg\pm0093_00_ghost\scenes");
        dirList.Add(@"C:\home\xy_pokemon\rg\pm0150_00_mewtwo\scenes");
        dirList.Add(@"C:\home\xy_pokemon\rg\pm0151_00_mew\scenes");
#endif

        totalPokeCount = dirList.Count;
        Parallel.ForEach(dirList, (dir) =>
        //foreach (var dir in dirList)
        {
          var mbs = Directory.GetFiles(dir, "*.mb").Where(mb => mb.Contains("_ba") || mb.Contains("_kw")).ToArray();

          foreach (var mb in mbs)
          {
            Console.WriteLine(mb);
            var startInfo = new System.Diagnostics.ProcessStartInfo(@"C:\home\niji\tool\DCCTools\Maya\runtime\2015_64bit\ExportGfMot.bat", mb);
            startInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            using (var process = System.Diagnostics.Process.Start(startInfo))
            {
              process.WaitForExit();
            }
          }

          var exportDir = Path.GetDirectoryName(dir);
          exportDir = Path.Combine(exportDir, "exports");
          if (Directory.Exists(exportDir))
          {
            var gfmotFiles = Directory.GetFiles(exportDir, "*.gfmot");

            var errorCount = 0;
            foreach (var gfmot in gfmotFiles)
            {
              var log = Path.ChangeExtension(gfmot, "txt");
              var cskla = Path.ChangeExtension(gfmot, "cskla");

              if (File.Exists(cskla))
              {
                if (!OldFormatChecker.Check(log, cskla, gfmot, null, null, 0.3f))
                {
                  lock (errorFileList)
                  {
                    ++errorCount;
                    errorFileList.Add(gfmot);
                  }
                }
              }
            }

            lock (errorFileList)
            {
              if (errorCount > 0)
              {
                ++errorPokeCount;
              }
              totalGfmotCount += gfmotFiles.Length;
            }
          }
        } );
      }

      errorFileList.Sort();
      var errorFileListCount = errorFileList.Count;
      errorFileList.Insert(0, string.Format("Gfmot : {0} / {1} : {2}%", errorFileListCount, totalGfmotCount, (int)((float)errorFileListCount / totalGfmotCount * 100)));
      errorFileList.Insert(1, string.Format("Pokemon : {0} / {1} : {2}%", errorPokeCount, totalPokeCount, (int)((float)errorPokeCount / totalPokeCount * 100)));
      File.WriteAllLines(Path.Combine(logOutputDirectory, "cskla_gfmot_check_result.txt"), errorFileList.ToArray());

      return true;
    }

    public static bool Check(string logPath, string csklaPath, string gfmotPath, string motPath, string bgfmotPath, float errorLevel)
    {
      // 中間ファイルを比較する
      var cskla = Common.DeserializeXml<Cskla.NintendoWareIntermediateFile>(csklaPath);
      var gfmot = Common.DeserializeXml<Gfmot.GfMotion>(gfmotPath);

      using (var sw = new StreamWriter(File.Open(logPath, FileMode.OpenOrCreate)))
      {
        sw.BaseStream.SetLength(0);

        CompareCsklaGfmot(sw, cskla.GraphicsContentCtr.Animations[0], gfmot);

        sw.Close();
      }

      var fileInfo = new FileInfo(logPath);
      if (fileInfo.Length == 0)
      {
        File.Delete(logPath);
        return true;
      }

      return false;
    }

    private static bool CompareCsklaGfmot(StreamWriter sw, Cskla.SkeletalAnimationData cskla, Gfmot.GfMotion gfmot)
    {
      // フレーム数
      if (cskla.FrameSize != gfmot.FrameSize)
      {
        return false;
      }

      if (gfmot.SkeltalAnimation == null)
      {
        return true;
      }
      if (gfmot.SkeltalAnimation.Node == null)
      {
        return true;
      }

      // アニメーションノード辞書
      var animNodeDict = new Dictionary<string, Gfmot.JointAnimation_Node>();
      foreach (var node in gfmot.SkeltalAnimation.Node)
      {
        var gfmotBoneName = GetGfmotName(node.Name);
        animNodeDict.Add(gfmotBoneName, node);
      }

      // Csklaボーン辞書
      var csklaBoneDict = new Dictionary<string, Cskla.Bone>();
      foreach (var bone in cskla.Skeleton.Bones)
      {
        csklaBoneDict.Add(bone.Name, bone);
      }

      // Gfmotボーン辞書
      var gfmotBoneDict = new Dictionary<string, Gfmot.Skelton_Node>();
      foreach (var node in gfmot.Skelton.Node)
      {
        var gfmotBoneName = GetGfmotName(node.Name);
        gfmotBoneDict.Add(gfmotBoneName, node);
      }

      // 全アニメーションチェック
      foreach (var csklaData in cskla.MemberAnimationDataSet)
      {
        var csklaBoneName = GetCsklaName(csklaData.Path);

        Gfmot.JointAnimation_Node gfmotData = null;
        if (animNodeDict.TryGetValue(csklaBoneName, out gfmotData))
        {
          var csklaBone = csklaBoneDict[csklaBoneName];
          var gfmotBone = gfmotBoneDict[csklaBoneName];
          CompareAnimation(sw, csklaBoneName, csklaData.TransformAnimation, gfmotData, csklaBone, gfmotBone, cskla.FrameSize);
        }
      }

      return true;
    }

    private static string GetCsklaName(string name)
    {
      var startIndex = name.IndexOf('[') + 2;
      var endIndex = name.IndexOf(']') - 1;

      return name.Substring(startIndex, endIndex - startIndex);
    }

    private static string GetGfmotName(string name)
    {
      return name;
    }

    private static bool EqualRange(float a, float b, float range)
    {
      return Math.Abs(a - b) < range;
    }

    private static readonly string[] CurveNames =
    {
      "Scale",
      "Rotate",
      "Translate",
    };

    private static readonly string[] VectorNames =
    {
      "X",
      "Y",
      "Z"
    };

    private static readonly bool[] RotateModes =
    {
      false,
      true,
      false,
    };

    private static readonly char[] SplitChar = new char[]{ ' ' };

    private static bool CompareAnimation(StreamWriter sw, string desc, Cskla.TransformAnimation csklaAnim, Gfmot.JointAnimation_Node gfmotAnim, Cskla.Bone csklaBone, Gfmot.Skelton_Node gfmotSkelton, int frameCount)
    {
      var csklaScale = csklaAnim.ScaleVector3Animation;
      var csklaRotate = csklaAnim.RotateVector3Animation;
      var csklaTranslate = csklaAnim.TranslateVector3Animation;
      var csklaFloatCurves = new Cskla.SegmentsFloatCurve[3][]
      {
        new Cskla.SegmentsFloatCurve[]
        {
          (csklaScale == null) ? null : csklaScale.XSegmentsFloatCurve,
          (csklaScale == null) ? null : csklaScale.YSegmentsFloatCurve,
          (csklaScale == null) ? null : csklaScale.ZSegmentsFloatCurve,
        },

        new Cskla.SegmentsFloatCurve[]
        {
          (csklaRotate == null) ? null : csklaRotate.XSegmentsFloatCurve,
          (csklaRotate == null) ? null : csklaRotate.YSegmentsFloatCurve,
          (csklaRotate == null) ? null : csklaRotate.ZSegmentsFloatCurve,
        },

        new Cskla.SegmentsFloatCurve[]
        {
          (csklaTranslate == null) ? null : csklaTranslate.XSegmentsFloatCurve,
          (csklaTranslate == null) ? null : csklaTranslate.YSegmentsFloatCurve,
          (csklaTranslate == null) ? null : csklaTranslate.ZSegmentsFloatCurve,
        },
      };

      var gfmotFloatCurves = new Gfmot.Common_Keys[3][]
      {
        new Gfmot.Common_Keys[]
        {
          gfmotAnim.ScaleX,
          gfmotAnim.ScaleY,
          gfmotAnim.ScaleZ,
        },

        new Gfmot.Common_Keys[]
        {
          gfmotAnim.RotateX,
          gfmotAnim.RotateY,
          gfmotAnim.RotateZ,
        },

        new Gfmot.Common_Keys[]
        {
          gfmotAnim.TranslateX,
          gfmotAnim.TranslateY,
          gfmotAnim.TranslateZ,
        },
      };

      var csklaBaseposes = new float[3][]
      {
        new float[3]
        {
          csklaBone.Transform.Scale.X,
          csklaBone.Transform.Scale.Y,
          csklaBone.Transform.Scale.Z,
        },

        new float[3]
        {
          csklaBone.Transform.Rotate.X,
          csklaBone.Transform.Rotate.Y,
          csklaBone.Transform.Rotate.Z,
        },

        new float[3]
        {
          csklaBone.Transform.Translate.X,
          csklaBone.Transform.Translate.Y,
          csklaBone.Transform.Translate.Z,
        },
      };

      var scaleText = gfmotSkelton.LocalScale.Split(SplitChar);
      var rotateText = gfmotSkelton.LocalRotation.Split(SplitChar);
      var translateText = gfmotSkelton.LocalTransform.Split(SplitChar);
      var gfmotBaseposes = new float[3][]
      {
        new float[3]
        {
          float.Parse(scaleText[0]),
          float.Parse(scaleText[1]),
          float.Parse(scaleText[2]),
        },

        new float[3]
        {
          float.Parse(rotateText[0]),
          float.Parse(rotateText[1]),
          float.Parse(rotateText[2]),
        },

        new float[3]
        {
          float.Parse(translateText[0]),
          float.Parse(translateText[1]),
          float.Parse(translateText[2]),
        },
      };

      for (var i = 0; i < csklaFloatCurves.Length; ++i)
      {
        var curves1 = csklaFloatCurves[i];
        var curves2 = gfmotFloatCurves[i];
        var basePoses1 = csklaBaseposes[i];
        var basePoses2 = gfmotBaseposes[i];
        var rotateMode = RotateModes[i];

        CompareCurve2(
          sw, desc + " : " + CurveNames[i],
          curves1, curves2,
          basePoses1, basePoses2,
          frameCount, rotateMode);
        //CompareCurve(sw, desc + " : " + CurveNames[i] + " : ", curve1, curve2, basePose, frameCount, rotateMode);
      }

      return true;
    }

    private static void CompareCurve(StreamWriter sw, string desc, Cskla.SegmentsFloatCurve curve1, Gfmot.Common_Keys curve2, float basePose, int frameCount, bool rotateMode)
    {
      if (curve1 == null)
      {
        return;
      }

      var values1 = Common.DecompCsklaKeys(curve1.Segments, frameCount, float.MaxValue);
      var values2 = Common.DecompGfmotKeys(curve2, frameCount, basePose);

      for (var i = 0; i < values1.Length; ++i)
      {
        var value1 = values1[i];
        var value2 = values2[i];

        if (rotateMode)
        {
          // Rotate用のチェック

          // 値確認
          if (!EqualRange(value1, value2, EqualRangeDefault))
          {
            // cosの値も確認
            var cos1 = (float)Math.Abs(Math.Cos(value1));
            var cos2 = (float)Math.Abs(Math.Cos(value2));

            if (!EqualRange(cos1, cos2, EqualRangeDefault))
            {
              sw.Write(desc);
              sw.WriteLine("値が違います : keys[{0}].Value {1}, {2}", i, value1, value2);
            }
          }
        }
        else
        {
          // Scale, Translate用のチェック

          // 値確認
          if (!EqualRange(value1, value2, EqualRangeDefault))
          {
            sw.Write(desc);
            sw.WriteLine("値が違います : keys[{0}].Value {1}, {2}", i, value1, value2);
          }
        }
      }
    }

    private static void CompareCurve2(
      StreamWriter sw, string desc, 
      Cskla.SegmentsFloatCurve[] curves1,
      Gfmot.Common_Keys[] curves2,
      float[] basePoses1,
      float[] basePoses2,
      int frameCount, bool rotateMode)
    {
      var values1 = new float[3][]
      {
        Common.DecompCsklaKeys((curves1[0] == null) ? null : curves1[0].Segments, frameCount, basePoses1[0]),
        Common.DecompCsklaKeys((curves1[1] == null) ? null : curves1[1].Segments, frameCount, basePoses1[1]),
        Common.DecompCsklaKeys((curves1[2] == null) ? null : curves1[2].Segments, frameCount, basePoses1[2]),
      };
      var values2 = new float[3][]
      {
        Common.DecompGfmotKeys(curves2[0], frameCount, basePoses2[0]),
        Common.DecompGfmotKeys(curves2[1], frameCount, basePoses2[1]),
        Common.DecompGfmotKeys(curves2[2], frameCount, basePoses2[2]),
      };

      for (var i = 0; i <= frameCount; ++i)
      {
        var value1 = new float[]
        {
          values1[0][i],
          values1[1][i],
          values1[2][i],
        };
        var value2 = new float[]
        {
          values2[0][i],
          values2[1][i],
          values2[2][i],
        };

        if (rotateMode)
        {
          // Rotate用のチェック

          // ジンバルロックしているか？
          if (EqualRange((float)Math.Abs(value1[1]), (float)Math.PI / 2, EqualRangeDefault) && EqualRange((float)Math.Abs(value2[1]), (float)Math.PI / 2, EqualRangeDefault))
          {
            // X, Zの合計が一致していればいい
            var sum1 = Math.Cos(value1[0] + value1[2]);
            var sum2 = Math.Cos(value2[0] + value2[2]);

            if (EqualRange((float)Math.Abs(sum1), (float)Math.Abs(sum2), EqualRangeDefault))
            {
              // 問題ない
              continue;
            }
          }

          // 値確認
          for (var j = 0; j < 3; ++j)
          {
            if (!EqualRange(value1[j], value2[j], EqualRangeDefault))
            {
              // cosの値も確認
              var cos1 = (float)(Math.Cos(value1[j]));
              var cos2 = (float)(Math.Cos(value2[j]));

              if (!EqualRange(cos1, cos2, EqualRangeDefault))
              {
                sw.Write(desc + VectorNames[j]);
                sw.WriteLine("値が違います : keys[{0}].Value {1}, {2}", i, value1[j], value2[j]);
              }
            }
          }
        }
        else
        {
          // Scale, Translate用のチェック

          // 値確認
          for (var j = 0; j < 3; ++j)
          {
            if (!EqualRange(value1[j], value2[j], EqualRangeDefault))
            {
              sw.Write(desc + VectorNames[j]);
              sw.WriteLine("値が違います : keys[{0}].Value {1}, {2}", i, value1[j], value2[j]);
            }
          }
        }
      }
    }
  }
}
