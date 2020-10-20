//==============================================================================
/**
 * @file    LayoutArcTest.cpp
 * @author  fukushima_yuya
 * @date    2015.02.09
 * @brief   レイアウトのリソース読み込みテスト
 */
//==============================================================================
#if PM_DEBUG

// niji
#include <Test/LayoutArcTest/include/LayoutArcTest.h>
#include <GameSys/include/GameProcManager.h>

// gflib2
#include <System/include/HeapDefine.h>
#include <system/include/GflUse.h>
#include <system/include/nijiAllocator.h>

// arc読み込み
#include <arc_index/conv_test.gaix>
#include <arc_def_index/arc_def.h>

// data  // @todo Win32, CTR共通のヘッダファイルになるので、共通の置き場所をつくってそこに持って行きたい
#include <niji_conv_header/app/test/fukushima_test/layout_arc_test/res2d/conv_test.h>
#include <niji_conv_header/app/test/fukushima_test/layout_arc_test/res2d/conv_test_pane.h>
#include <niji_conv_header/app/test/fukushima_test/layout_arc_test/res2d/conv_test_anim_list.h>



GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(LytArc)

//------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
//------------------------------------------------------------------------------
LayoutArcProc::LayoutArcProc( void )
: m_count(0)
, m_pMemHeap(NULL)
, m_pDevHeap(NULL)
, m_pMemAllocator(NULL)
, m_pDevAllocator(NULL)
, m_pLytSys(NULL)
, m_pLytWk(NULL)
, m_pResID(NULL)
, m_pEnv(NULL)
, m_pFont(NULL)
, m_pLytAccessor(NULL)
#if !defined(DISABLE_PIPELINE_20151214)
, m_pRenderingPipeLine(NULL)
#endif
, m_pUtilTextRenderingPipeLine(NULL)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
//------------------------------------------------------------------------------
LayoutArcProc::~LayoutArcProc( void )
{

}



//==============================================================================
/**
 * @brief    GameSys::GameProc関数
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief    PROCの初期化
 * @param    pManager
 */
//------------------------------------------------------------------------------
gfl2::proc::Result LayoutArcProc::InitFunc( gfl2::proc::Manager* pManager )
{
  GFL_PRINT("LayoutArcProc::InitFunc\n");

  m_pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );  // @todo 専用ヒープをつくること
  GFL_ASSERT( m_pMemHeap != NULL );
  m_pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );              // @todo 専用ヒープをつくること
  GFL_ASSERT( m_pDevHeap != NULL );

  m_pNijiAllocator = GFL_NEW(m_pDevHeap) System::nijiAllocator( m_pDevHeap );
  GFL_ASSERT( m_pNijiAllocator );
    
#if !defined(DISABLE_PIPELINE_20151214)
  m_pRenderingPipeLine  = GFL_NEW( pManager->GetHeap() ) MyRenderingPipeLine( m_pDevHeap, 512, 256 );
  GFL_ASSERT( m_pRenderingPipeLine != NULL );
#endif
  m_pUtilTextRenderingPipeLine = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator, 512, 256 );
  GFL_ASSERT( m_pUtilTextRenderingPipeLine != NULL );

  this->Initialize();

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief    PROCのメイン更新
 * @param    pManager
 */
