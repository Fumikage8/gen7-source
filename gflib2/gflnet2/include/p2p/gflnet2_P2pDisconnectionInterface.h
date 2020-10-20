#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pDisconnectionInterface.h
 * @brief	P2Pプロセス：切断インターフェイス
 * @author	ichiraku_katsuhiko
 * @date	2015/04/21 14:32:34
 */
// =============================================================================

#if !defined( P2P_DISCONNECTION_INTERFACE_H_INCLUDED )
#define P2P_DISCONNECTION_INTERFACE_H_INCLUDED

#pragma once

#include <gflnet2/include/p2p/gflnet2_P2pProcBaseProcess.h>
#include <gflnet2/include/p2p/gflnet2_P2pDefine.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class P2pDisconnectionInterface : public P2pBaseProcess
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pDisconnectionInterface);
public:
  P2pDisconnectionInterface(){ m_destorySessionTimer = 0; m_isMaster=false; };
  virtual ~P2pDisconnectionInterface(){};

private:
  static const int DESTROY_SESSION_TIME = 300;  ///< セッション破棄までの時間
  int m_destorySessionTimer;  ///< セッション破棄までの制限時間

  bool m_isMaster; //! Session切断開始時の状態。NMCat[4106]修正用。

public:

  //------------------------------------------------------------------
  /**
   * @brief   Nexの送受信機能を無効化
   */
  //------------------------------------------------------------------
  void DirectStreamDisable( void );

  //------------------------------------------------------------------
  /**
   * @brief   Session切断
   * @return  trueで呼び出し成功
   */
  //------------------------------------------------------------------
  bool DisconnectSession( void );

  //------------------------------------------------------------------
  /**
   * @brief   Session切断完了チェック
   * @return  trueで完了
   */
  //------------------------------------------------------------------
  bool IsDisconnectSession( void );

};

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif  // P2P_DISCONNECTION_INTERFACE_H_INCLUDED
#endif  // GF_PLATFORM_CTR