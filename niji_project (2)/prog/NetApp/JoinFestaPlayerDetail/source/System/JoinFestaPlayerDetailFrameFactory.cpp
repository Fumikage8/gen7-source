// ============================================================================
/*
 * @file JoinFestaPlayerDetailFrameFactory.cpp
 * @brief 詳細アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailFrameFactory.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailFrameID.h"
#include "NetApp/JoinFestaPlayerDetail/source/Frame/JoinFestaPlayerDetailFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerDetailFrameFactory::JoinFestaPlayerDetailFrameFactory( NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork, NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor* pJoinFestaPlayerDetailViewSystemAccessor ) :
  m_pJoinFestaPlayerDetailWork( pJoinFestaPlayerDetailWork ),
  m_pJoinFestaPlayerDetailViewSystemAccessor( pJoinFestaPlayerDetailViewSystemAccessor )
{
}


JoinFestaPlayerDetailFrameFactory::~JoinFestaPlayerDetailFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaPlayerDetailFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaPlayerDetailWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_FRAME_ID_DETAIL:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaPlayerDetail::Frame::JoinFestaPlayerDetailFrame( m_pJoinFestaPlayerDetailWork, m_pJoinFestaPlayerDetailViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)
