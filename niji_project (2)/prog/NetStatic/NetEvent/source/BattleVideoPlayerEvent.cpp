// ============================================================================
/*
 * @file BattleVideoPlayerEvent.cpp
 * @brief バトルビデオ再生アプリ起動イベント
 * @date 2015.09.24
 * @author yuto_uchida
 */
// ============================================================================
#include "NetStatic/NetEvent/include/BattleVideoPlayerEvent.h"

//  nijiのインクルード
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "System/include/HeapDefine.h"
#include "GameSys/include/DllProc.h"
#include "GameSys/include/GameData.h"
#include "Battle/Regulation/include/PokeRegulation.h"
#include "PokeTool/include/PokeToolBattleParty.h"
#include "Debug/StartMenu/include/StartLauncher.h"
#include "Savedata/include/PokeDiarySave.h"

//  proc
#include <Battle/include/battle_proc.h>

//  ローカルと通信でprocを分ける
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerLocalProc.h"
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerNetProc.h"

#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"

//  イベントのインクルード
#include "NetStatic/NetEvent/include/TeamSelectEvent.h"

//  リソース
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  // コンストラクタ
  Event::Event( gfl2::heap::HeapBase* pHeap ) : NetEvent::NetEventBase( pHeap )
  {
    _Clear();
  }

  // デストラクタ
  Event::~Event()
  {
  }

  //  イベント開始
  void Event::StartEvent(GameSys::GameManager* gmgr, const PlayerDataType dataType, const bool bDebugFlg )
  {
    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    Event*  pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Event>(pGameEventManager);

#if PM_DEBUG
    pEvent->m_bDebug  = bDebugFlg;
#endif
    pEvent->SetPlayerDataType( dataType );
  }

  void Event::InitFunc( GameSys::GameManager* pGameManager )
  {
    UCHIDA_PRINT( "Video Plaayer Event Start!\n" );

    //  通信モードをきめる
    NetAppLib::BattleVideoPlayer::VideoDataManager::NetMode  netMode = NetAppLib::BattleVideoPlayer::VideoDataManager::NET_MODE_OFFLINE;
    if( m_playerDataType == PLAYAER_DATA_TYPE_LOCAL )
    {
      netMode = NetAppLib::BattleVideoPlayer::VideoDataManager::NET_MODE_OFFLINE;
    }
    else
    {
      netMode = NetAppLib::BattleVideoPlayer::VideoDataManager::NET_MODE_ONLINE;
    }
    
    m_themSelectEventAppParam.in.pRegulationDrawInfo    = NULL;
    //  ビデオデータの１００件＋シリアル１件の１０１件のデータバッファ作成。
    //  それでも１M近くしかならない(状況によっては削る必要もある)
    {
      m_pLocalHeap  = GFL_CREATE_LOCAL_HEAP( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ), 1024 * 1024 * 2, gfl2::heap::HEAP_TYPE_EXP, false );
    }
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    {
      m_pLocalHeap->PrintHeapInfo( "BattleVideoPlayerEvent VideoDataManager Create Befear in InitFunc\n" );
    }
#endif

    m_appParam.pVideoDataManager  = GFL_NEW( m_pLocalHeap ) NetAppLib::BattleVideoPlayer::VideoDataManager( m_pLocalHeap, netMode );

#ifdef DEBUG_ONLY_FOR_uchida_yuto
    {
      m_pLocalHeap->PrintHeapInfo( "BattleVideoPlayerEvent VideoDataManager Create After in InitFunc\n" );
    }

    {
      m_pLocalHeap->PrintHeapInfo( "BattleVideoPlayerEvent PokeTool::BattleParty Create Befear in InitFunc\n" );
    }
#endif

    m_pBtlParty = GFL_NEW( m_pLocalHeap ) PokeTool::BattleParty( m_pLocalHeap );
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    {
      m_pLocalHeap->PrintHeapInfo( "BattleVideoPlayerEvent PokeTool::BattleParty Create After in InitFunc\n" );
    }
