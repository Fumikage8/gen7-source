// ============================================================================
/*
 * @file TeamSelectEvent.cpp
 * @brief チーム選択アプリ起動イベント
 * @date 2015.06.23
 * @author yuto_uchida
 */
// ============================================================================
#include "NetStatic/NetEvent/include/TeamSelectEvent.h"

//  nijiのインクルード
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "System/include/HeapDefine.h"
#include "GameSys/include/DllProc.h"
#include "GameSys/include/GameData.h"
#include "PokeTool/include/PokeToolBattleParty.h"
#include "Savedata/include/LiveMatchSave.h"
#include "Savedata/include/BattleSpotSave.h"

//  呼び出すイベント
#include "App/AppEvent/include/Box/BoxEvent.h"

#include "NetApp/TeamSelect/include/TeamSelectEventAppParam.h"

//  gfl2のインクルード
#include <util/include/gfl2_List.h>

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //  イベント内で使用するローカルヒープサイズ
  const u32  TeamSelectEvent::ms_useLocalHeapSize  = 1024 * 300;
  //  チーム名の文字列データサイズ
  const u32  TeamSelectEvent::ms_teamNameSize = 32 * 3;

  //  アプリ開始の選択位置
  static u32 s_localSaveTeamIdx  = NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT;

  // コンストラクタ
  TeamSelectEvent::TeamSelectEvent( gfl2::heap::HeapBase* pHeap ) :
   NetEvent::NetEventBase( pHeap )
  {
    _Clear();
  }

  // デストラクタ
  TeamSelectEvent::~TeamSelectEvent()
  {
  }

  //  イベント開始
  void TeamSelectEvent::StartEvent(GameSys::GameManager* gmgr, NetApp::TeamSelect::EVENT_APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );

    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    TeamSelectEvent*  pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, TeamSelectEvent>(pGameEventManager);
    pEvent->SetAppParam( pAppParam );
  }

  void TeamSelectEvent::InitFunc( GameSys::GameManager* pGameManager )
  {
    m_pUseLocalHeap = GFL_CREATE_LOCAL_HEAP( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ), ms_useLocalHeapSize, gfl2::heap::HEAP_TYPE_EXP, false );

    //  QR用の変更しないパラメータ設定
    {
      m_qrReaderEventParam.mode = NetEvent::QRReader::QRReaderEventParam::MODE_Team;
    }

    _StartApp( pGameManager );
  }

  void TeamSelectEvent::EndFunc( GameSys::GameManager* pGameManager )
  {
    // @fix 寺地さん要望　選択終了した時のカーソル位置を保存
    s_localSaveTeamIdx = NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT;
    if( m_appParam.out.selectTeamIdx < m_appParam.in.teamDataNum )
    {
      NetApp::TeamSelect::TEAM_DATA*  pSettingTeamData  = &m_appParam.in.aTeamData[ m_appParam.out.selectTeamIdx ];
      if( pSettingTeamData->status != NetApp::TeamSelect::TEAM_STATUS_QR_READ_REQUEST )
      {
        //  手持ちポケモン選択
        if( pSettingTeamData->bPokeTemochiData == true )
        {
          s_localSaveTeamIdx = NetApp::TeamSelect::TEAM_SELECT_TEMOCHI;
        }
        else
        {
          //  boxのチーム選択
          s_localSaveTeamIdx = m_appParam.in.aTeamData[ m_appParam.out.selectTeamIdx ].no;
        }
      }
    }

    _DeleteData();
    
    GFL_DELETE_HEAP( m_pUseLocalHeap );
  }

  GameSys::GMEVENT_RESULT TeamSelectEvent::MainFunc( GameSys::GameManager* pGameManager )
  {
    enum
    {
      SEQ_TEAM_SELECT_EVENT_WAIT  = 0,
      SEQ_OTHER_EVENT_WAIT,
      SEQ_ERROR_NET_WAIT
    };

    GFL_ASSERT( m_pEventAppParam );

    s32 seq = GameSys::GameEvent::GetSeqNo();
    if( seq == SEQ_TEAM_SELECT_EVENT_WAIT )
    {
      //  procが死んでいるかチェック
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pProc ) == false )
      {
        if( IsError() )
        {
          //  通信エラー
          seq = SEQ_ERROR_NET_WAIT;
        }
        else
        {
          if( m_appParam.out.procResultType == NetApp::TeamSelect::PROC_RESULT_TYPE_NEXT_QR_READ_SCENE )
          {
            // QRコード読み込みのイベント実行
            UCHIDA_PRINT( "Next QR Read Scene\n" );

            NetEvent::QRReader::QRReaderEvent::StartEvent( pGameManager, m_qrReaderEventParam );

            seq = SEQ_OTHER_EVENT_WAIT;
          }
          else
          if( m_appParam.out.procResultType == NetApp::TeamSelect::PROC_RESULT_TYPE_NEXT_TEAM_SETTING_SCENE )
          {
            //  チーム編成に(ボックス)
            UCHIDA_PRINT( "Next Team Setting Scene\n" );

            //  ボックスイベント開始
            App::Event::BoxEvent* pBoxEvent = App::Event::BoxEvent::StartEvent( pGameManager );
            pBoxEvent->SetBoxEventListener( this );

            // @fix 寺地さん要望　チームのカーソルがboxデータであればbox起動時の初期チームnoを設定
            u32 boxNo = Savedata::BOX::TEAM_MAX;
            {
              for( u32 i = 0; i < m_appParam.in.teamDataNum; ++i )
              {
                NetApp::TeamSelect::TEAM_DATA*  pSettingTeamData  = &m_appParam.in.aTeamData[ i ];
                if( _IsBoxTeamData( pSettingTeamData ) == true )
                {
                  //  box
                  if( i == m_appParam.out.selectTeamIdx )
                  {
                    //  noを設定する
                    boxNo = pSettingTeamData->no;
                    break;
                  }
                }
              }
            }

            //  バトルチーム用に状態設定
            if( boxNo == Savedata::BOX::TEAM_MAX )
            {
              pBoxEvent->SetBoxStartType( App::Box::BOX_CALL_BATTLE_TEAM );
            }
            else
            {
              pBoxEvent->SetBoxStartType( App::Box::BOX_CALL_BATTLE_TEAM, boxNo );
            }

            seq = SEQ_OTHER_EVENT_WAIT;
          }
          else
          if( m_appParam.out.procResultType == NetApp::TeamSelect::PROC_RESULT_TYPE_NEXT_REQULATION_CHK_SCENE )
          {
            //  レギュレーション確認に
            UCHIDA_PRINT( "Next Requlation Chk Scene\n" );
            m_regulationSelectAppParam.bootMode = NetApp::RegulationSelect::BOOT_MODE_DETAIL_BATTLE_TEAM;
            m_regulationSelectAppParam.pRegulationDrawInfo  = m_pEventAppParam->in.pRegulationDrawInfo;

            // @fix GFNMCat[2024] 違反したチームがＱＲかそうでないか
            {
              NetApp::TeamSelect::TEAM_DATA*  pCurrentTeamData  = &m_appParam.in.aTeamData[ m_appParam.out.selectTeamIdx ];
              m_regulationSelectAppParam.bQRPartyFlag         = ( pCurrentTeamData->status == NetApp::TeamSelect::TEAM_STATUS_QR_READ_REQUEST );
            }

            NetEvent::RegulationSelect::RegulationSelectEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), m_regulationSelectAppParam );

            seq = SEQ_OTHER_EVENT_WAIT;
          }
          else
          if( m_appParam.out.procResultType == NetApp::TeamSelect::PROC_RESULT_TYPE_TEAM_SET )
          {
            //  バトルチーム選択で設定したのを設定する
            UCHIDA_PRINT( "Next Team Set\n" );

            m_pEventAppParam->out.result  = NetApp::TeamSelect::EVENT_APP_RESULT_TEAM_SELECT;
            if( m_appParam.out.selectTeamIdx < GFL_NELEMS( m_appParam.in.aTeamData ) )
            {
              NetApp::TeamSelect::TEAM_DATA*  pSettingTeamData  = &m_appParam.in.aTeamData[ m_appParam.out.selectTeamIdx ];
              if( pSettingTeamData->status == NetApp::TeamSelect::TEAM_STATUS_QR_READ_REQUEST )
              {
                //  QRチームを選択
                m_pEventAppParam->out.teamIdx = NetApp::TeamSelect::TEAM_SELECT_QR;
              }
              else
              //  手持ちポケモン選択
              if( pSettingTeamData->bPokeTemochiData == true )
              {
                m_pEventAppParam->out.teamIdx = NetApp::TeamSelect::TEAM_SELECT_TEMOCHI;
              }
              else
              {
                //  boxのチーム選択
                m_pEventAppParam->out.teamIdx = m_appParam.in.aTeamData[ m_appParam.out.selectTeamIdx ].no;
              }

              if( m_pEventAppParam->out.pPokeParty != NULL )
              {
                //  選択したポケモンパーティを設定
                m_pEventAppParam->out.pPokeParty->CopyFrom( *( pSettingTeamData->pPokeParty->GetPokeParty() ) );
              }

              UCHIDA_PRINT( "select teamIdx %d\n", m_pEventAppParam->out.teamIdx );
            }
            else
            {
              GFL_ASSERT( 0 );
            }

            return GameSys::GMEVENT_RES_FINISH;
          }
          else
          if( m_appParam.out.procResultType == NetApp::TeamSelect::PROC_RESULT_TYPE_CANCEL )
          {
            //  キャンセル
            UCHIDA_PRINT( "Prev Scene\n" );
            
            m_pEventAppParam->out.result  = NetApp::TeamSelect::EVENT_APP_RESULT_CANCEL;
            return GameSys::GMEVENT_RES_FINISH;
          }
          else
          {
            //  ここにきたら論理バグ
            GFL_ASSERT( 0 );
          }
        }
      }
    }
    else
    if( seq == SEQ_OTHER_EVENT_WAIT )
    {
      //  エラーなら即時終了
      if( IsError() )
      {
        return GameSys::GMEVENT_RES_FINISH;
      }
      else
      if( m_appParam.out.procResultType == NetApp::TeamSelect::PROC_RESULT_TYPE_NEXT_QR_READ_SCENE )
      {
        //  QRコード読み込みイベントから戻った
        UCHIDA_PRINT( "Retrun QR Read Scene\n" );

        //    QRのロード結果
        m_bReadQR = ( m_qrReaderEventParam.result == NetEvent::QRReader::RESULT_Team_Decoded );

        _StartApp( pGameManager );

        seq = SEQ_TEAM_SELECT_EVENT_WAIT;
      }
      else
      if( m_appParam.out.procResultType == NetApp::TeamSelect::PROC_RESULT_TYPE_NEXT_TEAM_SETTING_SCENE )
      {
        //  チーム編成に(ボックス)イベントから戻った
        UCHIDA_PRINT( "Retrun Team Setting Scene\n" );

        _StartApp( pGameManager );
        seq = SEQ_TEAM_SELECT_EVENT_WAIT;
      }
      else
      if( m_appParam.out.procResultType == NetApp::TeamSelect::PROC_RESULT_TYPE_NEXT_REQULATION_CHK_SCENE )
      {
        //  レギュレーション確認イベントから戻った
        UCHIDA_PRINT( "Retrun Requlation Chk Scene\n" );

        _StartApp( pGameManager );
        seq = SEQ_TEAM_SELECT_EVENT_WAIT;
      }
      else
      {
        //  ここにきたら論理バグ
        GFL_ASSERT( 0 );
      }
    }
    else
    if( seq == SEQ_ERROR_NET_WAIT )
    {
      //  通信エラー
      return GameSys::GMEVENT_RES_FINISH;
    }

    GameSys::GameEvent::SetSeqNo( seq );

    return GameSys::GMEVENT_RES_CONTINUE;
  }

  //-----------------------------------------------------------------------------
  /**
    * @func    OnBattleTray
    * @brief   終了の時のバトルトレイの場所を得る
    * @param   outTray  バトルトレイ番号0-5
    */
  //-----------------------------------------------------------------------------
  void TeamSelectEvent::OnBattleTray(u8 outTray )
  {
    //  ボックスイベントから最後に参照しているチームnoを設定
    m_lastBoxEventSelectTeamNo  = (u32)outTray; 
  }

  //----------------------------------------------------------------------
  /**
    *  @brief  チーム選択アプリ開始
    */
  //----------------------------------------------------------------------
  void TeamSelectEvent::_StartApp( GameSys::GameManager* pGameManager )
  {
    GFL_ASSERT( pGameManager );

    //  リストの並ぶにはルールがある
    //  1番目はQR
    //  2番目は手持ちポケモン(手持ちポケモンでなければboxポケモン)
    //  3番目以降はboxポケモン
    _DeleteData();

    {
      m_appParam.in.tournamentType          = m_pEventAppParam->in.tournamentType;
      // @fix GFNMCat[1287] レギュレーションボタンを押せるかどうかフラグ設定
      m_appParam.in.bOpenRegulationButton   = m_pEventAppParam->in.bOpenRegulationButton;
      m_appParam.in.currentTeamListIdx      = NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT;

      GameSys::GameData*  pGameData = pGameManager->GetGameData();

      Savedata::BOX*  pBoxSaveData  = pGameData->GetBOX();
      GFL_ASSERT( pBoxSaveData );

      Savedata::BoxPokemon* pBoxPokemonSaveData = pGameData->GetBoxPokemon();
      GFL_ASSERT( pBoxPokemonSaveData );

      //  Boxにポケモンが配置されているかチェック
      //  Boxデータが存在リストを作成
      gfl2::util::List<u32> haveBoxTeamList( m_pUseLocalHeap, Savedata::BOX::TEAM_MAX );
      {
        for( u32 boxTeamIdx = 0; boxTeamIdx < Savedata::BOX::TEAM_MAX; ++boxTeamIdx )
        {
          //  ポケモンがトレイに配置しているか
          if( pBoxSaveData->IsTeamPokeIn( boxTeamIdx ) == true )
          {
            haveBoxTeamList.PushBack( boxTeamIdx );
          }
        }
      }

      //  Boxデータがあるのでチーム選択データを作成
      m_appParam.in.teamDataNum = 0;
      m_appParam.in.appointEnterTeamListIdx = NetApp::TeamSelect::APPOIND_ENTER_TEAM_LIST_IDX_ERROR;
      {
        //  QR選択追加
        m_appParam.in.bQRPokePartyButton = m_pEventAppParam->in.bAddQRPokeParty;
        if( m_pEventAppParam->in.bAddQRPokeParty == true )
        {
          GFL_ASSERT( m_appParam.in.teamDataNum < GFL_NELEMS( m_appParam.in.aTeamData ) );
          NetApp::TeamSelect::TEAM_DATA*  pSettingTeamData  = &m_appParam.in.aTeamData[ m_appParam.in.teamDataNum ];

          pSettingTeamData->pPokeParty = GFL_NEW( m_pUseLocalHeap ) PokeTool::BattleParty( m_pUseLocalHeap );
          pSettingTeamData->status     = NetApp::TeamSelect::TEAM_STATUS_QR_READ_REQUEST;

          pSettingTeamData->pPokeParty->CreateBattleParty( pBoxSaveData, pBoxPokemonSaveData, NetApp::TeamSelect::TEAM_SELECT_QR, m_pUseLocalHeap );

          //  ロードしたQRを選択確認
          pSettingTeamData->bQRReaderData = false;
          if( m_bReadQR == true )
          {
            m_appParam.in.appointEnterTeamListIdx = 0;
            pSettingTeamData->bQRReaderData = true;

            //  ＱＲでロードしたポケモンは必ず１つ以上はあるので０は絶対にない
            GFL_ASSERT( 0 < pSettingTeamData->pPokeParty->GetPokeParty()->GetMemberCount() );
            m_bReadQR = false;
          }

          ++m_appParam.in.teamDataNum;
        }

        //  手持ちポケモンを追加
        if( m_pEventAppParam->in.bAddLocalHavePokeParty == true )
        {
          //  手持ちにポケモンを設定していないとだめ
          if( 0 < pGameData->GetPlayerParty()->GetMemberCount() )
          {
            GFL_ASSERT( m_appParam.in.teamDataNum < GFL_NELEMS( m_appParam.in.aTeamData ) );
            NetApp::TeamSelect::TEAM_DATA*  pSettingTeamData  = &m_appParam.in.aTeamData[ m_appParam.in.teamDataNum ];

            pSettingTeamData->pPokeParty = GFL_NEW( m_pUseLocalHeap ) PokeTool::BattleParty( m_pUseLocalHeap );
            pSettingTeamData->pPokeParty->CreateBattleParty( pBoxSaveData, pBoxPokemonSaveData, NetApp::TeamSelect::TEAM_SELECT_TEMOCHI, m_pUseLocalHeap );

            //  手持ちポケモンデータ参照している
            pSettingTeamData->bPokeTemochiData = true;

            pSettingTeamData->status  = NetApp::TeamSelect::TEAM_STATUS_PARTY_SELECT;

            ++m_appParam.in.teamDataNum;
          }
        }

        //  Boxのチームを追加
        //  ここではデータがある対象のみ設定
        if( 0 < haveBoxTeamList.Size() )
        {
          u32   teamIdx     = 0;

          //  ロックしているチームを取得
          //  ライブ大会でロックしているインデックス
          u32 liveTournamentPartIdx = pGameData->GetLiveMatchData()->GetPartyIndex();

          //  インターネット大会でロックしているインデックス
          u32 internetTournamentPartIdx = pGameData->GetBattleSpotData()->GetInternetLockTeamIndex();

          //  仲間大会でロックしているインデックス
          u32 friendTournamentPartIdx = pGameData->GetBattleSpotData()->GetFriendLockTeamIndex();

          for( gfl2::util::ListIterator<u32> ite = haveBoxTeamList.Begin(); ite != haveBoxTeamList.End(); ++ite )
          {
            teamIdx   = *ite;
            GFL_ASSERT( m_appParam.in.teamDataNum < GFL_NELEMS( m_appParam.in.aTeamData ) );
            NetApp::TeamSelect::TEAM_DATA*  pSettingTeamData  = &m_appParam.in.aTeamData[ m_appParam.in.teamDataNum ];

            //  ポケモン追加
            pSettingTeamData->pPokeParty = GFL_NEW( m_pUseLocalHeap ) PokeTool::BattleParty( m_pUseLocalHeap );

            pSettingTeamData->status  = NetApp::TeamSelect::TEAM_STATUS_PARTY_SELECT;
            // @fix NMCat[2111] ロックステータス設定を初期化忘れ
            //                  ボックスから戻った時にボックス遷移前のロックステータスが残っていたのでロックしていないチームにロックステータスが設定されていた
            pSettingTeamData->lockStatus  = NetApp::TeamSelect::LOCK_STATUS_NONE;

            //  boxのチームnoを設定
            pSettingTeamData->no      = teamIdx;

            // @fix 寺地さん要望  ボックスイベントが最後に参照しているチームnoをカーソル位置にするようにする
            if( m_lastBoxEventSelectTeamNo != Savedata::BOX::TEAM_MAX )
            {
              if( pSettingTeamData->no == m_lastBoxEventSelectTeamNo )
              {
                //  boxチームに初回カーソル
                m_appParam.in.currentTeamListIdx  = m_appParam.in.teamDataNum;
                s_localSaveTeamIdx = NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT;
              }
            }

            pSettingTeamData->pPokeParty->CreateBattleParty( pBoxSaveData, pBoxPokemonSaveData, pSettingTeamData->no, m_pUseLocalHeap );

            //  ロックしているかどうか
            if( pSettingTeamData->pPokeParty->IsLock() == true )
            {
              // @fix GFNMCat[2674] 大会セーブ側で持っているboxチームロックのidを分岐条件に
              //  ロックしているのが各大会のどれに該当するかチェック
              if( liveTournamentPartIdx == pSettingTeamData->no )
              {
                pSettingTeamData->lockStatus  = NetApp::TeamSelect::LOCK_STATUS_LIVE;
              }
              else
              if( internetTournamentPartIdx == pSettingTeamData->no )
              {
                pSettingTeamData->lockStatus  = NetApp::TeamSelect::LOCK_STATUS_NET;
              }
              else
              if( friendTournamentPartIdx == pSettingTeamData->no )
              {
                pSettingTeamData->lockStatus  = NetApp::TeamSelect::LOCK_STATUS_FRIEND;
              }
              else
              {
                //  絶対にどれか該当していないといけない
                GFL_ASSERT( 0 );
                pSettingTeamData->lockStatus  = NetApp::TeamSelect::LOCK_STATUS_FRIEND;
              }
            }

            ++m_appParam.in.teamDataNum;
          }
        }
        m_lastBoxEventSelectTeamNo        = Savedata::BOX::TEAM_MAX;
      }
    }

    //  初回チームの選択
    // @fix 寺地さん要望　アプリ起動時にはグローバルで保存しているチームを初期選択に変える
    if( s_localSaveTeamIdx != NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT )
    {
      for( u32 i = 0; i < m_appParam.in.teamDataNum; ++i )
      {
        NetApp::TeamSelect::TEAM_DATA*  pSettingTeamData  = &m_appParam.in.aTeamData[ i ];
        if( pSettingTeamData->bPokeTemochiData == true )
        {
          //  手持ち
          if( s_localSaveTeamIdx == NetApp::TeamSelect::TEAM_SELECT_TEMOCHI )
          {
            m_appParam.in.currentTeamListIdx  = i;
            break;
          }
        }
        else
        if( _IsBoxTeamData( pSettingTeamData ) == true )
        {
          //  box
          if( s_localSaveTeamIdx == pSettingTeamData->no )
          {
            m_appParam.in.currentTeamListIdx  = i;
            break;
          }
        }
      }
      s_localSaveTeamIdx = NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT;
    }
    
    // @fix 寺地さん要望　チームが選ばれていない状態なら優先度に従って初期選択を変える
    if( m_appParam.in.currentTeamListIdx == NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT )
    {
      //  bpxのチーム選択があるのであれば、その中の小さいnoを取得
      u32 tempTeamSelectIdx     = NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT;
      u32 temochiTeamSelectIdx  = NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT;
      bool  bHaveTemochi  = false;
      {
        u32 boxNo = Savedata::BOX::TEAM_MAX;
        for( u32 i = 0; i < m_appParam.in.teamDataNum; ++i )
        {
          NetApp::TeamSelect::TEAM_DATA*  pSettingTeamData  = &m_appParam.in.aTeamData[ i ];
          if( _IsBoxTeamData( pSettingTeamData ) == true )
          {
            if( pSettingTeamData->no < boxNo )
            {
              boxNo = pSettingTeamData->no;
              tempTeamSelectIdx = i;
            }
          }
          else
          if( pSettingTeamData->bPokeTemochiData == true )
          {
            bHaveTemochi  = true;
            temochiTeamSelectIdx  = i;
          }
        }
      }

      if( tempTeamSelectIdx != NetApp::TeamSelect::CURRENT_TEAM_SELECT_IDX_DEFAULT )
      {
        m_appParam.in.currentTeamListIdx  = tempTeamSelectIdx;
      }
      else
      if( bHaveTemochi == true )
      {
        //  手持ちがあれば手持ち位置にする
        m_appParam.in.currentTeamListIdx  = temochiTeamSelectIdx;
      }
    }

    //  レギュレーション設定
    m_appParam.in.pRegulationDrawInfo = m_pEventAppParam->in.pRegulationDrawInfo;

    //  proc作成して実行
    m_pProc = GameSys::CallTeamSelectProc( &m_appParam );
  }

  //----------------------------------------------------------------------
  /**
    *  @brief  Boxのトレイ場所に変換
    */
  //----------------------------------------------------------------------
  u32   TeamSelectEvent::_ConvBoxTrayPos( u32 num )
  {
    return  ( ( num >> 8 ) & 0xff );
  }

  //----------------------------------------------------------------------
  /**
    *  @brief  Boxのパーティー場所に変換
    */
  //----------------------------------------------------------------------
  u32   TeamSelectEvent::_ConvBoxPokemonPartyPos( u32 num )
  {
    return  ( num & Savedata::BOX::BATTLE_BOX_TRAY_NUMBER );
  }

  //  @brief  データ破棄
  void  TeamSelectEvent::_DeleteData()
  {
    u32 loopNum = GFL_NELEMS( m_appParam.in.aTeamData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      GFL_SAFE_DELETE( m_appParam.in.aTeamData[ i ].pPokeParty );
    }
  }

  //  チームデータがboxか
  bool  TeamSelectEvent::_IsBoxTeamData( const NetApp::TeamSelect::TEAM_DATA* pTeamData )
  {
    if( pTeamData == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( ( pTeamData->status == NetApp::TeamSelect::TEAM_STATUS_PARTY_SELECT ) && ( pTeamData->bPokeTemochiData == false ) )
    {
      return  true;
    }

    return  false;
  }

  void  TeamSelectEvent::_Clear()
  {
    m_pProc           = NULL;
    m_pEventAppParam  = NULL;
    m_pUseLocalHeap   = NULL;
    m_lastBoxEventSelectTeamNo  = Savedata::BOX::TEAM_MAX;
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetEvent)
