#if defined(GF_PLATFORM_CTR)
// ============================================================================
/*
 * @file GameSyncCreateSaveDataRequestSequence.h
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#if !defined( GAMESYNCCREATESAVEDATAREQUESTSEQUENCE_H_INCLUDE )
#define GAMESYNCCREATESAVEDATAREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

#include "NetStatic/NetAppLib/include/GameSync/GameSyncCreateSaveDataResponseListener.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

#include <NetStatic/NetLib/include/PGL/PGLConnector.h>
#include <NetStatic/NetLib/include/PGL/PGLConnectorListener.h>

#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"
#include <layout/include/gfl2_LytSys.h>

#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)

class GameSyncCreateSaveDataRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase,
  public NetLib::Pgl::PGLConnectorListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncCreateSaveDataRequestSequence );

  enum
  {
    WIDTH_AND_HEIGHT = 64,
  };

  enum
  {
    SEQ_INIT = 0,
    // @fix GFNMCat[5431] [#5431 【ゲームシンク】ゲームシンク時に送信されるキャラクターアイコンのデータが正しくない]
    SEQ_ICON_SETUP,
    SEQ_ICON_SETUP_WAIT,//@fix[#5650 ゲームシンク：プレイヤーアイコン変換待ちが１フレーム足りない]
    SEQ_GAMESYNC_INIT,
    SEQ_WAIT,
  };

public:

  GameSyncCreateSaveDataRequestSequence();
  virtual ~GameSyncCreateSaveDataRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( GAMESYNC_PARAM* param )
  { 
    m_pWork = param->pWork;
    m_pReplacePane = param->pReplacePane;
    m_pTrainerIconUtility = param->pTrainerIconUtility;
    m_isSkipError = param->isErrorSkip;
  }
  virtual void RemoveListener(){ m_pGameSyncCreateSaveDataResponseListener = NULL; }
  void SetListener( GameSyncCreateSaveDataResponseListener* pGameSyncCreateSaveDataResponseListener ){ m_pGameSyncCreateSaveDataResponseListener = pGameSyncCreateSaveDataResponseListener; }

public:

  //------------------------------------------------------------------
  /**
  * @brief セーブデータ登録通信の完了イベントを検知
  *
  * @param  none
  */
  //------------------------------------------------------------------
  virtual void OnPGLCreateSavedataSuccess( void );

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

  NetAppLib::System::ApplicationWorkBase*      m_pWork;
  GameSyncCreateSaveDataResponseListener*      m_pGameSyncCreateSaveDataResponseListener;

  NetLib::Pgl::PGLConnector* m_pPglConnector;

  void* m_pPglBase64Savedata;   //!< BASE64エンコード済みセーブデータ
  void* m_pPglBase64PlayerIcon; //!< BASE64エンコード済みトレイナーアイコンデータ

  app::tool::AppToolTrainerIconTexture* m_pTex;

  NetAppLib::Util::NetAppTrainerIconUtility* m_pTrainerIconUtility;
  gfl2::lyt::LytPicture* m_pReplacePane;
  bool m_isSkipError;

  // @fix GFNMCat[5431] [#5431 【ゲームシンク】ゲームシンク時に送信されるキャラクターアイコンのデータが正しくない]
  void* m_tempIconBuffer;
};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)

#endif // GAMESYNCCREATESAVEDATAREQUESTSEQUENCE_H_INCLUDE
#endif