// ============================================================================
/*
 * @file JoinFestaBeaconFrameFactory.cpp
 * @brief ファクトリーメソッドです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconFrameFactory.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconFrameID.h"
#include "NetApp/JoinFestaBeacon/source/Frame/JoinFestaBeaconInviteFrame.h"
#include "NetApp/JoinFestaBeacon/source/Frame/JoinFestaBeaconWaitInvitedFrame.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)


JoinFestaBeaconFrameFactory::JoinFestaBeaconFrameFactory( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork ) :
  m_pJoinFestaBeaconWork( pJoinFestaBeaconWork )
{
}


JoinFestaBeaconFrameFactory::~JoinFestaBeaconFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaBeaconFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pJoinFestaBeaconWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaBeacon::JOINFESTABEACON_FRAME_ID_INVITE_SINGLE:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaBeacon::Frame::JoinFestaBeaconInviteFrame( m_pJoinFestaBeaconWork );
    break;
    case NetApp::JoinFestaBeacon::JOINFESTABEACON_FRAME_ID_WAIT_INVITED:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaBeacon::Frame::JoinFestaBeaconWaitInvitedFrame( m_pJoinFestaBeaconWork );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)
