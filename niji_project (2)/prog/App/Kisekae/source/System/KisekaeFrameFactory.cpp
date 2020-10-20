// ============================================================================
/*
 * @file KisekaeFrameFactory.cpp
 * @brief きせかえアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/System/KisekaeFrameFactory.h"
#include "App/Kisekae/source/KisekaeFrameID.h"
#include "App/Kisekae/source/Frame/KisekaeShopFrame.h"
#include "App/Kisekae/source/Frame/KisekaeDressUpFrame.h"
#include "App/Kisekae/source/Frame/KisekaeColorDressUpFrame.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)


FrameFactory::FrameFactory( App::Kisekae::System::Work* pWork ) :
  m_pWork( pWork )
{
}


NetAppLib::System::NetAppFrameBase* FrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case App::Kisekae::KISEKAE_FRAME_ID_SHOP:
    {
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::Frame::ShopFrame( m_pWork );
      pFrame->FadeInRequest();
      pFrame->FadeOutRequest();

      break;
    }

    case App::Kisekae::KISEKAE_FRAME_ID_DRESS_UP:
    {
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::Frame::DressUpFrame( m_pWork );
      pFrame->FadeInRequest();
      pFrame->FadeOutRequest();
      break;
    }

    case App::Kisekae::KISEKAE_FRAME_ID_COLOR_DRESS_UP:
    {
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::Frame::ColorDressUpFrame( m_pWork );
      pFrame->FadeInRequest();
      pFrame->FadeOutRequest();

      break;
    }
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
