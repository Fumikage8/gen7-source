//=============================================================================
/**
 * @file    gfl2_CTRGL.cpp
 * @brief   描画低レベルＡＰＩのＣＴＲ実装
 * @author  takebe_makoto
 * @date    2014
 *
 *  2016.5.10   ishiguro  バッファサイズ取得系関数追加
 *  2016.5.5    ishiguro  デバッグ掲示関数追加
 *
 */
//=============================================================================
//#include <string>
#include <stdio.h>
#include <string.h>
#define NOMINMAX
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRGL.h>
#include <gfx/include/CTR/gfl2_CTRConstantBuffer.h>
#include <gfx/include/CTR/gfl2_CTRSurface.h>
#include <gfx/include/CTR/gfl2_CTRVertexShader.h>
#include <gfx/include/CTR/gfl2_CTRGeometryShader.h>
#include <gfx/include/CTR/gfl2_CTRPixelShader.h>
#include <gfx/include/CTR/gfl2_CTRVertexBuffer.h>
#include <gfx/include/CTR/gfl2_CTRIndexBuffer.h>
#include <gfx/include/CTR/gfl2_CTRStateObject.h>
#include <gfx/include/CTR/gfl2_CTRTexture.h>
#include <gfx/include/CTR/gfl2_CTRTimerQuery.h>
#include <gfx/include/CTR/gfl2_CTROcclusionQuery.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <nn/os.h>
#if defined(GFX_CTR_PROFILING_ENABLED)
#include <nn/prof.h>
#endif


