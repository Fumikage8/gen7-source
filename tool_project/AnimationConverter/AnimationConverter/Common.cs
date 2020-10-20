using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;
using System.Xml;
using System.Xml.Serialization;

namespace AnimationConverter
{
	class Common
	{
    // 4バイトのハッシュ値作成
    public static int CreateHash(string buf)
    {
      var md5 = System.Security.Cryptography.MD5.Create();
      var hashBuf = md5.ComputeHash(Encoding.Unicode.GetBytes(buf));

      return (hashBuf[0] << 24) | (hashBuf[1] << 16) | (hashBuf[2] << 8) | hashBuf[3];
    }

    // XMLデシリアライズ
    public static Type DeserializeXml<Type>(string filepath) where Type : class
    {
      Type result = null;
      try
      {
        var text = File.ReadAllText(filepath);
        text = System.Text.RegularExpressions.Regex.Replace(text, "Name=\"(.*?)\"", (match) => {
          var name = match.Groups[1].Value;
          var refIndex = name.LastIndexOf(':');
          if (refIndex < 0)
          {
            // そのまま返す
            return match.Value;
          }

          var newName = "Name=\"" + name.Substring(refIndex + 1) + "\"";
          return newName;
        });

        using (var ms = new MemoryStream(Encoding.UTF8.GetBytes(text)))
        {
          var xmlSerializer = new XmlSerializer(typeof(Type));
          result = (Type)xmlSerializer.Deserialize(ms);
        }
      }
      catch
      {
        // 失敗するかもしれない
      }

      return result;
    }

    // XMLシリアライズ
    public static bool SerializeXml<Type>(string outputPath, Type obj) where Type : class
    {
      try
      {
        using (var fs = File.OpenWrite(outputPath))
        {
          fs.SetLength(0);
          var serializer = new XmlSerializer(typeof(Type));
          serializer.Serialize(fs, obj);
        }
      }
      catch 
      {
        // 失敗するかもしれない
        return false;
      }

      return true;
    }

    // エルミート補間
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

    // キーの復元
    public static float[] DecompGfmotKeys(Gfmot.Common_Keys keys, int frameCount, float basePose)
    {
      var result = new float[frameCount + 1];

      if (keys == null)
      {
        // ベースポーズ
        for (var i = 0; i <= frameCount; ++i)
        {
          result[i] = basePose;
        }
      }
      else if (keys.HermiteKeys == null)
      {
        if (keys.OneFrameKey == null)
        {
          // ベースポーズ
          for (var i = 0; i <= frameCount; ++i)
          {
            result[i] = basePose;
          }
        }
        else
        {
          // 同じ値
          for (var i = 0; i <= frameCount; ++i)
          {
            result[i] = keys.OneFrameKey.Value;
          }
        }
      }
      else
      {
        // エルミートを展開
        var hermiteKeys = keys.HermiteKeys;

        var indexCurrent = 0;
        var keyCurrent = hermiteKeys[indexCurrent];
        var keyNext = hermiteKeys[indexCurrent + 1];
        for (var i = 0; i <= frameCount; ++i)
        {
          if (i > keyNext.Frame)
          {
            keyCurrent = keyNext;
            ++indexCurrent;
            if (hermiteKeys.Length <= indexCurrent + 1)
            {
              result[i] = keyCurrent.Value;
              continue;
            }
            keyNext = hermiteKeys[indexCurrent + 1];
          }

          var p = i - keyCurrent.Frame;
          var d = keyNext.Frame - keyCurrent.Frame;
          var value = Hermite(keyCurrent.Value, keyCurrent.Slope, keyNext.Value, keyNext.Slope, p, d);
          result[i] = (float)value;
        }
      }

      return result;
    }
    public static float DecompGfmotKey(Gfmot.Common_Keys keys, float frame, float basePose)
    {
      var result = 0.0f;

      if (keys == null)
      {
        // ベースポーズ
        result = basePose;
      }
      else if (keys.HermiteKeys == null)
      {
        if (keys.OneFrameKey == null)
        {
          // ベースポーズ
          result = basePose;
        }
        else
        {
          // 同じ値
          result = keys.OneFrameKey.Value;
        }
      }
      else
      {
        // エルミートを展開
        var hermiteKeys = keys.HermiteKeys;

        var indexCurrent = 0;
        var keyCurrent = hermiteKeys[indexCurrent];
        var keyNext = hermiteKeys[indexCurrent + 1];
        for (var i = 0; i <= frame; ++i)
        {
          if (i > keyNext.Frame)
          {
            keyCurrent = keyNext;
            ++indexCurrent;
            keyNext = hermiteKeys[indexCurrent + 1];
          }
        }

        var p = frame - keyCurrent.Frame;
        var d = keyNext.Frame - keyCurrent.Frame;
        var value = Hermite(keyCurrent.Value, keyCurrent.Slope, keyNext.Value, keyNext.Slope, p, d);
        result = (float)value;
      }

      return result;
    }
    public static float[] DecompCsklaKeys(Cskla.Segments segments, int frameCount, float basePose)
    {
      var result = new float[frameCount + 1];

      if (segments == null)
      {
        // ベースポーズ
        for (var i = 0; i <= frameCount; ++i)
        {
          result[i] = basePose;
        }
      }
      else if (segments.HermiteFloatSegment == null)
      {
        if (segments.LinearFloatSegment == null)
        {
          // ベースポーズ
          for (var i = 0; i <= frameCount; ++i)
          {
            result[i] = basePose;
          }
        }
        else
        {
          // 同じ値
          for (var i = 0; i <= frameCount; ++i)
          {
            result[i] = segments.LinearFloatSegment.Keys[0].Value;
          }
        }
      }
      else
      {
        // エルミートを展開
        var hermiteKeys = segments.HermiteFloatSegment.Keys;

        var indexCurrent = 0;
        var keyCurrent = hermiteKeys[indexCurrent];
        var keyNext = hermiteKeys[indexCurrent + 1];
        for (var i = 0; i <= frameCount; ++i)
        {
          if (i > keyNext.Frame)
          {
            keyCurrent = keyNext;
            ++indexCurrent;
            keyNext = hermiteKeys[indexCurrent + 1];
          }

          var p = i - keyCurrent.Frame;
          var d = keyNext.Frame - keyCurrent.Frame;
          var value = Hermite(keyCurrent.Value, keyCurrent.InSlope, keyNext.Value, keyNext.InSlope, p, d);
          result[i] = (float)value;
        }
      }

      return result;
    }