//------------------------------------------------------------------------------
gfl2::proc::Result LayoutArcProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  GFL_PRINT("LayoutArcProc::UpdateFunc\n");
  
  this->Update();  // @todo DrawFuncで行うべきか？

  if(++m_count >= 1000)
  {
    return gfl2::proc::RES_FINISH;
  }
  else
  {
    return gfl2::proc::RES_CONTINUE;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief    PROCのメイン描画
 * @param    pManager
 * @param    displayNo
 */
//------------------------------------------------------------------------------
void LayoutArcProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  GFL_PRINT("LayoutArcProc::DrawFunc\n");

  switch( displayNo )
  {
    // 左画面描画
  case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      //m_pRenderingPipeLine->SetProjectionMatrix( m_ProjMatTable[ displayNo ] );
      //m_pRenderingPipeLine->SetViewMatrix( m_ViewMatTable[ displayNo ] );  

#if !defined(DISABLE_PIPELINE_20151214)
      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
#endif
      break;
    } // case gfl2::gfx::CtrDisplayNo::LEFT

    // 右画面描画(立体視時のみ)
  case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      //m_pRenderingPipeLine->SetProjectionMatrix( m_ProjMatTable[ displayNo ] );
      //m_pRenderingPipeLine->SetViewMatrix( m_ViewMatTable[ displayNo ] );  

#if !defined(DISABLE_PIPELINE_20151214)
      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
#endif
      break;
    } // case gfl2::gfx::CtrDisplayNo::RIGHT

    // 下画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN:
    {


      break;
    } // case gfl2::gfx::CtrDisplayNo::DOWN

  } // switch( displayNo )

  switch( displayNo )
  {
    // 左画面描画
  case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      //m_pUtilTextRenderingPipeLine->SetProjectionMatrix( m_ProjMatTable[ displayNo ] );
      //m_pUtilTextRenderingPipeLine->SetViewMatrix( m_ViewMatTable[ displayNo ] );  

      m_pUtilTextRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pUtilTextRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pUtilTextRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pUtilTextRenderingPipeLine->Execute();
      break;
    } // case gfl2::gfx::CtrDisplayNo::LEFT

    // 右画面描画(立体視時のみ)
  case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      //m_pUtilTextRenderingPipeLine->SetProjectionMatrix( m_ProjMatTable[ displayNo ] );
      //m_pUtilTextRenderingPipeLine->SetViewMatrix( m_ViewMatTable[ displayNo ] );  

      m_pUtilTextRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pUtilTextRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pUtilTextRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pUtilTextRenderingPipeLine->Execute();
      break;
    } // case gfl2::gfx::CtrDisplayNo::RIGHT

    // 下画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN:
    {


      break;
    } // case gfl2::gfx::CtrDisplayNo::DOWN

  } // switch( displayNo )
}

//------------------------------------------------------------------------------
/**
 * @brief    PROCのメインを行う
 * @param    pManager
 */
//------------------------------------------------------------------------------
gfl2::proc::Result LayoutArcProc::NextFrameMainFunc( gfl2::proc::Manager* pManager )
{
  GFL_PRINT("LayoutArcProc::NextFrameMainFunc\n");
  
  //GameSys::GameManager* pGameManager = mpGameProcManager->GetGameManager();
  
  // @todo NextFrameMainFuncでもRES_FINISHを返す必要があるか？  kawada_koji_20150209
  if(m_count >= 100)
  {
    return gfl2::proc::RES_FINISH;
  }
  else
  {
    return gfl2::proc::RES_CONTINUE;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief    PROCの終了を行う
 * @param    pManager
 */
//------------------------------------------------------------------------------
gfl2::proc::Result LayoutArcProc::EndFunc( gfl2::proc::Manager* pManager )
{
  GFL_PRINT("LayoutArcProc::EndFunc\n");

  this->Finalize();  // @todo 描画終了後1フレーム空けるのをもっとうまくやらねばならない

  GFL_SAFE_DELETE( m_pUtilTextRenderingPipeLine );
#if !defined(DISABLE_PIPELINE_20151214)
  GFL_SAFE_DELETE( m_pRenderingPipeLine );
#endif
  GFL_SAFE_DELETE( m_pNijiAllocator );

  m_pMemHeap = NULL;
  m_pDevHeap = NULL;

  return gfl2::proc::RES_FINISH;
}



//==============================================================================
/**
 * @brief    LayoutArcProc::LayoutPartsPro関数
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief    初期化処理
 */
//------------------------------------------------------------------------------
void LayoutArcProc::Initialize( void )
{
  m_pMemAllocator = GFL_NEW(m_pMemHeap) gfl2::heap::NwAllocator( m_pMemHeap );
  m_pDevAllocator = GFL_NEW(m_pDevHeap) gfl2::heap::NwAllocator( m_pDevHeap );

  gfl2::heap::HeapBase* pMemHeapForTemp = m_pMemHeap;  // @todo 一時メモリの確保場所をきちんとすること

  // LytSys初期化
  InitLytSys();
  // フォントの読み込み
  LoadFont( pMemHeapForTemp );
  // GARCの読み込み
  //LoadGARC( pMemHeapForTemp );
  // AppLytの読み込み
  LoadAppLyt( pMemHeapForTemp );
  // レイアウトワークの生成
  CreateLytWk();
  // 環境の生成
  CreateEnv();

  // DrawUtilTextの生成
  mpDrawUtilText = GFL_NEW( m_pMemHeap ) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator);
  mpDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pUtilTextRenderingPipeLine->GetUtilTextRenderPath() );
  mpDrawUtilText->SetFont( m_pFont );
}

//------------------------------------------------------------------------------
/**
 * @brief    終了処理
 */
