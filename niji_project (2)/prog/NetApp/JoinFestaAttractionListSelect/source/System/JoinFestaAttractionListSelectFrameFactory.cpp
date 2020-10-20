// ============================================================================
/*
 * @file JoinFestaAttractionListSelectFrameFactory.cpp
 * @brief アトラクション開催参加アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectFrameFactory.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectFrameID.h"
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectOpenFrame.h"
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectEntryFrame.h"
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectDescriptionFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionListSelectFrameFactory::JoinFestaAttractionListSelectFrameFactory( NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork, NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor ) :
  m_pJoinFestaAttractionListSelectWork( pJoinFestaAttractionListSelectWork ),
  m_pJoinFestaAttractionListSelectViewSystemAccessor( pJoinFestaAttractionListSelectViewSystemAccessor )
{
}


JoinFestaAttractionListSelectFrameFactory::~JoinFestaAttractionListSelectFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaAttractionListSelectFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaAttractionListSelectWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_OPEN:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaAttractionListSelect::Frame::JoinFestaAttractionListSelectOpenFrame( m_pJoinFestaAttractionListSelectWork, m_pJoinFestaAttractionListSelectViewSystemAccessor );
    break;

    case NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_ENTRY:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaAttractionListSelect::Frame::JoinFestaAttractionListSelectEntryFrame( m_pJoinFestaAttractionListSelectWork, m_pJoinFestaAttractionListSelectViewSystemAccessor );
    break;
    
    case NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_DESCRIPTION:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaAttractionListSelect::Frame::JoinFestaAttractionListSelectDescriptionFrame( m_pJoinFestaAttractionListSelectWork, m_pJoinFestaAttractionListSelectViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
