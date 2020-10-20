using System;
using System.IO;
using MsgPack.Serialization;

namespace GFMsgPack
{
  /// <summary>
  /// MessagePackを利用しやすくするための関数群クラス(MsgPack.dllに依存)
  /// </summary>
  public static class GFMsgPackFunction
  {
    /// <summary>
    /// object -> MessagePack(byte[]) の変換
    /// </summary>
    /// <typeparam name="T">変換対象のオブジェクトの型</typeparam>
    /// <param name="obj">変換対象のオブジェクト</param>
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
    /// MessagePack(byte[]) -> object の変換
    /// </summary>
    /// <typeparam name="T">変換対象のオブジェクトの型</typeparam>
    /// <param name="bytes">変換後のバイナリデータ</param>
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
}
