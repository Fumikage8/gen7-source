// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiFrameFactory.cpp
 * @brief プレイヤー複数選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiFrameFactory.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/JoinFestaPlayerListSelectMultiFrameID.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/Frame/JoinFestaPlayerListSelectMultiFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectMultiFrameFactory::JoinFestaPlayerListSelectMultiFrameFactory( NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork, NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor* pJoinFestaPlayerListSelectMultiViewSystemAccessor ) :
  m_pJoinFestaPlayerListSelectMultiWork( pJoinFestaPlayerListSelectMultiWork ),
  m_pJoinFestaPlayerListSelectMultiViewSystemAccessor( pJoinFestaPlayerListSelectMultiViewSystemAccessor )
{
}


JoinFestaPlayerListSelectMultiFrameFactory::~JoinFestaPlayerListSelectMultiFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaPlayerListSelectMultiFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaPlayerListSelectMultiWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_FRAME_ID_SELECT:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaPlayerListSelectMulti::Frame::JoinFestaPlayerListSelectMultiFrame( m_pJoinFestaPlayerListSelectMultiWork, m_pJoinFestaPlayerListSelectMultiViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)
