// ============================================================================
/*
 * @file JoinFestaAlertLowerView.cpp
 * @brief アラート下画面
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAlertLowerView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>

#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaAlertLowerView::JoinFestaAlertLowerView(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::System::ResourceID layoutResourceID ) :
  NetAppLib::System::NetApplicationViewBase(
    pWorkBase,
    layoutResourceID,
    LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000_BFLYT,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pWorkBase->GetMessageUtility(),
    GARC_message_jf_playerlist_DAT ),
  m_pWorkBase( pWorkBase ),
  m_pJoinFestaAlertLowerViewListener( NULL ),
  m_MessageID( 0xffffffff )
{
  setupButton();

  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
    gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( PANENAME_JFPL_ART_LOW_000_PANE_PAGE_TITLE_00 );
    pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_18 ) );
  }

  SetInputListener( this );

}


JoinFestaAlertLowerView::~JoinFestaAlertLowerView()
{
}


void JoinFestaAlertLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__BACKBUTTON_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__BACKBUTTON_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__BACKBUTTON_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__BACKBUTTON_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__BACKBUTTON_PASSIVE,
    },

    {
      BUTTON_ID_ARROW_LEFT, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_04_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_04_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_04_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_04_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_04_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_04_PASSIVE,
    },

    {
      BUTTON_ID_ARROW_RIGHT, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_03_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_03_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_03_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_03_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_03_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_ART_LOW_000__PARTS_03_PASSIVE,
    },

  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_JFPL_ART_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pArrowLeft = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_04 );
    buttonInfoTable[ BUTTON_ID_ARROW_LEFT ].picture_pane = pArrowLeft;
    buttonInfoTable[ BUTTON_ID_ARROW_LEFT ].bound_pane = pLower->GetBoundingPane( pArrowLeft, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pArrowRight = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_03 );
    buttonInfoTable[ BUTTON_ID_ARROW_RIGHT ].picture_pane = pArrowRight;
    buttonInfoTable[ BUTTON_ID_ARROW_RIGHT ].bound_pane = pLower->GetBoundingPane( pArrowRight, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_ARROW_LEFT, SEQ_SE_SELECT1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_ARROW_RIGHT, SEQ_SE_SELECT1 );
}


void JoinFestaAlertLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pWorkBase->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


void JoinFestaAlertLowerView::SetMessage( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );

  gfl2::lyt::LytParts* pPartsPane = pLower->GetPartsPane( PANENAME_JFPL_ART_LOW_000_PANE_MESSAGE_L8 );
  gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( pPartsPane, PANENAME_COMMON_MSG_LOW_007_PANE_TEXTBOX_00, &resourceID );
  pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, messageID ) );

  m_MessageID = messageID;
}


void JoinFestaAlertLowerView::SetMessage( u32 messageID, u32 waitTime )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );

  gfl2::lyt::LytParts* pPartsPane = pLower->GetPartsPane( PANENAME_JFPL_ART_LOW_000_PANE_MESSAGE_L8 );
  gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( pPartsPane, PANENAME_COMMON_MSG_LOW_007_PANE_TEXTBOX_00, &resourceID );
  pG2DUtil->SetRegisterNumber( 0, waitTime, 2 );
  pG2DUtil->SetTextBoxPaneStringExpand( pText, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, messageID ) );

  m_MessageID = messageID;
}


::app::ui::UIInputListener::ListenerResult JoinFestaAlertLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pJoinFestaAlertLowerViewListener )
      {
        m_pJoinFestaAlertLowerViewListener->TouchBackButton( m_MessageID );
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_ARROW_LEFT:
    {
      if( m_pJoinFestaAlertLowerViewListener )
      {
        m_pJoinFestaAlertLowerViewListener->PushLButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
    
    case BUTTON_ID_ARROW_RIGHT:
    {
      if( m_pJoinFestaAlertLowerViewListener )
      {
        m_pJoinFestaAlertLowerViewListener->PushRButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult JoinFestaAlertLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_BACK_BUTTON );
    result = DISABLE_ACCESS;
  }
  else
  if( pKey->IsTrigger( gfl2::ui::DIR_LEFT ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_ARROW_LEFT );
    result = DISABLE_ACCESS;
  }
  else
  if( pKey->IsTrigger( gfl2::ui::DIR_RIGHT ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_ARROW_RIGHT );
    result = DISABLE_ACCESS;
  }

  return result;
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
