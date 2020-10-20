//===================================================================
/**
 * @file    main.cpp
 * @brief   アプリケーション サンプルスケルトン
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================

#include "IApplication.h"
#include "StaticSymbolExport.h"
#include "GameSys/include/GameManager.h"

namespace Main{

  extern Framework::IApplication* CreateApplication(void);

  extern void DeleteApplication(void);

  extern void StartupApplication(void);
  extern void InitializeApplication(void);
  extern void TerminateApplication(void);
} //namespace Main

#include "MainHeap.hpp" //メインヒープの初期化/開放コード展開(Win32/CTR)

#if defined(GF_PLATFORM_WIN32)

#include <stdio.h>
#include <windows.h>


int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int sw )
{
	MSG msg;
  Main::Framework::IApplication	*app = Main::CreateApplication();

  //ハード初期化
  Main::InitializeApplication();

	app->OnInit();

	for ever
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) == TRUE )
		{
			if( msg.message == WM_QUIT ){
				break;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			app->OnUpdate();
		}

	}

#if 0 // win32 無限ループで終了してしまうため、コメントアウト（この状態だと正常終了できないが、processが残ることは回避できる）
	app->OnTerm();

  //ハード別終了
	Main::TerminateApplication();

  //共通終了
  Main::DeleteApplication();
#endif 
	return (int)msg.wParam;
}

#endif //GF_PLATFORM_WIN32


#if defined(GF_PLATFORM_CTR)

#include <nn/fnd.h>
#include <nn/os.h>
#include <nn/fs.h>
#include <nn/ro.h>
#include <nn/dbg.h>

#include "System/include/RomProtect.h"

//----------------------------------------------------------------------------
/**
 *  nninitStartUp()
 *  nnMainの前に動く
 */
//----------------------------------------------------------------------------
extern "C" void nninitStartUp(void)
{
  Main::StartupApplication();
  Main::HeapCreate();
}

//----------------------------------------------------------------------------
/**
 *  nnMain()
 */
//----------------------------------------------------------------------------
extern "C" void nnMain()
{
#if GFL_DEBUG
  // 通常newが使用された際にアサートを出す
  gfl2::heap::debug::EnableDefaultNewAssert();
#endif // #if GFL_DEBUG

  Main::Framework::IApplication	*app = Main::CreateApplication();

  //ハード初期化
  Main::InitializeApplication();

  app->OnInit();

  // 起動プロテクト
#if PM_DEBUG
  gfl2::heap::HeapBase *residentDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
  System::RomProtect* pDRP = GFL_NEW(residentDeviceHeap) System::RomProtect();
  pDRP->KeyCheckLoop(residentDeviceHeap);
  GFL_DELETE pDRP;
#endif //PM_DEBUG

  // 不揮発性メモリデバイスが劣化した際の動作や、バックグラウンドとの間でFSの競合が発生した際の挙動を、擬似的に再現させる機能を初期化
#if defined(GF_CONFIG_FINAL)
  gfl2::ui::DeviceManager*  pUiDeviceManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button*         pButton           = pUiDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  // 30フレーム前提動作になっているので2回呼んでいます
  pUiDeviceManager->Main();
  pUiDeviceManager->Main();
  if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
  {
    nn::fs::InitializeLatencyEmulation();
  }
#endif // GF_CONFIG_FINAL

  while( 1 )
  {
    app->OnUpdate();
  }
#pragma push
#pragma diag_suppress 111
  app->OnTerm();

  //ハード終了
  Main::TerminateApplication();

  Main::DeleteApplication();

  Main::HeapFree();

  StaticSymbolExport::Export( false );
#pragma pop
}


#endif //#ifdef GF_PLATFORM_CTR

