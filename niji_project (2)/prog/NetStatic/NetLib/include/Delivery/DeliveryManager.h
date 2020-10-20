#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryManager.h
 * @brief  ビーコン配信マネージャクラス
 * @author Shin Kosaka
 * @date   2012.7.23
 */
//=============================================================================
#ifndef __GFLNET_DELIVERY_MANAGER_H__
#define __GFLNET_DELIVERY_MANAGER_H__
#pragma once

#include <nn.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <base/include/gfl2_Singleton.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <gflnet2/include/base/gflnet2_InitParam.h>
#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include "NetStatic/NetLib/source/delivery/Wireless/DeliveryWirelessSendThread.h"
#include "NetStatic/NetLib/source/delivery/Wireless/DeliveryWirelessRecvThread.h"

#include "NetStatic/NetAppLib/include/Util/NetAppBossUtility.h"

#include <NetStatic/NetLib/include/Delivery/DeliveryRecvListener.h>

GFL_NAMESPACE_BEGIN(NetLib)
/**
* @namespace NetAppLib::delivery
* @brief     ビーコン配信関連
*/
GFL_NAMESPACE_BEGIN(Delivery)

//--------------------------------------------------------------------------
class DeliverySendThread;
class DeliveryRecvThread;
class DeliverySendListener;
class DeliveryRecvListener;
class DeliverySystem;
class DeliveryFileListFilter;
class DeliveryIRPowerManager;
//--------------------------------------------------------------------------


/***************************************************************************!
 * @brief  配信管理マネージャクラス
 ***************************************************************************/
class DeliveryManager : public gfl2::base::Singleton<DeliveryManager> , public DeliveryFileListFilter
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliveryManager); //コピーコンストラクタ＋代入禁止

public:

  typedef enum
  {//配信タイプ
    DELIVERY_TYPE_wireless = 0,	//無線
    DELIVERY_TYPE_wifi,			//Wifi
    DELIVERY_TYPE_ir,			//赤外線
  }DELIVERY_TYPE;

private:
  enum
  {
    THREAD_STACK_SIZE = 8192,
    ATTRIBUTE_INDEX_MAX = 2,
    MAX_FILE_NUM = 256,
  };

public:
  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  DeliveryManager( void );

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~DeliveryManager();

  //------------------------------------------------------------------
  /**
  * @brief  DeliveryManagerの初期化
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result Initialize( 	const gflnet2::InitParameter* pIParam,
              const gflnet2::InitParameterFixed* pIParamFix,
              const DELIVERY_TYPE type );

  //------------------------------------------------------------------
  /**
  * @brief  DeliveryManagerの終了処理
  * 			スレッドが起動中だと停止までブロックされます
         事前にPrepareFinalizeしてIsCompretePrepareFinalizeからtrueが返るのを待ってください
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result Finalize( void );

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   メインループ毎にポーリングすること
  */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   送信開始
   */
  //------------------------------------------------------------------
  bool StartSend( const int threadPriority , const u16 deliveryID , const void* pSendData , const size_t dataSize );


  //------------------------------------------------------------------
  /**
   * @brief   受信開始
   */
  //------------------------------------------------------------------
  bool StartRecv( const int threadPriority , bool isWifiConnected = false);

  //------------------------------------------------------------------
  /**
   * @brief   シリアルコード設定
   */
  //------------------------------------------------------------------
  void SetSerialCode( const u32 serialCode );

  //------------------------------------------------------------------
  /**
   * @brief  終了準備
   */
  //------------------------------------------------------------------
  void PrepareFinalize( void );

  //------------------------------------------------------------------
  /**
   * @brief  停止確認
   */
  //------------------------------------------------------------------
  bool IsCompretePrepareFinalize( void );

