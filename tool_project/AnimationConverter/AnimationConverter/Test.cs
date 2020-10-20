using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace AnimationConverter
{
  class Test
  {
    public static double Hermite(double v0, double t0, double v1, double t1, double p, double d)
    {
      var s = p / d;
      var s_1 = s - 1;

      var tmp1 = (v0 - v1);
      var tmp2 = (2 * s - 3);
      var tmp3 = s * s;
      var tmp4 = p * s_1;
      var tmp5 = s_1 * t0;
      var tmp6 = s * t1;

      var ret = v0 + tmp1 * tmp2 * tmp3 + tmp4 * (tmp5 + tmp6);
      return ret;
    }

    // pm025 ba10_waitA01 gfmot waist rotateX
    static readonly double[] Data0 =
    {
      0.003565,
      0.004251,
      0.004986,
      0.005737,
      0.006469,
      0.007149,
      0.007741,
      0.008212,
      0.008528,
      0.008643,
      0.008557,
      0.008300,
      0.007901,
      0.007390,
      0.006798,
      0.006153,
      0.005485,
      0.004817,
      0.004160,
      0.003528,
      0.002935,
      0.002397,
      0.001928,
      0.001542,
      0.001253,
      0.001069,
      0.000985,
      0.000998,
      0.001105,
      0.001304,
      0.001590,
      0.001963,
      0.002418,
      0.002953,
      0.003565,
    };

    // pm025 ba20_buturi01 cskla LThigh rotateX
    // 全フレーム出力されていたのでこのデータで計算方法を調査
    static readonly double[] Data1 =
    {
      -0.5901733637,
      -0.7155527472,
      -1.183398128,
      -1.895238876,
      -2.227016687,
      -2.747343302,
      -1.624067307,
      -0.2774668932,
      -0.2564762831,
      -0.3837563694,
      -0.6307381392,
      -0.9127301574,
      -1.106734753,
      -1.22075963,
      -1.00630033,
      -0.8960858583,
      -0.896821022,
      -0.9041041732,
      -0.9184085131,
      -0.9427476525,
      -0.9857237339,
      -1.062990904,
      -1.151840329,
      -1.114751816,
      -0.8404219747,
      -0.5502430797,
      -0.3745697141,
      -0.2862147391,
      -0.2276132554,
      -0.1886859983,
      -0.1622080803,
      -0.309163332,
      -0.4105945826,
      -0.4187283814,
      -0.3694935143,
      -0.4271787107,
      -0.3986501992,
      -0.2345652878,
      -0.1569336504,
      -0.2130258828,
      -0.2733176053,
      -0.246715188,
      -0.3040981293,
      -0.4167264402,
      -0.469984442,
      -0.4945111275,
      -0.5662950873,
      -0.5734472871,
      -2.072851181,
      -1.899162054,
      -1.584460974,
      -1.216496825,
      -0.9234315157,
      -0.7380615473,
      -0.6344019175,
      -0.5901733637,
    };

    class Key
    {
      public int frame;
      public double value;
      public double slope;

      public Key(int frame, double value, double slope)
      {
        this.frame = frame;
        this.value = value;
        this.slope = slope;
      }

      public Key() :
        this(0, 0.0, 0.0)
      {
      }
    };

    // 後ろから探索、通常こちらの方が圧縮効率は良くなる
    static int FindOptKeyBack(List<Key> originalKeys, int beginIndex, double errorValue)
    {
      // 後ろから全フレームをチェックする
      var endIndex = originalKeys.Count - 1;

      var keyCurrent = originalKeys[beginIndex];
      var keyNext = originalKeys[endIndex];

      for (var i = beginIndex + 1; i < endIndex; ++i)
      {
        // 計算式から復元
        var p = i - keyCurrent.frame;
        var d = keyNext.frame - keyCurrent.frame;
        var value = Hermite(keyCurrent.value, keyCurrent.slope, keyNext.value, keyNext.slope, p, d);

        // 差分が許容値以内か？
        var keyOriginal = originalKeys[i];
        var subValue = keyOriginal.value - value;
        subValue = Math.Abs(subValue);
        if (subValue > errorValue)
        {
          // 始めからやり直し
          --endIndex;
          keyNext = originalKeys[endIndex];

          i = beginIndex;
        }
      }

      return endIndex;
    }

    // 前から探索、圧縮効率は悪くなるがNW4Cの再現なのでこちらを使う
    static int FindOptKeyFront(List<Key> originalKeys, int beginIndex, double errorValue)
    {
      // 前から全フレームをチェックする
      var endIndex = beginIndex + 1;

      var keyCurrent = originalKeys[beginIndex];
      var keyNext = originalKeys[endIndex];

      while (true)
      {
        var isFound = false;

        for (var i = beginIndex + 1; i < endIndex; ++i)
        {
          // 意図的にfloat精度で計算させる

          // 計算式から復元
          var p = i - keyCurrent.frame;
          var d = keyNext.frame - keyCurrent.frame;
          var value = Hermite((float)keyCurrent.value, (float)keyCurrent.slope, (float)keyNext.value, (float)keyNext.slope, p, d);

          // 差分が許容値以内か？
          var keyOriginal = originalKeys[i];
          var subValue = (float)keyOriginal.value - (float)value;
          subValue = Math.Abs(subValue);
          if (subValue >= errorValue)
          {
            isFound = true;
            break;
          }
        }

        if (isFound)
        {
          --endIndex;
          break;
        }

        ++endIndex;
        if (endIndex >= originalKeys.Count)
        {
          endIndex = originalKeys.Count - 1;
          break;
        }

        keyNext = originalKeys[endIndex];
      }

      return endIndex;
    }

    static List<Key> CompressKey(List<Key> originalKeys, double errorValue)
    {
      // 最初のキーだけ入れておく
      var keys = new List<Key>();
      keys.Add(originalKeys[0]);

      // 後は最適なキーを見つけて足していく
      var addIndex = 0;
      while (addIndex != originalKeys.Count - 1)
      {
        addIndex = FindOptKeyFront(originalKeys, addIndex, errorValue);
        keys.Add(originalKeys[addIndex]);
      }

      return keys;
    }

    public static void DoTest()
    {
      var data = original;

      // キー作成
      var originalKeys = new List<Key>();
      for (var i = 0; i < data.Length; ++i)
      {
        var key = new Key();

        key.frame = i;
        key.value = data[i];

        if (i == 0)
        {
          // 最初のフレームは隣との差分だけ
          var subUp = data[i + 1] - data[i];
          key.slope = subUp;
        }
        else if (i == data.Length - 1)
        {
          // 最後のフレームも隣との差分だけ
          var subDown = data[i] - data[i - 1];
          key.slope = subDown;
        }
        else
        {
          // それ以外は両隣の差分の平均
          var subUp = data[i + 1] - data[i];
          var subDown = data[i] - data[i - 1];
          key.slope = (subUp + subDown) / 2.0;
        }

        originalKeys.Add(key);
      }

      // キーを間引く
      var errorValue = 0.01;
      //errorValue = errorValue * Math.PI / 180.0;
      Console.WriteLine(errorValue);
      var optKeys = CompressKey(originalKeys, errorValue);

      // 展開して確認
      var indexCurrent = 0;
      var keyCurrent = optKeys[indexCurrent];
      var keyNext = optKeys[indexCurrent + 1];
      for (var i = 0; i < originalKeys.Count; ++i)
      {
        if (i > keyNext.frame)
        {
          keyCurrent = keyNext;
          ++indexCurrent;
          keyNext = optKeys[indexCurrent + 1];
        }

        var p = i - keyCurrent.frame;
        var d = keyNext.frame - keyCurrent.frame;
        var value = Hermite(keyCurrent.value, keyCurrent.slope, keyNext.value, keyNext.slope, p, d);

        var subValue = originalKeys[i].value - value;

        Console.WriteLine("{0}: {1}, {2}, {3}", i, originalKeys[i].value.ToString("0.000000"), value.ToString("0.000000"), subValue.ToString("0.000000"));
      }
    }

    static readonly Key[] keys0 = new Key[]
    {
      new Key(0,  0.8999999762, 0),
      new Key(15, 0.9791370034, 0.007286816835),
      new Key(37, 1,            0),
      new Key(43, 1,            -0.007812529802),
      new Key(47, 0.8999999762, -0.007812529802),
      new Key(55, 0.8999999762, 0),
    };
    static readonly Key[] keys1 = new Key[]
    {
      new Key(0,  0.89999997616, 0),
      new Key(23, 1,             0),
      new Key(42, 1,             0),
      new Key(45, 0.94999998808, -0.03437495232),
      new Key(53, 0.89999997616, 0),
      new Key(55, 0.89999997616, 0),
    };

    static readonly double[] original = new double[]
    {
84.92501345685,
85.39399451674,
86.58662967412,
88.18145670682,
89.85701414759,
91.29183926186,
92.16447050144,
92.15344594065,
90.93730279292,
84.99608352378,
77.10960908288,
74.23482081578,
70.79011442906,
66.39021944314,
66.72926373883,
64.73090982441,
64.94085000215,
65.27912379196,
65.99247852719,
67.23419439796,
68.86446909307,
71.93557881367,
75.9174879369,
80.28013688009,
84.49349463932,
88.02750080419,
90.3521214073,
90.93730279292,
82.89892174246,
73.51701425732,
72.12249872613,
71.09429500447,
67.19956378312,
67.58216370464,
65.54025416439,
65.66039101865,
65.69170295655,
66.185588956,
67.16263026064,
68.72837501111,
70.72284357169,
72.98603385567,
75.35796498019,
77.67864960086,
79.78810734887,
81.52633531505,
82.73335417208,
83.24917699364,
83.34539479037,
83.40889045896,
83.4526033727,
83.48947303993,
83.59673529321,
83.81260111359,
84.09147135064,
84.38774789024,
84.65582932696,
84.85011756597,
84.92501345685,
    };

    public static void DoTest2()
    {
      var values0 = new List<double>();
      var values1 = new List<double>();

      // 展開して確認
      var indexCurrent = 0;
      var keyCurrent = keys0[indexCurrent];
      var keyNext = keys0[indexCurrent + 1];
      for (var i = 0; i <= 55; ++i)
      {
        if (i > keyNext.frame)
        {
          keyCurrent = keyNext;
          ++indexCurrent;
          keyNext = keys0[indexCurrent + 1];
        }

        var p = i - keyCurrent.frame;
        var d = keyNext.frame - keyCurrent.frame;
        var value = Hermite(keyCurrent.value, keyCurrent.slope, keyNext.value, keyNext.slope, p, d);
        values0.Add(value);
      }
      indexCurrent = 0;
      keyCurrent = keys1[indexCurrent];
      keyNext = keys1[indexCurrent + 1];
      for (var i = 0; i <= 55; ++i)
      {
        if (i > keyNext.frame)
        {
          keyCurrent = keyNext;
          ++indexCurrent;
          keyNext = keys1[indexCurrent + 1];
        }

        var p = i - keyCurrent.frame;
        var d = keyNext.frame - keyCurrent.frame;
        var value = Hermite(keyCurrent.value, keyCurrent.slope, keyNext.value, keyNext.slope, p, d);
        values1.Add(value);
      }

      for (var i = 0; i <= 55; ++i)
      {
        var ori = original[i];
        var subValue0 = ori - values0[i];
        var subValue1 = ori - values1[i];

        Console.WriteLine("{0}: {1}, ({2}, {3}) ({4}, {5})", i, ori.ToString("0.000000"), values0[i].ToString("0.000000"), subValue0.ToString("0.000000"), values1[i].ToString("0.000000"), subValue1.ToString("0.000000"));
      }
    }
  }
}
