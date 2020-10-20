//=============================================================================
/**
 * @file    gfl2_CTRGL.h
 * @brief   描画低レベルＡＰＩのＣＴＲ実装
 * @author  takebe_makoto
 * @date    2014
 *
 *  2016.5.10   ishiguro  バッファサイズ取得系関数追加
 *  2016.5.5    ishiguro  デバッグ掲示関数追加
 *
 */
//=============================================================================
#ifndef GFLIB2_GRX_CTR_CTRGL_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRGL_H_INCLUDED
#pragma once

//#define GFX_CTR_PROFILING_ENABLED

#include <stdio.h>
#include <memory>

#include <nn/tpl.h>
#include <nn/ulcd.h>

#include <math/include/gfl2_Matrix34.h>
#include <math/include/gfl2_Matrix44.h>

#include <gfx/include/gfl2_types.h>
#include <util/include/gfl2_AutoComPtr.h>

#include <gfx/include/gfl2_CommonGL.h>
#include <gfx/include/gfl2_ImageData.h>
#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/gfl2_StateObject.h>
#include <gfx/include/gfl2_texture.h>

#include <gfx/include/CTR/gfl2_CTRVertexShader.h>
#include <gfx/include/CTR/gfl2_CTRPixelShader.h>
#include <gfx/include/CTR/gfl2_CTRSurface.h>
#include <gfx/include/CTR/gfl2_CTROcclusionQuery.h>
#include <gfx/include/CTR/gfl2_CTRTimerQuery.h>
#include <gfx/include/CTR/gfl2_CTRIndexBuffer.h>
#include <gfx/include/CTR/gfl2_CTRTexture.h>
#include <gfx/include/CTR/gfl2_CTRStateObject.h>
#include <gfx/include/CTR/gfl2_CTRDisplayList.h>

#include <gfx/include/CTR/gfl2_CTRCommandList.h>
#include <gfx/include/CTR/gfl2_CTRGPUProfiler.h>

namespace gfl2 { namespace gfx { namespace ctr {

//==============================================================================
/**
 * @class CTRGL
 * @brief CTR用グラフィックライブラリ(実装クラス)
 * @date  2014
 */
//==============================================================================
class CTRGL : public CommonGL
{
public:

  struct InitDesc
  {
		InitDesc()
		{
			commandBufferSize = 0;
			requestCount = 0;
			colorBufferAllocation = Pool::Default;
			depthBufferAllocation = Pool::Default;
		}

    u32			commandBufferSize;
    u32			requestCount;
		Pool		colorBufferAllocation;
		Pool		depthBufferAllocation;
  };

public:
	//機種ごとの拡張コードでユーザ−に与えたい物のみ、ここに出す。

	static void GLAssertCheck();

  // CTR用グラフィック初期化
  /*static void InitializeCTR(const InitDesc& desc)
  {
    s_Gp->InitCTR(desc);
  }*/

  static void InitializeCTR(IGLAllocator* pObj, const InitDesc& desc)
  {
    GLMemory::SetInstanceAllocator( pObj );

    s_Gp->InitCTR(desc);

    GLMemory::ClearInstanceAllocator();
  }

  // 立体視設定
  static void SetStereoModeEnable(bool isStereoMode)
  {
    // ステレオモード設定
    s_Gp->m_isStereoMode = isStereoMode;

    GLenum mode = (isStereoMode)
      ? NN_GX_DISPLAYMODE_STEREO
      : NN_GX_DISPLAYMODE_NORMAL;

    nngxSetDisplayMode(mode);
  }

  // 立体視状態取得
  static bool IsStereoModeEnable()
  {
    return s_Gp->m_isStereoMode;
  }
    
  // 立体視用の行列取得
  static void GetStereoMatrix(const gfl2::math::Matrix44& projectionMatrix, const gfl2::math::Matrix34& viewMatrix, f32 depthLevel, f32 factor, 
    gfl2::math::Matrix44* out_pProjL, gfl2::math::Matrix34* out_pViewL, gfl2::math::Matrix44* out_pProjR, gfl2::math::Matrix34* out_pViewR);


#if defined(GFX_CTR_PROFILING_ENABLED)
  // GPUのプロファイリング結果を取得
  static const CTRGPUProfiler::ProfilingResult& GetGPUProfilingResult()
  {
    return s_Gp->m_GPUProfiler.GetGPUProfileResult();
  }

