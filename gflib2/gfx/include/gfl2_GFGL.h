#ifndef GFLIB2_GRX_GFGL_H_INCLUDED
#define GFLIB2_GRX_GFGL_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>

#include <gfx/include/gfl2_GLMemory.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_Matrix44.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_StateObject.h>
#include <gfx/include/gfl2_texture.h>
#include <gfx/include/gfl2_ImageData.h>
#include <gfx/include/gfl2_TimerQuery.h>
#include <gfx/include/gfl2_OcclusionQuery.h>
#include <gfx/include/gfl2_DisplayList.h>
#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/gfl2_Shader.h>

#if defined(GF_PLATFORM_WIN32)

#if defined(_WIN64)
	#include <gfx/include/WinDX11/gfl2_WinDX11GL.h>
#else
	#include <gfx/include/WinOpenGL/gfl2_WinOpenGLGL.h>
#endif

#elif defined(NN_PLATFORM_CTR)
  #include <gfx/include/CTR/gfl2_CTRGL.h>
#endif

#include <gfx/include/gfl2_GFGLUtil.h>

// ディスプレイリスト用マクロ
#if defined(_WIN32)
  #if defined(_WIN64)
    // DirectX版
    #define GFGL_BEGIN_DISPLAY_LIST(pDisplayList) {static const auto _display_list_func_ = [this]() {
    #define GFGL_END_DISPLAY_LIST(pDisplayList) }; static_cast<gfl2::gfx::windx11::WinDX11DisplayList*>(pDisplayList)->SetFunc(_display_list_func_);}
  #else
    // OpenGL版
    #define GFGL_BEGIN_DISPLAY_LIST(pDisplayList) {static const auto _display_list_func_ = [this]() {
    #define GFGL_END_DISPLAY_LIST(pDisplayList) }; static_cast<gfl2::gfx::winOpenGL::WinOpenGLDisplayList*>(pDisplayList)->SetFunc(_display_list_func_);}
  #endif
#else
#define GFGL_BEGIN_DISPLAY_LIST(pDisplayList) GFGL::BeginDisplayList(pDisplayList)
#define GFGL_END_DISPLAY_LIST(pDisplayList) GFGL::EndDisplayList(pDisplayList)
#endif

namespace gfl2 { namespace gfx {

class IGLGLCallBack
{
public:
  virtual void DrawEnd() = 0;
};

template <class Implement_>
class GFGLBase : public Implement_
{
public:
	inline static void Initialize( IGLAllocator* pObj )
	{
		//GLMemory::SetAllocator( pObj );
    GLMemory::SetInstanceAllocator( pObj );

		if ( s_Gp == NULL )
			s_Gp = new GFGLBase<Implement_>();

    GLMemory::ClearInstanceAllocator();
	}

	inline static void Terminate()
	{
		if ( s_Gp )
			delete s_Gp;

		//GLMemory::SetAllocator( NULL );
	}

	IGLGLCallBack		*m_pIGLGLCallBack;

	inline static void SetGLGLCallBack( IGLGLCallBack *pIGLGLCallBack )
	{
		if ( s_Gp )
		{
			s_Gp->m_pIGLGLCallBack = pIGLGLCallBack;
		}
	}

	//! @name ディスプレイデバイス関連
	//@{
		//! @brief ディスプレイデバイスを作成する
		//! @return ディスプレイのデバイスはライブラリで完全にコントロールします。よってハンドルのみを返します。
		inline static DisplayDeviceHandle CreateDisplay(
      IGLAllocator*     allocator,          //!< アロケーター
			DisplayMode				displayMode,				//!< ディスプレイモード
			Format						colorFormat,		    //!< カラーバッファのピクセルフォーマット
			Format						depthStencilFormat, //!< 深度ステンシルバッファのピクセルフォーマット
			MultiSample				multiSample					//!< マルチサンプルモード
		)
		{
			u32 width  = GFGLUtil::GetDisplayWidthByDisplayMode( displayMode );
			u32 height = GFGLUtil::GetDisplayHeightByDisplayMode( displayMode );
			b32 isFullScreen = GFGLUtil::IsFullScreenDisplayMode( displayMode );

      GLMemory::SetInstanceAllocator( allocator );
			DisplayDeviceHandle handle = s_Gp->CreateDisplay_( displayMode, width, height, isFullScreen, colorFormat, depthStencilFormat, multiSample );
      GLMemory::ClearInstanceAllocator();
      return handle;
		}

