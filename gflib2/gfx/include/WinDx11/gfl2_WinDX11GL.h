#ifndef GFLIB2_GRX_WINDX11_WINDX11GL_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11GL_H_INCLUDED

#include <stdio.h>
#include <memory>
#define NOMINMAX
#include <windows.h>

#include <gfx/include/gfl2_types.h>
#include <util/include/gfl2_AutoComPtr.h>

#include <gfx/include/gfl2_CommonGL.h>
#include <gfx/include/gfl2_ImageData.h>
#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/gfl2_StateObject.h>
#include <gfx/include/gfl2_texture.h>

#include <gfx/include/WinDX11/gfl2_WinDx11Surface.h>
#include <gfx/include/WinDX11/gfl2_WinDX11QueryObject.h>
#include <gfx/include/WinDx11/gfl2_WinDX11TimerQuery.h>
#include <gfx/include/WinDx11/gfl2_WinDX11OcclusionQuery.h>
#include <gfx/include/WinDx11/gfl2_WinDX11DisplayList.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11GL : public CommonGL
{
public:
	//機種ごとの拡張コードでユーザ−に与えたい物のみ、ここに出す。

	static DisplayDeviceHandle CreateDisplayEx(
    HWND hWnmd,
    Format colorFormat = Format::A8R8G8B8,
    Format depthStencilFormat = Format::D24S8,
    MultiSample multiSample = MultiSample::MSAA4X);
  
  //! @brief ディスプレイデバイスを作成する(ウィンドウスタイル指定版)
  //! @return ディスプレイのデバイスはライブラリで完全にコントロールします。よってハンドルのみを返します。
  static DisplayDeviceHandle CreateDisplayEx(
    u32								width,							//!< ディスプレイの幅
    u32								height,							//!< ディスプレイの高さ
    b32								isFullScreen,				//!< フルスクリーンか
    Format						colorFormat,		    //!< カラーバッファのピクセルフォーマット
    Format						depthStencilFormat, //!< 深度ステンシルバッファのピクセルフォーマット
    MultiSample				multiSample,				//!< マルチサンプルモード
    DWORD             windowStyle             //!< ウィンドウスタイル
    );

	static HWND GetHwnd(){ return s_Gp->m_hWnd; }
	static inline AutoComPtr<ID3D11Device>&				GetDevice(){ return s_Gp->m_pD3DDevice; }
	static inline AutoComPtr<ID3D11DeviceContext>&	GetDeviceContext(){ return s_Gp->m_pD3DDeviceCtx; }
	static char* ReadFile( const char* filename, s32* p_size, s32 align = 1 );
    

	//! @name シェーダー処理
	//@{
		//! @brief ファイルから頂点シェーダーを作成
		static Shader* CreateVertexShaderFromFile(const char* path, const char* entry, const VertexElement* pVertexElements, const char** args = NULL, FileFormat format = FileFormat::Unknown);
		
		//! @brief ファイルからジオメトリシェーダーを作成
		static Shader* CreateGeometryShaderFromFile(const char* path, const char* entry, const char** args = NULL, FileFormat format = FileFormat::Unknown);

		//! @brief ファイルからピクセルシェーダーを作成
		static Shader* CreatePixelShaderFromFile(const char* path, const char* entry, const char** args = NULL, FileFormat format = FileFormat::Unknown);
	//@}

protected:

	WinDX11GL();
	inline virtual ~WinDX11GL();

	//! @name ディスプレイデバイス関連
	//@{
		DisplayDeviceHandle CreateDisplay_( DisplayMode displayMode, u32 width, u32 height, b32 isFullScreen, Format colorFormat, Format depthStencilFormat, MultiSample multiSample, DWORD windowStyle = DEFAULT_WINDOW_STYLE );

		void TransferDisplay_();

		void SwapDisplay_( b32 fVsyncWait = true );

		void DestroyDisplay_( DisplayDeviceHandle hDisplay );
	//@}

	//! @name サーフェイス関連
	//@{
		//! @brief レンダリングターゲットサーフェイスを作成する
		Surface* CreateSurface_(u32 width, u32 height, u32 usage, Format format, Pool pool, MultiSample multiSample)
		{
			Surface::SurfaceDesc					desc;

			desc.usage = usage;
			desc.pool = pool;
			desc.width = width;
			desc.height = height;
			desc.depth = 1;
			desc.format = format;
			desc.multiSample = multiSample;

			return CreateSurface_( desc );
		}

		//! @brief レンダリングターゲットサーフェイスを作成する
		Surface* CreateSurface_( const Surface::SurfaceDesc &rDesc );

		//! @brief バックバッファを取得する
		Surface* GetBackBuffer_( DisplayDeviceHandle hDisplay );

		//! @brief 深度ステンシルバッファを取得する
		Surface* GetDepthStencilBuffer_( DisplayDeviceHandle hDisplay );

		//! @brief レンダリングターゲットを切り替える
		//! @par index でＭＲＴカラーバッファのインデクスを指定
		b32 SetRenderTarget_(u32 index, const Surface* const pSurface);

		//! @brief 現在のレンダリングターゲットを取得する
		const Surface* GetRenderTarget_(u32 index = 0);

		//! @brief 深度ステンシルを切り替える
		b32 SetDepthStencil_(const Surface* const pSurface);

		//! @brief 現在の深度ステンシルを取得する
		const Surface* GetDepthStencil_();
	//@}

	//! @name シェーダー処理
	//@{
		//! @brief ソースコードから頂点シェーダーを作成
		Shader* CreateVertexShaderFromSource_(const char* code, const char* entry, const VertexElement* pVertexElements, const char** args );

		//! @brief バイナリデータから頂点シェーダーを作成
		Shader* CreateVertexShaderFromMemory_(const void* code, u32 size, const VertexElement* pVertexElements);
		
		//! @brief ソースコードからピクセルシェーダーを作成
		Shader* CreateGeometryShaderFromSource_(const char* code, const char* entry, const char** args);

		//! @brief バイナリデータから頂点シェーダーを作成
		Shader* CreateGeometryShaderFromMemory_(const void* code, u32 size);

		//! @brief ソースコードからピクセルシェーダーを作成
		Shader* CreatePixelShaderFromSource_(const char* code, const char* entry, const char** args);

		//! @brief バイナリデータから頂点シェーダーを作成
		Shader* CreatePixelShaderFromMemory_(const void* code, u32 size);

		//! @brief 定数バッファを作成
		static ConstantBuffer* CreateConstantBuffer_( const ConstantBufferElement* pConstantBufferElements, u32 usage, Pool pool, const void* buffer = NULL );
	//@}

	//! @name 頂点処理
	//@{
		//! @brief 頂点バッファを作成
		VertexBuffer* CreateVertexBuffer_( u32 size, u32 vertex_num, u32 usage, Pool pool, const void* buffer = NULL );

		//! @brief インデックスバッファを作成
		static IndexBuffer* CreateIndexBuffer_( u32 size, u32 usage, Pool pool, IndexType type, const void* buffer = NULL );
	//@}

	//! @name 描画ステート関連
	//@{
		//! @brief ブレンドステートオブジェクトを作成
		BlendStateObject* CreateBlendStateObject_();

		//! @brief ラスタライザステートオブジェクトを作成
		RasterizerStateObject* CreateRasterizerStateObject_();

		//! @brief 深度ステンシルステートオブジェクトを作成
		DepthStencilStateObject* CreateDepthStencilStateObject_();

		//! @brief サンプラステートオブジェクトを作成
		SamplerStateObject* CreateSamplerStateObject_();
	//@}

	//! @name クエリ関連
	//@{
		//! @brief タイマークエリを作成
		TimerQuery* CreateTimerQuery_();
		//! @brief オクリュージョンクエリを作成
		OcclusionQuery* CreateOcclusionQuery_();
	//@}

	//! @name ディスプレイリスト関連
	//@{
		//! @brief ディスプレイリストを作成
		DisplayList* CreateDisplayList_();
		//! @brief ディスプレイリストの呼び出し
		void CallDisplayList_(DisplayList* pDisplayList);
	//@}

	//! @name テクスチャ処理
	//@{
		//! @brief ２Ｄテクスチャを作成する
		//! @param usage オプション
		//! @param mipLevels ミップマップレベル数指定( 1 - 13 )
		Texture* CreateTexture_(u32 width, u32 height, u32 mipLevels, u32 usage, Format format, Pool pool);

		//! @brief イメージデータから２Ｄテクスチャを作成する
		Texture* CreateTextureFromImage_( const ImageData &rImage, Pool pool);
	//@}

	//! @name 描画命令関連
	//! @attention  描画系メソッドは必ず BeginScene() 〜 EndScene() の間で呼ばなければいけない
	//@{
    b32 BeginTimerQuery_( const TimerQuery* pTimerQuery);

    void EndTimerQuery_( const TimerQuery* pTimerQuery);

    b32 BeginOcclusionQuery_( const OcclusionQuery* pOcclusionQuery);

    void EndOcclusionQuery_( const OcclusionQuery* pOcclusionQuery);

		b32 BeginConditionalRender_( const OcclusionQuery *pQuery );

		void EndConditionalRender_( const OcclusionQuery *pQuery );

		b32 BeginScene_();

		void EndScene_();

		void Finish_();

		//! @brief レンダリングターゲットをクリアする
		void ClearRenderTarget_( const Surface* pSurface, const Color &rClearColor );

		//! @brief 深度ステンシルをクリアする
		void ClearDepthStencil_( const Surface* pSurface, f32 depth, u8 stencil );

		void SetViewport_(const u32 xp, const u32 yp, const u32 width, const u32 height);

		//! @brief 頂点シェーダーを切り替える
		void SetVertexShader_( const Shader* pShader );

		//! @brief ジオメトリシェーダーを切り替える
		void SetGeometryShader_( const Shader* pShader );

		//! @brief ピクセルシェーダーを切り替える
		void SetPixelShader_( const Shader* pShader );

		//! @brief 頂点シェーダ定数設定
		void SetVertexShaderConstant_( u32 registerNo, ConstantBuffer *pBuffer );

		//! @brief ジオメトリシェーダ定数設定
		void SetGeometryShaderConstant_( u32 registerNo, ConstantBuffer *pBuffer );

		//! @brief ピクセルシェーダ定数設定
		void SetPixelShaderConstant_( u32 registerNo,	ConstantBuffer *pBuffer	);

		//! @brief インデクスバッファを設定します
		void SetIndices_(const IndexBuffer* pIndices);

		//! @brief 頂点バッファをストリームに設定
		void SetStreamSource_( const u32 index, const VertexBuffer* pVertexBuffer, const s32 offset, const s32 stride );

		//! @brief 頂点ストリームにセットされた頂点を使って描画
		void DrawPrimitive_( PrimitiveType primitive, u32 start, u32 count	);

		//! @brief 頂点ストリームにセットされた頂点とインデックスバッファを使って描画
		void DrawIndexedPrimitive_( PrimitiveType primitive, u32	start, u32	count );

		//! @brief ブレンドステートの設定
		void SetBlendState_( BlendStateObject *pState );

		//! @brief ラスタライザステートの設定
		void SetRasterizerState_( RasterizerStateObject *pState );

		//! @brief 深度ステンシルステートの設定
		void SetDepthStencilState_( DepthStencilStateObject *pState );

		//! @brief サンプラステートの設定
		void SetSamplerState_( u32 texNo, const SamplerStateObject *pState );

		//! @brief テクスチャの設定
		void SetTexture_( u32 texNo, const Texture *pTexture );
	//@}

	//! @name 処理負荷計測関係
	//@{
    // 1フレームにかかったGPUの総時間をマイクロ秒で取得
    u64 GetTotalGPUTimeMicroSecond_()
    {
      // 未対応
      return 0;
    }

    // 1フレームにかかったCPUの総時間をマイクロ秒で取得
    u64 GetTotalCPUTimeMicroSecond_()
    {
      // 未対応
      return 0;
    }
	//@}

	//! @name 画面サイズ取得関係
	//@{
    // 画面サイズ取得：幅
    u32 GetDisplayWidth_( DisplayDeviceHandle hDisplay )
    {
      // 未対応
      return 0;
    }
    // 画面サイズ取得：高さ
    u32 GetDisplayHeight_( DisplayDeviceHandle hDisplay )
    {
      // 未対応
      return 0;
    }
	//@}

private:

	static LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static inline bool registerWindowClass( const char* rcn );
	static inline HWND createWindow( HWND hParent, const char* rcn, b32 fullScreen, int width, int height, DWORD windowStyle = DEFAULT_WINDOW_STYLE );
	DisplayDeviceHandle createDisplay_( HWND hwnd, b32 isFullScreen, Format colorFormat, Format depthStencilFormat, MultiSample multiSample );

	HINSTANCE			m_hInst;
	HWND					m_hWnd;

	AutoComPtr<ID3D11Device>						m_pD3DDevice;
	AutoComPtr<ID3D11DeviceContext>			m_pD3DDeviceCtx;

	static const int					MRT_MAX = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
	static const DWORD				DEFAULT_WINDOW_STYLE = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_CLIPSIBLINGS | WS_VISIBLE;
	const WinDX11Surface			*m_ActgiveRenderTarget[ MRT_MAX ];
	const WinDX11Surface			*m_ActgiveDepthStencil;
	const WinDX11QueryObject	*m_ActgiveQueryObject;

	static WinDX11GL					*s_Gp;
};

}}}

#endif