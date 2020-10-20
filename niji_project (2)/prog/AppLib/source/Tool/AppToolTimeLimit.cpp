//==============================================================================
/**
 * @file    AppToolTimeLimit.h
 * @author  fukushima_yuya
 * @date    2015.03.05
 * @brief   制限時間チェック
 *          sango_project app_pokelist_TimeLimit
 */
//==============================================================================

// niji
#include "AppLib/include/Tool/AppToolTimeLimit.h"

// gflib2
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <math/include/gfl2_math_control.h>

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )

const u64 TimeLimit::ms_unitMillSec   = 1000;
const u64 TimeLimit::ms_unitSec       = 1000 * ms_unitMillSec;

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @brief[in] limit_time: チェックする制限時間値
 * @brief[in] unit_type:  limit_timeで設定した時間の単位
 */
//------------------------------------------------------------------------------
TimeLimit::TimeLimit( u64 limit_time, const UnitType unit_type )
{
  Setup( limit_time, unit_type );
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
TimeLimit::~TimeLimit( void )
{
  ;
}


//------------------------------------------------------------------
/**
  * @brief   クラスを動かすのに必要なパラメータ設定
  * @brief[in] limit_time: チェックする制限時間値
  * @brief[in] unit_type:  limit_timeで設定した時間の単位
  */
//------------------------------------------------------------------
void TimeLimit::Setup(  u64 limit_time, const UnitType unit_type )
{
  Reset();

  m_limitSec  = limit_time;
  //  unit_typeで指定した秒単位からマイクロ秒単位に直す
  if( unit_type == UNIT_TYPE_MILL_SEC )
  {
    m_limitSec *= ms_unitMillSec;
  }
  else
  if( unit_type == UNIT_TYPE_SEC )
  {
    m_limitSec *= ms_unitSec;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アップデート
 * @return  trueなら制限時間超過
 * @return  falseならそれ以外
 */
//------------------------------------------------------------------------------
bool TimeLimit::Update( void )
{
  // 制限時間なし
  if( m_limitSec <= 0 ) return false;

  // 制限時間オーバー( コンストラクタで指定した時間単位でチェック )
  u64 limitTime  = GetLimitTime();
  if( limitTime <= 0 ) return true;

  _Update();

  return false;
}

//------------------------------------------------------------------
/**
  * @brief   進行の比率を取得
  * @return  進行比率( 0.0f ～ 1.0f )
  */
//------------------------------------------------------------------
f32 TimeLimit::GetProgression()
{
  u64 limitTime = GetLimitTime();
  if( limitTime <= 0 )
  {
    return  1.0f;
  }

  if( m_limitSec <= 0 )
  {
    return  1.0f;
  }
  
  if( m_limitSec <= limitTime )
  {
    return  0.0f;
  }

  //  u64からf32に変換している
  //  u64はf32の浮動小数点の値範囲を超えている
  //  f32の表現できない範囲の時間設定をすると正しい値が取れない
  return  gfl2::math::Clamp<f32>( (f32)( m_limitSec - limitTime ) * ( 1.0f / (f32)( m_limitSec ) ), 0.0f, 1.0f );
}

//------------------------------------------------------------------
/**
  * @brief   残り時間の取得
  * @return  残り時間
  */
//------------------------------------------------------------------
u64 TimeLimit::GetLimitTime( void )
{
  if( m_bInit == true )
  {
    _Update();
  }

  u64 retTime = m_time;

  if( retTime <= 0 )
  {
    return  m_limitSec;
  }

  if( m_time >= m_limitSec ) return 0;

  return ( m_limitSec - retTime );
}

//------------------------------------------------------------------------------
/**
 * @brief   残り時間の取得
 * @return  残り時間(秒)
 */
//------------------------------------------------------------------
u32 TimeLimit::GetLimitSecond( void )
{
  //  秒単位に変換
  u64 limitTime = GetLimitTime();
  if( limitTime <= 0 )
  {
    return  0;
  }

  //  ミクロ秒から秒へ単位変換
  return static_cast<u32>( limitTime / ms_unitSec );
}

void TimeLimit::_Update()
{
  u64 now = gfl2::system::Timer::PerformanceCounter::GetTick();
  if( m_bInit == true )
  {
    m_startTick = now;
    m_bInit = false;
  }

  u64 ss = gfl2::system::Timer::PerformanceCounter::GetMicroSecond( m_startTick );
  u64 es = gfl2::system::Timer::PerformanceCounter::GetMicroSecond( now );

  m_time = es - ss;
}

GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )
