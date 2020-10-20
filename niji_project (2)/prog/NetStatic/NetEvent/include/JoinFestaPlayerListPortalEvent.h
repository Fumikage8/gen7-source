// ============================================================================
/*
 * @file JoinFestaPlayerListPortalEvent.h
 * @brief JoinFestaPlayerListSelect系のポータルイベント
 * @date 2015.12.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTA_PLAYERLISTPORTAL_EVENT_H_INCLUDE )
#define JOINFESTA_PLAYERLISTPORTAL_EVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

// リクエストID
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"

// プレイヤーリスト系パラメータ
#include "NetApp/JoinFestaPlayerListSelect/include/JoinFestaPlayerListSelectAppParam.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/include/JoinFestaPlayerListSelectMultiAppParam.h"

// ビーコンアプリパラメータ
#include "NetApp/JoinFestaBeacon/include/JoinFestaBeaconAppParam.h"

GFL_NAMESPACE_BEGIN(NetEvent)


/* -------------------------------------------------------------------------*/
/**
 *	@brief  ジョインフェスタリスト系イベントの元イベント
 */
/* -------------------------------------------------------------------------*/
class JoinFestaPlayerListPortalEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListPortalEvent );
public:

  //! コンストラクタ
  JoinFestaPlayerListPortalEvent( gfl2::heap::HeapBase* pHeap );
  //! デストラクタ
  virtual~JoinFestaPlayerListPortalEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  /* -------------------------------------------------------------------------*/
  /**
   *  @brief  起動するイベントを設定する
   *  @param  アプリ起動番号
   */
  /* -------------------------------------------------------------------------*/
  void SetRequestID( Field::SubScreen::FieldMenuOutRequestID requestId );

private:

  //! アプリのイベント呼び出し
  void CallAppEvent( GameSys::GameManager* pGameManager );

  //! アプリの結果確認
  bool ReturnAppEvent();

private:

  Field::SubScreen::FieldMenuOutRequestID             m_requestId;                //! 起動するイベントリクエストID
  Field::SubScreen::FieldMenuOutRequestID             m_originalRequestId;        //! 保存版起動するイベントリクエストID

  NetApp::JoinFestaPlayerListSelect::APP_RESULT       m_listSelectAppResult;      //! 選択リスト結果
  NetApp::JoinFestaPlayerListSelectMulti::APP_RESULT  m_listSelectMultiAppResult; //! マルチ選択リスト結果
  NetApp::JoinFestaBeacon::APP_PARAM                  m_beaconAppParam;           //!  ビーコンアプリパラメータ
  NetAppLib::JoinFesta::JoinFestaPersonalData         m_selectedPersonalDataArray[NetApp::JoinFestaBeacon::MULTI_CONNECT_NUM];     //!  リストで選択したパーソナルデータ

};


GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTA_PLAYERLISTPORTAL_EVENT_H_INCLUDE
