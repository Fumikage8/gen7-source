using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MsgPack;
using MsgPack.Serialization;

namespace ObjectPackerSample
{
  public class SampleData
  {
    public uint id;
    public string name;
  }

  //!< @todo : GFMsgPack.dllを作成してそちらに移行
  public static class GFMsgPack
  {
    /// <summary>
    /// object -> MessagePack(byte[])
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="obj"></param>
    /// <returns>byte[] コンバート後のMsgPack形式のバイト配列</returns>
    public static byte[] ConvertObjectToMessagePack<T>(this object obj)
    {
      var serializer = MessagePackSerializer.Get<T>();
      using (var memoryStream = new MemoryStream())
      {
        serializer.Pack(memoryStream, (T)obj);
        var ret = memoryStream.ToArray();
        return ret;
      }
    }

    /// <summary>
    /// MessagePack(byte[]) -> object
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="bytes"></param>
    /// <returns>T MsgPackでデシリアライズ後のオブジェクト</returns>
    public static T ConvertMessagePackToObject<T>(this byte[] bytes)
    {
      var serialize = MessagePackSerializer.Get<T>();
      using (var memoryStream = new MemoryStream(bytes))
      {
        var ret = serialize.Unpack(memoryStream);
        return ret;
      }
    }
  }

  class Program
  {
    static int Main(string[] args)
    {
      var rawData = new SampleData
      {
        id = 1,
        name = "fushigidane"
      };

      // 上記クラスをシリアライズ
      var serializedData = GFMsgPack.ConvertObjectToMessagePack<SampleData>(rawData);

      // デシリアライズして元に戻す
      var deserializedData = GFMsgPack.ConvertMessagePackToObject<SampleData>(serializedData);

      return 0;
    }
  }
}
