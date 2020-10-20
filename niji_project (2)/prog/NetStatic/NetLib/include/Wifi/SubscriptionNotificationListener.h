#if defined(GF_PLATFORM_CTR)
//======================================================================
/**
 * @file  SubscriptionNotificationListener.h
 *
 * @author  suzuki_hisanao
 */
//======================================================================
#ifndef __SUBSCRIPTION_NOTIFICATION_LISTENER_H__
#define __SUBSCRIPTION_NOTIFICATION_LISTENER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <nex.h>
#include <nn/friends.h>
#include <nn/version.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Wifi)


/**
 * @brief ゲームサーバからの通知イベントを検知するためのリスナクラス。<p>
 *        本クラスはSubscriptionManager内でのみ利用されます。
 */
class SubscriptionNotificationListener : public nn::nex::NotificationEventHandler
{
public:
  SubscriptionNotificationListener( void );
  ~SubscriptionNotificationListener();

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 更新イベントで取得した情報一式を引数リストに上書きする
   *
   * @note  本関数実行中は、NEXのシステムスレッドはロックされる
   *
   * @param[in] eventList 格納先イベントリスト
   *
   * @return 通知された情報が存在するならtrue、通知情報が存在しないならfalse
   */
  //--------------------------------------------------------------------------------------------
  nn::nex::qBool CopyNotification( nn::nex::qList<nn::nex::NotificationEvent>& eventList );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 更新イベントで取得した情報一式を引数リストにpush_backする。
   *
   * @note  本関数実行中は、NEXのシステムスレッドはロックされる
   *
   * @param[in] eventList 格納先イベントリスト
   *
   * @return 通知された情報が存在するならtrue、通知情報が存在しないならfalse
   */
  //--------------------------------------------------------------------------------------------
  qBool PushbackNotification( nn::nex::qList<nn::nex::NotificationEvent>& eventList );

public:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief    イベントが通知されたときに呼ばれるコールバック関数
  *
  * @caution    イベントハンドラの為、ユーザーの使用は禁止
  *
  *  @param[in]    oEvent    通知イベントオブジェクト
  */
  //--------------------------------------------------------------------------------------------
  virtual void ProcessNotificationEvent( const nn::nex::NotificationEvent& oEvent );

private:
  nn::nex::qList<nn::nex::NotificationEvent> m_eventList;   // 通知されたイベントのリスナ

};



GFL_NAMESPACE_END(Wifi)
GFL_NAMESPACE_END(NetLib)




#endif  // __SUBSCRIPTION_NOTIFICATION_LISTENER_H__
#endif
