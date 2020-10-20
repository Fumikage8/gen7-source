#if PM_DEBUG
//=============================================================================
/**
 * @file    d_uchida.cpp
 * @brief   内田用ランチャーリスト処理
 * @author  Yuto Uchida
 * @date    2015.02.20
 */
//=============================================================================
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/StartMenu/include/d_uchida.h>

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_uchida.h>

#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"

#include "GameSys/include/DllProc.h"
#include "Test/UIUnitTest/include/UIIconTestProc.h"
#include "Test/UIUnitTest/include/UICharaTexReplaceTestProc.h"

#include "App/Title/include/TitleProc.h"
#include "App/AppEvent/include/Config/ConfigEvent.h"
#include "App/AppEvent/include/CaptureTutorial/CaptureTutorialEvent.h"
#include "App/AppEvent/include/TrainerPhoto/TrainerPhotoEvent.h"
#include "Test/UIUnitTest/include/UIBoxChatViewTestProc.h"
#include "Test/UIUnitTest/include/UIBoxPokeStatusViewTestProc.h"
#include "NetStatic/NetEvent/include/BattleVideoRecordingEvent.h"
#include "NetStatic/NetEvent/include/BattleVideoPlayerEvent.h"
#include "App/AppEvent/include/Kisekae/KisekaeEvent.h"
#include "App/AppEvent/include/BattleRoyalResult/BattleRoyalResultEvent.h"
#include "App/AppEvent/include/BattleTree/BattleTreeEvent.h"
#include "App/AppEvent/include/SuperTraining/SuperTrainingEvent.h"

//  きせかえデータベース
#include "KisekaeData/include/KisekaeDataBase.h"

#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include "Savedata/include/JoinFestaDataSave.h"

//  gflib2のインクルード
#include <Fade/include/gfl2_FadeManager.h>
#include <System/include/HeapDefine.h>

#include <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>

#include <niji_conv_header/message/msg_kisekae_color.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

  static App::BattleRoyalResult::APP_PARAM s_debugBtlRoyalResultAppParam;
  static NetEvent::BattleVideoRecording::EVENT_APP_PARAM  s_debugBtlRecorderAppParam;
  static App::BattleTree::APP_PARAM s_debugBattleTreeAppParam;
  static App::SuperTraining::APP_PARAM  s_debugSuperTrainingAppParam;
  static u32 m_netInitCnt = 0;
  static pml::pokepara::PokemonParam* s_pSuperTrainingPokeParam = NULL;

