// ============================================================================
/*
 * @file JoinFestaOpenListDescriptionFrame.cpp
 * @brief 
 * @date 2016.04.04
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/Frame/JoinFestaOpenListDescriptionFrame.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListFrameResult.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"

#include <arc_index/message.gaix>

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"


#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(Frame)


static const u32 PAGE_MAX = 1;

JoinFestaOpenListDescriptionFrame::JoinFestaOpenListDescriptionFrame(
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork,
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor* pJoinFestaOpenListViewSystemAccessor ) :
  NetApp::JoinFestaOpenList::Frame::JoinFestaOpenListFrameBase( pJoinFestaOpenListWork, pJoinFestaOpenListViewSystemAccessor ),
  m_pInfoWindowBookTypeLowerView( NULL )
{
}


JoinFestaOpenListDescriptionFrame::~JoinFestaOpenListDescriptionFrame()
{
  if( m_pInfoWindowBookTypeLowerView )
  {
    m_pInfoWindowBookTypeLowerView->RemoveFromSuperView();
  }
  GFL_SAFE_DELETE( m_pInfoWindowBookTypeLowerView );
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaOpenListDescriptionFrame::startup()
{
  m_pJoinFestaOpenListViewObjects->InputDisableAll();
  m_pJoinFestaOpenListViewList->RemoveAllView();


  m_pJoinFestaOpenListViewList->AddUpperView( m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaOpenListViewList->AddUpperView( m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionInfoUpperView() );

  m_pJoinFestaOpenListViewList->AddLowerView( m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListBGLowerView() );

  app::util::Heap* pAppHeap = m_pJoinFestaOpenListWork->GetAppHeap();

  m_pInfoWindowBookTypeLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::InfoWindowBookType( pAppHeap );
  m_pInfoWindowBookTypeLowerView->SyncCreate( pAppHeap, m_pJoinFestaOpenListWork->GetAppRenderingManager(), PAGE_MAX, NULL );

  m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListBGLowerView()->AddSubView( m_pInfoWindowBookTypeLowerView );

  m_pInfoWindowBookTypeLowerView->ResetContents( 1 );
  OnChangePage(0);

  m_pInfoWindowBookTypeLowerView->StartOpen( this, gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM );

  return true;
}


bool JoinFestaOpenListDescriptionFrame::cleanup()
{
  return m_pInfoWindowBookTypeLowerView->EndFunc();
}


void JoinFestaOpenListDescriptionFrame::setListener()
{
}


void JoinFestaOpenListDescriptionFrame::removeListener()
{
}


void JoinFestaOpenListDescriptionFrame::updateSequence()
{
}


//-----------------------------------------------------------------------------
// InfoWindowBookTypeListener
//-----------------------------------------------------------------------------
void JoinFestaOpenListDescriptionFrame::OnClose()
{
  exitFrame( NetApp::JoinFestaOpenList::DESCRIPTION_FRAME_RESULT_BACK );
}


void JoinFestaOpenListDescriptionFrame::OnChangePage( u32 pageNo )
{
  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pDescriptionAttraction = m_pJoinFestaOpenListWork->GetDescriptionAttraction();

  if( pDescriptionAttraction )
  {
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaOpenListWork->GetMessageUtility();

    NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

    NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData attractionData = pJoinFestaAttractionManager->GetAttractionData( pDescriptionAttraction->GetAttractionID() );

    m_pInfoWindowBookTypeLowerView->SetTextContent( pageNo, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, static_cast<u32>( attractionData.attractionExplain ) ), NULL );
  }

}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
