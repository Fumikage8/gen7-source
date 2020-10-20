#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet_NexP2pStationManager.cpp
 * @brief	  nn::nex::Station監視クラス
 * @author	ichiraku_katsuhiko
 * @data	2013/05/23, 17:42
 *
 * @note  reliableパケットの値を監視し、一定時間変更がないと、切断を呼びます。
 *         ランダムマッチの切断対策用です。
 *        ※他の箇所では使用禁止です。
 */
// =============================================================================

// system

#include "gflnet2/include/dev/gflnet2_NetZAdapter.h"
// mine
#include <gflnet2/include/nex/gflnet2_NexP2pStationManager.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
namespace gflnet2{
namespace nex{

//----------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 */
//----------------------------------------------------------------------
NexP2pStationManager::NexP2pStationManager( void )
{
  Reset();
}

//----------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//----------------------------------------------------------------------
NexP2pStationManager::~NexP2pStationManager( void )
{
}

//----------------------------------------------------------------------
/**
 *  @brief  リセット
 */
//----------------------------------------------------------------------
void NexP2pStationManager::Reset( void )
{
  m_beforeValue   = 0;
  m_intervalTime  = nn::nex::Time::GetTime();
  m_disconnectTime= nn::nex::Time::GetTime();
}

//----------------------------------------------------------------------
/**
 *  @brief  更新    毎フレーム呼ぶ
 *
 *  @retval true:正常
 *  @retval false:切断とした
 */
//----------------------------------------------------------------------
bool NexP2pStationManager::Update( void )
{
  nn::nex::Station *pStation = gflnet2::dev::NetZAdapter::GetStationInstance();

  // Stationは必ずチェックする
  if( pStation == NULL ){
    return true;
  }

  nn::nex::Time nowTime = nn::nex::Time::GetTime();

  // 一定時間で実行
  if( nowTime - m_intervalTime >= TIME_INTERVAL )
  {
    m_intervalTime = nowTime;

    // GetEndPointInfoInterfaceを呼ぶためのお作法
    nn::nex::Scheduler* pNexScheduler = nn::nex::Scheduler::GetInstance();
    if( pNexScheduler != NULL )
    {
      nn::nex::ScopedCS scope(pNexScheduler->SystemLock());

      // 相手のStation取得（P2Pなので1つのはず）
      nn::nex::Station::SelectionIterator itStations;
      itStations.GotoStart(); // 強制的に先頭にする
      while( !itStations.EndReached() )
      {

        // EndInfoのパラメータ取得
        nn::nex::EndPointInfoInterface* pEndPointInfoInt = itStations->GetEndPointInfoInterface();
        if ( pEndPointInfoInt != NULL )
        {
          nn::nex::EndPointInfo info;
          if( pEndPointInfoInt->GetEndPointInfo( &info ) )
          {
            if( m_beforeValue != info.receiveUnreliableCount )
            {
              // 値保存
              m_beforeValue = info.receiveUnreliableCount;

              // 切断までの時間リセット
              m_disconnectTime = nowTime;
            }
            else
            {
              GFL_PRINT("StationのReliableパケット変化なし\n");
            }

            //info.Trace(nn::nex::EventLog::Info);
          }
        }

        ++itStations;
      }
    }
  }

  // 一定時間変化がないと切断とする
  if( nowTime - m_disconnectTime >= TIME_OUT )
  {
    // 判定をリセットする。(もう1度7秒を監視)
    m_disconnectTime = nowTime;
    GFL_PRINT("NexP2pStaionManager SetSessionLeft\n");
    return false;
  }
  return true;
}

} // nex
} // gflnet
#endif
