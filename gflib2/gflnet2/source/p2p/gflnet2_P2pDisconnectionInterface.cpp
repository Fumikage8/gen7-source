//==============================================================================
/**
 * @file	gflnet2_P2pDisconnectionInterface.cpp
 * @brief	P2Pプロセス：切断インターフェイス
 * @author	ichiraku_katsuhiko
 * @date	2015/04/21 14:33:20
 */
// =============================================================================
#ifdef  GF_PLATFORM_CTR

#include <nex.h>
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"

#include "gflnet2/include/p2p/gflnet2_P2pDisconnectionInterface.h"

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

//------------------------------------------------------------------
/**
 * @brief   Nexの送受信機能を無効化
 */
//------------------------------------------------------------------
void P2pDisconnectionInterface::DirectStreamDisable( void )
{
  // nexの送受信機能を無効化
  nn::nex::DirectStream* directStream =gflnet2::dev::NetZAdapter::GetDirectStreamInstance();
  if( directStream )
  {
    directStream->Disable();
    GFL_PRINT("nex::DirectStream Disable\n");
  }
}


//------------------------------------------------------------------
/**
* @brief   Session切断
* @return  trueで呼び出し成功
*/
//------------------------------------------------------------------
bool P2pDisconnectionInterface::DisconnectSession( void )
{
  nn::nex::Session* pSession = nn::nex::Session::GetInstance();
  if( pSession == NULL )
  {// ない場合はSkip
    return true;
  }

  // @fix NMCat[4106]：Session切断待ち中にホストマイグレーションが発生して、子が親になると切断完了しないため、切断開始時の状態を保存
  m_isMaster = ( nn::nex::Session::GetRole() == nn::nex::SESSION_ROLE_MASTER );

  GFL_RELEASE_PRINT("P2pDisconnectionSessionStart: isMaster[%d] \n", m_isMaster);
  // Session離脱
  if( m_isMaster )
  {
    if( GetInitParam()->IsMigrationPowerOff() == false ){
      qBool ret = pSession->GameOver();
      GFL_PRINT("P2pDisconnection: GameOver\n");
      OHNO_PRINT("だれかがよんだ gameover\n");
      if( !ret ){
        // 失敗
        GFL_PRINT("P2pDisconnection: GameOver失敗\n");
        return false;
      }
    }
  }
  // 切断タイムアウト値
  m_destorySessionTimer = DESTROY_SESSION_TIME;
  return true;
}

//------------------------------------------------------------------
/**
* @brief   Session切断完了チェック
* @return  trueで完了
*/
//------------------------------------------------------------------
bool P2pDisconnectionInterface::IsDisconnectSession( void )
{
  nn::nex::Session* pSession = nn::nex::Session::GetInstance();
  if( pSession == NULL )
  {// ない場合はSkip
    return true;
  }

  //GFL_PRINT("P2pDisconnection: GetRole%d \n", nn::nex::Session::GetRole());
  if( m_isMaster )
  {
    if( GetInitParam()->IsMigrationPowerOff() == false ){
      nn::nex::Station::SelectionIterator itStations;
      if(itStations.Count() > 1)  {
        // 相手がまだ接続している
        return false;
      }
    }
    else{
      GFL_PRINT("P2pDisconnection: DestroySession\n");
      pSession->DestroySession();
    }
  }
  else
  {//切断タイムアウトチェック
    m_destorySessionTimer--;
    if( m_destorySessionTimer == 0 )
    {
      GFL_PRINT("P2pDisconnection: TIMEOUT\n");
      pSession->DestroySession();
    }
  }

  // Session終了チェック
  if( pSession->CheckGameIsOver() )
  {
    GFL_PRINT("P2pDisconnection: CheckGameOver\n");
    return true;
  }
  return false;
}

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif // GF_PLATFORM_CTR