		//! @brief ディスプレイデバイスを作成する
		//! @return ディスプレイのデバイスはライブラリで完全にコントロールします。よってハンドルのみを返します。
		inline static DisplayDeviceHandle CreateDisplay(
      IGLAllocator*     allocator,          //!< アロケーター
			u32								width,							//!< ディスプレイの幅
			u32								height,							//!< ディスプレイの高さ
			b32								isFullScreen,				//!< フルスクリーンか
			Format						colorFormat,		    //!< カラーバッファのピクセルフォーマット
			Format						depthStencilFormat, //!< 深度ステンシルバッファのピクセルフォーマット
			MultiSample				multiSample				//!< マルチサンプルモード
		)
		{
      GLMemory::SetInstanceAllocator( allocator );
			DisplayDeviceHandle handle = s_Gp->CreateDisplay_( DisplayMode::UserSetting, width, height, isFullScreen, colorFormat, depthStencilFormat, multiSample );
      GLMemory::ClearInstanceAllocator();
      return handle;
		}
		//! @brief ディスプレイデバイスを作成する
		//! @return ディスプレイのデバイスはライブラリで完全にコントロールします。よってハンドルのみを返します。
		/*inline static DisplayDeviceHandle CreateDisplay(
			DisplayMode				displayMode,				//!< ディスプレイモード
			Format						colorFormat,		    //!< カラーバッファのピクセルフォーマット
			Format						depthStencilFormat, //!< 深度ステンシルバッファのピクセルフォーマット
			MultiSample				multiSample					//!< マルチサンプルモード
		)
		{
			return CreateDisplay(GLMemory::GetAllocator(), displayMode, colorFormat, depthStencilFormat, multiSample );
		}*/

		//! @brief ディスプレイデバイスを作成する
		//! @return ディスプレイのデバイスはライブラリで完全にコントロールします。よってハンドルのみを返します。
		/*inline static DisplayDeviceHandle CreateDisplay(
			u32								width,							//!< ディスプレイの幅
			u32								height,							//!< ディスプレイの高さ
			b32								isFullScreen,				//!< フルスクリーンか
			Format						colorFormat,		    //!< カラーバッファのピクセルフォーマット
			Format						depthStencilFormat, //!< 深度ステンシルバッファのピクセルフォーマット
			MultiSample				multiSample				//!< マルチサンプルモード
		)
		{
			return CreateDisplay(GLMemory::GetAllocator(),width, height, isFullScreen, colorFormat, depthStencilFormat, multiSample );
		}*/

		//! @brief アクティブなディスプレイを変更する
		inline static b32 SetActiveDisplay( DisplayDeviceHandle hDisplay )
		{
			return s_Gp->SetActiveDisplay_( hDisplay );
		}

		//! @brief アクティブなディスプレイを変更する
		inline static DisplayDeviceHandle GetActiveDisplay()
		{
			return s_Gp->GetActiveDisplay_();
		}

		//! @brief ディスプレイを破棄する
		inline static void DestroyDisplay( DisplayDeviceHandle hDisplay )
		{
			s_Gp->DestroyDisplay_( hDisplay );
		}

		//! @brief フレームバッファの内容をディスプレイに転送する
		inline static void TransferDisplay()
		{
			s_Gp->TransferDisplay_();
		}

		//! @brief 画面をフリップする
		inline static void SwapDisplay( b32 fVsyncWait = true )
		{
      // GPUコマンド実行待ち
      if (SwapDisplay_WaitGPUCommandDone())
      {
        // スワップバッファ
        SwapDisplay_SwapBuffers();

        if (fVsyncWait)
        {
          // 垂直同期待ち
          SwapDisplay_WaitVSync();
        }
      }

      // GPUコマンド実行
      SwapDisplay_RunGPUCommand();
		}

		//! @brief SwapDisplayの分割版：GPUコマンド実行待ち
		inline static bool SwapDisplay_WaitGPUCommandDone()
		{
			if ( s_Gp->m_pIGLGLCallBack )
				s_Gp->m_pIGLGLCallBack->DrawEnd();

			return s_Gp->SwapDisplay_WaitGPUCommandDone_();
		}
		//! @brief SwapDisplayの分割版：スワップバッファ
		inline static void SwapDisplay_SwapBuffers()
		{
			s_Gp->SwapDisplay_SwapBuffers_();
		}
		//! @brief SwapDisplayの分割版：垂直同期待ち
		inline static void SwapDisplay_WaitVSync()
		{
			s_Gp->SwapDisplay_WaitVSync_();
		}
		//! @brief SwapDisplayの分割版：GPUコマンド実行
		inline static void SwapDisplay_RunGPUCommand()
		{
			s_Gp->SwapDisplay_RunGPUCommand_();
		}
	//@}

	//! @name サーフェイス関連
	//@{
		//! @brief バックバッファを取得する
		inline static Surface* GetBackBuffer( DisplayDeviceHandle hDisplay )
		{
			return s_Gp->GetBackBuffer_( hDisplay );
		}

		//! @brief 深度ステンシルバッファを取得する
		inline static Surface* GetDepthStencilBuffer( DisplayDeviceHandle hDisplay )
		{
			return s_Gp->GetDepthStencilBuffer_( hDisplay );
		}

		//! @brief 深度ステンシルバッファを取得する
		inline static Texture* GetDepthStencilTexture( DisplayDeviceHandle hDisplay )
		{
			return s_Gp->GetDepthStencilTexture_( hDisplay );
		}

		//! @brief サーフェイスを作成する
    inline static Surface* CreateSurface( IGLAllocator* allocator, Surface::SurfaceDesc &rDesc )
		{
      GLMemory::SetInstanceAllocator( allocator );
			Surface* surface = s_Gp->CreateSurface_(rDesc);
      GLMemory::ClearInstanceAllocator();
      return surface;
		}

