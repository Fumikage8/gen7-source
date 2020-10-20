
namespace SymbolResolver
{
    /// <summary>
    /// 「文字列 -> バイト列」に変換するデコーダのインターフェース
    /// </summary>
    public interface IBinaryDecoder
    {
        /// <summary>
        /// 文字列をバイト列にデコードする
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        /// <exception cref="System.ArgumentException"></exception>
        byte[] Decode(string str);
    }
}
