//=================================================================================================
/**
 * @file app_tool_Gauge.h
 * @brief ゲージ関連処理
 * @author obat_toshihiro
 * @date 2011.07.05
 */
//=================================================================================================
#ifndef __APPTOOLGAUGE_H__
#define __APPTOOLGAUGE_H__ 

#include <types/include/gfl2_Typedef.h>

namespace app {
  namespace tool {


    class Gauge 
    { 
      public:

        // ゲージカラー
        enum GaugeColor {
          GAUGE_COLOR_GREEN,   // 緑
          GAUGE_COLOR_YELLOW,  // 黄
          GAUGE_COLOR_RED,     // 赤
          GAUGE_COLOR_NULL,    // HP0
        }; 

        //-----------------------------------------------------------------------------------------
        /**
         * @brief	現在値のゲージドット数を取得
         * @param	now_value	現在値
         * @param	max_value	最大値
         * @param	max_dot	最大ドット数
         * @return ドット数
         */
        //-----------------------------------------------------------------------------------------
        static u32 GetDottoNum( u32 now_value, u32 max_value, u32 max_dot );
        static f32 GetDottoNumF32( f32 now_value, u32 max_value, u32 max_dot );

        //-----------------------------------------------------------------------------------------
        /**
         * @brief HPゲージのカラーを取得
         * @param now_value	現在の値
         * @param max_value	最大値
         * @return ゲージカラー
         */
        //-----------------------------------------------------------------------------------------
        static GaugeColor GetDottoColor( u32 now_value, u32 max_value );
    };


  } // namespace tool
} // namespace app


#endif // __APPTOOLGAUGE_H__