		//! @brief サーフェイスを作成する
		/*inline static Surface* CreateSurface( const Surface::SurfaceDesc &rDesc )
		{
      return CreateSurface( GLMemory::GetAllocator(), rDesc );
		}*/

		//! @brief レンダリングターゲットサーフェイスを作成する
		inline static Surface* CreateRenderTarget(IGLAllocator* allocator, u32 width, u32 height, Format format, MultiSample multiSample = MultiSample::None)
		{
      GLMemory::SetInstanceAllocator( allocator );
			Surface* surface = s_Gp->CreateSurface_(width, height, Usage::RenderTarget | Usage::Tiled, format, Pool::Default, multiSample);
      GLMemory::ClearInstanceAllocator();
      return surface;
		}

		//! @brief レンダリングターゲットサーフェイスを作成する
		/*inline static Surface* CreateRenderTarget(u32 width, u32 height, Format format, MultiSample multiSample = MultiSample::None)
		{
      return CreateRenderTarget(GLMemory::GetAllocator(), width, height, Usage::RenderTarget | Usage::Tiled, format, Pool::Default, multiSample);
		}*/

		//! @brief レンダリングターゲットを切り替える
		//! @par index でＭＲＴカラーバッファのインデクスを指定
		inline static b32 SetRenderTarget(u32 index, const Surface* const pSurface)
		{
			return s_Gp->SetRenderTarget_(index, pSurface);
		}

		inline static b32 SetRenderTarget(const Surface* const pSurface)
		{
			return s_Gp->SetRenderTarget_(0, pSurface);
		}

		//! @brief 現在のレンダリングターゲットを取得する
		inline static const Surface* GetRenderTarget(u32 index = 0)
		{
			return s_Gp->GetRenderTarget_(index);
		}
		
		//! @brief 深度ステンシルサーフェイスを作成する
		inline static Surface* CreateDepthStencil(IGLAllocator* allocator, u32 width, u32 height, Format format, MultiSample multiSample = MultiSample::None)
		{
      GLMemory::SetInstanceAllocator( allocator );
			Surface* surface = s_Gp->CreateSurface_(width, height, Usage::DepthStencil | Usage::Tiled | Usage::ZCull, format, Pool::Default, multiSample);
      GLMemory::ClearInstanceAllocator();
      return surface;
		}

		//! @brief 深度ステンシルサーフェイスを作成する
		/*inline static Surface* CreateDepthStencil(u32 width, u32 height, Format format, MultiSample multiSample = MultiSample::None)
		{
			return CreateDepthStencil(GFMemory::GetAllocator(), width, height, format, multiSample);
		}*/

		//! @brief 深度ステンシルを切り替える
		inline static b32 SetDepthStencil(const Surface* const pSurface)
		{
			return s_Gp->SetDepthStencil_(pSurface);
		}
		//! @brief 現在の深度ステンシルを取得する
		inline static const Surface* GetDepthStencil()
		{
			return s_Gp->GetDepthStencil_();
		}

		//! @brief サーフェス間のブロックコピー
		inline static b32 BlitSurfaceToTexture(const Surface* pSurfaceSrc,const Texture *pTextureDst)
		{
		#if defined(_WIN32)
			return s_Gp->BlitSurfaceToTexture_(pSurfaceSrc,pTextureDst);
		#else
			return false;
		#endif
		}

	//@}

	//! @name シェーダー処理
	//@{
		//! @brief ソースコードから頂点シェーダーを作成
		static Shader* CreateVertexShaderFromSource(IGLAllocator * allocator,
                                                const char* code,
																								const char* entry,
																								const VertexElement* pVertexElements,	//!< 頂点宣言を定義した構造体
																								const char** args
																								)
		{
      GLMemory::SetInstanceAllocator( allocator );
			Shader* shader = s_Gp->CreateVertexShaderFromSource_(code, entry, pVertexElements, args);
      GLMemory::ClearInstanceAllocator();
      return shader;
		}

		//! @brief ソースコードから頂点シェーダーを作成
		/*static Shader* CreateVertexShaderFromSource(const char* code,
																								const char* entry,
																								const VertexElement* pVertexElements,	//!< 頂点宣言を定義した構造体
																								const char** args
																								)
		{
      return CreateVertexShaderFromSource(GLMemory::GetAllocator(), code, entry, pVertexElements, args);
		}*/

		//! @brief バイナリデータから頂点シェーダーを作成
		static Shader* CreateVertexShaderFromMemory(IGLAllocator* allocator,
                                                const void* code,
																								u32	size,
																								const VertexElement* pVertexElements	//!< 頂点宣言を定義した構造体
																								)
		{
      GLMemory::SetInstanceAllocator( allocator );
			Shader* shader = s_Gp->CreateVertexShaderFromMemory_(code, size, pVertexElements);
      GLMemory::ClearInstanceAllocator();
      return shader;
		}

		//! @brief バイナリデータから頂点シェーダーを作成
		/*static Shader* CreateVertexShaderFromMemory(const void* code,
																								u32	size,
																								const VertexElement* pVertexElements	//!< 頂点宣言を定義した構造体
																								)
		{
      return CreateVertexShaderFromMemory(GLMemory::GetAllocator(), code, size, pVertexElements);
		}*/