		// 4バイトアラインメントにそろえる
		public static void Align4Byte(BinaryWriter bw)
		{
			var pos = bw.BaseStream.Position;
			var remain = pos % 4;

			if (remain == 0)
			{
				return;
			}

			var pad = 4 - remain;
			for (var i = 0; i < pad; ++i)
			{
				bw.Write(byte.MaxValue);
			}
		}
    // 4バイトアラインメインのサイズ取得
    public static int GetAlign4ByteSize(int originalSize)
    {
			var remain = originalSize % 4;

			if (remain == 0)
			{
				return originalSize;
			}

			var pad = 4 - remain;
      return originalSize + pad;
    }

    public static char[] GetNameBuffer(string name, int length)
    {
      if (length <= 0)
      {
        return null;
      }

      var buf = new char[length];
      for (var i = 0; i < length; ++i)
      {
        buf[i] = '\0';
      }

      if (name == null)
      {
        return buf;
      }

      for (var i = 0; i < name.Length; ++i)
      {
        buf[i] = name[i];
      }

      return buf;
    }

    public static readonly char[] SplitChar =
    {
      ' '
    };

    public static float[] GetFloatArray(string text)
    {
      var textList = text.Split(SplitChar, StringSplitOptions.RemoveEmptyEntries);

      var values = new float[textList.Length];
      for (var i = 0; i < textList.Length; ++i)
      {
        values[i] = float.Parse(textList[i]);
      }

      return values;
    }

    public static bool IsNullOrEmpty(Array array)
    {
      if (array == null)
      {
        return true;
      }

      if (array.Length == 0)
      {
        return true;
      }

      return false;
    }

    public static bool IsFloatEqual(float a, float b)
    {
      return Math.Abs(a - b) < 0.000001f;
    }

    public static float Lerp(float a, float b, float rate)
    {
      var a_rate = 1.0f - rate;
      var b_rate = rate;

      return a * a_rate + b * b_rate;
    }
	}

	class OffsetListMaker
	{
		public OffsetListMaker(BinaryWriter bw, int count)
		{
			this.bw = bw;
			this.count = count;
			this.offsetList = new long[count];

			this.beginPosition = bw.BaseStream.Position;

			// 最初にダミーのリストを書き出し
			this.WriteList();
		}

		public void Mark()
		{
			this.offsetList[this.index] = this.bw.BaseStream.Position;
			++this.index;
		}

		public void Export()
		{
			// 位置を戻す
			var endPosition = this.bw.BaseStream.Position;
			this.bw.BaseStream.Position = this.beginPosition;

			// オフセットリスト書き出し
			this.WriteList();

			this.bw.BaseStream.Position = endPosition;
		}

		private void WriteList()
		{
			foreach (var offset in this.offsetList)
			{
				this.bw.Write((uint)offset);
			}
		}
		

		private BinaryWriter bw = null;
		private int count = 0;

		private long beginPosition = 0;
		private long[] offsetList = null;
		private int index = 0;
	}
}
