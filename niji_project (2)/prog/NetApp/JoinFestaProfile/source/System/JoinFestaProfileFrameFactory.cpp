// ============================================================================
/*
 * @file JoinFestaProfileFrameFactory.cpp
 * @brief プロフィールアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileFrameFactory.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileFrameID.h"
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileSelectEditMenuFrame.h"
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileDescriptionFrame.h"
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileNameInputFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


JoinFestaProfileFrameFactory::JoinFestaProfileFrameFactory( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork, NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor ) :
  m_pJoinFestaProfileWork( pJoinFestaProfileWork ),
  m_pJoinFestaProfileViewSystemAccessor( pJoinFestaProfileViewSystemAccessor )
{
}


JoinFestaProfileFrameFactory::~JoinFestaProfileFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaProfileFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaProfileWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_SELECT_EDIT_MENU:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaProfile::Frame::JoinFestaProfileSelectEditMenuFrame( m_pJoinFestaProfileWork, m_pJoinFestaProfileViewSystemAccessor );
    break;

    case NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_DESCRIPTION:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaProfile::Frame::JoinFestaProfileDescriptionFrame( m_pJoinFestaProfileWork, m_pJoinFestaProfileViewSystemAccessor );
    break;

    case NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_NAME_INPUT:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaProfile::Frame::JoinFestaProfileNameInputFrame( m_pJoinFestaProfileWork, m_pJoinFestaProfileViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
