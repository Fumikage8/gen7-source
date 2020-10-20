#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_WIRLESSSCANPROC_H__
#define __GFLNET_WIRLESSSCANPROC_H__
//=============================================================================================
/**
 * @file	 gflnet_WirelessScanProc.h
 * @brief	 ワイヤレス通信 スキャンプロセス
 * @date	 2012.9.9
 */
//=============================================================================================

#include <gflnet2/include/base/gflnet2_AccessPointHistoryBase.h>
#include <gflnet2/include/base/gflnet2_AccessPointScannerBase.h>

#include <gflnet2/include/base/gflnet2_BaseProcess.h>
#include <gflnet2/include/scan/gflnet2_Manager.h>

#include <proc/include/gfl2_ProcManager.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


//登録するPROCクラス BaseProcessを継承している
class NetWirelessScanProc : public gflnet2::BaseProcess {
  GFL_FORBID_COPY_AND_ASSIGN(NetWirelessScanProc); //コピーコンストラクタ＋代入禁止

  bool mbEndReq;
  gflnet2::base::AccessPointScannerBase* mpAccessPointScanner;
  gflnet2::base::AccessPointHistoryBase* mpAccessPointHistory;
  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ＆デストラクタ
   */
  //------------------------------------------------------------------

  NetWirelessScanProc(void)
    : gflnet2::BaseProcess()
    , mbEndReq(false)
    , mpAccessPointScanner(NULL)
    , mpAccessPointHistory(NULL)
  {};
  virtual ~NetWirelessScanProc(){};

  
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



  void SetAPAcc(  gflnet2::base::AccessPointScannerBase* pAccessPointScanner,
                  gflnet2::base::AccessPointHistoryBase* pAccessPointHistory)
    {
      mpAccessPointScanner = pAccessPointScanner;
      mpAccessPointHistory = pAccessPointHistory;
    };

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

#endif //__GFLNET_WIRLESSSCANPROC_H__

#endif
