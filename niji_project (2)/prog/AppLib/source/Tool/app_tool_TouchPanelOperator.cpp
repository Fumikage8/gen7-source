//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		app_tool_TouchPanelOperator.cpp
 *	@brief  タッチパネル操作クラス
 *	@author	Toru=Nagihashi
 *	@date		2012.06.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "AppLib/include/Tool/app_tool_TouchPanelOperator.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

const f32 TouchPanelOperator::FLICK_POWER_MAX = 1.0f;
const f32 TouchPanelOperator::FLICK_POWER_MIN = 0.0f;

const f32 TouchPanelOperator::FLICK_DISTANCE_MIN  = 10.0f;  ///<フリックするために最低限必要な長さ
const f32 TouchPanelOperator::FLICK_DISTANCE_MAX  = 150.f; ///<フリックする最大距離。
const f32 TouchPanelOperator::DECIDE_DISCANCE  = 8.0f; ///<これ以上トリガとリリースが離れると決定とみなされなくなる距離

//-----------------------------------------------------------------------
// 生成
//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ  
 * @param heap  ヒープ
 * @param touch_panel タッチパネル
 * @param framerate フレームレート
 */
TouchPanelOperator::TouchPanelOperator( const gfl2::ui::TouchPanel *touch_panel, GameSys::FrameMode framerate ) :
  m_touch_panel( touch_panel ),
  m_framerate( framerate ),
  m_start_value(),
  m_end_value(),
  m_state( STATE_NONE ),
  m_flick_power( 0.0f ),
  m_drag_counter( 0 ),
  m_spaced_decide( false ), 
  m_drag_log_start( 0 ),
  m_drag_log_end( 0 ),
  m_is_drag_log_around( false )
{
}

/**
 * @brief デストラクタ
 */
TouchPanelOperator::~TouchPanelOperator()
{
}
//-----------------------------------------------------------------------
// システム
//-----------------------------------------------------------------------
/**
 * @brief 更新
 */
void TouchPanelOperator::Update(void)
{
  if( m_touch_panel->IsTouchTrigger() )
  {
    m_start_value.x = m_touch_panel->GetX();
    m_start_value.y = m_touch_panel->GetY();
    m_state = STATE_DRAG_START;
    m_spaced_decide = false;
  }
  else if( m_touch_panel->IsTouchRelease() )
  {
    if( m_spaced_decide == false && this->CheckDecideRange( m_end_value ) )
    {
      m_state = STATE_DECIDE;
    }
    else if( this->CheckFlickEnable() )
    {
      m_flick_power = this->CalcFlickPower();
      m_state = STATE_FLICK;
    }
    else
    {
      m_state = STATE_NONE;
    }
  }
  else
  {
    if( m_touch_panel->IsTouch() )
    {
      m_end_value.x = m_touch_panel->GetX();
      m_end_value.y = m_touch_panel->GetY();
      m_state = STATE_DRAG_MAIN;
      if( !this->CheckDecideRange(m_end_value) )
      {
        m_spaced_decide = true;
      }
      
      if( m_drag_counter % 2 == 0 )
      {
        this->UpdateDragLogs( m_end_value.x, m_end_value.y );
      }
      if( m_framerate == GameSys::FRAMEMODE_60 )
      {
        m_drag_counter  += 1;
      }
      else
      {
        m_drag_counter  += 2;
      }
    }
    else
    {
      m_state = STATE_NONE;
      m_flick_power = 0.0f;
      
      m_drag_counter    = 0;
      m_drag_log_start  = 0;
      m_drag_log_end    = 0;
      m_is_drag_log_around  = false;
    }
  }
}
//-----------------------------------------------------------------------
// 取得
//-----------------------------------------------------------------------
/**
 * @brief ドラッグ開始かを取得
 *
 * @return trueドラッグ開始中 falseドラッグ開始していない
 */
