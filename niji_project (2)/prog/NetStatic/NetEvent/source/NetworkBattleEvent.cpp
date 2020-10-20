// ============================================================================
/*
 * @file NetworkBattleEvent.cpp
 * @brief 通信対戦イベント
 * @date 2015.08.18
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetStatic/NetEvent/include/NetworkBattleEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "Field/FieldStatic/include/Event/EventProcessCall.h"
#include "Sound/include/Sound.h"

// save
#include "Savedata/include/Record.h"
#include "Savedata/include/PokeDiarySave.h"

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
// P2P
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
// battle
#include "Battle/include/battle_proc.h"
// レギュレーション
#include "Battle/Regulation/include/Regulation.h"

// バトルロイヤル結果イベント
#include "App/AppEvent/include/BattleRoyalResult/BattleRoyalResultEvent.h"
// バトルビデオ録画イベント
#include "NetStatic/NetEvent/include/BattleVideoRecordingEvent.h"
// VSデモイベント
#include "NetStatic/NetEvent/include/VsDemoEvent.h"

// ネットワークシステム
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(NetworkBattle)

#if PM_DEBUG
  /**  Regulationフラグに関わらず、観戦コマンドをBeacon送信する  */
  u8 NetworkBattleEvent::s_dbgLiveRecSendFlag = 0;
#endif

//--------------------------------------------------------------
/**
 * @brief  通信対戦受付アプリ呼び出し
 * 
 * @param   pGameManager  ゲームマネージャ
 * @param   pEventParam   イベント呼び出しパラメータ構造体
 */
//--------------------------------------------------------------
void NetworkBattleEvent::StartEvent( GameSys::GameManager* pGameManager, EVENT_PARAM* pEventParam )
{
  NetworkBattleEvent* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetworkBattleEvent>( pGameManager->GetGameEventManager() );

  // 本イベントの呼び出しパラメータのポインタセット
  p_event->SetEventParam( pEventParam );

  GFL_PRINT("!! 自分の立ち位置 %ds \n", pEventParam->standingPos ); 
}


// コンストラクタ
NetworkBattleEvent::NetworkBattleEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent(pHeap)
 ,m_pEventParam(NULL)
 ,m_sequence(SEQ_CALL_APP)
 ,m_isStartBattleBgm(false)
 ,m_callAppType(CALL_POKELIST)
 ,m_battleSetupParam()
 ,m_pokeListJoinParam()
 ,m_battleRoyalResultParam()
 ,m_battleVideoRecordingParam()
 ,m_battleRecorderSaveData( pHeap, GFL_SINGLETON_INSTANCE( GameSys::GameManager ) )
{
  for( int i=0; i<BTL_CLIENT_NUM; i++ ){
    m_pBattleParty[i] = NULL;
    m_pMiseaiData[i] = NULL;

    // バトルロイヤルリザルト名前用
    m_battleRoyalResultParam.aTrainerData[i].pNameBuf = GFL_NEW(pHeap) gfl2::str::StrBuf( System::STRLEN_PERSON_NAME + System::EOM_LEN, pHeap );
  }
}

// デストラクタ
NetworkBattleEvent::~NetworkBattleEvent()
{
  for( int i=0; i<BTL_CLIENT_NUM; i++ ){
    GFL_SAFE_DELETE( m_pBattleParty[i] );
    GFL_SAFE_DELETE( m_pMiseaiData[i] );

    // バトルロイヤルリザルト名前用
    GFL_SAFE_DELETE( m_battleRoyalResultParam.aTrainerData[i].pNameBuf );
  }
}


//--------------------------------------------------------------------------
bool NetworkBattleEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//--------------------------------------------------------------------------
void NetworkBattleEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  // 受け取ったポケパーティを全回復させておく
  for( int btlClientId=0; btlClientId<BTL_CLIENT_NUM; btlClientId++ )
  {
    PersonalData* pPersonalData = m_pEventParam->pPersonalData[ btlClientId ];
    if( pPersonalData == NULL )
    {
      // いなかったら終わり
      break;
    }

    if( pPersonalData->pPokeParty != NULL )
    {
      pPersonalData->pPokeParty->RecoverAll();
    }
  }

  m_pEventParam->pRegulation->DebugPrint();

  // 初期呼び出しアプリはNetworkBattle
  SetCallApp( CALL_POKELIST );
}


//--------------------------------------------------------------------------
void NetworkBattleEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  // BGM復元
  if( m_isStartBattleBgm != false )
  {
    Sound::EndBattleBGMReq();
  }


  // バトルセットアップ初期化で確保していたワークを解放
  BATTLE_SETUP_Clear( &m_battleSetupParam );
}


