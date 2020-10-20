//=============================================================================
/**
 * @file app_tool_Interpolation.h
 * @brief エルミート補間計算クラス
 * @author obata
 * @date 2011.10.19
 */
//=============================================================================
#ifndef __APP_TOOL_INTERPOLATION_H__
#define __APP_TOOL_INTERPOLATION_H__

#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>

namespace app {
  namespace tool {


    class Interpolation
    {
      public:

        /**
         * @brief 制御点の最大数
         */
        static const u8 MAX_CONTROL_POINT_NUM = 5;


        //-----------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //-----------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        Interpolation( void );

        /**
         * @brief デストラクタ
         */
        virtual ~Interpolation( void );


        //-----------------------------------------------------------------------
        // 基本動作
        //-----------------------------------------------------------------------
        /**
         * @brief 制御点を追加する
         * @param frame  時間
         * @param pos    位置
         * @param vel    速度
         */
        void AddControlPoints( 
            u32 frame, const gfl2::math::VEC3& pos, const gfl2::math::VEC3& vel );

        /**
         * @brief 指定した時間での位置を計算する
         * @param[in]  frame  時間
         * @param[out] pos    時間frameにおける位置の格納先
         */
        void CalcPosition( u32 frame, gfl2::math::VEC3* pos ) const;

        /**
         * @brief すべての制御点を削除する
         */
        void RemoveAllControlPoints( void );



      private: 

        struct ControlPoint {
          u32 frame;
          gfl2::math::VEC3 pos;
          gfl2::math::VEC3 vel;
        };

        ControlPoint m_control_points[ MAX_CONTROL_POINT_NUM ];
        u8 m_control_point_count;

        void InsertControlPoint( 
            u8 index, u32 frame,
            const gfl2::math::VEC3& pos,
            const gfl2::math::VEC3& vel ); 

        void GetControlPoints( 
            u32 frame, ControlPoint* p0, ControlPoint* p1 ) const;

        void CalcHermite( 
            const ControlPoint& cp0, const ControlPoint& cp1, 
            u32 frame, gfl2::math::VEC3* pos ) const;

        f32 CalcValue( f32 t, f32 x0, f32 v0, f32 x1, f32 v1 ) const;
    };


  }  // namespace tool
}  // namespace app


#endif  // __APP_TOOL_INTERPOLATION_H__