namespace gfl2 { namespace gfx { namespace ctr {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  グローバル
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
CTRGL				*CTRGL::s_Gp = NULL;

// テクスチャとして使うので２のべき乗に合わせておく
static const u32 FRAMEBUFFER_WIDTH = 256;
static const u32 FRAMEBUFFER_HEIGHT = 512;

// nngx用のメモリ確保関数
static GLvoid* GxAllocate(GLenum type, GLenum usage, GLuint name, GLsizei size)
{
  // nngxInitialize()にしか使用しない
  // その他のバッファは全て自前管理している

#if 0
  // アライメントメモ
  NN_GX_MEM_TEXTURE テクスチャ （2D、環境マップ） 全フォーマットとも 128 Byte
  NN_GX_MEM_VERTEXBUFFER 頂点バッファ 頂点属性によって異なります。 4 Byte（GLfloat 型） 2 Byte（GLshort 型、GLushort 型） 1 Byte（GLbyte 型、GLubyte 型）
  NN_GX_MEM_RENDERBUFFER カラーバッファ 64 Byte デプスバッファ （ステンシルバッファ） 32 Byte（16 bit デプスの場合） 96 Byte（24 bit デプスの場合） 64 Byte（24 bit デプス＋8 bit ステンシルの場合）
  NN_GX_MEM_DISPLAYBUFFER ディスプレイバッファ 16 Byte
  NN_GX_MEM_COMMANDBUFFER 3D コマンドバッファ 16 Byte
  NN_GX_MEM_SYSTEM システム用バッファ 4 Byte（確保サイズが 4 の倍数） 2 Byte（確保サイズが 4 の倍数ではない 2 の倍数） 1 Byte（上記以外）
#endif

  // @attention アラインメント取得関数を作成するのが望ましいが、raw APIはこの関数を通らないのでとりあえずは不要
  u32 alignment = 128;

  // アロケータがセットされていない
  if (gfl2::gfx::GLMemory::IsAllocatorEmpty())
  {
    return NULL;
  }

  // デバイスメモリ
  if (type == NN_GX_MEM_FCRAM)
  {
    return gfl2::gfx::GLMemory::GetAllocator()->SystemMemoryMallocAlign(size, alignment);
  }

  // VRAMA, VRAMB
  return gfl2::gfx::GLMemory::GetAllocator()->LocalMemoryMalloc(size, alignment);
}

// nngx用のメモリ開放関数
static GLvoid GxDeallocate(GLenum type, GLenum usage, GLuint name, GLvoid* ptr)
{
  // アロケータがセットされていない
  if (gfl2::gfx::GLMemory::IsAllocatorEmpty())
  {
    return;
  }

  // デバイスメモリ
  if (type == NN_GX_MEM_FCRAM)
  {
    gfl2::gfx::GLMemory::GetAllocator()->SystemMemoryFree(ptr);
  }
  else
  {
    // VRAMA, VRAMB
    gfl2::gfx::GLMemory::GetAllocator()->LocalMemoryFree(ptr);
  }
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  ディスプレイウィンドウクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class DisplayWindow: public GLMemory
{
public:
  static const u32 DISPLAY_BUFFER_COUNT = 2;    // ディスプレイバッファ数：ダブルバッファ

public:
  void*               m_pDisplayBufferList[DISPLAY_BUFFER_COUNT];
  u32                 m_CurrentDisplayBuffer;

  u32                 m_Width;
  u32                 m_Height;

  DisplayMode         m_DisplayMode;
  CTRSurface	      *m_pColor;
  CTRSurface		  *m_pDepth;

	DisplayWindow() :
    GLMemory(),
    m_CurrentDisplayBuffer(0),
    m_Width(0),
    m_Height(0),
    m_DisplayMode(DisplayMode::Default),
    m_pColor(NULL),
    m_pDepth(NULL)
	{
    for (u32 i = 0; i < DISPLAY_BUFFER_COUNT; ++i)
    {
      m_pDisplayBufferList[i] = NULL;
    }
	}

	b32 Init(CTRSurface *pColor, CTRSurface *pDepth, DisplayMode displayMode, u32 width, u32 height)
	{
	  m_pColor = pColor;
	  m_pDepth = pDepth;

      m_DisplayMode = displayMode;

      m_Width = width;
      m_Height = height;

      // ディスプレイバッファ作成
      m_CurrentDisplayBuffer = 0;

      for (u32 i = 0; i < DISPLAY_BUFFER_COUNT; ++i)
      {
        m_pDisplayBufferList[i] = gfl2::gfx::GLMemory::Malloc(width * height * 3, 128); // GL_RGB8_OES
      }

	  return true;
	}

	virtual ~DisplayWindow( void )
	{
      for (u32 i = 0; i < DISPLAY_BUFFER_COUNT; ++i)
      {
        gfl2::gfx::GLMemory::Free(m_pDisplayBufferList[i]);
      }

	  delete m_pColor;
	  delete m_pDepth;
	}

  void* GetDisplayBuffer()
  {
    return m_pDisplayBufferList[m_CurrentDisplayBuffer];
  }
  void* GetDisplayBufferPrev()
  {
    return m_pDisplayBufferList[m_CurrentDisplayBuffer ^ 1];
  }

  u32 GetWidth() const
  {
    return m_Width;
  }

  u32 GetHeight() const
  {
    return m_Height;
  }

  u32 GetDisplayNo() const
  {
    return m_DisplayMode - DisplayMode::CTR_LEFT;
  }

  void Swap()
  {
    m_CurrentDisplayBuffer ^= 1;
  }


};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  CTRGLクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CTRGL::GLAssertCheck()
{
	GLuint err = glGetError();

	if ( err != GL_NO_ERROR )
	{
	  GFL_RELEASE_PRINT( "gl err = 0x%X", err );
      GFL_ASSERT( 0 );
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
CTRGL::CTRGL() : CommonGL(),
  m_IsInitialized(false),
  m_ActgiveRenderTarget(NULL),
  m_ActgiveDepthStencil(NULL),
  m_pCurrentVertexShader(NULL),
  m_pCurrentPixelShader(NULL),
  m_pCurrentIndexBuffer(NULL),
  m_pDefaultColorBuffer(NULL),
  m_pDefaultDepthStencilBuffer(NULL),
  m_CurrentCommandListNo(0),
  m_pCommandWriteBeginAddr(NULL),
  m_isSingleCommandListMode(false),
  m_isRequestSingleCommandListMode(false),
  m_FirstSwapFlag(true),
  m_pGrShader(NULL),
  m_GrHelper(NULL),
  m_DirtyFlags(0),
  m_isStereoMode(false),
  m_StereoEnableCounter(0)
#if defined(GFX_CTR_PROFILING_ENABLED)
  ,
  m_IsRequestExportOrcs(false)
#endif
{
  for( u32 i = 0; i < TEXTURE_MAX; ++i )
  {
  	m_pCurrentTextureList[i] = NULL;
    m_pCurrentSamplerList[i] = NULL;
  }

  for (u32 i = 0; i < CtrDisplayNo::NumberOf; ++i)
  {
    m_pSwapDisplayBufferList[i] = NULL;
    m_pSwapDisplayHandleList[i] = NULL;
  }

  s_Gp = this;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
CTRGL::~CTRGL()
{
	s_Gp = NULL;
}

#if GFL_DEBUG
void CTRGL::CommandListTimeoutCallback()
{
  static const char* s_nextRequestTypeList[] =
  {
    "NN_GX_CMDLIST_REQTYPE_DMA", // #define NN_GX_CMDLIST_REQTYPE_DMA       0x0310
    "NN_GX_CMDLIST_REQTYPE_RUN3D", // #define NN_GX_CMDLIST_REQTYPE_RUN3D     0x0311
    "NN_GX_CMDLIST_REQTYPE_FILLMEM", // #define NN_GX_CMDLIST_REQTYPE_FILLMEM   0x0312
    "NN_GX_CMDLIST_REQTYPE_POSTTRANS", // #define NN_GX_CMDLIST_REQTYPE_POSTTRANS 0x0313
    "NN_GX_CMDLIST_REQTYPE_COPYTEX", // #define NN_GX_CMDLIST_REQTYPE_COPYTEX   0x0314
  };
  static const char* s_hwStateList[] =
  {
    "[0]:トライアングルセットアップがビジーの場合1",
    "[1]:ラスタライゼーションモジュールがビジーの場合1",
    "[2]:テクスチャユニットがビジーの場合1",
    "[3]:フラグメントライティングがビジーの場合1",
    "[4]:テクスチャコンバイナがビジーの場合1",
    "[5]:カラーアップデータがビジーの場合1",
    "[6]:カラーアップデータがビジーの場合1",
    "[7]:アーリーデプステストモジュールがビジーの場合1",
    "[8]:コマンドバッファおよび頂点アレイをロードするモジュールの出力がビジーの場合1",
    "[9]:コマンドバッファおよび頂点アレイをロードするモジュールの入力がビジーの場合1",
    "[10]:レジスタ0x229の[1:0]が0でない場合1",
    "[11]:頂点プロセッサ0(ジオメトリシェーダプロセッサ兼用)がビジーの場合1",
    "[12]:頂点プロセッサ1がビジーの場合1",
    "[13]:頂点プロセッサ2がビジーの場合1",
    "[14]:頂点プロセッサ3がビジーの場合1",
    "[15]:レジスタ0x252の[1:0]に1が設定されている場合1",
    "[16]:ポスト頂点キャッシュがビジーの場合1",
    "[17]:上画面LCDのFIFOアンダーランエラー発生の場合1",
    "[18]:下画面LCDのFIFOアンダーランエラー発生の場合1",
    "[19]:メモリフィルが実行中の場合1",
    "[20]:ポスト転送が実行中の場合1",
  };

  GFL_RELEASE_PRINT("--------------------------------------------\n");
  GFL_RELEASE_PRINT("Error!! CommandList Timeout!!\n");

  CTRCommandList* pCommandList = (s_Gp->IsSingleCommandListMode())
    ? s_Gp->GetCurrentCommandList_()
    : s_Gp->GetPrevCommandList();

  bit32 hwState = pCommandList->GetHWState();
  GFL_RELEASE_PRINT("HW_STATE : 0x%08X\n", hwState);
  for (u32 i = 0; i <= 20; ++i)
  {
    if (hwState & (1 << i))
    {
      GFL_RELEASE_PRINT("%s\n", s_hwStateList[i]);
    }
  }

  u32* topBufferAddr = pCommandList->GetTopBufferAddr();
  u32* topRequestAddr = pCommandList->GetTopRequestAddr();
  GFL_RELEASE_PRINT("TOP_BUFFER_ADDR : %P\n", topBufferAddr);
  GFL_RELEASE_PRINT("TOP_REQUEST_ADDR : %P\n", topRequestAddr);

  u32 maxBufferSize = pCommandList->GetMaxBufferSize();
  u32 maxRequestCount = pCommandList->GetMaxRequestCount();
  GFL_RELEASE_PRINT("MAX_BUFFER_SIZE : 0x%08X\n", maxBufferSize);
  GFL_RELEASE_PRINT("MAX_REQUEST_COUNT : %u\n", maxRequestCount);

  u32 usedBufferSize = pCommandList->GetUsedBufferSize();
  u32 usedRequestCount = pCommandList->GetUsedRequestCount();
  GFL_RELEASE_PRINT("USED_BUFFER_SIZE : 0x%08X\n", usedBufferSize);
  GFL_RELEASE_PRINT("USED_REQUEST_COUNT : %u\n", usedRequestCount);

  u32* currentBuferAddr = pCommandList->GetCurrentBufferAddr();
  GFL_RELEASE_PRINT("CURRENT_BUFFER_ADDR : %P\n", currentBuferAddr);

  u32 runBufferSize = pCommandList->GetRunBufferSize();
  u32 runRequestCount = pCommandList->GetRunRequestCount();
  GFL_RELEASE_PRINT("RUN_BUFFER_SIZE : 0x%08X\n", runBufferSize);
  GFL_RELEASE_PRINT("RUN_REQUEST_COUNT : %u\n", runRequestCount);

  s32 nextRequestType = pCommandList->GetNextRequestType();
  if (nextRequestType - NN_GX_CMDLIST_REQTYPE_DMA >= 0)
  {
    GFL_RELEASE_PRINT("NEXT_REQUEST_TYPE : 0x%08X, %s\n", nextRequestType, s_nextRequestTypeList[nextRequestType - NN_GX_CMDLIST_REQTYPE_DMA]);
  }

  u32 nextBufferSize = pCommandList->GetNextBufferSize();
  GFL_RELEASE_PRINT("NEXT_BUFFER_SIZE : %u\n", nextBufferSize);
  
  GFL_RELEASE_PRINT("NEXT_BUFFER_Addr : %P\n", nextBufferSize);

  GFL_RELEASE_PRINT("--------------------------------------------\n");

  // GPUコマンドのダンプ
#if defined(GFX_CTR_PROFILING_ENABLED)
  {
    GFL_RELEASE_PRINT("GPUコマンドリストをダンプします。この処理には数分かかることがあります。\n");

    s_Gp->m_GPUProfiler.SetOrcsName("gpu_profiler.orcs");
    s_Gp->m_GPUProfiler.ExportOrcs(pCommandList);

    GFL_RELEASE_PRINT("出力完了しました。\n");
    GFL_RELEASE_PRINT("gpu_profiler.orcsをGPUデバッガに読み込ませエラーが無いかご確認ください。\n");
  }
#endif

  GFL_ASSERT(0);
}
#endif

void CTRGL::InitCTR(const InitDesc& initDesc)
{
  // CTR用グラフィック初期化
  GLboolean result = nngxInitialize(GxAllocate, GxDeallocate);
  GFL_ASSERT(result);

  // コマンドリスト初期化
  for (int i = 0; i < COMMAND_LIST_MAX; ++i)
  {
    CTRCommandList* pCommandList = &m_CommandLists[i];

    // コマンド用バッファ確保
    void* pCommandBuffer = gfl2::gfx::GLMemory::Malloc(initDesc.commandBufferSize, 128);

    // リクエスト用バッファ確保
    u32 requestSize = pCommandList->GetRequestSize(initDesc.requestCount);
    void* pRequestBuffer = gfl2::gfx::GLMemory::Malloc(requestSize, 128);

    pCommandList->Storage(initDesc.commandBufferSize, pCommandBuffer, initDesc.requestCount, pRequestBuffer);

    // クリア
    pCommandList->Clear();
  }

#if GFL_DEBUG
  // タイムアウト設定
  CTRCommandList::SetTimeout(nn::os::Tick::TICKS_PER_SECOND * 5, CTRGL::CommandListTimeoutCallback);
#endif

  // ジャンプヘルパー
  m_GrHelper = nn::gr::CommandBufferJumpHelper(this->GetCurrentCommandList_()->GetCurrentBufferAddr());

  // 全機能無効コマンド追加
  this->AddDisableAllCommand_(this->GetJumpHelper());

  // フレームバッファ作成
  Surface::SurfaceDesc				desc;

  desc.width = FRAMEBUFFER_WIDTH;
  desc.height = FRAMEBUFFER_HEIGHT;
  desc.depth = 1;
  desc.multiSample = gfx::MultiSample::None;

  // カラーバッファ作成
  {
		desc.pool = initDesc.colorBufferAllocation;
    desc.usage = Usage::RenderTarget;
    desc.format = gfx::Format::A8R8G8B8;

    m_pDefaultColorBuffer = new CTRSurface( desc );
  }

  // デプスステンシルバッファ作成
  {
		desc.pool = initDesc.depthBufferAllocation;
    desc.usage = Usage::DepthStencil;
    desc.format = gfx::Format::D24S8;

    m_pDefaultDepthStencilBuffer = new CTRSurface( desc );
  }

  // ステレオカメラ初期化
  m_StereoCamera.Initialize();

  // ステレオモード初期化
  SetStereoModeEnable(false);
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
DisplayDeviceHandle CTRGL::CreateDisplay_( DisplayMode displayMode, u32 width, u32 height, b32 isFullScreen, Format colorFormat, Format depthStencilFormat, MultiSample multiSample )
{
	// フレームバッファ作成
  Surface::SurfaceDesc				desc;

  desc.pool = Pool::Managed;

  switch( displayMode ){
  case DisplayMode::CTR_LEFT:
  case DisplayMode::CTR_RIGHT:
    desc.width = nn::gx::DISPLAY0_HEIGHT;
    desc.height = nn::gx::DISPLAY0_WIDTH;
    break;
  case DisplayMode::CTR_DOWN:
    desc.width = nn::gx::DISPLAY1_HEIGHT;
    desc.height = nn::gx::DISPLAY1_WIDTH;
    break;
  }

  desc.bufferWidth = FRAMEBUFFER_WIDTH;
  desc.bufferHeight = FRAMEBUFFER_HEIGHT;
  
  desc.depth = 1;
  desc.multiSample = gfx::MultiSample::None;

  CTRSurface				*pColor;
  // カラーバッファ作成
  {
    desc.usage = Usage::RenderTarget;
    desc.format = gfx::Format::A8R8G8B8;

    pColor = new CTRSurface( desc, (void*)m_pDefaultColorBuffer->GetHandle().colorBuffer.virtualAddr );
  }

  CTRSurface				*pDepth;
  // デプスステンシルバッファ作成
  {
    desc.usage = Usage::DepthStencil;
    desc.format = gfx::Format::D24S8;

    pDepth = new CTRSurface( desc, (void*)m_pDefaultDepthStencilBuffer->GetHandle().depthStencilBuffer.virtualAddr );
  }

  DisplayWindow* pDisplayWindow = new DisplayWindow();
  pDisplayWindow->Init( pColor, pDepth, displayMode, desc.width, desc.height);

  return pDisplayWindow;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void CTRGL::DestroyDisplay_( DisplayDeviceHandle hDisplay )
{
  DisplayWindow* const pDisplay = static_cast< DisplayWindow* >( hDisplay );
  if ( pDisplay == NULL )
  { 
    return;
  }
  delete pDisplay;
}

//----------------------------------------------------------------------------
/**
 *	@brief  バックバッファを取得する
 */
//-----------------------------------------------------------------------------
Surface* CTRGL::GetBackBuffer_( DisplayDeviceHandle hDisplay )
{
	DisplayWindow				*pDisplayWindow = (DisplayWindow*)hDisplay;
	return pDisplayWindow->m_pColor;
}

//----------------------------------------------------------------------------
/**
 *	@brief  深度ステンシルバッファを取得する
 */
//-----------------------------------------------------------------------------
Surface* CTRGL::GetDepthStencilBuffer_( DisplayDeviceHandle hDisplay )
{
	DisplayWindow				*pDisplayWindow = (DisplayWindow*)hDisplay;
	return pDisplayWindow->m_pDepth;
}

//----------------------------------------------------------------------------
/**
 *	@brief  深度ステンシルバッファを取得する
 */
//-----------------------------------------------------------------------------
Texture* CTRGL::GetDepthStencilTexture_( DisplayDeviceHandle hDisplay )
{
	return NULL;//@attention CTR版は対応しない
}

//----------------------------------------------------------------------------
/**
 *	@brief  サーフェイスを作成する
 */
//-----------------------------------------------------------------------------
Surface* CTRGL::CreateSurface_( const Surface::SurfaceDesc &rDesc )
{
	Surface* pSurface = new CTRSurface( rDesc );

	return pSurface;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void CTRGL::TransferDisplay_()
{
	DisplayWindow* const pDisplay = static_cast< DisplayWindow* >( m_pActiveDisplay );

  GLsizei blockSize = 8;

  void* displayBuffer = pDisplay->GetDisplayBuffer();

  u32 displayWidth = pDisplay->GetWidth();    // 400
  u32 displayHeight = pDisplay->GetHeight();  // 240の順番になるはず

  u32 frameBufferWidth = FRAMEBUFFER_HEIGHT;   // 256
  u32 frameBufferHeight = FRAMEBUFFER_WIDTH;   // 512の順番になるはずなので入れ替える

  // viewportは左下原点、colorBufferは左上原点
  // そのまま転送すると上に空白ができるので転送開始アドレスをずらす
  u32 offset = frameBufferHeight * (frameBufferWidth - displayWidth) * 4; // GL_RGBA8_OES

  // ディスプレイバッファに転送 ブロック形式からリニア形式へ -> B2L
  this->GetCurrentCommandList_()->AddB2LTransferCommand(
    reinterpret_cast<GLvoid*>(m_GrFrameBuffer.colorBuffer.virtualAddr + offset),
    frameBufferHeight,  // 256
    frameBufferWidth,   // 512の順番になるはず
    GL_RGBA8_OES,
    displayBuffer,
    displayHeight,  // 240
    displayWidth,   // 400の順番になるはず
    GL_RGB8_OES,
    NN_GX_ANTIALIASE_NOT_USED,
    false,
    blockSize
    );

  if (m_isSingleCommandListMode)
  {
    // シングルバッファ
    m_pSwapDisplayBufferList[pDisplay->GetDisplayNo()] = displayBuffer;
    m_pSwapDisplayHandleList[pDisplay->GetDisplayNo()] = pDisplay;
  }
  else
  {
    // ダブルバッファ

    // 前回の描画結果をスワップ予約
    m_pSwapDisplayBufferList[pDisplay->GetDisplayNo()] = pDisplay->GetDisplayBufferPrev();
    m_pSwapDisplayHandleList[pDisplay->GetDisplayNo()] = pDisplay;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
bool CTRGL::SwapDisplay_WaitGPUCommandDone_()
{
	GLAssertCheck();

#if defined(GFX_CTR_PROFILING_ENABLED)
  // CPU計測終了
  m_GPUProfiler.EndCPUTotalTime();
#endif

  CTRCommandList* pCommandList = this->GetCurrentCommandList_();
  
#if defined(GFX_CTR_PROFILING_ENABLED)
  // デバッグ用にコマンドリスト出力
  if (m_IsRequestExportOrcs)
  {
    m_IsRequestExportOrcs = false;

    m_GPUProfiler.SetOrcsName("gpu_profiler.orcs");
    m_GPUProfiler.ExportOrcs(pCommandList);
  }
#endif

  bool isDoneCommandList = false;
  if (m_isSingleCommandListMode)
  {
    // シングルバッファ
  }
  else
  {
    // ダブルバッファ

    // 前回のコマンドリストが終了するまで待つ
    CTRCommandList* pPrevCommandList = this->GetPrevCommandList();
    if (pPrevCommandList->GetIsRunning() || (pPrevCommandList->GetRunBufferSize() > 0))
    {
      pPrevCommandList->WaitDone();
      pPrevCommandList->Stop();
      pPrevCommandList->Clear();

      isDoneCommandList = true;
    }
  }

  // キャッシュのフラッシュ
  nngxUpdateBuffer(reinterpret_cast<void*>(nn::os::GetDeviceMemoryAddress()), nn::os::GetDeviceMemorySize());

  if (m_isSingleCommandListMode)
  {
    // シングルバッファ

    // 現在のコマンドバッファを実行
#if defined(GFX_CTR_PROFILING_ENABLED)
    m_GPUProfiler.StartGPUTotalTime(pCommandList);
#endif
    pCommandList->Run();

    // コマンドリストが終了するまで待つ
    pCommandList->WaitDone();
    pCommandList->Stop();
    pCommandList->Clear();

    isDoneCommandList = true;
  }
  else
  {
    // ダブルバッファ
  }

  // @fix GFNMCat[663] 描画実行が完了した場合は、必ずディスプレイバッファを有効にする。
  {
    // ディスプレイバッファスワップ
    if (m_pSwapDisplayBufferList[CtrDisplayNo::LEFT] != NULL)
    {
      // 上画面スワップ
      if ((m_pSwapDisplayBufferList[CtrDisplayNo::RIGHT] != NULL) && (m_StereoEnableCounter >= 3)) // 立体視有効にした直後は両方左目バッファを使う
      {
        // 左右別々
        nngxSwapBuffersByAddress(NN_GX_DISPLAY0, m_pSwapDisplayBufferList[CtrDisplayNo::LEFT], m_pSwapDisplayBufferList[CtrDisplayNo::RIGHT], nn::gx::DISPLAY0_WIDTH, GL_RGB8_OES);
      }
      else
      {
        // 左目用のバッファを右目にも使う
        nngxSwapBuffersByAddress(NN_GX_DISPLAY0, m_pSwapDisplayBufferList[CtrDisplayNo::LEFT], m_pSwapDisplayBufferList[CtrDisplayNo::LEFT], nn::gx::DISPLAY0_WIDTH, GL_RGB8_OES);
      }
    }
    if (m_pSwapDisplayBufferList[CtrDisplayNo::DOWN] != NULL)
    {
      // 下画面スワップ
      nngxSwapBuffersByAddress(NN_GX_DISPLAY1, m_pSwapDisplayBufferList[CtrDisplayNo::DOWN], NULL, nn::gx::DISPLAY1_WIDTH, GL_RGB8_OES);
    }
  }

  return isDoneCommandList;
}
void CTRGL::SwapDisplay_SwapBuffers_()
{
#if defined(GFX_CTR_PROFILING_ENABLED)
  // GPUプロファイリング
  m_GPUProfiler.StopGPUProfile();
  m_GPUProfiler.StartGPUProfile();
#endif

  // コマンドリストのモード切替
  m_isSingleCommandListMode = m_isRequestSingleCommandListMode;

  // ステレオモード取得
  b32 isStereoMode = IsStereoModeEnable() && nn::gx::IsStereoVisionAllowed();
  if (isStereoMode)
  {
    ++m_StereoEnableCounter;
    if (m_StereoEnableCounter >= 3)
    {
      m_StereoEnableCounter = 3;
    }
  }
  else
  {
    m_StereoEnableCounter = 0;
  }

#if 0
  // @fix GFNMCat[663] 描画実行が完了した場合は、必ずディスプレイバッファを有効にするため、WaitDoneに移動
  // ディスプレイバッファスワップ
  if (m_pSwapDisplayBufferList[CtrDisplayNo::LEFT] != NULL)
  {
    // 上画面スワップ
    if ((m_pSwapDisplayBufferList[CtrDisplayNo::RIGHT] != NULL) && (m_StereoEnableCounter >= 3)) // 立体視有効にした直後は両方左目バッファを使う
    {
      // 左右別々
      nngxSwapBuffersByAddress(NN_GX_DISPLAY0, m_pSwapDisplayBufferList[CtrDisplayNo::LEFT], m_pSwapDisplayBufferList[CtrDisplayNo::RIGHT], nn::gx::DISPLAY0_WIDTH, GL_RGB8_OES);
    }
    else
    {
      // 左目用のバッファを右目にも使う
      nngxSwapBuffersByAddress(NN_GX_DISPLAY0, m_pSwapDisplayBufferList[CtrDisplayNo::LEFT], m_pSwapDisplayBufferList[CtrDisplayNo::LEFT], nn::gx::DISPLAY0_WIDTH, GL_RGB8_OES);
    }
  }
  if (m_pSwapDisplayBufferList[CtrDisplayNo::DOWN] != NULL)
  {
    // 下画面スワップ
    nngxSwapBuffersByAddress(NN_GX_DISPLAY1, m_pSwapDisplayBufferList[CtrDisplayNo::DOWN], NULL, nn::gx::DISPLAY1_WIDTH, GL_RGB8_OES);
  }
#endif

  for (u32 i = 0; i < CtrDisplayNo::NumberOf; ++i)
  {
    DisplayWindow* pDisplay = static_cast< DisplayWindow* >(m_pSwapDisplayHandleList[i]);

    if (pDisplay == NULL)
    {
      continue;
    }

    pDisplay->Swap();
  }
}
void CTRGL::SwapDisplay_WaitVSync_()
{
#if defined(GFX_CTR_PROFILING_ENABLED)
  nnprofRecordWaitForVBlank();
#endif

  nngxWaitVSync(NN_GX_DISPLAY_BOTH);

#if defined(GFX_CTR_PROFILING_ENABLED)
  nnprofRecordTopMainLoop();
#endif

  // 初回はLCDのスタートが必要
  if (m_FirstSwapFlag)
  {
    m_FirstSwapFlag = false;
    nngxStartLcdDisplay();
  }
}

void CTRGL::SwapDisplay_RunGPUCommand_()
{
  CTRCommandList* pCommandList = this->GetCurrentCommandList_();

#if GFL_DEBUG
  DebugDump();
#endif

  if (m_isSingleCommandListMode)
  {
    // シングルバッファ
  }
  else
  {
    // ダブルバッファ

    // 現在のコマンドバッファを実行
#if defined(GFX_CTR_PROFILING_ENABLED)
    m_GPUProfiler.StartGPUTotalTime(pCommandList);
#endif
    pCommandList->Run();

    // コマンドリストスワップ
    this->SwapCommandList();
  }

  // スワップ予定のディスプレイバッファをクリア
  for (u32 i = 0; i < CtrDisplayNo::NumberOf; ++i)
  {
    m_pSwapDisplayBufferList[i] = NULL;
    m_pSwapDisplayHandleList[i] = NULL;
  }

  // 全ステートクリア
  this->ClearAllState();

  // ジャンプヘルパー
  m_GrHelper = nn::gr::CommandBufferJumpHelper(this->GetCurrentCommandList_()->GetCurrentBufferAddr());

  // 全無効コマンド追加
  this->AddDisableAllCommand_(this->GetJumpHelper());

#if defined(GFX_CTR_PROFILING_ENABLED)
  // CPU計測開始
  m_GPUProfiler.StartCPUTotalTime();
#endif

	GLAssertCheck();
}

//----------------------------------------------------------------------------
/**
 *   @brief レンダリングターゲットを切り替える
//! @par index でＭＲＴカラーバッファのインデクスを指定
*/
//-----------------------------------------------------------------------------
b32 CTRGL::SetRenderTarget_(u32 index, const Surface* const pSurface)
{
  // nn::gr用にconst外します
  Surface* pTmp = const_cast<Surface*>(pSurface);
	m_ActgiveRenderTarget = static_cast<CTRSurface*>(pTmp);

	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 現在のレンダリングターゲットを取得する
*/
//-----------------------------------------------------------------------------
const Surface* CTRGL::GetRenderTarget_(u32 index)
{
	return m_ActgiveRenderTarget;
}

//----------------------------------------------------------------------------
/**
 *   @brief 深度ステンシルを切り替える
*/
//-----------------------------------------------------------------------------
b32 CTRGL::SetDepthStencil_(const Surface* const pSurface)
{
  // nn::gr用にconst外します
  Surface* pTmp = const_cast<Surface*>(pSurface);
	m_ActgiveDepthStencil = static_cast<CTRSurface*>(pTmp);

	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 現在の深度ステンシルを取得する
*/
//-----------------------------------------------------------------------------
const Surface* CTRGL::GetDepthStencil_()
{
	return m_ActgiveDepthStencil;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
b32 CTRGL::BeginTimerQuery_( const TimerQuery* pTimerQuery )
{
#if defined(GFX_CTR_PROFILING_ENABLED)
  // const 外します
  TimerQuery* pTmp = const_cast<TimerQuery*>(pTimerQuery);
  CTRTimerQuery* pCTRTimerQuery = static_cast<CTRTimerQuery*>(pTmp);
  CTRCommandList* pCommandList = this->GetCurrentCommandList_();
  nn::gr::CommandBufferJumpHelper* pJumpHelper = this->GetJumpHelper();

  // 同期開始
  this->SyncCommandListAndJumpHelperBefore_(pCommandList, pJumpHelper);

  // 計測用にわざと区切りコマンドを入れる、Flushだと入れられないことがあるのでSplitDraw
  pCommandList->SplitDraw();

  // 同期終了
  this->SyncCommandListAndJumpHelperAfter_(pCommandList, pJumpHelper);

  // 計測開始
  pCTRTimerQuery->Begin(pCommandList, &m_GPUProfiler);
#endif
	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void CTRGL::EndTimerQuery_( const TimerQuery* pTimerQuery)
{
#if defined(GFX_CTR_PROFILING_ENABLED)
  // const 外します
  TimerQuery* pTmp = const_cast<TimerQuery*>(pTimerQuery);
  CTRTimerQuery* pCTRTimerQuery = static_cast<CTRTimerQuery*>(pTmp);
  CTRCommandList* pCommandList = this->GetCurrentCommandList_();
  nn::gr::CommandBufferJumpHelper* pJumpHelper = this->GetJumpHelper();

  // 同期開始
  this->SyncCommandListAndJumpHelperBefore_(pCommandList, pJumpHelper);

  // 計測用にわざと区切りコマンドを入れる、Flushだと入れられないことがあるのでSplitDraw
  pCommandList->SplitDraw();

  // 同期終了
  this->SyncCommandListAndJumpHelperAfter_(pCommandList, pJumpHelper);

  // 計測終了
  pCTRTimerQuery->End(pCommandList, &m_GPUProfiler);
#endif
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
b32 CTRGL::BeginOcclusionQuery_( const OcclusionQuery* pOcclusionQuery )
{
  // 対応しない
	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void CTRGL::EndOcclusionQuery_( const OcclusionQuery* pOcclusionQuery )
{
  // 対応しない
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
b32 CTRGL::BeginConditionalRender_( const OcclusionQuery* pOcclusionQuery )
{
  // 対応しない
	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void CTRGL::EndConditionalRender_( const OcclusionQuery* pOcclusionQuery)
{
  // 対応しない
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
b32 CTRGL::BeginScene_()
{
	u32			targetBit = nn::gr::FrameBuffer::COLOR_BUFFER_BIT;

  // カラーバッファ
	const CTRSurface* pCTRSurfaceColor = reinterpret_cast<const CTRSurface*>(m_ActgiveRenderTarget);
  const nn::gr::FrameBuffer::ColorBuffer& cb = pCTRSurfaceColor->GetHandle().colorBuffer;

	const gfx::Surface::SurfaceDesc				&rSurfaceDesc = pCTRSurfaceColor->GetSurfaceDesc();

  // デプスステンシルバッファ
	const CTRSurface* pCTRSurfaceDepthStencil = reinterpret_cast<const CTRSurface*>(m_ActgiveDepthStencil);

  // フレームバッファサイズ
	m_GrFrameBuffer.width = rSurfaceDesc.bufferWidth;
	m_GrFrameBuffer.height = rSurfaceDesc.bufferHeight;

  // カラーバッファ設定
  m_GrFrameBuffer.colorBuffer.virtualAddr = cb.virtualAddr;
  m_GrFrameBuffer.colorBuffer.format = cb.format;
  m_GrFrameBuffer.colorBuffer.blockSize = cb.blockSize;
  m_GrFrameBuffer.colorBuffer.width = rSurfaceDesc.bufferWidth;
  m_GrFrameBuffer.colorBuffer.height = rSurfaceDesc.bufferHeight;

	if ( pCTRSurfaceDepthStencil )
	{// デプスステンシル設定
		const nn::gr::FrameBuffer::DepthStencilBuffer& dsb = pCTRSurfaceDepthStencil->GetHandle().depthStencilBuffer;
		m_GrFrameBuffer.depthStencilBuffer.virtualAddr = dsb.virtualAddr;
		m_GrFrameBuffer.depthStencilBuffer.format = dsb.format;
		m_GrFrameBuffer.depthStencilBuffer.width = rSurfaceDesc.bufferWidth;
		m_GrFrameBuffer.depthStencilBuffer.height = rSurfaceDesc.bufferHeight;

		targetBit |= nn::gr::FrameBuffer::DEPTH_STENCIL_BUFFER_BIT;
	}

  // フレームバッファ設定コマンド追加
  this->AddFrameBufferCommand(this->GetJumpHelper(), targetBit);

	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void CTRGL::EndScene_()
{
  CTRCommandList* pCommandList = this->GetCurrentCommandList_();
  nn::gr::CommandBufferJumpHelper* pJumpHelper = this->GetJumpHelper();

  // ジオメオトリシェーダーを無効化する
  {
    u32* command = pJumpHelper->GetCurrentPtr();
    command = nn::gr::CTR::Shader::MakeDisableCommand(command);
    pJumpHelper->Put(command);
  }

  // 同期開始
  this->SyncCommandListAndJumpHelperBefore_(pCommandList, pJumpHelper);

  // 区切りを入れる
  pCommandList->Flush3DCommandNoCacheFlush();

  // 同期終了
  this->SyncCommandListAndJumpHelperAfter_(pCommandList, pJumpHelper);

	GLAssertCheck();
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void CTRGL::Finish_()
{
  // 対応しない
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void CTRGL::ClearRenderTarget_(u32 index, const Color &rClearColor )
{
  // ビューポート設定コマンド追加
  this->AddViewportCommand(this->GetJumpHelper());

  // シザーテスト設定コマンド追加
  this->AddScissorCommand(this->GetJumpHelper());

  // カラーバッファークリアコマンド追加
  m_GrFrameBuffer.colorBuffer.clearColor[0] = rClearColor.x;
  m_GrFrameBuffer.colorBuffer.clearColor[1] = rClearColor.y;
  m_GrFrameBuffer.colorBuffer.clearColor[2] = rClearColor.z;
  m_GrFrameBuffer.colorBuffer.clearColor[3] = rClearColor.w;
  this->AddClearBufferCommand(this->GetCurrentCommandList_(), this->GetJumpHelper(), nn::gr::FrameBuffer::COLOR_BUFFER_BIT);
}

//----------------------------------------------------------------------------
/**
 *   @brief 深度ステンシルをクリアする
*/
//-----------------------------------------------------------------------------
void CTRGL::ClearDepthStencil_( f32 depth, u8 stencil )
{
  // ビューポートコマンド追加
  this->AddViewportCommand(this->GetJumpHelper());

  // シザーテスト設定コマンド追加
  this->AddScissorCommand(this->GetJumpHelper());

  // デプスステンシルバッファークリアコマンド作成
  m_GrFrameBuffer.depthStencilBuffer.clearDepth = depth;
  m_GrFrameBuffer.depthStencilBuffer.clearStencil = stencil;
  this->AddClearBufferCommand(this->GetCurrentCommandList_(), this->GetJumpHelper(), nn::gr::FrameBuffer::DEPTH_STENCIL_BUFFER_BIT);
}

//----------------------------------------------------------------------------
/**
 *   @brief レンダリングターゲットと深度ステンシルをクリアする
*/
//-----------------------------------------------------------------------------
void CTRGL::ClearRenderTargetDepthStencil_(u32 index, const Color &rClearColor, f32 depth, u8 stencil )
{
  // ビューポートコマンド追加
  this->AddViewportCommand(this->GetJumpHelper());

  // シザーテスト設定コマンド追加
  this->AddScissorCommand(this->GetJumpHelper());

  // カラーバッファークリアコマンド追加
  m_GrFrameBuffer.colorBuffer.clearColor[0] = rClearColor.x;
  m_GrFrameBuffer.colorBuffer.clearColor[1] = rClearColor.y;
  m_GrFrameBuffer.colorBuffer.clearColor[2] = rClearColor.z;
  m_GrFrameBuffer.colorBuffer.clearColor[3] = rClearColor.w;

  // デプスステンシルバッファークリアコマンド作成
  m_GrFrameBuffer.depthStencilBuffer.clearDepth = depth;
  m_GrFrameBuffer.depthStencilBuffer.clearStencil = stencil;

  this->AddClearBufferCommand(this->GetCurrentCommandList_(), this->GetJumpHelper(), nn::gr::FrameBuffer::COLOR_BUFFER_BIT | nn::gr::FrameBuffer::DEPTH_STENCIL_BUFFER_BIT);
}

//----------------------------------------------------------------------------
/**
 *   @brief ビューポートを設定する
*/
//-----------------------------------------------------------------------------
void CTRGL::SetViewport_(const u32 xp, const u32 yp, const u32 width, const u32 height)
{
  // CTRは幅と高さが逆
  u32 ctrWidth = height;
  u32 ctrHeight = width;

  this->SetDirty(DIRTY_FLAG_VIEWPORT);
  m_GrViewport.Set(xp, yp, ctrWidth, ctrHeight);

  // カラーバッファクリア時 or フレームバッファクリア時 or 描画時に実際のコマンド発行
}

//----------------------------------------------------------------------------
/**
 *   @brief シザーを設定する
*/
//-----------------------------------------------------------------------------
void CTRGL::SetScissor_(const u32 xp, const u32 yp, const u32 width, const u32 height)
{
  // CTRは幅と高さが逆
  u32 ctrWidth = height;
  u32 ctrHeight = width;

  this->SetDirty(DIRTY_FLAG_SCISOR);
  m_GrScissor.isEnable = true;
  m_GrScissor.Set(xp, yp, ctrWidth, ctrHeight);

  // カラーバッファクリア時 or フレームバッファクリア時 or 描画時に実際のコマンド発行
}

//----------------------------------------------------------------------------
/**
//! @brief 頂点シェーダーを切り替える
*/
//-----------------------------------------------------------------------------
void CTRGL::SetVertexShader_( const Shader* pShader )
{
  this->SetDirty(DIRTY_FLAG_VERTEX_SHADER);

  // nn::gr用にconst外します
  Shader* pTmp = const_cast<Shader*>(pShader);
	m_pCurrentVertexShader = static_cast<CTRVertexShader*>(pTmp);

  // 描画時に実際のコマンド発行
}

//----------------------------------------------------------------------------
/**
//! @brief ジオメトリシェーダーを切り替える
*/
//-----------------------------------------------------------------------------
void CTRGL::SetGeometryShader_( const Shader* pShader )
{
  // 対応しない
}

//----------------------------------------------------------------------------
/**
//! @brief ピクセルシェーダーを切り替える
*/
//-----------------------------------------------------------------------------
void CTRGL::SetPixelShader_( const Shader* pShader )
{
  this->SetDirty(DIRTY_FLAG_PIXEL_SHADER);
  m_pCurrentPixelShader = (CTRPixelShader*)pShader;

  // 描画時に実際のコマンド発行
}

//----------------------------------------------------------------------------
/**
 *   @brief 頂点シェーダ定数設定
*/
//-----------------------------------------------------------------------------
void CTRGL::SetVertexShaderConstant_( u32 registerNo, ConstantBuffer *pBuffer )
{
  // uniform変数はダーティフラグで判定しない（できない）
	const CTRConstantBuffer* pCTRBuffer = reinterpret_cast<const CTRConstantBuffer*>( pBuffer );

  nn::gr::ShaderLite* pShader = m_pCurrentVertexShader->GetHandle();
  u8 symbolOffset = m_pCurrentVertexShader->GetUniformSymbolOffset();

  // 一つずつにしか対応していない
  ConstantType type = pCTRBuffer->GetType(0);
  const void* pBuf = pCTRBuffer->GetBuffer(0);

  nn::gr::CommandBufferJumpHelper* pJumpHelper = this->GetJumpHelper();
  switch (type)
  {
  case ConstantType::Vector:
    {
      nn::gr::BindSymbolVSFloat symbol;
      pShader->SearchBindSymbol(&symbol, registerNo + symbolOffset);

      const nn::math::Vector4* val = reinterpret_cast<const nn::math::Vector4*>(pBuf);
      pJumpHelper->Put(symbol.MakeUniformCommand(pJumpHelper->GetCurrentPtr(), *val));
    }
    break;
  case ConstantType::Matrix:
    {
      nn::gr::BindSymbolVSFloat symbol;
      pShader->SearchBindSymbol(&symbol, registerNo + symbolOffset);

      const nn::math::MTX44* mtx44 = reinterpret_cast<const nn::math::MTX44*>(pBuf);
      pJumpHelper->Put(symbol.MakeUniformCommand(pJumpHelper->GetCurrentPtr(), *mtx44));
    }
    break;

  case ConstantType::Bool:
    {
      nn::gr::BindSymbolVSBool symbol;
      pShader->SearchBindSymbol(&symbol, registerNo + symbolOffset);

      const bool* isEnable = reinterpret_cast<const bool*>(pBuf);
      pShader->SetUniformBool(symbol, *isEnable);
    }
    break;
  case ConstantType::Int:
    {
      nn::gr::BindSymbolVSInteger symbol;
      pShader->SearchBindSymbol(&symbol, registerNo + symbolOffset);

      // xに入れる
      const s32* ival = reinterpret_cast<const s32*>(pBuf);
      pJumpHelper->Put(symbol.MakeUniformCommand(pJumpHelper->GetCurrentPtr(), *ival, 0, 0));
    }
    break;
	case ConstantType::Float:
    {
      nn::gr::BindSymbolVSFloat symbol;
      pShader->SearchBindSymbol(&symbol, registerNo + symbolOffset);

      // xに入れる
      const f32* fval = reinterpret_cast<const f32*>(pBuf);
      nn::math::VEC4 val(*fval, 0, 0, 0);
      pJumpHelper->Put(symbol.MakeUniformCommand(pJumpHelper->GetCurrentPtr(), val));
    }
    break;
	case ConstantType::BoolArray:
    {
      // 対応していない
      GFL_ASSERT(0);
    }
    break;
	case ConstantType::IntArray:
    {
      nn::gr::BindSymbolVSInteger symbol;
      pShader->SearchBindSymbol(&symbol, registerNo + symbolOffset);

      const s32* ival = reinterpret_cast<const s32*>(pBuf);
      pJumpHelper->Put(symbol.MakeUniformCommand(pJumpHelper->GetCurrentPtr(), ival[0], ival[1], ival[2]));
    }
    break;
  case ConstantType::FloatArray:
    {
      nn::gr::BindSymbolVSFloat symbol;
      pShader->SearchBindSymbol(&symbol, registerNo + symbolOffset);

      const f32* fval = reinterpret_cast<const f32*>(pBuf);
      nn::math::VEC4 val(fval[0], fval[1], fval[2], fval[3]);
      pJumpHelper->Put(symbol.MakeUniformCommand(pJumpHelper->GetCurrentPtr(), val));
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *   @brief ジオメトリシェーダ定数設定
*/
//-----------------------------------------------------------------------------
void CTRGL::SetGeometryShaderConstant_( u32 registerNo,	ConstantBuffer *pBuffer	)
{
  // 対応しない
}

//----------------------------------------------------------------------------
/**
 *   @brief ピクセルシェーダ定数設定
*/
//-----------------------------------------------------------------------------
void CTRGL::SetPixelShaderConstant_( u32 registerNo,	ConstantBuffer *pBuffer	)
{
  // 対応しない
}

//----------------------------------------------------------------------------
/**
 *   @brief インデクスバッファを設定します
*/
//-----------------------------------------------------------------------------
void CTRGL::SetIndices_(const IndexBuffer* pIndices)
{
  // nn::gr用にconst外します
  IndexBuffer* pTmp = const_cast<IndexBuffer*>(pIndices);
	m_pCurrentIndexBuffer = static_cast<CTRIndexBuffer*>(pTmp);
}

//----------------------------------------------------------------------------
/**
 *   @brief 頂点バッファをストリームに設定
*/
//-----------------------------------------------------------------------------
void CTRGL::SetStreamSource_( const u32 index, const VertexBuffer* pVertexBuffer, const s32 offset, const s32 stride )
{
  // nn::gr用にconst外します
  VertexBuffer* pTmp = const_cast<VertexBuffer*>(pVertexBuffer);
	CTRVertexBuffer* pCTRVertexBuffer = static_cast<CTRVertexBuffer*>(pTmp);

  m_pCurrentVertexShader->BindVertexBuffer(pCTRVertexBuffer);
}

//----------------------------------------------------------------------------
/**
 *   @brief 頂点ストリームにセットされた頂点を使って描画
*/
//-----------------------------------------------------------------------------
void CTRGL::DrawPrimitive_( PrimitiveType primitive, u32 start, u32 count	)
{
  // @attention CTR版はインデックス無し描画は対応しない
}

//----------------------------------------------------------------------------
/**
 *   @brief 頂点ストリームにセットされた頂点とインデックスバッファを使って描画
*/
//-----------------------------------------------------------------------------
void CTRGL::DrawIndexedPrimitive_( PrimitiveType primitive, u32	start, u32	count )
{
  static const u8 sc_drawMode[] =
  {
		0xFF, // Points = 0,			//!< ポイント
		0xFF, // LineStrip,			//!< ラインストリップ
		0xFF, // Lines,					//!< ライン
		PICA_DATA_DRAW_TRIANGLES, // Triangles,			//!< トライアングル
		PICA_DATA_DRAW_TRIANGLE_STRIP, // TriangleStrip,	//!< トライアングルストリップ
  };

  // 描画設定
  nn::gr::ShaderLite* pShader = m_pCurrentVertexShader->GetHandle();
  u8 drawMode = sc_drawMode[primitive];
  GFL_ASSERT(drawMode != 0xFF);
  pShader->SetDrawMode(static_cast<PicaDataDrawMode>(drawMode));

  nn::gr::Vertex::IndexStream* pIndexStream = m_pCurrentIndexBuffer->GetHandle();
  pIndexStream->drawVtxNum = count;

  // start用にアドレスをずらす
  uptr tmp = pIndexStream->physicalAddr;
  u32 indexSize = (pIndexStream->isUnsignedByte)
    ? 1
    : 2;
  pIndexStream->physicalAddr += indexSize * start;

  nn::gr::CommandBufferJumpHelper* pJumpHelper = this->GetJumpHelper();

  // ダーティフラグが立っている全コマンドフラッシュ
  this->FlushAllDirtyCommand_(pJumpHelper);

  // 描画コマンド追加
  this->AddDrawCommand(pJumpHelper);


  // インデックスバッファのずらしたアドレスを戻す
  pIndexStream->physicalAddr = tmp;
}

//----------------------------------------------------------------------------
/**
 *   @brief ブレンドステートの設定
*/
//-----------------------------------------------------------------------------
void CTRGL::SetBlendState_( BlendStateObject *pState )
{
  this->SetDirty(DIRTY_FLAG_ALPHA_TEST);
  this->SetDirty(DIRTY_FLAG_BLEND);
  this->SetDirty(DIRTY_FLAG_MASK);

	CTRBlendStateObject* pCTRBlendStateObject = static_cast<CTRBlendStateObject*>( pState );
	pCTRBlendStateObject->Bind(&m_GrRenderState.alphaTest, &m_GrRenderState.blend, &m_GrRenderState.colorMask);

  // 描画時に実際のコマンドを発行
}

//----------------------------------------------------------------------------
/**
 *   @brief ラスタライザステートの設定
*/
//-----------------------------------------------------------------------------
void CTRGL::SetRasterizerState_( RasterizerStateObject *pState )
{
  this->SetDirty(DIRTY_FLAG_WBUFFER);
  this->SetDirty(DIRTY_FLAG_CULLING_TEST);

	CTRRasterizerStateObject* pCTRRasterizerStateObject = static_cast<CTRRasterizerStateObject*>( pState );
	pCTRRasterizerStateObject->Bind(&m_GrRenderState.wBuffer, &m_GrRenderState.cullingTest);
}

//----------------------------------------------------------------------------
/**
 *   @brief 深度ステンシルステートの設定
*/
//-----------------------------------------------------------------------------
void CTRGL::SetDepthStencilState_( DepthStencilStateObject *pState )
{
  this->SetDirty(DIRTY_FLAG_DEPTH_TEST);
  this->SetDirty(DIRTY_FLAG_STENCIL_TEST);
  this->SetDirty(DIRTY_FLAG_MASK); // 書き込みマスクがカラーと同じレジスタなので更新する必要あり

	CTRDepthStencilStateObject* pCTRDepthStencilStateObject = static_cast<CTRDepthStencilStateObject*>( pState );
	pCTRDepthStencilStateObject->Bind(&m_GrRenderState.depthTest, &m_GrRenderState.stencilTest);
}

//----------------------------------------------------------------------------
/**
 *   @brief サンプラステートの設定
*/
//-----------------------------------------------------------------------------
void CTRGL::SetSamplerState_( u32 texNo, const SamplerStateObject *pState )
{
  this->SetDirty(DIRTY_FLAG_TEXTURE_FUNC);
  this->SetDirtyTexture(texNo);

  // nn::grの関係でconst外します
  SamplerStateObject* pTmp = const_cast<SamplerStateObject*>(pState);
  m_pCurrentSamplerList[texNo] = static_cast<CTRSamplerStateObject*>(pTmp);
}

//----------------------------------------------------------------------------
/**
 *   @brief テクスチャの設定
*/
//-----------------------------------------------------------------------------
void CTRGL::SetTexture_( u32 texNo, const Texture *pTexture )
{
  this->SetDirtyTexture(texNo);

  // nn::grの関係でconst外します
  Texture* pTmp = const_cast<Texture*>(pTexture);
	m_pCurrentTextureList[texNo] = static_cast<CTRTexture*>(pTmp);
}

//----------------------------------------------------------------------------
/**
 *   @brief ソースコードから頂点シェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* CTRGL::CreateVertexShaderFromSource_(const char* code, const char* entry, const VertexElement* pVertexElements, const char** args)
{
  // 対応しない
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *   @brief バイナリデータから頂点シェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* CTRGL::CreateVertexShaderFromMemory_(const void* code, u32 size, const VertexElement* pVertexElements)
{
	Shader* pShader =  new CTRVertexShader(code, size, pVertexElements);
	return pShader;
}

//----------------------------------------------------------------------------
/**
 *   @brief ソースコードからジオメトリシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* CTRGL::CreateGeometryShaderFromSource_(const char* code, const char* entry, const char** args)
{
  // 対応しない
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *   @brief バイナリデータからジオメトリシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* CTRGL::CreateGeometryShaderFromMemory_(const void* code, u32 size)
{
  // 対応しない
  return NULL;
}
		
//----------------------------------------------------------------------------
/**
 *   @brief ソースコードからピクセルシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* CTRGL::CreatePixelShaderFromSource_(const char* code, const char* entry, const char** args)
{
  // 対応しない
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *   @brief バイナリデータからピクセルシェーダーを作成
*/
//-----------------------------------------------------------------------------
Shader* CTRGL::CreatePixelShaderFromMemory_(const void* code, u32 size)
{
	Shader* pShader = new CTRPixelShader(code, size);
	return pShader;
}

//----------------------------------------------------------------------------
/**
 *			@brief 定数バッファを作成
 */
//-----------------------------------------------------------------------------
ConstantBuffer* CTRGL::CreateConstantBuffer_( const ConstantBufferElement* pConstantBufferElements, u32 usage, Pool pool, const void* buffer )
{
	ConstantBuffer* pConstantBuffer = new CTRConstantBuffer(pConstantBufferElements, usage, pool, buffer);
	return pConstantBuffer;
}

//----------------------------------------------------------------------------
/**
 *   @brief バイナリデータから頂点シェーダーを作成
*/
//-----------------------------------------------------------------------------
VertexBuffer* CTRGL::CreateVertexBuffer_( u32 size, u32 vertex_num, u32 usage, Pool pool, const void* buffer )
{
	VertexBuffer* pVertexBuffer = new CTRVertexBuffer(size, vertex_num, usage, pool, buffer);
	return pVertexBuffer;
}

//! @brief インデックスバッファを作成
IndexBuffer* CTRGL::CreateIndexBuffer_( u32 size, u32 usage, Pool pool, IndexType type, const void* buffer )
{
	IndexBuffer* pIndexBuffer = new CTRIndexBuffer(size, usage, pool, type, buffer);
	return pIndexBuffer;
}

//----------------------------------------------------------------------------
/**
 *   @brief ブレンドステートオブジェクトを作成
*/
//-----------------------------------------------------------------------------
BlendStateObject* CTRGL::CreateBlendStateObject_()
{
	BlendStateObject* pStateObj = new CTRBlendStateObject();
	return pStateObj;
}

//----------------------------------------------------------------------------
/**
 *   @brief ラスタライザステートオブジェクトを作成
*/
//-----------------------------------------------------------------------------
RasterizerStateObject* CTRGL::CreateRasterizerStateObject_()
{
	RasterizerStateObject* pStateObj = new CTRRasterizerStateObject();
	return pStateObj;
}

//----------------------------------------------------------------------------
/**
 *   @brief 深度ステンシルステートオブジェクトを作成
*/
//-----------------------------------------------------------------------------
DepthStencilStateObject* CTRGL::CreateDepthStencilStateObject_()
{
	DepthStencilStateObject* pStateObj = new CTRDepthStencilStateObject();
	return pStateObj;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
TimerQuery* CTRGL::CreateTimerQuery_()
{
  CTRTimerQuery* pTimerQuery = new CTRTimerQuery();
  return pTimerQuery;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
OcclusionQuery* CTRGL::CreateOcclusionQuery_()
{
  // 対応しない
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *   @brief ディスプレイリストの作成
*/
//-----------------------------------------------------------------------------
DisplayList* CTRGL::CreateDisplayList_()
{
  CTRDisplayList* pCTRDisplayList = new CTRDisplayList();
  return pCTRDisplayList;
}
//! @brief ディスプレイリストの記録開始
void CTRGL::BeginDisplayList_(DisplayList* pDisplayList)
{
  CTRDisplayList* pCTRDisplayList = static_cast<CTRDisplayList*>(pDisplayList);
  CTRCommandList* pCommandList = s_Gp->GetCurrentCommandList_();
  nn::gr::CommandBufferJumpHelper* pJumpHelper = this->GetJumpHelper();

  // 同期開始
  this->SyncCommandListAndJumpHelperBefore_(pCommandList, pJumpHelper);

  pCTRDisplayList->Begin(pJumpHelper->GetCurrentPtr());

  // 同期終了
  this->SyncCommandListAndJumpHelperAfter_(pCommandList, pJumpHelper);

  // ダーティフラグをクリア
  this->ClearDirty();
}

//! @brief ディスプレイリストの記録終了
void CTRGL::EndDisplayList_(DisplayList* pDisplayList)
{
  CTRDisplayList* pCTRDisplayList = static_cast<CTRDisplayList*>(pDisplayList);
  CTRCommandList* pCommandList = s_Gp->GetCurrentCommandList_();
  nn::gr::CommandBufferJumpHelper* pJumpHelper = this->GetJumpHelper();

  // 同期開始
  u32 jumpSize = pJumpHelper->GetSizeToFirstJump();
  this->SyncCommandListAndJumpHelperBefore_(pCommandList, pJumpHelper);

  pCTRDisplayList->End(pJumpHelper->GetCurrentPtr(), jumpSize);

  // 同期終了
  this->SyncCommandListAndJumpHelperAfter_(pCommandList, pJumpHelper);

  // ダーティフラグをクリア
  this->ClearDirty();
}

//! @brief ディスプレイリストの呼び出し
void CTRGL::CallDisplayList_(DisplayList* pDisplayList)
{
  CTRDisplayList* pCTRDisplayList = static_cast<CTRDisplayList*>(pDisplayList);
  CTRCommandList* pCommandList = s_Gp->GetCurrentCommandList_();
  nn::gr::CommandBufferJumpHelper* pJumpHelper = this->GetJumpHelper();

  // 同期開始
  this->SyncCommandListAndJumpHelperBefore_(pCommandList, pJumpHelper);

  // コマンドの追加
  pCommandList->Add3DCommandNoCacheFlush(pCTRDisplayList->GetBeginAddr(), pCTRDisplayList->GetBufferSize());

  // 同期終了
  this->SyncCommandListAndJumpHelperAfter_(pCommandList, pJumpHelper);
}

//----------------------------------------------------------------------------
/**
 *   @brief サンプラステートオブジェクトを作成
*/
//-----------------------------------------------------------------------------
SamplerStateObject* CTRGL::CreateSamplerStateObject_()
{
	GLAssertCheck();
	SamplerStateObject							*pStateObj = NULL;
	pStateObj = new CTRSamplerStateObject();
	GLAssertCheck();
	return pStateObj;
}

//----------------------------------------------------------------------------
/**
 *   @brief ２Ｄテクスチャを作成する
//! @param usage オプション
//! @param mipLevels ミップマップレベル数指定( 1 - 13 )
*/
//-----------------------------------------------------------------------------
Texture* CTRGL::CreateTexture_(u32 width, u32 height, u32 mipLevels, u32 usage, Format format, Pool pool)
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
	pTexture = new CTRTexture( desc );
	GLAssertCheck();
	return pTexture;
}

//----------------------------------------------------------------------------
/**
 *   @brief イメージデータから２Ｄテクスチャを作成する
*/
//-----------------------------------------------------------------------------
Texture* CTRGL::CreateTextureFromImage_( const ImageData &rImage, Pool pool)
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
	pTexture = new CTRTexture( desc );

	//s32 surfaceLevel = 0;
	for( u32 surfaceLevel = 0; surfaceLevel < rImage.GetDepth(); ++surfaceLevel )
	{
		pTexture->Update( rImage.GetImage(surfaceLevel), surfaceLevel );
	}

	GLAssertCheck();
	return pTexture;
}

//! @brief サーフェースから２Ｄテクスチャを作成する
Texture* CTRGL::CreateTextureFromSurface_( const Surface* pSurface )
{
	Texture::TextureDesc textureDesc;
  const Surface::SurfaceDesc& surfaceDesc = pSurface->GetSurfaceDesc();

	textureDesc.type = TextureType::Image;
  textureDesc.mipLevels = 1;
  textureDesc.depth = 1;

  textureDesc.usage = Usage::Dynamic;
	textureDesc.pool = Pool::Managed;
	textureDesc.width = surfaceDesc.width;
	textureDesc.height = surfaceDesc.height;

	textureDesc.width = surfaceDesc.bufferWidth;
	textureDesc.height = surfaceDesc.bufferHeight;

	//textureDesc.format = surfaceDesc.format; // これだとステンシルをRで取得できなかった
  textureDesc.format = Format::A8R8G8B8;
	textureDesc.multiSample = MultiSample::None;

	Texture* pTexture = NULL;
	pTexture = new CTRTexture( textureDesc );

  const CTRSurface* pCTRSurface = static_cast<const CTRSurface*>(pSurface);
  uptr addr = (surfaceDesc.format == Format::D24S8)
    ? pCTRSurface->GetHandle().depthStencilBuffer.virtualAddr
    : pCTRSurface->GetHandle().colorBuffer.virtualAddr;
  pTexture->Update( reinterpret_cast<void*>(addr), 1 );

	GLAssertCheck();
	return pTexture;
}

// 画面サイズ取得：幅
u32 CTRGL::GetDisplayWidth_( DisplayDeviceHandle hDisplay )
{
	DisplayWindow* const pDisplay = reinterpret_cast< DisplayWindow* >( hDisplay );
  if (pDisplay == NULL)
  {
    return 0;
  }

  // 幅と高さは逆転させておく
  return (pDisplay->GetDisplayNo() == CtrDisplayNo::DOWN)
    ? nn::gx::DISPLAY1_HEIGHT
    : nn::gx::DISPLAY0_HEIGHT;
}

// 画面サイズ取得：高さ
u32 CTRGL::GetDisplayHeight_( DisplayDeviceHandle hDisplay )
{
	DisplayWindow* const pDisplay = reinterpret_cast< DisplayWindow* >( hDisplay );
  if (pDisplay == NULL)
  {
    return 0;
  }

  // 幅と高さは逆転させておく
  return (pDisplay->GetDisplayNo() == CtrDisplayNo::DOWN)
    ? nn::gx::DISPLAY1_WIDTH
    : nn::gx::DISPLAY0_WIDTH;
}

// ダーティフラグが立っている全コマンドフラッシュ
void CTRGL::FlushAllDirtyCommand_(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  // ビューポート設定コマンド追加
  this->AddViewportCommand(pJumpHelper);

  // シザー設定コマンド追加
  this->AddScissorCommand(pJumpHelper);

  // カリングテスト設定コマンド追加
  this->AddCullingTestCommand(pJumpHelper);

  // ブレンド設定コマンド追加
  this->AddBlendCommand(pJumpHelper);

  // GR的には以下の2つのコマンドもあるが対応しない
  //command = logicOp.MakeCommand( command, false );
  //command = shadowMap.MakeCommand( command, false );

  // アルファテスト設定コマンド追加
  this->AddAlphaTestCommand(pJumpHelper);

  // ステンシルテスト設定コマンド追加
  this->AddStencilTestCommand(pJumpHelper);

  // デプステスト設定コマンド追加
  this->AddDepthTestCommand(pJumpHelper);

  // Wバッファ設定コマンド追加
  this->AddWBufferCommand(pJumpHelper);

  // マスク設定コマンド追加
  this->AddMaskCommand(pJumpHelper);

  // バーテクスシェーダー設定コマンド追加
  this->AddVertexShaderCommand(pJumpHelper);

  // ピクセルシェーダー設定コマンド追加
  this->AddPixelShaderCommand(pJumpHelper);

  // テクスチャ0設定コマンド追加
  this->AddTexture0Command(pJumpHelper);

  // テクスチャ1設定コマンド追加
  this->AddTexture1Command(pJumpHelper);

  // テクスチャ2設定コマンド追加
  this->AddTexture2Command(pJumpHelper);

  // テクスチャ関数設定コマンド追加
  this->AddTextureFuncCommand(pJumpHelper);
}

// 全無効コマンド追加
void CTRGL::AddDisableAllCommand_(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  u32* command = pJumpHelper->GetCurrentPtr();

  // 全ステート無効化
  command = nn::gr::MakeDisableAllCommand(command);

  // 浮動小数点数定数レジスタ初期化
  nn::math::VEC4 v(0.0f, 0.0f, 0.0f, 0.0f);
  for (u32 i = 0; i <= 95; ++i)
  {
    command = nn::gr::MakeUniformCommandVS(command, i, v);
  }

  pJumpHelper->Put(command);
}

// フレームバッファ設定コマンド追加
void CTRGL::AddFrameBufferCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper, u32 targetBit )
{
  pJumpHelper->Put(m_GrFrameBuffer.MakeCommand(pJumpHelper->GetCurrentPtr(), targetBit) );
}

// バッファクリアコマンド追加
void CTRGL::AddClearBufferCommand(CTRCommandList* pCommandList, nn::gr::CommandBufferJumpHelper* pJumpHelper, u32 grClearBit)
{
  bool      isAddSplitDrawCmdlist = true;

  u32 sizeToFirstJump = pJumpHelper->GetSizeToFirstJump();
  if ( sizeToFirstJump > 0 )
    isAddSplitDrawCmdlist = false;//SyncCommandListAndJumpHelperBefore_関数でFlush(=split)される。

  // 同期開始
  this->SyncCommandListAndJumpHelperBefore_(pCommandList, pJumpHelper);

  // クリア
  m_GrFrameBuffer.MakeClearRequestRaw(this->GetCurrentCommandList_()->GetGxObject(), grClearBit, isAddSplitDrawCmdlist);

  // 同期終了
  this->SyncCommandListAndJumpHelperAfter_(pCommandList, pJumpHelper);
}

// ビューポート設定コマンド追加
void CTRGL::AddViewportCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_VIEWPORT))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_VIEWPORT);

  pJumpHelper->Put(m_GrViewport.MakeCommand(pJumpHelper->GetCurrentPtr()));
}

// シザー設定コマンド追加
void CTRGL::AddScissorCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_SCISOR))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_SCISOR);

  pJumpHelper->Put(m_GrScissor.MakeCommand(pJumpHelper->GetCurrentPtr()));
}
// カリングテスト設定コマンド追加
void CTRGL::AddCullingTestCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_CULLING_TEST))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_CULLING_TEST);

  // マスク設定は個別に発行、false
  pJumpHelper->Put(m_GrRenderState.cullingTest.MakeCommand(pJumpHelper->GetCurrentPtr(), false));
}

// ブレンド設定コマンド追加
void CTRGL::AddBlendCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_BLEND))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_BLEND);

  // マスク設定は個別に発行、false
  pJumpHelper->Put(m_GrRenderState.blend.MakeCommand(pJumpHelper->GetCurrentPtr(), false));
}

// アルファテスト設定コマンド追加
void CTRGL::AddAlphaTestCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_ALPHA_TEST))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_ALPHA_TEST);

  // マスク設定は個別に発行、false
  pJumpHelper->Put(m_GrRenderState.alphaTest.MakeCommand(pJumpHelper->GetCurrentPtr(), false));
}

// ステンシルテスト設定コマンド追加
void CTRGL::AddStencilTestCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_STENCIL_TEST))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_STENCIL_TEST);

  // マスク設定は個別に発行、false
  pJumpHelper->Put(m_GrRenderState.stencilTest.MakeCommand(pJumpHelper->GetCurrentPtr(), false));
}

// デプステスト設定コマンド追加
void CTRGL::AddDepthTestCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_DEPTH_TEST))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_DEPTH_TEST);

  // マスク設定は個別に発行、false
  pJumpHelper->Put(m_GrRenderState.depthTest.MakeCommand(pJumpHelper->GetCurrentPtr(), false));
}

