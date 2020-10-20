using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace Mot
{
  enum Kinds
  {
    FC_STATIC_0,
    FC_STATIC_BASEPOSE,
    FC_STATIC_HALF_PI,
    FC_STATIC_PI,
    FC_STATIC_ONE_HALF_PI,
    FC_STATIC_DATA,
    // above here static data
    // below here spline data
    FC_SPLINE_EXIST,
    FC_SPLINE_SLOPE_0 = FC_SPLINE_EXIST,
    FC_SPLINE_DATA,
  }

  enum TRSIndex
  {
    TransX,
    TransY,
    TransZ,

    RotX,
    RotY,
    RotZ,

    ScaleX,
    ScaleY,
    ScaleZ,

    Count
  }

  class FKey
  {
    public int frame;
    public float value;
    public float slope;
  }

  class FCurve
  {
    public string jointName;
    public Kinds kind;

    public int keyCount;
    public List<FKey> keyList = new List<FKey>();
  }

  class Mot
  {
    private int frameCount = 0;
    private FCurve[] fcurveList = null;

    public static Mot LoadMot(string motFilePath, string[] jointNames)
    {
      var mot = new Mot();

      using (var br = new BinaryReader(File.Open(motFilePath, FileMode.Open)))
      {
        var fcurveCount = br.ReadUInt16();
        var frameCount = br.ReadUInt16();

        var lpCount = (fcurveCount * 3) / 24;
        if ((fcurveCount & 7) != 0)
        {
          ++lpCount;
        }

        var fcurveList = new FCurve[fcurveCount];

        var jointNameIndex = 0;
        var trsIndex = TRSIndex.TransX;
        var index = 0;
        for (var lp = 0; lp < lpCount; ++lp)
        {
          var val0 = (uint)br.ReadByte();
          var val1 = (uint)br.ReadByte();
          var val2 = (uint)br.ReadByte();
          var value = (uint)(val0 | (val1 << 8) | (val2 << 16));

          for (var j = 0; j < 24; j += 3)
          {
            var kind = (Kinds)((value >> j) & 0x07);
            fcurveList[index] = new FCurve();
            fcurveList[index].kind = kind;

            var name = string.Empty;
            if (index == 0)
            {
              name = "LocatorRotY";
            }
            else if (index == 1)
            {
              name = "LocatorTransY";
            }
            else
            {
              name = jointNames[jointNameIndex];
              if (kind == Kinds.FC_STATIC_BASEPOSE)
              {
                var trs = string.Empty;
                if (trsIndex == TRSIndex.TransX)
                {
                  trs = "TransXYZ";
                }
                else if (trsIndex == TRSIndex.RotX)
                {
                  trs = "RotXYZ";
                }
                else
                {
                  trs = "ScaleXYZ";
                }
                name += "_" + trs;

                trsIndex += 3;
              }
              else
              {
                name += "_" + trsIndex.ToString();
                ++trsIndex;
              }

              if (trsIndex == TRSIndex.Count)
              {
                trsIndex = TRSIndex.TransX;
                ++jointNameIndex;
              }
            }
            fcurveList[index].jointName = name;
            ++index;

            if (index >= fcurveList.Length)
            {
              break;
            }
          }
        }

        index = 0;
        foreach (var fcurve in fcurveList)
        {
          if ((fcurve.kind == Kinds.FC_SPLINE_EXIST) || (fcurve.kind == Kinds.FC_SPLINE_DATA))
          {
            fcurve.keyCount = br.ReadByte();

            // 最初のキー
            var firstKey = new FKey();
            firstKey.frame = 0;
            fcurve.keyList.Add(firstKey);

            if (frameCount < byte.MaxValue)
            {
              // u8のキー
              for (var i = 0; i < fcurve.keyCount; ++i)
              {
                var key = new FKey();
                key.frame = br.ReadByte();
                fcurve.keyList.Add(key);
              }
            }
            else
            {
              // u16のキー

              // 2バイトアラインメント
              while (br.BaseStream.Position % 2 != 0)
              {
                var dummy = br.ReadByte();
              }

              for (var i = 0; i < fcurve.keyCount; ++i)
              {
                var key = new FKey();
                key.frame = br.ReadInt16();
                fcurve.keyList.Add(key);
              }
            }

            // 最後のキー
            var lastKey = new FKey();
            lastKey.frame = frameCount;
            fcurve.keyList.Add(lastKey);

            // 最初と最後のキーの分
            fcurve.keyCount += 2;
          }
        }

        // 4バイトアラインメント
        while (br.BaseStream.Position % 4 != 0)
        {
          var dummy = br.ReadByte();
        }

        foreach (var fcurve in fcurveList)
        {
          //if ((fcurve.kind == Kinds.FC_SPLINE_EXIST) || (fcurve.kind == Kinds.FC_SPLINE_DATA))
          if (Kinds.FC_SPLINE_EXIST <= fcurve.kind)
          {
            for (var i = 0; i < fcurve.keyCount; ++i)
            {
              fcurve.keyList[i].value = br.ReadSingle();
              fcurve.keyList[i].slope = br.ReadSingle();
            }

            if (fcurve.kind == Kinds.FC_SPLINE_EXIST)
            {
              br.BaseStream.Position -= fcurve.keyCount * sizeof(float);
            }
          }
          else if (fcurve.kind == Kinds.FC_STATIC_DATA)
          {
            var key = new FKey();
            key.value = br.ReadSingle();
            fcurve.keyList.Add(key);
          }
          else
          {
            var key = new FKey();
            switch (fcurve.kind)
            {
            case Kinds.FC_STATIC_0:
              key.value = 0.0f;
              break;
            case Kinds.FC_STATIC_BASEPOSE:
              key.value = float.NaN;
              break;
            case Kinds.FC_STATIC_HALF_PI:
              key.value = (float)Math.PI * 0.5f;
              break;
            case Kinds.FC_STATIC_PI:
              key.value = (float)Math.PI;
              break;
            case Kinds.FC_STATIC_ONE_HALF_PI:
              key.value = (float)Math.PI * 1.5f;
              break;
            }
            fcurve.keyList.Add(key);
          }

          var keyList = fcurve.keyList;
          ++index;
        }

        mot.frameCount = frameCount;
        mot.fcurveList = fcurveList.ToArray();
      }

      return mot;
    }

    public void DumpText(string outputTextPath)
    {
      using (var sw = new StreamWriter(File.Open(outputTextPath, FileMode.OpenOrCreate)))
      {
        sw.BaseStream.SetLength(0);

        sw.WriteLine("fcurveCount : {0}", this.fcurveList.Length);
        sw.WriteLine("frameCount : {0}", this.frameCount);

        sw.WriteLine();

        var fcurveIndex = 0;
        foreach (var fcurve in this.fcurveList)
        {
          sw.WriteLine("name : {0}", fcurve.jointName);
          sw.WriteLine("index : {0}", fcurveIndex);
          sw.WriteLine("kind : {0}", fcurve.kind);

          if ((fcurve.kind == Kinds.FC_SPLINE_EXIST) || (fcurve.kind == Kinds.FC_SPLINE_DATA))
          {
            sw.WriteLine("  keyCount : {0}", fcurve.keyCount);
            foreach (var key in fcurve.keyList)
            {
              //sw.WriteLine("  {0} : {1}, {2}", key.frame, key.value, key.slope);
              sw.WriteLine("  {0} : {1}", key.frame, key.value);
            }
          }
          else
          {
            sw.WriteLine("  staticValue : {0}", fcurve.keyList[0].value);
          }

          sw.WriteLine();
          ++fcurveIndex;
        }
      }
    }

    public static void DoTest5()
    {
#if false
      var file0 = new List<float>();
      var file1 = new List<float>();

      // .motファイルをダンプしてみる

        using (var sw = new StreamWriter(File.Open(@"R:\home\niji\git_program\tool_project\AnimationConverter\test_file\new_format\mot.txt", FileMode.OpenOrCreate)))
        {
          sw.WriteLine("fcurveCount : {0}", fcurveCount);
          sw.WriteLine("frameCount : {0}", frameCount);

          sw.WriteLine();

          var fcurveIndex = 0;
          foreach (var fcurve in fcurveList)
          {
            sw.WriteLine("index : {0}", fcurveIndex);
            sw.WriteLine("kind : {0}", fcurve.kind);

            if ((fcurve.kind == Kinds.FC_SPLINE_EXIST) || (fcurve.kind == Kinds.FC_SPLINE_DATA))
            {
              sw.WriteLine("  keyCount : {0}", fcurve.keyCount);
              foreach (var key in fcurve.keyList)
              {
                sw.WriteLine("  {0} : {1}, {2}", key.frame, key.a_value, key.b_slope);
              }
            }
            else
            {
              sw.WriteLine("  staticValue : {0}", fcurve.staticValue);
            }

            sw.WriteLine();
            ++fcurveIndex;
          }
        }


        // 値を復元してみる
        var fcurveIndex_ = 0;

        foreach (var fcurve in fcurveList)
        {
          //sw.WriteLine("index : {0}", fcurveIndex_);
          //sw.WriteLine("kind : {0}", fcurve.kind);

          var valueList = new List<float>();
          if ((fcurve.kind == Kinds.FC_SPLINE_EXIST) || (fcurve.kind == Kinds.FC_SPLINE_DATA))
          {
            //sw.WriteLine("  keyCount : {0}", fcurve.keyCount);
            //foreach (var key in fcurve.keyList)
            valueList.Add(fcurve.keyList[0].a_value);
            var keyIndex = 0;
            var current = fcurve.keyList[keyIndex];
            var next = fcurve.keyList[keyIndex + 1];
            for (var frame = 1; frame < frameCount; ++frame)
            {
              //sw.WriteLine("  {0} : {1}, {2}", key.frame, key.a_value, key.b_slope);
              if (next.frame < frame)
              {
                ++keyIndex;
                current = next;
                next = fcurve.keyList[keyIndex + 1];
              }

              float t, t2, t3, p0X, pX, p3X, v0;

              pX = frame / 30.0f;
              p0X = current.frame / 30.0f;
              p3X = next.frame / 30.0f;

              t2 = pX - p0X;
              t = t2 / (p3X - p0X);
              v0 = current.a_value;
              t3 = t - 1.0f;
              var value = (t * next.b_slope + t3 * current.b_slope) * t3 * t2 + t * t * ((t3 + t3) - 1.0f) * (v0 - next.a_value) + v0;
              valueList.Add(value);
            }
            valueList.Add(fcurve.keyList[fcurve.keyList.Count - 1].a_value);
          }
          else
          {
            //sw.WriteLine("  staticValue : {0}", fcurve.staticValue);
          }

          //sw.WriteLine();
          ++fcurveIndex_;
        }
#endif
    }
  }
}
