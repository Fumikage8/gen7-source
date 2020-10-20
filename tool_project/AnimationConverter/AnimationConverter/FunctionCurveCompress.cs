#if true
//using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class FunctionCurveCompress
{
	internal class InflectionPoint
	{
		public int frame;
		public float value;

		public InflectionPoint (int f = 0, float v = 0.0f)
		{
			frame = f;
			value = v;
		}
	}

	internal class Segment : FunctionCurve.FunctionCurveKey
	{
		public float[] valueArray;
		public bool error;

		public Segment (int kf, float v, float sl_l, float sl_r, bool er = true) : base ((float)kf, v, sl_l, sl_r)
		{
			error = er;
		}
	}

	List<Segment> segment = new List<Segment> ();
	int startSegment;

	float fabs (float val)
	{
		return (0.0f <= val) ? val : -val;
	}

	void SetValueSegment (int i)
	{
		List<Segment> lst = segment;
		float kf = lst [i + 1].frame - lst [i].frame;
		int kf_i = (int)kf;
		if (kf_i <= 1) {
			return;
		}
		float[] val;
		segment [i].valueArray = val = new float[kf_i - 1];
		float scl = 1.0f / kf;
		float v0 = lst [i].value;
		float vr = lst [i].slope_r;
		float v1 = lst [i + 1].value;
		float vl = lst [i + 1].slope_l;
//			Debug.Log ("i:" + i + ":" + segment.Length + ":" + max + ":" + kf + "=" + lst [i + 1].frame + "-" + lst [i].frame);
//			Debug.Log ("kf:" + lst [i + 1].frame + ":" + lst [i].frame + "?" + v0 + ":" + vr + ":" + v1 + ":" + vl);
		for (int j = 1; j < kf_i; ++j) {
			float t2 = (float)j;
			float t = t2 * scl;
			float t3 = t - 1.0f;
			val [j - 1] = (t * vl + t3 * vr) * t3 * t2 + t * t * ((t3 + t3) - 1.0f) * (v0 - v1) + v0;
//			Debug.Log ("val[" + j + "]" + val [j - 1]);
		}
	}

	bool SetErrorSegment (float[]value, float range)
	{
		bool ret = false;
		int max = segment.Count - 1;
		bool set_start = false;
		for (int i = startSegment; i < max; i++) {
			if (segment [i].error) {
				segment [i].error = false;
				SetValueSegment (i);
				float[] val = segment [i].valueArray;
				if (val != null) {
					int ofs = (int)segment [i].frame + 1;
					for (int j = 0; j < val.Length; ++j) {
						if (range < fabs (value [ofs + j] - val [j])) {
							segment [i].error = ret = true;
							if (!set_start) {
								startSegment = i;
								set_start = true;
//								Debug.Log ("startSegment:" + startSegment);
							}
							break;
						}
					}
				}
			}
		}
		return ret;
	}

	void InsertKey (float[] value, int ins, int i)
	{
//		Debug.Log ("insert:" + ins + ":" + value [ins]);
		if (segment [i - 1].frame + 1 == ins) {
			segment [i - 1].error = false;
		}
		segment.Insert (i, new Segment (ins, value [ins]
					, value [ins] - value [ins - 1], value [ins + 1] - value [ins], (ins + 1 != segment [i].frame)));
	}

	void InsertKey (float[] value, int ins)
	{
		int sum = segment.Count;
		float ins_f = (float)ins;
		for (int i = 1; i < sum; i++) {
			if (segment [i].frame == ins_f) {
				break;
			} else if (ins < segment [i].frame) {
				InsertKey (value, ins, i);
				break;
			}
		}
	}

	void SetSlopeKey (float[] value)
	{
		int sum = segment.Count - 1;
/*		Debug.Log ("bef:" + sum);
		for (int i = 0; i < sum; i++) {
			Debug.Log ("[" + i + "]:" + segment [i].frame + ":" + segment [i].value + ":" + segment [i]);
		}*/
		for (int i = startSegment; i < sum; i++) {
			if (segment [i].error) {
				float[] val = segment [i].valueArray;//GetValuesegment (segment, i);
				int ofs = (int)segment [i].frame + 1;
				float diff_max = 0.0f;
				int diff_i = 0;
				for (int j = 0; j < val.Length; ++j) {
					float diff = fabs (value [ofs + j] - val [j]);
					if (diff_max < diff) {
						diff_max = diff;
						diff_i = j;
					}
				}
				int ins = ofs + diff_i;
//				Debug.Log ("ins[" + i + "]:" + ins + "=" + ofs + "+" + diff_i + ":" + diff_max);
				InsertKey (value, ins, i + 1);
/*				++sum;
				Debug.Log ("aft:" + sum);
				for (int j = 0; j < sum; j++) {
					Debug.Log ("[" + j + "]:" + segment [j].frame + ":" + segment [j].value + ":" + segment [j].error);
				}*/
//				Debug.Break ();
				return;
			}
		}
	}

	FunctionCurve SetKeyByInflectionPoint (float[] value, float range)
	{
		int vmax = value.Length; // yamaha
		List<InflectionPoint> lst = new List<InflectionPoint> ();
		InflectionPoint ip = new InflectionPoint ();

		int vmax_1 = vmax - 1;
		ip.value = value [1] - value [0];
		for (int i = 1; i < vmax_1; i++) {
			float diff = value [i + 1] - value [i];
			if (diff * ip.value < 0.0f || (ip.value == 0.0f && diff != 0.0f) || (ip.value != 0.0f && diff == 0.0f)) {
				lst.Add (new InflectionPoint (i, fabs (ip.value)));
				ip.value = diff;
			} else {
				ip.value += diff;
			}
		}
/*		Debug.Log ("lst:" + lst.Count + ":vmax:" + vmax);
		for (int i = 0; i < lst.Count; i++) {
			Debug.Log ("[" + i + "]" + lst [i].value);
		}*/

		lst.Sort ((a, b) => (int)(b.value - a.value));

		segment.Add (new Segment (0, value [0], 0.0f, value [1] - value [0]));
		segment.Add (new Segment (vmax_1, value [vmax_1], value [vmax_1] - value [vmax_1 - 1], 0.0f));

		int cnt = 0;
		int lcnt = lst.Count;
		while (SetErrorSegment (value, range)) {
      if (lst.Count > cnt)
      {
        InsertKey(value, lst[cnt].frame);
      }
/*			for (int i = 0; i < segment.Count; i++) {
				Debug.Log ("[" + i + "]:" + segment [i].frame + ":" + segment [i].value + ":" + segment [i].error);
			}*/
			if (lcnt <= ++cnt) {
				while (SetErrorSegment (value, range)) {
					SetSlopeKey (value);
				}
				break;
			}
		}
		FunctionCurve fc = new FunctionCurve ();
		lcnt = segment.Count;
//		Debug.Log ("lcnt:" + lcnt);
//		string str = ("lcnt:" + lcnt);
		fc.functionCurveKey = new FunctionCurve.FunctionCurveKey[lcnt];
		for (int i = 0; i < lcnt; i++) {
			fc.functionCurveKey [i] = new FunctionCurve.FunctionCurveKey (segment [i]);
//			Debug.Log ("[" + i + "]" + segment [i].frame + ":" + segment [i].value + ":" + segment [i].slope_l + ":" + segment [i].slope_r);
//			str += ("[" + i + "]" + Segment [i].frame + ":" + Segment [i].value + ":" + Segment [i].slope_l + ":" + Segment [i].slope_r);
		}
//		Debug.Log (str);
		return fc;
	}

	public static FunctionCurve BakedDataToFunctionCurve (float[] value, float range = 0.01f, float one_frame_time = 1.0f / 60.0f)
	{
		FunctionCurveCompress fcc = new FunctionCurveCompress ();
		return fcc.SetKeyByInflectionPoint (value, range);
	}
}
#endif
