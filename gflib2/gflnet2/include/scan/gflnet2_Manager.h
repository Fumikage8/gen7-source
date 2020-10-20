﻿#ifdef  GF_PLATFORM_CTR
#ifndef __GFL_NETMANAGER_H__
#define __GFL_NETMANAGER_H__
//=============================================================================
/**
 * @file	 gflnet_Manager.h
 * @brief	 通信管理クラス
           マネージャーは通信ひとつにつき一個のつもりで設計してある
           赤外線、無線を同時に走らせる場合２こ必要
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================
#pragma once

#include <nn/uds.h>
#include <gflnet2/include/base/gflnet2_Base.h>
#include <gflnet2/include/beacon/gflnet2_WirelessBeaconSender.h>
#include <gflnet2/include/base/gflnet2_TransferedIDReceiver.h>
#include <gflnet2/include/base/gflnet2_AccessPointHistoryBase.h>
#include <gflnet2/include/base/gflnet2_AccessPointScannerBase.h>
#include <gflnet2/include/base/gflnet2_BaseProcess.h>
#include <gflnet2/include/base/gflnet2_NetStateListener.h>

#include <proc/include/gfl2_ProcManager.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)  





enum{
  NETPROC_TYPE_IDLE,
  NETPROC_TYPE_ACTIVE,
};


class Wireless;
class Ir;


class Manager : public gfl2::proc::Manager{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);


private:

  gflnet2::Base* pNetBase; ///< 基底クラスを保存しておく変数 今のところ一個だけしか起動できないようにしてある
  Wireless* mpNetW;
  Ir* mpIr;

  gflnet2::base::BeaconManager* mpBeaconManager;
  gflnet2::BaseProcess* mpBaseProc;
  
  gflnet2::wireless::WirelessBeaconSender* mpBeaconSender;
  

  gflnet2::NetStateListener*  mpNetStateListener; //状態遷移を伝えるリスナー

  gflnet2::base::TransferedIDReceiver* mpTransferedIDReceiver;

  gflnet2::base::AccessPointScannerBase* mpAccessPointScanner;
  gflnet2::base::AccessPointHistoryBase* mpAccessPointHistory;

  bool bEndSignal;

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ＆デストラクタ
   * @param   pHeapBase  ヒープクラス
   */
  //------------------------------------------------------------------
  Manager(gfl2::heap::HeapBase* pHeapBase);
  virtual ~Manager();

public:
  inline void SetBeaconManager(gflnet2::base::BeaconManager *pBeaconManager){ mpBeaconManager = pBeaconManager; };
  inline gflnet2::base::BeaconManager* GetBeaconManager(void){ return mpBeaconManager; };
  //------------------------------------------------------------------
  /**
   * @brief	  マネージャーで管理しているネットワーククラスを返す
   * @return  ネットワーク基底クラス
   */
  //------------------------------------------------------------------
  inline gflnet2::Base* GetNetBase(void){ return pNetBase; };
  //------------------------------------------------------------------
  /**
   * @brief	  通信初期化パラメータに対応した通信クラスを作成する
   * @param   pIParam   通信初期化パラメータ
   */
  //------------------------------------------------------------------
  void CreateNetwork(const gflnet2::InitParameter* pIParam,
                     const gflnet2::InitParameterFixed* pIParamFix,
                     gflnet2::base::BeaconManager* pBeaconManager,
                     gflnet2::base::AccessPointScannerBase* pAccessPointScanner=NULL,
                     gflnet2::base::AccessPointHistoryBase* pAccessPointHistory=NULL);
  //------------------------------------------------------------------
  /**
   * @brief	  通信状態を変更させる
   * @param   no  通信スレッド番号
   */
  //------------------------------------------------------------------
//  void ChangeNetwork(const gflnet2::EN_NET_THREAD no);
  //------------------------------------------------------------------
  /**
   * @brief	  通信終了をマネージャーに通知する
   */
  //------------------------------------------------------------------
  void Finalize(void);
  //------------------------------------------------------------------
  /**
   * @brief	  通信終了をマネージャーに通知する
   */
  //------------------------------------------------------------------
  void DeleteNetwork(void);
  //------------------------------------------------------------------
  /**
   * @brief	  ステートを変更する処理
   */
  //------------------------------------------------------------------
  gfl2::proc::MainStatus MainProcess(void);
  //void StateProcess(void);
  //------------------------------------------------------------------
  /**
   * @brief	  通信終了準備開始
   */
  //------------------------------------------------------------------
  bool EndingNetwork(void);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンのマックアドレスを得る
   * @param   index  受け取ったビーコンのindex
   * @return  マックアドレス
   */
  //------------------------------------------------------------------
//  bit64 GetRecvBeaconTransferedID(const int index);

//  bool CheckRecvBeaconCallTransferedID(const int index, const bit64 mytrans);

  //------------------------------------------------------------------
  /**
   * @brief	  条件
   * @param   index  受け取ったビーコンのindex
   * @return  マックアドレス
   */
  //------------------------------------------------------------------
  inline void SetManagerListener(NetStateListener* pNetStateListener){ mpNetStateListener = pNetStateListener; }
  inline NetStateListener* GetManagerListener(void){ return mpNetStateListener; }


  //------------------------------------------------------------------
  /**
   * @brief	  条件
   * @param   index  受け取ったビーコンのindex
   * @return  マックアドレス
   */
  //------------------------------------------------------------------
  inline void SetTransferedIDReceiver( gflnet2::base::TransferedIDReceiver* pTransferedIDReceiver){mpTransferedIDReceiver = pTransferedIDReceiver;};

private:
  bool IsProcEnd(void);
  
};

GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET_MANAGER_H__
#endif
