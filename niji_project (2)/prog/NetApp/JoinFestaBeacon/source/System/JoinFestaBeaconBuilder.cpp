// ============================================================================
/*
 * @file JoinFestaBeaconBuilder.cpp
 * @brief ビーコンアプリを構築するクラスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconBuilder.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconFrameSwitcher.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconFrameFactory.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconResourceID.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)


JoinFestaBeaconBuilder::JoinFestaBeaconBuilder( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork ) :
  m_pJoinFestaBeaconWork( pJoinFestaBeaconWork )
{
}


JoinFestaBeaconBuilder::~JoinFestaBeaconBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaBeaconBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaBeaconWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaBeacon::System::JoinFestaBeaconFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaBeaconBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaBeaconWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaBeacon::System::JoinFestaBeaconFrameFactory( m_pJoinFestaBeaconWork );
}


NetAppLib::System::ApplicationWorkBase* JoinFestaBeaconBuilder::GetApplicationWork()
{
  return m_pJoinFestaBeaconWork;
}


NetAppLib::System::FrameID JoinFestaBeaconBuilder::GetApplicationStartFrameID()
{
  if( m_pJoinFestaBeaconWork )
  {
    switch( m_pJoinFestaBeaconWork->GetAppParam()->type )
    {
    case TYPE_TRADE:
    case TYPE_BATTLE_S:
    case TYPE_BATTLE_D:
    case TYPE_BATTLE_M:
    case TYPE_BATTLE_R:
      if( m_pJoinFestaBeaconWork->GetAppParam()->inviteType == TYPE_INVITE )
        return NetApp::JoinFestaBeacon::JOINFESTABEACON_FRAME_ID_INVITE_SINGLE;
      else if( m_pJoinFestaBeaconWork->GetAppParam()->inviteType == TYPE_INVITED )
        return NetApp::JoinFestaBeacon::JOINFESTABEACON_FRAME_ID_WAIT_INVITED;
    break;
    }
  }

  GFL_ASSERT( 0 );//@fix

  return NetApp::JoinFestaBeacon::JOINFESTABEACON_FRAME_ID_INVITE_SINGLE;
}


u32 JoinFestaBeaconBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_MAX;
}

app::util::AppRenderingManager::Description* JoinFestaBeaconBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pJoinFestaBeaconWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)
