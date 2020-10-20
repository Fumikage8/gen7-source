//==============================================================================
/**
 * @file	gflnet2_NetGameMigrationListener.h
 * @brief	NetGame系のエラーリスナー
 * @author	ichiraku_katsuhiko
 * @date	2015/05/01 19:00:45
 */
// =============================================================================

#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_NETGAMEMIGRATIONLISTENER_H__
#define __GFLNET_NETGAMEMIGRATIONLISTENER_H__
#pragma once


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   NetGameエラー通知ハンドラ
 */
//------------------------------------------------------------------
namespace gflnet2{
namespace p2p{

class NetGameMigrationListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief P2P切断通知
   */
  //------------------------------------------------------------------
  virtual void OnNetGameDisconnectPlayer() = 0;

};

} // namespace p2p
} // namespace gflnet2

#endif // __GFLNET_NETGAMEERRORLISTENER_H__
#endif // GF_PLATFORM_CTR
