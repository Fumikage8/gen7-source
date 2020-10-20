#if defined(GF_PLATFORM_CTR)
// ============================================================================
/*
 * @file GameSyncConfirmPlayStatusRequestSequence.h
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#if !defined( GAMESYNCCONFIRMPLAYSTATUSREQUESTSEQUENCE_H_INCLUDE )
#define GAMESYNCCONFIRMPLAYSTATUSREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

#include "NetStatic/NetAppLib/include/GameSync/GameSyncConfirmPlayStatusResponseListener.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

#include <NetStatic/NetLib/include/PGL/PGLConnector.h>
#include <NetStatic/NetLib/include/PGL/PGLConnectorListener.h>

#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)

class GameSyncConfirmPlayStatusRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase,
  public NetLib::Pgl::PGLConnectorListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncConfirmPlayStatusRequestSequence );


public:

  GameSyncConfirmPlayStatusRequestSequence();
  virtual~GameSyncConfirmPlayStatusRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( NetAppLib::GameSync::GETPLAYSTATUS_PARAM* pWork )
  { 
    m_pWork = pWork->pWork;
    m_mode = pWork->mode;
    m_isSkipError = pWork->isErrorSkip;
  }
  virtual void RemoveListener(){ m_pGameSyncConfirmPlayStatusResponseListener = NULL; }
  void SetListener( GameSyncConfirmPlayStatusResponseListener* pGameSyncConfirmPlayStatusResponseListener ){ m_pGameSyncConfirmPlayStatusResponseListener = pGameSyncConfirmPlayStatusResponseListener; }

public:

  //==========================================================================
  /**
   *					PGLサーバー通信リスナー
   */
  //==========================================================================
  //------------------------------------------------------------------
  /**
   * @brief プレイステータス確認通信の完了イベントを検知
   *
   * @param  isSignIn  連携サイトへの登録有無（true:登録済み）
   */
  //------------------------------------------------------------------
  virtual void OnPGLGetPlayStatusSuccess( bool isSignIn );

    //------------------------------------------------------------------
  /**
   * @brief PGL通信のエラーイベントを検知
   *
   * @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
   *
   * @param commandType 完了したPGL通信コマンド
   * @param httpStatus サーバから返却されたHTTPステータスコード
   */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus );

  //------------------------------------------------------------------
  /**
   * @brief PGL通信のレスポンスデータのエラーコードを検知
   *
   * @param  commandType  完了したPGL通信コマンド
   * @param  resultCode   PGLサーバから受信したレスポンスエラーコード
   */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode );

  //------------------------------------------------------------------
  /**
   * @brief PGL通信のエラーイベントを検知
   *
   * @param  commandType  完了したPGL通信コマンド
   * @param  result       エラー内容
   * @param  errorCode    ネットワークエラーコード
   */
  //------------------------------------------------------------------
  virtual void OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode );

private:


  NetAppLib::System::ApplicationWorkBase*         m_pWork;
  GameSyncConfirmPlayStatusResponseListener*      m_pGameSyncConfirmPlayStatusResponseListener;

  NetLib::Pgl::PGLConnector* m_pPglConnector;
  u32 m_mode;
  bool m_isSkipError;
};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)

#endif // GAMESYNCCONFIRMPLAYSTATUSREQUESTSEQUENCE_H_INCLUDE
#endif