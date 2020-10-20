#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexDispatchThreadListener.h
 * @brief  Dispatchスレッド用ポーリング通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_DISPATCH_THREAD_LISTENER_H__
#define __GFLNET_NEX_DISPATCH_THREAD_LISTENER_H__

#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief Dispatchスレッド用ポーリング通知イベントハンドラ
 *
 * @note Dispatchスレッド中にポーリング処理を行いたい場合は
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexDispatchThreadListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexDispatchThreadListener); //コピーコンストラクタ＋代入禁止
public:
   //------------------------------------------------------------------
  /**
  * @brief Dispatchスレッド中のポーリング処理実行通知
  *
  * @note 本関数はDispathスレッドからコールされる為、スレッドアンセーフです。
  * @note 適切に排他制御を行ってください
  */
  //------------------------------------------------------------------
  virtual void OnNexDispatchPollingEvent( void ) = 0;
};


} // nex
} // gflnet



#endif // __GFLNET_NEX_DISPATCH_THREAD_LISTENER_H__
#endif
