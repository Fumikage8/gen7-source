// ============================================================================
/*
 * @file P2PTradeMessageLowerView.cpp
 * @brief P2P交換アプリの下画面を表示するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/View/P2PTradeMessageLowerView.h"
#include "NetApp/P2PTrade/source/P2PTradeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/net/net.h>
#include <niji_conv_header/app/net/net_anim_list.h>
#include <niji_conv_header/app/net/net_pane.h>
#include <niji_conv_header/message/msg_net_save.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(View)

P2PTradeMessageLowerView::P2PTradeMessageLowerView(
  NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
  NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu ) :
  NetAppLib::System::NetApplicationViewBase(
    pP2PTradeWork,
    NetApp::P2PTrade::P2PTRADE_RESOURCE_ID_LAYOUT_CONNECTION,
    LYTRES_NET_NET_BACK_BUTTON_BFLYT,
    LA_NET_NET_BACK_BUTTON___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pP2PTradeWork->GetMessageUtility(),
    GARC_message_net_save_DAT ),
  m_pP2PTradeWork( pP2PTradeWork ),
  m_pP2PTradeMessageLowerViewListener( NULL ),
  m_NetAppMessageMenu( netAppMessageMenu ),
  m_MessageID( 0xffffffff )
{
  setupButton();

  SetInputListener( this );

  m_NetAppMessageMenu.SetEventHandler( this );
  AddSubView( &m_NetAppMessageMenu );

  SetVisibleBackButton( false );
}


P2PTradeMessageLowerView::~P2PTradeMessageLowerView()
{
  m_NetAppMessageMenu.RemoveFromSuperView();
  m_NetAppMessageMenu.RemoveEventHandler();
}


void P2PTradeMessageLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_NET_NET_BACK_BUTTON__BACKBUTTON_TOUCH,
      LA_NET_NET_BACK_BUTTON__BACKBUTTON_RELEASE,
      LA_NET_NET_BACK_BUTTON__BACKBUTTON_CANSEL,
      LA_NET_NET_BACK_BUTTON__BACKBUTTON_TOUCH_RELEASE,
      LA_NET_NET_BACK_BUTTON__BACKBUTTON_ACTIVE,
      LA_NET_NET_BACK_BUTTON__BACKBUTTON_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_P2PTRADE );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_NET_BACK_BUTTON_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pP2PTradeWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
}


void P2PTradeMessageLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pP2PTradeWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


void P2PTradeMessageLowerView::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  switch( eEventCode )
  {
    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:
    {
      if( m_pP2PTradeMessageLowerViewListener )
      {
        m_pP2PTradeMessageLowerViewListener->TouchScreen( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedYes:
    {
      if( m_pP2PTradeMessageLowerViewListener )
      {
        m_pP2PTradeMessageLowerViewListener->TouchYesButton( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedNo:
    {
      if( m_pP2PTradeMessageLowerViewListener )
      {
        m_pP2PTradeMessageLowerViewListener->TouchNoButton( m_MessageID );
      }
    }
    break;
  }
}


::app::ui::UIInputListener::ListenerResult P2PTradeMessageLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pP2PTradeMessageLowerViewListener )
      {
        m_pP2PTradeMessageLowerViewListener->TouchBackButton( m_MessageID );
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult P2PTradeMessageLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( IsInputEnabled() )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      m_button_manager->StartSelectedAct( BUTTON_ID_BACK_BUTTON );
      result = DISABLE_ACCESS;
    }
  }

  return result;
}


void P2PTradeMessageLowerView::SetMessage( gfl2::fs::ArcFile::ARCDATID garcID, u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( garcID, messageID, true, isShowTimerIcon, false );
}


void P2PTradeMessageLowerView::SetStreamMessage( gfl2::fs::ArcFile::ARCDATID garcID, u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( garcID, messageID, false, isShowTimerIcon, false );
}


void P2PTradeMessageLowerView::showMessage( gfl2::fs::ArcFile::ARCDATID garcID, u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pP2PTradeWork->GetMessageUtility();
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &pMessageUtility->GetString( garcID, messageID ), bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


void P2PTradeMessageLowerView::SetMessageYesNoBlackFilter( gfl2::fs::ArcFile::ARCDATID garcID, u32 messageID )
{
  m_NetAppMessageMenu.SetYNListMode( true );
  m_NetAppMessageMenu.ShowMessageWithListMenu( garcID, messageID, true, garcID, net_save_sel_01, net_save_sel_02 );
  m_MessageID = messageID;
}


void P2PTradeMessageLowerView::SetVisibleBackButton( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_NET_BACK_BUTTON_PANE_BACKBUTTON );

  if( pBtnBack )
  {
    pBtnBack->SetVisible( bVisible );
  }
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)
