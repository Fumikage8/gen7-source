//==============================================================================
/**
 @file    DrawUtilTextProc.h
 @author  fukushima_yuya
 @date    2015.02.19
 @brief   DrawUtilTextのTestProc
 */
//==============================================================================
#if PM_DEBUG

#include <Test/DrawUtilTextTest/include/DrawUtilTextProc.h>

// gflib2
#include <debug/include/gfl2_DebugPrint.h>

// heap
#include <System/include/nijiAllocator.h>
#include <System/include/HeapDefine.h>
#include <system/include/GflUse.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(UtilText)

static u32 counter = 0;


DrawUtilTextProc::DrawUtilTextProc( void )
:m_count(0)
{
  m_pMemAllocator = NULL;
  m_pDevAllocator = NULL;
  m_pNijiAllocator = NULL;
  m_pRenderingPipeLine = NULL;
  m_pResFont = NULL;
}

DrawUtilTextProc::~DrawUtilTextProc( void )
{
}


gfl2::proc::Result DrawUtilTextProc::InitFunc( gfl2::proc::Manager* pManager )
{
  GFL_PRINT("DrawUtilTextProc::InitFunc\n");

  gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );  // @todo 専用ヒープをつくること
  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );              // @todo 専用ヒープをつくること
  
  m_pNijiAllocator = GFL_NEW(pDevHeap) System::nijiAllocator( pDevHeap );
  GFL_ASSERT( m_pNijiAllocator );

  // レンダリングパイプラインの生成
  m_pRenderingPipeLine  = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator, 512, 256 );
  // 初期化処理
  this->Initialize( pMemHeap, pDevHeap );

  return gfl2::proc::RES_FINISH;
}

gfl2::proc::Result DrawUtilTextProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  GFL_PRINT("DrawUtilTextProc::UpdateFunc\n");
  // 更新処理
  this->Update();

  if(++m_count >= 1000)
  {
    return gfl2::proc::RES_FINISH;
  }
  else
  {
    return gfl2::proc::RES_CONTINUE;
  }
}

void DrawUtilTextProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  GFL_PRINT("DrawUtilTextProc::DrawFunc\n");

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

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
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

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
      break;
    } // case gfl2::gfx::CtrDisplayNo::RIGHT

    // 下画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN:
    {

      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32                  displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      //m_pRenderingPipeLine->SetProjectionMatrix( m_ProjMatTable[ displayNo ] );
      //m_pRenderingPipeLine->SetViewMatrix( m_ViewMatTable[ displayNo ] );  

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();

      break;
    } // case gfl2::gfx::CtrDisplayNo::DOWN

  } // switch( displayNo )
}

gfl2::proc::Result DrawUtilTextProc::NextFrameMainFunc( gfl2::proc::Manager* pManager )
{
  GFL_PRINT("DrawUtilTextProc::NextFrameMainFunc\n");
  
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

gfl2::proc::Result DrawUtilTextProc::EndFunc( gfl2::proc::Manager* pManager )
{
  GFL_PRINT("DrawUtilTextProc::EndFunc\n");

  // 終了処理
  this->Finalize();

  GFL_SAFE_DELETE( m_pNijiAllocator );

  return gfl2::proc::RES_FINISH;
}


//-----------------------------------------------------------------------
/**
 * @brief   初期化処理
 */
//-----------------------------------------------------------------------
void DrawUtilTextProc::Initialize( gfl2::heap::HeapBase* pMemHeap, gfl2::heap::HeapBase* pDevHeap )
{
  gfl2::heap::HeapBase* pMemHeapForTemp = pMemHeap;

  // アロケーターの生成
  m_pMemAllocator = GFL_NEW(pMemHeap) gfl2::heap::NwAllocator( pMemHeap );
  m_pDevAllocator = GFL_NEW(pDevHeap) gfl2::heap::NwAllocator( pDevHeap );

  // フォントの生成
  m_pResFont = gfl2::str::CreateResFont( "data/app/test/fukushima_test/layout_sample_data/fonts/cbf_std_kujira.bffnt", pMemHeap, pDevHeap, pMemHeapForTemp );

  // DrawUtilTextの生成
  mpDrawUtilText = GFL_NEW(pMemHeap) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator);
  mpDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
  //gfl2::util::DrawUtilText::SetRenderPath( m_pRenderingPipeLine->GetUtilTextRenderPath() );
  mpDrawUtilText->SetFont( m_pResFont );
}

//-----------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//-----------------------------------------------------------------------
void DrawUtilTextProc::Finalize( void )
{
  // @todo 描画終了後1フレーム空けねばならない！
  if( m_pRenderingPipeLine != NULL )
  {
    m_pRenderingPipeLine->GetUtilTextRenderPath()->ClearDrawableText();
  }

  // DrawUtilTextの解放
  mpDrawUtilText->Terminate();
  GFL_SAFE_DELETE(mpDrawUtilText);

  // フォントの破棄
  void* font_buffer = m_pResFont->RemoveResource();
  GflHeapFreeMemory( font_buffer );
  GFL_SAFE_DELETE(m_pResFont);

  // アロケーターの解放
  GFL_SAFE_DELETE( m_pDevAllocator );
  GFL_SAFE_DELETE( m_pMemAllocator );
}

//-----------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//-----------------------------------------------------------------------
void DrawUtilTextProc::Update( void )
{
  mpDrawUtilText->ClearDrawText();

  //if( counter == 0 )
  //{
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

  if( counter == 0 )
  {
    mpDrawUtilText->SetTextScale( 0.5f, 0.5f );
    mpDrawUtilText->DrawText( 0, 30, L"DebugWinTop" );
  }
  else {
    mpDrawUtilText->SetTextScale( 0.5f, 0.5f );
    mpDrawUtilText->DrawText( 0, 30, L"しょりふか" );
  }

  counter++;
}

GFL_NAMESPACE_END(UtilText)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
