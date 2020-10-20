//==============================================================================
/**
 * @file	NetAppTimeout.cpp
 * @brief	時間計測クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
 */
// =============================================================================
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// mine
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"

#include "GameSys/include/NijiTime.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)

// コンストラクタ
NetAppTimeout::NetAppTimeout() :
  m_timeout(0),
  m_startingTime(),
  m_timerCheckWait(0),
  m_elapsedTime(0),
  m_isTimeout(false),
  m_isSuspend(false),
  m_suspendTime(),
  m_emergencyFrame(0)
{
}

// デストラクタ
NetAppTimeout::~NetAppTimeout()
{
}

/**
 *  @brief タイムアウト計測開始
   * @param[in] timeout  タイムアウト値（ms)
 */
void NetAppTimeout::TimeOutStart( const u32 timeout )
{
  m_timeout = timeout;
  m_isTimeout = false;
  GameSys::DeviceDate date;
  gfl2::system::Date::Copy( date.GetDateConst() , &m_startingTime );
  m_timerCheckWait = 0;
  m_elapsedTime = 0;
  // @fix GFNMCat[3968]：サスペンドしたまま終わる場合に計測されない 
  m_isSuspend = false;
  m_emergencyFrame = EMERGENCY_TIMEOUT_FRAME + ((timeout/1000)*30); // 1秒未満は切り捨てる
  GFL_RELEASE_PRINT( "TimeOutStart %d \n", timeout );
  GFL_RELEASE_PRINT( "[%lld] \n", m_startingTime.GetDateTimeToMilliSecond() );
}

/**
 *  @brief タイムアウトチェック
 *  @param[out] pElapsedTime 経過時間(ms)
 *             ※異常状態による強制時間経過でtrueが返るとタイムアウト値にならない事があります。
 *  @return タイムアウトならtrue
 */
bool NetAppTimeout::IsTimeOut( u32* pElapsedTime )
{
  // まだタイムアウトしていない かつ 一時停止していない かつ 10フレーム経過したら
  if( !m_isTimeout && !m_isSuspend && ++m_timerCheckWait >= 10 )
  {
    GameSys::DeviceDate date;
    m_elapsedTime = static_cast<u32>( date.GetDateConst()->GetDateTimeToMilliSecond() - m_startingTime.GetDateTimeToMilliSecond() );

    m_timerCheckWait = 0;

    m_isTimeout = ( m_elapsedTime >= m_timeout );

    // タイムアウトで同じ時間になる
    if( m_isTimeout ) { m_elapsedTime = m_timeout; }
    GFL_RELEASE_PRINT("IsTimeOut 経過時間:%d \n", m_elapsedTime);

#if PM_DEBUG
    if( !m_isTimeout && gfl2::system::Date::GetFakeMode() != gfl2::system::Date::FAKE_NONE )
    {
      GFL_ASSERT_MSG(0,"デバッグ機能で時間操作している為、強制的に時間経過したことにします。\nこのアサートはバグではありません。\n");
      m_isTimeout = true;
    }
#endif
  }

  if( pElapsedTime != NULL )
  {
    *pElapsedTime = m_elapsedTime;
  }

  // @fix NMCat[390]：何らかの要因で時間が経過しない状況が発生した場合のケア処理
  // @fix NMCat[4109][4115]：サスペンド時に意図せず動いていたので、条件に追加
  // タイムアウトしていない　かつ　一時停止していない
  if( !m_isTimeout && !m_isSuspend )
  {
    // TimeOutStartを呼んでいるかどうか
    if( m_timeout>0 )
    {
      if( m_emergencyFrame-- == 0 )
      {
        GameSys::DeviceDate date;
        GFL_RELEASE_PRINT( "m_timeout[%d]\n", m_timeout );
        GFL_RELEASE_PRINT( "m_timerCheckWait[%d]\n", m_timerCheckWait );
        GFL_RELEASE_PRINT( "isTimeout[%d] isSuspend[%d]\n", m_isTimeout, m_isSuspend );
        GFL_RELEASE_PRINT( "[%lld] [%lld]\n", date.GetDateConst()->GetDateTimeToMilliSecond(), m_startingTime.GetDateTimeToMilliSecond() );
        GFL_RELEASE_PRINT( "[%d] \n", static_cast<u32>(date.GetDateConst()->GetDateTimeToMilliSecond() - m_startingTime.GetDateTimeToMilliSecond()) );

        GFL_ASSERT_MSG(0,"時間経過不具合による非常時処理[%d]\n", m_elapsedTime );
        m_isTimeout = true;
      }
    }
  }

  return m_isTimeout;
}

/**
 * @brief 計測一時停止
 * @note  Resumeで再開して下さい
 */
void NetAppTimeout::Suspend()
{
  GFL_RELEASE_PRINT( "NetAppTimeout::Suspend\n");
  GameSys::DeviceDate date;
  gfl2::system::Date::Copy( date.GetDateConst() , &m_suspendTime );

  m_isSuspend = true;
}

/**
 * @brief 計測再開
 */
void NetAppTimeout::Resume()
{
  GFL_RELEASE_PRINT( "NetAppTimeout::Resume\n");
  GameSys::DeviceDate date;

  // 計測停止した時間から何秒経過したか計算
  s64 dateTime1 = date.GetDateConst()->GetDateTimeToSecond(); 
  dateTime1 -= m_suspendTime.GetDateTimeToSecond();

  // その経過秒数を開始時間に足して設定しなおす
  s64 dateTime2 = m_startingTime.GetDateTimeToSecond();
  dateTime2 += dateTime1;

  m_startingTime.SetDateTimeToSecond( dateTime2 );


  m_isSuspend = false;
}

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