  // GPUプロファイラー用の.orcsファイルを出力する：HIOの機能を使います
  static void ExportOrcsFileRequest(const char* pFileName)
  {
    s_Gp->m_GPUProfiler.SetOrcsName(pFileName);
    s_Gp->m_IsRequestExportOrcs = true;
  }
#endif

  // シングルコマンドリストモードか？
  static b32 IsSingleCommandListMode()
  {
    return s_Gp->m_isSingleCommandListMode;
  }

  // シングルコマンドリストモード設定
  static void SetSigleCommandListMode(b32 isSingleMode)
  {
    // 切り替え予約
    s_Gp->m_isRequestSingleCommandListMode = isSingleMode;
  }

  // コマンド追加：コピー
  static void AddCommandCopy(const u32* pCommand, u32 commandSize)
  {
    s_Gp->AddCommandCopy_(s_Gp->GetJumpHelper(), pCommand, commandSize / sizeof(bit32));
  }

  // コマンド追加：ジャンプ
  static void AddCommandJump(const u32* pCommand, u32 commandSize)
  {
    s_Gp->AddCommandJump_(s_Gp->GetJumpHelper(), pCommand, commandSize);
  }

  // ジャンプヘルパー取得
  static nn::gr::CommandBufferJumpHelper* GetJumpHelper()
  {
    return s_Gp->GetJumpHelper_();
  }

  // 現在のコマンドリスト取得
  static CTRCommandList* GetCurrentCommandList()
  {
    return s_Gp->GetCurrentCommandList_();
  }

  // コマンドリストとジャンプヘルパーの同期：前処理
  static void SyncCommandListAndJumpHelperBefore(CTRCommandList* pCommandList, nn::gr::CommandBufferJumpHelper* pJumpHelper)
  {
    s_Gp->SyncCommandListAndJumpHelperBefore_( pCommandList, pJumpHelper );
  }

  // コマンドリストとジャンプヘルパーの同期：終了処理
  static void SyncCommandListAndJumpHelperAfter(CTRCommandList* pCommandList, nn::gr::CommandBufferJumpHelper* pJumpHelper)
  {
    s_Gp->SyncCommandListAndJumpHelperAfter_( pCommandList, pJumpHelper );
  }

  // 全無効コマンド追加
  static void AddDisableAllCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
  {
    s_Gp->AddDisableAllCommand_( pJumpHelper );
  }

  //kawa20150428
  // レンダーステート取得
  static nn::gr::RenderState* GetRenderState()
  {
    return s_Gp->GetRenderState_();
  }

  // ダーティフラグが立っている全コマンドフラッシュ
  static void FlushAllDirtyCommand()
  {
    s_Gp->FlushAllDirtyCommand_( s_Gp->GetJumpHelper() );
  }

  //araki20150528
  // ディスプレイバッファアドレス取得
  static const void* GetDisplayBuffer_(const DisplayDeviceHandle hDisplay);

  //araki20150618
  // デプスステンシルバッファアドレス取得
  static uptr GetDepthStencilBufferStatic_(const DisplayDeviceHandle hDisplay);

  //araki20150618
  static u32 GetFrameBufferHeight_();

  //araki20150618
  static u32 GetFrameBufferWidth_();

  // カラーバッファ転送コマンドを追加
  // リニア形式に変換して転送します
  // pDestAddrは16バイトアラインで、あらかじめ十分な容量を確保しておいてください（256 x 512 縦長左下原点 ピクセルフォーマットはRGB）
  static void AddTransferLinearColorBufferCommand(void* pDestAddr, b32 includeAlpha = false);

  // カラーバッファ転送コマンドを追加
  // カラーバッファを元の形式のまま転送します（CTR：ブロック形式）
  // pDestAddrは16バイトアラインで、あらかじめ十分な容量を確保しておいてください（256 x 512 縦長左下原点 ピクセルフォーマットはRGBA）
  static void AddTransferColorBufferCommand(void* pDestAddr);

  // Surfaceクラス間でコピーを行うコマンドを追加します
  static void AddReadColorBufferToTextureCommand(Texture* pTexture);

  // VRAMへのDMA転送コマンドを追加
  static void AddVramDmaCommand(const void* pSourceAddr, void* pDestAddr, u32 size);

