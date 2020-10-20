//=================================================================================================
/**
 * @file  JoinFestaEventLinkEvent.h
 * @brief ジョインフェスタからイベントを起動する際のイベント（親のイベントになる）
 * @date 2015.06.23
 * @author ichiraku_katsuhiko
 */
//=================================================================================================
#ifndef __JOIN_FESTA_EVENT_LINK_EVENT_H__
#define __JOIN_FESTA_EVENT_LINK_EVENT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"


// リクエストID
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"

// field
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldStatic/include/FieldLocation.h"
#include "Field/FieldStatic/include\Launcher/FieldApplicationLauncher.h"

// インターネット接続アプリ起動パラメータ
#include "NetApp/Connection/include/ConnectionAppParam.h"

// プレイヤーリスト系パラメータ
#include "NetApp/JoinFestaOpenList/include/JoinFestaOpenListAppParam.h"

// ビーコンアプリパラメータ
#include "NetApp/JoinFestaBeacon/include/JoinFestaBeaconAppParam.h"

// バッグアプリパラメータ
#include "App/Bag/include/BagAppParam.h"

GFL_NAMESPACE_BEGIN(NetEvent)


/* -------------------------------------------------------------------------*/
/**
 *	@brief  ジョインフェスタからイベントを起動する際のイベント
 */
/* -------------------------------------------------------------------------*/
class JoinFestaEventLinkEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaEventLinkEvent ); // コピーを禁止

public:
  //! コンストラクタ
  JoinFestaEventLinkEvent( gfl2::heap::HeapBase* pHeap );
  //! デストラクタ
  virtual ~JoinFestaEventLinkEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

public:

  /* -------------------------------------------------------------------------*/
  /**
   *  @brief  起動するイベントを設定する
   *  @param  アプリ起動番号
   */
  /* -------------------------------------------------------------------------*/
  void SetRequestID( Field::SubScreen::FieldMenuOutRequestID requestId );

private:
  
  /**
   * @enum Sequence
   *       シーケンス定義
   */
  enum Sequence{
    SEQ_FIELD_CLOSE,                  //! フィールド破棄
    SEQ_CHECK_INTERNET_CONNECT,       //! インターネット接続が必要か確認
    SEQ_CHECK_INTERNET_CONNECT_WAIT,  //! インターネット接続待ち
    SEQ_CALL_REQUEST_EVENT,           //! リクエストされたイベント呼び出し
    SEQ_CHECK_APP_RESULT,             //! アプリの結果を確認
    SEQ_CHECK_INTERNET_DISCONNECT,    //! インターネット切断が必要か確認
    SEQ_CHECK_INTERNET_DISCONNECT_WAIT,//! インターネット切断待ち
    SEQ_NETWORK_ERROR_HANDLING,       //! 通信エラー対処
    SEQ_FIELD_OPEN,                   //! フィールド開始
    SEQ_SCRIPT_CALL,                  //! スクリプト呼び出し
    SEQ_END,                          //! 終了
  };

  //! フィールド破棄
  void FieldClose( GameSys::GameManager* pGameManager );
  //! フィールド生成
  void FieldOpen( GameSys::GameManager* pGameManager );

  //! インターネット接続必至か
  bool IsMustInternet() const;

  //! フィールド破棄・生成が必要か
  bool IsMustFieldCloseOpen() const;

  //! アプリ切り替え時にBGMのプッシュ/ポップを行うか
  bool IsBgmPushPop();

  //! アプリ起動時のパケット設定
  void SetPacketForCallApp();
  //! アプリ終了してフィールド戻るのパケット設定
  void SetPacketForEndApp();
  
  //! アプリのイベント呼び出し
  void CallAppEvent( GameSys::GameManager* pGameManager );

  //! アプリの結果確認
  void CheckAppResult( GameSys::GameManager* pGameManager );

  //! フィールド復帰直後のスクリプト実行
  void CallScript( GameSys::GameManager* pGameManager );

  //! 通信エラー対処
  void NetworkErrorHandling( GameSys::GameManager* pGameManager );

  //! ホームスリープ禁止フラグ設定
  void SetupHomeSleepFlag( void );

  //! ホームスリープ禁止フラグ解除
  void ReleaseHomeSleepFlag( void );

private:

  Sequence                                  m_sequence;       //! シーケンス
  Field::SubScreen::FieldMenuOutRequestID   m_requestId;      //! 起動するイベントリクエストID
  bool                                      m_isBgmPush;      //! BGMプッシュしたフラグ
  bool                                      m_isCallConnectionEvent;    //! インターネット接続イベント呼び出しかフラグ
  bool                                      m_isAttractionStart;        //! ローカルアトラクション開始フラグ


  NetApp::Connection::APP_RESULT                      m_connectionAppResult;      //! インターネット接続アプリ結果
  NetApp::JoinFestaOpenList::APP_RESULT               m_OpenListAppResult;        //! 開催選択結果
  NetApp::JoinFestaBeacon::APP_PARAM                  m_beaconAppParam;           //!  ビーコンアプリパラメータ
  app::bag::APP_PARAM                                 m_bagAppParam;              //! バッグアプリパラメータ

  // フィールド復帰用
  Field::Location                           m_playerLocation;
  
  Field::Application::FieldmapData  m_fieldmapData;   //! フィールドマップから生成するデータ
};


GFL_NAMESPACE_END(NetEvent)


#endif // __JOIN_FESTA_EVENT_LINK_EVENT_H__
