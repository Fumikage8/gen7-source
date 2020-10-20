//===================================================================
/**
 * @file    CommonApplication.cpp
 * @brief   コモンアプリケーション
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================

#include <gfx/include/gfl2_GLMemory.h>

#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#if defined(GF_PLATFORM_CTR)
#include <gfx/include/CTR/gfl2_CTRGL.h>
#endif

#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "System/include/GflUse.h"
#include "System/include/PmlUse.h"
#include <System/include/SystemEventManager.h>
#include <system/include/HomeSleepAndPowerSystem.h>
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"

#include "IApplication.h"
#include "CommonApplication.hpp"

// niji
#include "GameSys/include/GameProcManager.h"

// Debug
#include "Debug/DebugWin/include/DebugPause.h"
#include  <debug/include/gfl2_DebugLogWindow.h>
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinLocal.h>
#include "StartEvent.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/Event/EventGameStart.h"
#include <fs/include/gfl2_FsPcUtil.h>
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

namespace Main{

CommonApplication *g_pApp = NULL;


CommonApplication::CommonApplication() :
  Main::Framework::IApplication(),
  mpGameManager(NULL)
{
}

CommonApplication::~CommonApplication()
{
}

bool CommonApplication::OnInit()
{
  System::HomeSleepAndPowerSystem::Initialize();

  System::GflUse::Initialize();

  //常駐システム用heap取得
  gfl2::heap::HeapBase *deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
  //PokeLib初期化
  System::PmlUse::CreateLocalHeap( deviceHeap );
  System::PmlUse::Initialize();

  //ゲームマネージャー初期化
  mpGameManager = GFL_NEW(deviceHeap) GameSys::GameManager( deviceHeap );
  mpGameManager->Initialize();

#if PM_DEBUG
  {
    //ポーズシステム初期化
    Debug::DebugWin::PauseSystem_Initialize( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ) );

    //デバッグウィンドウ初期化
    Debug::DebugWin::InitializeSystem( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ),gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ));

    //デバッグログウィンドウ初期化
    gfl2::debug::InitDebugLogWindow( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ) , GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetDrawUtilText() , GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetRenderingPipeLine() );
    gfl2::debug::LogWindow_SetFont(print::SystemFont_GetFont());
  }
#endif

  //Proc初期化
  //GameSys::GameProcManager* pProcManager = mpGameManager->GetProcManager();

#if defined(GF_PLATFORM_CTR)
  // 立体視設定
  gfl2::gfx::GFGL::SetStereoModeEnable(false);
#endif

  bool normalStart = true;

  // Release版での自動実行
#if PM_DEBUG && (GFL_ENABLE_AUTO_TEST==1)
  {
    Field::EventGameStart::Description desc;

    desc.mode = Field::EventGameStart::GAMEINIT_MODE_DEBUG_BATTLE_INST_TEST;
    desc.gmgr = mpGameManager;
    Field::EventGameStart::CallEventGameStart( desc );

    return true; 
  }
#endif
  // Release版での自動実行オールマップジャンプ（プログラムメモリオーバー観測用）
#if PM_DEBUG && (GFL_ENABLE_AUTO_TEST==2)
  {
    Field::EventGameStart::Description desc;

    desc.mode = Field::EventGameStart::GAMEINIT_MODE_DEBUG_ALLMAPJUMP;
    desc.gmgr = mpGameManager;
    Field::EventGameStart::CallEventGameStart(desc);

    Field::Debug::DebugTypes::s_isOutputStaticModelMemoryText = true;

    return true;
  }
#endif

  // 特定のシステム環境変数(ユーザ環境変数)が設定されている環境でのみ、
  // 起動時にタイトルに遷移せず、自動で「全マップジャンプ」を実行する
#if PM_DEBUG
  {
    // CTRのHIO
    bool pcAccessEnableFlag = false;
    gfl2::fs::Result            res;
    res = gfl2::fs::PcUtil::IsPcAccessEnableFlag(&pcAccessEnableFlag);

    if(pcAccessEnableFlag)
    {
      const char* fileName = "../AutoDebugTest.txt";

      bool existFile = false;
      gfl2::fs::PcUtil::CheckFileExist(&existFile, fileName);
      if (existFile)
      {
        bool autoDebugTestResult = this->DoAutoDebugTest(mpGameManager, fileName);
        normalStart = (autoDebugTestResult == false);
      }
    }
  }
#endif // PM_DEBUG

  if (normalStart)
  {
    StartEvent::FirstEventStart(mpGameManager);
  }
  
	return true;
}

#if PM_DEBUG
bool CommonApplication::DoAutoDebugTest(GameSys::GameManager* pGameManager, const char* fileName)
{
  size_t fileSize = 0;
  char buf[64];

  gfl2::fs::PcUtil::GetFileSize(&fileSize, fileName);
  gfl2::fs::PcUtil::ReadFile(&fileSize, buf, 64, fileName);
  buf[fileSize] = '\0';

  if (gfl2::str::StrComp_J(buf, "ALL_MAP_JUMP"))
  {
    Field::EventGameStart::Description desc;

    desc.mode = Field::EventGameStart::GAMEINIT_MODE_DEBUG_ALLMAPJUMP;
    desc.gmgr = pGameManager;
    Field::EventGameStart::CallEventGameStart( desc );

    Field::Debug::DebugTypes::s_isOutputStaticModelMemoryText = true;

    return true;
  }
  else if (gfl2::str::StrComp_J(buf, "ONLY_LANDING"))
  {
    Field::EventGameStart::Description desc;

    desc.mode = Field::EventGameStart::GAMEINIT_MODE_DEBUG_FIRST;
    desc.gmgr = pGameManager;
    Field::EventGameStart::CallEventGameStart( desc );

    return true;  
  }

  return false;
}
#endif // PM_DEBUG

bool CommonApplication::OnUpdate()
{
  
  {
    System::GflUse::UpdateBeforeProc();

    //if(!GFL_SINGLETON_INSTANCE(xynet::XyNetErrManager)->IsFatalError())
    {
      // Gameマネージャ更新
      if(!mpGameManager->Process()){
        return false;
      }
    }

    System::GflUse::UpdateAfterProc();

    // VSyncWait
    mpGameManager->VSyncWait();

    //ここにスリープや電源ボタン等の処理を書く
#if defined(GF_PLATFORM_CTR)
    if( gfl2::gfx::ctr::CTRGL::IsGPURunning() == false )//GPU実行中は遷移できない
    {
#else
    {
#endif
      System::GflUse::UpdateAppletSystem();
      GFL_SINGLETON_INSTANCE(System::SystemEventManager)->CallSystemApplet();
      System::HomeSleepAndPowerSystem::Update();//この中でHome遷移、スリープ遷移、電源ボタン終了、リセットする
    }

	// RunCommandList
    mpGameManager->RunCommandList();
  }
  return true;
}

bool CommonApplication::OnTerm()
{
  Sound::FinalizeForGameReset(mpGameManager->GetAsyncFileManager());

  //デバッグウィンドウ終了
#if PM_DEBUG
  gfl2::debug::DebugLogWindow*  pLogWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugLogWindow); 
  if( pLogWin )
  {
    GFL_SAFE_DELETE( pLogWin );
  }
#endif
  Debug::DebugWin::FinalizeSystem();
  mpGameManager->Finalize();
  GFL_DELETE(mpGameManager);
  System::PmlUse::Finalize();
  System::PmlUse::DeleteLocalHeap();
  System::GflUse::Finalize();
  return true;
}


Main::Framework::IApplication* CreateApplication(void)
{
	if( g_pApp == NULL ) {
    gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );    
		g_pApp = GFL_NEW(systemHeap) CommonApplication();
	}
	return g_pApp;
}

//=======================================================
/*
 */
//=======================================================
void DeleteApplication(void)
{
	GFL_DELETE g_pApp;
}


} //namespace Main
