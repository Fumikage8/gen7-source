// ============================================================================
/*
 * @file QRCodeTestProc.cpp
 * @brief QRCodeのテストです。
 * @date 2015.03.04
 */
// ============================================================================
#if PM_DEBUG

#include "Test/QRCodeTest/include/QRCodeTestProc.h"
#include <system/include/GflUse.h>
#include <ui/include/gfl2_UI.h>
#include <Device/include/Camera/gfl2_DeviceCamera_DefaultSettings.h>
#include <Device/include/Camera/gfl2_DeviceCamera_Controller.h>
#include "Test/QRCodeTest/include/QRTestEventListener.h"
#include <System/include/ThreadPriority.h>
#include <System/include/nijiAllocator.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(QRCodeTest)


QRCodeTestProc::QRCodeTestProc() :
#if GF_PLATFORM_CTR
  m_pCameraSystem( NULL ),
  m_DeviceCameraState( 0 ),
#endif // GF_PLATFORM_CTR
  m_pMemAllocator( NULL ),
  m_pDevAllocator( NULL ),
  m_pNijiAllocator( NULL ),
  m_pRenderingPipeLine( NULL ),
  m_pResFont( NULL ),
  m_pQREncoder( NULL ),
  m_pQRDecoder( NULL ),
  m_pQREventListener( NULL ),
  m_pQRYUVImageInfo( NULL ),
  m_pQREncodeData( NULL ),
  m_YuvImageSize( 0 )
{
}


QRCodeTestProc::~QRCodeTestProc()
{
}


gfl2::proc::Result QRCodeTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
  gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // font
  {
 	  m_pMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
	  m_pDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
    m_pNijiAllocator = GFL_NEW(pDevHeap) System::nijiAllocator( pDevHeap );

    m_pRenderingPipeLine = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator, 512, 256 );
    m_pResFont = gfl2::str::CreateResFont( "data/app/test/fukushima_test/layout_sample_data/fonts/cbf_std_kujira.bffnt", pMemHeap, pDevHeap, pMemHeap );
		mpDrawUtilText = GFL_NEW(pManager->GetHeap()) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator);
		mpDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
	  mpDrawUtilText->SetFont( m_pResFont );
  }

#if GF_PLATFORM_CTR
  // camera
  {
    gfl2::device::camera::System::SetupParam param = gfl2::device::camera::DefaultSettings_Single;
    param.pHeap = pMemHeap;
    param.pDeviceHeap = pDevHeap;
    m_pCameraSystem = GFL_NEW( pMemHeap ) gfl2::device::camera::System( param );
  }
#endif // GF_PLATFORM_CTR

  // QR
  {
    m_pQREncoder = gfl2::qr::IQREncoder::CreateQREncoder( pMemHeap, pDevHeap, System::ThreadPriority::ASYNC_QR_ENCODE_THREAD_PRIORITY );
    m_pQRDecoder = gfl2::qr::IQRDecoder::CreateQRDecoder( pMemHeap, pDevHeap, System::ThreadPriority::ASYNC_QR_DECODE_THREAD_PRIORITY );
    m_pQREventListener = GFL_NEW( pMemHeap ) QRTestEventListener();
    m_pQRYUVImageInfo = GFL_NEW( pDevHeap ) gfl2::qr::QRYUVImageInfo( pDevHeap, 192000 );
    m_pQREncodeData = GFL_NEW( pMemHeap ) gfl2::qr::QREncodeData( pDevHeap, 512 );
  }

  return gfl2::proc::RES_FINISH;
}


gfl2::proc::Result QRCodeTestProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

  m_pQREncoder->Update();
  m_pQRDecoder->Update();

  DisplayPrint();


