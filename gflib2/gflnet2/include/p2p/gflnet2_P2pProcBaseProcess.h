#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_P2PPROCBASEPROCESS_H__
#define __GFLNET_P2PPROCBASEPROCESS_H__
//=============================================================================
/**
 * @file	  gflnet_P2pProcBaseProcess.h
 * @brief	  P2pのプロセス基底クラス
 * @author  k.ohno
 * @date	  2012.4.3
 */
//=============================================================================
#pragma once

#include <nn.h>

#include <gflnet2/include/base/gflnet2_InitParam.h>
#include <gflnet2/include/p2p/gflnet2_P2pManager.h>

#include <proc/include/gfl2_ProcBase.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

//------------------------------------------------------------------
/**
 * @brief	プロセス動作関数の結果
 */
//------------------------------------------------------------------
class P2pBaseProcess : public gfl2::proc::BaseProcess {
  GFL_FORBID_COPY_AND_ASSIGN(P2pBaseProcess);

private:
  InitParam* mpInitParam;
  P2pManager* mpP2pManager;

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ＆デストラクタ
   */
  //------------------------------------------------------------------
  P2pBaseProcess(void) :
    BaseProcess(),
    mpInitParam( NULL ),
    mpP2pManager( NULL )
  {
  };
  virtual ~P2pBaseProcess(){};

  // Drawは使わないので空実装しておく
  virtual void DrawFunc(gfl2::proc::Manager*){};

public:
  inline void SetInitParam(InitParam* pParam){ mpInitParam = pParam; };
  inline InitParam* GetInitParam(void){ return mpInitParam; };

  inline void SetP2pManager(P2pManager* pP2pManager){ mpP2pManager = pP2pManager; }
  inline P2pManager* GetP2pManager(void){ return mpP2pManager; };

  // プロセスの戻り値をセット
  void SetProcReturn( P2pManager::E_PROC_RETURN procReturn ){ mpP2pManager->SetProcReturn(procReturn); };
};

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET_P2PPROCBASEPROCESS_H__
#endif