// 項目文字列データテーブル
const u32 LauncherUchida::ItemList[ msg_d_uchida_max ] =
{
  //  タイトルプロセスを呼ぶ
  label_uchida_ui_title_test,
  //  コンフィグプロセスを呼ぶ
  label_uchida_ui_config_test,
  //  捕獲チュートリルプロセルを呼ぶ
  label_uchida_ui_capture_tutorial_test,
  //  アイコンテスト
  label_uchida_ui_icon_test,
  //  チーム選択テスト
  label_uchida_ui_team_select_test,
  //  BoxチャットのViewテスト
  label_uchida_ui_box_chat_view_test,
  //  BoxポケモンスタータスViewテスト
  label_uchida_ui_box_status_view_test,
  //  キャラアイコンテスト
  label_uchida_ui_char_icon_test,
  //  証明写真
  label_uchida_ui_trainer_photo__test,

  //  バトル再生
  label_uchida_ui_video_player_test,

  //  バトル録画
  label_uchida_ui_video_rec_test,

  //  きせかえshop
  label_uchida_ui_kisekae_shop_test,

  //  きせかえ
  label_uchida_ui_kisekae_test,

  //  きせかえ（そめものや）
  label_uchida_ui_kisekae_color_test,

  //  きせかえ（そめものや＋木の実使用）
  label_uchida_ui_kisekae_color_test_1,

  //  デバッグパラメータ設定
  label_uchida_set_debug_param,

  //  きせかえ所持をクリア
  label_uchida_clear_kisekae_have,

  //  バトルロイヤルの勝敗
  label_uchida_btl_royal_result,

  //  バトルロイヤルの勝敗1位
  label_uchida_btl_royal_result_win,

  //  主人公を女に
  label_uchida_chg_sex_male,

  //  主人公を男に
  label_uchida_chg_sex_female,

  //  バトルビデオ再生ネット
  label_uchida_ui_video_player_net,

  //  キャラアイコンテスト(NPC限定)
  label_uchida_ui_char_icon_test_1,

  //  バトルツリー
  label_uchida_btl_tree,

  //  キャラアイコンテスト（テクスチャー出力）
  label_uchida_ui_char_icon_test_2,

  //  すごい特訓(金の王冠)
  label_uchida_ui_super_training,

  //  すごい特訓（銀の王冠）
  label_uchida_ui_super_training_1,

  //  全きせかえの購入
  label_uchida_all_buy_kisekae,

  //  スタッフロール（初回）
  label_uchida_staffroll_test_01,

  //  スタッフロール
  label_uchida_staffroll_test,

  //  エンドロール
  label_uchida_end_roll,
};

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LauncherUchida::LauncherUchida(void)
{
  gfl2::heap::HeapBase* pDebugHeap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  //  すごい特訓用のテストポケモンパラメータ
  {
    s_pSuperTrainingPokeParam = GFL_NEW( pDebugHeap ) pml::pokepara::PokemonParam( pDebugHeap, (MonsNo)25, 100, 0 );

    s_pSuperTrainingPokeParam->SetWaza(0,WAZANO_BORUTEKKAA);
    s_pSuperTrainingPokeParam->SetWaza(1,WAZANO_KAENHOUSYA);
    s_pSuperTrainingPokeParam->SetWaza(2,WAZANO_DOKUDOKU);
    s_pSuperTrainingPokeParam->SetWaza(3,WAZANO_KAWARAWARI);
    s_pSuperTrainingPokeParam->SetItem(ITEM_PIKATYUUBIIZU_1);

    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->SetPokemonParent( s_pSuperTrainingPokeParam, pDebugHeap );

    s_pSuperTrainingPokeParam->SetTokuseiIndex( 0 );
    s_pSuperTrainingPokeParam->ChangeSeikaku( pml::pokepara::SEIKAKU_IJIPPARI );
    s_pSuperTrainingPokeParam->ChangeSex( pml::SEX_MALE );

    for( int j=0; j<pml::pokepara::POWER_NUM; ++j )
    {
      s_pSuperTrainingPokeParam->ChangeTalentPower( static_cast<pml::pokepara::PowerID>(j), 31 );
      s_pSuperTrainingPokeParam->ChangeEffortPower( static_cast<pml::pokepara::PowerID>(j), 16 );
    }

    //親名
    GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
    Savedata::MyStatus* pMyStatus = pGameData->GetPlayerStatus();
    pMyStatus->SetPokemonParent( s_pSuperTrainingPokeParam, pDebugHeap );
    //ContactBBS[195] CBSBTS[195] 親名以外にも必須項目の設定
    s_pSuperTrainingPokeParam->SetCassetteVersion( static_cast<u32>(GET_VERSION()) );
    s_pSuperTrainingPokeParam->SetLangId( System::LANGUAGE_JAPAN );
    s_pSuperTrainingPokeParam->SetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL, item::BALLID_MONSUTAABOORU );
    s_pSuperTrainingPokeParam->SetMemories( pml::pokepara::MEMORIES_CODE_WITH_PARENT, 1 );
    s_pSuperTrainingPokeParam->SetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_PARENT, 1 );
  }

  //  バトル録画のテストデータ
  {
    m_pEnemyParty  = GFL_NEW(pDebugHeap) pml::PokeParty(pDebugHeap);
    m_pEnemyParty->Clear();

    gfl2::std::MemClear( m_pEnemyPokeParam, sizeof( m_pEnemyPokeParam ) );
    // 敵手持ち
    for( int i=0;i<3;i++ )
    {
      m_pEnemyPokeParam[i] = GFL_NEW( pDebugHeap ) pml::pokepara::PokemonParam( pDebugHeap, (MonsNo)(i+1), 15, 0 );
      m_pEnemyPokeParam[i]->SetWaza(0,WAZANO_BORUTEKKAA);
      m_pEnemyPokeParam[i]->SetWaza(1,WAZANO_KAENHOUSYA);
      m_pEnemyPokeParam[i]->SetWaza(2,WAZANO_DOKUDOKU);
      m_pEnemyPokeParam[i]->SetWaza(3,WAZANO_KAWARAWARI);
      m_pEnemyPokeParam[i]->SetItem(ITEM_PIKATYUUBIIZU_1);
      {
        m_pEnemyParty->AddMember( *m_pEnemyPokeParam[0] );
      }
    }

    BATTLE_SETUP_FIELD_SITUATION_Init( &m_sit );

    BATTLE_SETUP_Init( &m_btlSetupParam );
    BATTLE_SETUP_Wild( &m_btlSetupParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager) , m_pEnemyParty ,&m_sit , BTL_RULE_SINGLE , static_cast<BattleEffectId>(0) , NULL , pDebugHeap );
    BATTLE_PARAM_AllocRecBuffer( &m_btlSetupParam, pDebugHeap );

    //  セーブするインスタンスを作成
    {
      m_pRecorderSaveData = GFL_NEW( pDebugHeap )  ExtSavedata::BattleRecorderSaveData( pDebugHeap, GFL_SINGLETON_INSTANCE(GameSys::GameManager) );

      //  レギュレーション作成
      {
        m_pBtlRegulation  = GFL_NEW( pDebugHeap ) Regulation( pDebugHeap );
        m_pBtlRegulation->LoadData( Regulation::PRESET_SINGLE_NORMAL );
      }

      m_pRecorderSaveData->CreateSaveData( &m_btlSetupParam, m_pBtlRegulation );
    }

    //  テスト録画データ設定
    s_debugBtlRecorderAppParam.in.appInParam.pRecorderSaveData      = m_pRecorderSaveData;
    s_debugBtlRecorderAppParam.in.appInParam.saveDataServerVersion  = BTL_NET_SERVER_VERSION;
  }

  //  ロイヤル勝敗の仮データ作成
  {
    //  プレイヤー自身の順位
    {
      s_debugBtlRoyalResultAppParam.selfRanking = 2;
    }

    for( u32 i = 0; i < GFL_NELEMS( s_debugBtlRoyalResultAppParam.aTrainerData ); ++i )
    {
      App::BattleRoyalResult::TRINER_BTL_RESULT_DATA* pTrainerData  = &s_debugBtlRoyalResultAppParam.aTrainerData[ i ];
      if( i == 0 )
      {
        pTrainerData->pNameBuf  = GFL_NEW( pDebugHeap ) gfl2::str::StrBuf( L"test", pDebugHeap );
        app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData( 0, &pTrainerData->iconObjData );
        pTrainerData->ranking = 2;
        pTrainerData->killCount = 3;
        pTrainerData->correrColor = CORRER_COLOR_BLUE;

        for( u32 j = 0; j < GFL_NELEMS( pTrainerData->aPokeData ); ++j )
        {
          pTrainerData->aPokeData[ j ].hp = 0;
          if( j == 1 )
          {
            pTrainerData->aPokeData[ j ].hp = 100;
          }

          pTrainerData->aPokeData[ j ].simpleParam.monsNo = MONSNO_HUSIGIDANE;
        }
      }
      else
      if( i == 1 )
      {
        pTrainerData->pNameBuf  = GFL_NEW( pDebugHeap ) gfl2::str::StrBuf( L"tamago", pDebugHeap );
//        app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData( 0, &pTrainerData->iconObjData );
        pTrainerData->ranking = 1;
        pTrainerData->killCount = 5;
        pTrainerData->trainerType = trainer::TRTYPE_ADULTGIRL;
        pTrainerData->correrColor = CORRER_COLOR_RED;

        for( u32 j = 0; j < GFL_NELEMS( pTrainerData->aPokeData ); ++j )
        {
          pTrainerData->aPokeData[ j ].hp = 10;
          pTrainerData->aPokeData[ j ].simpleParam.monsNo = MONSNO_RIZAADON;
        }
      }
      else
      if( i == 2 )
      {
        pTrainerData->pNameBuf  = GFL_NEW( pDebugHeap ) gfl2::str::StrBuf( L"tori", pDebugHeap );
//        app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData( 0, &pTrainerData->iconObjData );
        pTrainerData->trainerType = trainer::TRTYPE_ELITEM;
        pTrainerData->ranking = 3;
        pTrainerData->killCount = 2;
        pTrainerData->correrColor = CORRER_COLOR_GREEN;

        for( u32 j = 0; j < GFL_NELEMS( pTrainerData->aPokeData ); ++j )
        {
          pTrainerData->aPokeData[ j ].hp = 30;
          pTrainerData->aPokeData[ j ].simpleParam.monsNo = MONSNO_HUSIGIDANE;
        }
      }
      else
      {
        pTrainerData->pNameBuf  = GFL_NEW( pDebugHeap ) gfl2::str::StrBuf( L"aaaa", pDebugHeap );
        //app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData( 0, &pTrainerData->iconObjData );
        pTrainerData->trainerType = trainer::TRTYPE_VETERANW;
        pTrainerData->killCount = 1;
        pTrainerData->ranking = 4;
        pTrainerData->correrColor = CORRER_COLOR_YELLOW;

        for( u32 j = 0; j < GFL_NELEMS( pTrainerData->aPokeData ); ++j )
        {
          pTrainerData->aPokeData[ j ].hp = 0;
          pTrainerData->aPokeData[ j ].simpleParam.monsNo = MONSNO_HUSIGIDANE;
        }
      }
    }
  }

  //  チーム選択のレギュレーション情報クラス作成(一時的に作成)
  m_teamSelectAppParam.in.pRegulationDrawInfo    = GFL_NEW_LOW( pDebugHeap ) NetAppLib::UI::RegulationDrawInfo( pDebugHeap, Regulation::PRESET_MOCK_SINGLE, NetAppLib::UI::RegulationDrawInfo ::RANK_NORMAL );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherUchida::~LauncherUchida()
{
  GFL_SAFE_DELETE( m_teamSelectAppParam.in.pRegulationDrawInfo );
  GFL_SAFE_DELETE( s_pSuperTrainingPokeParam );

  if( 0 < m_netInitCnt )
  {
    if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) ) 
    {
      GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Finalize();

    }

    m_netInitCnt  = 0;
  }

  for( u32 i = 0; i < GFL_NELEMS( s_debugBtlRoyalResultAppParam.aTrainerData ); ++i )
  {
    App::BattleRoyalResult::TRINER_BTL_RESULT_DATA* pTrainerData  = &s_debugBtlRoyalResultAppParam.aTrainerData[ i ];
    GFL_SAFE_DELETE( pTrainerData->pNameBuf );
  }

  GFL_SAFE_DELETE( m_pBtlRegulation );
  GFL_SAFE_DELETE( m_pRecorderSaveData );

  {
    u32 loopNum = GFL_NELEMS( m_pEnemyPokeParam );
    for( u32 i = 0; i < loopNum; ++i )
    {
      GFL_SAFE_DELETE( m_pEnemyPokeParam[ i ] );
    }
  }

  GFL_SAFE_DELETE( m_pEnemyParty );

  if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) ) 
  {
    GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Finalize();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetArcDataID
 * @brief   アークデータID取得
 * @date    2015.02.20
 *
 * @return  アークデータID
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID LauncherUchida::GetArcDataID(void)
{
  return GARC_debug_message_d_uchida_DAT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブル取得
 * @date    2015.02.20
 *
 * @return  文字列データテーブル
 */
//-----------------------------------------------------------------------------
const u32 * LauncherUchida::GetListTable(void)
{
  return ItemList;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブルの要素数取得
 * @date    2015.02.20
 *
 * @return  文字列データテーブルの要素数
 */
//-----------------------------------------------------------------------------
u32 LauncherUchida::GetListTableSize(void)
{
  return GFL_NELEMS( ItemList );
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectListener
 * @brief   決定時のリスナー
 * @date    2015.02.18
 *
 * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
 * @param   launcher  呼び出し元のランチャークラス
 *
 * @return  ランチャーへのリクエストID
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherUchida::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  static app::title::APP_PARAM param;

  // キャンセル
  if( pos == launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    if( param.out_param.ret == app::title::PROC_RET_DEBUG_SEQ )
    {
      GFL_PRINT( "title to DebugSeq\n" );
    }
    else
    if( param.out_param.ret == app::title::PROC_RET_GAME_SEQ )
    {
      GFL_PRINT( "title to GameSeq\n" );
    }
    else
    if( param.out_param.ret == app::title::PROC_RET_DELETE_SAVE )
    {
      GFL_PRINT( "title to DeleteSaveSeq\n" );
    }

    if( m_teamSelectAppParam.out.result == NetApp::TeamSelect::EVENT_APP_RESULT_CANCEL )
    {
      //  キャンセル
      GFL_PRINT( "TeamSelect to EVENT_APP_RESULT_CANCEL\n" );
    }
    else
    if( m_teamSelectAppParam.out.result == NetApp::TeamSelect::EVENT_APP_RESULT_TEAM_SELECT )
    {
      //  チーム選択
      GFL_PRINT( "TeamSelect to EVENT_APP_RESULT_TEAM_SELECT\n" );
    }

    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
    //
    case label_uchida_ui_title_test:
    {
      GameSys::CallTitleProc( &param );
      break;
    }

    case label_uchida_ui_config_test:
    {
      app::event::ConfigEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));
      break;
    }

    case label_uchida_ui_capture_tutorial_test:
    {
      //  デバッグパラメータ設定をしないとフィールドのゾーン設定でアサートになる
      {
        gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetDebugData( GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, heap, 0 );
      }

      app::event::CaptureTutorialEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      return launcher::ListData::REQID_CALL;
    }

    case label_uchida_ui_icon_test:
    {
      //  UIアイコン表示のテスト
      Test::UIIconTestProc*  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::UIIconTestProc>( 
                GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "UIUnitTest.cro" );
      break;
    }

    case label_uchida_ui_team_select_test:
    {
      //  チーム選択のテスト
      m_teamSelectAppParam.in.bOpenRegulationButton  = false;
      m_teamSelectAppParam.in.bAddLocalHavePokeParty = true;
      m_teamSelectAppParam.in.bAddQRPokeParty        = false;
      m_teamSelectAppParam.in.tournamentType         = NetApp::TeamSelect::TOURNAMENT_TYPE_LIVE;

      NetEvent::TeamSelect::TeamSelectEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &m_teamSelectAppParam );

      break;
    }

    case label_uchida_ui_box_chat_view_test:
    {
      //  BoxチャットのViewテスト
      Test::UIBoxChatViewTestProc*  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::UIBoxChatViewTestProc>( 
                GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "UIUnitTest.cro" );

      gfl2::ro::RoManager* pRoMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
      gfl2::ro::Module* pBoxModule       = pRoMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Box.cro");

      //  レーダーチャート用のdllが必要
      gfl2::ro::Module* pRaderChartModule = pRoMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "RaderChart.cro");

      pRoMgr->StartModule( pBoxModule, true );
      pRoMgr->StartModule( pRaderChartModule, true );

      pProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
      pProc->SetChildModulePtr( 0 , pBoxModule );
      pProc->SetChildModulePtr( 1 , pRaderChartModule );

      break;
    }

    case label_uchida_ui_box_status_view_test:
    {
      //  BoxステータスのViewテスト

      Test::UIBoxPokeStatusViewTestProc*  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::UIBoxPokeStatusViewTestProc>( 
                GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "UIUnitTest.cro" );

      gfl2::ro::RoManager* pRoMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
      gfl2::ro::Module* pBoxModule        = pRoMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Box.cro");

      //  レーダーチャート用のdllが必要
      gfl2::ro::Module* pRaderChartModule = pRoMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "RaderChart.cro");

      pRoMgr->StartModule( pBoxModule, true );
      pRoMgr->StartModule( pRaderChartModule, true );

      pProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
      pProc->SetChildModulePtr( 0 , pBoxModule );
      pProc->SetChildModulePtr( 1 , pRaderChartModule );

      break;
    }

    case label_uchida_ui_char_icon_test:
    {
      //  キャラアイコン
      Test::UICharaTexReplaceTestProc*  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::UICharaTexReplaceTestProc>( 
                GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "UIUnitTest.cro" );

      pProc->SetEnableNPCOnly( false );

      break;
    }

    case label_uchida_ui_trainer_photo__test:
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

      //  証明写真アプリのテスト
      app::event::TrainerPhotoEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      break;
    }

    //  バトル再生
    case label_uchida_ui_video_player_test:
    {
      //  フェードを使って、黒で覆う
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );
      }

      //  ローカル用
      NetEvent::BattleVideoPlayer::Event::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), NetEvent::BattleVideoPlayer::Event::PLAYAER_DATA_TYPE_LOCAL, true );

      //  終了させないとバトルの終了を検知できない
      return launcher::ListData::REQID_FINISH;
    }

    //  バトル録画
    case label_uchida_ui_video_rec_test:
    {
      //  フェードを使って、黒で覆う
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );
      }

      NetEvent::BattleVideoRecording::Event::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &s_debugBtlRecorderAppParam );

      break;
    }
    //  きせかえshop
    case label_uchida_ui_kisekae_shop_test:
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

      static App::Kisekae::APP_PARAM  param;
      //  着せ替えを購入
      param.runType = App::Kisekae::EVENT_RUN_TYPE_SHOP;
      //  ショップidを指定
      param.typeData.shop.shopId = app::util::Kisekae::KISEKAE_SHOP_ID_0;

      App::Event::KisekaeEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &param );

      break;
    }

    //  きせかえ
    case label_uchida_ui_kisekae_test:
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

      static App::Kisekae::APP_PARAM  param;
      //  着せ替えをする
      param.runType = App::Kisekae::EVENT_RUN_TYPE_DRESS_UP;

      App::Event::KisekaeEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &param );

      break;
    }

    //  きせかえそめものや
    case label_uchida_ui_kisekae_color_test:
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

      static App::Kisekae::APP_PARAM  param;
      param.runType = App::Kisekae::EVENT_RUN_TYPE_COLOR_SHOP;
      //  ショップidを指定
      param.typeData.colorShop.shopId   = 22;
      //  ショップで染める色指定(ショップの全色対象)
      param.typeData.colorShop.colorId  = App::Kisekae::ALL_COLOR_SELECT;

      Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
      GFL_ASSERT( pJoinFestaDataSave );
      pJoinFestaDataSave->SetCoin( 999 );

      App::Event::KisekaeEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &param );

      break;
    }

    //  きせかえ（そめものや＋木の実使用）
    case label_uchida_ui_kisekae_color_test_1:
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

      static App::Kisekae::APP_PARAM  param;
      param.runType = App::Kisekae::EVENT_RUN_TYPE_COLOR_SHOP;
      //  ショップidを指定
      param.typeData.colorShop.shopId   = 0;
      //  ショップで染める色指定
      param.typeData.colorShop.colorId  = DRESSUP_COLOR_ID_CLOTH_RD1;

      //  木の実を使う
      param.typeData.colorShop.bUseKinomiDiscount = true;

      App::Event::KisekaeEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &param );

      break;
    }

    //  デバッグパラメータ設定
    case label_uchida_set_debug_param:
    {
      Savedata::MyStatus* pStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

      gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetDebugData( GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, heap, pStatus->GetSex() );

      //  所持金のダミー値
      Savedata::Misc* pMiscSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetMisc();
      Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
      GFL_ASSERT( pMiscSaveData);
      GFL_ASSERT( pJoinFestaDataSave);

      pMiscSaveData->SetGold( 100000 );
      pJoinFestaDataSave->SetCoin( 10000 );

      break;
    }

    //  きせかえ所持をクリア
    case label_uchida_clear_kisekae_have:
    {
      Savedata::Fashion* pFashionSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFashion();
      GFL_ASSERT( pFashionSaveData);
      {
        pFashionSaveData->Clear( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
      }

      Savedata::MyStatus* pStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

      pFashionSaveData->InitializeDefaultFashionItem( ( pStatus->GetSex() == poke_3d::model::DressUpParam::SEX_FEMALE ) );

      break;
    }

    //  バトルロイヤル結果
    case label_uchida_btl_royal_result:
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

      s_debugBtlRoyalResultAppParam.selfRanking = 2;
      App::Event::BattleRoyalResultEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &s_debugBtlRoyalResultAppParam );
      break;
    }

    //  バトルロイヤル結果1位
    case label_uchida_btl_royal_result_win:
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

      s_debugBtlRoyalResultAppParam.selfRanking = 1;
      App::Event::BattleRoyalResultEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &s_debugBtlRoyalResultAppParam );

      break;
    }

    //  主人公を女に
    case label_uchida_chg_sex_male:
    {
      break;
    }

    //  主人公を男に
    case label_uchida_chg_sex_female:
    {
      break;
    }

    //  バトルビデオ再生ネット
    case label_uchida_ui_video_player_net:
    {
      //  フェードを使って、黒で覆う
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );
      }

      NetEvent::BattleVideoPlayer::Event::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), NetEvent::BattleVideoPlayer::Event::PLAYAER_DATA_TYPE_INTERNET, true );

      //  終了させないとバトルの終了を検知できない
      return launcher::ListData::REQID_FINISH;
    }

    //  キャラアイコンテスト(NPC限定)
    case label_uchida_ui_char_icon_test_1:
    {
      //  キャラアイコン
      Test::UICharaTexReplaceTestProc*  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::UICharaTexReplaceTestProc>( 
                GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "UIUnitTest.cro" );

      pProc->SetEnableNPCOnly( true );

      break;
    }

    //  バトルツリー
    case label_uchida_btl_tree:
    {
      s_debugBattleTreeAppParam.in.npcSelectListNum  = 30;
      App::BattleTree::NPC_SELECT_ITEM* pItem = NULL;
      PokeTool::SimpleParam*  pSimpleParam  = NULL;

      gfl2::heap::HeapBase* pDebugHeap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      gfl2::str::StrBuf*  pName = GFL_NEW_LOW( pDebugHeap ) gfl2::str::StrBuf( 256, pDebugHeap->GetLowerHandle() );

      for( u32 i = 0; i < s_debugBattleTreeAppParam.in.npcSelectListNum; ++i )
      {
        pItem = &s_debugBattleTreeAppParam.in.aNpcSelectItem[ i ];
        pItem->type  = (trainer::TrType)( trainer::TRTYPE_TANPAN + i );

        pName->SetStr( L"battleTree" );

        pSimpleParam  = &pItem->aPokeSimpleParam[ 0 ];
        pSimpleParam->monsNo  = MONSNO_NOKOTTI;

        if( i == 1 )
        {
          pName->SetStr( L"uchida" );
          pSimpleParam->monsNo  = MONSNO_NAGAREBOSI;
          pItem->type  = trainer::TRTYPE_SWIMMERM;
        }
        else if( i == 2 )
        {
          pName->SetStr( L"リーリエ" );
          pSimpleParam->monsNo  = MONSNO_PIKATYUU;
          pItem->type  = trainer::TRTYPE_LILIE;
        }
        else if( i == 3 )
        {
          pName->SetStr( L"ククイ" );
          pSimpleParam->monsNo  = MONSNO_RIZAADON;
          pItem->type  = trainer::TRTYPE_DOCTORNRBT;
        }
        else if( i == 9 )
        {
          pName->SetStr( L"tanaka" );

          pSimpleParam->monsNo  = MONSNO_NAGAREBOSI;
          pItem->type  = trainer::TRTYPE_GENTLE;

          PokeTool::SimpleParam*  pSimpleParam02  = &pItem->aPokeSimpleParam[ 1 ];
          pSimpleParam02->monsNo  = MONSNO_NOKOTTI;
        }

        if( i == 29 )
        {
          pName->SetStr( L"maou" );
        }

        pName->PutStr( pItem->aName, GFL_NELEMS( pItem->aName ) );
      }

      GFL_SAFE_DELETE( pName );

      App::Event::BattleTreeEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &s_debugBattleTreeAppParam );

      break;
    }

    //  キャラアイコンテスト（テクスチャー出力）
    case label_uchida_ui_char_icon_test_2:
    {
      //  キャラアイコン
      Test::UICharaTexReplaceTestProc*  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::UICharaTexReplaceTestProc>( 
                GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "UIUnitTest.cro" );

      pProc->SetEnableOutputTexture( true );
      break;
    }

    //  すごい特訓(金の王冠)
    case label_uchida_ui_super_training:
    {
      s_debugSuperTrainingAppParam.in.pPokeParam   = s_pSuperTrainingPokeParam;
      s_debugSuperTrainingAppParam.in.useCrown  = App::SuperTraining::USE_CROWN_GOLD;

      //  テストで金の王冠を一つ追加
      {
        Savedata::MyItem* pMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();
        pMyItem->Add( ITEM_KINNOOUKAN, 456 );
      }

      App::Event::SuperTrainingEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &s_debugSuperTrainingAppParam );
      break;
    }

    //  すごい特訓（銀の王冠）
    case label_uchida_ui_super_training_1:
    {
      s_debugSuperTrainingAppParam.in.pPokeParam   = s_pSuperTrainingPokeParam;
      s_debugSuperTrainingAppParam.in.useCrown  = App::SuperTraining::USE_CROWN_SILVER;

      //  テストで銀の王冠を追加
      {
        Savedata::MyItem* pMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();
        pMyItem->Add( ITEM_GINNOOUKAN, 999 );
      }

      App::Event::SuperTrainingEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), &s_debugSuperTrainingAppParam );

      break;
    }

    //  全きせかえ設定（色染めは抜かす）
    case label_uchida_all_buy_kisekae:
    {
      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
      Savedata::Fashion* pFashionSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFashion();

      //  いったんすべてきせかえクリアする
      {
        pFashionSaveData->Clear( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
        //  デフォルト所持
        pFashionSaveData->InitializeDefaultFashionItem( ( pMyStatus->GetSex() == poke_3d::model::DressUpParam::SEX_FEMALE ) );
      }

      poke_3d::model::DressUpParam::Sex sex = (poke_3d::model::DressUpParam::Sex)pMyStatus->GetSex();

      gfl2::ro::RoManager* pRoMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
      gfl2::ro::Module* pModule        = pRoMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "KisekaeData.cro");
      pRoMgr->StartModule( pModule, true );

      for( u32 category_idx = 0; category_idx < Savedata::Fashion::CATEGORY_MAX; ++category_idx )
      {
        u32 goodsMax  = app::util::Kisekae::KisekaeDataBase::GetGoodsDataNum( (Savedata::Fashion::Category)category_idx );
        for( u32 goods_idx = 0; goods_idx < goodsMax; ++goods_idx )
        {
          const table_data_GoodsData* pGoodsData  = app::util::Kisekae::KisekaeDataBase::GetGoodsData( (Savedata::Fashion::Category)category_idx, goods_idx );
          if( pGoodsData->sex != sex )
          {
            continue;
          }

          if( pGoodsData->color_change == 1 )
          {
            //  色ではなく色テキストで判定
            //  ホワイトとライトグレーは追加
            if( ( pGoodsData->color_txt_id != kisekae_color_01_01 )
            &&  ( pGoodsData->color_txt_id != kisekae_color_06_03 ) )
            {
              continue;
            }
          }

          //  セーブに追加
          pFashionSaveData->Add( category_idx, pGoodsData->item_id );
        }
      }
      pRoMgr->DisposeModule( pModule );

      break;
    }

    //  スタッフロール（初回）
    case label_uchida_staffroll_test_01:
    {
      //  白フェードアウトから始まるのでその想定画面を作る
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      static const gfl2::math::Vector4 scou(255.0f,255.0f,255.0f,255.0f);

      pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &scou );

      GameSys::CallStaffrollProc( true );
      break;
    }

    //  スタッフロール
    case label_uchida_staffroll_test:
    {
      GameSys::CallStaffrollProc( false );
      break;
    }

    case label_uchida_end_roll:
    {
      //  エンドロール
      GameSys::CallEndRollProc( NULL );
      break;
    }
  }

  return launcher::ListData::REQID_NONE;
}

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