// Wバッファ設定コマンド追加
void CTRGL::AddWBufferCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_WBUFFER))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_WBUFFER);

  pJumpHelper->Put(m_GrRenderState.wBuffer.MakeCommand(pJumpHelper->GetCurrentPtr()));
}

// マスク設定コマンド追加
void CTRGL::AddMaskCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_MASK))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_MASK);

  // キャッシュは転送時にまとめてクリア
  pJumpHelper->Put(m_GrRenderState.fbAccess.MakeCommand(pJumpHelper->GetCurrentPtr(), false));
}

// バーテクスシェーダ設定コマンド追加
void CTRGL::AddVertexShaderCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_VERTEX_SHADER))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_VERTEX_SHADER);

  nn::gr::ShaderLite* pShaderLite = m_pCurrentVertexShader->GetHandle();

  pJumpHelper->Put(pShaderLite->MakeFullCommand(pJumpHelper->GetCurrentPtr()));
}

// ピクセルシェーダ設定コマンド追加
void CTRGL::AddPixelShaderCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_PIXEL_SHADER))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_PIXEL_SHADER);

  const u32* pShaderCommand = m_pCurrentPixelShader->GetCommandBuffer();
  u32 shaderSize = m_pCurrentPixelShader->GetCommandSize();

#if 0
  // コピーする版
  this->AddCommandCopy_(pJumpHelper, pShaderCommand, shaderSize);
