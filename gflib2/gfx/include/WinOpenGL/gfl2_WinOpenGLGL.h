#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLGL_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLGL_H_INCLUDED

#include <stdio.h>
#include <memory>
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#include <windows.h>

#include <gfx/include/gfl2_types.h>
#include <util/include/gfl2_AutoComPtr.h>

#include <gfx/include/gfl2_CommonGL.h>
#include <gfx/include/gfl2_ImageData.h>
#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/gfl2_StateObject.h>
#include <gfx/include/gfl2_texture.h>

#include <gfx/include/WinOpenGL/gfl2_WinOpenGLVertexShader.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLSurface.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLOcclusionQuery.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTimerQuery.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLIndexBuffer.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTexture.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLDisplayList.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLGL : public CommonGL
{
public:
	//機種ごとの拡張コードでユーザ−に与えたい物のみ、ここに出す。

	static DisplayDeviceHandle CreateDisplayEx(
    IGLAllocator* allocator,          //!< アロケーター
    HWND hWnmd,
    Format colorFormat = Format::A8R8G8B8,
    Format depthStencilFormat = Format::D24S8,
    MultiSample multiSample = MultiSample::MSAA4X);
  
  //! @brief ディスプレイデバイスを作成する(ウィンドウスタイル指定版)
  //! @return ディスプレイのデバイスはライブラリで完全にコントロールします。よってハンドルのみを返します。
  static DisplayDeviceHandle CreateDisplayEx(
    IGLAllocator*     allocator,          //!< アロケーター
    u32								width,							//!< ディスプレイの幅
    u32								height,							//!< ディスプレイの高さ
    b32								isFullScreen,				//!< フルスクリーンか
    Format						colorFormat,		    //!< カラーバッファのピクセルフォーマット
    Format						depthStencilFormat, //!< 深度ステンシルバッファのピクセルフォーマット
    MultiSample				multiSample,				//!< マルチサンプルモード
    DWORD             windowStyle = DEFAULT_WINDOW_STYLE             //!< ウィンドウスタイル
    );

	static HWND GetHwnd(){ return s_Gp->m_hWnd; }
	static char* ReadFile( const char* filename, s32* p_size, s32 align = 1, gfx::IGLAllocator* pAllocator = NULL );

	//! @name シェーダー処理
	//@{
		//! @brief ファイルから頂点シェーダーを作成
		static Shader* CreateVertexShaderFromFile(const char* path, const char* entry, const VertexElement* pVertexElements, const char** args = NULL, FileFormat format = FileFormat::Unknown);
		
		//! @brief バイナリデータから頂点シェーダーを作成
		static Shader* CreateVertexShaderFromMemory(const void* code, u32 size, const VertexElement* pVertexElements);

		//! @brief ファイルからジオメトリシェーダーを作成
		static Shader* CreateGeometryShaderFromFile(const char* path, const char* entry, const char** args = NULL, FileFormat format = FileFormat::Unknown);

		//! @brief バイナリデータからジオメトリシェーダーを作成
		static Shader* CreateGeometryShaderFromMemory(const void* code, u32 size);

		//! @brief ファイルからピクセルシェーダーを作成
		static Shader* CreatePixelShaderFromFile(const char* path, const char* entry, const char** args = NULL, FileFormat format = FileFormat::Unknown);

		//! @brief バイナリデータからピクセルシェーダーを作成
		static Shader* CreatePixelShaderFromMemory(const void* code, u32 size);
	//@}

   static void SetTargetFPS(f64 fps);

   //araki20150527
   // アクティブなディスプレイを変更
   static void SetActiveDisplay(DisplayDeviceHandle hDisplay)
   {
     s_Gp->SetActiveDisplay_(hDisplay);
   }

   // カラーバッファ転送コマンドを追加
   // カラーバッファを元の形式のまま転送します（Windows：リニア形式）
   // pDestAddrは16バイトアラインで、あらかじめ十分な容量を確保しておいてください（512 x 256 横長左下原点 ピクセルフォーマットはRGBA）
   static void AddTransferColorBufferCommand(void* pDestAddr);

   // カラーバッファ転送コマンドを追加
   // リニア形式で転送します
   // pDestAddrは16バイトアラインで、あらかじめ十分な容量を確保しておいてください（512 x 256 横長左下原点 ピクセルフォーマットはRGB）
   static void AddTransferLinearColorBufferCommand(void* pDestAddr, b32 includeAlpha = false);

   // テクスチャに現在描画中のカラーバッファをテクスチャに転送するコマンドを追加します
   static void AddReadColorBufferToTextureCommand(Texture* pTexture);

    //! @name VSync用のタイマー取得
    static s64 GetVSyncBeginTime()
    {
      return s_Gp->m_VSyncBeginTime;
    }

    //! @name VSync用のフレームカウント取得
    static s64 GetVSyncFrameCount()
    {
      return s_Gp->m_VSyncFrameCount;
    }

protected:

	WinOpenGLGL();
	inline virtual ~WinOpenGLGL();

	//! @name ディスプレイデバイス関連
	//@{
		DisplayDeviceHandle CreateDisplay_( DisplayMode displayMode, u32 width, u32 height, b32 isFullScreen, Format colorFormat, Format depthStencilFormat, MultiSample multiSample, DWORD windowStyle = DEFAULT_WINDOW_STYLE );

		void TransferDisplay_();
		bool SwapDisplay_WaitGPUCommandDone_();
		void SwapDisplay_SwapBuffers_();
		void SwapDisplay_WaitVSync_();
		void SwapDisplay_RunGPUCommand_();

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

		//! @brief 深度ステンシルテクスチャを取得する
		Texture* GetDepthStencilTexture_( DisplayDeviceHandle hDisplay );

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

    //! @brief サーフェースから２Ｄテクスチャを作成する
    Texture* CreateTextureFromSurface_( const Surface* pSurface );
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

		//! @brief サーフェス間のブロックコピー
		b32  BlitSurfaceToTexture_(const Surface* pSurfaceSrc,const Texture *pTextureDst);

		//! @brief レンダリングターゲットをクリアする
		void ClearRenderTarget_( u32 index, const Color &rClearColor );

		//! @brief 深度ステンシルをクリアする
		void ClearDepthStencil_( f32 depth, u8 stencil );

		//! @brief レンダリングターゲットと深度ステンシルをクリアする
    void ClearRenderTargetDepthStencil_( u32 index, const Color &rClearColor, f32 depth, u8 stencil );

		//! @brief ビューポートを設定する
		void SetViewport_(const u32 xp, const u32 yp, const u32 width, const u32 height);

    //! @brief シザーを設定する
		void SetScissor_(const u32 xp, const u32 yp, const u32 width, const u32 height);

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
      return m_pTotalTimerQuery->GetGPUElapsedTimeMicroSeconds();
    }

    // 1フレームにかかったCPUの総時間をマイクロ秒で取得
    u64 GetTotalCPUTimeMicroSecond_()
    {
      return m_pTotalTimerQuery->GetCPUElapsedTimeMicroSeconds();
    }
	//@}

	//! @name 画面サイズ取得関係
	//@{
    // 画面サイズ取得：幅
    u32 GetDisplayWidth_( DisplayDeviceHandle hDisplay );
    // 画面サイズ取得：高さ
    u32 GetDisplayHeight_( DisplayDeviceHandle hDisplay );
	//@}
