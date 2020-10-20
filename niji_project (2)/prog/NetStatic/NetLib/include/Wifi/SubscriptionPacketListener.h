#if defined(GF_PLATFORM_CTR)
#ifndef __SUBSCRIPTION_PACKET_LISTENER_H__
#define __SUBSCRIPTION_PACKET_LISTENER_H__
#pragma once

//=============================================================================
/**
 * @file   SubscriptionPacketListener.h
 * @author suzuki_hisanao
 */
//=============================================================================
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Wifi)

/**
 * @brief  Subscriptionサーバからのパケット受信通知用イベントリスナ
 *
 * @note   Subscriptionサーバからの通知を受け取る場合は、本クラスを継承し仮想関数を実装してください。
 */
class SubscriptionPacketListener
{
  //GFL_FORBID_COPY_AND_ASSIGN(SubscriptionPacketListener); //コパケットストラクタ＋代入禁止
public:
  //------------------------------------------------------------------
  /**
  * @brief  パケット更新通知イベント(from Subscription)
  *
  * @note SubscriptionClient経由で通知されたパケットデータ受信時にコールされます。
  *
  * @param[in]  playerData         更新されたデータの実体
  * @param[in]  principalId        データが更新されたユーザのプリンシバルＩＤ
  * @param[in]  isOnline           オンライン状態ならtrueを指定
  * @param[in]  isUpdatingStranger 通りすがりの更新通知であればtrueを指定
  */
  //------------------------------------------------------------------
  virtual void OnUpdateSubscription( nn::nex::SubscriptionData& playerData, u32 principalId,
                                     bool isOnline, bool isUpdatingStranger ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  パケット更新通知イベント(from MessageClient)
  *
  * @note MessageClient経由で通知されたパケットデータ受信時にコールされます。
  *
  * @param[in]  packetData  更新されたパケットデータの実体
  */
  //------------------------------------------------------------------
  virtual void OnUpdateMessagePacket( void* packetData ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  ログアウト通知イベント
  *
  * @param[in] principalId ログアウトしたユーザのプリンシバルＩＤ
  */
  //------------------------------------------------------------------
  virtual void OnLogoutSubscription( u32 principalId ) = 0;
};


GFL_NAMESPACE_END(Wifi)
GFL_NAMESPACE_END(NetLib)



#endif // __SUBSCRIPTION_PACKET_LISTENER_H__
#endif