#else
  // ジャンプする版
  this->AddCommandJump_(pJumpHelper, pShaderCommand, shaderSize);
#endif
}

// テクスチャ0設定コマンド追加
void CTRGL::AddTexture0Command(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_TEXTURE_0))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_TEXTURE_0);

  m_pCurrentSamplerList[0]->Bind(&m_GrTexture, 0, m_pCurrentTextureList[0]);

  // テクスチャ関数は後でまとめて設定、false
  pJumpHelper->Put(m_GrTexture.unit0.MakeCommand(pJumpHelper->GetCurrentPtr(), false));
}

// テクスチャ1設定コマンド追加
void CTRGL::AddTexture1Command(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_TEXTURE_1))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_TEXTURE_1);

  m_pCurrentSamplerList[1]->Bind(&m_GrTexture, 1, m_pCurrentTextureList[1]);

  // テクスチャ関数は後でまとめて設定、false
  pJumpHelper->Put(m_GrTexture.unit1.MakeCommand(pJumpHelper->GetCurrentPtr(), false));
}

// テクスチャ2設定コマンド追加
void CTRGL::AddTexture2Command(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_TEXTURE_2))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_TEXTURE_2);

  m_pCurrentSamplerList[2]->Bind(&m_GrTexture, 2, m_pCurrentTextureList[2]);

  // テクスチャ関数は後でまとめて設定、false
  pJumpHelper->Put(m_GrTexture.unit2.MakeCommand(pJumpHelper->GetCurrentPtr(), false));
}

