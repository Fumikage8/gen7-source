#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_YIELD_H__
#define __GFLNET_YIELD_H__
//=============================================================================
/**
 * @file	 gflnet_Yield.h
 * @brief	 指定した時間Yieldする関数
 * @author k.ohno
 * @date	 2011.8.12
 */
//=============================================================================
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math_random.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)


class Yield {
  GFL_FORBID_COPY_AND_ASSIGN( Yield ); //コピーコンストラクタ＋代入禁止

private:
  gfl2::math::Random mRand;
  int mNumMax;
  u64 mStart;
  
public:
  Yield(void);
  virtual ~Yield();

  //------------------------------------------------------------------
  /**
   * @brief	  値の初期化
   * @param   msec   ミリ秒
   */
  //------------------------------------------------------------------
  void Initialize( int msec );

  //------------------------------------------------------------------
  /**
   * @brief	  通信状態を続けるためにYieldする関数
   * @retval  true  時間終了
   * @retval  false まだ終わってない
   */
  //------------------------------------------------------------------
  bool IsEnd(void);

  //------------------------------------------------------------------
  /**
   * @brief	  時間までYieldする
   */
  //------------------------------------------------------------------
  void KeepStop(void);


};

GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)


#endif //__GFLNET_YIELD_H__

#endif
