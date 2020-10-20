#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pMainWifi.h
 * @brief	P2Pプロセス：WIfiメイン
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:28:52
 */
// =============================================================================
#ifndef __GFLNET2_P2P_MAIN_WIFI__
#define __GFLNET2_P2P_MAIN_WIFI__
#pragma once

#include "gflnet2/include/p2p/gflnet2_P2pProcBaseProcess.h"

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )
GFL_NAMESPACE_BEGIN( proc )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class P2pMainWifi : public P2pBaseProcess , public gflnet2::nex::NexMatchmakeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pMainWifi); //コピーコンストラクタ＋代入禁止

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  bool m_isKickMe;  // sessionをキックしたフラグ

public:
  P2pMainWifi() : m_isKickMe( false ) {};
  virtual ~P2pMainWifi(){};

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

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションのエラー終了通知
  *
  * @param[in] result エラー理由
  * @param[in] result エラーアプレットに渡されるエラーコード
  *
  * @par  下記関数のエラー時に発行される。
  *       - NexMatchMakingClient::CreateSession()
  *       - NexMatchMakingClient::SearchSession()
  *       - NexMatchMakingClient::AutoMatchmake()
  *
  * @note 以下のエラーをハンドリングする必要がある
  *        QERROR(RendezVous, SessionVoid) ギャザリングが存在しない。
  *        QERROR(RendezVous, SessionFull) すでに最大参加人数に達している。
  *        QERROR(RendezVous, SessionClosed) ギャザリングが締め切られている。
  */
  //------------------------------------------------------------------
  virtual void OnNexMatchmakeError( const nn::nex::qResult& result, u32 errorCode );

  //------------------------------------------------------------------
  /**
  * @brief マッチメイクセッションのオーナー変更通知
  *
  * @param[in] newOwnerId 新たにオーナーとなったユーザーのプリンシバルID
  */
  //------------------------------------------------------------------
  virtual void OnMatchmakingOwnerChange( nn::nex::PrincipalID newOwnerId );

  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================

};

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // __GFLNET2_P2P_MAIN_WIFI__
#endif // GF_PLATFORM_CTR
