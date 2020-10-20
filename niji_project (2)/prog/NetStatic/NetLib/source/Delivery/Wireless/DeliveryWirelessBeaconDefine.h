#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryWirelessBeaconDefine.h
 * @brief  ワイヤレスビーコン定義
 * @author Shin Kosaka
 * @date   2012.7.24
 */
//=============================================================================
#ifndef __GFLNET_DELIVERY_WIRELESS_BEACON_DEFINE_H__
#define __GFLNET_DELIVERY_WIRELESS_BEACON_DEFINE_H__

#include <nn.h>

#include <gflnet2/include/beacon/gflnet2_Beacon.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)


typedef struct
{
  u16 deliveryID;			//配信データID（配信するものごとにユニークである必要がある。同一の場所に存在してなければ重複可能だが、できればユニークであることが望ましい）
  s8 splitNum;			//分割数（データがいくつに分かれているか）
  s8 splitIndex;			//分割番号（何番目のデータか）
} WIRELESS_BEACON_DATA;

enum
{
  SPLIT_BEACON_BODY_SIZE = gflnet2::base::BEACON_CONNECT_MAXBYTE - sizeof(WIRELESS_BEACON_DATA),
  MAX_SPLIT_NUM = 128,//128分割まで(20キロバイトぐらい？)
  //MAX_SPLIT_NUM = 32,//32分割まで(3.2キロバイトぐらい？)
};


GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)

#endif // __GFLNET_DELIVERY_WIRELESS_BEACON_DEFINE_H__

#endif
