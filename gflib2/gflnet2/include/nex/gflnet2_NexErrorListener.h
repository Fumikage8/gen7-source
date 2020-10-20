#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexErrorListener.h
 * @brief  ゲームサーバ関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_ERROR_LISTENER_H__
#define __GFLNET_NEX_ERROR_LISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief ゲームサーバ関連通知イベントハンドラ
 *        本通知を検知したいクラスに継承し、仮想関数を実装すること。
 *
 * @note ゲームサーバ関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexErrorListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexErrorListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
  * @brief NEX内部で確保されたヒープメモリが枯渇したことを通知
  */
  //------------------------------------------------------------------
  virtual void OnNexErrorOutOfMemory( void ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief NEX内部のエラー通知
  *
  * @param[in] result     エラー値
  */
  //------------------------------------------------------------------
  virtual void OnNexErrorHandling( const nn::nex::qResult& result ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief ゲームサーバログイン後、ゲームサーバとの接続が切断されたことを通知
   */
  //------------------------------------------------------------------
  virtual void OnNexErrorGameServerShutdown( void ) = 0;
};


} // nex
} // gflnet


#endif // __GFLNET_NEX_ERROR_LISTENER_H__
#endif