// テクスチャ関数設定コマンド追加
void CTRGL::AddTextureFuncCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  if (!this->IsDirty(DIRTY_FLAG_TEXTURE_FUNC))
  {
    return;
  }
  this->ResetDirty(DIRTY_FLAG_TEXTURE_FUNC);

  pJumpHelper->Put(m_GrTexture.MakeFuncCommand(pJumpHelper->GetCurrentPtr()));
}

// 描画コマンド追加
void CTRGL::AddDrawCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  CTRIndexBuffer* pCTRIndexBuffer = (CTRIndexBuffer*)m_pCurrentIndexBuffer;
  nn::gr::Vertex::IndexStream* pIndexStream = pCTRIndexBuffer->GetHandle();
  nn::gr::Vertex* pVertex = m_pCurrentVertexShader->GetVertex();

  pJumpHelper->Put(pVertex->MakeEnableAttrCommand(pJumpHelper->GetCurrentPtr()));
  pJumpHelper->Put(pVertex->MakeDrawCommand(pJumpHelper->GetCurrentPtr(), *pIndexStream));
  pJumpHelper->Put(pVertex->MakeDisableAttrCommand(pJumpHelper->GetCurrentPtr()));
}

// コマンド追加：コピー
void CTRGL::AddCommandCopy_(nn::gr::CommandBufferJumpHelper* pJumpHelper, const u32* pCommand, u32 commandSize)
{
  pJumpHelper->CopyFrom(pCommand, commandSize);
}

