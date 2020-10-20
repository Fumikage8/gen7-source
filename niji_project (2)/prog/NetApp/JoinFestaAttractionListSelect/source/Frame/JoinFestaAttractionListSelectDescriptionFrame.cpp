// ============================================================================
/*
 * @file JoinFestaAttractionListSelectDescriptionFrame.cpp
 * @brief アトラクション開催参加アプリの○○プロセスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectDescriptionFrame.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectFrameResult.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"

#include <arc_index/message.gaix>

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"


#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(Frame)


static const u32 PAGE_MAX = 1;

JoinFestaAttractionListSelectDescriptionFrame::JoinFestaAttractionListSelectDescriptionFrame(
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor ) :
  NetApp::JoinFestaAttractionListSelect::Frame::JoinFestaAttractionListSelectFrameBase( pJoinFestaAttractionListSelectWork, pJoinFestaAttractionListSelectViewSystemAccessor ),
  m_pInfoWindowBookTypeLowerView( NULL )
{
}


JoinFestaAttractionListSelectDescriptionFrame::~JoinFestaAttractionListSelectDescriptionFrame()
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
bool JoinFestaAttractionListSelectDescriptionFrame::startup()
{
  m_pJoinFestaAttractionListSelectViewObjects->InputDisableAll();
  m_pJoinFestaAttractionListSelectViewList->RemoveAllView();


  m_pJoinFestaAttractionListSelectViewList->AddUpperView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaAttractionListSelectViewList->AddUpperView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionInfoUpperView() );

  m_pJoinFestaAttractionListSelectViewList->AddLowerView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListBGLowerView() );

  app::util::Heap* pAppHeap = m_pJoinFestaAttractionListSelectWork->GetAppHeap();

  m_pInfoWindowBookTypeLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::InfoWindowBookType( pAppHeap );
  m_pInfoWindowBookTypeLowerView->SyncCreate( pAppHeap, m_pJoinFestaAttractionListSelectWork->GetAppRenderingManager(), PAGE_MAX, NULL );

  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListBGLowerView()->AddSubView( m_pInfoWindowBookTypeLowerView );

  m_pInfoWindowBookTypeLowerView->ResetContents( 1 );
  OnChangePage(0);

  m_pInfoWindowBookTypeLowerView->StartOpen( this, gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM );

  return true;
}


bool JoinFestaAttractionListSelectDescriptionFrame::cleanup()
{
  return m_pInfoWindowBookTypeLowerView->EndFunc();
}


void JoinFestaAttractionListSelectDescriptionFrame::setListener()
{
}


void JoinFestaAttractionListSelectDescriptionFrame::removeListener()
{
}


void JoinFestaAttractionListSelectDescriptionFrame::updateSequence()
{
}


//-----------------------------------------------------------------------------
// InfoWindowBookTypeListener
//-----------------------------------------------------------------------------
void JoinFestaAttractionListSelectDescriptionFrame::OnClose()
{
  exitFrame( m_pJoinFestaAttractionListSelectWork->GetDescriptionBootFrameID() );
}


void JoinFestaAttractionListSelectDescriptionFrame::OnChangePage( u32 pageNo )
{
  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pDescriptionAttraction = m_pJoinFestaAttractionListSelectWork->GetDescriptionAttraction();

  if( pDescriptionAttraction )
  {
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaAttractionListSelectWork->GetMessageUtility();

    NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

    NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData attractionData = pJoinFestaAttractionManager->GetAttractionData( pDescriptionAttraction->GetAttractionID() );

    m_pInfoWindowBookTypeLowerView->SetTextContent( pageNo, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, static_cast<u32>( attractionData.attractionExplain ) ), NULL );
  }

}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