//--------------------------------------------------------------------------
GameSys::GMEVENT_RESULT NetworkBattleEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  // シーケンス制御
  switch( m_sequence ){
  case SEQ_CALL_APP:   //! アプリ呼び出し
    CallApp( pGameManager );
    break;
  case SEQ_RETURN_APP: //! アプリ戻り
    ReturnApp();
    break;
  case SEQ_END:        //! 終了
    return GameSys::GMEVENT_RES_FINISH;
  default:
    GFL_ASSERT(0);
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * @brief  呼び出すアプリ設定
 * @param  callAppType アプリ種類
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::SetCallApp( CallAppType callAppType )
{
  m_callAppType = callAppType;
  ICHI_PRINT("呼び出しアプリ %d \n", callAppType);

  m_sequence = SEQ_CALL_APP;
}

//--------------------------------------------------------------------------
/**
 * @brief  アプリ呼び出し

 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::CallApp( GameSys::GameManager* pGameManager )
{
  switch( m_callAppType ){
  case CALL_POKELIST:          //! 参加ポケモン選択
    {
      // ポケモンリストアプリ呼び出しパラメータの初期化
      InitializePokeListAppParam();
      // ポケモンリストイベント
      NetEvent::PokeListJoin::PokeListJoinEvent::StartEvent( pGameManager, &m_pokeListJoinParam );
    }
    break;

  case CALL_BEFORE_BATTLE_DEMO://! 戦闘前VSデモ
    {
      // BGM
      if( m_pEventParam->bgm == 0 )
      {//BGM設定なし（ライブ大会）
        // @fix GFMMCat[177]【ライブ大会】決勝以外のBGMがnijiのトレーナー戦になっている
        Sound::StartBattleBGMReq( STRM_BGM_VS_TRAINER_MJ );
      }
      else
      {//BGM設定あり（NBR、バトルスポット、ライブ大会のWCS決勝戦）
        Sound::StartBattleBGMReq( m_pEventParam->bgm );
      }
      m_isStartBattleBgm = true;

      // 戦闘前デモアプリ呼び出しパラメータの初期化
      App::AppDemo::AppDemoProcParam_VS vsDemoParam( m_pEventParam->pRegulation->GetBattleRule() );
      InitializeBeforeBattleDemoParam( vsDemoParam );
      NetEvent::VsDemoEvent::StartEvent( pGameManager, vsDemoParam );
    }
    break;

  case CALL_BATTLE:            //! バトル
    {
      // バトル呼び出しパラメータの初期化
      InitializeBattleSetupParam();

      btl::BATTLE_PROC_PARAM battleProcParam;
      battleProcParam.pAppMemory = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE);
      battleProcParam.pAppDeviceMemory = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE);
      battleProcParam.pGameManager = pGameManager;
      battleProcParam.pSetupParam = &m_battleSetupParam;
      GameSys::CallBattleProc( &battleProcParam);
      GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventProcessCall>( pGameManager->GetGameEventManager() );
    }
    break;

  case CALL_AFTER_BATTLE_DEMO: //! 戦闘後VSデモ
    {
      // 戦闘後デモアプリ呼び出しパラメータの初期化
      App::AppDemo::AppDemoProcParam_VS vsDemoParam( m_pEventParam->pRegulation->GetBattleRule() );
      InitializeAfterBattleDemoParam( vsDemoParam );
      NetEvent::VsDemoEvent::StartEvent( pGameManager, vsDemoParam );
    }
    break;

  case CALL_BATTLE_ROYAL_RESULT: //! バトルロイヤル結果
    {
      // @fix GFNMCat[4641]：ロイヤル結果画面のBGMを再生する。バトルスポットは再生しない。
      switch( m_pEventParam->pRegulation->GetBattleCategory() ){
      case Regulation::BATTLE_CATEGORY_NET_BTL:              //! 通信対戦(P2P)
      case Regulation::BATTLE_CATEGORY_DELIVERY:             //! 配信レギュレーション
        {
          if( m_isStartBattleBgm != false )
          {
            Sound::ChangeBattleBGMReq( STRM_BGM_WIN8 );
          }
          break;
        }
      }

      InitializeBattleRoyalResultSetupParam();
      App::Event::BattleRoyalResultEvent::StartEvent( pGameManager, &m_battleRoyalResultParam );
    }
    break;

  case CALL_BATTLE_VIDEO_SAVE: //! バトルビデオ録画確認
    {
      InitializeBattleVideoRecordingParam();
      BattleVideoRecording::Event::StartEvent( pGameManager, &m_battleVideoRecordingParam );
    }
    break;

  default:
    GFL_ASSERT(0);
    break;
  }

  // 終了待ち
  m_sequence = SEQ_RETURN_APP;
}

//--------------------------------------------------------------------------
/**
 * @brief  アプリ戻り
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::ReturnApp()
{
  // IsError は最後にチェックしています。

  switch( m_callAppType ){
  case CALL_POKELIST:          //! 参加ポケモン選択
    {
      // バトルへの参加ポケモン決定
      DecideBattleParty();

      // ライブ大会観戦用の選択ポケモンインデックス送信処理
      SendSelectedIndexForLiveCup();

      // 通常は先に進む
      SetCallApp( CALL_BEFORE_BATTLE_DEMO );
    }
    break;
  case CALL_BEFORE_BATTLE_DEMO://! 戦闘前VSデモ
    {
      SetCallApp( CALL_BATTLE );
    }
    break;
  case CALL_BATTLE:            //! バトル
    {
      /*  バトル結果受け取り  */
      if(m_pEventParam)
      {
        m_pEventParam->isAfterBattle    = true;
        // 不整合フラグ
        m_pEventParam->isIllegalBattle  = m_battleSetupParam.cmdIllegalFlag;
        // 勝敗
        m_pEventParam->battleResult     = m_battleSetupParam.result;
        GFL_PRINT("通信対戦の勝敗　%d \n", m_battleSetupParam.result);
        m_pEventParam->recHeader        = m_battleSetupParam.recHeader;

        // @fix NMCat[2698]：ホストマイグレーションだと通信エラーではないのでこのフラグで見る必要がある。
        m_pEventParam->isDisconnectOccur= m_battleSetupParam.isDisconnectOccur;

        // バトルに参加したポケモン数を格納
        for(u32 clientIndex = 0; clientIndex < BTL_CLIENT_NUM; ++clientIndex)
        {
          if(m_pBattleParty[clientIndex])
          {
            m_pEventParam->battlePartyMembers[clientIndex]  = m_pBattleParty[clientIndex]->GetMemberCount();
          }
        }


        /*  replayデータを構築  */
        if(m_pEventParam->pBattleRecorderSaveData)
        {
          m_pEventParam->pBattleRecorderSaveData->CreateSaveData(&m_battleSetupParam, m_pEventParam->pRegulation); /*  成否は保存時にハンドリングする想定  */
        }

        /*  PGL集計データを呼び出し元に返す */
        if(m_pEventParam->pglRecord) 
        {
          gfl2::std::MemCopy( &(m_battleSetupParam.PGL_Record), m_pEventParam->pglRecord, sizeof(m_battleSetupParam.PGL_Record)); 
          GFL_PRINT("---PGL集計データ設定(deadPokeNumber)\n"); 
          GFL_PRINT("   me: %d\n", m_battleSetupParam.PGL_Record.myParty[0].deadPokeNumber ); 
          GFL_PRINT("enemy: %d\n", m_battleSetupParam.PGL_Record.enemysParty[0].deadPokeNumber ); 
        }

        // サーバーバージョン
        if( BTL_NET_SERVER_VERSION != m_battleSetupParam.commServerVer )
        {
          m_pEventParam->isServerVersionHigh = true;
          GFL_PRINT(">>>バトルのサーバーバージョンと違った my:%d btl:%d<<<\n", BTL_NET_SERVER_VERSION, m_battleSetupParam.commServerVer);
        }

        m_pEventParam->commServerVer = m_battleSetupParam.commServerVer;
      }


      // バトル後のレコード加算
      AfterBattleRecordAdd();

      // ロイヤルの場合は専用リザルト画面
      CallAppType callAppType = ( m_pEventParam->pRegulation->GetBattleRule() == Regulation::BATTLE_RULE_ROYAL ) ? CALL_BATTLE_ROYAL_RESULT : CALL_AFTER_BATTLE_DEMO;
      SetCallApp( callAppType );
    }
    break;
  case CALL_AFTER_BATTLE_DEMO: //! 戦闘後VSデモ
  case CALL_BATTLE_ROYAL_RESULT: //! バトルロイヤル結果
    {
      // NBR、バトルスポットは録画確認へ
      switch( m_pEventParam->pRegulation->GetBattleCategory() ){
      case Regulation::BATTLE_CATEGORY_NET_BTL:              //! 通信対戦(P2P)
      case Regulation::BATTLE_CATEGORY_DELIVERY:             //! 配信レギュレーション
        {
          // 不正コマンドたっていたら録画出来ない
          if( m_battleSetupParam.cmdIllegalFlag == true )
          {
            m_sequence = SEQ_END;
          }
          else
          {
            SetCallApp( CALL_BATTLE_VIDEO_SAVE );
          }
        }
        break;
      case Regulation::BATTLE_CATEGORY_SPOT_FREE_LEGEND:     //! ランダムマッチ[フリー]伝説あり
      case Regulation::BATTLE_CATEGORY_SPOT_FREE:            //! ランダムマッチ[フリー]伝説無し
      case Regulation::BATTLE_CATEGORY_SPOT_RATE:            //! ランダムマッチ[レーティング]
      case Regulation::BATTLE_CATEGORY_SPOT_SP:              //! ランダムマッチ[スペシャル]
      case Regulation::BATTLE_CATEGORY_WCS:                  //! ランダムマッチ[レーティング]WCSルール
      case Regulation::BATTLE_CATEGORY_NET_CUP:              //! インターネット大会
      case Regulation::BATTLE_CATEGORY_GROUP_CUP:            //! 仲間大会
        {//バトルビデオ保存はログ送信やP2P切断後に行う
          m_sequence = SEQ_END;
        }
      break;
      default:
        m_sequence = SEQ_END;
        break;
      }
    }
    break;
  case CALL_BATTLE_VIDEO_SAVE: //! バトルビデオ録画確認
    m_sequence = SEQ_END;
    break;
  default:
    GFL_ASSERT(0);
    m_sequence = SEQ_END;
    break;
  }


  // 全てのアプリにおいて通信エラーだった場合は即終了
  //  戻り値を格納したい為、タイミングはここです。
  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
  {
    m_sequence = SEQ_END;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief  ポケモンリストアプリ呼び出しパラメータの初期化
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::InitializePokeListAppParam()
{
  gfl2::std::MemClear( &m_pokeListJoinParam, sizeof(NetEvent::PokeListJoin::EVENT_PARAM) );
  // レギュレーション
  m_pokeListJoinParam.pRegulation = m_pEventParam->pRegulation;
  // 個々人のデータ
  for( int i=0; i<BTL_CLIENT_NUM; i++ ) {
    m_pokeListJoinParam.pPersonalData[i] = m_pEventParam->pPersonalData[i];
  }
  // 戦闘立ち位置
  m_pokeListJoinParam.standingPos = m_pEventParam->standingPos; 
}

//--------------------------------------------------------------------------
/**
 * @brief  戦闘前デモアプリ呼び出しパラメータの初期化
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::InitializeBeforeBattleDemoParam( App::AppDemo::AppDemoProcParam_VS& procParam )
{
  procParam.demoType = 0; // VSBATTLE_PRESTART

  for( int btlClientId=0; btlClientId<BTL_CLIENT_NUM; btlClientId++ )
  {
    // 渡されているプレイヤー情報
    PersonalData* pPersonalData = m_pEventParam->pPersonalData[ btlClientId ];

    // ないならSkip
    if( pPersonalData == NULL ){ continue; }

    // 立ち位置とデモのインデックス違う・・・
    const int demoIndex = BtlClientId2DemoIndex( btlClientId );

    // MyStatus
    procParam.pMyStatus[ demoIndex ] = pPersonalData->pMyStatus;

    // ポケモンデータ
    // ポケモン状態(0:無効値 1:通常 2:状態異常 3:瀕死）
    {
      pml::PokeParty* pParty = m_pBattleParty[btlClientId];
      for( u32 pokeIndex=0; pokeIndex<pParty->GetMemberCount(); pokeIndex++ )
      {
        procParam.pokemonStatus[demoIndex][pokeIndex] = 1;
      }
    }
  }

  // 背景タイプ (0:通常背景 1:WCS予選 2:WCS決勝）
  procParam.bgType = 0;
  if( m_pEventParam->pRegulation->IsWcs() )
  {
    if( m_pEventParam->pRegulation->GetBackGroundType() == Regulation::BACKGROUND_WCS1 )
    {
      procParam.bgType = 1;
    }
    else if( m_pEventParam->pRegulation->GetBackGroundType() == Regulation::BACKGROUND_WCS2 )
    {
      procParam.bgType = 2;
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief  戦闘後デモアプリ呼び出しパラメータの初期化
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::InitializeAfterBattleDemoParam( App::AppDemo::AppDemoProcParam_VS& procParam )
{
  // デモは、勝ち／負け／降参の3種類。勝ちを流すとWinが左に出る為、
  // ここで、自分がどこにいて勝敗はどうたったのかを判断してからデモ種類を決める必要がある。
  {
    // デモタイプの値メモ
    // 0:戦闘前デモ
    // 1:VSBATTLE_WIN
    // 2:VSBATTLE_LOSE
    // 3:VSBATTLE_DRAW

    switch( m_pEventParam->standingPos ){
    case BTL_CLIENT_PLAYER:
    case BTL_CLIENT_PARTNER:
      { // 自分は左
        switch( m_battleSetupParam.result ){
        case BTL_RESULT_LOSE:
        case BTL_RESULT_RUN:
          procParam.demoType = 2;
          break;
        case BTL_RESULT_WIN:
        case BTL_RESULT_RUN_ENEMY:
          procParam.demoType = 1;
          break;
        case BTL_RESULT_DRAW:
          procParam.demoType = 3;
          break;
        default:
          GFL_ASSERT(0);
          procParam.demoType = 2;
          break;
        }
      }
      break;

    case BTL_CLIENT_ENEMY1:
    case BTL_CLIENT_ENEMY2:
      { // 自分は右
        switch( m_battleSetupParam.result ){
        case BTL_RESULT_LOSE:
        case BTL_RESULT_RUN:
          procParam.demoType = 1;
          break;
        case BTL_RESULT_WIN:
        case BTL_RESULT_RUN_ENEMY:
          procParam.demoType = 2;
          break;
        case BTL_RESULT_DRAW:
          procParam.demoType = 3;
          break;
        default:
          GFL_ASSERT(0);
          procParam.demoType = 2;
          break;
        }
      }
      break;
    default:
      GFL_ASSERT(0);
      procParam.demoType = 2;
      break;
    }
  }

  for( int btlClientId=0; btlClientId<BTL_CLIENT_NUM; btlClientId++ )
  {
    // 渡されているプレイヤー情報
    PersonalData* pPersonalData = m_pEventParam->pPersonalData[ btlClientId ];

    // ないならSkip
    if( pPersonalData == NULL ){ continue; }

    // 立ち位置とデモのインデックス違う・・・
    const int demoIndex = BtlClientId2DemoIndex( btlClientId );

    // MyStatus
    procParam.pMyStatus[ demoIndex ] = pPersonalData->pMyStatus;

    // ポケモンデータ
    // ポケモン状態(0:無効値 1:通常 2:状態異常 3:瀕死）
    {
      pml::PokeParty* pParty = m_pBattleParty[btlClientId];
      for( u32 pokeIndex=0; pokeIndex<pParty->GetMemberCount(); pokeIndex++ )
      {
        procParam.pokemonStatus[demoIndex][pokeIndex] = m_battleSetupParam.party_state[btlClientId][pokeIndex] + 1; // 1オリジンにあわせる
      }
    }
  }


  // 背景タイプ (0:通常背景 1:WCS予選 2:WCS決勝）
  procParam.bgType = 0;
  if( m_pEventParam->pRegulation->IsWcs() )
  {
    if( m_pEventParam->pRegulation->GetBackGroundType() == Regulation::BACKGROUND_WCS1 )
    {
      procParam.bgType = 1;
    }
    else if( m_pEventParam->pRegulation->GetBackGroundType() == Regulation::BACKGROUND_WCS2 )
    {
      procParam.bgType = 2;
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief  バトルロイヤル結果アプリのパラメータ初期化
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::InitializeBattleRoyalResultSetupParam()
{
  ICHI_PRINT(">ICHI ----ロイヤルリザルトパラメータ----\n");

  //  プレイヤー自身の順位（１オリジンに変換）
  m_battleRoyalResultParam.selfRanking = m_battleSetupParam.battleRoyalResult.clientRanking[ m_pEventParam->standingPos ] + 1;
  ICHI_PRINT(">ICHI プレイヤーの順位 %d \n", m_battleRoyalResultParam.selfRanking);

  // 各プレイヤーのリザルト情報
  for( int i=0; i<BTL_CLIENT_NUM; i++ )
  {
    ICHI_PRINT(">ICHI =========================== \n");
    App::BattleRoyalResult::TRINER_BTL_RESULT_DATA* pTrainer = &m_battleRoyalResultParam.aTrainerData[ i ]; 
    // 渡されているプレイヤー情報
    PersonalData* pPersonalData = m_pEventParam->pPersonalData[ i ];

    // 順位（1～4）（１オリジンに変換）
    pTrainer->ranking = m_battleSetupParam.battleRoyalResult.clientRanking[ i ] + 1;
    ICHI_PRINT(">ICHI 順位 %d \n", pTrainer->ranking );
    // トレーナータイプ（顔アイコン表示の指針になる）
    pTrainer->trainerType = pPersonalData->pMyStatus->GetSex() == PM_MALE ? trainer::TRTYPE_HERO : trainer::TRTYPE_HEROINE;
    ICHI_PRINT(">ICHI トレーナータイプ %d \n", pTrainer->trainerType );
    // トレーナー顔アイコンデータ(主人公の男女のアイコンを表示する場合は設定)
    pTrainer->iconObjData = pPersonalData->pMyStatus->GetIcon();
    // コーナーの色
    pTrainer->correrColor = GetCornerColorFromBtlClientId( static_cast<BTL_CLIENT_ID>(i) );
    ICHI_PRINT(">ICHI コーナーの色 %d \n", pTrainer->correrColor );
    // 名前
    pPersonalData->pMyStatus->GetNameString( pTrainer->pNameBuf );
    gfl2::str::PrintCode( pTrainer->pNameBuf->GetPtr(), ">ICHI 名前：" );
    // ポケモンを倒した数
    pTrainer->killCount = m_battleSetupParam.battleRoyalResult.killCount[i];

    // ポケモンデータ
    {
      pml::PokeParty* pParty = m_pBattleParty[i];
      for( u32 pokeIndex=0; pokeIndex<pParty->GetMemberCount(); pokeIndex++ )
      {
        PokeTool::GetSimpleParam( &pTrainer->aPokeData[ pokeIndex ].simpleParam, pParty->GetMemberPointer(pokeIndex) );
        pTrainer->aPokeData[ pokeIndex ].maxHp = m_battleSetupParam.battleRoyalResult.maxHp[i][pokeIndex];
        pTrainer->aPokeData[ pokeIndex ].hp = m_battleSetupParam.battleRoyalResult.restHp[i][pokeIndex];
        ICHI_PRINT(">ICHI Pokemon[%d] maxHp:%d restHp:%d \n", pokeIndex, pTrainer->aPokeData[ pokeIndex ].maxHp, pTrainer->aPokeData[ pokeIndex ].hp );
      }
    }
  }

}

//--------------------------------------------------------------------------
/**
 * @brief  バトルビデオ録画アプリのパラメータ初期化
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::InitializeBattleVideoRecordingParam()
{
  // バトルで使用したバトルサーバーバージョンを渡す
  m_battleVideoRecordingParam.in.appInParam.saveDataServerVersion = m_battleSetupParam.commServerVer;
  // 切断が発生していたら録画できなくする
  m_battleVideoRecordingParam.in.appInParam.bRecNG = m_battleSetupParam.isDisconnectOccur;

  // BattleSetupParamとレギュレーションからバトルビデオセーブデータ作成
  m_battleRecorderSaveData.CreateSaveData( &m_battleSetupParam, m_pEventParam->pRegulation );
  m_battleVideoRecordingParam.in.appInParam.pRecorderSaveData = &m_battleRecorderSaveData;
}


//--------------------------------------------------------------------------
/**
 * @brief  バトル呼び出しパラメータの初期化
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::InitializeBattleSetupParam()
{
  gfl2::std::MemClear( &m_battleSetupParam, sizeof(BATTLE_SETUP_PARAM) );

  BTL_FIELD_SITUATION bfs;
  BATTLE_SETUP_FIELD_SITUATION_Init( &bfs );
  bfs.player_rotate = 0;

  // 通信タイプ
  BtlCommMode commMode;
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsWifi() )
  {
    commMode = BTL_COMM_WIFI;
  }
  else if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsIr() )
  {//ライブ大会
    commMode = BTL_COMM_IR;
  }
  else
  {
    commMode = BTL_COMM_WIRELESS;
  }

  // 通信対戦セットアップ
  BATTLE_SETUP_Comm( &m_battleSetupParam, 
                     GFL_SINGLETON_INSTANCE( GameSys::GameManager ),
                     &bfs,
                     commMode,
                     m_pEventParam->standingPos,
                     GetWorkHeap() );

  // 設定したポケパーティをセット
  BATTLE_PARAM_SetPokeParty( &m_battleSetupParam, m_pBattleParty[ BTL_CLIENT_PLAYER ], BTL_CLIENT_PLAYER );
  BATTLE_PARAM_SetPokeParty( &m_battleSetupParam, m_pBattleParty[ BTL_CLIENT_ENEMY1 ], BTL_CLIENT_ENEMY1 );

  // マルチの場合
  if( m_pEventParam->pRegulation->GetBattleRule() == Regulation::BATTLE_RULE_MULTI )
  {
    BATTLE_PARAM_SetPokeParty( &m_battleSetupParam, m_pBattleParty[ BTL_CLIENT_PARTNER ], BTL_CLIENT_PARTNER );
    BATTLE_PARAM_SetPokeParty( &m_battleSetupParam, m_pBattleParty[ BTL_CLIENT_ENEMY2 ], BTL_CLIENT_ENEMY2 );
    BATTLE_SETUP_ToMultiMode( &m_battleSetupParam );
  }
  // ロイヤルの場合
  else if( m_pEventParam->pRegulation->GetBattleRule() == Regulation::BATTLE_RULE_ROYAL )
  {
    BATTLE_PARAM_SetPokeParty( &m_battleSetupParam, m_pBattleParty[ BTL_CLIENT_PARTNER ], BTL_CLIENT_PARTNER );
    BATTLE_PARAM_SetPokeParty( &m_battleSetupParam, m_pBattleParty[ BTL_CLIENT_ENEMY2 ], BTL_CLIENT_ENEMY2 );
  }

  // レギュレーション通りのバトル設定にする
  BATTLE_PARAM_SetRegulation( &m_battleSetupParam, m_pEventParam->pRegulation, GetWorkHeap() );

  //お手入れ禁止
  BATTLE_PARAM_SetBtlStatusFlag( &m_battleSetupParam, BTL_STATUS_FLAG_OTEIRE_DISABLE );

  // 戦闘曲を選んでいた場合は上書き
  if( m_pEventParam->bgm != 0 )
  {
    m_battleSetupParam.btlEffData.bgm_default = m_pEventParam->bgm; 
  }

  // レート値
  if( m_pEventParam->pRegulation->IsRateBattle() )
  {
    BATTLE_PARAM_SetRatingValue( &m_battleSetupParam,
                                  m_pEventParam->pPersonalData[ BTL_CLIENT_PLAYER ]->rate,
                                  m_pEventParam->pPersonalData[ BTL_CLIENT_ENEMY1 ]->rate );
  }

  // 見せ合いデータ(niji新規)
  SetupMiseaiData();

#if PM_DEBUG
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsIr() )
  {//ライブ大会

    /*
      GFNMcat[5717]   ライブ大会：観戦機能が正常に動作しない
        RegulationのWatchフラグがtrueの場合、各所でm_battleSetupParam.isLiveRecSendEnableがtrueにされる
          s_dbgLiveRecSendFlag == true  : Regulation.Watch にかかわらず、 isLiveRecSendEnable == true
          s_dbgLiveRecSendFlag == false : Regulation.Watch に準拠
        とするため、修正
    */
    if( s_dbgLiveRecSendFlag )
    {
      /*  デバッグ時：trueで上書き  */
      m_battleSetupParam.isLiveRecSendEnable = true;
      GFL_PRINT("%s: isLiveRecSendEnable = %d\n", __FUNCTION__, m_battleSetupParam.isLiveRecSendEnable);
    }
  }
