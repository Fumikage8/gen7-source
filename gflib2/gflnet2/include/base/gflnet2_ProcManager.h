#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_PROCMANAGER_H__
#define __GFLNET_MANAGER_H__
//=============================================================================
/**
 * @file	 gflnet_ProcManager.h
 * @brief	 通信のProcManager
 * @author k.ohno
 * @date	 2011.4.20
 */
//=============================================================================
#pragma once

#if 0

#include "gflnet2/include/base/gflnet2_ProcBase.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

namespace gflnet2 {

  
  class ProcManager {
    GFL_FORBID_COPY_AND_ASSIGN(ProcManager);

    //gflnet2::proc::ProcBase* mpProc;		    ///< プロセスのポインタ

  public:
    //------------------------------------------------------------------
    /**
     * @brief	  コンストラクタ＆デストラクタ
     */
    //------------------------------------------------------------------
    ProcManager(void){};
    virtual ~ProcManager(){};
    

    //------------------------------------------------------------------
    /**
     * @brief	 PROCの処理
     * @return マネージャーを削除して良い場合true
     */
    //------------------------------------------------------------------
    inline bool SetProc(gflnet2::proc::ProcBase* my){ mpProc = my; };

    //------------------------------------------------------------------
    /**
     * @brief	 PROCの処理
     * @return マネージャーを削除して良い場合true
     */
    //------------------------------------------------------------------
    bool MainProc(void);

  };



}//namespace gflnet
#endif //__GFLNET_MANAGER_H__
#endif

#endif
