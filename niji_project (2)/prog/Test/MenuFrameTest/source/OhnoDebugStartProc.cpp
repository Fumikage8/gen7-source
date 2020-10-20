//==============================================================================
/**
 @file    OhnoDebugStartProc.cpp
 @author  k.ohno
 @date    2015.12.22
 @attention  かなり強引なDLL化になっていますのでまねしないようにお願いします。
 */
//==============================================================================
#if PM_DEBUG


// nijiのインクルード
#include <System/include/HeapDefine.h>

#include <Test/MenuFrameTest/include/OhnoDebugStartProc.h>


#include <Debug/StartMenu/include/StartLauncher.h>
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

#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/DllProc.h>

#include "App/Box/include/BoxAppParam.h"

#include "niji_conv_header/poke_lib/monsno_def.h"
#include "pml/include/pml_Const.h"

#include <System/include/HeapDefine.h>

#include "App/AppEvent/include/PokeIsland/PokeIslandEvent.h"

#include "App/AppEvent/include/Congrats/CongratsEvent.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( MenuFrameTest )


OhnoDebugStartProc::OhnoDebugStartProc(void)
{
  mSeqCnt=10;
}

OhnoDebugStartProc::~OhnoDebugStartProc()
{
}

void OhnoDebugStartProc::SetNo(int no)
{
  mNo = no;
}


gfl2::proc::Result OhnoDebugStartProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
  return gfl2::proc::RES_FINISH;
}


gfl2::proc::Result OhnoDebugStartProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  gfl2::proc::Result res = gfl2::proc::RES_CONTINUE;

  switch( mNo )
  {
  case debug_menu_parts_launcher_00:

    MenuSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));
    break;

  case debug_menu_parts_launcher_01:

    Test::ThreadTest::ThreadSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));
    break;

  case debug_menu_parts_launcher_07:

    Test::FieldMessageTest::FieldMessageSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));
    break;


  case debug_menu_parts_launcher_09:

    Test::TalkWindowTest::TalkWindowSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));

    break;

    
    
  case debug_menu_parts_launcher_10:

    Test::List::ListSampleEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));
    break;

  case debug_menu_parts_launcher_13:

    App::Event::PokeIslandEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager), true);
    break;


  case debug_menu_parts_launcher_14:
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
      App::Event::BoxEvent* pEvent = App::Event::BoxEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
      pEvent->SetBoxStartType(App::Box::BOX_CALL_PC);
    }
    break;
    
  case debug_menu_parts_launcher_15:
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
			App::Event::BoxEvent* pEvent = App::Event::BoxEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
      pEvent->SetBoxStartType(App::Box::BOX_CALL_MIRACLE);
      pEvent->TradePokemon(MONSNO_HITOKAGE, pml::SEX_NUM, 50);
    }
    break;

  case debug_menu_parts_launcher_16:
    {
			GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
      App::Event::BoxEvent* pEvent = App::Event::BoxEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
      pEvent->SetBoxStartType(App::Box::BOX_CALL_BATTLE_TEAM);
    }
    break;

  case debug_menu_parts_launcher_17:
    {
			GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
      App::Event::BoxEvent* pEvent = App::Event::BoxEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
      pEvent->SetBoxStartType(App::Box::BOX_CALL_RESORT_GIM);
    }
    break;

  case debug_menu_parts_launcher_19:
    {
			GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
      App::Event::BoxEvent* pEvent = App::Event::BoxEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
      pEvent->SetBoxStartType(App::Box::BOX_CALL_FIELD_TRADE);
      pEvent->TradePokemon(101,pml::SEX_NUM,0);
    }
    break;


  case debug_menu_parts_launcher_20:

    App::Event::MovieEvent* pEvent = App::Event::MovieEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true,0 );    // movie
    App::Event::APP_PARAM x;
    x.movieNo=0;
    pEvent->SetAppParam(&x);


//    App::Event::CongratsEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager) );  //おめでとう動作OK
    
   
    break;

  }
  mNo=-1;

  GameSys::GameEventManager * p_geventman = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameEventManager();
  
  if(!p_geventman->IsExists() ){
    return gfl2::proc::RES_FINISH;
  }

  

  return res;
}

void OhnoDebugStartProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
}

void OhnoDebugStartProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
}

gfl2::proc::Result OhnoDebugStartProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
{
  return gfl2::proc::RES_FINISH;
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)

#endif  // PM_DEBUG