#endif

}

//--------------------------------------------------------------------------
/**
 * @brief  バトルへの参加ポケモン決定
 *         ポケモンリストで参加選択ポケモンのインデックスが決定したので、
 *         フルパーティから参観順のポケモンパーティを作成する。
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::DecideBattleParty()
{
  // BTL_CLIENT人数分
  for( int btlClientId=0; btlClientId<BTL_CLIENT_NUM; btlClientId++ )
  {
    PersonalData* pPersonalData = m_pEventParam->pPersonalData[ btlClientId ];
    if( pPersonalData == NULL )
    {
      // いなかったら終わり
      break;
    }

    m_pBattleParty[ btlClientId ] = GFL_NEW( GetWorkHeap() ) pml::PokeParty( GetWorkHeap() );
    m_pBattleParty[ btlClientId ]->Clear();

    GFL_PRINT("参加ポケモン BTL_CLIENT_ID %d \n", btlClientId );
    // 参加ポケモンインデックス
    for( int i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      int idx = m_pokeListJoinParam.join_order[ btlClientId ][ i ];
      if( idx == -1 )
      {
        break;
      }

      const pml::pokepara::PokemonParam *pp = pPersonalData->pPokeParty->GetMemberPointer( idx );
      GFL_PRINT("%d番目:パーティIdx=%d monsno:%d \n", i+1, idx, pp->GetMonsNo() );

      // 戦闘パーティに追加
      m_pBattleParty[ btlClientId ]->AddMember( *pp );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief  ライブ大会観戦用の選択ポケモンインデックス送信処理
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::SendSelectedIndexForLiveCup()
{
  /*
    GFNMcat[5717]   ライブ大会：観戦機能が正常に動作しない
      RegulationのWatchフラグがtrueの場合、各所でm_battleSetupParam.isLiveRecSendEnableがtrueにされる
        s_dbgLiveRecSendFlag == true  : Regulation.Watch にかかわらず、 isLiveRecSendEnable == true
        s_dbgLiveRecSendFlag == false : Regulation.Watch に準拠
      とするため、修正
  */

  /*  通常のRegulation設定参照  */
  m_battleSetupParam.isLiveRecSendEnable  = m_pEventParam->pRegulation->GetBoolParam( Regulation::WATCH );