#if defined( GF_PLATFORM_CTR )

  switch( m_DeviceCameraState )
  {
    case 0:
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        gfl2::device::camera::Controller* pCameraController = m_pCameraSystem->GetCameraController( gfl2::device::camera::CAMERA_1 );
        pCameraController->Activate();
        pCameraController->StartCapture();
        ++m_DeviceCameraState;
      }
    break;

    case 1:
    {
      gfl2::device::camera::Controller* pCameraController = m_pCameraSystem->GetCameraController( gfl2::device::camera::CAMERA_1 );
      m_YuvImageSize = pCameraController->GetYuvImageSize();
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        pCameraController->StopCapture();
        ++m_DeviceCameraState;
      }
      else
      if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
      {
        u8 temp[128];
        m_pQREncodeData->CopyData( temp, 128 );
        m_pQREncoder->EncodeAsync( m_pQREventListener, m_pQREncodeData );

      }
      else
      if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
      {
        m_pQRYUVImageInfo->CopyImage( (const u8*)pCameraController->GetYuvImage(), pCameraController->GetYuvImageSize() );
        m_pQRDecoder->DecodeAsync( m_pQREventListener, m_pQRYUVImageInfo );
      }
    }
    break;

    case 2:
    {
      gfl2::device::camera::Controller* pCameraController = m_pCameraSystem->GetCameraController( gfl2::device::camera::CAMERA_1 );
      if( !pCameraController->IsCapturing() && !m_pQREncoder->IsBusy() && !m_pQRDecoder->IsBusy() )
      {
        pCameraController->DeactivateAllCamera();
        return gfl2::proc::RES_FINISH;
      }
    }
    break;
  }

#elif defined( GF_PLATFORM_WIN32 )

  // win32 版の camera ライブラリは対応していないのでエンコーダとデコーダの単体テスト
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    if( !m_pQREncoder->IsBusy() && !m_pQRDecoder->IsBusy() )
    {
      return gfl2::proc::RES_FINISH;
    }
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    m_pQREncoder->EncodeAsync( m_pQREventListener, m_pQREncodeData );
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    m_pQRDecoder->DecodeAsync( m_pQREventListener, m_pQRYUVImageInfo );
  }

#endif

  return gfl2::proc::RES_CONTINUE;
}


void QRCodeTestProc::DisplayPrint()
{
  mpDrawUtilText->ClearDrawText();


	mpDrawUtilText->SetTextScale( 1.0f, 1.0f );
  mpDrawUtilText->DrawText( 0,  50, L"A Camera Activate" );
  mpDrawUtilText->DrawText( 0,  80, L"B Exit" );
  mpDrawUtilText->DrawText( 0, 110, L"X Encode" );
  mpDrawUtilText->DrawText( 0, 140, L"Y Decode" );
  mpDrawUtilText->DrawText( 0, 170, L"YUVImageSize = %d", m_YuvImageSize );


}


void QRCodeTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  switch( displayNo )
  {
    case gfl2::gfx::CtrDisplayNo::LEFT:
    case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
    }
    break;
  }

}


gfl2::proc::Result QRCodeTestProc::EndFunc( gfl2::proc::Manager* pManager )
{
  // QR
  {
    GFL_SAFE_DELETE( m_pQREncodeData );
    GFL_SAFE_DELETE( m_pQRYUVImageInfo );
    GFL_SAFE_DELETE( m_pQREventListener );
    GFL_SAFE_DELETE( m_pQRDecoder );
    GFL_SAFE_DELETE( m_pQREncoder );
  }

#if GF_PLATFORM_CTR
  // camera
  {
    GFL_SAFE_DELETE( m_pCameraSystem );
  }
#endif // GF_PLATFORM_CTR

  // font
  {
	  mpDrawUtilText->Terminate();
		GFL_SAFE_DELETE(mpDrawUtilText);

    void* pFontBuffer = m_pResFont->RemoveResource();
	  GflHeapFreeMemory( pFontBuffer );
	  GFL_SAFE_DELETE( m_pResFont );

    GFL_SAFE_DELETE( m_pRenderingPipeLine );

    GFL_SAFE_DELETE( m_pNijiAllocator );
    GFL_SAFE_DELETE( m_pDevAllocator );
	  GFL_SAFE_DELETE( m_pMemAllocator );
  }

  return gfl2::proc::RES_FINISH;
}


GFL_NAMESPACE_END(QRCodeTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
