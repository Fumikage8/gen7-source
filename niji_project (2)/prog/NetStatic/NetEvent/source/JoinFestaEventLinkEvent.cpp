//=================================================================================================
/**
 * @file    JoinFestaEventLinkEvent.cpp
 * @brief ジョインフェスタからイベントを起動する際のイベント（親のイベントになる）
 * @date 2015.06.23
 * @author ichiraku_katsuhiko
 */
//=================================================================================================
#include "NetStatic/NetEvent/include/JoinFestaEventLinkEvent.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "GameSys/include/GameEventManager.h" // GAMESYSTEM_EVENT_CALL
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketEventList.h"
// field
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "Field\FieldStatic\include\Launcher\FieldApplicationLauncher.h"
#include "Field/FieldStatic/include/Item/FieldItemUse.h"

// sound
#include "Sound/include/Sound.h"

// エラーマネージャー
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// FatalErrorEvent
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"

// インターネット接続イベント
#include "NetStatic/NetEvent/include/ConnectionEvent.h"

//プレイヤーリスト系イベント
#include "NetStatic/NetEvent/include/JoinFestaBeaconEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaOpenListEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerListPortalEvent.h"

// アトラクション開始スクリプト用
#include "FieldScript/include/EventScriptCall.h"
#include <niji_conv_header/field/script/inc/join_festa_attraction.inc>

// Bag
#include "App/AppEvent/include/Bag/BagEvent.h"

//　ホームスリープ制御
#include "System/include/HomeSleepAndPowerSystem.h"

GFL_NAMESPACE_BEGIN( NetEvent )

// コンストラクタ
JoinFestaEventLinkEvent::JoinFestaEventLinkEvent( gfl2::heap::HeapBase* pHeap ) :
GameSys::GameEvent( pHeap ),
m_sequence( SEQ_FIELD_CLOSE ),
m_requestId( Field::SubScreen::OUT_REQ_ID_NONE ),
m_isBgmPush(false),
m_isCallConnectionEvent(false),
m_isAttractionStart(false),
m_connectionAppResult(),
m_OpenListAppResult(),
m_beaconAppParam(),
m_bagAppParam(),
m_playerLocation(),
m_fieldmapData()
//m_selectedPersonalData()
{
  ::std::memset( &m_connectionAppResult , 0 , sizeof(m_connectionAppResult) );
  ::std::memset( &m_OpenListAppResult , 0 , sizeof(m_OpenListAppResult) );
  ::std::memset( &m_beaconAppParam, 0 , sizeof(m_beaconAppParam) );
  GFL_PRINT("JoinFestaEventLinkEvent 開始\n");
}

// デストラクタ
JoinFestaEventLinkEvent::~JoinFestaEventLinkEvent()
{
  GFL_PRINT("JoinFestaEventLinkEvent 終了\n");
}

//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool JoinFestaEventLinkEvent::BootChk( GameSys::GameManager* pGameManager )
{
  GFL_UNUSED( pGameManager );
  return true;
}

//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void JoinFestaEventLinkEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  GFL_UNUSED( pGameManager );

  // アプリ起動前のパケット設定
  SetPacketForCallApp();
}