  // GPUが実行中か確認
  static bool IsGPURunning();

#if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)

  //! @brief コマンドバッファの使用量取得関数
  //! @return コマンドバッファの使用量(GFGL未初期化時は0を返す)
  //! @attention GFGL::SwapDisplay関数後など描画コマンドを積み終わった後で取得してください。
  static u32 GetCommandListUsedBufferSize();

  //! @brief コマンドバッファサイズ取得関数
  //! @return コマンドバッファサイズ(GFGL未初期化時は0を返す)
  static u32 GetCommandListMaxBufferSize();

  //! @brief コマンドリクエスト使用数取得関数
  //! @return コマンドリクエスト使用数(GFGL未初期化時は0を返す)
  //! @attention GFGL::SwapDisplay関数後など描画コマンドを積み終わった後で取得してください。
  static u32 GetCommandListUsedRequestCount();

  //! @brief コマンドリクエスト使用最大数取得関数
  //! @return コマンドリクエスト使用最大数(GFGL未初期化時は0を返す)
  static u32 GetCommandListMaxRequestCount();

#endif // GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP

protected:

	CTRGL();
	virtual ~CTRGL();

	//! @name ディスプレイデバイス関連
	//@{
		DisplayDeviceHandle CreateDisplay_( DisplayMode displayMode, u32 width, u32 height, b32 isFullScreen, Format colorFormat, Format depthStencilFormat, MultiSample multiSample );

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
		//! @brief ディスプレイリストの記録開始
		void BeginDisplayList_(DisplayList* pDisplayList);
		//! @brief ディスプレイリストの記録終了
		void EndDisplayList_(DisplayList* pDisplayList);
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
    static Texture* CreateTextureFromSurface_( const Surface* pSurface );
	//@}

	//! @name 描画命令関連
	//! @attention  描画系メソッドは必ず BeginScene() 〜 EndScene() の間で呼ばなければいけない
	//@{
    b32 BeginTimerQuery_( const TimerQuery* pTimerQuery);

    void EndTimerQuery_( const TimerQuery* pTimerQuery);

		b32 BeginOcclusionQuery_( const OcclusionQuery* pOcclusionQuery );

		void EndOcclusionQuery_( const OcclusionQuery* pOcclusionQuery );

		b32 BeginConditionalRender_( const OcclusionQuery* pOcclusionQuery );

		void EndConditionalRender_( const OcclusionQuery* pOcclusionQuery );

		b32 BeginScene_();

		void EndScene_();

		void Finish_();

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
#if defined(GFX_CTR_PROFILING_ENABLED)
      return m_GPUProfiler.GetTotalGPUTimeMicroSeconds();
#else
      return 0;
#endif
    }

