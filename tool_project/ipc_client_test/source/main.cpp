//=================================================================================================
//
//  main.cpp
//
//=================================================================================================


#include "main.h"

#if 0
#define STRICT					      // 型チェックを厳密に行なう
#define WIN32_LEAN_AND_MEAN		// ヘッダーからあまり使われない関数を省く
#define WINVER        0x0600	// Windows Vista以降対応アプリを指定(なくてもよい)
#define _WIN32_WINNT  0x0600	// 同上
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
// 必要なライブラリをリンクする


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
//  構造体、定義、配列
//=================================================================================================
//-------------------------------------------------------------------------------------------------
// ウインドウ関連
//-------------------------------------------------------------------------------------------------
HINSTANCE	g_hInstance = NULL;	// インスタンス ハンドル
HWND		  g_hWindow		= NULL;	// ウインドウ ハンドル

WCHAR		  g_szWndClass[]	= L"D3D11SAMPLE";
SIZE		  g_sizeWindow	= { 1024, 768 };		// ウインドウのクライアント領域

//-------------------------------------------------------------------------------------------------
// システムクラス
//-------------------------------------------------------------------------------------------------

ipcnet::ClientMain* g_ClientMain;

//--------------------------------------------
//
/**

 @class HeapObject

 静的なクラスのコンストラクタ呼び出し順序は、下記のルールに従う
 （１）同じコンパイル単位（*.objファイル）内では、宣言された順番
 （２）別のコンパイル単位では、リンカの振る舞いによるため、未定義

 new/deleteをオーバーライドする際に、コンストラクタ内でnewを使われると
 heapシステムが起動していないため問題が起きる。
 それを防ぐため、すべての静的オブジェクトの中で真っ先にHeapObjectの
 コンストラクタが動くように配置している。

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

HeapObject		heapSys;		// メモリ管理用・静的オブジェクトの先頭に置くこと！！

MMTimerControl mmTimer;

MyWindow* mpMyWindow;


u64 before_time;
static int debugx=0;

bool          appPause;
//-------------------------------------------------------------------------------------------------
// 関数定義
//-------------------------------------------------------------------------------------------------
bool Initialize(HINSTANCE hInst);
bool Cleanup(void);
bool Main(void);



LRESULT CALLBACK  MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);

//-------------------------------------------------------------------------------------------------
// アプリケーションベースシステム(呼び出し～終了)
//-------------------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
  bool err = false;

	// デバッグ ヒープ マネージャによるメモリ割り当ての追跡方法を設定
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


  if(err == false){
	  // 初期化
	  if(Initialize(hInst) == false){
      MessageBox(g_hWindow, L"Windowの初期化に失敗しました", L"Error", MB_OK);
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
	  // メッセージ ループ
	  do{
		  if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			  TranslateMessage(&msg);
			  DispatchMessage(&msg);
		  }
		  else {
			  // メイン処理
				if(g_ClientMain){
					g_ClientMain->Update();
				}
				app.processEvents();

				if(!mpMyWindow->isVisible()){
				  break;
				}
				if(Main() == false){
					DestroyWindow(g_hWindow); // エラーがある場合，アプリケーションを強制終了する
					break;
				}
		  }
	 } while (msg.message != WM_QUIT);
  }
	if(g_ClientMain){
		g_ClientMain->Finalize();
		g_ClientMain=NULL;
	}

    // 終了処理
	Cleanup();

	delete mpMyWindow;

	return 0;//(int)msg.wParam;
}

//-------------------------------------------------------------------------------------------------
// 初期化
//-------------------------------------------------------------------------------------------------
bool Initialize(HINSTANCE hInst)
{
	// アプリケーションのインスタンス ハンドルを保存
	g_hInstance = hInst;
#if 0
	//@todo ウィンドウの取得は暫定
	g_hWindow = FindWindow(L"gfl2.gfx.WinDX11", NULL );
#endif
#if 0  // USE_NEW_RENDERER	

	// メモリの初期化
	//HeapInit();

	// ウインドウ クラスの登録
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

	// メイン ウインドウ作成
  DWORD dwExStyle = 0;

  HWND hDeskWnd = GetDesktopWindow();
  RECT rect;
  GetWindowRect(hDeskWnd, &rect);

  //全員の所でフルハイビジョン比率になるようにした 24inchモニターならば 120まで大丈夫

  //フルハイビジョン 1920x1080   16:9
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
  //元の設定、全画面用
//  DWORD dwStyle = WS_POPUP;
//  g_sizeWindow.cx = rect.right;
//  g_sizeWindow.cy = rect.bottom;

  g_hWindow = CreateWindowEx( dwExStyle, g_szWndClass, NULL, dwStyle,
                              0, 0, g_sizeWindow.cx, g_sizeWindow.cy,
			                        NULL, NULL, hInst, NULL);
  if(g_hWindow == NULL){ return false; }

	//CoInitialize(NULL);

  
	// ウインドウ表示
//	ShowWindow(g_hWindow, SW_SHOWNORMAL);
//	UpdateWindow(g_hWindow);
#endif // USE_NEW_RENDERER

	return true;
}

//-------------------------------------------------------------------------------------------------
// 終了処理
//-------------------------------------------------------------------------------------------------
bool Cleanup(void)
{
	// ウインドウ クラスの登録解除
	UnregisterClass(g_szWndClass, g_hInstance);

    //HeapFree();
	return true;
}

//-------------------------------------------------------------------------------------------------
// メイン処理
//-------------------------------------------------------------------------------------------------
//#include <QtWidgets/QApplication>


bool Main(void)
{
	
  
  return true;
}

//-------------------------------------------------------------------------------------------------
// ウィンドウ処理
//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;
	//BOOL fullscreen;

	switch(msg)
	{
	case WM_DESTROY:
		// ウインドウを閉じる
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;

	// ウインドウ サイズの変更処理
	case WM_SIZE:

		break;

	case WM_KEYDOWN:
		// キー入力の処理
		switch(wParam)
		{
		case VK_ESCAPE:	// [ESC]キーでウインドウを閉じる
      {
        int reply = MessageBox(g_hWindow, L"終了しますか？", L"Question", MB_YESNO);
        if(reply == IDYES){
			    PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
      }
			break;

    case VK_PAUSE:	// [PAUSE]キーでアプリ一時停止
      appPause = (appPause == true)? false : true;
			break;
    }
		break;

	case WM_TIMER:
#if 0
    if(appPause == false){
      if(Main() == false){
		    DestroyWindow(g_hWindow); // エラーがある場合，アプリケーションを強制終了する
      }
    }
#endif
		break;
	}

	// デフォルト処理
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ASSERTループ内コールバック処理
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
   *    ヒープの初期化
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
   *    ヒープの破棄
   */
  //-----------------------------------------------------------------------------
  gfl2::heap::Manager::Finalize();

}