#endif
  }

  void Event::EndFunc( GameSys::GameManager* pGameManager )
  {
    GFL_SAFE_DELETE( m_pBtlParty );
    GFL_SAFE_DELETE( m_themSelectEventAppParam.in.pRegulationDrawInfo );
    GFL_SAFE_DELETE( m_appParam.pVideoDataManager );

    if( m_pLocalHeap != NULL )
    {
      GFL_DELETE_HEAP( m_pLocalHeap );
      m_pLocalHeap  = NULL;
    }

    UCHIDA_PRINT( "Video Player Event End!\n" );
  }

  GameSys::GMEVENT_RESULT Event::MainFunc( GameSys::GameManager* pGameManager )
  {
    s32 seq = GameSys::GameEvent::GetSeqNo();
    if( seq == SEQ_NONE )
    {
      if( pGameManager->IsProcessExists() == false )
      {
        seq = _StartSeq( SEQ_VIDEO_PLAYER_PROC, pGameManager );
      }
    }
    else
    if( seq == SEQ_VIDEO_PLAYER_PROC )
    {
      //  procが死んでいるかチェック
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pProc ) == false )
      {
        if( m_appParam.out.procResultType == NetApp::BattleVideoPlayer::PROC_RESULT_TYPE_FATAL || 
            IsFatalError() )
        {
          //  フェイタル
          m_fatalEventParam.eFatalErrorNo = NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED;
          NetEvent::FatalError::FatalErrorEvent::StartEvent( pGameManager, m_fatalEventParam );          

          seq = _StartSeq( SEQ_ERROR_NET_WAIT, pGameManager );
        }
        else
        if( IsError() == true )
        {
          //  通信エラー
          seq = _StartSeq( SEQ_ERROR_NET_WAIT, pGameManager );
        }
        else
        if( m_appParam.out.procResultType == NetApp::BattleVideoPlayer::PROC_RESULT_TYPE_CANCEL )
        {
          //  キャンセル
          seq = _StartSeq( SEQ_END, pGameManager );
        }
        else
        if( ( m_appParam.out.procResultType == NetApp::BattleVideoPlayer::PROC_RESULT_TYPE_CALL_BTL_AI )
        ||  ( m_appParam.out.procResultType == NetApp::BattleVideoPlayer::PROC_RESULT_TYPE_CALL_BTL_PLAY ) )
        {
          GFL_ASSERT( m_appParam.out.pActiveVideoData );

          //  パラメータを設定
          {
            ExtSavedata::BattleRecorderSaveData*  pSaveData = m_appParam.pVideoDataManager->GetBtlRecorderSaveData();

             // アップロードデータを設定
            pSaveData->SetBattleRecUploadData( &m_appParam.out.pActiveVideoData->sdCardData.upload );

             // バトルビデオIDセット
            pSaveData->SetBattleVideoID( m_appParam.out.pActiveVideoData->sdCardData.dataID );

             // バトルビデオアップロードフラグセット
            pSaveData->SetBattleVideoUploadFlag( m_appParam.out.pActiveVideoData->sdCardData.upload_flag );

             // バトルビデオの再生回数をセット
            pSaveData->SetBattleVideoPlayCount( m_appParam.out.pActiveVideoData->sdCardData.playCount );

             // バトルビデオアップロード登録日をセット
            pSaveData->SetBattleVideoRegistDate( &m_appParam.out.pActiveVideoData->registDate );

            //  バトルで再生するBGMIDを設定
            m_battleVideoCallParam.in.bgmDefaultId  = m_appParam.out.pActiveVideoData->sdCardData.upload.body.setupSubset.battleEffect.bgm_default;
          }

          //  模擬戦起動
          if( m_appParam.out.procResultType == NetApp::BattleVideoPlayer::PROC_RESULT_TYPE_CALL_BTL_AI )
          {
            seq = _StartSeq( SEQ_BTL_TAME_PROC, pGameManager );
          }
          else
          //  再生起動
          if( m_appParam.out.procResultType == NetApp::BattleVideoPlayer::PROC_RESULT_TYPE_CALL_BTL_PLAY )
          {
            //  バトルデータを再生
            seq = _StartSeq( SEQ_BTL_PROC, pGameManager );
          }
        }
        else
        {
          seq = _StartSeq( SEQ_END, pGameManager );
        }
      }
    }
    else
    if( seq == SEQ_BTL_TAME_PROC )
    {
      if( IsError() == true )
      {
        //  通信エラー
        seq = _StartSeq( SEQ_ERROR_NET_WAIT, pGameManager );
      }
      else
      if( m_themSelectEventAppParam.out.result == NetApp::TeamSelect::EVENT_APP_RESULT_TEAM_SELECT )
      {
        GFL_SAFE_DELETE( m_themSelectEventAppParam.in.pRegulationDrawInfo );

        // 選んだチームのバトルパーティ作成
        {
          const Savedata::BOX* pBox = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX();
          Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

          // @fix NMCat[494] ポケモンパーティーをいったんクリアしないといけない
          m_pBtlParty->GetPokeParty()->Clear();
          m_pBtlParty->CreateBattleParty( pBox, pBoxPokemon, m_themSelectEventAppParam.out.teamIdx, m_pLocalHeap );
        }

        //  模擬戦起動
        //  模擬戦に合わせてパーティー構成
        if( _CreateDuelPokemonData( m_pBtlParty->GetPokeParty() ) == true )
        {
          seq = _StartBtlAISeq( m_pBtlParty->GetPokeParty(), pGameManager );
        }
        else
        {
          //  模擬戦失敗
          m_appParam.in.procRunType = NetApp::BattleVideoPlayer::PROC_RUN_TYPE_RETURN;
          seq = _StartSeq( SEQ_VIDEO_PLAYER_PROC, pGameManager );
        }
      }
      else
      if( m_themSelectEventAppParam.out.result == NetApp::TeamSelect::EVENT_APP_RESULT_CANCEL )
      {
        GFL_SAFE_DELETE( m_themSelectEventAppParam.in.pRegulationDrawInfo );

        //  レコーダーに戻る
        m_appParam.in.procRunType = NetApp::BattleVideoPlayer::PROC_RUN_TYPE_RETURN;

        seq = _StartSeq( SEQ_VIDEO_PLAYER_PROC, pGameManager );
      }
    }
    else
    if( ( seq == SEQ_BTL_PROC )
    ||  ( seq == SEQ_BTL_AI_PROC ) )
    {
      if( m_battleVideoCallParam.out.bEventEnd == true )
      {
        //  バトル起動中
        m_appParam.in.procRunType       = NetApp::BattleVideoPlayer::PROC_RUN_TYPE_RETURN;
        m_appParam.in.bPlayCompletion   = m_battleVideoCallParam.out.bCompPlay;

        _DestroyBtlSetupParam( &m_battleVideoCallParam.in.btlSetupParam );

        // @fix NMCat[1872] バトル終了後に通信エラーになっているか追加
        if( IsError() == true )
        {
          //  通信エラー
          seq = _StartSeq( SEQ_ERROR_NET_WAIT, pGameManager );
        }
        else
        {
          seq = _StartSeq( SEQ_VIDEO_PLAYER_PROC, pGameManager );
        }
      }
    }
    else
    if( seq == SEQ_ERROR_NET_WAIT )
    {
      seq = _StartSeq( SEQ_END, pGameManager );
    }

    if( seq == SEQ_END )
    {
      // @fix GFNMCat[2298] セーブ・ロードの終了処理待ちをする
      if( m_appParam.pVideoDataManager->EndFunc() == false )
      {
        return GameSys::GMEVENT_RES_CONTINUE;
      }

#if PM_DEBUG
      if( m_bDebug )
      { // ランチャー
        debug::launcher::LauncherProc::InitCallParam( &m_launcherParam );
        debug::launcher::LauncherProc* pLauncherProc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( pGameManager->GetProcManager(), &m_launcherParam );
        pLauncherProc->CreateStartListData< debug::startmenu::StartLauncher >();
      }
#endif

      return GameSys::GMEVENT_RES_FINISH;
    }

    GameSys::GameEvent::SetSeqNo( seq );

    return GameSys::GMEVENT_RES_CONTINUE;
  }

  //  プレイヤーアプリ呼び出し
  void Event::_CallPlayerProc()
  {
    //  Proc作成
    GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

    if( m_playerDataType == PLAYAER_DATA_TYPE_LOCAL )
    {
      NetApp::BattleVideoPlayer::LocalProc* pProc = 
        NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::BattleVideoPlayer::LocalProc, NetApp::BattleVideoPlayer::PROC_APP_PARAM>( gameManager->GetProcManager(), &m_appParam, gameManager->GetAsyncFileManager() , "BattleVideoPlayer.cro" );

      m_pProc = pProc;
    }
    else
    if( m_playerDataType == PLAYAER_DATA_TYPE_INTERNET )
    {
      NetApp::BattleVideoPlayer::NetProc* pProc = 
        NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::BattleVideoPlayer::NetProc, NetApp::BattleVideoPlayer::PROC_APP_PARAM>( gameManager->GetProcManager(), &m_appParam, gameManager->GetAsyncFileManager() , "BattleVideoPlayer.cro" );

      m_pProc = pProc;
    }

    GFL_ASSERT( m_pProc );
  }

  //  シーケンス設定
  u32 Event::_StartSeq( const u32 seq, GameSys::GameManager* pGameManager )
  {
    u32 newSeq  = seq;

    if( newSeq == SEQ_VIDEO_PLAYER_PROC )
    {
      _CallPlayerProc();
    }
    else
    if( newSeq == SEQ_BTL_TAME_PROC )
    {
      NetAppLib::BattleVideoPlayer::VIDEO_DATA*  pVideoData  = m_appParam.out.pActiveVideoData;
      if( pVideoData == NULL )
      {
        GFL_ASSERT( pVideoData );
        newSeq = _StartSeq( SEQ_END, pGameManager );
      }
      else
      {
        //  チーム選択を呼び出す
        {
          m_themSelectEventAppParam.in.bAddLocalHavePokeParty = true;
          m_themSelectEventAppParam.in.bAddQRPokeParty        = true;
        }

        //  レギュレーション作成
        {
          Regulation::PRESET preset = Regulation::PRESET_MOCK_SINGLE;//念のため初期化

          if( pVideoData->sdCardData.upload.header.rule == Regulation::BATTLE_RULE_SINGLE )
          {
            preset = Regulation::PRESET_MOCK_SINGLE;
          }
          else
          if( pVideoData->sdCardData.upload.header.rule == Regulation ::BATTLE_RULE_DOUBLE )
          {
            preset = Regulation::PRESET_MOCK_DOUBLE;
          }
          else
          {
            GFL_ASSERT(0);//ここにはこないはず
          }

          //  チーム選択のレギュレーション情報クラス作成(一時的に作成)
          m_themSelectEventAppParam.in.pRegulationDrawInfo    = GFL_NEW_LOW( m_pLocalHeap ) NetAppLib::UI::RegulationDrawInfo( m_pLocalHeap, preset, NetAppLib::UI::RegulationDrawInfo ::RANK_NORMAL );
        }

        // @fix NMCat[2030] 保存したビデオのライブ・通信などの種類でチーム選択決定時のメッセージを変えていたが、模擬戦ではライブ・通信などは関係ない
        m_themSelectEventAppParam.in.tournamentType         = NetApp::TeamSelect::TOURNAMENT_TYPE_NONE;

        NetEvent::TeamSelect::TeamSelectEvent::StartEvent( pGameManager, &m_themSelectEventAppParam );
      }
    }
    else
    if( newSeq == SEQ_BTL_PROC )
    {
      //  バトル再生
      if( _CreateBtlSetupParam( &m_battleVideoCallParam.in.btlSetupParam, pGameManager ) == true )
      {
        //  再生カウントを増やす
        Savedata::IncRecord( Savedata::Record::RECID_BATTLEMOVIE );

        //  再生情報をダイアリーに記録
        // @fix GFNMCat[1725] ダイアリー設定が間違っていた。
        PokeDiary::Set( PDID_COUNT_BATTLEMOVIE, m_appParam.out.pActiveVideoData->sdCardData.upload.header.category, m_appParam.out.pActiveVideoData->sdCardData.upload.header.rule );

        //  バトルイベント開始
        NetEvent::BattleVideoCall::Event::StartEvent( pGameManager, &m_battleVideoCallParam );
      }
      else
      {
        //  失敗した場合はレコーダーに戻す
        m_appParam.in.procRunType = NetApp::BattleVideoPlayer::PROC_RUN_TYPE_RETURN;
        newSeq  = _StartSeq( SEQ_VIDEO_PLAYER_PROC, pGameManager );
      }
    }

    return  newSeq;
  }

  //  バトル模擬線シーケンス設定
  u32 Event::_StartBtlAISeq( pml::PokeParty* pSelfPokeParty, GameSys::GameManager* pGameManager )
  {
    u32 newSeq  = _StartSeq( SEQ_BTL_AI_PROC, pGameManager );

    if( newSeq == SEQ_BTL_AI_PROC )
    {
      // BtlSetupParam生成
      bool  bSuccess  = false;
      if( _CreateBtlSetupParam( &m_battleVideoCallParam.in.btlSetupParam, pGameManager ) == true )
      {
        // 模擬戦用のデータにコンバートする
        if( BTL_SETUP_ConvertToRecordFightMode( &m_battleVideoCallParam.in.btlSetupParam, m_appParam.out.dualTargetIdx, pGameManager->GetGameData(), m_pLocalHeap ) )
        {
          // 設定したポケパーティをセット
          BATTLE_PARAM_SetPokeParty( &m_battleVideoCallParam.in.btlSetupParam, pSelfPokeParty, BTL_CLIENT_PLAYER );

          //  バトルイベント開始
          NetEvent::BattleVideoCall::Event::StartEvent( pGameManager, &m_battleVideoCallParam );

          bSuccess  = true;
        }
      }

      if( bSuccess == false )
      {
        //  失敗した場合はレコーダーに戻す
        m_appParam.in.procRunType = NetApp::BattleVideoPlayer::PROC_RUN_TYPE_RETURN;
        newSeq  = _StartSeq( SEQ_VIDEO_PLAYER_PROC, pGameManager );
      }
      else
      {
        //  模擬戦カウントを増やす
        Savedata::IncRecord( Savedata::Record::RECID_BATTLEMOVIE_FIGHT );
      }
    }

    return  newSeq;
  }

  //  模擬戦可能かチェック
  bool Event::_CreateDuelPokemonData( pml::PokeParty* pSelfPokeParty )
  {
    if( pSelfPokeParty == NULL )
    {
      GFL_ASSERT( pSelfPokeParty );
      return  false;
    }

    // 全回復させる
    pSelfPokeParty->RecoverAll();

    // バトルビデオのポケモン数
    const NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData = m_appParam.out.pActiveVideoData;
    if( pVideoData->bServerDataFlg == 0 )
    {
      //  すぐバトル
      // @fix GFNMCat[1122] 仕様把握もれです。自身のビデオデータの場合は補正はないと思っていました。 
      // return  true;
    }

    //  他人のデータであればチェックする
    {
      // 相手のポケモンの中で一番レベルの高いポケモンで以上補正をする
      u32 maxLevel = 0;

      // 相手のレベルに合わせて、レベル補正をかける
      {
        pml::PokeParty* tmpParty = m_appParam.pVideoDataManager->GetBtlRecorderSaveData()->GetPokeParty( (BTL_CLIENT_ID) m_appParam.out.dualTargetIdx, m_pLocalHeap->GetLowerHandle() );
        if( tmpParty != NULL )
        {
          u32 loopNum = tmpParty->GetMemberCount();
          for( u32 i = 0; i < loopNum; i++ )
          {
            pml::pokepara::PokemonParam *pp = tmpParty->GetMemberPointer( i );
            if( maxLevel < pp->GetLevel() )
            {
              maxLevel = pp->GetLevel();
            }
          }
        }

        GFL_SAFE_DELETE( tmpParty );
        UCHIDA_PRINT("MAX LEVEL = %d \n", maxLevel );
      }

      //  レベル補正をする
      PokeRegulation::PokePartyLevelRevise( pSelfPokeParty, maxLevel );

      return true;
    }
  }

  //  バトルパラメータ生成
  bool Event::_CreateBtlSetupParam( BATTLE_SETUP_PARAM* pOutputParam, GameSys::GameManager* pGameManager )
  {
    // 選択設定からBtlSetupParamを作成する
    if( m_appParam.pVideoDataManager->GetBtlRecorderSaveData()->CreateBattleSetupParam( pOutputParam, pGameManager->GetGameData()->GetConfig() ) )
    {
      return true;
    }

    return false;
  }

  //  バトル実行パラメータ破棄
  void  Event::_DestroyBtlSetupParam( BATTLE_SETUP_PARAM* pParam )
  {
    m_appParam.pVideoDataManager->GetBtlRecorderSaveData()->ClearBattleSetupParam( pParam );
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetEvent)