    // 1フレームにかかったCPUの総時間をマイクロ秒で取得
    u64 GetTotalCPUTimeMicroSecond_()
    {
#if defined(GFX_CTR_PROFILING_ENABLED)
      return m_GPUProfiler.GetTotalCPUTimeMicroSeconds();
#else
      return 0;
#endif
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

  void InitCTR(const InitDesc& desc);
#if GFL_DEBUG
  static void CommandListTimeoutCallback();
#endif

  // ダーティフラグ
  enum DirtyFlag
  {
    DIRTY_FLAG_DEPTH_TEST = 1 << 0,
    DIRTY_FLAG_STENCIL_TEST = 1 << 1,
    DIRTY_FLAG_SCISSOR = 1 << 2,
    DIRTY_FLAG_VIEWPORT = 1 << 3,
    DIRTY_FLAG_VERTEX_SHADER = 1 << 4,
    DIRTY_FLAG_PIXEL_SHADER = 1 << 5,
    DIRTY_FLAG_ALPHA_TEST = 1 << 6,
    DIRTY_FLAG_BLEND = 1 << 7,
    DIRTY_FLAG_MASK = 1 << 8,
    DIRTY_FLAG_SCISOR = 1 << 9,
    DIRTY_FLAG_WBUFFER = 1 << 10,
    DIRTY_FLAG_CULLING_TEST = 1 << 11,
    DIRTY_FLAG_TEXTURE_0 = 1 << 12,
    DIRTY_FLAG_TEXTURE_1 = 1 << 13,
    DIRTY_FLAG_TEXTURE_2 = 1 << 14,
    DIRTY_FLAG_TEXTURE_FUNC = 1 << 15,

    DIRTY_INDEX_TEXTURE_0 = 12,
  };

  // ダーティフラグクリア
  void ClearDirty()
  {
    m_DirtyFlags = 0;
  }
  // ダーティフラグを立てる
  void SetDirty(DirtyFlag flag)
  {
    m_DirtyFlags |= flag;
  }
  // ダーティフラグを下ろす
  void ResetDirty(DirtyFlag flag)
  {
    m_DirtyFlags &= ~flag;
  }
  // ダーティフラグが立っているか？
  b32 IsDirty(DirtyFlag flag)
  {
    return m_DirtyFlags & flag;
  }
  // ダーティフラグを立てる：テクスチャ用
  void SetDirtyTexture(u32 texNo)
  {
    this->SetDirty(static_cast<DirtyFlag>(1 << (DIRTY_INDEX_TEXTURE_0 + texNo)));
  }
  // ダーティフラグを下ろす：テクスチャ用
  void ResetDirtyTexture(u32 texNo)
  {
    this->ResetDirty(static_cast<DirtyFlag>(1 << (DIRTY_INDEX_TEXTURE_0 + texNo)));
  }
  // ダーティフラグが立っているか？：テクスチャ用
  b32 IsDirtyTexture(u32 texNo)
  {
    return this->IsDirty(static_cast<DirtyFlag>(1 << (DIRTY_INDEX_TEXTURE_0 + texNo)));
  }

  // 現在のコマンドリスト取得
  CTRCommandList* GetCurrentCommandList_()
  {
    return &m_CommandLists[m_CurrentCommandListNo];
  }

  // 前回のコマンドリスト取得
  CTRCommandList* GetPrevCommandList()
  {
    return &m_CommandLists[m_CurrentCommandListNo ^ 1];
  }

  // コマンドリストのスワップ
  void SwapCommandList()
  {
    m_CurrentCommandListNo ^= 1;
  }

  // ジャンプヘルパー取得
  nn::gr::CommandBufferJumpHelper* GetJumpHelper_()
  {
    return &m_GrHelper;
  }

  // 全ステートクリア
  void ClearAllState()
  {
    // ダーティフラグを全クリア
    this->ClearDirty();

    // フレームバッファは不要

    m_pCurrentVertexShader = NULL;
    m_pCurrentPixelShader = NULL;
	  m_pCurrentIndexBuffer = NULL;
    for (u32 i = 0; i < TEXTURE_MAX; ++i)
    {
      m_pCurrentTextureList[i] = NULL;
      m_pCurrentSamplerList[i] = NULL;
    }
  }

  // ダーティフラグが立っている全コマンドフラッシュ
  void FlushAllDirtyCommand_(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // 全無効コマンド追加
  void AddDisableAllCommand_(nn::gr::CommandBufferJumpHelper* pJumpHelper);
  // フレームバッファ設定コマンド追加
  void AddFrameBufferCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper, u32 targetBit = nn::gr::FrameBuffer::COLOR_BUFFER_BIT | nn::gr::FrameBuffer::DEPTH_STENCIL_BUFFER_BIT);

  // バッファクリアコマンド追加
  void AddClearBufferCommand(CTRCommandList* pCommandList, nn::gr::CommandBufferJumpHelper* pJumpHelper, u32 grClearBit);

  // ビューポート設定コマンド追加
  void AddViewportCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // シザー設定コマンド追加
  void AddScissorCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // カリングテスト設定コマンド追加
  void AddCullingTestCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // ブレンド設定コマンド追加
  void AddBlendCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // アルファテスト設定コマンド追加
  void AddAlphaTestCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // ステンシルテスト設定コマンド追加
  void AddStencilTestCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // デプステスト設定コマンド追加
  void AddDepthTestCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // Wバッファ設定コマンド追加
  void AddWBufferCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // マスク設定コマンド追加
  void AddMaskCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // バーテクスシェーダ設定コマンド追加
  void AddVertexShaderCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // ピクセルシェーダ設定コマンド追加
  void AddPixelShaderCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // テクスチャ0設定コマンド追加
  void AddTexture0Command(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // テクスチャ1設定コマンド追加
  void AddTexture1Command(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // テクスチャ2設定コマンド追加
  void AddTexture2Command(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // テクスチャ関数設定コマンド追加
  void AddTextureFuncCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // 描画コマンド追加
  void AddDrawCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // コマンド追加：コピー
  void AddCommandCopy_(nn::gr::CommandBufferJumpHelper* pJumpHelper, const u32* pCommand, u32 commandSize);

  // コマンド追加：ジャンプ
  void AddCommandJump_(nn::gr::CommandBufferJumpHelper* pJumpHelper, const u32* pCommand, u32 commandSize);

  // コマンドのカレントポインタ取得
  u32* GetCurrentCommandPtr_(nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // コマンドのカレントポインタ設定
  void SetCurrentCommandPtr_(nn::gr::CommandBufferJumpHelper* pJumpHelper, u32* pCurrent);

  // コマンドリストとジャンプヘルパーの同期：前処理
  void SyncCommandListAndJumpHelperBefore_(CTRCommandList* pCommandList, nn::gr::CommandBufferJumpHelper* pJumpHelper);

  // コマンドリストとジャンプヘルパーの同期：終了処理
  void SyncCommandListAndJumpHelperAfter_(CTRCommandList* pCommandList, nn::gr::CommandBufferJumpHelper* pJumpHelper);

  //kawa20150428
  // レンダーステート取得
  nn::gr::RenderState* GetRenderState_()
  {
    return &m_GrRenderState;
  }

#if GFL_DEBUG
  //----------------------------------------------------------------------------
  /**
   *   @brief デバッグ掲示関数(ライブラリの使用状況報告)
  */
  //-----------------------------------------------------------------------------
  void DebugDump();
#endif

private:

  static const int MRT_MAX = 1;
	static const int TEXTURE_MAX = 3;
  static const int COMMAND_LIST_MAX = 2;

  bool                    m_IsInitialized;

  // カレントのオブジェクト
	CTRSurface*             m_ActgiveRenderTarget;
	CTRSurface*             m_ActgiveDepthStencil;
	CTRVertexShader*        m_pCurrentVertexShader;
  CTRPixelShader*         m_pCurrentPixelShader;
	CTRIndexBuffer*         m_pCurrentIndexBuffer;
	CTRTexture*             m_pCurrentTextureList[TEXTURE_MAX];
	CTRSamplerStateObject*  m_pCurrentSamplerList[TEXTURE_MAX];

  // デフォルトのフレームバッファ
  CTRSurface*             m_pDefaultColorBuffer;
  CTRSurface*             m_pDefaultDepthStencilBuffer;

  // コマンドリスト
  CTRCommandList          m_CommandLists[COMMAND_LIST_MAX];
  u32                     m_CurrentCommandListNo;
  u32*                    m_pCommandWriteBeginAddr;
  b32                     m_isSingleCommandListMode;
  b32                     m_isRequestSingleCommandListMode;

  // ディスプレイバッファ
  void*                   m_pSwapDisplayBufferList[CtrDisplayNo::NumberOf];
  DisplayDeviceHandle     m_pSwapDisplayHandleList[CtrDisplayNo::NumberOf];
  b32                     m_FirstSwapFlag;

  // nn::gr系
  nn::gr::FrameBuffer     m_GrFrameBuffer;
  nn::gr::RenderState     m_GrRenderState;
  nn::gr::Scissor         m_GrScissor;
  nn::gr::ShaderLite*     m_pGrShader;
  nn::gr::Viewport        m_GrViewport;
  nn::gr::Texture         m_GrTexture;

  nn::gr::CommandBufferJumpHelper m_GrHelper;

  // ダーティフラグ
  u32 m_DirtyFlags;

  // ステレオカメラ
  nn::ulcd::StereoCamera m_StereoCamera;

  // ステレオモード
  b32                   m_isStereoMode;
  u32                   m_StereoEnableCounter;

#if defined(GFX_CTR_PROFILING_ENABLED)
  // GPUプロファイラ
  CTRGPUProfiler  m_GPUProfiler;
  b32             m_IsRequestExportOrcs;
#endif

  // インスタンス
	static CTRGL					*s_Gp;
};

}}}//gfl2::gfx::ctr

#endif//  GFLIB2_GRX_CTR_CTRGL_H_INCLUDED