#if PM_DEBUG
  if(s_dbgLiveRecSendFlag)
  {
    /*  デバッグ時：trueで上書き  */
    m_battleSetupParam.isLiveRecSendEnable = true;
    GFL_PRINT("%s: isLiveRecSendEnable = %d\n", __FUNCTION__, m_battleSetupParam.isLiveRecSendEnable);
  }
#endif

  if(m_battleSetupParam.isLiveRecSendEnable)
  {
    NetLib::NijiNetworkSystem::SetupLiveRecSender( m_pEventParam->standingPos , &m_battleSetupParam );
    NetLib::NijiNetworkSystem::SendLiveRecSelectedIndex( m_pokeListJoinParam.join_order );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief  見せ合いデータ設定
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::SetupMiseaiData()
{

  // BTL_CLIENT人数分
  for( int btlClientId=0; btlClientId<BTL_CLIENT_NUM; btlClientId++ )
  {
    PersonalData* pPersonalData = m_pEventParam->pPersonalData[ btlClientId ];
    if( pPersonalData == NULL )
    {
      // いなかったら終わり
      break;
    }

    m_pMiseaiData[ btlClientId ] = GFL_NEW( GetWorkHeap() ) MiseaiData(); 

    // フルポケパーティ
    m_pMiseaiData[ btlClientId ]->partyFullMember = pPersonalData->pPokeParty;

    // 各メンバーが何番目のポケモンとして選出されたか
    //  memo:join_order は選出順にポケモンインデックスが格納されている
    for( int i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      int idx = m_pokeListJoinParam.join_order[ btlClientId ][ i ];
      if( idx == -1 )
      {
        break;
      }

      m_pMiseaiData[ btlClientId ]->memberSelectedIndex[ idx ] = i;
    }


    // 見せ合いデータポインタセット
    m_battleSetupParam.miseaiData[ btlClientId ] = m_pMiseaiData[ btlClientId ];
  }
}


//--------------------------------------------------------------------------
/**
 * @brief  バトルクライアントIDをデモIndexにする
 * @param  btlClientId  立ち位置
 * @param  デモの個人パラメータのIndex
 */
//--------------------------------------------------------------------------
u32 NetworkBattleEvent::BtlClientId2DemoIndex( const int btlClientId )
{
  // 立ち位置とデモのインデックス違う・・・
  int demoIndex = 0;

  // ロイヤルの場合
  if( m_pEventParam->pRegulation->GetBattleRule() == Regulation::BATTLE_RULE_ROYAL )
  {
    // ロイヤルは色なので同じ
    demoIndex = btlClientId;
  }
  // マルチの場合
  else if( m_pEventParam->pRegulation->GetBattleRule() == Regulation::BATTLE_RULE_MULTI )
  {
    switch( btlClientId ){
    case BTL_CLIENT_PLAYER: demoIndex = 0; break;
    case BTL_CLIENT_ENEMY1: demoIndex = 2; break;
    case BTL_CLIENT_PARTNER:demoIndex = 1; break;
    case BTL_CLIENT_ENEMY2: demoIndex = 3; break;
    }
  }
  else
  {
    switch( btlClientId ){
    case BTL_CLIENT_PLAYER: demoIndex = 0; break;
    case BTL_CLIENT_ENEMY1: demoIndex = 1; break;

    // ↓シングルなので使われないが、↑の値とかぶると上書きされるので、別のIndexにしておく必要がある。
    case BTL_CLIENT_PARTNER:demoIndex = 2; break;
    case BTL_CLIENT_ENEMY2: demoIndex = 3; break;
    }
  }

  return demoIndex;
}

//--------------------------------------------------------------------------
/**
 * @brief  バトル後のレコード加算
 */
//--------------------------------------------------------------------------
void NetworkBattleEvent::AfterBattleRecordAdd()
{
  enum BATTLE_RESULT{
    BATTLE_RESULT_WIN,
    BATTLE_RESULT_LOSE,
    BATTLE_RESULT_DRAW,
  };

  // 勝敗結果
  BATTLE_RESULT battleResult;
  switch( m_battleSetupParam.result ){
  case BTL_RESULT_LOSE:
  case BTL_RESULT_RUN:
    battleResult = BATTLE_RESULT_LOSE;
    break;
  case BTL_RESULT_WIN:
  case BTL_RESULT_RUN_ENEMY:
    battleResult = BATTLE_RESULT_WIN;
    break;
  case BTL_RESULT_DRAW:
    battleResult = BATTLE_RESULT_LOSE;
    break;
  default:
    return;
  }


  // P2P対戦
  switch( m_pEventParam->pRegulation->GetBattleCategory() ){
  case Regulation::BATTLE_CATEGORY_NET_BTL:              //! 通信対戦(P2P)
  case Regulation::BATTLE_CATEGORY_DELIVERY:             //! 配信レギュレーション
    {
      if( battleResult == BATTLE_RESULT_WIN )
      {
        // P2P対戦の勝利数
        Savedata::IncRecord( Savedata::Record::RECID_COMM_BTL_WIN );
      }
      else if( battleResult == BATTLE_RESULT_LOSE )
      {
        // P2P対戦の敗北数
        Savedata::IncRecord( Savedata::Record::RECID_COMM_BTL_LOSE );
      }

      // P2P対戦をした回数
      Savedata::IncRecord( Savedata::Record::RECID_COMM_BATTLE );

      // ダイアリー
      PokeDiary::Set( PDID_COUNT_P2PBATTLE, (u16)m_pEventParam->pRegulation->GetBattleRule() );
    }
    break;

  // バトルスポット
  case Regulation::BATTLE_CATEGORY_SPOT_FREE_LEGEND:     //! ランダムマッチ[フリー]伝説あり
  case Regulation::BATTLE_CATEGORY_SPOT_FREE:            //! ランダムマッチ[フリー]伝説無し
  case Regulation::BATTLE_CATEGORY_SPOT_RATE:            //! ランダムマッチ[レーティング]
  case Regulation::BATTLE_CATEGORY_SPOT_SP:              //! ランダムマッチ[スペシャル]
  case Regulation::BATTLE_CATEGORY_NET_CUP:              //! インターネット大会
  case Regulation::BATTLE_CATEGORY_GROUP_CUP:            //! 仲間大会
  case Regulation::BATTLE_CATEGORY_WCS:                  //! ランダムマッチ[レーティング]WCSルール
    {
      if( battleResult == BATTLE_RESULT_WIN )
      {
        // バトルスポットの勝利数
        Savedata::IncRecord( Savedata::Record::RECID_WIFI_BTL_WIN );
      }
      else if( battleResult == BATTLE_RESULT_LOSE )
      {
        // バトルスポットの敗北数
        Savedata::IncRecord( Savedata::Record::RECID_WIFI_BTL_LOSE );
      }

      // バトルスポットをした回数
      Savedata::IncRecord( Savedata::Record::RECID_WIFI_BATTLE );
    }
    break;
  }


  // バトルロイヤル
  if( m_pEventParam->pRegulation->GetBattleRule() == Regulation::BATTLE_RULE_ROYAL )
  {
    // バトルロイヤルを行った回数
    Savedata::IncRecord( Savedata::Record::RECID_BATTLEROYAL );

    // 自分のランキングが0ならば1位
    if( m_battleSetupParam.battleRoyalResult.clientRanking[ m_pEventParam->standingPos ] == 0 )
    {
      // バトルロイヤルの勝利数
      Savedata::IncRecord( Savedata::Record::RECID_BATTLEROYAL_WIN );
    }
  }
}


GFL_NAMESPACE_END(NetworkBattle)
GFL_NAMESPACE_END(NetEvent)