		//! @brief ソースコードからジオメトリシェーダーを作成
		static Shader* CreateGeometryShaderFromSource(IGLAllocator * allocator, const char* code, const char* entry, const char** args)
		{
      GLMemory::SetInstanceAllocator( allocator );
			Shader* shader = s_Gp->CreateGeometryShaderFromSource_(code, entry, args);
      GLMemory::ClearInstanceAllocator();
      return shader;
		}

		//! @brief ソースコードからジオメトリシェーダーを作成
		/*static Shader* CreateGeometryShaderFromSource(const char* code, const char* entry, const char** args)
		{
      return CreateGeometryShaderFromSource(GLMemory::GetAllocator(), code, entry, args);
		}*/

		//! @brief バイナリデータからジオメトリシェーダーを作成
		static Shader* CreateGeometryShaderFromMemory(IGLAllocator* allocator, const void* code, u32 size)
		{
      GLMemory::SetInstanceAllocator( allocator );
			Shader* shader = s_Gp->CreateGeometryShaderFromMemory_(code, size);
      GLMemory::ClearInstanceAllocator();
      return shader;
		}

		//! @brief バイナリデータからジオメトリシェーダーを作成
		/*static Shader* CreateGeometryShaderFromMemory(const void* code, u32 size)
		{
      return CreateGeometryShaderFromMemory(GLMemory::GetAllocator(), code, size);
		}*/
		
		//! @brief ソースコードからピクセルシェーダーを作成
		static Shader* CreatePixelShaderFromSource(IGLAllocator* allocator, const char* code, const char* entry, const char** args)
		{
      GLMemory::SetInstanceAllocator( allocator );
			Shader* shader = s_Gp->CreatePixelShaderFromSource_(code, entry, args);
      GLMemory::ClearInstanceAllocator();
      return shader;
		}

		//! @brief ソースコードからピクセルシェーダーを作成
		/*static Shader* CreatePixelShaderFromSource(const char* code, const char* entry, const char** args)
		{
      return CreatePixelShaderFromSource(GLMemory::GetAllocator(), code, entry, args);
		}*/

		//! @brief バイナリデータからピクセルシェーダーを作成
		static Shader* CreatePixelShaderFromMemory(IGLAllocator* allocator, const void* code, u32 size)
		{
      GLMemory::SetInstanceAllocator( allocator );
			Shader* shader = s_Gp->CreatePixelShaderFromMemory_(code, size);
      GLMemory::ClearInstanceAllocator();
      return shader;
		}

		//! @brief バイナリデータからピクセルシェーダーを作成
		/*static Shader* CreatePixelShaderFromMemory(const void* code, u32 size)
		{
      return CreatePixelShaderFromMemory(GLMemory::GetAllocator(), code, size);
		}*/

		//! @brief 定数バッファを作成
		static ConstantBuffer* CreateConstantBuffer(
      IGLAllocator*                   allocator,                //!< アロケーター
			const ConstantBufferElement*		pConstantBufferElements,	//!< 定数バッファを定義した構造体
			u32															usage,										//!< バッファの使われ方を示すフラグ
			Pool														pool,											//!< バッファを確保するプールを指定
			const void*											buffer = NULL							//!< バッファにコピーするデータのポインタ。\nNULLだったらバッファ内容は不定。
		)
		{
      GLMemory::SetInstanceAllocator( allocator );
			ConstantBuffer* constant =  s_Gp->CreateConstantBuffer_( pConstantBufferElements, usage, pool, buffer );
      GLMemory::ClearInstanceAllocator();
      return constant;
		}

		//! @brief 定数バッファを作成
		/*static ConstantBuffer* CreateConstantBuffer(
			const ConstantBufferElement*		pConstantBufferElements,	//!< 定数バッファを定義した構造体
			u32															usage,										//!< バッファの使われ方を示すフラグ
			Pool														pool,											//!< バッファを確保するプールを指定
			const void*											buffer = NULL							//!< バッファにコピーするデータのポインタ。\nNULLだったらバッファ内容は不定。
		)
		{
      return CreateConstantBuffer( GLMemory::GetAllocator(), pConstantBufferElements, usage, pool, buffer );
		}*/
	//@}

	//! @name 頂点処理
	//@{
		//! @brief 頂点バッファを作成
		static VertexBuffer* CreateVertexBuffer(
      IGLAllocator* allocator, //!< アロケーター
			u32       size,         //!< １頂点あたりのサイズ
			u32       vertex_num,   //!< 頂点数
			u32       usage,        //!< バッファの使われ方を示すフラグ
			Pool        pool,         //!< バッファを確保するプールを指定
			const void* buffer = NULL //!< バッファにコピーするデータのポインタ。\nNULLだったらバッファ内容は不定。
		)
		{
      GLMemory::SetInstanceAllocator( allocator );
			VertexBuffer* vertex = s_Gp->CreateVertexBuffer_( size, vertex_num, usage, pool, buffer );
      GLMemory::ClearInstanceAllocator();
      return vertex;
		}