private:

	static LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static inline bool registerWindowClass( const char* rcn );
	static inline HWND createWindow( HWND hParent, const char* rcn, b32 fullScreen, u32 width, u32 height, DWORD windowStyle = DEFAULT_WINDOW_STYLE );
	DisplayDeviceHandle createDisplay_( DisplayMode displayMode, u32 widtn, u32 height, b32 isFullScreen, Format colorFormat, Format depthStencilFormat, MultiSample multiSample );
  bool initOpenGL();
  static void APIENTRY GlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void* userParam);

	HINSTANCE			m_hInst;
	HWND					m_hWnd;

  HDC           m_hdc;
  HGLRC         m_RenderContext;

	static const int						MRT_MAX = 8;
	static const int						TEXTURE_MAX = 16;
	static const DWORD					DEFAULT_WINDOW_STYLE = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_CLIPSIBLINGS | WS_VISIBLE;

  class DisplayWindow         *m_pCtrSwapDisplayWindowList[CtrDisplayNo::NumberOf];
  bool                        m_IsCtrMode;

	const WinOpenGLSurface			*m_ActgiveRenderTarget[ MRT_MAX ];
	const WinOpenGLSurface			*m_ActgiveDepthStencil;

	WinOpenGLVertexShader				*m_pCurrentShader;
	const WinOpenGLIndexBuffer	*m_pCurrentIndexBuffer;
	const WinOpenGLTexture			*m_pCurrentTexture[TEXTURE_MAX];

  TimerQuery                  *m_pTotalTimerQuery;

  GLuint                      m_ProgramPipeline;
  f64                         m_TargetFPS;
  s64                         m_OverTime;

  s64                         m_VSyncBeginTime;
  s64                         m_VSyncFrameCount;

	static WinOpenGLGL					*s_Gp;
};

}}}

#endif