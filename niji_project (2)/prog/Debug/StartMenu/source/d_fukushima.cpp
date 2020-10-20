#if PM_DEBUG
//=============================================================================
/**
 * @file    d_fukushima.cpp
 * @brief   福島用ランチャーリスト処理
 * @author  fukushima_yuya
 * @date    2015.03.02
 */
//=============================================================================
#if defined(PM_DEBUG)

// niji
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/StartMenu/include/d_fukushima.h>

#include <App/AppEvent/include/PokeList/PokeListEvent.h>
#include <App/AppEvent/include/FinderStudio/FinderStudioCaptureEvent.h>
#include <App/AppEvent/include/MyAlbum/MyAlbumEvent.h>
#include <App/BoxSearch/include/BoxSearchProc.h>
#include <App/HeroSelect/include/HeroSelectProc.h>
#include <App/LangSelect/include/LangSelectProc.h>
#include <App/TitleMenu/include/TitleMenuProc.h>
#include <App/WazaOshie/include/WazaOshieProc.h>
//#include <App/EventList/include/EventListProc.h>
#include <AppLib/include/Tool/AppToolTimeLimit.h>
#include <GameSys/include/DllProc.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>
#include <System/include/gflUse.h>
#include <System/include/HeapDefine.h>
#include <Test/LayoutArcTest/include/LayoutArcTest.h>
#include <Test/DrawUtilTextTest/include/DrawUtilTextProc.h>
#include <Test/FukushimaTest/include/FukushimaTestProc.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_fukushima.h>



GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

// 項目文字列データテーブル
const u32 LauncherFukushima::ItemList[] =
{
  ImageDBTest_00,
  StudioDecoTest_01,
  DebugFSCapture,
  DebugFSAlbum,
  Debug_PokeList_00,
  Debug_PokeList_01,
  Debug_PokeList_02,
  Debug_PokeList_03,
  Debug_PokeList_04,
  Debug_PokeList_05,
  Debug_PokeList_06,
  Debug_WazaOshie_00,
  Debug_WazaOshie_01,
  Debug_TitleMenu_00,
  Debug_LangSelect_00,
  Debug_BoxSearch_00,
  Debug_BtlFont_00,
  Debug_HeroSelect_00,
  Debug_FinderMenu_00,
  Debug_StrInput_00,
  Debug_StrInput_01,
  Debug_StrInput_02,
  Debug_StrInput_03,
  Debug_StrInput_04,
  Debug_StrInput_05,
  Debug_Test_00,
  Debug_Test_01,
  Debug_Test_02,
  Debug_Test_03,
  Debug_EventList_00,
};

enum {
  ITEM_IMGDB_TEST,
  ITEM_DECO_TEST,
  ITEM_DEBUG_FINDER_STUDIO_CAPTURE,
  ITEM_DEBUG_FINDER_STUDIO_ALBUM,
  ITEM_POKELIST_DEBUG_FIELD,
  ITEM_POKELIST_JOIN,
  ITEM_POKELIST_SIMPLE,
  ITEM_POKELIST_SIMPLE2,
  ITEM_POKELIST_SODATEYA,
  ITEM_POKELIST_WAZAOSHIE,
  ITEM_POKELIST_FIELD,
  ITEM_WAZAOBOE,
  ITEM_WAZAWASURE,
  ITEM_TITLE_MENU,
  ITEM_LANG_SELECT,
  ITEM_BOX_SEARCH,
  ITEM_BATTLE_FONT_TEST,
  ITEM_HERO_SELECT,
  ITEM_FINDER_MENU,
  ITEM_STRINPUT_CHARNAME,
  ITEM_STRINPUT_POKEMONNAME,
  ITEM_STRINPUT_BOXNAME,
  ITEM_STRINPUT_GTS,
  ITEM_STRINPUT_TEAMNAME,
  ITEM_STRINPUT_KOREA_TEST,
  ITEM_TEST_POKELIST,
  ITEM_TEST_STATUS,
  ITEM_TEST_ICON,
  ITEM_TEST_MODEL,
  ITEM_EVENT_LIST,
  ITEM_NUM,
};


