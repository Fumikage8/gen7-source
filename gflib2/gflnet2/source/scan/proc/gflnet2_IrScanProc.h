#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_IRSCANPROC_H__
#define __GFLNET_IRSCANPROC_H__
//=============================================================================================
/**
 * @file	 gflnet_IrScanProc.h
 * @brief	 赤外線通信 スキャンプロセス
 * @date	 2012.9.10
 */
//=============================================================================================

#include <proc/include/gfl2_ProcManager.h>
#include <gflnet2/include/base/gflnet2_BaseProcess.h>
#include <gflnet2/source/proc/gflnet2_IrAutoMatchThread.h>
#include <gflnet2/include/base/gflnet2_NetStateListener.h>
#include <gflnet2/include/scan/gflnet2_Manager.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


//登録するPROCクラス BaseProcessを継承している
class IrScanProc : public gflnet2::BaseProcess {
  GFL_FORBID_COPY_AND_ASSIGN(IrScanProc); //コピーコンストラクタ＋代入禁止

  IrAutoMatchThread* mpAutoMatch;
  
  bool mbEndReq;
  gflnet2::base::TransferedIDReceiver* mpTransferedIDReceiver;
  gflnet2::NetStateListener*  mpNetStateListener; //状態遷移を伝えるリスナー

  int mCount;
  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ＆デストラクタ
   */
  //------------------------------------------------------------------

  IrScanProc(void)
    : gflnet2::BaseProcess()
    , mpAutoMatch(NULL)
    , mbEndReq(false)
    , mpTransferedIDReceiver(NULL)
    , mpNetStateListener(NULL)
    , mCount(0)
  {};
  virtual ~IrScanProc(){};

  
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメインを行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  void SetTransferedIDReceiver(gflnet2::base::TransferedIDReceiver* pTr){mpTransferedIDReceiver=pTr;};
  inline void SetManagerListener(NetStateListener* pNetStateListener){ mpNetStateListener = pNetStateListener; }

private:
//  void delThread(void);

  //------------------------------------------------------------------
  /**
   * @brief	  プロックのタイプを返す
   * @return  プロセスタイプ
   */
  //------------------------------------------------------------------
    virtual int GetProcType(void){ return gflnet2::NETPROC_TYPE_IDLE; };
  //------------------------------------------------------------------
  /**
   * @brief	  終了リクエスト
   */
  //------------------------------------------------------------------
  virtual int ReqEnd(void);



  
};


GFL_NAMESPACE_END(gflnet2)

#endif //__GFLNET_IRSCANPROC_H__

#endif
