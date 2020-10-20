//using UnityEngine;
using System.Collections;
using System.Runtime.CompilerServices;

#if true
using System.IO;


[assembly: InternalsVisibleTo ("FunctionCurveCompress")]
[assembly: InternalsVisibleTo ("FunctionCurveCompress.Segment")]
#endif
public class FunctionCurve
{
	internal class FunctionCurveKey
	{
		public float frame;
		public float value;
		public float slope_l;
		public float slope_r;

		public FunctionCurveKey (float kf, float v, float sl_l, float sl_r)
		{
			frame = kf;
			value = v;
			slope_l = sl_l;
			slope_r = sl_r;
		}

		public FunctionCurveKey (FunctionCurveKey fck)
		{
			frame = fck.frame;
			value = fck.value;
			slope_l = fck.slope_l;
			slope_r = fck.slope_r;
		}
	}

	internal FunctionCurveKey[] functionCurveKey;
	//		public int currentKeyFrame; // @todo optimize
	//		public float frame;
	//		public float frameCount;

	public float GetFrameValue (float fr)
	{
		FunctionCurveKey n0, n1;
		n0 = n1 = null;
		float p0X, p3X;
		p0X = p3X = 0.0f; // no need to do this...

		int sum = functionCurveKey.Length;
		for (int i = 0; i < sum; ++i) {
			FunctionCurveKey fckv = functionCurveKey [i];
			p3X = fckv.frame;
			if (fr < p3X) {
				n1 = fckv;
				break;
			}
			n0 = fckv;
			p0X = p3X;
		}
		float v0 = n0.value;
		if (n1 == null) {
			return v0;
		}

		float vr = n0.slope_r;
		float v1 = n1.value;
		float vl = n1.slope_l;

		float t2 = fr - p0X;
		float t = t2 / (p3X - p0X);
		float t3 = t - 1.0f;
		return (t * vl + t3 * vr) * t3 * t2 + t * t * ((t3 + t3) - 1.0f) * (v0 - v1) + v0;
	}

	float ReadFloat (byte[] dat, ref int idx)
	{
		float ret = System.BitConverter.ToSingle (dat, idx);
//		Debug.Log ("uv val:" + ret);
		idx += 4;
		return ret;
	}

	public FunctionCurve (byte[] dat, ref int idx)
	{
		int sum = System.BitConverter.ToInt32 (dat, idx);
		idx += 4;
//		Debug.Log ("uv sum:" + sum);
		functionCurveKey = new FunctionCurveKey[sum];
		for (int i = 0; i < sum; i++) {
			functionCurveKey [i] = new FunctionCurveKey (
				ReadFloat (dat, ref idx), ReadFloat (dat, ref idx), ReadFloat (dat, ref idx), ReadFloat (dat, ref idx));
		}
	}

  // アニメーションしているか？
  public bool HasAnimation()
  {
    // 要素数が２より多ければアニメーションしている
    if (this.functionCurveKey.Length > 2)
    {
      return true;
    }

    // 要素数が２で同じ値ならアニメーションしていない
    if (this.functionCurveKey[0].value != this.functionCurveKey[1].value)
    {
      return true;
    }

    if (this.functionCurveKey[0].slope_l != 0.0f)
    {
      return true;
    }
    if (this.functionCurveKey[0].slope_r != 0.0f)
    {
      return true;
    }
    if (this.functionCurveKey[1].slope_l != 0.0f)
    {
      return true;
    }
    if (this.functionCurveKey[1].slope_r != 0.0f)
    {
      return true;
    }

    return false;
  }

	public FunctionCurve ()
	{
	}

	public void Save(BinaryWriter bw)
	{
		bw.Write((uint)functionCurveKey.Length);
		foreach (var fcurve in functionCurveKey)
    {
			bw.Write((int)fcurve.frame);
			bw.Write(fcurve.value);
			bw.Write(fcurve.slope_l);
			bw.Write(fcurve.slope_r);
		}
	}

  public void SaveText(StreamWriter sw)
  {
    sw.Write("FrameCount : ");
		sw.WriteLine((uint)functionCurveKey.Length);
		foreach (var fcurve in functionCurveKey)
    {
      sw.Write("  Frame=");
			sw.Write((int)fcurve.frame);

      sw.Write(" Value=");
			sw.Write(fcurve.value);

      sw.Write(" InSlope=");
			sw.Write(fcurve.slope_l);

      sw.Write(" OutSlope=");
			sw.WriteLine(fcurve.slope_r);
		}
  }

  public void SaveDefaultValue(BinaryWriter bw)
  {
    bw.Write(this.functionCurveKey[0].value);
  }
}
