#if 0//廃止
//==============================================================================
/**
 * @file        pawn.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2014/12/02(火) 15:42:45
 */
//==============================================================================

#if !defined(__PAWN_H__)  // 重複定義防止
#define __PAWN_H__  // 重複定義防止マクロ
#pragma once


#ifdef _DEBUG
  #define GFL_PAWN_DEBUG  (1)
#endif  // _DEBUG


#define USE_GFLHEAP       (1)


#ifdef GF_PLATFORM_CTR
  #include <nn/types.h>
#endif


#ifdef  __cplusplus
  #ifdef USE_GFLHEAP
    #include "heap/include/gfl2_Heap.h"
  #endif

  #if GFL_PAWN_DEBUG
    #include "debug/include/gfl2_assert.h"
  #endif
#endif

#endif // __PAWN_H__ 重複定義防止
#endif
