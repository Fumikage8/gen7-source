#include <string>
#define NOMINMAX
#include <windows.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDx11GL.h>
#include <gfx/include/WinDX11/gfl2_WinDX11ConstantBuffer.h>
#include <gfx/include/WinDX11/gfl2_WinDx11Surface.h>
#include <gfx/include/WinDX11/gfl2_WinDx11VertexShader.h>
#include <gfx/include/WinDX11/gfl2_WinDx11GeometryShader.h>
#include <gfx/include/WinDX11/gfl2_WinDx11PixelShader.h>
#include <gfx/include/WinDX11/gfl2_WinDx11VertexBuffer.h>
#include <gfx/include/WinDX11/gfl2_WinDx11IndexBuffer.h>
#include <gfx/include/WinDX11/gfl2_WinDx11StateObject.h>
#include <gfx/include/WinDX11/gfl2_WinDx11Texture.h>
#include <gfx/include/WinDX11/gfl2_WinDX11QueryObject.h>

#include <gfx/include/gfl2_GFGL.h>

#include <vector>

namespace gfl2 { namespace gfx { namespace windx11 {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  グローバル
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
static const char* s_szRegName = "gfl2.gfx.WinDX11";
static const HINSTANCE s_hInstance = GetModuleHandle( NULL );

WinDX11GL				*WinDX11GL::s_Gp = NULL;

class StringUtil{
public:
	static std::wstring ShitJIStoUTF16( const std::string& str )
	{
		int len = ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, NULL, 0 );

		if ( len > 0 ) { 
			std::vector<wchar_t> utf16( len );

			if ( ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, &utf16[0], len ) ){
				return std::wstring( &utf16[0] );
			}
		}