//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT JoinFestaEventLinkEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  switch( m_sequence ) {
  case SEQ_FIELD_CLOSE:        //! フィールド破棄
    {
      // フィールド破棄が必至か
      if( IsMustFieldCloseOpen() )
      {
        // フィールドマップから生成するデータを設定
        Field::Application::SetupFieldmapData( pGameManager, &m_fieldmapData );

        // フィールド破棄
        FieldClose( pGameManager );
      }

      m_sequence = SEQ_CHECK_INTERNET_CONNECT; 
    }
    break;

  case SEQ_CHECK_INTERNET_CONNECT:     //! インターネット接続が必要か確認
    {
      //ホームスリープ禁止フラグの設定
      SetupHomeSleepFlag();

      // 通信エラー発生チェック
      if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
      {
        // エラーが発生していた場合は、エラー対処してフィールドに戻る
        m_sequence = SEQ_NETWORK_ERROR_HANDLING;
      }
      else
      {
        // インターネット必至だった場合は、インターネット接続イベントを呼ぶ
        // インターネット未接続の場合のみ呼ぶ
        if( IsMustInternet() && NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
        {
          // インターネット接続モード
          NetApp::Connection::APP_PARAM connectionAppParam;
          connectionAppParam.bootMode = NetApp::Connection::BOOT_MODE_INTERNET_CONNECTION;
          NetEvent::Connection::ConnectionEvent::StartEvent( pGameManager, connectionAppParam, &m_connectionAppResult );

          m_isCallConnectionEvent = true;

          m_sequence = SEQ_CHECK_INTERNET_CONNECT_WAIT; 
        }
        else
        {
          m_sequence = SEQ_CALL_REQUEST_EVENT; 
        }
      }
    }
    break;

  case SEQ_CHECK_INTERNET_CONNECT_WAIT://! インターネット接続待ち
    {
      // キャンセルしたらフィールドに戻す
      // @fix NMcat[54]：インターネット切断確認にいかずにフィールド開始に直接いっていた。
      m_sequence = ( m_connectionAppResult.bIsCanceled == true ) ? SEQ_CHECK_INTERNET_DISCONNECT : SEQ_CALL_REQUEST_EVENT; 
    }
    break;

  case SEQ_CALL_REQUEST_EVENT: //! リクエストされたイベント呼び出し
    {
      // 通信エラー発生チェック
      if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
      {
        // エラーが発生していた場合は、エラー対処してフィールドに戻る
        m_sequence = SEQ_NETWORK_ERROR_HANDLING;
      }
      else
      {
        // BGMのプッシュ
        if( IsBgmPushPop() == true )
        {
          Sound::PushBGMReq(); 
          m_isBgmPush = true;
        }

        CallAppEvent( pGameManager );
        m_sequence = SEQ_CHECK_APP_RESULT;
      }
    }
    break;
  case SEQ_CHECK_APP_RESULT: //! アプリの結果を確認
    {
      // アプリ結果確認
      CheckAppResult( pGameManager );
      m_sequence = SEQ_CHECK_INTERNET_DISCONNECT;
    }
    break;
  case SEQ_CHECK_INTERNET_DISCONNECT:    //! インターネット切断が必要か確認
    {
      // 通信エラー発生チェック
      if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
      {
        // エラーが発生していた場合は、エラー対処してフィールドに戻る
        m_sequence = SEQ_NETWORK_ERROR_HANDLING;
      }
      else
      {
        // レギュレーションDLアプリをローカルから起動した時は、切断してから戻る
        // @fix NMcat[1070]：インターネット接続をキャンセしていた場合は切断にいかない
        if( m_requestId == Field::SubScreen::OUT_REQ_ID_REGULATION_DOWNLOAD &&
            m_isCallConnectionEvent == true &&
            m_connectionAppResult.bIsCanceled == false )
        {
          // インターネット切断モード
          NetApp::Connection::APP_PARAM connectionAppParam;
          connectionAppParam.bootMode = NetApp::Connection::BOOT_MODE_INTERNET_DISCONNECTION;
          NetEvent::Connection::ConnectionEvent::StartEvent( pGameManager, connectionAppParam, &m_connectionAppResult );
        }
        m_sequence = SEQ_CHECK_INTERNET_DISCONNECT_WAIT;
      }
    }
    break;
  case SEQ_CHECK_INTERNET_DISCONNECT_WAIT://! インターネット切断待ち
    {
      m_sequence = SEQ_NETWORK_ERROR_HANDLING;
    }
    /* break through */
  case SEQ_NETWORK_ERROR_HANDLING:       //! 通信エラー対処
    {
      // 通信エラー対処（発生していなくても呼ぶ）
      NetworkErrorHandling( pGameManager );
      m_sequence = SEQ_FIELD_OPEN;
    }
    break;
  case SEQ_FIELD_OPEN:         //! フィールド開始
    {
      // ホームスリープ禁止の解除
      ReleaseHomeSleepFlag();

      // BGMのポップ
      if( IsBgmPushPop() == true && m_isBgmPush ==  true )
      {
        Sound::PopBGMReq( Sound::BGM_FADE_FAST );
      }
      // フィールド生成が必至か
      if( IsMustFieldCloseOpen() )
      {
        // フィールド生成
        FieldOpen( pGameManager );
      }

      m_sequence = SEQ_SCRIPT_CALL; 
    }
    break;

  case SEQ_SCRIPT_CALL:        //! スクリプト呼び出し
    {
      // フィールド復帰前スクリプト実行
      CallScript( pGameManager );
      
      m_sequence = SEQ_END; 
    }
    break;

  case SEQ_END:        //! 終了
    return GameSys::GMEVENT_RES_FINISH;

  default:
    GFL_ASSERT(0);
    break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void JoinFestaEventLinkEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  GFL_UNUSED( pGameManager );

  // パケットの設定
  SetPacketForEndApp();
}