//------------------------------------------------------------------------------
void LayoutArcProc::Finalize( void )
{
  mpDrawUtilText->Terminate();
  GFL_SAFE_DELETE(mpDrawUtilText);

#if !defined(DISABLE_PIPELINE_20151214)
  gfl2::lyt::LayoutRenderPath* pRenderPath =
    m_pRenderingPipeLine->GetLayoutRenderPath();
  pRenderPath->ClearDrawableLytWk();
#endif

  // GARCの破棄
  DeleteGARC();

  GFL_SAFE_DELETE( m_pEnv );
  GFL_SAFE_DELETE( m_pResID );

  if( m_pFont )
  {
    void* font_buffer = m_pFont->RemoveResource();
    GflHeapFreeMemory( font_buffer );
    GFL_SAFE_DELETE( m_pFont );
  }

  GFL_SAFE_DELETE( m_pLytAccessor );
  GFL_SAFE_DELETE( m_pLytSys );

  m_pLytWk = NULL;

  // レイアウトシステムの終了
  gfl2::lyt::LytSys::FinalizeSystem();
  GFL_SAFE_DELETE( m_pDevAllocator );
  GFL_SAFE_DELETE( m_pMemAllocator );
}

//------------------------------------------------------------------------------
/**
 * @brief    更新処理
 */
//------------------------------------------------------------------------------
void LayoutArcProc::Update( void )
{
#if !defined(DISABLE_PIPELINE_20151214)
  gfl2::lyt::LayoutRenderPath* pRenderPath =
      m_pRenderingPipeLine->GetLayoutRenderPath();
  if( m_pLytSys )
  {
    pRenderPath->ClearDrawableLytWk();
    pRenderPath->AddDrawableLytWk( m_pLytWk, gfl2::lyt::DISPLAY_UPPER );
  }
#endif
  mpDrawUtilText->ClearDrawText();

  mpDrawUtilText->SetTextScale( 0.5f, 0.5f );
  mpDrawUtilText->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  mpDrawUtilText->DrawText( 0, 50, L"DrawUtilTextProc" );

  mpDrawUtilText->SetTextScale( 1.0f, 1.0f );
  mpDrawUtilText->DrawText( 0, 100, L"UtilTextRenderPath", gfl2::math::Vector4(0.2f, 0.5f, 0.8f, 1.0f) );

  mpDrawUtilText->SetTextScale( 1.2f, 1.2f );
  mpDrawUtilText->DrawText( 0, 150, L"UtilTextRenderingPipeLine", gfl2::math::Vector4(0.9f, 0.2f, 0.3f, 1.0f), gfl2::math::Vector4(0.4f, 0.9f, 0.6f, 1.0f) );

  mpDrawUtilText->SetTextScale( 0.7f, 0.7f );
  mpDrawUtilText->SetTextColor( gfl2::math::Vector4( 0.5f, 0.8f, 0.1f, 1.0f ) );
  mpDrawUtilText->DrawText( 0, 200, L"DrawText" );
}


//------------------------------------------------------------------------------
/**
 * @brief    レイアウトシステムの初期化
 */
//------------------------------------------------------------------------------
void LayoutArcProc::InitLytSys( void )
{
  const gfl2::lyt::LytSysSetUp c_LytSysSetup = {
    GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes(),
    5, 5
  };

  m_pLytSys = GFL_NEW( m_pMemHeap ) gfl2::lyt::LytSys( &c_LytSysSetup, m_pMemAllocator, m_pDevAllocator, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetDrawingLytWkManager() );

  GFL_ASSERT( m_pLytSys != NULL );
}

//------------------------------------------------------------------------------
/**
 * @brief    フォントの読み込み
 * @param    pTempHeap    一時確保用メモリ
 */
//------------------------------------------------------------------------------
void LayoutArcProc::LoadFont( gfl2::heap::HeapBase* pTempHeap )
{
  m_pFont = gfl2::str::CreateResFont(
              "data/app/test/fukushima_test/layout_arc_test/fonts/cbf_std_kujira.bffnt",
              m_pMemHeap,
              m_pDevHeap,
              pTempHeap );

  GFL_ASSERT( m_pFont != NULL );
}

//------------------------------------------------------------------------------
/**
 * @brief    GARCの読み込み
 * @param    pTempHeap    一時確保用メモリ
 */
