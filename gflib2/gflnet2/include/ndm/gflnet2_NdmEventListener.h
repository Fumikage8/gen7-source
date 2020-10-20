﻿#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NdmEventListener.h
 * @brief  デーモン関連イベント通知イベントハンドラ
 * @author Shin Kosaka
 *
 * @note デーモン動作関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//=============================================================================

#ifndef __GFLNET_NDM_EVENT_LISTENER_H__
#define __GFLNET_NDM_EVENT_LISTENER_H__
#pragma once

#include <nn.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2) 
GFL_NAMESPACE_BEGIN(ndm)

/*!
 * @brief デーモン関連イベント通知イベントハンドラ
 */
class NdmEventListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NdmEventListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
   * @brief デーモンスケジューラ全体停止の成功通知
   *  このイベントはライブラリ内部のスレッドから発行されるので注意してください。
   */
  //------------------------------------------------------------------
   virtual void OnNdmSuspendSuccess( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief デーモンスケジューラ全体停止の失敗通知
   *  このイベントはライブラリ内部のスレッドから発行されるので注意してください。
   *
   * @param result    実行結果
   */
  //------------------------------------------------------------------
   virtual void OnNdmSuspendFailed( const nn::Result& result ) = 0;
};


GFL_NAMESPACE_END(ndm)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET_NDM_EVENT_LISTENER_H__
#endif