/* -------------------------------------------------------------------------*/
/**
 *  @brief  起動するイベントを設定する
 *  @param  アプリ起動番号
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::SetRequestID( Field::SubScreen::FieldMenuOutRequestID requestId )
{
  GFL_PRINT("JoinFestaEventLinkEvent::SetRequestID %d \n", requestId );
  m_requestId = requestId;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief フィールド破棄
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::FieldClose( GameSys::GameManager* pGameManager )
{
  // 復帰時に設定するため、現在のロケーションを保持しておく
  m_playerLocation.worldId  = pGameManager->GetFieldmap()->GetWorldID();
  m_playerLocation.zoneId   = pGameManager->GetFieldmap()->GetZoneID();
  m_playerLocation.position = pGameManager->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().translate;
  m_playerLocation.quaternion = pGameManager->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().rotation;

  // クローズイベント
  Field::EventFieldmapClose* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventFieldmapClose >( pGameManager->GetGameEventManager() );
  Field::EventFieldmapClose::Desc desc;
  desc.closeType = Field::FM_CLOSE_MODE_APP; // メモリを破棄
  p_event->SetDesc( desc );
  p_event->SetFadeCallBackDefault();  //フェードはデフォルト
}

/* -------------------------------------------------------------------------*/
/**
 * @brief フィールド生成
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::FieldOpen( GameSys::GameManager* pGameManager )
{
  // オープンイベント
  Field::EventFieldmapOpen* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventFieldmapOpen >( pGameManager->GetGameEventManager() );
  p_event->SetDesc( Field::FM_OPEN_MODE_APP_RECOVER, m_playerLocation );
  p_event->SetFadeCallBackDefault();
}

/* -------------------------------------------------------------------------*/
/**
 * @brief インターネット接続必至か
 * @return 必至の時true
 */
/* -------------------------------------------------------------------------*/
bool JoinFestaEventLinkEvent::IsMustInternet() const
{
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_MIRACLE:           //!< ミラクル交換
  case Field::SubScreen::OUT_REQ_ID_GTS:               //!< GTS
  case Field::SubScreen::OUT_REQ_ID_BATTLE_SPOT:       //!< バトルスポット
  case Field::SubScreen::OUT_REQ_ID_REGULATION_DOWNLOAD: //!< レギュレーションダウンロード
  case Field::SubScreen::OUT_REQ_ID_GAME_SYNC:           //!< ゲームシンク
  case Field::SubScreen::OUT_REQ_ID_WIFI_BTLVIDEO:       //!< インターネット版バトルビデオ
  case Field::SubScreen::OUT_REQ_ID_GLOBAL_ATTRACTION:   //!< グローバルアトラクションアプリ
    return true;
  }
  return false;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief フィールド破棄・生成が必要か
 * @return 必至の時true
 */
