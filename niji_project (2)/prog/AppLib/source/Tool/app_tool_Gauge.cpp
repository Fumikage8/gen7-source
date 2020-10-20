//=================================================================================================
/**
 * @file app_tool_Gauge.cpp
 * @brief ゲージ関連処理
 * @author obat_toshihiro
 * @date 2011.07.05
 */
//=================================================================================================
#include "AppLib/include/Tool/app_tool_Gauge.h"

namespace app {
  namespace tool {


    //-----------------------------------------------------------------------------------------
    /**
     * @brief	現在値のゲージドット数を取得
     * @param	now_value	現在値
     * @param	max_value	最大値
     * @param	max_dot	最大ドット数
     * @return ドット数
     */
    //-----------------------------------------------------------------------------------------
    u32 Gauge::GetDottoNum( u32 now_value, u32 max_value, u32 max_dot )
    {
      f32 dot_f = 0.0f;
      if( ( now_value == 0 ) && ( max_value == 0 ) )
      {
        //  0除算を防ぐために０の場合は問答無用で最大に
        return  max_dot;
      }
      else
      {
        dot_f = static_cast<f32>(now_value*max_dot) / static_cast<f32>(max_value);
      }

      u32  dot_i = static_cast<u32>( dot_f );
      if( ( dot_f - static_cast<f32>(dot_i) ) != 0.0f ){
        dot_i += 1;
      }
      // 最大値以外は最大ドットにしない
      if( dot_i == max_dot && now_value != max_value ){
        dot_i -= 1;
      }
      return dot_i;
    }
    f32 Gauge::GetDottoNumF32( f32 now_value, u32 max_value, u32 max_dot )
    {
      f32 dot_f = 0.0f;
      if( ( now_value == 0 ) && ( max_value == 0 ) )
      {
        //  0除算を防ぐために０の場合は問答無用で最大に
        return  max_dot;
      }
      else
      {
        dot_f = (now_value*max_dot) / static_cast<f32>(max_value);
      }
      //HPが0ではない場合は最低1ドット残す
      if( now_value > 0.0f && dot_f < 1.0f )
      {
        dot_f = 1.0f;
      }

      // 最大値以外は最大ドットにしない
      if( dot_f >= max_dot && now_value < max_value ){
        dot_f -= 1.0f;
      }
      return dot_f;
    }

    //-----------------------------------------------------------------------------------------
    /**
     * @brief HPゲージのカラーを取得
     * @param now_value	現在の値
     * @param max_value	最大値
     * @return ゲージカラー
     */
    //-----------------------------------------------------------------------------------------
    Gauge::GaugeColor Gauge::GetDottoColor( u32 now_value, u32 max_value )
    {
      now_value <<= 8;		//割り算使用の為、小数レベルまで見れるように固定小数化
      max_value <<= 8;

      if( now_value > (max_value/2) )
      {
        return GAUGE_COLOR_GREEN;	// 緑
      }
      else if( now_value > (max_value/5) )
      {
        return GAUGE_COLOR_YELLOW;	// 黄
      }
      else if( now_value > 0 )
      {
        return GAUGE_COLOR_RED;		// 赤
      }
      return GAUGE_COLOR_NULL;			// HP=0
    }


  } // namespace tool
} // namespace app
