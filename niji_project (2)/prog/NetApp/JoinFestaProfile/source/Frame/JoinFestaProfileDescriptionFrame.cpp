// ============================================================================
/*
 * @file JoinFestaProfileDescriptionFrame.cpp
 * @brief 
 * @date 2015.11.04
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileDescriptionFrame.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileFrameResult.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"

#include <arc_index/message.gaix>

#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileUpperView.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"


#include <niji_conv_header/message/msg_jf_myprofile.h>

#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileLowerView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(Frame)

static const u32 PAGE_MAX = 2;


JoinFestaProfileDescriptionFrame::JoinFestaProfileDescriptionFrame(
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor ) :
  NetApp::JoinFestaProfile::Frame::JoinFestaProfileFrameBase( pJoinFestaProfileWork, pJoinFestaProfileViewSystemAccessor ),
  m_pInfoWindowBookTypeLowerView( NULL )
{
}


JoinFestaProfileDescriptionFrame::~JoinFestaProfileDescriptionFrame()
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
bool JoinFestaProfileDescriptionFrame::startup()
{
  m_pJoinFestaProfileViewObjects->InputDisableAll();
  m_pJoinFestaProfileViewList->RemoveAllView();
  
  app::util::Heap* pAppHeap = m_pJoinFestaProfileWork->GetAppHeap();

  m_pJoinFestaProfileViewList->AddUpperView( m_pJoinFestaProfileViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaProfileViewList->AddLowerView( m_pJoinFestaProfileViewObjects->GetJoinFestaPlayerListBGLowerView() );

  u32 pageMax = 0;
  app::tool::InfoWindowBookType::OpenType eOpenType = app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM;

  if( m_pJoinFestaProfileWork->GetDescriptionType() == NetApp::JoinFestaProfile::System::JoinFestaProfileWork::DESCRIPTION_TYPE_FIRST_BOOT_DESCRIPTION )
  {
    pageMax = PAGE_MAX;
    eOpenType = ( m_pJoinFestaProfileWork->GetDescriptionOpenAnimeFlag() ) ? app::tool::InfoWindowBookType::OPEN_TYPE_SCALE_ANIM : app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM;
  }
  else
  {
    m_pJoinFestaProfileViewList->AddUpperView( m_pJoinFestaProfileViewObjects->GetJoinFestaProfileUpperView() );
    
    NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu = m_pJoinFestaProfileWork->GetSelectEditMenu();

    pageMax = pSelectEditMenu->descriptionPageMax;
    eOpenType = app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM;
  }


  m_pInfoWindowBookTypeLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::InfoWindowBookType( pAppHeap );
  m_pInfoWindowBookTypeLowerView->SyncCreate( pAppHeap, m_pJoinFestaProfileWork->GetAppRenderingManager(), pageMax, NULL );

  m_pJoinFestaProfileViewObjects->GetJoinFestaPlayerListBGLowerView()->AddSubView( m_pInfoWindowBookTypeLowerView );

  m_pInfoWindowBookTypeLowerView->ResetContents( pageMax );
  OnChangePage(0);

  // @fix GFMMCat[27]: インフォボタンの座標を設定
  gfl2::math::Vector3 vec;
  m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView()->GetInfoButtonPos( &vec );
  m_pInfoWindowBookTypeLowerView->StartOpen( this, vec, eOpenType );

  return true;
}


bool JoinFestaProfileDescriptionFrame::cleanup()
{
  return m_pInfoWindowBookTypeLowerView->EndFunc();
}


void JoinFestaProfileDescriptionFrame::setListener()
{
}


void JoinFestaProfileDescriptionFrame::removeListener()
{
}


void JoinFestaProfileDescriptionFrame::updateSequence()
{
}


//-----------------------------------------------------------------------------
// InfoWindowBookTypeListener
//-----------------------------------------------------------------------------
void JoinFestaProfileDescriptionFrame::OnClose()
{
  exitFrame( NetApp::JoinFestaProfile::DESCRIPTION_FRAME_RESULT_BACK );
}


void JoinFestaProfileDescriptionFrame::OnChangePage( u32 pageNo )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaProfileWork->GetMessageUtility();

  u32 messageID = NetApp::JoinFestaProfile::INVALID_MESSAGE_ID;

  if( m_pJoinFestaProfileWork->GetDescriptionType() == NetApp::JoinFestaProfile::System::JoinFestaProfileWork::DESCRIPTION_TYPE_FIRST_BOOT_DESCRIPTION )
  {
    u32 aMessageIDTable[ PAGE_MAX ] =
    {
      msg_jf_myprofile_exp_12,
      msg_jf_myprofile_exp_13
    };

    if( pageNo < PAGE_MAX )
    {
      messageID = aMessageIDTable[ pageNo ];
    }
  }
  else
  {
    NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu = m_pJoinFestaProfileWork->GetSelectEditMenu();

    switch( pageNo )
    {
      case 0: messageID = pSelectEditMenu->descriptionMessageID0; break;
      case 1: messageID = pSelectEditMenu->descriptionMessageID1; break;
      case 2: messageID = pSelectEditMenu->descriptionMessageID2; break;
    }
  }

  if( messageID != NetApp::JoinFestaProfile::INVALID_MESSAGE_ID )
  {
    m_pInfoWindowBookTypeLowerView->SetTextContent( pageNo, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, messageID ), NULL );
  }

}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