		//! @brief 頂点バッファを作成
		/*static VertexBuffer* CreateVertexBuffer(
			u32       size,         //!< １頂点あたりのサイズ
			u32       vertex_num,   //!< 頂点数
			u32       usage,        //!< バッファの使われ方を示すフラグ
			Pool        pool,         //!< バッファを確保するプールを指定
			const void* buffer = NULL //!< バッファにコピーするデータのポインタ。\nNULLだったらバッファ内容は不定。
		)
		{
      return CreateVertexBuffer( GLMemory::GetAllocator(), size, vertex_num, usage, pool, buffer );
		}*/

		//! @brief インデックスバッファを作成
		static IndexBuffer* CreateIndexBuffer(
      IGLAllocator* allocator,  //!< アロケーター
			u32					size,         //!< 確保するバッファサイズ
			u32					usage,        //!< バッファの使われ方を示すフラグ
			Pool        pool,         //!< バッファを確保するプールを指定
			IndexType   type,         //!< インデクスタイプ
			const void* buffer = NULL //!< バッファにコピーするデータのポインタ。\nNULLだったらバッファ内容は不定。
		)
		{
      GLMemory::SetInstanceAllocator(allocator);
			IndexBuffer* index = s_Gp->CreateIndexBuffer_( size, usage, pool, type, buffer );
      GLMemory::ClearInstanceAllocator();
      return index;
		}

		//! @brief インデックスバッファを作成
		/*static IndexBuffer* CreateIndexBuffer(
			u32					size,         //!< 確保するバッファサイズ
			u32					usage,        //!< バッファの使われ方を示すフラグ
			Pool        pool,         //!< バッファを確保するプールを指定
			IndexType   type,         //!< インデクスタイプ
			const void* buffer = NULL //!< バッファにコピーするデータのポインタ。\nNULLだったらバッファ内容は不定。
		)
		{
      return CreateIndexBuffer( GLMemory::GetAllocator(), size, usage, pool, type, buffer );
		}*/
	//@}

	//! @name 描画ステート関連
	//@{
		//! @brief ブレンドステートオブジェクトを作成
		static BlendStateObject* CreateBlendStateObject( IGLAllocator* allocator )
		{
      GLMemory::SetInstanceAllocator( allocator );
      BlendStateObject* blend = s_Gp->CreateBlendStateObject_();
      GLMemory::ClearInstanceAllocator();
      return blend;
		}

		//! @brief ブレンドステートオブジェクトを作成
		/*static BlendStateObject* CreateBlendStateObject()
		{
      return CreateBlendStateObject( GLMemory::GetAllocator() );
		}*/

		//! @brief ラスタライザステートオブジェクトを作成
		static RasterizerStateObject* CreateRasterizerStateObject( IGLAllocator* allocator )
		{
      GLMemory::SetInstanceAllocator( allocator );
      RasterizerStateObject* resterizer = s_Gp->CreateRasterizerStateObject_();
      GLMemory::ClearInstanceAllocator();
      return resterizer;
		}

		//! @brief ラスタライザステートオブジェクトを作成
		/*static RasterizerStateObject* CreateRasterizerStateObject()
		{
			return CreateRasterizerStateObject( GLMemory::GetAllocator() );
		}*/

		//! @brief 深度ステンシルステートオブジェクトを作成
		static DepthStencilStateObject* CreateDepthStencilStateObject( IGLAllocator* allocator )
		{
      GLMemory::SetInstanceAllocator( allocator );
			DepthStencilStateObject* depth = s_Gp->CreateDepthStencilStateObject_();
      GLMemory::ClearInstanceAllocator();
      return depth;
		}

		//! @brief 深度ステンシルステートオブジェクトを作成
		/*static DepthStencilStateObject* CreateDepthStencilStateObject()
		{
      return CreateDepthStencilStateObject( GLMemory::GetAllocator() );
		}*/

		//! @brief サンプラステートオブジェクトを作成
		static SamplerStateObject* CreateSamplerStateObject( IGLAllocator* allocator )
		{
      GLMemory::SetInstanceAllocator( allocator );
      SamplerStateObject* sampler = s_Gp->CreateSamplerStateObject_();
      GLMemory::ClearInstanceAllocator();
      return sampler;
		}

		//! @brief サンプラステートオブジェクトを作成
		/*static SamplerStateObject* CreateSamplerStateObject()
		{
      return CreateSamplerStateObject( GLMemory::GetAllocator() );
		}*/
	//@}

	//! @name クエリ関連
	//@{
		//! @brief タイマークエリを作成
    static TimerQuery* CreateTimerQuery( IGLAllocator* allocator )
    {
      GLMemory::SetInstanceAllocator( allocator );
      TimerQuery* time = s_Gp->CreateTimerQuery_();
      GLMemory::ClearInstanceAllocator();
      return time;
    }

		//! @brief タイマークエリを作成
		/*static TimerQuery* CreateTimerQuery()
    {
      return CreateTimerQuery( GLMemory::GetAllocator() );
    }*/

		//! @brief オクリュージョンクエリを作成
		static OcclusionQuery* CreateOcclusionQuery( IGLAllocator* allocator )
		{
      GLMemory::SetInstanceAllocator(allocator);
			OcclusionQuery* occlusion = s_Gp->CreateOcclusionQuery_();
      GLMemory::ClearInstanceAllocator();
      return occlusion;
		}

