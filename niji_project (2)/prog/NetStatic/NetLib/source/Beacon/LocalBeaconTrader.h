#ifdef  GF_PLATFORM_CTR

//============================================================================================
/**
 * @file LocalBeaconTrader.h
 *
 * @author kousaka_shin
 */
//============================================================================================
#ifndef __LOCAL_BEACON_TRADER_
#define __LOCAL_BEACON_TRADER_
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>
#include <gflnet2/include/beacon/gflnet2_BeaconListener.h>
#include <gflnet2/include/beacon/gflnet2_WirelessBeaconSender.h>

#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

namespace gflnet2
{
  class InitParam;
  class NetWirelessCycleThread;
}

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Beacon)

class LocalBeaconListener;

//--------------------------------------------------------------------------------------------
/**
* @brief  ローカルビーコンシステム
*/
//--------------------------------------------------------------------------------------------
class LocalBeaconTrader : gflnet2::base::BeaconListener
{
public:

  enum
  {
    CONNECT_LIMIT_NUM = 4,//ダミーの接続人数
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ/デストラクタ
   */
  //------------------------------------------------------------------
  LocalBeaconTrader();
  ~LocalBeaconTrader();




public:

  //------------------------------------------------------------------
  /**
   * @brief   リスナー設定
   */
  //------------------------------------------------------------------
  void SetListener( LocalBeaconListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   有効化
   */
  //------------------------------------------------------------------
  void EnableBeacon( void );

  //------------------------------------------------------------------
  /**
   * @brief   無効化
   */
  //------------------------------------------------------------------
  void DisableBeacon( void );


  //------------------------------------------------------------------
  /**
   * @brief   前更新
   */
  //------------------------------------------------------------------
  void UpdateBefore( void );

  //------------------------------------------------------------------
  /**
   * @brief   後更新
   */
  //------------------------------------------------------------------
  void UpdateAfter( void );


  //------------------------------------------------------------------
  /**
    * @brief   ビーコンデータ設定
    */
  //------------------------------------------------------------------
  void SetBeaconData( const u8* pSendData, u32 size );

  //------------------------------------------------------------------
  /**
  * @brief  一時的に取得するビーコンのIDを設定
  */
  //------------------------------------------------------------------
  void SetTempBeaconID( u8 tempID );

  //------------------------------------------------------------------
  /**
   * @brief   ビーコンの開始
   */
  //------------------------------------------------------------------
  void StartBeacon( void );

  //------------------------------------------------------------------
  /**
   * @brief   ビーコンの停止(trueが返るまで呼ぶ)
   */
  //------------------------------------------------------------------
  bool UpdateStopBeacon( void );

  //------------------------------------------------------------------
  /**
   * @brief   ビーコンの再開
   */
  //------------------------------------------------------------------
  bool UpdateRestartBeacon( void );

private:

  //------------------------------------------------------------------
  /**
    * @brief   スレッドの終了確認＆終了
    */
  //------------------------------------------------------------------
  void CheckAndExitThread( void );

public:

  
  //------------------------------------------------------------------
  /**
  * @brief  ビーコン更新イベント
  *
  * @param  beacon 更新されたビーコン情報
  *                ※引き渡されたデータアドレス領域は永続保障されない。
  *                  ポインタを保持せず、自前のバッファにコピーすること。
  *
  * @param  localFriendCode ローカルフレンドコード。
  *                         フレンドユーザーのみ有効な値がセットされ、
  *                         その他のユーザーからのアクセス時には
  *                         NN_FRIENDS_INVALID_LOCAL_FRIEND_CODEがセットされる。
  *
  * @param  transferdId     送信者のtransferdId
  */
  //------------------------------------------------------------------
  virtual void OnUpdateBeacon( const void* beacon, u64 localFriendCode, u64 transferdId );

  //------------------------------------------------------------------
  /**
  * @brief  ビーコン削除前イベント
  *
  * @param  beacon 更新されるビーコン情報
  *                ※引き渡されたデータアドレス領域は永続保障されない。
  *                  ポインタを保持せず、自前のバッファにコピーすること。
  *
  *
  * @param  localFriendCode ローカルフレンドコード。
  *                         フレンドユーザーのみ有効な値がセットされ、
  *                         その他のユーザーからのアクセス時には
  *                         NN_FRIENDS_INVALID_LOCAL_FRIEND_CODEがセットされる。
  *
  * @param  transferdId     送信者のtransferdId
  */
  //------------------------------------------------------------------
  virtual void OnDeleteBeacon( const void* beacon, u64 localFriendCode, u64 transferdId );

private:

  bool m_isInitialized;
  gflnet2::NetWirelessCycleThread* m_pBeaconThread;
  gflnet2::base::BeaconManager m_beaconManager;
  gflnet2::wireless::WirelessBeaconSender m_beaconSender;
  gflnet2::InitParam m_initParam;
  LocalBeaconListener* m_pListener;
  bool m_isEnable;
};

GFL_NAMESPACE_END(Beacon)
GFL_NAMESPACE_END(NetLib)

#endif // __LOCAL_BEACON_TRADER_

#endif