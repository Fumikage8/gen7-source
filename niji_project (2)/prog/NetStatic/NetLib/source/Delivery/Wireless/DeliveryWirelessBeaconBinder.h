#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryWirelessBeaconBinder.h
 * @brief  ワイヤレスビーコン結合クラス
 * @author Shin Kosaka
 * @date   2012.7.24
 */
//=============================================================================
#ifndef __GFLNET_DELIVERY_WIRELESS_BEACON_BINDER_H__
#define __GFLNET_DELIVERY_WIRELESS_BEACON_BINDER_H__

#include "NetStatic/NetLib/source/Delivery/Wireless/DeliveryWirelessBeaconDefine.h"
#include <NetStatic/NetLib/include/Delivery/DeliveryRecvListener.h>
#include <gflnet2/include/base/gflnet2_InitParam.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)

/***************************************************************************!
 * @brief  ワイヤレスビーコン結合クラス
 ***************************************************************************/
class DeliveryWirelessBeaconBinder : public gflnet2::base::BeaconListener
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliveryWirelessBeaconBinder); //コピーコンストラクタ＋代入禁止
public:


  enum
  {
    MAX_BEACON_SLOT_NUM = 16,//同時に受信できるタイプは16種類まで
    COMPLETE_PROGRESS = 100,//完成したら100%
    RECV_BIT_SIZE = 4,//32バイト*4で128分割まで管理
  };

private:

  typedef struct
  {
    u16 deliveryID;		//配信データID
    s8 splitNum;		//分割数（データがいくつに分かれているか）
    s8 recvNum;			//いくつまで受信したか
    bit32 recvBit[RECV_BIT_SIZE];		//受信フラグ
    bit8 beaconSlot[MAX_SPLIT_NUM][SPLIT_BEACON_BODY_SIZE];
  } WIRELESS_BEACON_SLOT_DATA;
public:
  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  DeliveryWirelessBeaconBinder( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam, const bool bDoVerifySign = false );

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~DeliveryWirelessBeaconBinder();


  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  * @return 受信終了か (true : 終了　、false : 継続)
  * @note   メインループ毎にポーリングすること
  */
  //------------------------------------------------------------------
  bool Update( DeliveryRecvListener* pRecvListener );


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

  //------------------------------------------------------------------
  /**
   * @brief   ビーコン保存処理
   */
  //------------------------------------------------------------------
  void AddBeaconDataInternal( const void* pBeacon );

private:
  gfl2::heap::HeapBase* 		m_pHeap;				//!< ヒープ
  u32 						m_lastProgress;			//!< 最新の進捗（%）
  bool						m_isFinish;				//!< 受信終了したかどうか
  bool            m_bDoVerifySign;
  nn::os::CriticalSection 	m_criticalSectionForBeadonData;
  gflnet2::InitParam* m_pInitParam;  ///< 通信初期化パラメータ
  WIRELESS_BEACON_SLOT_DATA 	m_beaconDataBuffer[MAX_BEACON_SLOT_NUM];
  bit8 m_uncompressBuf[MAX_SPLIT_NUM][SPLIT_BEACON_BODY_SIZE];
};

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)

#endif // __GFLNET_DELIVERY_WIRELESS_BEACON_BINDER_H__

#endif
