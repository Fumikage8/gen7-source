using System;

namespace SymbolResolver
{
    /// <summary>
    /// 『1バイトごとに「'\' + 10進数3桁」で記述された文字列』をバイト列配列にデコードするデコーダ
    ///
    /// 例
    /// "\001\201" = bytes[]{1, 201}
    /// </summary>
    public class OctDecoder : IBinaryDecoder
    {
        public byte[] Decode(string str)
        {
            if (str.Length % 4 != 0)
            {
                throw new ArgumentException("引数["+str+"]は不正です。(長さが4の倍数である必要があります)");
            }

            var res = new byte[str.Length / 4];

            int idx = 0;
            foreach (var oct in str.Split('\\'))
            {
                if (oct.Length == 0)
                {
                    continue;
                }

                res[idx] = byte.Parse(oct);

                ++idx;
            }

            return res;
        }
    }
}