// コマンド追加：ジャンプ
void CTRGL::AddCommandJump_(nn::gr::CommandBufferJumpHelper* pJumpHelper, const u32* pCommand, u32 commandSize)
{
  pJumpHelper->Jump((uptr)pCommand, commandSize);
}

// コマンドのカレントポインタ取得
u32* CTRGL::GetCurrentCommandPtr_(nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  return pJumpHelper->GetCurrentPtr();
}

// コマンドのカレントポインタ設定
void CTRGL::SetCurrentCommandPtr_(nn::gr::CommandBufferJumpHelper* pJumpHelper, u32* pCurrent)
{
  return pJumpHelper->Put(pCurrent);
}

// コマンドリストとジャンプヘルパーの同期：前処理
void CTRGL::SyncCommandListAndJumpHelperBefore_(CTRCommandList* pCommandList, nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  // ダーティフラグが立っている全コマンドフラッシュ
  this->FlushAllDirtyCommand_(pJumpHelper);

  // ジャンプするか？
  u32 sizeToFirstJump = pJumpHelper->GetSizeToFirstJump();
  if (sizeToFirstJump > 0)
  {
    // コマンドバッファのサイズが16の倍数になるように調整
    pJumpHelper->Put(nn::gr::AddDummyDataForCommandBuffer(pJumpHelper->GetCurrentPtr(), pJumpHelper->GetSize() + sizeof(u32) * 6));

    // ヘルパーとコマンドリストのアドレスを揃える
    pCommandList->MoveCommandbufferPointer((pJumpHelper->GetCurrentPtr() - pCommandList->GetCurrentBufferAddr()) * sizeof(u32));

    // ジャンプ用の区切りコマンド追加
    pCommandList->Flush3DCommandPartially(sizeToFirstJump);

    // ジャンプする
    pJumpHelper->FinalizeJump(pCommandList->GetCurrentBufferAddr());
  }
  else
  {
    // ジャンプしない

    // コマンドバッファの開始アドレスが16の倍数になるように調整
    u32* pCommand = pJumpHelper->GetCurrentPtr();
    while (reinterpret_cast<uptr>(pCommand) % 16 != 0)
    {
      *pCommand = 0;
      ++pCommand;
    }
    pJumpHelper->Put(pCommand);

    // ヘルパーとコマンドリストのアドレスを揃える
    pCommandList->MoveCommandbufferPointer((pJumpHelper->GetCurrentPtr() - pCommandList->GetCurrentBufferAddr()) * sizeof(u32));
  }
}

