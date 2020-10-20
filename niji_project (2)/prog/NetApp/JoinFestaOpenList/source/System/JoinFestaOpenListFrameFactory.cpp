// ============================================================================
/*
 * @file JoinFestaOpenListFrameFactory.cpp
 * @brief 開催選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListFrameFactory.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListFrameID.h"
#include "NetApp/JoinFestaOpenList/source/Frame/JoinFestaOpenListSelectFrame.h"
#include "NetApp/JoinFestaOpenList/source/Frame/JoinFestaOpenListDescriptionFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


JoinFestaOpenListFrameFactory::JoinFestaOpenListFrameFactory( NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork, NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor* pJoinFestaOpenListViewSystemAccessor ) :
  m_pJoinFestaOpenListWork( pJoinFestaOpenListWork ),
  m_pJoinFestaOpenListViewSystemAccessor( pJoinFestaOpenListViewSystemAccessor )
{
}


JoinFestaOpenListFrameFactory::~JoinFestaOpenListFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaOpenListFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaOpenListWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_FRAME_ID_SELECT:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaOpenList::Frame::JoinFestaOpenListSelectFrame( m_pJoinFestaOpenListWork, m_pJoinFestaOpenListViewSystemAccessor );
    break;

    case NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_FRAME_ID_DESCRIPTION:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaOpenList::Frame::JoinFestaOpenListDescriptionFrame( m_pJoinFestaOpenListWork, m_pJoinFestaOpenListViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