		return NULL;
	}
};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  ディスプレイウィンドウクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class DisplayWindow: public GLMemory
{
public:
  DisplayMode					displayMode;
  float								aspect;
  u32									width;
  u32									height;
  HWND								hWnd;
  bool								autoDestroyWindow;

	IDXGISwapChain					*m_pSwapChain;
	Surface									*m_pRenderTarget;
	Surface									*m_pDepthStencil;

	DisplayWindow( void ) : GLMemory()
	{
		m_pSwapChain = NULL;

		displayMode = DisplayMode::Default;
		aspect = 0.0f;
		width = 0;
		height = 0;
		hWnd = NULL;
		autoDestroyWindow = false;
	}

	virtual ~DisplayWindow( void )
	{
		delete m_pRenderTarget;
		delete m_pDepthStencil;

		GL_SAFE_RELEASE(m_pSwapChain);

		//  ディスプレイウィンドウを削除
		if( autoDestroyWindow == true )
			::DestroyWindow( hWnd );
	}
};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  WinDX11GLクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
LRESULT CALLBACK WinDX11GL::MainWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		PostQuitMessage( 0 );
		break;
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE ){
			SendMessage( hWnd, WM_CLOSE, 0, 0 );
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
DisplayDeviceHandle WinDX11GL::CreateDisplayEx( HWND hWnmd, Format colorFormat, Format depthStencilFormat, MultiSample multiSample )
{
	return s_Gp->createDisplay_( hWnmd, false, colorFormat, depthStencilFormat, multiSample );
}

//! @brief ディスプレイデバイスを作成する(ウィンドウスタイル指定版)
//! @return ディスプレイのデバイスはライブラリで完全にコントロールします。よってハンドルのみを返します。
DisplayDeviceHandle WinDX11GL::CreateDisplayEx(
  u32								width,							//!< ディスプレイの幅
  u32								height,							//!< ディスプレイの高さ
  b32								isFullScreen,				//!< フルスクリーンか
  Format						colorFormat,		    //!< カラーバッファのピクセルフォーマット
  Format						depthStencilFormat, //!< 深度ステンシルバッファのピクセルフォーマット
  MultiSample				multiSample,				//!< マルチサンプルモード
  DWORD             windowStyle             //!< ウィンドウスタイル
  )
{
  return s_Gp->CreateDisplay_( DisplayMode::UserSetting, width, height, isFullScreen, colorFormat, depthStencilFormat, multiSample, windowStyle );
}

//-----------------------------------------------------------------------------
/// @brief	リードファイル関数
//-----------------------------------------------------------------------------
char* WinDX11GL::ReadFile( const char* filename, s32* p_size, s32 align )
{
	FILE		*hFile;
	int			file_size;
	char		*read_buff;

	fopen_s( &hFile, filename, "rb" );

	if ( hFile == NULL ){
		assert(0);
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	*p_size = file_size;
	fseek( hFile, 0, SEEK_SET );

	file_size += align;//必ずNULLが入るようにしたい。
	u32 buf_size = file_size + ((-file_size) & (align-1));

	read_buff = reinterpret_cast<char*>( GLMemory::Malloc(buf_size) );
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );

	
	return( read_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11GL::WinDX11GL() : CommonGL(),
	m_hInst(NULL),
	m_hWnd(NULL),
	m_pD3DDevice(NULL),
	m_pD3DDeviceCtx(NULL)
{
	for( int i = 0; i < MRT_MAX; ++i )
		m_ActgiveRenderTarget[ i ] = NULL;

	m_ActgiveDepthStencil = NULL;

	//  ウィンドウクラスを登録
  if( registerWindowClass( s_szRegName ) == false )
	{
      return;
  }

	s_Gp = this;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11GL::~WinDX11GL()
{
	// デバイス・ステートのクリア
	if(m_pD3DDeviceCtx.get() )
		m_pD3DDeviceCtx->ClearState();

	m_pD3DDeviceCtx.reset();
	m_pD3DDevice.reset();

	s_Gp = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ウィンドウクラスを登録する
 */
//-----------------------------------------------------------------------------
inline bool WinDX11GL::registerWindowClass( const char* rcn )
{
	WNDCLASSEX  wc;
	
	wc.lpszClassName = rcn;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WinDX11GL::MainWndProc;
	wc.hInstance     = s_hInstance;
	wc.hIcon         = NULL;
	wc.hIconSm       = NULL;
	wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)GetStockObject( NULL_BRUSH );
	wc.lpszMenuName  = NULL;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	
	if( ::RegisterClassEx( &wc ) == 0 ){
		return false;
	}
	
	return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ウィンドウを作成する
 */
//-----------------------------------------------------------------------------
inline HWND WinDX11GL::createWindow( HWND hParent, const char* rcn, b32 fullScreen, int width, int height, DWORD windowStyle )
{
  RECT rec;
  HWND hWnd = NULL;

  if( fullScreen )
	{
      static const DWORD kStyleEx = WS_EX_APPWINDOW;

      hWnd = CreateWindowEx(
          kStyleEx, rcn, "", windowStyle,
          0, 0, width, height,
          hParent, NULL, s_hInstance, NULL
      );
  }
  else{
      static const DWORD kStyleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

      ::SetRect( &rec, 0, 0, width, height );
      if( AdjustWindowRectEx( &rec, windowStyle, FALSE, kStyleEx ) == 0 ){
          return false;
      }

		hWnd = CreateWindowEx(
			kStyleEx, rcn, "", windowStyle,
			CW_USEDEFAULT, CW_USEDEFAULT ,
			rec.right - rec.left, rec.bottom - rec.top,
			hParent, NULL, s_hInstance, NULL
		);
  }

	// ウインドウ表示
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

  return hWnd;
}

DisplayDeviceHandle WinDX11GL::createDisplay_( HWND hwnd, b32 isFullScreen, Format colorFormat, Format depthStencilFormat, MultiSample multiSample )
{
	DisplayWindow			*pDisplayWindow = NULL;
	RECT							rc;
	u32								width;
	u32								height;

	m_hWnd = hwnd;
	GetClientRect(m_hWnd, &rc);

	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	{
		D3D_FEATURE_LEVEL g_pFeatureLevels[] =  { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 }; 
		UINT              g_FeatureLevels    = 3;   // 配列の要素数
		D3D_FEATURE_LEVEL g_FeatureLevelsSupported; // デバイス作成時に返される機能レベル


		// ウインドウのクライアント エリア
		

		// デバイスとスワップ・チェインの作成
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));    // 構造体の値を初期化(必要な場合)
		sd.BufferCount       = 1;       // バック・バッファ数
		sd.BufferDesc.Width  = width;     // バック・バッファの幅
		sd.BufferDesc.Height = height;     // バック・バッファの高さ
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // フォーマット
		sd.BufferDesc.RefreshRate.Numerator = 60;  // リフレッシュ・レート(分子)
		sd.BufferDesc.RefreshRate.Denominator = 1; // リフレッシュ・レート(分母)
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バック・バッファの使用法
		sd.OutputWindow = m_hWnd;    // 関連付けるウインドウ
		sd.SampleDesc.Count = 1;        // マルチ・サンプルの数
		sd.SampleDesc.Quality = 0;      // マルチ・サンプルのクオリティ
		sd.Windowed = !isFullScreen;             // ウインドウ・モード
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // モード自動切り替え

#if defined(DEBUG_DEVICE)
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
    UINT createDeviceFlags = 0;
#endif

		pDisplayWindow = new DisplayWindow();

		// ハードウェア・デバイスを作成
		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
			g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
			&pDisplayWindow->m_pSwapChain, &m_pD3DDevice, &g_FeatureLevelsSupported,
			&m_pD3DDeviceCtx);

		/*ID3D11Debug				*pD3DDebug;
		m_pD3DDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pD3DDebug));
		//pD3DDebug->ReportLiveObjects( ReportingLevel.Detail );
		pD3DDebug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
		//Please call ReportLiveObjects()*/

		if(FAILED(hr))
		{
			assert( 0 );
			return NULL;
		}
	}

	{
		HRESULT hr;

		Surface::SurfaceDesc				desc;

		desc.usage = Usage::RenderTarget | Usage::Tiled;
		desc.pool = Pool::Default;
		desc.width = width;
		desc.height = height;
		desc.depth = 1;
		desc.format = Format::A8R8G8B8;
		desc.multiSample = MultiSample::None;

		AutoComPtr<ID3D11Texture2D>					pRenderTarget;
		AutoComPtr<ID3D11RenderTargetView>	pRenderTargetView;

    // スワップ・チェインから最初のバック・バッファを取得する
    hr = pDisplayWindow->m_pSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pRenderTarget);

    if(FAILED(hr))
			return NULL;

		// バック・バッファの情報
		D3D11_TEXTURE2D_DESC descBackBuffer;
		pRenderTarget->GetDesc(&descBackBuffer);

		// バック・バッファの描画ターゲット・ビューを作る
		hr = m_pD3DDevice->CreateRenderTargetView( pRenderTarget.get(),  NULL, &pRenderTargetView);

		Surface						*pSurface = new WinDX11Surface( desc, pRenderTarget, pRenderTargetView, AutoComPtr<ID3D11DepthStencilView>(NULL) );
		pDisplayWindow->m_pRenderTarget = pSurface;
		pDisplayWindow->m_pDepthStencil = this->CreateSurface_( width, height, Usage::DepthStencil | Usage::Tiled | Usage::ZCull, Format::D24S8, Pool::Default, MultiSample::None);
	}

	return pDisplayWindow;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
DisplayDeviceHandle WinDX11GL::CreateDisplay_( DisplayMode displayMode, u32 width, u32 height, b32 isFullScreen, Format colorFormat, Format depthStencilFormat, MultiSample multiSample, DWORD windowStyle )
{
  m_hWnd = createWindow( NULL, s_szRegName, isFullScreen, width, height, windowStyle );

	return createDisplay_( m_hWnd, isFullScreen, colorFormat, depthStencilFormat, multiSample );
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11GL::DestroyDisplay_( DisplayDeviceHandle hDisplay )
{
	DisplayWindow* const pDisplay = reinterpret_cast< DisplayWindow* >( hDisplay );
	if ( pDisplay == NULL )
		return;
	delete pDisplay;
}

//----------------------------------------------------------------------------
/**
 *	@brief  バックバッファを取得する
 */
//-----------------------------------------------------------------------------
Surface* WinDX11GL::GetBackBuffer_( DisplayDeviceHandle hDisplay )
{
	DisplayWindow* const pDisplay = reinterpret_cast< DisplayWindow* >( hDisplay );
	if ( pDisplay == NULL )
		return NULL;

	return pDisplay->m_pRenderTarget;
}

//----------------------------------------------------------------------------
/**
 *	@brief  深度ステンシルバッファを取得する
 */
//-----------------------------------------------------------------------------
Surface* WinDX11GL::GetDepthStencilBuffer_( DisplayDeviceHandle hDisplay )
{
	DisplayWindow* const pDisplay = reinterpret_cast< DisplayWindow* >( hDisplay );
	if ( pDisplay == NULL )
		return NULL;

	return pDisplay->m_pDepthStencil;
}

//----------------------------------------------------------------------------
/**
 *	@brief  サーフェイスを作成する
 */
//-----------------------------------------------------------------------------
Surface* WinDX11GL::CreateSurface_( const Surface::SurfaceDesc &rDesc )
{
	Surface									*pSurface = NULL;
	pSurface = new WinDX11Surface( rDesc );
	return pSurface;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11GL::TransferDisplay_()
{
  // 空実装
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11GL::SwapDisplay_( b32 fVsyncWait )
{
	assert( m_pActiveDisplay );

	DisplayWindow* const pDisplay = reinterpret_cast< DisplayWindow* >( m_pActiveDisplay );

	// バック・バッファの表示
	pDisplay->m_pSwapChain->Present( fVsyncWait ? 1 : 0, 0);
}

//----------------------------------------------------------------------------
/**
 *   @brief レンダリングターゲットを切り替える
//! @par index でＭＲＴカラーバッファのインデクスを指定
*/
//-----------------------------------------------------------------------------
b32 WinDX11GL::SetRenderTarget_(u32 index, const Surface* const pSurface)
{
	if ( index >= MRT_MAX )
		return false;

	m_ActgiveRenderTarget[index] = down_cast<const WinDX11Surface*>( pSurface );

	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 現在のレンダリングターゲットを取得する
*/
//-----------------------------------------------------------------------------
const Surface* WinDX11GL::GetRenderTarget_(u32 index)
{
	if ( index >= MRT_MAX )
		return NULL;
	return m_ActgiveRenderTarget[index];
}

//----------------------------------------------------------------------------
/**
 *   @brief 深度ステンシルを切り替える
*/
//-----------------------------------------------------------------------------
b32 WinDX11GL::SetDepthStencil_(const Surface* const pSurface)
{
	m_ActgiveDepthStencil = down_cast<const WinDX11Surface*>( pSurface );
	assert( m_ActgiveDepthStencil );
	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 現在の深度ステンシルを取得する
*/
//-----------------------------------------------------------------------------
const Surface* WinDX11GL::GetDepthStencil_()
{
	return m_ActgiveDepthStencil;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
b32 WinDX11GL::BeginTimerQuery_( const TimerQuery* pTimerQuery )
{
	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void WinDX11GL::EndTimerQuery_( const TimerQuery* pTimerQuery)
{
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
b32 WinDX11GL::BeginOcclusionQuery_( const OcclusionQuery* pOcclusionQuery)
{
	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void WinDX11GL::EndOcclusionQuery_( const OcclusionQuery* pOcclusionQuery)
{
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
b32 WinDX11GL::BeginConditionalRender_( const OcclusionQuery *pQuery )
{
  return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void WinDX11GL::EndConditionalRender_( const OcclusionQuery *pQuery )
{
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
b32 WinDX11GL::BeginScene_()
{
	ID3D11RenderTargetView		*targets[MRT_MAX];
	s32												targetNum = 0;

	for( s32 i = 0; i < MRT_MAX; ++i )
	{
		if ( m_ActgiveRenderTarget[i] == NULL )
			continue;

		targets[i] = const_cast<WinDX11Surface*>(m_ActgiveRenderTarget[i])->GetRenderTargetView().get();
		if ( targets[i] )
			targetNum = i + 1;
	}
	
	//将来のDirectXでは、RTと深度バッファを１対１で結び付けられるようになるだろう。
	m_pD3DDeviceCtx->OMSetRenderTargets( targetNum, targets, const_cast<WinDX11Surface*>(m_ActgiveDepthStencil)->GetDepthStencilView().get() );

	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void WinDX11GL::EndScene_()
{
	//Windows DirectXはリングバッファ形式。キックコマンドなどはない。
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void WinDX11GL::Finish_()
{
	AutoComPtr<ID3D11Query>				pEvent;
	D3D11_QUERY_DESC	desc;

	desc.MiscFlags = 0;
	desc.Query = D3D11_QUERY_EVENT;

	m_pD3DDevice->CreateQuery( &desc, &pEvent );

	while( S_FALSE == m_pD3DDeviceCtx->GetData( pEvent.get(), NULL, 0, D3D11_ASYNC_GETDATA_DONOTFLUSH  ) )
	{
		::Sleep(0);
	}
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void WinDX11GL::ClearRenderTarget_( const Surface* pSurface, const Color &rClearColor )
{
	const WinDX11Surface				*pWinDX11Surface = down_cast<const WinDX11Surface*>( pSurface );
	assert( pWinDX11Surface );

	float clearColor[4] = { rClearColor.x, rClearColor.y, rClearColor.z, rClearColor.w }; // RGBA
	m_pD3DDeviceCtx->ClearRenderTargetView( const_cast<WinDX11Surface*>(pWinDX11Surface)->GetRenderTargetView().get(), clearColor );
}

//----------------------------------------------------------------------------
/**
 *   @brief 深度ステンシルをクリアする
*/
//-----------------------------------------------------------------------------
void WinDX11GL::ClearDepthStencil_( const Surface* pSurface, f32 depth, u8 stencil )
{
	const WinDX11Surface				*pWinDX11Surface = down_cast<const WinDX11Surface*>( pSurface );
	assert( pWinDX11Surface );

	m_pD3DDeviceCtx->ClearDepthStencilView( const_cast<WinDX11Surface*>(pWinDX11Surface)->GetDepthStencilView().get(), D3D11_CLEAR_DEPTH, depth, stencil );
}

//----------------------------------------------------------------------------
/**
 *   @brief 深度ステンシルをクリアする
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetViewport_(const u32 xp, const u32 yp, const u32 width, const u32 height)
{
	D3D11_VIEWPORT          viewPort[1];

	// ビューポートの設定
  viewPort[0].TopLeftX = static_cast<f32>(xp);    // ビューポート領域の左上X座標。
  viewPort[0].TopLeftY = static_cast<f32>(yp);    // ビューポート領域の左上Y座標。
  viewPort[0].Width    = static_cast<f32>(width);  // ビューポート領域の幅
	viewPort[0].Height   = static_cast<f32>(height);  // ビューポート領域の高さ
  viewPort[0].MinDepth = 0.0f; // ビューポート領域の深度値の最小値
  viewPort[0].MaxDepth = 1.0f; // ビューポート領域の深度値の最大値

	m_pD3DDeviceCtx->RSSetViewports(1, viewPort);					// RSにビューポートを設定
}

//! @brief 頂点シェーダーを切り替える
void WinDX11GL::SetVertexShader_( const Shader* pShader )
{
	const WinDX11VertexShader		*pDXShader = down_cast<const WinDX11VertexShader*>(pShader);
	pDXShader->Bind();
}

//! @brief ジオメトリシェーダーを切り替える
void WinDX11GL::SetGeometryShader_( const Shader* pShader )
{
	if ( pShader )
	{
		const WinDX11GeometryShader		*pDXShader = down_cast<const WinDX11GeometryShader*>(pShader);
		pDXShader->Bind();
	}
	else
	{
		GFGL::GetDeviceContext()->GSSetShader( NULL, NULL, 0);
	}
}

//! @brief ピクセルシェーダーを切り替える
void WinDX11GL::SetPixelShader_( const Shader* pShader )
{
	const WinDX11PixelShader		*pDXShader = down_cast<const WinDX11PixelShader*>(pShader);
	pDXShader->Bind();
}

//----------------------------------------------------------------------------
/**
 *   @brief 頂点シェーダ定数設定
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetVertexShaderConstant_( u32 registerNo, ConstantBuffer *pBuffer )
{
	WinDX11ConstantBuffer				*pWinDX11ConstantBuffer = down_cast<WinDX11ConstantBuffer*>( pBuffer );
	assert( pWinDX11ConstantBuffer );

	m_pD3DDeviceCtx->VSSetConstantBuffers(registerNo, 1, &pWinDX11ConstantBuffer->m_pConstantBuffer );
}

//----------------------------------------------------------------------------
/**
 *   @brief ジオメトリシェーダ定数設定
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetGeometryShaderConstant_( u32 registerNo,	ConstantBuffer *pBuffer	)
{
	WinDX11ConstantBuffer				*pWinDX11ConstantBuffer = down_cast<WinDX11ConstantBuffer*>( pBuffer );
	assert( pWinDX11ConstantBuffer );

	m_pD3DDeviceCtx->GSSetConstantBuffers(registerNo, 1, &pWinDX11ConstantBuffer->m_pConstantBuffer );
}

//----------------------------------------------------------------------------
/**
 *   @brief ピクセルシェーダ定数設定
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetPixelShaderConstant_( u32 registerNo,	ConstantBuffer *pBuffer	)
{
	WinDX11ConstantBuffer				*pWinDX11ConstantBuffer = down_cast<WinDX11ConstantBuffer*>( pBuffer );
	assert( pWinDX11ConstantBuffer );

	m_pD3DDeviceCtx->PSSetConstantBuffers(registerNo, 1, &pWinDX11ConstantBuffer->m_pConstantBuffer );
}

//----------------------------------------------------------------------------
/**
 *   @brief インデクスバッファを設定します
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetIndices_(const IndexBuffer* pIndices)
{
	WinDX11IndexBuffer				*pWinDX11IndexBuffer = const_cast<WinDX11IndexBuffer*>( down_cast<const WinDX11IndexBuffer*>( pIndices ) );
	assert( pWinDX11IndexBuffer );

	m_pD3DDeviceCtx->IASetIndexBuffer( pWinDX11IndexBuffer->m_pIndexBuffer.get(),
																		(pWinDX11IndexBuffer->m_IndexType == IndexType::Index16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,
																		0);
}

//----------------------------------------------------------------------------
/**
 *   @brief 頂点バッファをストリームに設定
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetStreamSource_( const u32 index, const VertexBuffer* pVertexBuffer, const s32 offset, const s32 stride )
{
	WinDX11VertexBuffer				*pWinDX11VertexBuffer = const_cast<WinDX11VertexBuffer*>( down_cast<const WinDX11VertexBuffer*>( pVertexBuffer ) );
	assert( pWinDX11VertexBuffer );//DX11ではNULLは渡せない。

	u32 offset_ = 0;
	u32 stride_ = pWinDX11VertexBuffer->m_BufferDesc.ByteWidth / pWinDX11VertexBuffer->m_VertexNum;

	if ( offset != -1 )
		offset_ = static_cast<u32>( offset );

	if ( stride != -1 )
		stride_ = static_cast<u32>( stride );

	m_pD3DDeviceCtx->IASetVertexBuffers(index, 1, &pWinDX11VertexBuffer->m_pVerBuffer, &stride_, &offset_ );			// IAに入力レイアウト・オブジェクトを設定
}

//----------------------------------------------------------------------------
/**
 *   @brief 頂点ストリームにセットされた頂点を使って描画
*/
//-----------------------------------------------------------------------------
void WinDX11GL::DrawPrimitive_( PrimitiveType primitive, u32 start, u32 count	)
{
	m_pD3DDeviceCtx->IASetPrimitiveTopology( s_d3d11PrimitiveType[ primitive ] );
	m_pD3DDeviceCtx->Draw(count, start);
}

//----------------------------------------------------------------------------
/**
 *   @brief 頂点ストリームにセットされた頂点とインデックスバッファを使って描画
*/
//-----------------------------------------------------------------------------
void WinDX11GL::DrawIndexedPrimitive_( PrimitiveType primitive, u32	start, u32	count )
{
	m_pD3DDeviceCtx->IASetPrimitiveTopology( s_d3d11PrimitiveType[ primitive ] );
	m_pD3DDeviceCtx->DrawIndexed( count, start, 0);
}

//----------------------------------------------------------------------------
/**
 *   @brief ブレンドステートの設定
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetBlendState_( BlendStateObject *pState )
{
	WinDX11BlendStateObject				*pWinDX11BlendStateObject = down_cast<WinDX11BlendStateObject*>( pState );
	assert( pWinDX11BlendStateObject );

	FLOAT BlendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_pD3DDeviceCtx->OMSetBlendState( pWinDX11BlendStateObject->m_pBlendState.get(), BlendFactor, 0xffffffff);		// OMにブレンド・ステート・オブジェクトを設定
}

//----------------------------------------------------------------------------
/**
 *   @brief ラスタライザステートの設定
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetRasterizerState_( RasterizerStateObject *pState )
{
	WinDX11RasterizerStateObject				*pWinDX11RasterizerStateObject = down_cast<WinDX11RasterizerStateObject*>( pState );
	assert( pWinDX11RasterizerStateObject );

	m_pD3DDeviceCtx->RSSetState( pWinDX11RasterizerStateObject->m_pRasterizerState.get() );// RSにラスタライザ・ステート・オブジェクトを設定

	if ( pWinDX11RasterizerStateObject->m_StateData.m_ScissorTestEnable )
	{
		D3D11_RECT						rects[1];
		const math::Vector		&rRect = pWinDX11RasterizerStateObject->m_StateData.m_ScissorRect;
		// x -> x, y -> y, z -> width, w -> heightとなっているので,
		// 整合性を保つべくここで補正をかける.
		rects[0].left		= static_cast<LONG>(rRect.x);
		rects[0].top		= static_cast<LONG>(rRect.y);
		rects[0].right	= static_cast<LONG>(rRect.z + rRect.x);
		rects[0].bottom	= static_cast<LONG>(rRect.w + rRect.y);

		m_pD3DDeviceCtx->RSSetScissorRects( 1, rects );
	}
}

//----------------------------------------------------------------------------
/**
 *   @brief 深度ステンシルステートの設定
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetDepthStencilState_( DepthStencilStateObject *pState )
{
	WinDX11DepthStencilStateObject				*pWinDX11DepthStencilStateObject = down_cast<WinDX11DepthStencilStateObject*>( pState );
	assert( pWinDX11DepthStencilStateObject );

	// OMに深度/ステンシル・ステート・オブジェクトを設定
	m_pD3DDeviceCtx->OMSetDepthStencilState( pWinDX11DepthStencilStateObject->m_pDepthStencilState.get(),
																					 pWinDX11DepthStencilStateObject->m_StateData.m_StencilRef & pWinDX11DepthStencilStateObject->m_StateData.m_StencilMask );
}

//----------------------------------------------------------------------------
/**
 *   @brief サンプラステートの設定
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetSamplerState_( u32 texNo, const SamplerStateObject *pState )
{
	const WinDX11SamplerStateObject	*pConstWinDX11SamplerStateObject = down_cast<const WinDX11SamplerStateObject*>( pState );
	assert( pConstWinDX11SamplerStateObject );

	WinDX11SamplerStateObject				*pWinDX11SamplerStateObject = const_cast<WinDX11SamplerStateObject*>( pConstWinDX11SamplerStateObject );//外さないと渡せない。
	m_pD3DDeviceCtx->PSSetSamplers( texNo, 1, &pWinDX11SamplerStateObject->m_pTextureSampler );
}

//----------------------------------------------------------------------------
/**
 *   @brief テクスチャの設定
*/
//-----------------------------------------------------------------------------
void WinDX11GL::SetTexture_( u32 texNo, const Texture *pTexture )
{
	WinDX11Texture				*pWinDX11Texture = const_cast<WinDX11Texture*>( down_cast<const WinDX11Texture*>( pTexture ) );

	if ( pWinDX11Texture )
	{
		m_pD3DDeviceCtx->PSSetShaderResources( texNo, 1, &pWinDX11Texture->m_pShaderResourceView );
	}
	else
	{
		ID3D11ShaderResourceView *const pSRV[1] = {NULL};
		m_pD3DDeviceCtx->PSSetShaderResources( texNo, 1, pSRV );
	}
	
}

//----------------------------------------------------------------------------
/**
 *   @brief ファイルから頂点シェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* WinDX11GL::CreateVertexShaderFromFile(const char* path, const char* entry, const VertexElement* pVertexElements, const char** args, FileFormat format)
{
	Shader		*pShader;

	if ( format == FileFormat::ShaderCode )
	{
		HRESULT					hr;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG_DEVICE )
		dwShaderFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL0;
#endif

		AutoComPtr<ID3DBlob>		pBlobVS;
		AutoComPtr<ID3DBlob>		pErrorBlob;

		D3D_SHADER_MACRO  deflist[]= {
			{  "HLSL",  "1", },
			{  NULL, NULL },
		};

		std::string		pathStr = path;
		std::wstring	pathWStr = StringUtil::ShitJIStoUTF16( pathStr );

		//VisualStudio2012でシェーダーコードをトレース実行したい場合はD3DX11CompileFromFileを用いないとダメぽい。
		hr = D3DCompileFromFile(
				pathWStr.c_str(),					 // ファイル名
				deflist,          // マクロ定義
				NULL,          // インクルード・ファイル定義(なし)
				entry,         // 「VS関数」がシェーダから実行される
				"vs_4_0",      // 頂点シェーダ
				dwShaderFlags, // コンパイル・オプション
				0,             // エフェクトのコンパイル・オプション(なし)
				&pBlobVS,      // コンパイルされたバイト・コード
				&pErrorBlob );// エラーメッセージは不要

		if( FAILED(hr) )
		{
			if( pErrorBlob.get() != NULL )
			{
				std::string			err = (char*)pErrorBlob->GetBufferPointer();
				assert(0);
			}
		}

		pShader = s_Gp->CreateVertexShaderFromMemory_( pBlobVS->GetBufferPointer(), static_cast<u32>(pBlobVS->GetBufferSize()), pVertexElements);
	}
	else
	{
		s32			size;
		char		*read_buff;

		read_buff = ReadFile( path, &size );
		pShader = s_Gp->CreateVertexShaderFromMemory_( read_buff, size, pVertexElements);

		GLMemory::Free( read_buff );
	}

	return pShader;
}

//----------------------------------------------------------------------------
/**
 *   @brief ソースコードから頂点シェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* WinDX11GL::CreateVertexShaderFromSource_(const char* code, const char* entry, const VertexElement* pVertexElements, const char** args)
{
	HRESULT					hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG_DEVICE )
  dwShaderFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL0;
#endif

	AutoComPtr<ID3DBlob>						pBlobVS;
	AutoComPtr<ID3DBlob>						pErrorBlob;

	D3D_SHADER_MACRO  deflist[]= {
		{  "HLSL",  "1", },
		{  NULL, NULL },
	};

	hr = D3DCompile( code, strlen( code ), NULL, deflist, NULL, entry, "vs_4_0", dwShaderFlags, 0, &pBlobVS, &pErrorBlob );

	if( FAILED(hr) )
	{
		if( pErrorBlob.get() != NULL )
		{
			std::string			err = (char*)pErrorBlob->GetBufferPointer();
			assert(0);
		}
	}

	Shader			*pShader = CreateVertexShaderFromMemory_( pBlobVS->GetBufferPointer(), static_cast<u32>(pBlobVS->GetBufferSize()), pVertexElements );

	return pShader;
}

//----------------------------------------------------------------------------
/**
 *   @brief バイナリデータから頂点シェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* WinDX11GL::CreateVertexShaderFromMemory_(const void* code, u32 size, const VertexElement* pVertexElements)
{
	Shader					*pShader = NULL;
	pShader = new WinDX11VertexShader( code, size, pVertexElements );
	return pShader;
}

//----------------------------------------------------------------------------
/**
 *   @brief ファイルからジオメトリシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* WinDX11GL::CreateGeometryShaderFromFile(const char* path, const char* entry, const char** args, FileFormat format)
{
	Shader		*pShader;

	if ( format == FileFormat::ShaderCode )
	{
		HRESULT					hr;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG_DEVICE )
		dwShaderFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL0;;
#endif

		AutoComPtr<ID3DBlob>		pBlobGS;
		AutoComPtr<ID3DBlob>		pErrorBlob;

		D3D_SHADER_MACRO  deflist[]= {
			{  "HLSL",  "1", },
			{  NULL, NULL },
		};

		std::string		pathStr = path;
		std::wstring	pathWStr = StringUtil::ShitJIStoUTF16( pathStr );

		//VisualStudio2012でシェーダーコードをトレース実行したい場合はD3DX11CompileFromFileを用いないとダメぽい。
		hr = D3DCompileFromFile(
				pathWStr.c_str(),					 // ファイル名
				deflist,          // マクロ定義
				NULL,          // インクルード・ファイル定義(なし)
				entry,         // 「VS関数」がシェーダから実行される
				"gs_4_0",      // 頂点シェーダ
				dwShaderFlags, // コンパイル・オプション
				0,             // エフェクトのコンパイル・オプション(なし)
				&pBlobGS,      // コンパイルされたバイト・コード
				&pErrorBlob  );// エラーメッセージは不要

		if( FAILED(hr) )
		{
			if( pErrorBlob.get() != NULL )
			{
				std::string			err = (char*)pErrorBlob->GetBufferPointer();
				assert(0);
			}
		}

		pShader = s_Gp->CreateGeometryShaderFromMemory_( pBlobGS->GetBufferPointer(), static_cast<u32>(pBlobGS->GetBufferSize()));
	}
	else
	{
		s32			size;
		char		*read_buff;

		read_buff = ReadFile( path, &size );
		pShader = s_Gp->CreateGeometryShaderFromMemory_( read_buff, size );

		GLMemory::Free( read_buff );
	}

	return pShader;
}

//----------------------------------------------------------------------------
/**
 *   @brief ソースコードからジオメトリシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* WinDX11GL::CreateGeometryShaderFromSource_(const char* code, const char* entry, const char** args)
{
	HRESULT					hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG_DEVICE )
  dwShaderFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL0;
#endif

	AutoComPtr<ID3DBlob>						pBlobPS;
	AutoComPtr<ID3DBlob>						pErrorBlob;

	D3D_SHADER_MACRO  deflist[]= {
			{  "HLSL",  "1", },
			{  NULL, NULL },
		};

	hr = D3DCompile( code, strlen( code ), NULL, deflist, NULL, entry, "gs_4_0", dwShaderFlags, 0, &pBlobPS, &pErrorBlob );

	if( FAILED(hr) )
	{
		if( pErrorBlob.get() != NULL )
		{
			std::string			err = (char*)pErrorBlob->GetBufferPointer();
			assert(0);
		}
	}

	Shader*				pShader = CreateGeometryShaderFromMemory_(pBlobPS->GetBufferPointer(), static_cast<u32>(pBlobPS->GetBufferSize()));

	return pShader;
}

//----------------------------------------------------------------------------
/**
 *   @brief バイナリデータからジオメトリシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* WinDX11GL::CreateGeometryShaderFromMemory_(const void* code, u32 size)
{
	Shader							*pShader = NULL;
	pShader = new WinDX11GeometryShader( code, size );
	return pShader;
}

//----------------------------------------------------------------------------
/**
 *   @brief ファイルからピクセルシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* WinDX11GL::CreatePixelShaderFromFile(const char* path, const char* entry, const char** args, FileFormat format)
{
	Shader*		pShader;

	if ( format == FileFormat::ShaderCode )
	{
		HRESULT					hr;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG_DEVICE )
		dwShaderFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL0;;
#endif

		AutoComPtr<ID3DBlob>		pBlobPS;
		AutoComPtr<ID3DBlob>		pErrorBlob;

		D3D_SHADER_MACRO  deflist[]= {
			{  "HLSL",  "1", },
			{  NULL, NULL },
		};

		std::string		pathStr = path;
		std::wstring	pathWStr = StringUtil::ShitJIStoUTF16( pathStr );

		//VisualStudio2012でシェーダーコードをトレース実行したい場合はD3DX11CompileFromFileを用いないとダメぽい。
		hr = D3DCompileFromFile(
				pathWStr.c_str(),					 // ファイル名
				deflist,          // マクロ定義
				NULL,          // インクルード・ファイル定義(なし)
				entry,         // 「PS関数」がシェーダから実行される
				"ps_4_0",      // ピクセルシェーダ
				dwShaderFlags, // コンパイル・オプション
				0,             // エフェクトのコンパイル・オプション(なし)
				&pBlobPS,      // コンパイルされたバイト・コード
				&pErrorBlob);  // エラーメッセージは不要

		if( FAILED(hr) )
		{
			if( pErrorBlob.get() != NULL )
			{
				std::string			err = (char*)pErrorBlob->GetBufferPointer();
				assert(0);
			}
		}

		pShader = s_Gp->CreatePixelShaderFromMemory_( pBlobPS->GetBufferPointer(), static_cast<u32>(pBlobPS->GetBufferSize()));
	}
	else
	{
		s32			size;
		char		*read_buff;

		read_buff = ReadFile( path, &size );
		pShader = s_Gp->CreatePixelShaderFromMemory_( read_buff, size );

		GLMemory::Free( read_buff );
	}

	return pShader;
}
		
//----------------------------------------------------------------------------
/**
 *   @brief ソースコードからピクセルシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* WinDX11GL::CreatePixelShaderFromSource_(const char* code, const char* entry, const char** args)
{
	HRESULT					hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG_DEVICE )
  dwShaderFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL0;
#endif

	AutoComPtr<ID3DBlob>						pBlobPS;
	AutoComPtr<ID3DBlob>						pErrorBlob;

	D3D_SHADER_MACRO  deflist[]= {
			{  "HLSL",  "1", },
			{  NULL, NULL },
		};

	hr = D3DCompile( code, strlen( code ), NULL, deflist, NULL, entry, "ps_4_0", dwShaderFlags, 0, &pBlobPS, &pErrorBlob );

	if( FAILED(hr) )
	{
		if( pErrorBlob.get() != NULL )
		{
			std::string			err = (char*)pErrorBlob->GetBufferPointer();
			assert(0);
		}
	}

	Shader				*pShader = CreatePixelShaderFromMemory_(pBlobPS->GetBufferPointer(), static_cast<u32>(pBlobPS->GetBufferSize()));

	return pShader;
}

//----------------------------------------------------------------------------
/**
 *   @brief バイナリデータからピクセルシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* WinDX11GL::CreatePixelShaderFromMemory_(const void* code, u32 size)
{
	Shader							*pShader = NULL;
	pShader = new WinDX11PixelShader( code, size );
	return pShader;
}

//----------------------------------------------------------------------------
/**
 *			@brief 定数バッファを作成
 */
//-----------------------------------------------------------------------------
ConstantBuffer* WinDX11GL::CreateConstantBuffer_( const ConstantBufferElement* pConstantBufferElements, u32 usage, Pool pool, const void* buffer )
{
	ConstantBuffer							*pConstantBuffer = NULL;
	pConstantBuffer = new WinDX11ConstantBuffer( pConstantBufferElements, usage, pool, buffer );
	return pConstantBuffer;
}

//----------------------------------------------------------------------------
/**
 *   @brief バイナリデータから頂点シェーダーを作成
*/
//-----------------------------------------------------------------------------
VertexBuffer* WinDX11GL::CreateVertexBuffer_( u32 size, u32 vertex_num, u32 usage, Pool pool, const void* buffer )
{
	VertexBuffer							*pVertexBuffer = NULL;
	pVertexBuffer = new WinDX11VertexBuffer( size, vertex_num, usage, pool, buffer );
	return pVertexBuffer;
}

//! @brief インデックスバッファを作成
IndexBuffer* WinDX11GL::CreateIndexBuffer_( u32 size, u32 usage, Pool pool, IndexType type, const void* buffer )
{
	IndexBuffer							*pIndexBuffer = NULL;
	pIndexBuffer = new WinDX11IndexBuffer( size, usage, pool, type, buffer );
	return pIndexBuffer;
}

//----------------------------------------------------------------------------
/**
 *   @brief ブレンドステートオブジェクトを作成
*/
//-----------------------------------------------------------------------------
BlendStateObject* WinDX11GL::CreateBlendStateObject_()
{
	BlendStateObject							*pStateObj = NULL;
	pStateObj = new WinDX11BlendStateObject();
	return pStateObj;
}

//----------------------------------------------------------------------------
/**
 *   @brief ラスタライザステートオブジェクトを作成
*/
//-----------------------------------------------------------------------------
RasterizerStateObject* WinDX11GL::CreateRasterizerStateObject_()
{
	RasterizerStateObject							*pStateObj = NULL;
	pStateObj = new WinDX11RasterizerStateObject();
	return pStateObj;
}

//----------------------------------------------------------------------------
/**
 *   @brief 深度ステンシルステートオブジェクトを作成
*/
//-----------------------------------------------------------------------------
DepthStencilStateObject* WinDX11GL::CreateDepthStencilStateObject_()
{
	DepthStencilStateObject							*pStateObj = NULL;
	pStateObj = new WinDX11DepthStencilStateObject();
	return pStateObj;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
TimerQuery* WinDX11GL::CreateTimerQuery_()
{
  WinDX11TimerQuery* pTimerQuery = new WinDX11TimerQuery();
  return pTimerQuery;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
OcclusionQuery* WinDX11GL::CreateOcclusionQuery_()
{
  // 対応しない
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *   @brief ディスプレイリストの作成
*/
//-----------------------------------------------------------------------------
DisplayList* WinDX11GL::CreateDisplayList_()
{
  WinDX11DisplayList* pDisplayList = new WinDX11DisplayList();
  return pDisplayList;
}

//----------------------------------------------------------------------------
/**
 *   @brief ディスプレイリストの呼び出し
*/
//-----------------------------------------------------------------------------
void WinDX11GL::CallDisplayList_(DisplayList* pDisplayList)
{
  WinDX11DisplayList* pWinDisplayList = static_cast<WinDX11DisplayList*>(pDisplayList);
  pWinDisplayList->Call();
}

//----------------------------------------------------------------------------
/**
 *   @brief サンプラステートオブジェクトを作成
*/
//-----------------------------------------------------------------------------
SamplerStateObject* WinDX11GL::CreateSamplerStateObject_()
{
	SamplerStateObject							*pStateObj = NULL;
	pStateObj = new WinDX11SamplerStateObject();
	return pStateObj;
}

//----------------------------------------------------------------------------
/**
 *   @brief ２Ｄテクスチャを作成する
//! @param usage オプション
//! @param mipLevels ミップマップレベル数指定( 1 - 13 )
*/
//-----------------------------------------------------------------------------
Texture* WinDX11GL::CreateTexture_(u32 width, u32 height, u32 mipLevels, u32 usage, Format format, Pool pool)
{
	Texture::TextureDesc				desc;

	desc.type = TextureType::Image;
	desc.usage = usage;
	desc.pool = pool;
	desc.width = width;
	desc.height = height;
	desc.depth = 1;
	desc.mipLevels = mipLevels;
	desc.format = format;
	desc.multiSample = MultiSample::None;

	Texture											*pTexture = NULL;
	pTexture = new WinDX11Texture( desc );
	return pTexture;
}

//----------------------------------------------------------------------------
/**
 *   @brief イメージデータから２Ｄテクスチャを作成する
*/
//-----------------------------------------------------------------------------
Texture* WinDX11GL::CreateTextureFromImage_( const ImageData &rImage, Pool pool)
{
	Texture::TextureDesc				desc;

	desc.type = TextureType::Image;

	if ( rImage.IsCubemap() ) 
	{
		desc.type = TextureType::Cube;
		desc.mipLevels = rImage.GetDepth();
		desc.depth = 1;
	}
	else if ( rImage.IsVolume() ) 
	{
		desc.type = TextureType::Volume;
		desc.mipLevels = 1;
		desc.depth = rImage.GetDepth();
	}
	else
	{
		desc.type = TextureType::Image;
		desc.mipLevels = rImage.GetDepth();
		desc.depth = 1;
	}

	desc.usage = Usage::WriteOnly;
	desc.pool = pool;
	desc.width = rImage.GetWidth();
	desc.height = rImage.GetHeight();
	desc.format = rImage.GetFormat();
	desc.multiSample = MultiSample::None;

	Texture											*pTexture = NULL;
	pTexture = new WinDX11Texture( desc );

	//s32 surfaceLevel = 0;
	for( u32 surfaceLevel = 0; surfaceLevel < rImage.GetDepth(); ++surfaceLevel )
	{
		pTexture->Update( rImage.GetImage(surfaceLevel), surfaceLevel );
	}

	return pTexture;
}
}}}