// コマンドリストとジャンプヘルパーの同期：終了処理
void CTRGL::SyncCommandListAndJumpHelperAfter_(CTRCommandList* pCommandList, nn::gr::CommandBufferJumpHelper* pJumpHelper)
{
  // ヘルパーとコマンドリストのアドレスを揃える
  *pJumpHelper = nn::gr::CommandBufferJumpHelper(pCommandList->GetCurrentBufferAddr());
}

// 立体視用の行列取得
void CTRGL::GetStereoMatrix(const gfl2::math::Matrix44& projectionMatrix, const gfl2::math::Matrix34& viewMatrix, f32 depthLevel, f32 factor,
                            gfl2::math::Matrix44* out_pProjL, gfl2::math::Matrix34* out_pViewL, gfl2::math::Matrix44* out_pProjR, gfl2::math::Matrix34* out_pViewR)
{
  b32 isStereoMode = IsStereoModeEnable() && nn::gx::IsStereoVisionAllowed();
  if (!isStereoMode)
  {
    // 立体視無効ならfactorを0にする
    factor = 0.0f;
  }
  s_Gp->m_StereoCamera.SetBaseFrustum(reinterpret_cast<const nn::math::MTX44*>(&projectionMatrix));
  s_Gp->m_StereoCamera.SetBaseCamera(reinterpret_cast<const nn::math::MTX34*>(&viewMatrix));

  s_Gp->m_StereoCamera.CalculateMatrices(
    reinterpret_cast<nn::math::MTX44*>(out_pProjL),
    reinterpret_cast<nn::math::MTX34*>(out_pViewL),
    reinterpret_cast<nn::math::MTX44*>(out_pProjR),
    reinterpret_cast<nn::math::MTX34*>(out_pViewR),
    depthLevel, factor);
}

