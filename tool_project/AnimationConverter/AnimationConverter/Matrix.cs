using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AnimationConverter
{
  class Matrix
  {
    static public Matrix GetRotationZYX(float radiansZ, float radiansY, float radiansX)
    {
      float xsin = (float)Math.Sin(radiansX);
      float ysin = (float)Math.Sin(radiansY);
      float zsin = (float)Math.Sin(radiansZ);
      float xcos = (float)Math.Cos(radiansX);
      float ycos = (float)Math.Cos(radiansY);
      float zcos = (float)Math.Cos(radiansZ);

      return new Matrix
        (
        (ycos * zcos), (ycos * zsin), (-ysin), 0.0f,
        (xsin * ysin * zcos + xcos * -zsin), (xsin * ysin * zsin + xcos * zcos), (xsin * ycos), 0.0f,
        (xcos * ysin * zcos + -xsin * -zsin), (xcos * ysin * zsin + -xsin * zcos), (xcos * ycos), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    public Matrix()
      : this(
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1)
    {
    }

    public Matrix(
     float m00, float m01, float m02, float m03,
     float m10, float m11, float m12, float m13,
     float m20, float m21, float m22, float m23,
     float m30, float m31, float m32, float m33)
    {
      this.m00 = m00;
      this.m01 = m01;
      this.m02 = m02;
      this.m03 = m03;

      this.m10 = m10;
      this.m11 = m11;
      this.m12 = m12;
      this.m13 = m13;

      this.m20 = m20;
      this.m21 = m21;
      this.m22 = m22;
      this.m23 = m23;

      this.m30 = m30;
      this.m31 = m31;
      this.m32 = m32;
      this.m33 = m33;
    }

    public static bool Eqaul3x3(Matrix a, Matrix b, float errorValue)
    {
      var dotX = Matrix.DotX(a, b);
      var dotY = Matrix.DotY(a, b);
      var dotZ = Matrix.DotZ(a, b);
      if (dotX < errorValue)
      {
        return false;
      }
      if (dotY < errorValue)
      {
        return false;
      }
      if (dotZ < errorValue)
      {
        return false;
      }
      return true;
    }

    public static float DotX(Matrix a, Matrix b)
    {
      return (a.m00 * b.m00) + (a.m01 * b.m01) + (a.m02 * b.m02);
    }
    public static float DotY(Matrix a, Matrix b)
    {
      return (a.m10 * b.m10) + (a.m11 * b.m11) + (a.m12 * b.m12);
    }
    public static float DotZ(Matrix a, Matrix b)
    {
      return (a.m20 * b.m20) + (a.m21 * b.m21) + (a.m22 * b.m22);
    }

    public static bool IsNearly(float a, float b)
    {
      var sub = a - b;
      var subAbs = Math.Abs(sub);

      var error = 0.1f;
      var result = (subAbs < error);

      return result;
    }

    public float m00, m01, m02, m03;
    public float m10, m11, m12, m13;
    public float m20, m21, m22, m23;
    public float m30, m31, m32, m33;
  }
}