/* -------------------------------------------------------------------------*/
bool JoinFestaEventLinkEvent::IsMustFieldCloseOpen() const
{
  // スクリプト起動なので、スクリプト側でフィールド破棄・生成を行う
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_GAME_SYNC:           //!< ゲームシンク
  case Field::SubScreen::OUT_REQ_ID_WIFI_BTLVIDEO:       //!< インターネット版バトルビデオ
  case Field::SubScreen::OUT_REQ_ID_GLOBAL_ATTRACTION:   //!< グローバルアトラクションアプリ
    return false;
  }
  return true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アプリ切り替え時にBGMのプッシュ/ポップを行うか
 * @return trueのときBGMをプッシュ/ポップする
 */
/* -------------------------------------------------------------------------*/
bool JoinFestaEventLinkEvent::IsBgmPushPop()
{
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_MIRACLE:           //!< ミラクル交換
  case Field::SubScreen::OUT_REQ_ID_GTS:               //!< GTS
    return true;
  }
  return false;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アプリ起動時のパケット設定
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::SetPacketForCallApp()
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();

#if defined(GF_PLATFORM_CTR)
  // プレイ状況更新
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_MIRACLE:           //!< ミラクル交換
    pMyData->SetRequestPlayingMiracleTrade();
    break;
  case Field::SubScreen::OUT_REQ_ID_GTS:               //!< GTS
    pMyData->SetRequestPlayingGTS();
    break;
  case Field::SubScreen::OUT_REQ_ID_BATTLE_SPOT:       //!< バトルスポット
    pMyData->SetRequestPlayingBattleSpot();
    break;
  default:
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SetPacketGameStatusWithMe();
    break;
  }
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アプリ終了してフィールド戻るのパケット設定
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::SetPacketForEndApp()
{
  // アトラクション開催していたら、既にアトラクションパケットなのでリセットしない
  if( m_isAttractionStart == true )
  {
    return;
  }

#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SetPacketGameStatusWithMe();
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アプリのイベント呼び出し
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::CallAppEvent( GameSys::GameManager* pGameManager )
{
  //アプリの結果が見たい（APP_PARAM渡したい）場合は個別に呼び出す
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_S:       //!< プレイヤーリスト（シングル対戦選択モード１）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_D:       //!< プレイヤーリスト（ダブル対戦選択モード１）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_TRADE:          //!< プレイヤーリスト（交換選択モード１）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_M:       //!< プレイヤーリスト（マルチ対戦選択モード２）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_R:       //!< プレイヤーリスト（ロイヤル対戦選択モード２）
    {
      // プレイヤーリストのポータルEventへ
      NetEvent::JoinFestaPlayerListPortalEvent *pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::JoinFestaPlayerListPortalEvent>( pGameManager->GetGameEventManager() );
      pEvent->SetRequestID( m_requestId );
    }
    break;

  // お誘いOKでのマッチング
  case  Field::SubScreen::OUT_REQ_ID_BEACON_MATCHING:
    {
      ::std::memset( &m_beaconAppParam , 0 , sizeof(m_beaconAppParam) );

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
      NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = pPersonalDataManager->GetPersonalData( pPersonalDataManager->GetInviteFriendKey() );

      if( pPersonalData == NULL )
      {
        return;
      }

      m_beaconAppParam.inviteType = NetApp::JoinFestaBeacon::TYPE_INVITED;
      m_beaconAppParam.m_pSelectedPersonalDataArray[0] = pPersonalData;
      m_beaconAppParam.connectNum = NetApp::JoinFestaBeacon::SINGLE_CONNECT_NUM;

      if( pPersonalData->GetRequestCommand() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE )
      {
        m_beaconAppParam.type = NetApp::JoinFestaBeacon::TYPE_TRADE;
      }
      else
      {
        if( pPersonalData->IsEnablePacketBattleInvite() )
        {
          switch( pPersonalData->GetPacketBattleInvite().battle_rule ){
          case Regulation::BATTLE_RULE_SINGLE:
            m_beaconAppParam.type = NetApp::JoinFestaBeacon::TYPE_BATTLE_S;
            break;
          case Regulation::BATTLE_RULE_DOUBLE:
            m_beaconAppParam.type = NetApp::JoinFestaBeacon::TYPE_BATTLE_D;
            break;
          case Regulation::BATTLE_RULE_MULTI:
            m_beaconAppParam.type = NetApp::JoinFestaBeacon::TYPE_BATTLE_M;
            m_beaconAppParam.connectNum = NetApp::JoinFestaBeacon::MULTI_CONNECT_NUM;
            break;
          case Regulation::BATTLE_RULE_ROYAL:
            m_beaconAppParam.type = NetApp::JoinFestaBeacon::TYPE_BATTLE_R;
            m_beaconAppParam.connectNum = NetApp::JoinFestaBeacon::MULTI_CONNECT_NUM;
            break;
          }
        }
      }

#if defined(GF_PLATFORM_CTR)
      // @fix NMCat[3312]:お誘いをOKしたので他のお誘いはクリアする
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketEventList)->Reset();
#endif // GF_PLATFORM_CTR
      
      NetEvent::JoinFestaBeacon::JoinFestaBeaconEvent::StartEvent( pGameManager, &m_beaconAppParam );
    }
  break;
  case Field::SubScreen::OUT_REQ_ID_OPEN_LIST:                         //!< 募集一覧のリスト（ストリーミングタッチで行く）
    {
      NetEvent::JoinFestaOpenList::JoinFestaOpenListEvent::StartEvent( pGameManager, &m_OpenListAppResult );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_BAG:
    {
      App::Event::BagEvent *pBagEvent = App::Event::BagEvent::StartEvent( pGameManager );

      // あまいみつ使用時用にパラメータ渡して結果を取得する必要がある
      gfl2::std::MemClear( &m_bagAppParam, sizeof(app::bag::APP_PARAM) );
      m_bagAppParam.call_mode = app::bag::CALL_FIELD_MENU;
      pBagEvent->SetupAppParam( &m_bagAppParam );
      pBagEvent->SetupFieldParam( &m_fieldmapData.item_use_work, m_fieldmapData.weather );
    }
    break;
  default:
    Field::Application::CallAppEvent( m_requestId, pGameManager, &m_fieldmapData ); //アプリの結果を見る必要がない場合は共通の呼び出し
    break;
  }
}
 