bool TouchPanelOperator::IsDragStart(void) const
{
  return m_state == STATE_DRAG_START;
}
/**
 * @brief ドラッグ中かを取得(IsDragStart==trueも含みます)
 *
 * @return trueドラッグ中 falseドラッグしていない
 */
bool TouchPanelOperator::IsDrag(void) const
{
  return m_state == STATE_DRAG_MAIN
    || m_state == STATE_DRAG_START;;
}
/**
 * @brief ドラッグ開始座標を取得
 *
 * @param start_value ドラッグ開始座標受け取り
 *
 * @return trueドラッグ中で取得OK falseドラッグ中ではないため取得できなかった
 */
bool TouchPanelOperator::GetDragStartValue( gfl2::math::VEC2 *start_value ) const
{
  if( this->IsDrag() )
  {
    *start_value = m_start_value;
    return true;
  }
  return false;
}
/**
 * @brief ドラッグ現在座標を取得
 *
 * @param now_value ドラッグ現在座標受け取り
 *
 * @return trueドラッグ中で取得OK falseドラッグ中ではないため取得できなかった
 */
bool TouchPanelOperator::GetDragNowValue( gfl2::math::VEC2 *now_value ) const
{
  if( this->IsDrag() )
  {
    now_value->x = m_touch_panel->GetX();
    now_value->y = m_touch_panel->GetY();

    return true;
  }
  return false;
}
/**
 * @brief フリックをしたかを取得
 *
 * @return trueフリックした falseフリックしていない
 * 
 * @note  タイミング的にはIsTouchRelease == true時の１フレームのみ返ってきます。
 *        またかならずIsDrag() != IsFlick()が保証されています。
 */
bool TouchPanelOperator::IsFlick(void) const
{
  return m_state == STATE_FLICK;
}
/**
 * @brief フリック現在座標を取得
 *
 * @param end_value フリック現在座標受け取り
 *
 * @return trueフリック中で取得OK falseフリック中ではないため取得できなかった
 */
bool TouchPanelOperator::GetFlickEndValue( gfl2::math::VEC2 *end_value ) const
{
  if( this->IsFlick() )
  {
    *end_value  = m_end_value;
    return true;
  }
  return false;
}

/**
 * @brief フリックの強さを取得
 *
 * @return フリックの強さ(0~1.0f)
 */
f32 TouchPanelOperator::GetFlickPower( void ) const
{
  return m_flick_power;
}

/**
 * @brief フリックの強さベクトルを取得
 *
 * @param vectorベクトル（now_value-start_value を正規化してFlickPowerをかけたものです）
 */
void TouchPanelOperator::GetFlickPowerVector( gfl2::math::VEC2 *vector ) const
{
  *vector = m_end_value - m_start_value;
  *vector  = vector->Normalize();
  *vector *= this->GetFlickPower();
}

/**
 * @brief 決定を取得（決定とは、タッチリリースしたとき・しかしドラッグをしていないときという条件です）
 *
 * @return true決定 fale決定してない
 */
bool TouchPanelOperator::IsDecide(void) const
{
  return m_state == STATE_DECIDE;
}
/**
 * @brief 決定したときの値を取得
 *
 * @param decide_value 決定したときの値
 * @return true決定中で取得OK false決定中ではないため取得できなかった
 */
bool TouchPanelOperator::GetDecideValue( gfl2::math::VEC2 *decide_value )
{
  if( this->IsDecide() )
  {
    *decide_value  = m_end_value;
    return true;
  }
  return false;
}
/**
 * @brief タッチパネルを取得
 *
 * @return タッチパネル
 */
const gfl2::ui::TouchPanel *TouchPanelOperator::GetTouchPanel(void) const
{
  return m_touch_panel;
}
//-----------------------------------------------------------------------
// protected
//-----------------------------------------------------------------------
/**
 * @brief フリック検知
 *
 * @return trueでフリック検知 falseで検知しなかった
 * 
 * @note override可 フリック検知の仕組みを変えたい時を想定
 *      タッチリリース時にstartとendから検知している
 */
