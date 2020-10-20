using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MsgPack;
using MsgPack.Serialization;
using GFMsgPack;

namespace PokemonDataPackSample
{
  /// <summary>
  /// 色情報を表すクラス(RGBA. 各要素float)
  /// </summary>
  public class Color
  {
    public float m_Red;
    public float m_Green;
    public float m_Blue;
    public float m_Alpha;

    public Color()
    {
      m_Red = 0.0f;
      m_Green = 0.0f;
      m_Blue = 0.0f;
      m_Alpha = 0.0f;
    }

    public Color(float r, float g, float b, float a)
    {
      m_Red = r;
      m_Green = g;
      m_Blue = b;
      m_Alpha = a;
    }
  }

  /// <summary>
  /// レアカラーのコンスタントカラーオフセット情報
  /// </summary>
  public class RareColorConstantOffset
  {
    private static readonly uint NumberOfMaterials = 16;
    private static readonly uint NumberOfConstantColors = 6;
    private static readonly uint NumberOfConstantColorOffset = NumberOfMaterials * NumberOfConstantColors;
    public uint m_DevelopmentId;
    public uint m_FormId;
    public string m_DevelopmentName;
    public Color[] m_ConstantColorOffset;

    public RareColorConstantOffset()
    {
      m_DevelopmentId = 0;
      m_FormId = 0;
      m_DevelopmentName = "";
      m_ConstantColorOffset = new Color[NumberOfConstantColorOffset];
      m_ConstantColorOffset = m_ConstantColorOffset.Select(offset => offset = new Color()).ToArray();
    }
  }

  class Program
  {
    static int Main(string[] args)
    {
      // レアカラーのオフセットオブジェクトを作成
      var rawData = new RareColorConstantOffset();
      rawData.m_DevelopmentId = 1;
      rawData.m_FormId = 0;
      rawData.m_DevelopmentName = "fushigidane";
      rawData.m_ConstantColorOffset[0] = new Color(0.0f, 0.25f, 0.5f, 1.0f);

      // 上記クラスをシリアライズ
      var serializedData = GFMsgPackFunction.ConvertObjectToMessagePack<RareColorConstantOffset>(rawData);

      // デシリアライズして元に戻す
      var deserializedData = GFMsgPackFunction.ConvertMessagePackToObject<RareColorConstantOffset>(serializedData);
      Console.WriteLine(deserializedData.m_DevelopmentId);
      Console.WriteLine(deserializedData.m_FormId);
      Console.WriteLine(deserializedData.m_DevelopmentName);
      
      // デシリアライズ結果を出力
      foreach (var offset in deserializedData.m_ConstantColorOffset)
      {
        Console.WriteLine("R = " + offset.m_Red);
        Console.WriteLine("G = " + offset.m_Green);
        Console.WriteLine("B = " + offset.m_Blue);
        Console.WriteLine("A = " + offset.m_Alpha);
      }

      // Dictionary<int, string>をGFMsgPackFunction.Convert...()を使用してシリアライズ, デシリアライズ
      var dictionary = new Dictionary<int, string>();
      dictionary.Add(1, "fushigidane");
      dictionary.Add(2, "fushigisou");
      var serializedDictionary = GFMsgPackFunction.ConvertObjectToMessagePack<Dictionary<int, string>>(dictionary);
      var deserializedDictionary = GFMsgPackFunction.ConvertMessagePackToObject<Dictionary<int, string>>(serializedDictionary);
      // デシリアライズ結果を出力
      foreach (var pair in dictionary)
      {
        Console.WriteLine("key = " + pair.Key + ", value = " + pair.Value);
      }

      Console.ReadLine();

      return 0;
    }
  }
}
