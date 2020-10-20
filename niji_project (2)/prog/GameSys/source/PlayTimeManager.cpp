//============================================================================================
/**
 * @file	PlayTimeManager.cpp
 * @brief	プレイ時間情報の管理
 * @date	2011.12.29
 * @author  Hisanao Suzuki
 *
 */
//============================================================================================

#if defined(GF_PLATFORM_CTR)
#include <nn/os.h>
#elif defined(GF_PLATFORM_WIN32)
#endif
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include "GameSys/include/PlayTimeManager.h"
#include "Savedata/include/PlayTime.h"
//#include <ro/gfl_RoManager.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
//#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(GameSys::PlayTimeManager);
template class gfl2::base::SingletonAccessor<GameSys::PlayTimeManager>;
//#endif

GFL_NAMESPACE_BEGIN(GameSys)

//------------------------------------------------------------------
/**
 * @brief 初期化（ゲームメインループの前に１回だけ呼ぶ）
 */
//------------------------------------------------------------------
void PlayTimeManager::Initialize( void )
{
  m_isStartFlag = false;
  m_lastTick    = 0;
  m_passedSec   = 0;
  m_startTick   = 0;
}

//------------------------------------------------------------------
/**
 * @brief プレイ時間のカウント開始（ゲーム開始直後に１回だけ呼ぶ）
 */
//------------------------------------------------------------------
void PlayTimeManager::Start( void )
{
  m_isStartFlag = true;
  m_lastTick    = 0;
  m_passedSec   = 0;
#if defined(GF_PLATFORM_CTR)
  m_startTick   = nn::os::Tick::GetSystemCurrent();
#elif defined(GF_PLATFORM_WIN32)
  m_startTick   = gfl2::system::Timer::PerformanceCounter::GetTick();
#endif
}

//------------------------------------------------------------------
/**
 * @brief プレイ時間のカウント（メインループ内で毎回呼ぶ）
 *
 * @param time		プレイ時間セーブデータポインタ
 */
//------------------------------------------------------------------
void PlayTimeManager::Update( Savedata::PlayTime* playTime )
{
	if( m_isStartFlag )
	{
	  // 一定周期でしか実行しない
	  if( TICK_TIME_INTERVAL > ++m_interval ){
	    return;
	  }
    m_interval = 0;

    s64 sec;
#if defined(GF_PLATFORM_CTR)
    s64 currentTick = nn::os::Tick::GetSystemCurrent();
#elif defined(GF_PLATFORM_WIN32)
    s64 currentTick = gfl2::system::Timer::PerformanceCounter::GetTick();
#endif
    s64 tick        = currentTick;

    // チックタイムの逆点現象チェック
    // Wifi接続時にtickは０クリアされる。
    if( m_lastTick > tick ){
      // 0クリアされる前のチック分も足しこむ。
      tick = tick + m_lastTick;
    }else{
      m_lastTick = tick;
    }

    sec = ConvertTicksToSeconds( tick - m_startTick );
		if( sec > m_passedSec )
		{
		  playTime->CountUp( sec - m_passedSec );

      // 通常時
      if( m_lastTick == tick ){
        m_passedSec = sec;
      }
      // 何かしらの処理でTickがクリアされた場合、再初期化。
      else
      {
        m_startTick = currentTick;
        m_passedSec = ConvertTicksToSeconds( currentTick );
        m_lastTick  = currentTick;
      }
		}
	}
}


//------------------------------------------------------------------
/**
 * @brief スリープ復帰処理（スリープしていた時間はカウントしない）
 */
//------------------------------------------------------------------
void PlayTimeManager::Restert( void )
{
  if( m_isStartFlag )
  {
#if defined(GF_PLATFORM_CTR)
    s64 currentTick = nn::os::Tick::GetSystemCurrent();
#elif defined(GF_PLATFORM_WIN32)
    s64 currentTick = gfl2::system::Timer::PerformanceCounter::GetTick();
#endif
    m_startTick = currentTick;
    m_passedSec = 0;
    m_lastTick  = currentTick;
  }
}

//------------------------------------------------------------------
/**
 * @brief システムチックを秒数に変換
 *
 * @param tickTime システムチック
 *
 * @return 秒数
 */
//------------------------------------------------------------------
inline s64 PlayTimeManager::ConvertTicksToSeconds( s64 tickTime )
{
//  return tickTime / nn::os::Tick::TICKS_PER_SECOND;
  return tickTime / gfl2::system::Timer::PerformanceCounter::GetFrequency();  // CTRではnn::os::Tick::TICKS_PER_SECONDが返ってくる

}


GFL_NAMESPACE_END(GameSys)
