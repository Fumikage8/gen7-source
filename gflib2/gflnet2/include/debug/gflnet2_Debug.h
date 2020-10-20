#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_DEBUG_H__
#define __GFLNET_DEBUG_H__
//=============================================================================
/**
 * @file	 gflnet_Debug.h
 * @brief	 通信基底クラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================
#pragma once


#include <nn.h>


#if GFL_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)

class Debug {
  GFL_FORBID_COPY_AND_ASSIGN(Debug);
public:

  //------------------------------------------------------------------
  /**
   * @brief	  ダンプ
   * @param   pData  データ
   * @param   pMsg   メッセージ
   */
  //------------------------------------------------------------------
  static void Dump(const u8* pData,const u8* pMsg);

};


GFL_NAMESPACE_END(gflnet2)

#endif //GFL_DEBUG
#endif //__GFLNET_DEBUG_H__
#endif
