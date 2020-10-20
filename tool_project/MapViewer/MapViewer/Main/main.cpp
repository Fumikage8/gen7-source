#include "IApplication.h"

namespace Main{

  extern Framework::IApplication* CreateApplication(void);

  extern void DeleteApplication(void);

  ///extern void StartupApplication(void); @saito今は何もしてないので不要
  ///extern void InitializeApplication(void); @saito今は何もしてないので不要
  ///extern void TerminateApplication(void); @saito今は何もしてないので不要
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
  ///Main::InitializeApplication();@saito今は何もしてないので不要

  app->OnInit();

  while( 1 )
  {
    if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) == TRUE )
    {
      if( msg.message == WM_QUIT ){
	app->EndRequest();
	break;
      }
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
    else
    {
      app->OnUpdate();
      app->OnDraw();
    }
  }

  while(1)
  {
    bool ret = app->OnUpdate();
    if(!ret) break;
  }

  app->OnTerm();

  //ハード別終了
  ///Main::TerminateApplication();@saito今は何もしてないので不要

  //共通終了
  Main::DeleteApplication();

  return (int)msg.wParam;
}

#endif //GF_PLATFORM_WIN32