pml::PokeParty* LauncherFukushima::m_spParty_Myself  = NULL;
pml::PokeParty* LauncherFukushima::m_spParty_Friend  = NULL;
pml::PokeParty* LauncherFukushima::m_spParty_Enemy_0 = NULL;
pml::PokeParty* LauncherFukushima::m_spParty_Enemy_1 = NULL;
App::Tool::TimeLimit* LauncherFukushima::m_spTimeLimit = NULL;

//-----------------------------------------------------------------------------
// @brief   破棄
//-----------------------------------------------------------------------------
void LauncherFukushima::Delete( void )
{
  GFL_SAFE_DELETE(m_spTimeLimit);
  GFL_SAFE_DELETE(m_spParty_Enemy_1);
  GFL_SAFE_DELETE(m_spParty_Enemy_0);
  GFL_SAFE_DELETE(m_spParty_Friend);
  GFL_SAFE_DELETE(m_spParty_Myself);
}


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LauncherFukushima::LauncherFukushima(void)
  : m_pGameManager( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) )
  , m_pFadeManager( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager ) )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherFukushima::~LauncherFukushima()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetArcDataID
 * @brief   アークデータID取得
 * @date    2015.02.18
 *
 * @return  アークデータID
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID LauncherFukushima::GetArcDataID(void)
{
  return GARC_debug_message_d_fukushima_DAT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブル取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブル
 */
//-----------------------------------------------------------------------------
const u32 * LauncherFukushima::GetListTable(void)
{
  return ItemList;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブルの要素数取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブルの要素数
 */
//-----------------------------------------------------------------------------
u32 LauncherFukushima::GetListTableSize(void)
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
launcher::ListData::RequestID LauncherFukushima::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  launcher::ListData::RequestID reqID = launcher::ListData::REQID_NONE;

  // フェードアウト
  FadeOut();

  switch( pos )
  {
    case ITEM_POKELIST_DEBUG_FIELD:   //!< フィールドポケモンリスト(デバッグ)
    case ITEM_POKELIST_JOIN:          //!< 参加選択ポケモンリスト
    case ITEM_POKELIST_SIMPLE:        //!< 対象選択ポケモンリスト
    case ITEM_POKELIST_SIMPLE2:       //!< 対象選択２ポケモンリスト
    case ITEM_POKELIST_SODATEYA:      //!< 育て屋ポケモンリスト
    case ITEM_POKELIST_WAZAOSHIE:     //!< 技教えポケモンリスト
    case ITEM_POKELIST_FIELD:         //!< フィールドポケモンリスト
      {
        reqID = CallPokeProc( pos );
      } break;

    case ITEM_WAZAOBOE:     //!< 技教え
    case ITEM_WAZAWASURE:   //!< 技忘れ
      {
        reqID = CallWazaOshieProc( pos );
      } break;

    case ITEM_TITLE_MENU:   //!< タイトルメニュー
      {
        reqID = CallTitleMenuProc( pos );
      } break;

    case ITEM_LANG_SELECT:  //!< 言語選択
      {
        reqID = CallLangSelectProc( pos );
      } break;

    case ITEM_BOX_SEARCH:   //!< ボックス検索
      {
        reqID = CallBoxSearchProc( pos );
      } break;

    case ITEM_EVENT_LIST:   //!< イベントリスト
      {
        reqID = CallEventListProc( pos );
      } break;

    case ITEM_HERO_SELECT:   //!< 容姿・性別選択
      {
        reqID = CallHeroSelectProc( pos );
      } break;

    case ITEM_FINDER_MENU:   //!< ファインダーメニュー
      {
        reqID = CallFinderMenuProc( pos );
      } break;

    case ITEM_STRINPUT_CHARNAME:      //!< キャラクタ名入力
    case ITEM_STRINPUT_POKEMONNAME:   //!< ポケモン名入力
    case ITEM_STRINPUT_BOXNAME:       //!< ボックス名入力
    case ITEM_STRINPUT_GTS:           //!< GTS検索ポケモン名入力
    case ITEM_STRINPUT_TEAMNAME:      //!< チーム名入力
    case ITEM_STRINPUT_KOREA_TEST:    //!< 韓国語テスト
      {
        reqID = CallStrInputProc( pos );
      } break;

    case ITEM_TEST_POKELIST:    //!< テスト環境：ポケモンリストテスト
    case ITEM_TEST_STATUS:      //!< テスト環境：ステータステスト
    case ITEM_TEST_ICON:        //!< テスト環境：アイコンテスト
    case ITEM_TEST_MODEL:       //!< テスト環境：ポケモンモデルテスト
      {
        reqID = CallTestProc( pos );
      } break;

    case ITEM_BATTLE_FONT_TEST:
      {
        reqID = CallBattleFontTestProc( pos );
      } break;

    case ITEM_IMGDB_TEST:
    {
      reqID = CallImageDBTestProc( pos );
    } break;

    case ITEM_DECO_TEST:
    {
      reqID = CallFSDecoTestProc( pos );
    } break;

    case ITEM_DEBUG_FINDER_STUDIO_CAPTURE:
    {
      reqID = CallFinderStudioCaptureProc( pos );
    } break;

    case ITEM_DEBUG_FINDER_STUDIO_ALBUM:
    {
      reqID = CallFinderStudioAlbumProc( pos );
    } break;
  } // switch( pos )

  return reqID;
}



//-----------------------------------------------------------------------------
/*
 * @brief   フェード処理
 */
//-----------------------------------------------------------------------------
void LauncherFukushima::FadeOut( void )
{
  gfl2::math::Vector4 color( 0, 0, 0, 255 );  //!< フェードカラー

  m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/*
 * @brief   ポケモンリスト
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallPokeProc( u32 pos )
{
  app::event::PokeListEvent * pEvent =
    app::event::PokeListEvent::StartEvent( m_pGameManager, pos, true );

  if( pos != Debug_PokeList_06 )
  {
    InitParty();    //!< 仮データの設定
  }

  u32 index = static_cast<u32>( pos - Debug_PokeList_00 );
  SetPokeListAppParam( index );

  pEvent->SetPokeListAppParam( &m_PokeListContext );

  return ListData::REQID_FINISH;
}

//-----------------------------------------------------------------------------
/*
 * @brief   技教え・技忘れ
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallWazaOshieProc( u32 pos )
{
  InitParty();    //!< 仮データの設定

  m_WazaOshieAppParam.mode = static_cast<u8>(pos - Debug_WazaOshie_00);
  m_WazaOshieAppParam.select_poke_index = 0;//static_cast<u8>( System::GflUse::GetPublicRand() % pml::PokeParty::MAX_MEMBERS );
  m_WazaOshieAppParam.waza_no = static_cast<WazaNo>( System::GflUse::GetPublicRand() % WAZANO_MAX );
  m_WazaOshieAppParam.upper_draw = static_cast<u8>( System::GflUse::GetPublicRand() % 2 );

  App::WazaOshie::Proc * proc = GameSys::CallWazaOshieProc();
  proc->SetAppParam( &m_WazaOshieAppParam );

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
 * @brief   タイトルメニュー
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallTitleMenuProc( u32 pos )
{
  GFL_UNUSED( pos );

  m_TitleMenuAppParam.initMode = App::TitleMenu::INIT_MODE_NONE;

  App::TitleMenu::Proc * proc = GameSys::CallTitleMenuProc();
  proc->SetAppParam( &m_TitleMenuAppParam );

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
 * @brief   言語選択
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallLangSelectProc( u32 pos )
{
  GFL_UNUSED( pos );

  m_LangSelectAppParam.defaultLang = POKEMON_LANG_JAPAN;
  m_LangSelectAppParam.debugFlag = true;

  App::LangSelect::Proc * proc = GameSys::CallLangSelectProc();
  proc->SetAppParam( &m_LangSelectAppParam );

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
 * @brief   ボックス検索
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallBoxSearchProc( u32 pos )
{
  GFL_UNUSED( pos );

  App::BoxSearch::Proc * proc = GameSys::CallBoxSearchProc();
  proc->SetAppParam( &m_BoxSearchAppParam );

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
 * @brief   イベントリスト
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallEventListProc( u32 pos )
{
  GFL_UNUSED( pos );

  ////m_EventListAppParam.mode = App::EventList::EVENT_LIST_MODE_MARK_MAIN;

  //GameSys::GameData* gd = m_pGameManager->GetGameData();
  //Savedata::Misc* misc = gd->GetMisc();
  //misc->SetTargetEventID( Savedata::Misc::EVENT_LIST_TYPE_MAIN, 1 );
  //misc->SetTargetEventID( Savedata::Misc::EVENT_LIST_TYPE_SUB, 0 );

  //App::EventList::Proc * proc = GameSys::CallEventListProc();
  //proc->SetAppParam( &m_EventListAppParam );

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
 * @brief   容姿・性別選択
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallHeroSelectProc( u32 pos )
{
  GFL_UNUSED( pos );

  GameSys::CallHeroSelectProc();

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
 * @brief   ファインダーメニュー
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallFinderMenuProc( u32 pos )
{
  GFL_UNUSED( pos );

  GameSys::CallFinderMenuProc();

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
 * @brief   文字入力
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallStrInputProc( u32 pos )
{
  InitParty();    //!< 仮データの設定

  u32 index = static_cast<u32>( pos - Debug_StrInput_00 );

  Test::Fukushima::Proc * proc = GameSys::CallFukushimaTestProc();
  
  if( pos != ITEM_STRINPUT_KOREA_TEST )
  {
    proc->SetCreateFrame( TEST_STRINPUT );
    proc->SetStrInputMode( index );
  }
  else {
    proc->SetCreateFrame( TEST_STRINPUT_KOREA );
    proc->SetStrInputMode( 0 );
  }

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
 * @brief   ポケモンリストの外部設定パラメータの設定
 * @param   pos   選択した項目インデックス
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallTestProc( u32 pos )
{
  static const u32 c_TestProcList[] = {
    TEST_POKELIST,    //!< ポケモンリストテスト
    TEST_STATUS,      //!< ステータステスト
    TEST_ICON,        //!< アイコンテスト
    TEST_POKE_MODEL,  //!< ポケモンモデルテスト
  };

  InitParty();    //!< 仮データの設定

  u32 index = static_cast<u32>( pos - Debug_Test_00 );
  Test::Fukushima::Proc * proc = GameSys::CallFukushimaTestProc();
  proc->SetCreateFrame( c_TestProcList[index] );

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
* @brief   バトルフォント表示テスト
* @param   pos   選択した項目インデックス
*/
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallBattleFontTestProc( u32 pos )
{
  GameSys::CallBattleFontTestProc();

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
* @brief   ImageDBテスト
* @param   pos   選択した項目インデックス
*/
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallImageDBTestProc( u32 pos )
{
  GameSys::CallImageDBTestProc();

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
* @brief   ファインダースタジオデコテスト
* @param   pos   選択した項目インデックス
*/
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallFSDecoTestProc( u32 pos )
{
  GameSys::CallFSDecoTestProc();

  return ListData::REQID_CALL;
}

//-----------------------------------------------------------------------------
/*
* @brief   ファインダースタジオ
* @param   pos   選択した項目インデックス
*/
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallFinderStudioCaptureProc( u32 pos )
{
  app::event::FinderStudioCaptureEvent::StartEvent( m_pGameManager, true );

  return ListData::REQID_FINISH;
}

//-----------------------------------------------------------------------------
/*
* @brief   ファインダースタジオアルバム
* @param   pos   選択した項目インデックス
*/
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherFukushima::CallFinderStudioAlbumProc( u32 pos )
{
  App::Event::MyAlbumEvent::StartEvent( m_pGameManager );

  return ListData::REQID_FINISH;
}





//-----------------------------------------------------------------------------
/*
 * @brief   ポケモンリストの外部設定パラメータの設定
 */
//-----------------------------------------------------------------------------
void LauncherFukushima::SetPokeListAppParam( u32 mode )
{
  static const App::PokeList::Mode modeTbl[] = {
    App::PokeList::MODE_FIELD,
    App::PokeList::MODE_JOIN,
    App::PokeList::MODE_SIMPLE,
    App::PokeList::MODE_SIMPLE2,
    App::PokeList::MODE_SODATEYA,
    App::PokeList::MODE_WAZAOSHIE,
    App::PokeList::MODE_FIELD,
  };
  m_PokeListContext.mode = modeTbl[mode];

  m_PokeListContext.join_count_max = 4;
  m_PokeListContext.join_count_min = 3;
  m_PokeListContext.join_upp_mode = static_cast<App::PokeList::JoinUppMode>( System::GflUse::GetPublicRand() % App::PokeList::JOIN_UPP_MODE_NUM );

  if( m_PokeListContext.mode == App::PokeList::MODE_JOIN )
  {
    gfl2::heap::HeapBase * heap =
      gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    {
      m_spTimeLimit = GFL_NEW(heap) App::Tool::TimeLimit( 100 );
      {
        m_PokeListContext.time_limit = m_spTimeLimit;
      }
    }
  }

  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    { // ポケモン選択可能
      u8 rand = static_cast<u8>( System::GflUse::GetPublicRand() % 2 );
      m_PokeListContext.poke_select_enable[i] = rand;
    }
    { // 技習得状態
      u8 rand = static_cast<u8>( System::GflUse::GetPublicRand() % 3 );
      m_PokeListContext.waza_set_param[i] = rand;
    }
  }

  for( u32 i=0; i<App::PokeList::PARTY_NUM; ++i )
  {
    { // 通信待機状態
      u8 rand = static_cast<u8>( System::GflUse::GetPublicRand() % 2 );
      m_PokeListContext.selected_wait[i] = rand;
    }
    {
      u8 rand = static_cast<u8>( System::GflUse::GetPublicRand() % 2 );
      m_PokeListContext.qrcord[i] = rand;
    }
  }

  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  {
    GameSys::GameData * gd = gm->GetGameData();
    {
      m_PokeListContext.my_status = gd->GetPlayerStatus();

      m_PokeListContext.my_party.SetParameter( m_spParty_Myself );
      m_PokeListContext.friend_party.SetParameter( m_spParty_Friend );
      m_PokeListContext.enemy_party_0.SetParameter( m_spParty_Enemy_0 );
      m_PokeListContext.enemy_party_1.SetParameter( m_spParty_Enemy_1 );
    }
  }
}






//-----------------------------------------------------------------------------
/*
 * @brief   パーティの初期化
 */
//-----------------------------------------------------------------------------
void LauncherFukushima::InitParty( void )
{
  gfl2::heap::HeapBase * heap =
    gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  {
    m_spParty_Myself  = GFL_NEW(heap) pml::PokeParty( heap );
    m_spParty_Friend  = GFL_NEW(heap) pml::PokeParty( heap );
    m_spParty_Enemy_0 = GFL_NEW(heap) pml::PokeParty( heap );
    m_spParty_Enemy_1 = GFL_NEW(heap) pml::PokeParty( heap );
  }

  pml::PokeParty* partyTable[App::PokeList::PARTY_NUM] = {
    m_spParty_Myself,
    m_spParty_Friend,
    m_spParty_Enemy_0,
    m_spParty_Enemy_1,
  };

  const char* partyNameTable[] = {
    "MYSELF",
    "FRIEND",
    "ENEMY0",
    "ENEMY1",
  };

  const u32 memberCountTable[] = {
    3,//pml::PokeParty::MAX_MEMBERS,
    1,//pml::PokeParty::MAX_MEMBERS,
    pml::PokeParty::MAX_MEMBERS,
    pml::PokeParty::MAX_MEMBERS,
  };

  for( int i=0; i<App::PokeList::PARTY_NUM; ++i )
  {
    int memberCount = memberCountTable[i];//(System::GflUse::GetPublicRand() % 5) + 1;

    FUKUSHIMA_PRINT( "MemberCount : Name[%s] = [%d]\n",
      partyNameTable[i], memberCount );

    for( int j=0; j<memberCount; ++j )
    {
      CreatePokeParam( partyTable[i], j );
    }
  }
}


//-----------------------------------------------------------------------------
/*
 * @brief   パーティの初期化
 */
//-----------------------------------------------------------------------------
void LauncherFukushima::CreatePokeParam( pml::PokeParty* party, u32 index )
{
  gfl2::heap::HeapBase * heap =
    gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  pml::pokepara::PokemonParam* pp = NULL;
  {
    // ポケモンID
    MonsNo monsNo = static_cast<MonsNo>(System::GflUse::GetPublicRand() % MONSNO_MAX);
    // レベル
    u16 level = static_cast<u16>(System::GflUse::GetPublicRand() % 100) + 1;

    // ポケモンパラメータの生成
    pp = GFL_NEW_LOW(heap) pml::pokepara::PokemonParam( heap, monsNo, level, 0 );
    {
      // 性別
      if( pp->GetSex() != pml::SEX_UNKNOWN )
      {
        pml::Sex sex = static_cast<pml::Sex>(System::GflUse::GetPublicRand() % 2 );
        pp->ChangeSex( sex );
      }

      // 特性のセット
      pp->SetTokuseiIndex( 0 );
      // 性格のセット
      pml::pokepara::Seikaku seikaku = static_cast<pml::pokepara::Seikaku>(System::GflUse::GetPublicRand() % pml::pokepara::SEIKAKU_NUM );
      pp->ChangeSeikaku( seikaku );

      // 親名
      Savedata::MyStatus* mystatus = m_pGameManager->GetGameData()->GetPlayerStatus();
      {
        mystatus->SetPokemonParent( pp, heap );
      }
      // カセットバージョン
      pp->SetCassetteVersion( static_cast<u32>(GET_VERSION()) );
      // 言語
      pp->SetLangId( System::LANGUAGE_JAPAN );
      // ボール
      u32 ballID = static_cast<u32>(System::GflUse::GetPublicRand() % item::BALLID_MAX);
      pp->SetGetBall( ballID );
      // 思い出
      pp->SetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL, item::BALLID_MONSUTAABOORU );
      pp->SetMemories( pml::pokepara::MEMORIES_CODE_WITH_PARENT, 1 );
      pp->SetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_PARENT, 1 );

      // 固体値
      {
        u32 val = System::GflUse::GetPublicRand() % 31;

        for( int i=0; i<pml::pokepara::POWER_NUM; ++i )
        {        
          pp->ChangeTalentPower( static_cast<pml::pokepara::PowerID>(i), val );
        }
      }

      // 技
      {
        u32 wazaCount = (System::GflUse::GetPublicRand() % 4) + 1;
        
        for( u32 i=0; i<4; ++i )
        {
          WazaNo wazaNo = WAZANO_NULL;

          if( i < wazaCount )
          {
            wazaNo = static_cast<WazaNo>( System::GflUse::GetPublicRand() % WAZANO_MAX );
          }
          pp->SetWaza( i, wazaNo );
        }
      }

      // アイテム
      {
        bool itemFlg = static_cast<bool>(System::GflUse::GetPublicRand() % 2);
        u16 itemNo = ITEM_DUMMY_DATA;

        if( itemFlg )
        {
          itemNo = System::GflUse::GetPublicRand() % ITEM_DATA_MAX;
        }

        pp->SetItem( itemNo );
      }

      // 経験値
      {
        u32 expCurrent = pp->GetExpForCurrentLevel();
        u32 expNext    = pp->GetExpForNextLevel();
        f32 expSub     = static_cast<f32>(expNext - expCurrent);

        f32 rate = static_cast<f32>( System::GflUse::GetPublicRand() % 100 + 1 ) / 100.0f;
        u32 expOffset = static_cast<u32>( expSub * rate );

        pp->SetExp( expCurrent + expOffset );
      }

      // 状態異常
      {
        bool sickFlg = static_cast<bool>(System::GflUse::GetPublicRand() % 2);
        pml::pokepara::Sick sick = pml::pokepara::SICK_NULL;

        if( sickFlg )
        {
          sick = static_cast<pml::pokepara::Sick>(System::GflUse::GetPublicRand() % pml::pokepara::SICK_MAX );
        }

        pp->SetSick( sick );
      }

      party->ReplaceMember( index, *pp );
    }
    GFL_DELETE pp;
  }
}

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
#endif //PM_DEBUG
