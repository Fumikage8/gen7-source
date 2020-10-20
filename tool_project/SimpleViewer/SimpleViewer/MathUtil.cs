using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleViewer
{
  public class MathUtil
  {
    /**
     * @brief 小数点以下を切り捨てる関数
     * @param value 切り捨てる値
     * @param decimals 小数点第何位で切り捨てるか
     */
    public static float Truncate(float value, int decimals)
    {
      // 小数部桁数の10の累乗を取得
      float pow = (float)Math.Pow(10, decimals);
      return 0 <= value ? (float)Math.Floor(value * pow) / pow :
          (float)Math.Ceiling(value * pow) / pow;
    }
  }
}
