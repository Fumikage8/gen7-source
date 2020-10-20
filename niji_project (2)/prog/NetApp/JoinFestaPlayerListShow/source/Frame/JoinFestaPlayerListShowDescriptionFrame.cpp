// ============================================================================
/*
 * @file JoinFestaPlayerListShowDescriptionFrame.cpp
 * @brief 
 * @date 2016.03.16
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/Frame/JoinFestaPlayerListShowDescriptionFrame.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowFrameResult.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"

#include <arc_index/message.gaix>


#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"


#include <niji_conv_header/message/msg_jf_playerlist.h>

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(Frame)


static const u32 PAGE_MAX = 6;

JoinFestaPlayerListShowDescriptionFrame::JoinFestaPlayerListShowDescriptionFrame(
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor ) :
  NetApp::JoinFestaPlayerListShow::Frame::JoinFestaPlayerListShowFrameBase( pJoinFestaPlayerListShowWork, pJoinFestaPlayerListShowViewSystemAccessor ),
  m_pInfoWindowBookTypeLowerView( NULL )
{
}


JoinFestaPlayerListShowDescriptionFrame::~JoinFestaPlayerListShowDescriptionFrame()
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
bool JoinFestaPlayerListShowDescriptionFrame::startup()
{
  m_pJoinFestaPlayerListShowViewObjects->InputDisableAll();
  m_pJoinFestaPlayerListShowViewList->RemoveAllView();

  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->TimerIconDisable();

  m_pJoinFestaPlayerListShowViewList->AddUpperView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaPlayerListShowViewList->AddLowerView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListBGLowerView() );

  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListShowWork->GetAppHeap();

  m_pInfoWindowBookTypeLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::InfoWindowBookType( pAppHeap );
  m_pInfoWindowBookTypeLowerView->SyncCreate( pAppHeap, m_pJoinFestaPlayerListShowWork->GetAppRenderingManager(), PAGE_MAX, NULL );

  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListBGLowerView()->AddSubView( m_pInfoWindowBookTypeLowerView );

  m_pInfoWindowBookTypeLowerView->ResetContents( PAGE_MAX );
  OnChangePage(0);

  app::tool::InfoWindowBookType::OpenType eOpenType = ( m_pJoinFestaPlayerListShowWork->GetDescriptionOpenAnimeFlag() ) ? app::tool::InfoWindowBookType::OPEN_TYPE_SCALE_ANIM : app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM;

  // @fix GFMMCat[27]: インフォボタンの座標を設定
  gfl2::math::Vector3 vec;
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->GetInfoButtonPos( &vec );
  m_pInfoWindowBookTypeLowerView->StartOpen( this, vec, eOpenType );

  return true;
}


bool JoinFestaPlayerListShowDescriptionFrame::cleanup()
{
  return m_pInfoWindowBookTypeLowerView->EndFunc();
}


void JoinFestaPlayerListShowDescriptionFrame::setListener()
{
}


void JoinFestaPlayerListShowDescriptionFrame::removeListener()
{
}


void JoinFestaPlayerListShowDescriptionFrame::updateSequence()
{
}


//-----------------------------------------------------------------------------
// InfoWindowBookTypeListener
//-----------------------------------------------------------------------------
void JoinFestaPlayerListShowDescriptionFrame::OnClose()
{
  exitFrame( NetApp::JoinFestaPlayerListShow::DESCRIPTION_FRAME_RESULT_BACK );
}


void JoinFestaPlayerListShowDescriptionFrame::OnChangePage( u32 pageNo )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaPlayerListShowWork->GetMessageUtility();

  u32 aMessageIDTable[ PAGE_MAX ] =
  {
    jf_playerlist_exp_08,
    jf_playerlist_exp_09,
    jf_playerlist_exp_10,
    jf_playerlist_exp_11,
    jf_playerlist_exp_12,
    jf_playerlist_exp_13
  };

  if( pageNo < PAGE_MAX )
  {
    m_pInfoWindowBookTypeLowerView->SetTextContent( pageNo, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, aMessageIDTable[ pageNo ] ), NULL );
  }

}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)
