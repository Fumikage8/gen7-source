//=============================================================================
/**
 * @brief スクロールする値の管理
 * @file app_tool_ScrollValue.cpp
 * @author obata_toshihiro
 * @date 2011.04.25
 */
//=============================================================================
#include "AppLib/include/Tool/app_tool_ScrollValue.h"

#include <math/include/gfl2_math.h>
#include <debug/include/gfl2_DebugPrint.h>

namespace app {
  namespace tool {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param settings  動作を定義するデータ
     */
    //-------------------------------------------------------------------------
    ScrollValue::ScrollValue( const Settings& settings ) : 
      m_min_value( settings.min_value ),
      m_max_value( settings.max_value ),
      m_default_accel( settings.default_decel ),
      m_overrun_enable( settings.overrun_enable ),
      m_overrun_frame( settings.overrun_frame ),
      m_locked( false ),
      m_state( STATE_STOP ),
      m_now_frame( 0 ),
      m_end_frame( 0 ),
      m_end_value( 0.0f ),
      m_start_value( 0.0f ),
      m_start_speed( 0.0f ),
      m_accel( 0.0f ),
      m_value( settings.init_value ),
      m_stop_request( false ),
      m_stop_value( 0.0f )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    ScrollValue::~ScrollValue()
    {
    }