const void* CTRGL::GetDisplayBuffer_(const DisplayDeviceHandle hDisplay)
{
  DisplayWindow* const pDisplay = static_cast< DisplayWindow* >( hDisplay );
  const void* displayBuffer = pDisplay->GetDisplayBuffer();

  return displayBuffer;
}

uptr CTRGL::GetDepthStencilBufferStatic_(const DisplayDeviceHandle hDisplay)
{
  return s_Gp->m_pDefaultDepthStencilBuffer->GetHandle().depthStencilBuffer.virtualAddr;
}

u32 CTRGL::GetFrameBufferHeight_()
{
  return FRAMEBUFFER_HEIGHT;
}

u32 CTRGL::GetFrameBufferWidth_()
{
  return FRAMEBUFFER_WIDTH;
}

void CTRGL::AddTransferColorBufferCommand(void* pDestAddr)
{
  u32 frameBufferWidth = FRAMEBUFFER_HEIGHT;   // 256
  u32 frameBufferHeight = FRAMEBUFFER_WIDTH;   // 512の順番になるはずなので入れ替える

  // pDestAddrに転送、元の形式のまま
  s_Gp->GetCurrentCommandList_()->AddBlockImageCopyCommand(
    reinterpret_cast<GLvoid*>(s_Gp->m_GrFrameBuffer.colorBuffer.virtualAddr),
    frameBufferHeight,  // 256
    0,
    pDestAddr,
    frameBufferHeight,
    0,
    frameBufferHeight * frameBufferWidth * 4   // 256 * 512の順番になるはず
    );
}

void CTRGL::AddTransferLinearColorBufferCommand(void* pDestAddr, b32 includeAlpha)
{
  GLsizei blockSize = 8;

  u32 frameBufferWidth = FRAMEBUFFER_HEIGHT;   // 256
  u32 frameBufferHeight = FRAMEBUFFER_WIDTH;   // 512の順番になるはずなので入れ替える

  // ｐDestAddrに転送 ブロック形式からリニア形式へ -> B2L
  s_Gp->GetCurrentCommandList_()->AddB2LTransferCommand(
    reinterpret_cast<GLvoid*>(s_Gp->m_GrFrameBuffer.colorBuffer.virtualAddr),
    frameBufferHeight,  // 256
    frameBufferWidth,   // 512の順番になるはず
    GL_RGBA8_OES,
    pDestAddr,
    frameBufferHeight,  // 256
    frameBufferWidth,   // 512の順番になるはず
    includeAlpha? GL_RGBA8_OES : GL_RGB8_OES,
    NN_GX_ANTIALIASE_NOT_USED,
    false,
    blockSize
    );
}

void CTRGL::AddReadColorBufferToTextureCommand(Texture* pTexture)
{
  AddTransferColorBufferCommand(reinterpret_cast<GLvoid*>(reinterpret_cast<CTRTexture*>(pTexture)->GetVirtualAddr()));
}

// VRAMへのDMA転送コマンドを追加
void CTRGL::AddVramDmaCommand(const void* pSourceAddr, void* pDestAddr, u32 size)
{
  CTRCommandList* pCommandList = s_Gp->GetCurrentCommandList_();
  nn::gr::CommandBufferJumpHelper* pJumpHelper = s_Gp->GetJumpHelper();

  // 同期開始
  s_Gp->SyncCommandListAndJumpHelperBefore_(pCommandList, pJumpHelper);

  // DMA転送
  pCommandList->AddVramDmaCommandNoCacheFlush(pSourceAddr, pDestAddr, size);

  // 同期終了
  s_Gp->SyncCommandListAndJumpHelperAfter_(pCommandList, pJumpHelper);
}

// GPUが実行中か確認
bool CTRGL::IsGPURunning()
{
  CTRCommandList* pCurrentCommandList = s_Gp->GetCurrentCommandList();
  CTRCommandList* pPrevCommandList = s_Gp->GetPrevCommandList();

  return (pCurrentCommandList->GetIsRunning() || pPrevCommandList->GetIsRunning());
}

#if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)

//! @brief コマンドバッファの使用量取得関数
//! @return コマンドバッファの使用量(GFGL未初期化時は0を返す)
//! @attention GFGL::SwapDisplay関数後など描画コマンドを積み終わった後で取得してください。
u32 CTRGL::GetCommandListUsedBufferSize()
{
  if ( s_Gp == NULL )
  {
    return 0;
  }

  CTRCommandList* pCommandList = s_Gp->GetCurrentCommandList_();
  return pCommandList->GetUsedBufferSize();
}

//! @brief コマンドバッファサイズ取得関数
//! @return コマンドバッファサイズ(GFGL未初期化時は0を返す)
u32 CTRGL::GetCommandListMaxBufferSize()
{
  if ( s_Gp == NULL )
  {
    return 0;
  }

  CTRCommandList* pCommandList = s_Gp->GetCurrentCommandList_();
  return pCommandList->GetMaxBufferSize();
}

//! @brief コマンドリクエスト使用数取得関数
//! @return コマンドリクエスト使用数(GFGL未初期化時は0を返す)
//! @attention GFGL::SwapDisplay関数後など描画コマンドを積み終わった後で取得してください。
u32 CTRGL::GetCommandListUsedRequestCount()
{
  if ( s_Gp == NULL )
  {
    return 0;
  }

  CTRCommandList* pCommandList = s_Gp->GetCurrentCommandList_();
  return pCommandList->GetUsedRequestCount();
}

//! @brief コマンドリクエスト使用最大数取得関数
//! @return コマンドリクエスト使用最大数(GFGL未初期化時は0を返す)
u32 CTRGL::GetCommandListMaxRequestCount()
{
  if ( s_Gp == NULL )
  {
    return 0;
  }

  CTRCommandList* pCommandList = s_Gp->GetCurrentCommandList_();
  return pCommandList->GetMaxRequestCount();
}

#endif // GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP

#if GFL_DEBUG
//----------------------------------------------------------------------------
/**
 *   @brief デバッグ掲示関数(ライブラリの使用状況報告)
*/
//-----------------------------------------------------------------------------
void CTRGL::DebugDump()
{
#define   WARNING_PERCENTAGE    (0.9f)//使用率が９割を超えたら警告

  CTRCommandList* pCommandList = this->GetCurrentCommandList_();

  {
    u32       usedBufferSize = pCommandList->GetUsedBufferSize();
    u32       maxBufferSize = pCommandList->GetMaxBufferSize();
    f32       usedPercentage = static_cast<f32>(usedBufferSize) / static_cast<f32>(maxBufferSize);

    if ( usedPercentage > WARNING_PERCENTAGE )
    {
      GFL_RELEASE_PRINT( "[gfx] warning : コマンドバッファの空きが少なくなっています。使用サイズ %d / %d\n", usedBufferSize, maxBufferSize );
    }
  }

  {
    u32       usedRequestCount = pCommandList->GetUsedRequestCount();
    u32       maxRequestCount = pCommandList->GetMaxRequestCount();
    f32       usedPercentage = static_cast<f32>(usedRequestCount) / static_cast<f32>(maxRequestCount);

    if ( usedPercentage > WARNING_PERCENTAGE )
    {
      GFL_RELEASE_PRINT( "[gfx] warning : リクエストの空きが少なくなっています。使用数 %d / %d\n", usedRequestCount, maxRequestCount );
    }
  }
}
#endif

}}}//gfl2::gfx::ctr
