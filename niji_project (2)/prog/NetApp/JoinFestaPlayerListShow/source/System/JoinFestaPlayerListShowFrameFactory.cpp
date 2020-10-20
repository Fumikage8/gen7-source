// ============================================================================
/*
 * @file JoinFestaPlayerListShowFrameFactory.cpp
 * @brief 閲覧アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowFrameFactory.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowFrameID.h"
#include "NetApp/JoinFestaPlayerListShow/source/Frame/JoinFestaPlayerListShowSelectFrame.h"
#include "NetApp/JoinFestaPlayerListShow/source/Frame/JoinFestaPlayerListShowDetailFrame.h"
#include "NetApp/JoinFestaPlayerListShow/source/Frame/JoinFestaPlayerListShowDescriptionFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListShowFrameFactory::JoinFestaPlayerListShowFrameFactory( NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor ) :
  m_pJoinFestaPlayerListShowWork( pJoinFestaPlayerListShowWork ),
  m_pJoinFestaPlayerListShowViewSystemAccessor( pJoinFestaPlayerListShowViewSystemAccessor )
{
}


JoinFestaPlayerListShowFrameFactory::~JoinFestaPlayerListShowFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaPlayerListShowFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaPlayerListShowWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_SELECT:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaPlayerListShow::Frame::JoinFestaPlayerListShowSelectFrame( m_pJoinFestaPlayerListShowWork, m_pJoinFestaPlayerListShowViewSystemAccessor );
    break;
    
    case NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_DETAIL:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaPlayerListShow::Frame::JoinFestaPlayerListShowDetailFrame( m_pJoinFestaPlayerListShowWork, m_pJoinFestaPlayerListShowViewSystemAccessor );
    break;
    
    case NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_DESCRIPTION:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaPlayerListShow::Frame::JoinFestaPlayerListShowDescriptionFrame( m_pJoinFestaPlayerListShowWork, m_pJoinFestaPlayerListShowViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)