//------------------------------------------------------------------------------
void LayoutArcProc::LoadGARC( gfl2::heap::HeapBase* pTempHeap )
{
  GameSys::GameManager* pGameManager = mpGameProcManager->GetGameManager();
  gfl2::fs::AsyncFileManager* pAsyncFileMan = pGameManager->GetAsyncFileManager();

  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.arcId        = ARCID_LYTARCTEST;
  req.heapForFile  = m_pMemHeap;
  req.heapForReq  = pTempHeap;

  pAsyncFileMan->SyncArcFileOpen( req );

  // リソースの読み込み
  LoadResource();
}

//------------------------------------------------------------------------------
/**
 * @brief    AppLytの読み込み
 * @param    pTempHeap    一時確保用メモリ
 */
//------------------------------------------------------------------------------
void LayoutArcProc::LoadAppLyt( gfl2::heap::HeapBase* pTempHeap )
{
  m_pLytAccessor = GFL_NEW( m_pMemHeap ) app::util::AppLytAccessor();
  GFL_ASSERT( m_pLytAccessor != NULL );

  GameSys::GameManager* pGameManager = mpGameProcManager->GetGameManager();
  gfl2::fs::AsyncFileManager* pAsyncFileMan = pGameManager->GetAsyncFileManager();
  gfl2::fs::Result fsResult;
  void* pLytArcBinary = NULL;

  char* filePath = "data/app/test/fukushima_test/layout_arc_test/arcs/conv_test.applyt";

  gfl2::fs::AsyncFileManager::FileEasyReadReq req;
  req.fileName          = filePath;                                     // [in]  ファイル名。終端は'\0'になっていること。
  req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
  req.ppBuf             = &pLytArcBinary;                                // [out] heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。  // ****キー****
  req.pBufSize          = NULL;                                         // [out] heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもよい。
  req.pFileSize         = NULL;                                         // [out] ファイルサイズ。(*pBufSize >= *pFileSize)となる。NULLでもよい。
  req.heapForBuf        = m_pDevHeap;                                   // [in]  ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。←デバイスヒープ
  req.align             = 128;                                          // [in]  heapForBufから確保する際のアラインメント  @todo NWを使っちゃダメ！
  req.heapForReq        = pTempHeap;                                    // [in]  リクエスト終了まで一時的に使用するヒープ。
  req.heapForCompressed = NULL;                                         // [in]  解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。
  req.result            = &fsResult;                                    // [out] 結果。NULLでもよい。

  pAsyncFileMan->SyncFileEasyRead( req );
  if( !fsResult.IsSuccess() )
  {
    GFL_ASSERT(0);
  }

  m_pLytAccessor->SetAppLytBuf( pLytArcBinary );
  
  // リソースの読み込み
  LoadResource( m_pLytAccessor->GetLytDat() );
}

//------------------------------------------------------------------------------
/**
 * @brief    リソースの読み込み
 */
//------------------------------------------------------------------------------
void LayoutArcProc::LoadResource( void )
{
  GameSys::GameManager* pGameManager = mpGameProcManager->GetGameManager();
  gfl2::fs::AsyncFileManager* pAsyncFileMan = pGameManager->GetAsyncFileManager();

  m_pResID = GFL_NEW( m_pMemHeap ) gfl2::lyt::LytMultiResID();
  GFL_ASSERT( m_pResID != NULL );

  *m_pResID = m_pLytSys->InitMultiResource( m_pMemHeap, 1 );
  *m_pResID = m_pLytSys->LoadLytRes( pAsyncFileMan, m_pResID, m_pDevAllocator, ARCID_LYTARCTEST, GARC_conv_test_conv_test_APPLYT, true );
  m_pLytSys->TransferAllTexture( m_pResID );

  // フォントリソースの登録
  gfl2::lyt::LytResourceAccessor* pAccessor =
    m_pLytSys->GetResource( m_pResID )->GetResourceAccessor();
  if( pAccessor ) pAccessor->RegistFont( "cbf_std_kujira.bffnt", m_pFont );
}

//------------------------------------------------------------------------------
/**
 * @brief    リソースの読み込み
 * @param    pBinary    lytdatバイナリデータ
 */
//------------------------------------------------------------------------------
void LayoutArcProc::LoadResource( const void* pBinary )
{
  m_pResID = GFL_NEW( m_pMemHeap ) gfl2::lyt::LytMultiResID();
  GFL_ASSERT( m_pResID != NULL );

  *m_pResID = m_pLytSys->InitMultiResource( m_pMemHeap, 1 );
  *m_pResID = m_pLytSys->LoadLytRes( m_pResID, m_pDevAllocator, pBinary );
  m_pLytSys->TransferAllTexture( m_pResID );

  // フォントリソースの登録
  gfl2::lyt::LytResourceAccessor* pAccessor =
    m_pLytSys->GetResource( m_pResID )->GetResourceAccessor();
  if( pAccessor ) pAccessor->RegistFont( "cbf_std_kujira.bffnt", m_pFont );
}

