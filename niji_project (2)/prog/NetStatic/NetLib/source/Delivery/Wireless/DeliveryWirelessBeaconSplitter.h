#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryWirelessBeaconSplitter.h
 * @brief  ワイヤレスビーコン分割クラス
 * @author Shin Kosaka
 * @date   2012.7.24
 */
//=============================================================================
#ifndef __GFLNET_DELIVERY_WIRELESS_BEACON_SPLITTER_H__
#define __GFLNET_DELIVERY_WIRELESS_BEACON_SPLITTER_H__

#include <nn.h>


#include "NetStatic/NetLib/source/Delivery/Wireless/DeliveryWirelessBeaconDefine.h"
#include <gflnet2/include/base/gflnet2_InitParam.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)

/***************************************************************************!
 * @brief  ワイヤレスビーコン分割クラス
 ***************************************************************************/
class DeliveryWirelessBeaconSplitter
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliveryWirelessBeaconSplitter); //コピーコンストラクタ＋代入禁止

public:
  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  * @note    署名機能はPM_DEBUG時のみ有効（おくりもの送信ツールでの使用を想定しており、署名処理及び秘密鍵が製品版に載らないように）
  */
  //------------------------------------------------------------------
  DeliveryWirelessBeaconSplitter( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam , const u16 deliveryID , const void* pSendData , const size_t dataSize, const bool bDoSign = false );

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~DeliveryWirelessBeaconSplitter();


  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  * @note   メインループ毎にポーリングすること
  */
  //------------------------------------------------------------------
  void Update();

  //------------------------------------------------------------------
  /**
  * @brief  分割数取得
  */
  //------------------------------------------------------------------
  u32 GetSplitNum();


  //------------------------------------------------------------------
  /**
  * @brief  分割データ取得
  */
  //------------------------------------------------------------------
  size_t GetSplitData( u32 index , void* pOutput );



private:
  gfl2::heap::HeapBase* 		m_pHeap;				//!< ヒープ
  gflnet2::InitParam* m_pInitParam;  ///< 通信初期化パラメータ
  size_t m_dataSize;
  bit8* m_pSendData;
  u32 m_deliveryID;
};

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)

#endif // __GFLNET_DELIVERY_WIRELESS_BEACON_SPLITTER_H__

#endif
