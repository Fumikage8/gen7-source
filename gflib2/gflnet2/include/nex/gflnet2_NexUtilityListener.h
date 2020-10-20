#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet_NexUtilityListener.h
 * @brief  ゲームサーバ　Utility　イベント通知用ハンドラ
 * @author	ichiraku_katsuhiko
 * @date	2013/05/15, 16:05
 */
// =============================================================================

#if !defined( __GFLNET_NEXUTILITYLISTENER_H__ )
#define	__GFLNET_NEXUTILITYLISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief  ゲームサーバ「ログイン・ログアウト」イベント通知用ハンドラ
 *
 * @note ゲームサーバ関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexUtilityListener
{
public:

  //------------------------------------------------------------------
  /**
   *	@brief ゲームサーバ疎通テスト結果通知
   */
  //------------------------------------------------------------------
  virtual void OnTestConnectivityResult( const bool isSuccess ){};

};


} // nex
} // gflnet



#endif	/* __GFLNET_NEXUTILITYLISTENER_H__ */

#endif
