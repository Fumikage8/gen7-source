#ifdef  GF_PLATFORM_CTR
//======================================================================
/**
 * @file	gflnet_NexMatchMakingNotificationListener.cpp
 * @author	suzuki
 */
//======================================================================
#include "gflnet2_NexMatchMakingNotificationListener.h"
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
namespace gflnet2{
namespace nex{


//--------------------------------------------------------------------------------------------
/**
* @brief    コンストラクタ
*
* @param[in] client マッチメイククライアント
*/
//--------------------------------------------------------------------------------------------
NexMatchMakingNotificationListener::NexMatchMakingNotificationListener( void ) : nn::nex::NotificationEventHandler()
{
}


//--------------------------------------------------------------------------------------------
/**
* @brief    デストラクタ
*
* @param    none
*/
//--------------------------------------------------------------------------------------------
NexMatchMakingNotificationListener::~NexMatchMakingNotificationListener()
{
}


//--------------------------------------------------------------------------------------------
/**
* @brief    イベントが通知されたときに呼ばれるコールバック関数
*           イベントハンドラの為、ユーザーの使用は禁止
*
*  @param[in]    oEvent    通知イベントオブジェクト
*/
//--------------------------------------------------------------------------------------------
void NexMatchMakingNotificationListener::ProcessNotificationEvent(const nn::nex::NotificationEvent &oEvent)
{
  NexMatchMakingClient*    matchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);

  if( matchMakeClient == NULL ){
    return;
  }
  NexSessionEventListener* sessionListener = matchMakeClient->GetSessionEventListener();
  NexMatchmakeListener*    matchListener   = matchMakeClient->GetMatchmakeListener();


  // メッセージ毎の処理
  switch( oEvent.GetType() )
  {
    case nn::nex::NotificationEvents::ParticipationEvent:
      if( sessionListener != NULL )
      {
        switch( oEvent.GetSubType() )
        {
          case nn::nex::ParticipationEvents::Participate:
            sessionListener->OnSessionParticipate( oEvent.GetParam2(), oEvent.GetParam1() );
            GFL_PRINT("PARTICIPATION_EVENT PARTICIPATE: [PID %u] join [session %u]\n", oEvent.GetParam2(), oEvent.GetParam1());
            break;

          case nn::nex::ParticipationEvents::EndParticipation:
            sessionListener->OnSessionEndParticipation( oEvent.GetParam2(), oEvent.GetParam1() );
            GFL_PRINT("PARTICIPATION_EVENT END: [PID %u] end [session %u]\n", oEvent.GetParam2(), oEvent.GetParam1());
            break;

          case nn::nex::ParticipationEvents::Disconnect:
            sessionListener->OnSessionDisconnect( oEvent.GetParam2(), oEvent.GetParam1() );
            GFL_PRINT("PARTICIPATION_EVENT DISCONNECT: [PID %u] disconnect [session %u]\n", oEvent.GetParam2(), oEvent.GetParam1());
            break;
        }
      }
      break;

    case nn::nex::NotificationEvents::OwnershipChangeEvent:
      if( matchListener != NULL ){
        matchListener->OnMatchmakingOwnerChange( oEvent.GetParam2() );
      }
      GFL_PRINT("NOTIFICATION_EVENT OWNERSHIPCHANGE: [session %u] changed owner to [PID %u]\n", oEvent.GetParam1(), oEvent.GetParam2());
      // 新しいオーナーの PID を記録
      matchMakeClient->SetOwner( oEvent.GetParam2() );
      break;

    case nn::nex::NotificationEvents::GatheringUnregistered:
      if( matchListener != NULL ){
        matchListener->OnMatchmakingUnregisterd( oEvent.GetParam1() );
      }
      GFL_PRINT("NOTIFICATION_EVENT GATHERINGUNREGISTERED: [session %u] unregistered\n", oEvent.GetParam1() );
      break;
  }
}



} // nex
} // gflnet


#endif
