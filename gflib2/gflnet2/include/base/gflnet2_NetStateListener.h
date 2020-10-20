#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_NETSTATELISTENER_H__
#define __GFLNET_NETSTATELISTENER_H__
//=============================================================================
/**
 * @file   gflnet_NetStateListener.h
 * @brief  外部から内部の状態をつかむクラス
 * @author k.ohno
 * @date   2012.4.13
 */
//=============================================================================
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


class NetStateListener
{
  //GFL_FORBID_COPY_AND_ASSIGN(BeaconListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
   * @brief  指定された処理の終了
   */
  //------------------------------------------------------------------
  virtual void OnSpecificProcessingEnd( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief  作業中の取り決めた状態を返す
   */
  //------------------------------------------------------------------
  virtual void OnUpdateState( int num ) = 0;

};


GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET_NETSTATELISTENER_H__
#endif