    //-------------------------------------------------------------------------
    /**
     * @brief 値の更新を停止する
     */
    //-------------------------------------------------------------------------
    void ScrollValue::LockScroll( void ) 
    { 
      this->StopScroll();
      m_locked = true; 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 値の更新を再開する
     */
    //-------------------------------------------------------------------------
    void ScrollValue::UnlockScroll( void ) 
    { 
      m_locked = false; 
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 値の更新が停止しているかどうかを調べる
     * @retval true   更新が停止されている
     * @retval false  更新が有効
     */
    //-------------------------------------------------------------------------
    bool ScrollValue::IsScrollLocked( void ) const
    {
      return m_locked;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief スクロールの現在値を設定する
     * @param value  セットする値
     */
    //-------------------------------------------------------------------------
    void ScrollValue::SetCurrentValue( f32 value )
    { 
      if( !m_overrun_enable ) 
      {
        // 限界値を超えないように補正
        if( value < m_min_value ) {
          m_value = m_min_value;
        }
        else if( m_max_value < value ) {
          m_value = m_max_value;
        }
        else {
          m_value = value;
        }
      }
      else
      {
        m_value = value;
      }
    } 






    //-------------------------------------------------------------------------
    /**
     * @brief スクロールを開始する
     * @param init_speed  スクロールの初速度[value/frame]
     */
    //-------------------------------------------------------------------------
    void ScrollValue::StartScroll( f32 init_speed )
    {
      if( this->IsScroll() ) {
        this->StopScroll();
      }

      if( this->IsScrollStartEnable( m_value, init_speed ) ) {
        f32 accel = ( 0.0f < init_speed ) ?  (-m_default_accel) : (m_default_accel);
        this->StartScrollCore_V0_A( init_speed, accel );
        m_state = STATE_MOVE;
      }
    }

    /**
     * @brief スクロールを開始できる状態かどうかを調べる
     * @param init_speed  初速度
     * @param now_value
     * @retval true   スクロールできる
     * @retval false  スクロールできない
     */
    bool ScrollValue::IsScrollStartEnable( f32 now_value, f32 init_speed ) const
    {
      // 既に限界値を超えているなら, 戻す方向以外は不可
      if( this->IsLimitOver( now_value ) ) {
        if( ( now_value < m_min_value ) && ( init_speed < 0.0f ) ) {
          return false;
        }
        else if( ( m_max_value < now_value ) && ( 0.0f < init_speed ) ) {
          return false;
        }
      }
      return true;
    }

    /**
     * @brief スクロールパラメータを決定する( 初速度, 加速度を固定 )
     * @param init_speed  初速度
     * @param accel       加速度
     */
    void ScrollValue::StartScrollCore_V0_A( f32 init_speed, f32 accel )
    {
      f32 end_frame = -init_speed / accel;
      f32 end_value = this->CalcValue( m_value, init_speed, accel, end_frame );
      this->SetScrollParams( end_frame, end_value, m_value, init_speed, accel );
    }

    /**
     * @brief スクロールの値を求める
     * @param start_value  始点
     * @param start_speed  初速度
     * @param accel        加速度
     * @param frame        現在のフレーム数
     * @return 指定したフレーム数での位置
     */
    f32 ScrollValue::CalcValue( 
        f32 start_value, f32 start_speed, f32 accel, u32 frame ) const
    {
      return start_value + ( start_speed + 0.5f * accel * frame ) * frame;
    }

    /**
     * @brief スクロールの動作を設定する
     * @param end_frame    停止までのフレーム数
     * @param end_value    終点
     * @param start_value  始点
     * @param start_speed  初速度
     * @param accel        加速度
     * @param loopFlag     循環計算を行うか？
     */
    void ScrollValue::SetScrollParams( 
        u32 end_frame, f32 end_value, f32 start_value, f32 start_speed, f32 accel, bool loopFlag )
    {
      m_now_frame = 0;
      m_speed = 0;
      m_accel = accel;
      m_value = start_value;
      m_end_frame = end_frame;
      m_end_value = end_value;
      m_start_value = start_value;
      m_start_speed = start_speed; 
      if(loopFlag){
        m_state = STATE_CIRCULAR_DIV;
      }

      GFL_PRINT( "t=%d, a=%.3f, v0=%.3f, x0=%.3f, x1=%.3f, max=%f\n", 
          end_frame, accel, start_speed, start_value, end_value, m_max_value );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief スクロールを開始する
     * @param init_speed      スクロールの初速度[value/frame]
     * @param end_value_unit  終点の最低単位[value]
     *
     * @note end_value_unit の整数倍の位置で停止します
     */
    //-------------------------------------------------------------------------
    void ScrollValue::StartScroll( f32 init_speed, f32 end_value_unit )
    {
      if( this->IsScroll() ) {
        this->StopScroll();
      }

      if( this->IsScrollStartEnable( m_value, init_speed ) ) {
        m_state = STATE_MOVE; 
        f32 accel = ( 0.0f < init_speed ) ? ( -m_default_accel ) : ( m_default_accel );
        this->StartScrollCore_V0( init_speed, accel, end_value_unit );
      }
    }

    /**
     * @brief スクロールパラメータを決定する( 初速度を固定 )
     * @param init_speed      初速度
     * @param accel           加速度
     * @param end_value_unit  停止位置の最小単位
     */
    void ScrollValue::StartScrollCore_V0( f32 init_speed, f32 accel, f32 end_value_unit )
    {
        f32 x0 = m_value;
        f32 v0 = init_speed;
        f32 a = accel;
        f32 t = -v0 / a;
        f32 x1 = x0 + ( v0 + 0.5f * a * t ) * t;

/*
	x0 = 3;
	v0 = 0.5;
	a  = -0.08;
	t  = 6.25;
	x1 = 3.0 + ( 0.5 + 0.5 * -0.08 * 6.25 ) * 6.25 = 4.5625

	x0 = 3.0;
	v0 = -0.5;
	a  = 0.08;
	t  = 6.25;
	x1 = 3.0 + ( -0.5 + 0.5 * 0.08 * 6.25 ) * 6.25 = 1.4375
*/
			
#if 1	// 初速度、加速度が同じなのに、+方向と-方向で移動量が違う 2012/08/11 by nakahiro
        // 終点が最小単位の整数倍になるように補正
				s32	move_unit = static_cast<s32>( (x1-x0) / end_value_unit );

        // 最低でも単位分移動するように補正
        if( gfl2::math::Abs(move_unit) < 1 ) {
          move_unit = ( v0 < 0.0f ) ? (-1) : (1);
        }

        x1 = (x0/end_value_unit+static_cast<f32>(move_unit)) * end_value_unit;
#else
				s32	move_unit = static_cast<s32>( (x1-x0) / end_value_unit );

        // 最低でも1単位分移動するように補正
        if( gfl::math::Abs(move_unit) < 1 ) {
          move_unit = ( v0 < 0.0f ) ? (-1) : (1);
        }

				x1 = x0 + static_cast<f32>( move_unit );
				s32	x1_unit = static_cast<s32>( x1 / end_value_unit );
				x1 = static_cast<f32>( x1_unit ) * end_value_unit;
#endif

        // 補正後の終点から, 加速度, フレーム数を調整
        a = -( v0 * v0 ) * 0.5f / ( x1 - x0 );
        t = -v0 / a;

        this->SetScrollParams( t, x1, x0, v0, a ); 
    }






    //-------------------------------------------------------------------------
    /**
     * @brief スクロールを開始する
     * @param end_value  終点の値
     * @param end_frame  終点までのフレーム数
     */
    //-------------------------------------------------------------------------
    void ScrollValue::StartBindScroll( f32 end_value, u32 end_frame )
    {
      if( this->IsScroll() ) {
        this->StopScroll();
      }

      // 限界値を超えないように補正
      if( !m_overrun_enable && 
          this->IsLimitOver( end_value ) ) 
      {
        end_value = ( end_value < m_min_value ) ? m_min_value : m_max_value;
      }

      m_state = STATE_MOVE;
      this->StartScrollCore_X1_T( end_value, end_frame );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 循環スクロールを開始する
     *        minとmaxの間でoperateの向きに向かってend_frameで移動する
     *        minより小さくなったらmaxから,maxを超えたらminからend_valueに向かう
     * @param end_value  終点の値
     * @param end_frame  終点までのフレーム数
     * @param operate    加減の向き( +1 か -1 )
     */
    //-------------------------------------------------------------------------
    void ScrollValue::StartBindScrollCircular( f32 end_value, u32 end_frame, int operate )
    {
      this->StartScrollCore_Circular( end_value, end_frame, operate );
    }

    /**
     * @brief スクロールパラメータを決定する( 終点, フレーム数を固定 )
     * @param end_value  終点
     * @param end_frame  最終フレーム数
     */
    void ScrollValue::StartScrollCore_X1_T( f32 end_value, u32 end_frame )
    {
      f32 x1 = end_value;
      f32 x0 = m_value;
      f32 a = ( x0 < x1 ) ? ( -m_default_accel ) : ( m_default_accel );
      u32 t = end_frame;
      f32 v0 = ( x1 - x0 ) / t - 0.5f * a * t;

      // 通り過ぎてしまう場合は, 初速度と加速度を補正する
      if( -v0/a < t ) {
        v0 = 2*(x1-x0)/t;
        a=-v0/t;
      }

      this->SetScrollParams( t, x1, x0, v0, a );
    } 

    void ScrollValue::StartScrollCore_Circular( f32 end_value, u32 end_frame, int operate )
    {
      f32 distance_1=0, distance_2=0;
      f32 distance=0;
      // 現在地と最終地のどちらが大きいか
      if(m_value<end_value){ 
        // 最終地点が大きい
        distance_1 = end_value - m_value;
        distance_2 = (m_max_value - m_min_value) - distance_1;
      }else{
        // 開始地点が大きい
        distance_1 = m_value - end_value;
        distance_2 = (m_max_value - m_min_value) - distance_1;
      }

      // プラス方向の際の距離算出
      if(operate>0){
        if(distance_1 < distance_2){ distance = distance_1; }
        else{                        distance = distance_2; }
        f32 v0 = ( distance / end_frame ) * operate;
        this->SetScrollParams( end_frame, end_value, m_value, v0, 0, true );
      }else{
        if(distance_1 < distance_2){ distance = distance_1; }
        else{                        distance = distance_2; }
        f32 v0 = ( distance / end_frame ) * operate;
        this->SetScrollParams( end_frame, end_value, m_value, v0, 0, true );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief スクロールを停止する
     * @note 即座に停止します
     */
    //-------------------------------------------------------------------------
    void ScrollValue::StopScroll( void )
    {
      m_state = STATE_STOP;
      m_stop_request = false;
      m_speed = 0.0f;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief スクロール停止リクエストを受けたかどうかを調べる
     * @retval true   リクエストを受けた
     * @retval false  リクエストを受けていない
     *
     * @note
     * StopRequest()を呼んだ直後から停止するまでの間, true を返します.
     */
    //-------------------------------------------------------------------------
    bool ScrollValue::HaveStopRequest( void ) const
    {
      return m_stop_request;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スクロール停止リクエスト
     * @note end_value_unit の整数倍の位置で停止します
     */
    //-------------------------------------------------------------------------
    void ScrollValue::StopRequest( u32 end_value_unit )
    {
      if( m_state == STATE_MOVE ) {
        if( 0 <= m_speed ) {
          m_stop_value = gfl2::math::S16ToF32(
              static_cast<s16>( m_value / end_value_unit + 1 ) * end_value_unit );
        }
        else {
          m_stop_value = gfl2::math::S16ToF32(
              static_cast<s16>( m_value / end_value_unit - 1 ) * end_value_unit );
        }
        m_stop_request = true;
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief スクロール中かどうかを調べる
     * @retval true   スクロール中
     * @retval false  停止中
     */
    //-------------------------------------------------------------------------
    bool ScrollValue::IsScroll( void ) const
    {
      return ( m_state != STATE_STOP );
    }








    //-------------------------------------------------------------------------
    /**
     * @brief 1フレーム分だけ値を更新する
     */
    //-------------------------------------------------------------------------
    void ScrollValue::UpdateScroll( void )
    { 
      if( this->IsScrollLocked() ) {
        return;
      }

      if( this->IsScroll() ) {
        this->UpdateScrollValue();
      }
      else if( this->IsLimitOver( m_value ) ) 
      {
        this->StartOverAct();
        m_state = STATE_HERMITE;
      }
    } 

    /**
     * @brief スクロール更新処理
     */
    void ScrollValue::UpdateScrollValue( void )
    {
      switch( m_state )
      {
      case STATE_MOVE:
        this->UpdateScrollValue_MOVE();
        break; 

      case STATE_HERMITE:
        this->UpdateScrollValue_HERMITE();
        break;
      
      case STATE_CIRCULAR_DIV:  // 循環分割計算
        this->UpdateScrollValue_CIRCULAR();
        break;
      } 
    }


    //=========================================================================
    /**
     * @brief スクロール更新処理 ( 等加速度運動によりスクロール中 )
     */
    //=========================================================================
    void ScrollValue::UpdateScrollValue_MOVE( void )
    {
      // フレーム数が経過
      if( m_end_frame <= m_now_frame ) {
        this->StopScroll();
        m_value = m_end_value;
        m_state = STATE_STOP;
        return;
      }

      // 次のフレームに更新
      m_now_frame++;
      
      gfl2::math::VEC3 pos;

      f32 old_value = m_value;
      f32 new_value = this->CalcValue( m_start_value, m_start_speed, m_accel, m_now_frame );
      m_speed = new_value - old_value;
      m_value = new_value;

      // 限界値を突破
      if( !this->IsLimitOver( old_value ) && this->IsLimitOver( new_value ) ) 
      {
        if( m_overrun_enable ) {
          this->StartOverAct();
          m_state = STATE_HERMITE;
          return;
        }
        else {
          this->StopScroll();
          m_state = STATE_STOP;
          m_value = ( m_value < m_min_value ) ? m_min_value : m_max_value;
          return;
        }
      }

      // 停止リクエスト条件を満たしたら停止する
      if( ( m_stop_request ) && ( 0 < m_speed ) &&
          ( old_value <= m_stop_value ) && ( m_stop_value <= new_value ) ) {
        m_value = m_stop_value;
        this->StopScroll();
      }
      else if( ( m_stop_request ) && ( m_speed < 0 ) &&
          ( new_value <= m_stop_value ) && ( m_stop_value <= old_value ) ) {
        m_value = m_stop_value;
        this->StopScroll();
      }
    } 

    /**
     * @biref 限界値を超えているかどうかを調べる
     * @param value  チェックする値
     * @retval true   超えている
     * @retval false  超えていない
     */
    bool ScrollValue::IsLimitOver( f32 value ) const
    {
      return ( ( value < m_min_value ) || ( m_max_value < value ) );
    } 

    /**
     * @brief 限界値突破時の収束動作を開始する
     */
    void ScrollValue::StartOverAct( void )
    { 
      this->RemoveAllControlPoints(); 
      f32 limit_value = ( m_value < m_min_value ) ? m_min_value : m_max_value;

      gfl2::math::VEC3 p0( m_value, 0, 0 );
      gfl2::math::VEC3 v0( m_speed, 0, 0 );
      gfl2::math::VEC3 p1( limit_value, 0, 0 );
      gfl2::math::VEC3 v1( -m_speed, 0, 0 );

      this->AddControlPoints( 0, p0, v0 );
      this->AddControlPoints( m_overrun_frame, p1, v1 );
      m_now_frame = 0;
      m_end_frame = m_overrun_frame;
      m_end_value = limit_value;
    }


    //=========================================================================
    /**
     * @brief スクロール更新処理( エルミート補間によりスクロール中 )
     */
    //=========================================================================
    void ScrollValue::UpdateScrollValue_HERMITE( void )
    {
      m_now_frame++;
      if( m_end_frame <= m_now_frame ) {
        this->StopScroll();
        m_value = m_end_value;
        m_state = STATE_STOP;
      }
      else
      {
        gfl2::math::VEC3 pos;
        this->CalcPosition( m_now_frame, &pos );
        f32 old_value = m_value;
        f32 new_value = pos.x;
        m_speed = new_value - old_value;
        m_value = new_value;
      }
    }

    //=========================================================================
    /**
     * @brief スクロール更新処理( 循環計算 符号+-あり )
     *        -の場合はminより小さくなってもmaxからmaxに戻って任意の値に向かう
     *        +の場合はmaxより大きくなってもminに戻って任意の値に向かう
     */
    //=========================================================================
    void ScrollValue::UpdateScrollValue_CIRCULAR( void )
    {
      m_now_frame++;
      if(m_end_frame <= m_now_frame){
        m_value = m_end_value;
        m_state = STATE_STOP;
      }else{
        f32 value = m_value + m_start_speed;
        // 加算の結果,最少最大の範囲を超えたら逆側に移動させて補正
        if(value<m_min_value){
          value = m_max_value - (value-m_min_value);
        }else if(value>m_max_value){
          value = m_min_value + (value-m_max_value);
        }
        m_value = value;
      }
    }


  } // namespace tool
} // namespace app
