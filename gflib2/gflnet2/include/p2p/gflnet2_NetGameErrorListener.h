//==============================================================================
/**
 * @file	gflnet2_NetGameErrorListener.h
 * @brief	NetGame系のエラーリスナー
 * @author	ichiraku_katsuhiko
 * @date	2015/05/01 19:00:45
 */
// =============================================================================

#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_NETGAMEERRORLISTENER_H__
#define __GFLNET_NETGAMEERRORLISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   NetGameエラー通知ハンドラ
 * @return  NetGameBase及び、NetGameの通知を受け取る場合は、本クラスを継承し仮想関数を実装して下さい。
 */
//------------------------------------------------------------------
namespace gflnet2{
namespace p2p{

class NetGameErrorListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief P2P切断通知
   */
  //------------------------------------------------------------------
  virtual void OnNetGameSessionLeft() = 0;

};

} // namespace p2p
} // namespace gflnet2

#endif // __GFLNET_NETGAMEERRORLISTENER_H__
#endif // GF_PLATFORM_CTR