		//! @brief オクリュージョンクエリを作成
		/*static OcclusionQuery* CreateOcclusionQuery()
		{
      return CreateOcclusionQuery( GLMemory::GetAllocator() );
		}*/
	//@}

	//! @name ディスプレイリスト関連
	//@{
		//! @brief ディスプレイリストを作成
    static DisplayList* CreateDisplayList( IGLAllocator* allocator )
    {
      GLMemory::SetInstanceAllocator(allocator);
      DisplayList* display = s_Gp->CreateDisplayList_();
      GLMemory::ClearInstanceAllocator();
      return display;
    }
		//! @brief ディスプレイリストを作成
		/*static DisplayList* CreateDisplayList()
    {
      return CreateDisplayList( GLMemory::GetAllocator() );
    }*/
#if defined(_WIN32)
		//! @brief ディスプレイリストの記録開始
		//! @brief ディスプレイリストの記録終了
#else
		//! @brief ディスプレイリストの記録開始
		static void BeginDisplayList(DisplayList* pDisplayList)
    {
			s_Gp->SetDrawFlag_(true);
      s_Gp->BeginDisplayList_(pDisplayList);
    }
		//! @brief ディスプレイリストの記録終了
		static void EndDisplayList(DisplayList* pDisplayList)
    {
      s_Gp->EndDisplayList_(pDisplayList);
			s_Gp->SetDrawFlag_(false);
    }
#endif
		//! @brief ディスプレイリストの呼び出し
		static void CallDisplayList(DisplayList* pDisplayList)
    {
      s_Gp->CallDisplayList_(pDisplayList);
    }
	//@}

	//! @name テクスチャ処理
	//@{
		//! @brief ２Ｄテクスチャを作成する
		//! @param usage オプション
		//! @param mipLevels ミップマップレベル数指定( 1 - 13 )
		static Texture* CreateTexture(IGLAllocator* allocator, u32 width, u32 height, u32 mipLevels, u32 usage, Format format, Pool pool)
		{
      GLMemory::SetInstanceAllocator(allocator);
			Texture* tex = s_Gp->CreateTexture_( width, height, mipLevels, usage, format, pool );
      GLMemory::ClearInstanceAllocator();
      return tex;
		}

		//! @brief ２Ｄテクスチャを作成する
		//! @param usage オプション
		//! @param mipLevels ミップマップレベル数指定( 1 - 13 )
		/*static Texture* CreateTexture(u32 width, u32 height, u32 mipLevels, u32 usage, Format format, Pool pool)
		{
      return CreateTexture( GLMemory::GetAllocator(), width, height, mipLevels, usage, format, pool );
		}*/


		//! @brief イメージデータから２Ｄテクスチャを作成する
		static Texture* CreateTextureFromImage( IGLAllocator* allocator, const ImageData &rImage, Pool pool)
		{
      GLMemory::SetInstanceAllocator(allocator);
			Texture* tex = s_Gp->CreateTextureFromImage_( rImage, pool);
      GLMemory::ClearInstanceAllocator();
      return tex;
		}

		//! @brief イメージデータから２Ｄテクスチャを作成する
		/*static Texture* CreateTextureFromImage( const ImageData &rImage, Pool pool)
		{
      return CreateTextureFromImage( GLMemory::GetAllocator(), rImage, pool);
		}*/

    //! @brief サーフェースから２Ｄテクスチャを作成する
    static Texture* CreateTextureFromSurface( IGLAllocator* allocator, const Surface* pSurface )
    {
      GLMemory::SetInstanceAllocator(allocator);
      Texture* tex = s_Gp->CreateTextureFromSurface_(pSurface);
      GLMemory::ClearInstanceAllocator();
      return tex;
    }

    //! @brief サーフェースから２Ｄテクスチャを作成する
    /*static Texture* CreateTextureFromSurface( const Surface* pSurface )
    {
      return CreateTextureFromSurface(GLMemory::GetAllocator(), pSurface);
    }*/

	//@}

	//! @name 描画命令関連
	//! @attention  描画系メソッドは必ず BeginScene() 〜 EndScene() の間で呼ばなければいけない
	//@{
		//! @brief シーンの描画中かどうかを返す
		inline static b32 IsSceneDrawing()
		{
			return s_Gp->IsSceneDrawing_();
		}
		
		//! @brief タイマークエリの描画処理を開始する。\n
		//! タイマークエリ時の描画系メソッドは必ず BeginQuery() 〜 EndQuery() の間で呼ばなければいけない
		//! @return 描画可能な状態になければ false を返す。この場合は EndQuery() を呼び出す必要はない。
		inline static b32 BeginTimerQuery( TimerQuery *pTimerQuery )
		{
			return s_Gp->BeginTimerQuery_( pTimerQuery );
		}

		//! @brief タイマークエリの描画処理を終了する
		inline static void EndTimerQuery( TimerQuery* pTimerQuery )
		{
			s_Gp->EndTimerQuery_(pTimerQuery);
		}

