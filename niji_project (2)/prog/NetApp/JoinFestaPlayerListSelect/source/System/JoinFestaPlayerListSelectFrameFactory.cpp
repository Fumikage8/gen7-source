// ============================================================================
/*
 * @file JoinFestaPlayerListSelectFrameFactory.cpp
 * @brief プレイヤー単体選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectFrameFactory.h"
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectFrameID.h"
#include "NetApp/JoinFestaPlayerListSelect/source/Frame/JoinFestaPlayerListSelectFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectFrameFactory::JoinFestaPlayerListSelectFrameFactory( NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork, NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor* pJoinFestaPlayerListSelectViewSystemAccessor ) :
  m_pJoinFestaPlayerListSelectWork( pJoinFestaPlayerListSelectWork ),
  m_pJoinFestaPlayerListSelectViewSystemAccessor( pJoinFestaPlayerListSelectViewSystemAccessor )
{
}


JoinFestaPlayerListSelectFrameFactory::~JoinFestaPlayerListSelectFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaPlayerListSelectFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaPlayerListSelectWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_FRAME_ID_SELECT:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaPlayerListSelect::Frame::JoinFestaPlayerListSelectFrame( m_pJoinFestaPlayerListSelectWork, m_pJoinFestaPlayerListSelectViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)