bool TouchPanelOperator::CheckFlickEnable( void ) const
{
  return this->CalcFlickPower() > FLICK_POWER_MIN;
}
/**
 * @brief フリック強度の計算
 *
 * @return (0.0f〜1.0f)フリックの強度
 *
 * @note override可 フリック強度の仕組みを変えたい時を想定
 */      
f32 TouchPanelOperator::CalcFlickPower( void ) const
{
  if( m_drag_log_start != m_drag_log_end )
  {
    f32 dist_sq = gfl2::math::VEC2DistSq( &this->GetDragLogEndVec(), &this->GetDragLogStartVec() );
    
    if( dist_sq >= FLICK_DISTANCE_MIN*FLICK_DISTANCE_MIN )
    {
      f32 flick_distance_max  = (FLICK_DISTANCE_MAX-FLICK_DISTANCE_MIN) * (FLICK_DISTANCE_MAX-FLICK_DISTANCE_MIN);
      f32 distance  = gfl2::math::Clamp( dist_sq-FLICK_DISTANCE_MIN*FLICK_DISTANCE_MIN, 0.0f, flick_distance_max );
      f32 distance_power  = distance / flick_distance_max;
      
      f32 power = distance_power;
      //GFL_PRINT( "power %f now%f max%f\n", power, distance, flick_distance_max );
      //GFL_PRINT( "end x=%f y=%f start x=%f y=%f\n", this->GetDragLogEndVec().x, this->GetDragLogEndVec().y, this->GetDragLogStartVec().x, this->GetDragLogStartVec().y );
      //GFL_PRINT( "start_idx %d end_idx %d\n", m_drag_log_start, m_drag_log_end );
      return FLICK_POWER_MIN + (FLICK_POWER_MAX - FLICK_POWER_MIN) * power ;
    }
  }
    
  return FLICK_POWER_MIN;
}
/**
 * @brief 決定の範囲をチェック
 * @param now現在の座標
 * @return trueで決定範囲内 falseで決定範囲外
 * 
 * @note override可 決定範囲の仕組みを変えたい時を想定
 */
bool TouchPanelOperator::CheckDecideRange( const gfl2::math::VEC2 & now ) const
{
  return gfl2::math::VEC2DistSq( &now, &m_start_value ) < (DECIDE_DISCANCE * DECIDE_DISCANCE);
}

void TouchPanelOperator::UpdateDragLogs( u16 x, u16 y )
{
  ++m_drag_log_end;
  if( m_drag_log_end >= DRAG_LOG_NUM )
  {
    m_is_drag_log_around  = true;
    m_drag_log_end = 0;
  }

  u16 index = 0;
  if( m_drag_log_end == 0 )
  {
    index = DRAG_LOG_NUM-1;
  }
  else
  {
    index = m_drag_log_end-1;
  }
  
  //一周してたら開始もずらす
  if( m_is_drag_log_around )
  {
    ++m_drag_log_start;
    m_drag_log_start  %= DRAG_LOG_NUM;
  }

  m_drag_log_value[index].x  = x;
  m_drag_log_value[index].y  = y;
  

  //GFL_PRINT( "ログstart=%d end=%d idx=%d\n", m_drag_log_start, m_drag_log_end, index );
  //GFL_PRINT( "ログ x%f y%f\n", m_drag_log_value[index].x, m_drag_log_value[index].y );
}

const gfl2::math::VEC2 & TouchPanelOperator::GetDragLogStartVec( void ) const
{
  return m_drag_log_value[ m_drag_log_start ];
}
const gfl2::math::VEC2 & TouchPanelOperator::GetDragLogEndVec( void ) const
{
  u32 index = (m_drag_log_end == 0 ? DRAG_LOG_NUM-1: m_drag_log_end-1);
  return m_drag_log_value[ index ];
}

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
