//===================================================================
/**
 * @file    CommonApplication.cpp
 * @brief   コモンアプリケーション
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================

/**
#include "../../System/include/MyRenderingPipeLine.h"
*///@saito
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "System/include/GflvUse.h"
///#include "System/include/PmlUse.h"@saito不要
#include "GameSystem/include/GameManager.h"
///#include "Sound/include/Sound.h"@saito不要

#include "IApplication.h"
#include "CommonApplication.hpp"
// niji
#include "GameSystem/include/GameProcManager.h"

#include "ViewerProc/include/ViewerProc.h"//@saito add
/**
#include "test/FieldTest/include/FieldTestProc.h"   // @todo タイトルが実装されるまタイトル処理が出来るまで仮対処 150207 ikuta_junya

#include <App/AppTemplate/include/AppTemplate.h>  // レイアウト描画テスト  @todo kawada_koji_20150209

// @todo LayoutのGARC読み込みテスト用	fukushima_yuya
#include <Test/LayoutArcTest/include/LayoutArcTest.h>
// @todo DrawUtilTextのテスト用 fukushima_yuya
#include <Test/DrawUtilTextTest/include/DrawUtilTextProc.h>

// @todo テスト表示コードはテストプロセスに引っ越し予定  150207 ikuta_junya
*///@saito不要
///#include <Debug/Launcher/include/LauncherProc.h>@saito不要
///#include <Debug/StartMenu/include/StartLauncher.h>@saito不要
#include "DebugWin/include/DebugWinSystem.h"

#if 1 // @todo GflUseでやるもの
#include "../Print/include/SystemFont.h"
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
  System::GflvUse::Initialize();

  // win32ではフレームレートを固定化するため、垂直同期を切る @todo 仮 150219 ikuta_junya
#if defined(GF_PLATFORM_WIN32)
#include <GL/glew.h>
  // 拡張関数を受け取る関数ポインタ
  BOOL (WINAPI *wglSwapIntervalEXT)(int) = NULL;

  if( strstr( (char*)glGetString( GL_EXTENSIONS ), "WGL_EXT_swap_control") == 0)
  {
    // WGL_EXT_swap_controlをサポートしていない
  }
  else
  {
    // APIを取得して実行
    wglSwapIntervalEXT = (BOOL (WINAPI*)(int))wglGetProcAddress("wglSwapIntervalEXT");
    if( wglSwapIntervalEXT )
    {
      wglSwapIntervalEXT( 0 );
    }
  }
#endif

  //常駐システム用heap取得
  gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
  gfl2::heap::HeapBase *deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
  //PokeLib初期化
  ///System::PmlUse::Initialize(systemHeap);@saito不要
  //ゲームマネージャー初期化
  mpGameManager = GFL_NEW(systemHeap) GameSystem::GameManager( systemHeap, deviceHeap );
  mpGameManager->Initialize();

  //デバッグウィンドウ初期化
  ///Debug::DebugWin::InitializeSystem( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ),gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEVICE ) );

  //Proc初期化
  GameSystem::GameProcManager* pProcManager = mpGameManager->GetProcManager();

#if defined(GF_PLATFORM_CTR)
    // 立体視設定
	gfl2::gfx::GFGL::SetStereoModeEnable(true);
#endif

#if 1 // @todo GflUseでやるもの
	m_pMemAllocator = GFL_NEW(systemHeap) gfl2::heap::NwAllocator( systemHeap );
	m_pDevAllocator = GFL_NEW(deviceHeap) gfl2::heap::NwAllocator( deviceHeap );


  // font
  {
// 	  m_pMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
//	  m_pDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
  //  m_pRenderingPipeLine = GFL_NEW( systemHeap ) gfl2::util::UtilTextRenderingPipeLine( 512, 256 );
  //  gfl2::util::DrawUtilText::Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
	 // gfl2::util::DrawUtilText::SetFont( print::SystemFont_GetFont() );
  }


  // LytSys初期化
  ///gfl2::lyt::LytSys::InitializeSystem( m_pMemAllocator, m_pDevAllocator );
  //フォント設定
  ///view_print::SystemFont_SetLytSystemDefaultFont();
#endif

  // @todo プロセス起動、タイトル処理が出来るまで仮対処(kujira/sangoでは言語選択イベントをコールしていた)  150207 ikuta_junya
//  NIJI_PROC_CALL< GameSys::GameProcManager, Test::FieldTest::FieldTestProc >( pProcManager );
  
  // @todo 150217 ikuta_junya 描画パイプライン移植に伴い、無効化しました
  // mpGameManager->SetLayoutRenderPath( m_pRenderingPipeLine->GetLayoutRenderPath() );


  //{  // レイアウト描画テスト  @todo kawada_koji_20150209
  //    NIJI_PROC_CALL< GameSys::GameProcManager, app::app_template::AppTemplateProc >( pProcManager );
  //}
  /**
#if PM_DEBUG
  { // ランチャー
    debug::launcher::LauncherProc::InitCallParam( &m_pLauncherParam );
    debug::launcher::LauncherProc * launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( pProcManager, &m_pLauncherParam );
    launcher->CreateStartListData<debug::startmenu::StartLauncher>();
  }
#endif  // PM_DEBUG
  *///@saito不要

  ///@saito add次のプロックを設定
  NIJI_PROC_CALL< GameSystem::GameProcManager, Field::ViewerProc >( pProcManager );

  // @todo LayoutのGARC読み込みテスト用	fukushima_yuya
  //NIJI_PROC_CALL< GameSys::GameProcManager, Test::LytArc::LayoutArcProc >( pProcManager );

  {
  // @todo DrawUtilTextのテスト用
  //    NIJI_PROC_CALL< GameSys::GameProcManager, Test::UtilText::DrawUtilTextProc >( pProcManager );
  }

	return true;
}

bool CommonApplication::OnUpdate()
{
  // @todo GameProcが存在すると、OnDraw呼ばれなくなってしまうので、一旦コメントアウトしました 150207
  // while( true )
  {
    System::GflvUse::Update();

    //if(!GFL_SINGLETON_INSTANCE(xynet::XyNetErrManager)->IsFatalError())
    {
      // Gameマネージャ更新
      if(!mpGameManager->Process())
      {
        return false;
      }
    }
    // VSync処理
    //xy_system::GflUse::VSync();
  }
  return true;
}
bool CommonApplication::OnDraw()
{
  // @note 描画パイプラインはGameManagerに移植しました 150217 ikuta_junya
  //@saito_todo描画パイプラインを作成
	return true;
}

bool CommonApplication::OnTerm()
{
#if 1 // @todo GflUseでやるもの
  ///gfl2::lyt::LytSys::FinalizeSystem();
  GFL_SAFE_DELETE( m_pDevAllocator );
  GFL_SAFE_DELETE( m_pMemAllocator );
#endif
  ///Sound::FinalizeForGameReset(mpGameManager->GetAsyncFileManager());@saito不要
  //デバッグウィンドウ終了
  ///Debug::DebugWin::FinalizeSystem();
  mpGameManager->Finalize();
  GFL_SAFE_DELETE(mpGameManager);
  ///System::PmlUse::Finalize();@saito不要
  System::GflvUse::Finalize();
  return true;
}

void CommonApplication::EndRequest()
{
	mpGameManager->EndRequest();
}

Main::Framework::IApplication* CreateApplication(void)
{
	if( g_pApp == NULL ) {
    gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );    
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