//------------------------------------------------------------------------------
/**
 * @brief    部品レイアウトの読み込み
 */
//------------------------------------------------------------------------------
void LayoutArcProc::LoadLytParts()
{
  m_pResID = GFL_NEW( m_pMemHeap ) gfl2::lyt::LytMultiResID();
  GFL_ASSERT( m_pResID != NULL );

  *m_pResID = m_pLytSys->InitMultiResource( m_pMemHeap, 1 );
}

//------------------------------------------------------------------------------
/**
 * @brief    GARCの破棄
 */
//------------------------------------------------------------------------------
void LayoutArcProc::DeleteGARC( void )
{
  GameSys::GameManager* pGameManager = mpGameProcManager->GetGameManager();
  gfl2::fs::AsyncFileManager* pAsyncFileMan = pGameManager->GetAsyncFileManager();

  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId = ARCID_LYTARCTEST;
       
  pAsyncFileMan->SyncArcFileClose(req);
}

//------------------------------------------------------------------------------
/**
 * @brief    LytWkの生成
 */
//------------------------------------------------------------------------------
void LayoutArcProc::CreateLytWk( void )
{
  const gfl2::lyt::LytWkSetUp c_LytWkSetup[] = {
    {
      gfl2::math::Vector3( 0, 0, 0 ),    // pos
      gfl2::math::Vector2( 1, 1 ),        // scale
      gfl2::math::Vector3( 0, 0, 0 ),    // rotate
      0,                              // priority
      0
    },
    {
      gfl2::math::Vector3( 0, -290, 0 ),  // pos
      gfl2::math::Vector2( 1, 1 ),        // scale
      gfl2::math::Vector3( 0, 0, 0 ),    // rotate
      0,                              // priority
      0
    }
  };

  if( m_pLytSys )
  {
    m_pLytWk = m_pLytSys->CreateLytWk(
                  m_pResID,
                  LYTRES_CONV_TEST_ONE_PICTURE_PANE_TEST_BFLYT,
                  NULL,
                  0,
                  0,
                  &c_LytWkSetup[0] );
    GFL_ASSERT( m_pLytWk != NULL );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief    環境の生成
 */
//------------------------------------------------------------------------------
void LayoutArcProc::CreateEnv( void )
{
  // カメラ設定
  const gfl2::lyt::ProjectionParam proj_param[] = {
    {
      gfl2::lyt::PROJECTION_ORTHO,
      { -200.0f, 200.0f, -120.0f, 120.0f },
      0.0f,
      300.0f,
      gfl2::math::PIVOT_UPSIDE_TO_TOP
    },
    {
      gfl2::lyt::PROJECTION_ORTHO,
      { -160.0f, 160.0f, -120.0f, 120.0f },
      0.0f,
      300.0f,
      gfl2::math::PIVOT_UPSIDE_TO_TOP
    }
  };
  const gfl2::lyt::ViewParam view_param[] = {
    {// 上画面(左目)
      gfl2::math::Vector3(0.f, 0.f, 1.f),
      gfl2::math::Vector3(0.f, 1.f, 0.f),
      gfl2::math::Vector3(0.f, 0.f, 0.f),
    },
    {// 上画面(右目)
      gfl2::math::Vector3(10,0,1),  // カメラ座標
      gfl2::math::Vector3(0,1,0),   // 上方向
      gfl2::math::Vector3(10,0,0),  // ターゲット座標
    },
    { // 下画面
      gfl2::math::Vector3(0.f, -290.f, 1.f),
      gfl2::math::Vector3(0.f, 1.f, 0.f),
      gfl2::math::Vector3(0.f, -290.f, 0.f),
    }
  };

  m_pEnv = GFL_NEW( m_pMemHeap ) gfl2::lyt::Env();
  GFL_ASSERT( m_pEnv != NULL );

  m_pEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_NORMAL, proj_param[0], view_param[0] );
  m_pEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_RIGHT, proj_param[0], view_param[1] );
  if( m_pLytWk ) m_pLytWk->SetUserEnv( m_pEnv, NULL );
}

GFL_NAMESPACE_END(LytArc)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
