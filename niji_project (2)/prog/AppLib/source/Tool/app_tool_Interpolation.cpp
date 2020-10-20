//=============================================================================
/**
 * @file app_tool_Interpolation.cpp
 * @brief エルミート補間計算クラス
 * @author obata
 * @date 2011.10.19
 */
//=============================================================================
#include "AppLib/include/Tool/app_tool_Interpolation.h"

namespace app {
  namespace tool {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    Interpolation::Interpolation( void ) : 
      m_control_points(),
      m_control_point_count( 0 )
    {
      this->RemoveAllControlPoints();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    Interpolation::~Interpolation( void )
    {
    }



    //-------------------------------------------------------------------------
    /**
     * @brief すべての制御点を削除する
     */
    //-------------------------------------------------------------------------
    void Interpolation::RemoveAllControlPoints( void )
    {
      for( u8 i=0; i<MAX_CONTROL_POINT_NUM; i++ ) {
        m_control_points[i].frame = 0xffffffff;
      }
      m_control_point_count = 0;
    }



    //-------------------------------------------------------------------------
    /**
     * @brief 制御点を追加する
     * @param frame  時間
     * @param pos    位置
     * @param vel    速度
     */
    //-------------------------------------------------------------------------
    void Interpolation::AddControlPoints( 
        u32 frame, const gfl2::math::VEC3& pos, const gfl2::math::VEC3& vel )
    {
      if( MAX_CONTROL_POINT_NUM <= m_control_point_count ) {
        GFL_ASSERT(0);  // これ以上追加できない
        return;
      }

      // frame が昇順になる位置に挿入する
      for( u8 i=0; i<MAX_CONTROL_POINT_NUM; i++ ) {
        if( frame == m_control_points[i].frame ) {
          m_control_points[i].pos = pos;
          m_control_points[i].vel = vel;
          break;
        }
        else if( frame < m_control_points[i].frame ) {
          this->InsertControlPoint( i, frame, pos, vel );
          break;
        }
      }
    }

    /**
     * @brief 指定した位置に制御点を挿入する
     * @param index  挿入位置
     * @param frame  時間
     * @param pos    位置
     * @param vel    速度
     */
    void Interpolation::InsertControlPoint( 
        u8 index, u32 frame, const gfl2::math::VEC3& pos, const gfl2::math::VEC3& vel )
    {
      GFL_ASSERT( m_control_point_count < MAX_CONTROL_POINT_NUM ); 
      for( s16 i=m_control_point_count-1; index<=i; i-- ) {
        if( (i+1) < MAX_CONTROL_POINT_NUM ) {
          m_control_points[i+1] = m_control_points[i];
        }
      }
      m_control_points[ index ].frame = frame;
      m_control_points[ index ].pos = pos;
      m_control_points[ index ].vel = vel;
      if( m_control_point_count < MAX_CONTROL_POINT_NUM ) {
        m_control_point_count++;
      }
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 指定した時間での位置を計算する
     * @param[in]  frame  時間
     * @param[out] pos    時間frameにおける位置の格納先
     */
    //-------------------------------------------------------------------------
    void Interpolation::CalcPosition( u32 frame, gfl2::math::VEC3* pos ) const
    {
      // 全体の始点よりも前
      if( frame <= m_control_points[0].frame ) {
        *pos = m_control_points[0].pos;
        return;
      }
      // 全体の終点よりも後
      if( m_control_points[ m_control_point_count - 1 ].frame <= frame ) {
        *pos = m_control_points[ m_control_point_count - 1 ].pos;
        return;
      }
      // いずれかの区間に属する
      ControlPoint p0;
      ControlPoint p1;
      this->GetControlPoints( frame, &p0, &p1 );
      this->CalcHermite( p0, p1, frame, pos );
    }

    /**
     * @brief 指定した時間についての補間で
     *        始点, 終点となる制御点のインデックスを取得する
     * @param[in]  frame  時間
     * @param[out] p0     補間区間の始点
     * @param[out] p1     補間区間の終点
     */
    void Interpolation::GetControlPoints( 
        u32 frame, ControlPoint* p0, ControlPoint* p1 ) const
    {
      for( u8 i=0; i<m_control_point_count; i++ ) {
        if( frame < m_control_points[i].frame ) {
          *p1 = m_control_points[i];
          *p0 = m_control_points[i-1];
          break;
        }
      }
    }

    /**
     * @brief 指定した2点でエルミート補間する
     * @param[in]  cp0    補間区間の始点
     * @param[in]  cp1    補間区間の終点
     * @param[in]  frame  時間
     * @param[out] pos    時間frameにおける位置の格納先
     */
    void Interpolation::CalcHermite( 
        const ControlPoint& cp0, const ControlPoint& cp1,
        u32 frame, gfl2::math::VEC3* pos ) const
    {
      // 区間内での時間[0, 1]に変換
      f32 t = static_cast<f32>( frame - cp0.frame ) / static_cast<f32>( cp1.frame - cp0.frame );  
      pos->x = this->CalcValue( t, cp0.pos.x, cp0.vel.x, cp1.pos.x, cp1.vel.x );
      pos->y = this->CalcValue( t, cp0.pos.y, cp0.vel.y, cp1.pos.y, cp1.vel.y );
      pos->z = this->CalcValue( t, cp0.pos.z, cp0.vel.z, cp1.pos.z, cp1.vel.z );
    }

    /**
     * @brief 補間値を計算する
     * @param[in] t   時間[0, 1]
     * @param[in] x0  始点の値
     * @param[in] v0  始点の変位量( 微分値 )
     * @param[in] x1  終点の値
     * @param[in] v1  終点の変位量( 微分値 )
     */
    f32 Interpolation::CalcValue( f32 t, f32 x0, f32 v0, f32 x1, f32 v1 ) const
    {
      // 2制御点によるエルミート補間
      f32 a = v0 + v1 + ( 2 * ( x0 - x1 ) );
      f32 b = -( 2 * v0 ) - v1 - ( 3 * ( x0 - x1 ) );
      f32 c = v0;
      f32 d = x0;
      return ( t * ( t * ( a * t + b ) + c ) ) + d;
    }



  }  // namespace tool
}  // namespace app
