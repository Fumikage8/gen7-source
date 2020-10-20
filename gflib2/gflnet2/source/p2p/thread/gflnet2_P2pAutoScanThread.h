#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_P2PAUTOSCANTHREAD_H__
#define __GFLNET_P2PAUTOSCANTHREAD_H__

#pragma once
//=============================================================================================
/**
 * @file	 gflnet_P2pAutoScanThread.h
 * @brief	 ワイヤレス通信 ビーコン収集
 * @date	 2012.6.24
 */
//=============================================================================================
#include <pia/local.h>

#include <gflnet2/include/base/gflnet2_NetThread.h>
#include <gflnet2/include/base/gflnet2_Yield.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

class P2pManager;

class P2pAutoScanThread : public gflnet2::base::NetThread {
  GFL_FORBID_COPY_AND_ASSIGN( P2pAutoScanThread ); // コピーコンストラクタ, 代入演算子を禁止

private:
//  gflnet2::Base* mpNetBase;  // 無線通信の元クラス

  P2pManager* mpP2pManager;
  
  u8* mpBeaconScanBuffer;  // スキャン時に一度に探す数
  u8 mTempBuff[nn::uds::NET_DESC_APPDATA_SIZE_MAX];  //一時的に読み込むバッファ
  gflnet2::base::Yield* mpYield;  //スレッドの権限を譲渡する関数

  bool m_isSuccess;

  static const int BEACON_SEND_MSEC = 800; ///< ビーコンを送るほうは受け取るほうの1/2程度が望ましい
  static const int BEACON_RECV_MSEC = BEACON_SEND_MSEC * 2; ///< ビーコンを受け取るのは倍

  static const int BEACON_SCAN_BUFF = 1024*16; //スキャン時に一度に探す数 一個1K


  
public:


  //------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //------------------------------------------------------------------------------------
  P2pAutoScanThread( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //------------------------------------------------------------------------------------
  virtual ~P2pAutoScanThread(){FinalizeInternal();};


private:

  //----------------------------------------------------------------------------
  /**
   * @brief メイン処理
   * ※スレッド関数のループ内で呼ばれ続ける関数です。
   */
  //----------------------------------------------------------------------------
  virtual void Function( void );

  void FinalizeInternal( void );

  //----------------------------------------------------------------------------
  /**
   * @brief  ビーコン格納バッファの指定
   * @param  pData   データ
   * @param  num     数
   * @param  size    サイズ
   */
  //----------------------------------------------------------------------------
  void SetBeaconDataBuff(u8** pData, int num, int size);
  //------------------------------------------------------------------
  /**
   * @brief	  ポーズ処理
   */
  //------------------------------------------------------------------
  void PauseCheckFunc( void );
  void Participate(nn::pia::local::UdsGatheringHolder* pGathering);
public:
  void SetP2pManager(P2pManager* pP2pManager);
  bool IsSuccess(void){ return m_isSuccess; };

};

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)


#endif// __GFLNET_P2PAUTOSCANTHREAD_H__
#endif
