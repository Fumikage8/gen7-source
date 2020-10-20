#include "IApplication.h"

namespace Main{

  extern Framework::IApplication* CreateApplication(void);

  extern void DeleteApplication(void);

  ///extern void StartupApplication(void); @saito���͉������ĂȂ��̂ŕs�v
  ///extern void InitializeApplication(void); @saito���͉������ĂȂ��̂ŕs�v
  ///extern void TerminateApplication(void); @saito���͉������ĂȂ��̂ŕs�v
} //namespace Main

#include "MainHeap.hpp" //���C���q�[�v�̏�����/�J���R�[�h�W�J(Win32/CTR)

#if defined(GF_PLATFORM_WIN32)

#include <stdio.h>
#include <windows.h>


int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int sw )
{
  MSG msg;
  Main::Framework::IApplication	*app = Main::CreateApplication();

  //�n�[�h������
  ///Main::InitializeApplication();@saito���͉������ĂȂ��̂ŕs�v

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

  //�n�[�h�ʏI��
  ///Main::TerminateApplication();@saito���͉������ĂȂ��̂ŕs�v

  //���ʏI��
  Main::DeleteApplication();

  return (int)msg.wParam;
}

#endif //GF_PLATFORM_WIN32
