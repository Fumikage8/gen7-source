#if PM_DEBUG
//=============================================================================
/**
 * @file    DebugPartsLauncher.cpp
 * @brief   大野メニューデバッグ用ランチャー
 * @author  k.ohno
 * @date    2015.02.20
 */
//=============================================================================
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Test/MenuFrameTest/include/DebugPartsLauncher.h>
#include <Fade/include/gfl2_FadeManager.h>

#include <Test/MenuFrameTest/include/MenuTestProc.h>
#include <Test/MenuFrameTest/include/MenuSampleEvent.h>
#include <Test/MenuFrameTest/include/ThreadSampleEvent.h>
#include <Test/MenuFrameTest/include/FieldMessageSampleEvent.h>
#include <Test/MenuFrameTest/include/TalkWindowSampleEvent.h>
#include <Test/MenuFrameTest/include/ListSampleEvent.h>

#include "App/AppEvent/include/Movie/MovieEvent.h"
#include "App/AppEvent/include/Box/BoxEvent.h"

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_menuparts.h>
#include <niji_conv_header/message/debug/msg_d_langselect.h>

#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/DllProc.h>

#include "App/Box/include/BoxAppParam.h"

#include "niji_conv_header/poke_lib/monsno_def.h"
#include "pml/include/pml_Const.h"

#include <System/include/HeapDefine.h>

#include "App/AppEvent/include/PokeIsland/PokeIslandEvent.h"

#include <GameSys/include/DllProc.h>
#include <Test/MenuFrameTest/include/OhnoDebugStartProc.h>


#include <App/AppDemo/include/AppDemoProc.h>
#include <App/AppDemo/include/AppDemoProcParam.h>
#include "Field/FieldStatic/include/Event/EventGameStart.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(MenuFrameTest)

// 項目文字列データテーブル
const u32 DebugPartsLauncher::ItemList[] =
{
  debug_menu_parts_launcher_19,
  debug_menu_parts_launcher_16,
  debug_menu_parts_launcher_17,
  debug_menu_parts_launcher_14,
  debug_menu_parts_launcher_15,
  debug_menu_parts_launcher_13,
  debug_menu_parts_launcher_00,
  debug_menu_parts_launcher_01,
  debug_menu_parts_launcher_07,
  debug_menu_parts_launcher_08,
  debug_menu_parts_launcher_09,
  debug_menu_parts_launcher_10,
  debug_menu_parts_launcher_20,
  debug_menu_parts_launcher_21,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
DebugPartsLauncher::DebugPartsLauncher(void)
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
DebugPartsLauncher::~DebugPartsLauncher()
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
gfl2::fs::ArcFile::ARCDATID DebugPartsLauncher::GetArcDataID(void)
{
  return GARC_debug_message_d_menuparts_DAT;
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
const u32 * DebugPartsLauncher::GetListTable(void)
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
u32 DebugPartsLauncher::GetListTableSize(void)
{
  return elementof(ItemList);
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
debug::launcher::ListData::RequestID DebugPartsLauncher::SelectListener( u32 pos, debug::launcher::LauncherProc * launcher )
{
	 Test::MenuFrameTest::OhnoDebugStartProc* pProc;
	// キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
  case debug_menu_parts_launcher_00:
    pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);

    //    MenuSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));

    return debug::launcher::ListData::REQID_FINISH;

  case debug_menu_parts_launcher_01:
    pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);

//    Test::ThreadTest::ThreadSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));

    return debug::launcher::ListData::REQID_FINISH;

  case debug_menu_parts_launcher_07:
    pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);

//    Test::FieldMessageTest::FieldMessageSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));

    return debug::launcher::ListData::REQID_FINISH;

  case debug_menu_parts_launcher_09:
    pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);

//    Test::TalkWindowTest::TalkWindowSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));

    return debug::launcher::ListData::REQID_FINISH;
    
  case debug_menu_parts_launcher_10:
    pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);

//    Test::List::ListSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));

    return debug::launcher::ListData::REQID_FINISH;

  case debug_menu_parts_launcher_13:
    pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);

//    App::Event::PokeIslandEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager), true);

    return debug::launcher::ListData::REQID_FINISH;

  case debug_menu_parts_launcher_14:
    pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);
/*    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
      App::Event::BoxEvent* pEvent = App::Event::BoxEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
      pEvent->SetBoxStartType(App::Box::BOX_CALL_PC);
    }*/
    return debug::launcher::ListData::REQID_CALL;
    
  case debug_menu_parts_launcher_15:
		pProc = GameSys::CallMenuFrameTestProc( );
		pProc->SetNo(ItemList[pos]);
/*
		{
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
			App::Event::BoxEvent* pEvent = App::Event::BoxEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
      pEvent->SetBoxStartType(App::Box::BOX_CALL_MIRACLE);
      pEvent->TradePokemon(MONSNO_HITOKAGE, pml::SEX_NUM, 50);
    }
		*/
    return debug::launcher::ListData::REQID_CALL;

  case debug_menu_parts_launcher_16:
     pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);
