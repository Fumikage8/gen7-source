// ============================================================================
/*
 * @file JoinFestaSelectWordDescriptionFrame.cpp
 * @brief 
 * @date 2016.03.16
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/Frame/JoinFestaSelectWordDescriptionFrame.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordFrameResult.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"

#include <arc_index/message.gaix>


#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordBGUpperView.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordBGLowerView.h"


#include <niji_conv_header/message/msg_setphrase.h>

#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordLowerView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(Frame)


static const u32 PAGE_MAX = 1;

JoinFestaSelectWordDescriptionFrame::JoinFestaSelectWordDescriptionFrame(
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork,
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor* pJoinFestaSelectWordViewSystemAccessor ) :
  NetApp::JoinFestaSelectWord::Frame::JoinFestaSelectWordFrameBase( pJoinFestaSelectWordWork, pJoinFestaSelectWordViewSystemAccessor ),
  m_pInfoWindowBookTypeLowerView( NULL )
{
}


JoinFestaSelectWordDescriptionFrame::~JoinFestaSelectWordDescriptionFrame()
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
bool JoinFestaSelectWordDescriptionFrame::startup()
{
  m_pJoinFestaSelectWordViewObjects->InputDisableAll();
  m_pJoinFestaSelectWordViewList->RemoveAllView();

  m_pJoinFestaSelectWordViewList->AddUpperView( m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordBGUpperView() );
  m_pJoinFestaSelectWordViewList->AddLowerView( m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordBGLowerView() );

  app::util::Heap* pAppHeap = m_pJoinFestaSelectWordWork->GetAppHeap();

  m_pInfoWindowBookTypeLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::InfoWindowBookType( pAppHeap );
  m_pInfoWindowBookTypeLowerView->SyncCreate( pAppHeap, m_pJoinFestaSelectWordWork->GetAppRenderingManager(), PAGE_MAX, NULL );

  m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordBGLowerView()->AddSubView( m_pInfoWindowBookTypeLowerView );

  m_pInfoWindowBookTypeLowerView->ResetContents( PAGE_MAX );
  OnChangePage(0);

  app::tool::InfoWindowBookType::OpenType eOpenType = ( m_pJoinFestaSelectWordWork->GetDescriptionOpenAnimeFlag() ) ? app::tool::InfoWindowBookType::OPEN_TYPE_SCALE_ANIM : app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM;

  // @fix GFMMCat[27]: インフォボタンの座標を設定
  gfl2::math::Vector3 vec;
  m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordLowerView()->GetInfoButtonPos( &vec );
  m_pInfoWindowBookTypeLowerView->StartOpen( this, vec, eOpenType );

  return true;
}


bool JoinFestaSelectWordDescriptionFrame::cleanup()
{
  return m_pInfoWindowBookTypeLowerView->EndFunc();
}


void JoinFestaSelectWordDescriptionFrame::setListener()
{
}


void JoinFestaSelectWordDescriptionFrame::removeListener()
{
}


void JoinFestaSelectWordDescriptionFrame::updateSequence()
{
}


//-----------------------------------------------------------------------------
// InfoWindowBookTypeListener
//-----------------------------------------------------------------------------
void JoinFestaSelectWordDescriptionFrame::OnClose()
{
  exitFrame( NetApp::JoinFestaSelectWord::DESCRIPTION_FRAME_RESULT_BACK );
}


void JoinFestaSelectWordDescriptionFrame::OnChangePage( u32 pageNo )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaSelectWordWork->GetMessageUtility();

  u32 aMessageIDTable[ PAGE_MAX ] =
  {
    msg_setphrase_exp_01
  };

  if( pageNo < PAGE_MAX )
  {
    m_pInfoWindowBookTypeLowerView->SetTextContent( pageNo, &pMessageUtility->GetString( GARC_message_setphrase_DAT, aMessageIDTable[ pageNo ] ), NULL );
  }

}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
