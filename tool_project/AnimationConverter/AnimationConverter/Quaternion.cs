using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AnimationConverter
{
  class Quaternion
  {
    public Quaternion()
      : this(0, 0, 0, 1)
    {
    }
    public Quaternion(float x, float y, float z, float w)
    {
      this.x = x;
      this.y = y;
      this.z = z;
      this.w = w;
    }

    public void FromMatrix(Matrix mat)
    {
      float tr = mat.m00 + mat.m11 + mat.m22;
      float ww, max;
      int index;

      if( tr >= 0.0f ){
        ww = (float)Math.Sqrt( tr + 1.0f );
        this.w = ( ww * 0.5f );
        ww = -0.5f / ww;

        this.x = ( ( mat.m21 - mat.m12 ) * ww );
        this.y = ( ( mat.m02 - mat.m20 ) * ww );
        this.z = ( ( mat.m10 - mat.m01 ) * ww );
        return;
      }

      index = 0;
      max = mat.m00;
      if( mat.m11 > max ){
        index = 1;
        max = mat.m11;
      }
      if( mat.m22 > max ){
        index = 2;
      }

      switch( index ){
      case 0:
        ww = (float)Math.Sqrt( mat.m00 - mat.m11 - mat.m22 + 1.0f );
        this.x = ( ww * 0.5f );
        ww = 0.5f / ww;
        this.y = ( ( mat.m01 + mat.m10 ) * ww );
        this.z = ( ( mat.m20 + mat.m02 ) * ww );
        this.w = ( ( mat.m12 - mat.m21 ) * ww );
        break;
      case 1:
        ww = (float)Math.Sqrt( mat.m11 - mat.m00 - mat.m22 + 1.0f );
        this.y = ( ww * 0.5f );
        ww = 0.5f / ww;
        this.z = ( ( mat.m12 + mat.m21 ) * ww );
        this.x = ( ( mat.m01 + mat.m10 ) * ww );
        this.w = ( ( mat.m20 - mat.m02 ) * ww );
        break;
      case 2:
        ww = (float)Math.Sqrt( mat.m22 - mat.m00 - mat.m11 + 1.0f );
        this.z = ( ww * 0.5f );
        ww = 0.5f / ww;
        this.x = ( ( mat.m20 + mat.m02 ) * ww );
        this.y = ( ( mat.m12 + mat.m21 ) * ww );
        this.w = ( ( mat.m01 - mat.m10 ) * ww );
        break;
      }

    }

    public Quaternion Clone()
    {
      return (Quaternion)MemberwiseClone();
    }

    public Quaternion Conj()
    {
      var conj = this.Clone();
      conj.x = -conj.x;
      conj.y = -conj.y;
      conj.z = -conj.z;
      conj.w = -conj.w;

      return conj;
    }

    // 対数クォータニオンに変換
    public bool ConvLn()
    {
      var theta = Math.Acos(this.w);
      var sinTheta = Math.Sin(theta);
      var s = (float)(theta / sinTheta);
      if (float.IsNaN(s))
      {
        s = 0.0f;
      }

#if false
      var oneMinusEpsilon = 1.0f - 0.00001f;
      var isBounds = Bounds(this.w, oneMinusEpsilon);

      if (isBounds)
      {
        this.x *= s;
        this.y *= s;
        this.z *= s;
      }
      this.w = 0;
      return isBounds;
#else
      var isIdentity = (Math.Abs(this.w) == 1);
      var oldQ = this.Clone();

      this.x *= s;
      this.y *= s;
      this.z *= s;
      this.w = 0;

      var newQ = this.Clone();
      newQ.ConvExp();

      if (!Quaternion.Eqaul(oldQ, newQ))
      {
        // ほぼ単位クォータニオン
        this.x = oldQ.x;
        this.y = oldQ.y;
        this.z = oldQ.z;
        this.w = 0;

        return false;
      }
      if (isIdentity)
      {
        return false;
      }

      return true;
#endif
    }

    // 対数クォータニオンから復元
    public void ConvExp()
    {
      var len = (this.x * this.x) + (this.y * this.y) + (this.z * this.z);

      // 単位クォータニオンの判定
      if (len == 0.0f)
      {
        this.x = 0.0f;
        this.y = 0.0f;
        this.z = 0.0f;
        this.w = 1.0f;
        return;
      }

      // それ以外
      var theta = Math.Sqrt(len);
      var sinTheta = Math.Sin(theta);
      var s = (float)(sinTheta / theta);

      this.x *= s;
      this.y *= s;
      this.z *= s;
      this.w = (float)Math.Cos(theta);
    }

#if true
    // 行列へ変換
    public Matrix ConvToMatrix()
    {
      var mat = new Matrix();

      var xx = x * x;
      var yy = y * y;
      var zz = z * z;
      var xy = x * y;
      var xz = x * z;
      var yz = y * z;
      var wx = w * x;
      var wy = w * y;
      var wz = w * z;

      mat.m00 = 1.0f - 2.0f * ( yy + zz );
      mat.m10 = 2.0f * ( xy - wz );
      mat.m20 = 2.0f * ( xz + wy );
      mat.m30 = 0.0f;

      mat.m01 = 2.0f * ( xy + wz );
      mat.m11 = 1.0f - 2.0f * ( xx + zz );
      mat.m21 = 2.0f * ( yz - wx );
      mat.m31 = 0.0f;

      mat.m02 = 2.0f * ( xz - wy );
      mat.m12 = 2.0f * ( yz + wx );
      mat.m22 = 1.0f - 2.0f * ( xx + yy );
      mat.m32 = 0.0f;

      mat.m03 = 0.0f;
      mat.m13 = 0.0f;
      mat.m23 = 0.0f;
      mat.m33 = 1.0f;

      return mat;
    }
#else
    // 行列へ変換
    public Matrix ConvToMatrix()
    {
      var q0 = Quaternion.Add(this, this);
      var q1 = Quaternion.MulVec(this, q0);

      var v0 = Quaternion.Shuffle(q1, new int[] { 3, 0, 0, 1 });
      v0.w = 0;
      var v1 = Quaternion.Shuffle(q1, new int[] { 3, 1, 2, 2 });
      v1.w = 0;
      var r0 = Quaternion.Sub(new Quaternion(1, 1, 1, 0), v0);
      r0 = Quaternion.Sub(r0, v1);

      v0 = Quaternion.Shuffle(this, new int[] { 3, 1, 0, 0 });
      v1 = Quaternion.Shuffle(q0, new int[] { 3, 2, 1, 2 });
      v0 = Quaternion.MulVec(v0, v1);

      v1 = Quaternion.Shuffle(this, new int[] { 3, 3, 3, 3 });
      var v2 = Quaternion.Shuffle(q0, new int[] { 3, 0, 2, 1 });
      v1 = Quaternion.MulVec(v1, v2);

      var r1 = Quaternion.Add(v0, v1);
      var r2 = Quaternion.Sub(v0, v1);

      v0 = Quaternion.Shuffle(r1, r2, new int[] { 1, 0, 2, 1 });
      v0 = Quaternion.Shuffle(v0, new int[] { 1, 3, 2, 0 });
      v1 = Quaternion.Shuffle(r1, r2, new int[] { 2, 2, 0, 0 });
      v1 = Quaternion.Shuffle(v1, new int[] { 2, 0, 2, 0 });

      q1 = Quaternion.Shuffle(r0, v0, new int[] { 1, 0, 3, 0 });
      q1 = Quaternion.Shuffle(q1, new int[] { 1, 3, 2, 0 });

      var mat = new Matrix();

      mat.m00 = q1.x;
      mat.m01 = q1.y;
      mat.m02 = q1.z;
      mat.m03 = q1.w;

      q1 = Quaternion.Shuffle(r0, v0, new int[] { 3, 2, 3, 1 });
      q1 = Quaternion.Shuffle(q1, new int[] { 1, 3, 0, 2 });

      mat.m10 = q1.x;
      mat.m11 = q1.y;
      mat.m12 = q1.z;
      mat.m13 = q1.w;

      q1 = Quaternion.Shuffle(v1, r0, new int[] { 3, 2, 1, 0 });
      mat.m20 = q1.x;
      mat.m21 = q1.y;
      mat.m22 = q1.z;
      mat.m23 = q1.w;

      mat.m03 = 0.0f;
      mat.m13 = 0.0f;
      mat.m23 = 0.0f;
      mat.m33 = 1.0f;

      return mat;
    }
#endif

    public float LengthSq()
    {
      return Dot(this, this);
    }

    // 内積
    public static float Dot(Quaternion a, Quaternion b)
    {
      return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    }

    public static bool Bounds(float value, float bounds)
    {
      return (value <= bounds) && (value >= -bounds);
    }

    public static bool Eqaul(Quaternion a, Quaternion b)
    {
      var error = 0.00001f;
      var oneMinusError = 1.0f - error;

      if ((Math.Abs(a.w) > oneMinusError) && (Math.Abs(b.w) > oneMinusError))
      {
        // ほぼ単位クォータニオン
        return true;
      }

      if (Math.Abs(a.x - b.x) > error)
      {
        return false;
      }
      if (Math.Abs(a.y - b.y) > error)
      {
        return false;
      }
      if (Math.Abs(a.z - b.z) > error)
      {
        return false;
      }
      if (Math.Abs(a.w - b.w) > error)
      {
        return false;
      }
      return true;
    }

    public static Quaternion Slerp(Quaternion t, Quaternion s, float rate)
    {
      var result = t.Clone();

      var dot = Quaternion.Dot(t, s);
      var oneMinusEpsilon = 1.0f - 0.00001f;

      if (dot < oneMinusEpsilon)
      {
        var ang = Math.Acos(dot);
        var sin_recip = 1.0 / Math.Sin(ang);
        var t_rate = Math.Sin((1.0f - rate) * ang) * sin_recip;
        var s_rate = Math.Sin(rate * ang) * sin_recip;

        result.x = (float)(t.x * t_rate + s.x * s_rate);
        result.y = (float)(t.y * t_rate + s.y * s_rate);
        result.z = (float)(t.z * t_rate + s.z * s_rate);
        result.w = (float)(t.w * t_rate + s.w * s_rate);
      }

      return result;
    }

    public static Quaternion Lerp(Quaternion t, Quaternion s, float rate)
    {
      var result = new Quaternion();

      var t_rate = 1.0f - rate;
      var s_rate = rate;

      result.x = (float)(t.x * t_rate + s.x * s_rate);
      result.y = (float)(t.y * t_rate + s.y * s_rate);
      result.z = (float)(t.z * t_rate + s.z * s_rate);
      result.w = (float)(t.w * t_rate + s.w * s_rate);

      return result;
    }

    public static Quaternion Add(Quaternion a, Quaternion b)
    {
      return new Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
    }

    public static Quaternion Sub(Quaternion a, Quaternion b)
    {
      return new Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
    }

    public static Quaternion MulVec(Quaternion a, Quaternion b)
    {
      return new Quaternion(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
    }

    public static Quaternion Shuffle(Quaternion q, int[] shuffle)
    {
      var q_array = new float[]
      {
        q.x,
        q.y,
        q.z,
        q.w,
      };

      return new Quaternion(q_array[shuffle[3]], q_array[shuffle[2]], q_array[shuffle[1]], q_array[shuffle[0]]);
    }

    public static Quaternion Shuffle(Quaternion a, Quaternion b, int[] shuffle)
    {
      var a_array = new float[]
      {
        a.x,
        a.y,
        a.z,
        a.w,
      };
      var b_array = new float[]
      {
        b.x,
        b.y,
        b.z,
        b.w,
      };

      return new Quaternion(a_array[shuffle[3]], a_array[shuffle[2]], b_array[shuffle[1]], b_array[shuffle[0]]);
    }

    public float x, y, z, w;
  }
}
