#if defined(GF_PLATFORM_CTR)
// ============================================================================
/*
 * @file GameSyncSaveDataSyncRequestSequence.h
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#if !defined( GAMESYNCSAVEDATASYNCREQUESTSEQUENCE_H_INCLUDE )
#define GAMESYNCSAVEDATASYNCREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

#include "NetStatic/NetAppLib/include/GameSync/GameSyncSaveDataSyncResponseListener.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

#include <NetStatic/NetLib/include/PGL/PGLConnector.h>
#include <NetStatic/NetLib/include/PGL/PGLConnectorListener.h>

#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"
#include <layout/include/gfl2_LytSys.h>

#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"

#include <Savedata/include/MysteryGiftSave.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)

class GameSyncSaveDataSyncRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase,
  public NetLib::Pgl::PGLConnectorListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncSaveDataSyncRequestSequence );

public:

  enum
  {
    WIDTH_AND_HEIGHT = 64,
    GIFT_ITEM_SLOT_NUM = 6,//ふしぎカードに入るitem数
  };

  enum
  {
    SEQ_INIT = 0,
    // @fix GFNMCat[5431] [#5431 【ゲームシンク】ゲームシンク時に送信されるキャラクターアイコンのデータが正しくない]
    SEQ_ICON_SETUP,
    SEQ_ICON_SETUP_WAIT,//@fix[#5650 ゲームシンク：プレイヤーアイコン変換待ちが１フレーム足りない]
    SEQ_GAMESYNC_INIT,
    SEQ_WAIT,
    SEQ_SAVE_WAIT,
    SEQ_LAST_SAVE_WAIT,
    SEQ_END
  };

public:

  GameSyncSaveDataSyncRequestSequence();
  virtual~GameSyncSaveDataSyncRequestSequence();

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
  virtual void RemoveListener(){ m_pGameSyncSaveDataSyncResponseListener = NULL; }
  void SetListener( GameSyncSaveDataSyncResponseListener* pGameSyncSaveDataSyncResponseListener ){ m_pGameSyncSaveDataSyncResponseListener = pGameSyncSaveDataSyncResponseListener; }

public:

  //------------------------------------------------------------------
  /**
  * @brief セーブデータ同期通信の完了イベントを検知
  *
  * @param  isSignIn  連携サイトへの登録有無（true:登録済み）
  * @param  itemCount 持ち帰ったアイテム数
  * @param  itemInf   持ち帰ったアイテム情報配列(itemCountが0の時はNULL)
  * @param  crc       CRC値。持ち帰ったアイテム数が0の場合は0。
  */
  //------------------------------------------------------------------
  virtual void OnPGLSyncSaveDataSuccess( bool isSignIn, u8 itemCount, NetLib::Pgl::ResultItemInfo* itemInf );

  //------------------------------------------------------------------
  /**
  * @brief セーブデータ同期確定通信の完了イベントを検知
  *
  * @param  none
  */
  //------------------------------------------------------------------
  virtual void OnPGLCommitGameSyncSuccess( void );

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
  GameSyncSaveDataSyncResponseListener*        m_pGameSyncSaveDataSyncResponseListener;

  NetLib::Pgl::PGLConnector* m_pPglConnector;

  void* m_pPglBase64Savedata;   //!< BASE64エンコード済みセーブデータ
  void* m_pPglBase64PlayerIcon; //!< BASE64エンコード済みトレイナーアイコンデータ

  app::tool::AppToolTrainerIconTexture* m_pTex;

  NetAppLib::Util::NetAppTrainerIconUtility* m_pTrainerIconUtility;
  gfl2::lyt::LytPicture* m_pReplacePane;
  bool m_isSkipError;

  // @fix GFNMCat[5431] [#5431 【ゲームシンク】ゲームシンク時に送信されるキャラクターアイコンのデータが正しくない]
  void* m_tempIconBuffer;
  //@fix [#5693 ゲームシンク時のセーブが、巻き戻し対応されていない]
  Savedata::MysteryGiftSave::CORE_DATA  m_GiftSaveCoreDataClone;
};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)

#endif // GAMESYNCSAVEDATASYNCREQUESTSEQUENCE_H_INCLUDE
#endif