#ifdef  GF_PLATFORM_CTR
//======================================================================
/**
 * @file   gflnet_NexMatchMakingNotificationListener.h
 * @author suzuki_hisanao
 */
//======================================================================
#ifndef __NEX_MATCHMAKING_NOTIFICATION_LISTENER_H__
#define __NEX_MATCHMAKING_NOTIFICATION_LISTENER_H__

#pragma once


#include <nex.h>



namespace gflnet2{
namespace nex{

class NexMatchMakingClient;

//--------------------------------------------------------------------------------------------
/**
* @brief NEXマッチメイクイベント管理用クラス。<p>
*        NexMatchMakingClientクラス内部でのみ利用されます。
*/
//--------------------------------------------------------------------------------------------
class NexMatchMakingNotificationListener : public nn::nex::NotificationEventHandler
{
public:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief    コンストラクタ
  */
  //--------------------------------------------------------------------------------------------
  NexMatchMakingNotificationListener( void );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief    デストラクタ
  */
  //--------------------------------------------------------------------------------------------
  virtual ~NexMatchMakingNotificationListener();

public:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief    イベントが通知されたときに呼ばれるコールバック関数。
  *
  * @note     イベントハンドラの為、ユーザーの使用は禁止
  *
  * @param[in]    oEvent    通知イベントオブジェクト
  */
  //--------------------------------------------------------------------------------------------
  virtual void ProcessNotificationEvent( const nn::nex::NotificationEvent& oEvent );
};



} // (nex)
} // (gflnet)





#endif  // __NEX_MATCHMAKING_NOTIFICATION_LISTENER_H__
#endif
