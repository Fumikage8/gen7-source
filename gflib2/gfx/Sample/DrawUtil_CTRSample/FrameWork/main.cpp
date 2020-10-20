#include "IApplication.h"

extern Sample::Framework::IApplication* GetApplication();
extern void TerminateApplication();

#if defined(WIN32)

#include <windows.h>

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int sw )
{	
	MSG msg;

	Sample::Framework::IApplication	*app = GetApplication();

	app->OnInit();

	while( 1 )
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
			app->OnDraw();
		}
	}

	app->OnTerm();

	TerminateApplication();

	return (int)msg.wParam;
}

#elif defined(NN_PLATFORM_CTR)

#include <nn/os.h>
#include <nn/dbg.h>
#include <nn/init.h>

#include <nn/applet.h>

void nnMain(void)
{
  // ホームメニューからも実行できるよう nn::applet::Enable() のみ呼ぶ
  // ホームメニュー遷移や電源ボタンクリック、スリープには非対応
  nn::applet::Enable();

  Sample::Framework::IApplication	*app = GetApplication();

  app->OnInit();

  while( 1 )
  {
    app->OnUpdate();
    app->OnDraw();
  }

  app->OnTerm();

  TerminateApplication();
}

#endif
