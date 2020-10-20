#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pDisconnectionWireless.h
 * @brief	P2Pプロセス：Wireless切断
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:31:50
 */
// =============================================================================

#ifndef __GFLNET2_P2P_DISCONNECTION_WIRELESS__
#define __GFLNET2_P2P_DISCONNECTION_WIRELESS__
#pragma once

#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "gflnet2/include/p2p/gflnet2_P2pDisconnectionInterface.h"

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )
GFL_NAMESPACE_BEGIN( proc )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class P2pDisconnectionWireless : public P2pDisconnectionInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pDisconnectionWireless); //コピーコンストラクタ＋代入禁止

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

  nn::nex::CallContext m_context;

public:
  P2pDisconnectionWireless(){};
  virtual ~P2pDisconnectionWireless(){};

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //---------------------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   Manager  管理クラス
   * @return  RES_FINISHで終了
   */
  //---------------------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* );
  //---------------------------------------------------------------------------------
  /**
   * @brief   実行処理
   * @param   Manager  管理クラス
   * @return  RES_FINISHで終了
   */
  //---------------------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager*);
  //---------------------------------------------------------------------------------
  /**
   * @brief   終了処理
   * @param   Manager  管理クラス
   * @return  RES_FINISHで終了
   */
  //---------------------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager*);


  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================

};

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // __GFLNET2_P2P_DISCONNECTION_WIRELESS__
#endif // GF_PLATFORM_CTR
