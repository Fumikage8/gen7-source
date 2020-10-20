#include "IApplication.h"

extern Sample::Framework::IApplication* GetApplication();
extern void TerminateApplication();

#if defined(WIN32)

#include <windows.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int sw )
{	
  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
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

#endif