		//! @brief オクリュージョンクエリの描画処理を開始する。\n
		//! オクリュージョンクエリ時の描画系メソッドは必ず BeginQuery() 〜 EndQuery() の間で呼ばなければいけない
		//! @return 描画可能な状態になければ false を返す。この場合は EndQuery() を呼び出す必要はない。
		inline static b32 BeginOcclusionQuery( OcclusionQuery *pOcclusionQuery )
		{
			return s_Gp->BeginOcclusionQuery_( pOcclusionQuery );
		}

		//! @brief オクリュージョンクエリの描画処理を終了する
		inline static void EndOcclusionQuery( OcclusionQuery* pOcclusionQuery )
		{
			s_Gp->EndOcclusionQuery_(pOcclusionQuery);
		}

		//! @brief オクリュージョンクエリを用いた描画処理を開始する。\n
		//! 描画系メソッドは必ず BeginConditionalRender() 〜 EndConditionalRender() の間で呼ばなければいけない
		//! @return 描画可能な状態になければ false を返す。この場合は EndConditionalRender() を呼び出す必要はない。
		inline static b32 BeginConditionalRender( const OcclusionQuery *pOcclusinQuery )
		{
			return s_Gp->BeginConditionalRender_( pOcclusinQuery );
		}

		//! @brief オクリュージョンクエリを用いた描画処理を終了する
		inline static void EndConditionalRender( const OcclusionQuery *pOcclusinQuery )
		{
			s_Gp->EndConditionalRender_( pOcclusinQuery );
		}

		//! @brief シーンの描画処理を開始する。\n
		//! 描画系メソッドは必ず BeginScene() 〜 EndScene() の間で呼ばなければいけない
		//! @return 描画可能な状態になければ false を返す。この場合は EndScene() を呼び出す必要はない。
		inline static b32 BeginScene()
		{
			s_Gp->SetDrawFlag_(true);
			return s_Gp->BeginScene_();
		}

		//! @brief シーンの描画処理を終了する
		inline static void EndScene()
		{
			s_Gp->EndScene_();
			s_Gp->SetDrawFlag_(false);
		}
		
		//! @brief ＧＰＵがIdle状態になるまでブロックする
		inline static void Finish()
		{
			s_Gp->Finish_();
		}

    // -- begin 旧関数
		//! @brief レンダリングターゲットをクリアする
		inline static void ClearRenderTarget( const Surface* pSurface, const Color &rClearColor )
		{
      (void)pSurface;

      ClearRenderTarget(rClearColor);
		}

		//! @brief 深度ステンシルをクリアする
		inline static void ClearDepthStencil( const Surface* pSurface, f32 depth = 1.0f, u8 stencil = 0xFF )
		{
      (void)pSurface;

      ClearDepthStencil(depth, stencil);
		}

		//! @brief レンダリングターゲットと深度ステンシルをクリアする
    inline static void ClearRenderTargetDepthStencil( const Surface* pRenderTarget, const Color &rClearColor, const Surface* pDepthStencil, f32 depth = 1.0f, u8 stencil = 0xFF )
    {
      (void)pRenderTarget;
      (void)pDepthStencil;

      ClearRenderTargetDepthStencil(rClearColor, depth, stencil);
    }
    // -- end 旧関数

		//! @brief レンダリングターゲットをクリアする
		inline static void ClearRenderTarget( const Color &rClearColor )
		{
      ClearRenderTarget(0U, rClearColor);
		}

		//! @brief 深度ステンシルをクリアする
		inline static void ClearDepthStencil( f32 depth = 1.0f, u8 stencil = 0xFF )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->ClearDepthStencil_( depth, stencil );
		}

		//! @brief レンダリングターゲットと深度ステンシルをクリアする
    inline static void ClearRenderTargetDepthStencil( const Color &rClearColor, f32 depth = 1.0f, u8 stencil = 0xFF )
    {
      ClearRenderTargetDepthStencil(0U, rClearColor, depth, stencil);
    }

		//! @brief レンダリングターゲットをクリアする：MRT用にインデックス指定付き
		inline static void ClearRenderTarget(u32 index, const Color &rClearColor )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->ClearRenderTarget_( index, rClearColor );
		}

		//! @brief レンダリングターゲットと深度ステンシルをクリアする：MRT用にインデックス指定付き
    inline static void ClearRenderTargetDepthStencil(u32 index, const Color &rClearColor, f32 depth = 1.0f, u8 stencil = 0xFF )
    {
      GFL_ASSERT( IsSceneDrawing() );
			s_Gp->ClearRenderTargetDepthStencil_( index, rClearColor, depth, stencil );
    }