public:

  //------------------------------------------------------------------
  /**
   * @brief   送信リスナー設定
   */
  //------------------------------------------------------------------
  void SetSendListener( DeliverySendListener* pSendListener );
  //------------------------------------------------------------------
  /**
   * @brief   送信リスナー解除
   */
  //------------------------------------------------------------------
  void RemoveSendListener();
  //------------------------------------------------------------------
  /**
   * @brief   受信リスナー設定
   */
  //------------------------------------------------------------------
  void SetRecvListener( DeliveryRecvListener* pRecvListener );
  //------------------------------------------------------------------
  /**
   * @brief   受信リスナー解除
   */
  //------------------------------------------------------------------
  void RemoveRecvListener();

  //------------------------------------------------------------------
  /**
  * @brief  初期化状態を調査
  *
  * @retval true： 初期済み
  * @retval false: 未初期化・または初期化中
  */
  //------------------------------------------------------------------
  bool IsInitialized( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief  初期化パラメータの取得
   */
  //------------------------------------------------------------------
  gflnet2::InitParam* GetInitParam( void );


  //------------------------------------------------------------------
  /**
    *  @brief    UDS送受信時に署名機能を使用するかを設定する
    *  @note     Final時は送信時の署名が強制的に無効となる
    */
  //------------------------------------------------------------------
  void  SetUDSSignatureMode(const bool bDoSigning, const bool bDoVerifySign) {m_bDoUDSSigning = bDoSigning;   m_bDoUDSVerifySign = bDoVerifySign;}


  //------------------------------------------------------------------
  /**
   * @brief   検索属性の削除（Wifi用）
   */
  //------------------------------------------------------------------
  void ClearAttribute( s32 attributeIndex );

private:
  //↓公開関数から非公開にしたもの
  //------------------------------------------------------------------
  /**
   * @brief   検索属性の設定（Wifi用）
   */
  //------------------------------------------------------------------
  void SetAttribute( const gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES* pAttributes );

  //------------------------------------------------------------------
  /**
   * @brief   検索用フィルターの設定（Wifi用）
   */
  //------------------------------------------------------------------
  void SetFileListFilter( DeliveryFileListFilter* pFilter );
  //↑公開関数から非公開にしたもの

  //非同期通信が実行中かどうかを調べる
  bool CheckAsyncInternal( void ) const;
  //実行中の非同期通信をキャンセルする
  void CancelAsyncInternal( void );
  //スレッドの終了待ちを行います。
  void WaitAsyncFinishedInternal( void );
  //非同期処理スレッドを破棄する
  void DeleteAsyncInternal( void );

private:

    //------------------------------------------------------------------
  /**
  * @brief 受け取るファイルを決定する
  */
  //------------------------------------------------------------------
  virtual s32 CheckFileList( const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum );

  //------------------------------------------------------------------
  /**
  * @brief シリアルIDの範囲取得（最大）
  */
  //------------------------------------------------------------------
  virtual u32 GetSerialIdMax( void );

  //------------------------------------------------------------------
  /**
  * @brief シリアルIDの範囲取得（最少）
  */
  //------------------------------------------------------------------
  virtual u32 GetSerialIdMin( void );

private:
  bool                 	m_isInit;            	//!< 初期済みフラグ
  DeliveryWirelessSendThread* m_pSendThread;
  DeliveryWirelessRecvThread* m_pRecvThread;

  DELIVERY_TYPE			m_deliveryType;			//!< 配信タイプ（無線、Wifi、赤外線）
  gfl2::heap::HeapBase* 	m_pHeap;				//!< ヒープ

  DeliverySendListener*	m_pSendListener;		//!< 送信イベントリスナー
  DeliveryRecvListener*	m_pRecvListener;		//!< 受信イベントリスナー

  DeliverySystem*			m_pDeliverySystem;

  gflnet2::InitParam m_InitParam;  ///< 通信初期化パラメータ
  gflnet2::InitParameter m_InitParameter;//!<アプリ固有通信初期化パラメータ（InitParamに入ってるが、個別でも必要）
  gflnet2::InitParameterFixed m_InitParameterFixed;//!<タイトル固有通信初期化パラメータ（InitParamに入ってるが、個別でも必要）

  gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES m_attributes;
  DeliveryFileListFilter* m_pFilter;

  //niji追加分
//  const wchar_t* m_pSerialCode;
  u32  m_SerialCode;
  char m_attribute1[NetAppLib::Util::BossUtility::ATTRIBUTE_BUFFER_SIZE];
  char m_attribute2[NetAppLib::Util::BossUtility::ATTRIBUTE_BUFFER_SIZE];

  /*  署名  */
  bool  m_bDoUDSSigning;      /**<  送信時署名      */
  bool  m_bDoUDSVerifySign;   /**<  受信時署名照合  */

};

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)

#endif // __GFLNET_DELIVERY_MANAGER_H__

#endif