/* -------------------------------------------------------------------------*/
/**
 * @brief アプリの結果確認
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::CheckAppResult( GameSys::GameManager* pGameManager )
{
  //アプリの結果で分岐
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_OPEN_LIST:                         //!< 募集一覧のリスト（ストリーミングタッチで行く）
    {
      ICHI_PRINT(">ICHI OPEN_LIST Result %d \n",m_OpenListAppResult.bIsSelectedJoinFestaAttractionData);

      // アトラクション参加していたら、アトラクションスタートスクリプト呼ぶ
      if( m_OpenListAppResult.bIsSelectedJoinFestaAttractionData == true )
      {
        m_isAttractionStart = true;
        GFL_PRINT("アトラクション選択した！\n");
      }
    }
    break;
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief フィールド復帰直後のスクリプト実行
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::CallScript( GameSys::GameManager* pGameManager )
{
  //アプリの結果で分岐
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_OPEN_LIST:                         //!< 募集一覧のリスト（ストリーミングタッチで行く）
    {
      // アトラクション参加していたら、アトラクションスタートスクリプト呼ぶ
      if( m_isAttractionStart == true )
      {
        // ローカルアトラクション開始スクリプト起動
        Field::EventScriptCall::CallScript( pGameManager, SCRID_JOIN_FESTA_ATTRACTION_START, NULL );
      }
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_BAG:
    {
      // あまいみつだけ。
      if( m_bagAppParam.select_item != ITEM_DUMMY_DATA )
      {
        Field::FieldItemUse::CallItem( pGameManager, m_bagAppParam.select_item );
      }
    }
    break;
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 通信エラー対処
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::NetworkErrorHandling( GameSys::GameManager* pGameManager )
{
  // エラー処理対応
  // memo:このEventは全ての通信アプリ起動の親イベントになっている。
  //      対処すべきエラーは、
  //      ・P2P切断  → 強制P2P切断をしてP2Pを終わらせる必要がある
  //      ・WiFi切断 → 強制WiFi切断をしてWiFiを切断する必要がある
  //    
  //      P2P切断は、P2Pを接続するEventが各自呼んでいるはずなので、ここでは処理しない
  //      WiFi切断はここで処理する
  //
  NetLib::Error::NijiNetworkErrorManager* pErrorManager = GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager);

  if( pErrorManager->IsFatalError() )
  {
    // FatalError呼び出し
    NetEvent::FatalError::EventParam eventParam;
    eventParam.eFatalErrorNo = NetEvent::FatalError::FATAL_ERROR_NO_NET_ERROR;
    NetEvent::FatalError::FatalErrorEvent::StartEvent( pGameManager, eventParam );
  }
  else if( pErrorManager->IsError() )
  {
    // 共通通信エラー対処
    pErrorManager->CommonNetworkErrorHandling();
  }

  // 通信エラークリア
  pErrorManager->Clear();
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ホームスリープ禁止フラグ設定
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::SetupHomeSleepFlag( void )
{
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_MIRACLE:           //!< ミラクル交換
  case Field::SubScreen::OUT_REQ_ID_GTS:               //!< GTS
  case Field::SubScreen::OUT_REQ_ID_BATTLE_SPOT:       //!< バトルスポット
  case Field::SubScreen::OUT_REQ_ID_REGULATION_DOWNLOAD: //!< レギュレーションダウンロード
  case Field::SubScreen::OUT_REQ_ID_GAME_SYNC:           //!< ゲームシンク
  case Field::SubScreen::OUT_REQ_ID_WIFI_BTLVIDEO:       //!< インターネット版バトルビデオ
  case Field::SubScreen::OUT_REQ_ID_BEACON_MATCHING://!<ビーコンマッチング
  case Field::SubScreen::OUT_REQ_ID_CHANGE_WIFI://!<Wifi切り替え
  case Field::SubScreen::OUT_REQ_ID_CHANGE_LOCAL://!<ローカル切り替え
  case Field::SubScreen::OUT_REQ_ID_GLOBAL_ATTRACTION:   //!< グローバルアトラクションアプリ
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
    break;
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SHOW:                  //!< プレイヤーリスト（管理閲覧モード３）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_S:       //!< プレイヤーリスト（シングル対戦選択モード１）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_D:       //!< プレイヤーリスト（ダブル対戦選択モード１）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_M:       //!< プレイヤーリスト（マルチ対戦選択モード２）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_R:       //!< プレイヤーリスト（ロイヤル対戦選択モード２）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_TRADE:          //!< プレイヤーリスト（交換選択モード１）
  case Field::SubScreen::OUT_REQ_ID_OPEN_LIST:                         //!< 募集一覧のリスト（ストリーミングタッチで行く）
    if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )//これらはWifiのときだけHomeSleep禁止
    {
      System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
    }
    break;
  default:
    break;
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ホームスリープ禁止フラグ解除
 */
/* -------------------------------------------------------------------------*/
void JoinFestaEventLinkEvent::ReleaseHomeSleepFlag( void )
{
  System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
}

GFL_NAMESPACE_END( NetEvent )
