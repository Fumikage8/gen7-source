#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_BASEPROCESS_H__
#define __GFLNET_BASEPROCESS_H__
//=============================================================================================
/**
 * @file	 gflnet_BaseProcess.h
 * @brief	 通信 基本プロセス
 * @date	 2011.1.30
 */
//=============================================================================================
#pragma once

#include "gflnet2/include/base/gflnet2_Base.h"
#include <proc/include/gfl2_ProcBase.h>
#include <gflnet2/include/base/gflnet2_NetThread.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)

class Manager;

// BaseProcessを継承している
class BaseProcess : public gfl2::proc::BaseProcess {
  GFL_FORBID_COPY_AND_ASSIGN(BaseProcess); //コピーコンストラクタ＋代入禁止

protected:
  gflnet2::base::NetThread* mpThread;   //起動スレッド
  Base* mpNetBase;   //通信ベース
  gflnet2::Manager* mpNetManager;
 // gflnet2::EN_NET_THREAD mThreadNo;  ///<スレッド番号
  int mThreadNo;  ///<スレッド番号
  int mChangeNo;
  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ＆デストラクタ
   */
  //------------------------------------------------------------------

  BaseProcess(void);
  virtual ~BaseProcess(){};

  
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメインを行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの描画を行う関数
   */
  //------------------------------------------------------------------
//  virtual void DrawFunc(gfl2::proc::Manager* pManager);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager) = 0;

  //------------------------------------------------------------------
  /**
   * @brief	  起動させるスレッド番号
   * @param	  no    起動スレッド番号EN_NET_THREAD
   * @param   pBase ワイヤレスのデバイスクラス
   */
  //------------------------------------------------------------------
  inline void SetThread(int no, Base* pBase){ mThreadNo = no; mpNetBase = pBase; };

  //------------------------------------------------------------------
  /**
   * @brief	  Managerをセット ゲット
   * @param   pNetManager
   */
  //------------------------------------------------------------------
  inline void SetNetManager(Manager* pNetManager){ mpNetManager = pNetManager; };
  inline Manager* GetNetManager(void){ return mpNetManager; };


  inline void SetChangeNo(int no ){ mChangeNo=no; };
  inline int GetChangeNo(void){ return mChangeNo; };

  
  //------------------------------------------------------------------
  /**
   * @brief	  プロックのタイプを返す
   * @return  プロセスタイプ
   */
  //------------------------------------------------------------------
  virtual int GetProcType(void) = 0;
  
  //------------------------------------------------------------------
  /**
   * @brief	  終了リクエスト
   */
  //------------------------------------------------------------------
  virtual int ReqEnd(void){ return 0; };

  //------------------------------------------------------------------
  /**
   * @brief	  起動スレッドを終了させる
   * @return  
   */
  //------------------------------------------------------------------
  virtual bool KillThread(void);

  void delThread(void);

  
};


GFL_NAMESPACE_END(gflnet2)




#endif // __GFLNET_BASEPROCESS_H__
#endif