		//! @brief ビューポートを設定する
		inline static void SetViewport(const u32 xp, const u32 yp, const u32 width, const u32 height)
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetViewport_( xp, yp, width, height);
		}
		//! @brief シザーを設定する
		inline static void SetScissor(const u32 xp, const u32 yp, const u32 width, const u32 height)
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetScissor_(xp, yp, width, height);
		}

		//! @brief 頂点シェーダーを切り替える
		inline static void SetVertexShader( const Shader* pShader )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetVertexShader_( pShader );
		}

		//! @brief ジオメトリシェーダーを切り替える
		inline static void SetGeometryShader( const Shader* pShader )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetGeometryShader_( pShader );
		}

		//! @brief ピクセルシェーダーを切り替える
		inline static void SetPixelShader( const Shader* pShader )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetPixelShader_( pShader );
		}

		//! @brief 頂点シェーダ定数設定
		static void SetVertexShaderConstant(
			u32								registerNo,	  //!< 最初の定数を格納する定数レジスタの番号 or 定数バッファ番号
			ConstantBuffer		*pBuffer			//!< 定数の配列へのポインタ
		)
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetVertexShaderConstant_( registerNo, pBuffer );
		}

		//! @brief ジオメトリシェーダ定数設定
		static void SetGeometryShaderConstant(
			u32								registerNo,	  //!< 最初の定数を格納する定数レジスタの番号 or 定数バッファ番号
			ConstantBuffer		*pBuffer			//!< 定数の配列へのポインタ
		)
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetGeometryShaderConstant_( registerNo, pBuffer );
		}

		//! @brief ピクセルシェーダ定数設定
		static void SetPixelShaderConstant(
			u32								registerNo,	  //!< 最初の定数を格納する定数レジスタの番号 or 定数バッファ番号
			ConstantBuffer		*pBuffer			//!< 定数の配列へのポインタ
		)
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetPixelShaderConstant_( registerNo, pBuffer );
		}

		//! @brief インデクスバッファを設定します
		static void SetIndices(const IndexBuffer* pIndices)
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetIndices_(pIndices);
		}

		//! @brief 頂点バッファをストリームに設定
		inline static void SetStreamSource(
			const u32 index,										//!< ストリーム番号
			const VertexBuffer* pVertexBuffer,	//!< 頂点バッファ
			const s32 offset = -1,							//!< 頂点バッファの先頭からのオフセット
			const s32 stride = -1								//!< １頂点のバイト幅
		)
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetStreamSource_( index, pVertexBuffer, offset, stride);
		}

		//! @brief 頂点ストリームにセットされた頂点を使って描画
		inline static void DrawPrimitive(
			PrimitiveType   primitive,		//!< プリミティブタイプ
			u32							start,				//!< 描画を始める頂点のインデクス
			u32							count					//!< 描画する頂点数
		)
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->DrawPrimitive_( primitive, start, count );
		}

		//! @brief 頂点ストリームにセットされた頂点とインデックスバッファを使って描画
		static void DrawIndexedPrimitive(
			PrimitiveType   primitive,		   //!< プリミティブタイプ
			u32							start,					 //!< インデクスバッファの描画開始オフセットインデクス
			u32							count						 //!< 描画する頂点数
		)
		{
			s_Gp->DrawIndexedPrimitive_( primitive, start, count );
		}

		//! @brief ブレンドステートの設定
		inline static void SetBlendState( BlendStateObject *pState )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetBlendState_( pState );
		}

		//! @brief ラスタライザステートの設定
		inline static void SetRasterizerState( RasterizerStateObject *pState )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetRasterizerState_( pState );
		}

		//! @brief 深度ステンシルステートの設定
		inline static void SetDepthStencilState( DepthStencilStateObject *pState )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetDepthStencilState_( pState );
		}

		//! @brief テクスチャの設定
		inline static void SetTexture( u32 texNo, const Texture *pTexture )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetTexture_( texNo, pTexture );
		}

		//! @brief サンプラステートの設定
		//! @note SetTexture後に行ってください。
		inline static void SetSamplerState( u32 texNo, const SamplerStateObject *pState )
		{
			GFL_ASSERT( IsSceneDrawing() );
			s_Gp->SetSamplerState_( texNo, pState );
		}

	//@}

	//! @name 処理負荷計測関係
	//@{
    // 1フレームにかかったGPUの総時間をマイクロ秒で取得
    inline static u64 GetTotalGPUTimeMicroSecond()
    {
      return s_Gp->GetTotalGPUTimeMicroSecond_();
    }

    // 1フレームにかかったCPUの総時間をマイクロ秒で取得
    inline static u64 GetTotalCPUTimeMicroSecond()
    {
      return s_Gp->GetTotalCPUTimeMicroSecond_();
    }
	//@}

	//! @name 画面サイズ取得関係
	//@{
    // 画面サイズ取得：幅
    inline static u32 GetDisplayWidth( DisplayDeviceHandle hDisplay )
    {
      return s_Gp->GetDisplayWidth_(hDisplay);
    }
    // 画面サイズ取得：高さ
    inline static u32 GetDisplayHeight( DisplayDeviceHandle hDisplay )
    {
      return s_Gp->GetDisplayHeight_(hDisplay);
    }
	//@}


private:

	GFGLBase(): Implement_(),
		m_pIGLGLCallBack( NULL )
	{
	}

	inline virtual ~GFGLBase(){}

	static GFGLBase					*s_Gp;
};

#if defined(GF_PLATFORM_WIN32)

#if defined(_WIN64)
	typedef		GFGLBase<windx11::WinDX11GL>					GFGL;
#else
	typedef		GFGLBase<winOpenGL::WinOpenGLGL>				GFGL;
#endif

#elif defined(NN_PLATFORM_CTR)
  typedef   GFGLBase<ctr::CTRGL>                  GFGL;

#endif

}}

#endif
