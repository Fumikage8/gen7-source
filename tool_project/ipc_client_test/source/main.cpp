//=================================================================================================
//
//  main.cpp
//
//=================================================================================================


#include "main.h"

#if 0
#define STRICT					      // �^�`�F�b�N�������ɍs�Ȃ�
#define WIN32_LEAN_AND_MEAN		// �w�b�_�[���炠�܂�g���Ȃ��֐����Ȃ�
#define WINVER        0x0600	// Windows Vista�ȍ~�Ή��A�v�����w��(�Ȃ��Ă��悢)
#define _WIN32_WINNT  0x0600	// ����
#endif

#define MAINTIMER_ID	(128)

#include "common.h"
#include "resource.h"
#include "util.h"

#include <heap/include/gfl_heap_manager.h>
#include <system/include/Timer/PerformanceCounter.h>
#include <QtWidgets/QApplication>

#include <crtdbg.h>

#include "../include/client_main.h"

#include "window.h"

#define _CRTDBG_MAP_ALLOC
// �K�v�ȃ��C�u�����������N����


//Qt5.1.1
#pragma comment( lib, "C:/Qt/Qt5.1.1/5.1.1/msvc2012_64/lib/Qt5Core.lib" )
#pragma comment( lib, "C:/Qt/Qt5.1.1/5.1.1/msvc2012_64/lib/Qt5Gui.lib" )
#pragma comment( lib, "C:/Qt/Qt5.1.1/5.1.1/msvc2012_64/lib/Qt5Widgets.lib" )
#pragma comment( lib, "C:/Qt/Qt5.1.1/5.1.1/msvc2012_64/lib/Qt5Network.lib" )

// Windows System
#pragma comment( lib, "winmm.lib" )




static void HeapInit(void * buf);
static void HeapFree(void);
//=================================================================================================
//  �\���́A��`�A�z��
//=================================================================================================
//-------------------------------------------------------------------------------------------------
// �E�C���h�E�֘A
//-------------------------------------------------------------------------------------------------
HINSTANCE	g_hInstance = NULL;	// �C���X�^���X �n���h��
HWND		  g_hWindow		= NULL;	// �E�C���h�E �n���h��

WCHAR		  g_szWndClass[]	= L"D3D11SAMPLE";
SIZE		  g_sizeWindow	= { 1024, 768 };		// �E�C���h�E�̃N���C�A���g�̈�

//-------------------------------------------------------------------------------------------------
// �V�X�e���N���X
//-------------------------------------------------------------------------------------------------

ipcnet::ClientMain* g_ClientMain;

//--------------------------------------------
//
/**

 @class HeapObject

 �ÓI�ȃN���X�̃R���X�g���N�^�Ăяo�������́A���L�̃��[���ɏ]��
 �i�P�j�����R���p�C���P�ʁi*.obj�t�@�C���j���ł́A�錾���ꂽ����
 �i�Q�j�ʂ̃R���p�C���P�ʂł́A�����J�̐U�镑���ɂ�邽�߁A����`

 new/delete���I�[�o�[���C�h����ۂɁA�R���X�g���N�^����new���g�����
 heap�V�X�e�����N�����Ă��Ȃ����ߖ�肪�N����B
 �����h�����߁A���ׂĂ̐ÓI�I�u�W�F�N�g�̒��Ő^�����HeapObject��
 �R���X�g���N�^�������悤�ɔz�u���Ă���B

*/
//--------------------------------------------
class HeapObject
{
public:
	HeapObject()
	{
		m_addr = malloc(heapdefine::HEAP_MEMORY); //1G
		HeapInit(m_addr);
	}
	~HeapObject()
	{
		HeapFree();
		free(m_addr);
		m_addr = NULL;
	}
private:
	void * m_addr;
};

HeapObject		heapSys;		// �������Ǘ��p�E�ÓI�I�u�W�F�N�g�̐擪�ɒu�����ƁI�I

MMTimerControl mmTimer;

MyWindow* mpMyWindow;


