using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace AnimationConverter
{
  enum AnimTypes
  {
    Basepose,
    Static0,
    Static1,
    OneFrame,
    HermiteU16,
    HermiteF32,
    //Linear, // 対応する？
    //Step, // 対応する？

    // 3bitまでしか使えない
  }

  class AnimKey
  {
    public int frame;
    public float value;
    public float slope;

    public AnimKey(int frame, float value, float slope)
    {
      this.frame = frame;
      this.value = value;
      this.slope = slope;
    }
  }

  class AnimKeyList
  {
    public static AnimKeyList CreateFromGfmot(Gfmot.Common_Keys gfmotKeys)
    {
      var animKeyList = new AnimKeyList();
      animKeyList.gfmotKeys = gfmotKeys;

      if (gfmotKeys == null)
      {
        // ベースポーズ
      }
      else if (gfmotKeys.HermiteKeys != null)
      {
        // エルミートキー
        animKeyList.keys = new AnimKey[gfmotKeys.HermiteKeys.Length];
        for (var i = 0; i < gfmotKeys.HermiteKeys.Length; ++i)
        {
          var gfmotKey = gfmotKeys.HermiteKeys[i];
          animKeyList.keys[i] = new AnimKey(gfmotKey.Frame, gfmotKey.Value, gfmotKey.Slope);
        }
      }
      else if (gfmotKeys.OneFrameKey != null)
      {
        // １フレームキー
        animKeyList.keys = new AnimKey[1];

        var gfmotKey = gfmotKeys.OneFrameKey;
        animKeyList.keys[0] = new AnimKey(0, gfmotKey.Value, 0.0f);
      }

      return animKeyList;
    }

    private const float OneFrameCompRange = 0.000001f;

    public AnimTypes AnimType
    {
      get
      {
        if (this.keys == null)
        {
          // キーがないのでベースポーズを使う
          return AnimTypes.Basepose;
        }

        // 値が1つだけのキー
        if (this.keys.Length == 1)
        {
          if (this.keys[0].value == 0.0f)
          {
            // 0で圧縮
            return AnimTypes.Static0;
          }
          if (this.keys[0].value == 1.0f)
          {
            // 1で圧縮
            return AnimTypes.Static1;
          }
          return AnimTypes.OneFrame;
        }

        // 値が2つだけのキー
        if (this.keys.Length == 2)
        {
          var sub = Math.Abs(this.keys[0].value - this.keys[1].value);
          if (sub < OneFrameCompRange)
          {
            if (this.keys[0].value == 0.0f)
            {
              // 0で圧縮
              return AnimTypes.Static0;
            }
            if (this.keys[0].value == 1.0f)
            {
              // 1で圧縮
              return AnimTypes.Static1;
            }
            return AnimTypes.OneFrame;
          }
        }

        if (this.CanExpoertU16())
        {
          // u16で大丈夫
          return AnimTypes.HermiteU16;
        }

        // 値の変動が大き過ぎるものはfloat32bitで
        return AnimTypes.HermiteF32;
      }
    }

    private bool CanExpoertU16()
    {
      var valueScale = 0.0f;
      var valueOffset = 0.0f;
      var slopeScale = 0.0f;
      var slopeOffset = 0.0f;
      var newValues = this.GetU16Values(out valueScale, out valueOffset);
      if (newValues == null)
      {
        return false;
      }
      var newSlopes = this.GetU16Slopes(out slopeScale, out slopeOffset);

      return (newSlopes != null);
    }

    public void Save(BinaryWriter bw, int frameCount)
    {
      switch (this.AnimType)
      {
      case AnimTypes.Basepose:
        // ベースポーズ
        break;
      case AnimTypes.Static0:
        // 0
        break;
      case AnimTypes.Static1:
        // 1
        break;
      case AnimTypes.OneFrame:
        // １フレームアニメ
        bw.Write(this.keys[0].value);
        break;
      case AnimTypes.HermiteU16:
        // エルミートU16
        this.SaveHermiteU16(bw, frameCount);
        break;
        // エルミートF32
      case AnimTypes.HermiteF32:
        this.SaveHermiteF32(bw, frameCount);
        break;
      }
    }

    private void SeveHermiteCommon(BinaryWriter bw, int frameCount)
    {
      // キー数
      bw.Write(this.keys.Length);

      // キーインデックス
      if (frameCount <= byte.MaxValue)
      {
        // キーを１バイトで出力
        foreach (var key in this.keys)
        {
          bw.Write((byte)key.frame);
        }
      }
      else
      {
        // キーを２バイトで出力
        foreach (var key in this.keys)
        {
          bw.Write((ushort)key.frame);
        }
      }

      // 4バイトアラインメントに揃える
      Common.Align4Byte(bw);
    }

    private void SaveHermiteU16(BinaryWriter bw, int frameCount)
    {
      // 共通部分
      this.SeveHermiteCommon(bw, frameCount);

      // スケールとオフセット
      var valueScale = 0.0f;
      var valueOffset = 0.0f;
      var slopeScale = 0.0f;
      var slopeOffset = 0.0f;
      var newValues = this.GetU16Values(out valueScale, out valueOffset);
      var newSlopes = this.GetU16Slopes(out slopeScale, out slopeOffset);

      bw.Write(valueScale);
      bw.Write(valueOffset);
      bw.Write(slopeScale);
      bw.Write(slopeOffset);

      // 値とスケール
      for (var i = 0; i < newValues.Length; ++i)
      {
        bw.Write(newValues[i]);
        bw.Write(newSlopes[i]);
      }
    }
    private void SaveHermiteF32(BinaryWriter bw, int frameCount)
    {
      // 共通部分
      this.SeveHermiteCommon(bw, frameCount);

      // 値とスケール
      for (var i = 0; i < this.keys.Length; ++i)
      {
        bw.Write(this.keys[i].value);
        bw.Write(this.keys[i].slope);
      }
    }

    private ushort[] GetU16Values(out float scale, out float offset)
    {
      var values = new float[this.keys.Length];
      for (var i = 0; i < values.Length; ++i)
      {
        values[i] = this.keys[i].value;
      }

      return ConvU16(values, out scale, out offset);
    }
    private ushort[] GetU16Slopes(out float scale, out float offset)
    {
      var slopes = new float[this.keys.Length];
      for (var i = 0; i < slopes.Length; ++i)
      {
        slopes[i] = this.keys[i].slope;
      }

      return ConvU16(slopes, out scale, out offset);
    }

    private static ushort[] ConvU16(float[] values, out float scale, out float offset)
    {
      var minValue = float.MaxValue;
      var maxValue = -float.MaxValue;

      // 0.0f ～ 1.0fに変換
      foreach (var value in values)
      {
        if (minValue > value)
        {
          minValue = value;
        }
        if (maxValue < value)
        {
          maxValue = value;
        }
      }

      offset = minValue;
      scale = maxValue - minValue;
      // 差が激し過ぎる
      if (scale > ushort.MaxValue)
      {
        return null;
      }

      var valuesU16 = new ushort[values.Length];
      for (var i = 0; i < values.Length; ++i)
      {
        var original = values[i];
        var value = original - offset;
        value /= scale;

        var valueU16 = (ushort)(value * ushort.MaxValue);
        valuesU16[i] = valueU16;
      }

      return valuesU16;
    }

    public AnimKey[] keys = null;
    public Gfmot.Common_Keys gfmotKeys = null;
  }

  class KeyCompress
  {
    // 再圧縮
    public static AnimKeyList ReComp(AnimKeyList animKeyList, float errorValue)
    {
      // 値を展開
      var frameCount = animKeyList.keys[animKeyList.keys.Length - 1].frame;
      var values = Common.DecompGfmotKeys(animKeyList.gfmotKeys, frameCount, float.NaN);

      return Comp(values, errorValue, animKeyList.gfmotKeys);
    }

    public static AnimKeyList Comp(float[] values, float errorValue, Gfmot.Common_Keys gfmotKeys)
    {
      // キー作成
      var originalKeys = new List<AnimKey>();
      for (var i = 0; i < values.Length; ++i)
      {
        var slope = 0.0f;

        if (i == 0)
        {
          // 最初のフレームは隣との差分だけ
          var subUp = values[i + 1] - values[i];
          slope = subUp;
        }
        else if (i == values.Length - 1)
        {
          // 最後のフレームも隣との差分だけ
          var subDown = values[i] - values[i - 1];
          slope = subDown;
        }
        else
        {
          // それ以外は両隣の差分の平均
          var subUp = values[i + 1] - values[i];
          var subDown = values[i] - values[i - 1];
          slope = (subUp + subDown) / 2.0f;
        }

        var key = new AnimKey(i, values[i], slope);
        originalKeys.Add(key);
      }

      // キーを間引く
      var newAnimKeyList = new AnimKeyList();
      newAnimKeyList.keys = Do(originalKeys, gfmotKeys, errorValue).ToArray();
      newAnimKeyList.gfmotKeys = gfmotKeys;

      return newAnimKeyList;
    }

    // 回転をクォータニオンに変換して再圧縮
    public static bool ReCompRotateQuat(int frameCount, ref AnimKeyList animKeyListX, ref AnimKeyList animKeyListY, ref AnimKeyList animKeyListZ, float[] basepose, float errorValue, string debugNodeName)
    {
      // オイラー角との誤差を出したくないのでキーは増えるが厳しくチェックする
      errorValue *= 0.1f;

      // 値を展開
      var valuesX = Common.DecompGfmotKeys(animKeyListX.gfmotKeys, frameCount, basepose[0]);
      var valuesY = Common.DecompGfmotKeys(animKeyListY.gfmotKeys, frameCount, basepose[1]);
      var valuesZ = Common.DecompGfmotKeys(animKeyListZ.gfmotKeys, frameCount, basepose[2]);

      // スローモーションチェック用に間にキーを挿入する
      var slowValuesX = new float[valuesX.Length * 2 - 1];
      var slowValuesY = new float[slowValuesX.Length];
      var slowValuesZ = new float[slowValuesX.Length];
      for (var i = 0; i < valuesX.Length; ++i)
      {
        var slowIndex = i * 2;
        slowValuesX[slowIndex] = valuesX[i];
        slowValuesY[slowIndex] = valuesY[i];
        slowValuesZ[slowIndex] = valuesZ[i];
      }
      for (var i = 1; i < slowValuesX.Length; i += 2)
      {
        var prevIndex = i - 1;
        var nextIndex = i + 1;

        {
          var prev = slowValuesX[prevIndex];
          var next = slowValuesX[nextIndex];
          slowValuesX[i] = Common.Lerp(prev, next, 0.5f);
        }
        {
          var prev = slowValuesY[prevIndex];
          var next = slowValuesY[nextIndex];
          slowValuesY[i] = Common.Lerp(prev, next, 0.5f);
        }
        {
          var prev = slowValuesZ[prevIndex];
          var next = slowValuesZ[nextIndex];
          slowValuesZ[i] = Common.Lerp(prev, next, 0.5f);
        }
      }

      // オイラー角からクォータニオンへ変換
      var quatList = new Quaternion[slowValuesX.Length];
      var rotMatrixList = new Matrix[slowValuesX.Length];
      var oldQuat = null as Quaternion;
      for (var i = 0; i < quatList.Length; ++i)
      {
        var rotMatrix = Matrix.GetRotationZYX(slowValuesZ[i], slowValuesY[i], slowValuesX[i]);
        var quat = new Quaternion();
        quat.FromMatrix(rotMatrix);

        // 共役の方が近いかもしれない
        if (oldQuat != null)
        {
          var dot = Quaternion.Dot(quat, oldQuat);
          if (dot < 0.0f)
          {
            var conj = quat.Conj();
            quat = conj;
          }
        }

        var rotMatirxA = quat.ConvToMatrix();
        if (!Matrix.Eqaul3x3(rotMatrix, rotMatirxA, 0.9f))
        {
          throw new Exception("Faild to Convert Quaternion : 0");
        }

        oldQuat = quat;
        quatList[i] = quat;
        rotMatrixList[i] = rotMatrix;
      }

      // 対数クォータニオンに変換
      var quatLnList = new Quaternion[quatList.Length];
      for (var i = 0; i < quatList.Length; ++i)
      {
        var quatLn = quatList[i].Clone();
        quatLn.ConvLn();

        quatLnList[i] = quatLn;
      }

      // フリップチェック
      for (var i = 1; i < quatList.Length; i += 2)
      {
        var prevIndex = i - 1;
        var nextIndex = i + 1;

        // 半分のところ
        var quatLnPrev = quatLnList[prevIndex].Clone();
        var quatLnNext = quatLnList[nextIndex].Clone();
        var quatLn = Quaternion.Lerp(quatLnPrev, quatLnNext, 0.5f);
        quatLn.ConvExp();

        var rotMatrix = rotMatrixList[i];
        var rotMatrixB = quatLn.ConvToMatrix();

        if (!Matrix.Eqaul3x3(rotMatrix, rotMatrixB, 0.5f))
        {
          // オイラー角を使う
          return false;
        }

        var rotXPrev = slowValuesX[prevIndex];
        var rotXNext = slowValuesX[nextIndex];

        var rotYPrev = slowValuesY[prevIndex];
        var rotYNext = slowValuesY[nextIndex];

        var rotZPrev = slowValuesZ[prevIndex];
        var rotZNext = slowValuesZ[nextIndex];

        // 0.25
        var quatLn_025 = Quaternion.Lerp(quatLnPrev, quatLnNext, 0.25f);
        quatLn_025.ConvExp();

        var rotMatrix_025 = Matrix.GetRotationZYX(
          Common.Lerp(rotZPrev, rotZNext, 0.25f),
          Common.Lerp(rotYPrev, rotYNext, 0.25f),
          Common.Lerp(rotXPrev, rotXNext, 0.25f)
          );
        var rotMatrixB_025 = quatLn_025.ConvToMatrix();

        if (!Matrix.Eqaul3x3(rotMatrix_025, rotMatrixB_025, 0.5f))
        {
          // オイラー角を使う
          return false;
        }

        // 0.75
        var quatLn_075 = Quaternion.Lerp(quatLnPrev, quatLnNext, 0.75f);
        quatLn_075.ConvExp();

        var rotMatrix_075 = Matrix.GetRotationZYX(
          Common.Lerp(rotZPrev, rotZNext, 0.75f),
          Common.Lerp(rotYPrev, rotYNext, 0.75f),
          Common.Lerp(rotXPrev, rotXNext, 0.75f)
          );
        var rotMatrixB_075 = quatLn_075.ConvToMatrix();

        if (!Matrix.Eqaul3x3(rotMatrix_075, rotMatrixB_075, 0.5f))
        {
          // オイラー角を使う
          return false;
        }
      }

      var quatX = new float[frameCount + 1];
      var quatY = new float[quatX.Length];
      var quatZ = new float[quatX.Length];

      for (var i = 0; i < quatLnList.Length; i += 2)
      {
        var quat = quatLnList[i];

        var index = i / 2;
        quatX[index] = quat.x;
        quatY[index] = quat.y;
        quatZ[index] = quat.z;
      }

      // 圧縮
      animKeyListX = Comp(quatX, errorValue, null);
      animKeyListY = Comp(quatY, errorValue, null);
      animKeyListZ = Comp(quatZ, errorValue, null);

      // クォータニオンを使う
      return true;
    }

    // クォータニオン用のフィルター
    static bool QuatFilter(float[] values, float[] others0, float[] others1)
    {
      var result = false;
      var oldValue = values[0];

      var pi = (float)Math.PI;
      var half_pi = pi * 0.95f;
      for (var i = 1; i < values.Length; ++i)
      {
        var value = values[i];
        var other0 = others0[i];
        var other1 = others1[i];

        // 繋がらない可能性があるので補正してやる
        if ((other0 == 0.0f) && (other1 == 0.0f))
        {
          while (true)
          {
            // 差分がpi/2より大きければそちらに寄せる
            var sub = oldValue - value;
            if (Math.Abs(sub) < half_pi)
            {
              break;
            }

            // 符号が変わるのは禁止
            var newValue = value;
            newValue += (sub > 0.0f) ? pi : -pi;

            var valueSign = Math.Sign(value);
            if (valueSign != 0)
            {
              var newValueSign = Math.Sign(newValue);
              if (valueSign * newValueSign != 1)
              {
                break;
              }
            }

            value = newValue;
            result = true;
          }
        }

        values[i] = value;
        oldValue = value;
      }

      return result;
    }

    // 後ろから探索
    static int FindOptKeyBack(List<AnimKey> originalKeys, Gfmot.Common_Keys gfmotKeys, int beginIndex, float errorValue)
    {
      // 後ろから全フレームをチェックする
      var endIndex = originalKeys.Count - 1;

      var keyCurrent = originalKeys[beginIndex];
      var keyNext = originalKeys[endIndex];

      for (var i = beginIndex + 1; i < endIndex; ++i)
      {
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
            continue;
          }
        }

        // gfmotのキーがあるなら中間フレームもチェックする
        if (gfmotKeys != null)
        {
          // 計算式から復元
          var p = (i + 0.5f) - keyCurrent.frame;
          var d = keyNext.frame - keyCurrent.frame;
          var value = Hermite(keyCurrent.value, keyCurrent.slope, keyNext.value, keyNext.slope, p, d);

          // 差分が許容値以内か？
          //var keyOriginal = originalKeys[i];
          var originalValue = Common.DecompGfmotKey(gfmotKeys, i + 0.5f, float.NaN);
          var subValue = originalValue - value;
          subValue = Math.Abs(subValue);
          if (subValue > errorValue)
          {
            // 始めからやり直し
            --endIndex;
            keyNext = originalKeys[endIndex];

            i = beginIndex;
            continue;
          }
        }

      }

      return endIndex;
    }

    static List<AnimKey> Do(List<AnimKey> originalKeys, Gfmot.Common_Keys gfmotKeys, float errorValue)
    {
      // 最初のキーだけ入れておく
      var keys = new List<AnimKey>();
      keys.Add(originalKeys[0]);

      // 後は最適なキーを見つけて足していく
      var addIndex = 0;
      while (addIndex != originalKeys.Count - 1)
      {
        addIndex = FindOptKeyBack(originalKeys, gfmotKeys, addIndex, errorValue);
        keys.Add(originalKeys[addIndex]);
      }

      return keys;
    }

    public static float Hermite(float v0, float t0, float v1, float t1, float p, float d)
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
    //public static float Hermite(float v0, float s0, float v1, float s1, float frame, float currentFrame, float nextFrame)
    //{
    //  float t, t2, t3, p0X, pX, p3X;

    //  pX = frame / 30.0f;
    //  p0X = currentFrame / 30.0f;
    //  p3X = nextFrame / 30.0f;

    //  t2 = pX - p0X;
    //  t = t2 / (p3X - p0X);
    //  t3 = t - 1.0f;
    //  var value = (t * s1 + t3 * s0) * t3 * t2 + t * t * ((t3 + t3) - 1.0f) * (v0 - v1) + v0;

    //  return value;
    //}
  }
}
