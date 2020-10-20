#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryRecvListener.h
 * @brief  配信システム受信リスナー
 * @author Shin Kosaka
 */
//=============================================================================

#ifndef __GFLNET_DELIVERY_RECV_LISTENER_H__
#define __GFLNET_DELIVERY_RECV_LISTENER_H__
#pragma once

#include <nn/Result.h>

#include <gflnet2/include/connect/gflnet2_AutoIrConnect.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)

//------------------------------------------------------------------
/**
 * @brief 配信システム受信リスナークラス
 *
 * @note 配信システムを利用して、受信処理関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class DeliveryRecvListener
{
public:

  //------------------------------------------------------------------
  /**
  * @brief 受信完了イベント
  *
  * @param pData 受信データ
  * @return 受信を続けるかどうか( true : 続ける , false : 続けない)
  */
  //------------------------------------------------------------------
  virtual bool OnRecvComplete( const void* pData , size_t dataSize ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief 受信経過更新イベント
  *
  * @param progress 受信経過（一番進んでいるものを%で表したもの）
  */
  //------------------------------------------------------------------
  virtual void OnUpdateProgress( const u32 progress ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief ニンテンドーゾーン接続通知
  */
  //------------------------------------------------------------------
  virtual void OnNintendoZoneConnected() = 0;

  //------------------------------------------------------------------
  /**
  * @brief ネットワークエラーイベント
  *
  * @param result 結果
  */
  //------------------------------------------------------------------
  virtual void OnNetworkError( const nn::Result &result , u32 errorCode ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief ネットワークエラーコード
  *
  * @param result 結果
  */
  //------------------------------------------------------------------
  virtual void OnWifiError( const u32 errorCode ) = 0;
};


//------------------------------------------------------------------
/**
 * @brief 配信ファイル用フィルター
 */
//------------------------------------------------------------------
class DeliveryFileListFilter
{
public:

  //------------------------------------------------------------------
  /**
  * @brief 受け取るファイルを決定する
  */
  //------------------------------------------------------------------
  virtual s32 CheckFileList( const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief シリアルIDの範囲取得（最大）
  */
  //------------------------------------------------------------------
  virtual u32 GetSerialIdMax( void ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief シリアルIDの範囲取得（最少）
  */
  //------------------------------------------------------------------
  virtual u32 GetSerialIdMin( void ) = 0;
};



GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)



#endif // __GFLNET_DELIVERY_RECV_LISTENER_H__
#endif