u64 before_time;
static int debugx=0;

bool          appPause;
//-------------------------------------------------------------------------------------------------
// �֐���`
//-------------------------------------------------------------------------------------------------
bool Initialize(HINSTANCE hInst);
bool Cleanup(void);
bool Main(void);



LRESULT CALLBACK  MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);

//-------------------------------------------------------------------------------------------------
// �A�v���P�[�V�����x�[�X�V�X�e��(�Ăяo���`�I��)
//-------------------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
  bool err = false;

	// �f�o�b�O �q�[�v �}�l�[�W���ɂ�郁�������蓖�Ă̒ǐՕ��@��ݒ�
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


  if(err == false){
	  // ������
	  if(Initialize(hInst) == false){
      MessageBox(g_hWindow, L"Window�̏������Ɏ��s���܂���", L"Error", MB_OK);
		  err = true;
	  }
  }

	g_ClientMain = new ipcnet::ClientMain();


	QApplication app(debugx,NULL);


	mpMyWindow = new MyWindow(g_ClientMain);

	g_ClientMain->SetOutput(mpMyWindow);
  

  appPause = false;


	MSG msg;
	msg.wParam = 0;



  if(err == false){
	  // ���b�Z�[�W ���[�v
	  do{
		  if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			  TranslateMessage(&msg);
			  DispatchMessage(&msg);
		  }
		  else {
			  // ���C������
				if(g_ClientMain){
					g_ClientMain->Update();
				}
				app.processEvents();

				if(!mpMyWindow->isVisible()){
				  break;
				}
				if(Main() == false){
					DestroyWindow(g_hWindow); // �G���[������ꍇ�C�A�v���P�[�V�����������I������
					break;
				}
		  }
	 } while (msg.message != WM_QUIT);
  }
	if(g_ClientMain){
		g_ClientMain->Finalize();
		g_ClientMain=NULL;
	}

    // �I������
	Cleanup();

	delete mpMyWindow;

	return 0;//(int)msg.wParam;
}

//-------------------------------------------------------------------------------------------------
// ������
//-------------------------------------------------------------------------------------------------
bool Initialize(HINSTANCE hInst)
{
	// �A�v���P�[�V�����̃C���X�^���X �n���h����ۑ�
	g_hInstance = hInst;
#if 0
	//@todo �E�B���h�E�̎擾�͎b��
	g_hWindow = FindWindow(L"gfl2.gfx.WinDX11", NULL );
#endif
#if 0  // USE_NEW_RENDERER	

	// �������̏�����
	//HeapInit();

	// �E�C���h�E �N���X�̓o�^
	WNDCLASS wc;
	wc.style			    = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	  = (WNDPROC)MainWndProc;
	wc.cbClsExtra		  = 0;
	wc.cbWndExtra		  = 0;
	wc.hInstance		  = hInst;
	wc.hIcon			    = LoadIcon(hInst, (LPCTSTR)IDI_ICON1);
	wc.hCursor			  = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= g_szWndClass;

  if(!RegisterClass(&wc)){ return false; }

	// ���C�� �E�C���h�E�쐬
  DWORD dwExStyle = 0;

  HWND hDeskWnd = GetDesktopWindow();
  RECT rect;
  GetWindowRect(hDeskWnd, &rect);

  //�S���̏��Ńt���n�C�r�W�����䗦�ɂȂ�悤�ɂ��� 24inch���j�^�[�Ȃ�� 120�܂ő��v

  //�t���n�C�r�W���� 1920x1080   16:9
#ifdef DEBUG_ONLY_FOR_ohno_katsumi
  DWORD dwStyle = WS_OVERLAPPEDWINDOW;
  g_sizeWindow.cx = 16*60;
  g_sizeWindow.cy = 9*60;
#elif defined(DEBUG_ONLY_FOR_tamada)
  DWORD dwStyle = WS_OVERLAPPEDWINDOW;
  g_sizeWindow.cx = 16*60;
  g_sizeWindow.cy = 9*64;
#else
  DWORD dwStyle = WS_OVERLAPPEDWINDOW;
  g_sizeWindow.cx = 16*120;
  g_sizeWindow.cy = 9*120;
#endif
  //���̐ݒ�A�S��ʗp
//  DWORD dwStyle = WS_POPUP;
//  g_sizeWindow.cx = rect.right;
//  g_sizeWindow.cy = rect.bottom;

  g_hWindow = CreateWindowEx( dwExStyle, g_szWndClass, NULL, dwStyle,
                              0, 0, g_sizeWindow.cx, g_sizeWindow.cy,
			                        NULL, NULL, hInst, NULL);
  if(g_hWindow == NULL){ return false; }

	//CoInitialize(NULL);

  
	// �E�C���h�E�\��
//	ShowWindow(g_hWindow, SW_SHOWNORMAL);
//	UpdateWindow(g_hWindow);
#endif // USE_NEW_RENDERER

	return true;
}

