// ============================================================================
/*
 * @file ConnectionBuilder.cpp
 * @brief インターネット接続アプリを構築するクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/System/ConnectionBuilder.h"
#include "NetApp/Connection/source/System/ConnectionFrameSwitcher.h"
#include "NetApp/Connection/source/System/ConnectionFrameFactory.h"
#include "NetApp/Connection/source/ConnectionResourceID.h"
#include "NetApp/Connection/source/ConnectionFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


ConnectionBuilder::ConnectionBuilder( NetApp::Connection::System::ConnectionWork* pConnectionWork ) :
  m_pConnectionWork( pConnectionWork ),
  m_pConnectionViewSystemAccessor( NULL )
{
}


ConnectionBuilder::~ConnectionBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* ConnectionBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pConnectionWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::Connection::System::ConnectionFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* ConnectionBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pConnectionWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::Connection::System::ConnectionFrameFactory( m_pConnectionWork, m_pConnectionViewSystemAccessor );
}

app::util::AppRenderingManager::Description* ConnectionBuilder::CreateAppRenderingManagerDescription()
{
  switch( m_pConnectionWork->GetBootMode() )
  {
  case NetApp::Connection::BOOT_MODE_CHANGE_INTERNET_MODE:
  case NetApp::Connection::BOOT_MODE_INTERNET_CONNECTION:
    {
      app::util::Heap* pAppHeap = m_pConnectionWork->GetAppHeap();
      app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
      pDescription->isUpperDisplayStereo                                                                     = false;
      return pDescription;
    }
    //break;
  case NetApp::Connection::BOOT_MODE_CHANGE_LOCAL_MODE:
  case NetApp::Connection::BOOT_MODE_INTERNET_DISCONNECTION:
  case NetApp::Connection::BOOT_MODE_FATAL_ERROR:
  default:
    {
      app::util::Heap* pAppHeap = m_pConnectionWork->GetAppHeap();
      app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder         = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].lytWkMax            = 8;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].pokeModelNumMax     = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].modelNumMax         = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].charaModelNumMax    = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].dressUpModelNumMax  = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder         = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].lytWkMax            = 8;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].pokeModelNumMax     = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].modelNumMax         = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].charaModelNumMax    = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].dressUpModelNumMax  = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder         = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].lytWkMax            = 8;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].pokeModelNumMax     = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].modelNumMax         = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].charaModelNumMax    = 0;
      pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].dressUpModelNumMax  = 0;
      pDescription->isUpperDisplayStereo                                                                     = false;
      return pDescription;
    }
    //break;
  }
}


NetAppLib::System::ApplicationWorkBase* ConnectionBuilder::GetApplicationWork()
{
  return m_pConnectionWork;
}


NetAppLib::System::FrameID ConnectionBuilder::GetApplicationStartFrameID()
{
  NetAppLib::System::FrameID startFrameID = NetApp::Connection::CONNECTION_FRAME_ID_CHANGE_INTERNET_MODE;

  switch( m_pConnectionWork->GetBootMode() )
  {
    case NetApp::Connection::BOOT_MODE_CHANGE_INTERNET_MODE:
      startFrameID = NetApp::Connection::CONNECTION_FRAME_ID_INTERNET_CONNECTION;
    break;

    case NetApp::Connection::BOOT_MODE_CHANGE_LOCAL_MODE:
      startFrameID = NetApp::Connection::CONNECTION_FRAME_ID_CHANGE_LOCAL_MODE;
    break;

    case NetApp::Connection::BOOT_MODE_INTERNET_CONNECTION:
      startFrameID = NetApp::Connection::CONNECTION_FRAME_ID_INTERNET_CONNECTION;
    break;

    case NetApp::Connection::BOOT_MODE_INTERNET_DISCONNECTION:
      startFrameID = NetApp::Connection::CONNECTION_FRAME_ID_INTERNET_DISCONNECTION;
    break;

    case NetApp::Connection::BOOT_MODE_FATAL_ERROR:
      startFrameID = NetApp::Connection::CONNECTION_FRAME_ID_FATAL_ERROR;
    break;
  }

  return startFrameID;
}


u32 ConnectionBuilder::GetApplicationResourceCount()
{
  return NetApp::Connection::CONNECTION_RESOURCE_MAX;
}


bool ConnectionBuilder::IsCreateMessageUtility()
{
  return true;
}


// トレーナーアイコンを使う
bool ConnectionBuilder::IsCreateAppToolTrainerIconRendering()
{ 
  switch( m_pConnectionWork->GetBootMode() )
  {
  case NetApp::Connection::BOOT_MODE_CHANGE_INTERNET_MODE:
  case NetApp::Connection::BOOT_MODE_INTERNET_CONNECTION:
    {
      return true;
    }
    //break;
  case NetApp::Connection::BOOT_MODE_CHANGE_LOCAL_MODE:
  case NetApp::Connection::BOOT_MODE_INTERNET_DISCONNECTION:
  case NetApp::Connection::BOOT_MODE_FATAL_ERROR:
  default:
    {
      return false;
    }
    //break;
  }
}
// トレーナーアイコンを使う個数
u32  ConnectionBuilder::GetTrainerIconNum()
{ 
  switch( m_pConnectionWork->GetBootMode() )
  {
  case NetApp::Connection::BOOT_MODE_CHANGE_INTERNET_MODE:
  case NetApp::Connection::BOOT_MODE_INTERNET_CONNECTION:
    {
      return 1;
    }
    //break;
  case NetApp::Connection::BOOT_MODE_CHANGE_LOCAL_MODE:
  case NetApp::Connection::BOOT_MODE_INTERNET_DISCONNECTION:
  case NetApp::Connection::BOOT_MODE_FATAL_ERROR:
  default:
    {
      return 0;
    }
    //break;
  }
}
// トレーナーアイコンの場所
app::util::G2DUtil::SETUP_DISPLAY_TYPE ConnectionBuilder::GetTrainerIconDrawDisplayType()
{ 
  return app::util::G2DUtil::SETUP_UPPER; 
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)
