#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_BASE_H__
#define __GFLNET_BASE_H__
//=============================================================================
/**
 * @file	 gflnet_Base.h
 * @brief	 通信基底クラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================
#pragma once

#include <nn.h>
#include <nn/uds.h>
#include <nex_P2p.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include "gflnet2/include/beacon/gflnet2_BeaconListener.h"
#include "gflnet2/include/beacon/gflnet2_Beacon.h"
#include "gflnet2/include/beacon/gflnet2_BeaconManager.h"

#include "gflnet2/include/base/gflnet2_InitParam.h"  //初期化構造体とアクセサ

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)



//static const u32 IRRECEIVE_BUFFER = (4096*6);  ///< 2048最大で送るための設計
static const u32 UDSRECEIVE_BUFFER = (4096*20);  ///< SampleDemos/uds/simple/uds_Main.cppから持ってきたサイズ
static const u32 UDSRECEIVE_ALIGN = 4096;  ///< udsとirが要求するバッファ

static const u32 NETBASE_STACK_SIZE = 4096;  ///< スタックサイズ
//static const gfl2::thread::Thread::StackType NETBASE_STACK_TYPE = gfl2::thread::Thread::STACK_TYPE_SAFE;



class Base {

  GFL_FORBID_COPY_AND_ASSIGN(Base);
private:

  InitParam mInitParam;  ///< 通信初期化パラメータ

private:
  bit32 mLocalCommunicationId;     ///<  CreateLocalCommunicationId から得られたID

  nn::Result mResult;  ///< 通信関数の戻り値

  const u8* mpBeaconAddress;  ///< ビーコンのデータのポインタ
  u32 mBeaconSize;  ///< ビーコンデータサイズ

  u8* mpRecvBeaconAddress;   ///< 受信用ビーコンアドレス
  bool bBeaconSetSuccess; ///< ビーコンを送ったかどうか

  void* ReceiveBuffer;  //受信バッファは4096アライメントのデバイスバッファ


  nn::os::Event maReceiveEvent;  //イベント
  nn::os::Event maSendEvent;     //イベント
  nn::os::Event maConnectEvent;  //イベント  //nn::os::Event maStatusUpdateEvent;  //イベント

  gflnet2::base::BeaconManager* mpBeaconManager;  //ビーコンマネージャー


public:


public:

  //------------------------------------------------------------------
  /**
   * @brief	  NetBaseコンストラクタ＆デストラクタ
   * @return
   */
  //------------------------------------------------------------------
  Base(const InitParameter* pIParam, const InitParameterFixed* pIParamFix);
  virtual ~Base();


  void SetBeaconManager(gflnet2::base::BeaconManager* pBeaconManager){ mpBeaconManager = pBeaconManager; };
  gflnet2::base::BeaconManager* GetBeaconManager(void){ return mpBeaconManager; };


  void MainCheckRecvBeacon(void);

public:


  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンをセットする
   * @param   pData  ビーコンデータ
   * @param   size   ビーコンサイズ
   * @return  成功したらtrue
   */
  //------------------------------------------------------------------
  virtual b32 SetBeacon(const u8* pData, const u32 size) = 0;

  //------------------------------------------------------------------
  /**
   * @brief	  接続人数をかえす
   * @return  接続人数
   */
  //------------------------------------------------------------------
  virtual u8 GetConnectNum(void) const = 0;

  //------------------------------------------------------------------
  /**
   * @brief	   ビーコンを返す
   * @param    index データ
   * @param    ビーコンのサイズ
   * @return   ビーコンデータ
   */
  //------------------------------------------------------------------
  virtual void* GetRecvBeacon(const int index,int size) = 0;

  //------------------------------------------------------------------
  /**
   * @brief	  任天堂のシステムが要求する 4K単位のアライメントの 4K単位のバッファを返す
   * @return  アドレス
   */
  //------------------------------------------------------------------
  static u8* GetSystemBaseBufferAddress(void);

  //------------------------------------------------------------------
  /**
   * @brief	  初期化パラメータクラスを得る
   * @return  初期化パラメータクラス
   */
  //------------------------------------------------------------------
  InitParam* GetInitParam(void);

  inline gfl2::heap::HeapBase* GetHeapBase(void){ return GetInitParam()->GetHeapBase(); };

  inline nn::os::Event* GetRecvEvent(void){ return &maReceiveEvent; };
  inline nn::os::Event* GetSendEvent(void){ return &maSendEvent; };
  inline nn::os::Event* GetConnectEvent(void){ return &maConnectEvent; };

  inline void* GetReceiveBuffer(void){ return ReceiveBuffer; };

  inline u32 GetReceiveBufferSize(void) const { return UDSRECEIVE_BUFFER; };

};

GFL_NAMESPACE_END(gflnet2)
#endif // __GFLNET_BASE_H__
#endif