//-------------------------------------------------------------------------------------------------
// �I������
//-------------------------------------------------------------------------------------------------
bool Cleanup(void)
{
	// �E�C���h�E �N���X�̓o�^����
	UnregisterClass(g_szWndClass, g_hInstance);

    //HeapFree();
	return true;
}

//-------------------------------------------------------------------------------------------------
// ���C������
//-------------------------------------------------------------------------------------------------
//#include <QtWidgets/QApplication>


bool Main(void)
{
	
  
  return true;
}

//-------------------------------------------------------------------------------------------------
// �E�B���h�E����
//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;
	//BOOL fullscreen;

	switch(msg)
	{
	case WM_DESTROY:
		// �E�C���h�E�����
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;

	// �E�C���h�E �T�C�Y�̕ύX����
	case WM_SIZE:

		break;

	case WM_KEYDOWN:
		// �L�[���͂̏���
		switch(wParam)
		{
		case VK_ESCAPE:	// [ESC]�L�[�ŃE�C���h�E�����
      {
        int reply = MessageBox(g_hWindow, L"�I�����܂����H", L"Question", MB_YESNO);
        if(reply == IDYES){
			    PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
      }
			break;

    case VK_PAUSE:	// [PAUSE]�L�[�ŃA�v���ꎞ��~
      appPause = (appPause == true)? false : true;
			break;
    }
		break;

	case WM_TIMER:
#if 0
    if(appPause == false){
      if(Main() == false){
		    DestroyWindow(g_hWindow); // �G���[������ꍇ�C�A�v���P�[�V�����������I������
      }
    }
#endif
		break;
	}

	// �f�t�H���g����
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ASSERT���[�v���R�[���o�b�N����
 */
//-----------------------------------------------------------------------------
static bool AssertLoopFunc( const char* pbuff )
{
	GFL_PRINT(pbuff);
	return true;
}


void HeapInit(void * addr)
{
  static const int  HEAPID_CHILD_MAX = 128;

  //-----------------------------------------------------------------------------
  /**
   *    �q�[�v�̏�����
   */
  //-----------------------------------------------------------------------------
  gfl2::heap::Manager::Initialize( addr, heapdefine::HEAP_MEMORY, HEAPID_CHILD_MAX );

  gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  GFL_CREATE_HEAP( systemHeap, heapdefine::HEAPID_DEVICE, heapdefine::DEVICE_MEMORY, gfl2::heap::HEAP_TYPE_EXP, false );

	gfl2::debug::GFL_ASSERT_SetLoopFunc( AssertLoopFunc );

}



void HeapFree(void)
{

  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( heapdefine::HEAPID_DEVICE );
  gfl2::heap::Manager::DeleteHeap(devHeap);
  gfl2::heap::HeapBase *sysHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  gfl2::heap::Manager::DeleteHeap(sysHeap);
  
  //-----------------------------------------------------------------------------
  /**
   *    �q�[�v�̔j��
   */
  //-----------------------------------------------------------------------------
  gfl2::heap::Manager::Finalize();

}