/*    {
			GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
      App::Event::BoxEvent* pEvent = App::Event::BoxEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
      pEvent->SetBoxStartType(App::Box::BOX_CALL_BATTLE_TEAM);
    }*/

    return debug::launcher::ListData::REQID_CALL;

  case debug_menu_parts_launcher_17:
     pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);
/*    {
			GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
      App::Event::BoxEvent* pEvent = App::Event::BoxEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
      pEvent->SetBoxStartType(App::Box::BOX_CALL_RESORT_GIM);
    }*/

    return debug::launcher::ListData::REQID_CALL;

  case debug_menu_parts_launcher_19:
    pProc = GameSys::CallMenuFrameTestProc( );
    pProc->SetNo(ItemList[pos]);

    return debug::launcher::ListData::REQID_CALL;


  case debug_menu_parts_launcher_20:
    {
      App::Event::MovieEvent* pEvent = App::Event::MovieEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true,0 );    // movie
      App::Event::APP_PARAM x;
      x.movieNo=11;
      pEvent->SetAppParam(&x);
      
    }
//    pProc = GameSys::CallMenuFrameTestProc( );  //movie
  // / pProc->SetNo(ItemList[pos]);
    return debug::launcher::ListData::REQID_FINISH;

    
  }


  return debug::launcher::ListData::REQID_NONE;
}





























// 項目文字列データテーブル
const u32 DebugLangSelectLauncher::ItemList[] =
{
  DEBUG_LANGSELECT_001,
  DEBUG_LANGSELECT_002,
  DEBUG_LANGSELECT_003,
  DEBUG_LANGSELECT_004,
  DEBUG_LANGSELECT_005,
  DEBUG_LANGSELECT_006,
  DEBUG_LANGSELECT_007,
  DEBUG_LANGSELECT_008,
  DEBUG_LANGSELECT_009,
  DEBUG_LANGSELECT_010,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
DebugLangSelectLauncher::DebugLangSelectLauncher(void)
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
DebugLangSelectLauncher::~DebugLangSelectLauncher()
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
gfl2::fs::ArcFile::ARCDATID DebugLangSelectLauncher::GetArcDataID(void)
{
  return GARC_debug_message_d_langselect_DAT;
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
const u32 * DebugLangSelectLauncher::GetListTable(void)
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
u32 DebugLangSelectLauncher::GetListTableSize(void)
{
  return elementof(ItemList);
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
debug::launcher::ListData::RequestID DebugLangSelectLauncher::SelectListener( u32 pos, debug::launcher::LauncherProc * launcher )
{
  Test::MenuFrameTest::OhnoDebugStartProc* pProc;
  gfl2::heap::HeapBase * devHeap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
	// キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
  case DEBUG_LANGSELECT_001:
    System::SetLang( POKEMON_LANG_JAPAN, devHeap, false );
    break;
  case DEBUG_LANGSELECT_002:
    System::SetLang( POKEMON_LANG_JAPAN, devHeap, true );
    break;
  case DEBUG_LANGSELECT_003:
    System::SetLang( POKEMON_LANG_ENGLISH, devHeap, false );
    break;
	case DEBUG_LANGSELECT_004:
    System::SetLang( POKEMON_LANG_FRANCE, devHeap, false );
    break;
  case DEBUG_LANGSELECT_005:
    System::SetLang( POKEMON_LANG_ITALY, devHeap, false );
    break;
  case DEBUG_LANGSELECT_006:
    System::SetLang( POKEMON_LANG_GERMANY, devHeap, false );
    break;
  case DEBUG_LANGSELECT_007:
    System::SetLang( POKEMON_LANG_SPAIN, devHeap, false );
    break;
  case DEBUG_LANGSELECT_008:
    System::SetLang( POKEMON_LANG_KOREA, devHeap, false );
    break;
  case DEBUG_LANGSELECT_009:
    System::SetLang( POKEMON_LANG_CHINA, devHeap, false );
    break;
  case DEBUG_LANGSELECT_010:
    System::SetLang( POKEMON_LANG_TAIWAN, devHeap, false );
    break;

  }
/*
#define POKEMON_LANG_ENGLISH       2  ///<  言語コード：英語        USA
#define POKEMON_LANG_FRANCE        3  ///<  言語コード：フランス語  FRA
#define POKEMON_LANG_ITALY         4  ///<  言語コード：イタリア語  ITA
#define POKEMON_LANG_GERMANY       5  ///<  言語コード：ドイツ語    DEU
#define POKEMON_LANG_SPAIN         7  ///<  言語コード：スペイン語  ESP
#define POKEMON_LANG_KOREA         8  ///<  言語コード：韓国語      KOR
#define POKEMON_LANG_CHINA         9  ///<  言語コード：簡体字（中国）      CHN
#define POKEMON_LANG_TAIWAN        10 ///<  言語コード：繁体字（台湾、香港）    TWN
*/
  
  {
    Field::EventGameStart::Description desc = {
      Field::EventGameStart::GAMEINIT_MODE_CONTINUE,
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)
      };
    Field::EventGameStart::CallEventGameStart( desc );
  }
  return debug::launcher::ListData::REQID_FINISH;
}






GFL_NAMESPACE_END(MenuFrameTest)
GFL_NAMESPACE_END(Test)

#endif  // PM_DEBUG
