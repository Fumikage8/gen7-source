// ============================================================================
/*
 * @file JoinFestaRecordDetailLowerView.cpp
 * @brief レコード詳細画面
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaRecordDetailLowerView::JoinFestaRecordDetailLowerView(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::System::ResourceID layoutResourceID,
  NetAppLib::System::ResourceID menuCursorResourceID ) :
    NetAppLib::System::NetApplicationViewBase(
      pWorkBase,
      layoutResourceID,
      LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_001_BFLYT,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_001___NUM,
      app::util::G2DUtil::SETUP_LOWER,
      pWorkBase->GetMessageUtility(),
      GARC_message_jf_playerlist_DAT ),
    m_pJoinFestaRecordDetailLowerViewListener( NULL ),
    m_pWorkBase( pWorkBase )
{
  setupButton();

  SetInputListener( this );
}


JoinFestaRecordDetailLowerView::~JoinFestaRecordDetailLowerView()
{
}


bool JoinFestaRecordDetailLowerView::IsDrawing(void) const
{
  bool result = false;
  
  if( UIView::IsDrawing() )
  {
    result = true;
  }

  return result;
}


void JoinFestaRecordDetailLowerView::Update()
{
  NetAppLib::System::NetApplicationViewBase::Update();
}


void JoinFestaRecordDetailLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pWorkBase->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


void JoinFestaRecordDetailLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_001__BACKBUTTON_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_001__BACKBUTTON_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_001__BACKBUTTON_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_001__BACKBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_001__BACKBUTTON_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_001__BACKBUTTON_PASSIVE,
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

  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
}


void JoinFestaRecordDetailLowerView::SetRecordData( NetAppLib::JoinFestaUI::JoinFestaRecordData& data )
{
  setMessage( PANENAME_JFPL_MIN_LOW_001_PANE_DAIJI_00, data.titleMessageID, true, true );

  setMessage( PANENAME_JFPL_MIN_LOW_001_PANE_RECORD_00, data.recordMenuMessageIDTable[0], data.bEnable[0], data.bVisible[0] );
  setMessage( PANENAME_JFPL_MIN_LOW_001_PANE_RECORD_01, data.recordMenuMessageIDTable[1], data.bEnable[1], data.bVisible[1] );
  setMessage( PANENAME_JFPL_MIN_LOW_001_PANE_RECORD_02, data.recordMenuMessageIDTable[2], data.bEnable[2], data.bVisible[2] );
  setMessage( PANENAME_JFPL_MIN_LOW_001_PANE_RECORD_03, data.recordMenuMessageIDTable[3], data.bEnable[3], data.bVisible[3] );
  setMessage( PANENAME_JFPL_MIN_LOW_001_PANE_RECORD_04, data.recordMenuMessageIDTable[4], data.bEnable[4], data.bVisible[4] );
  setMessage( PANENAME_JFPL_MIN_LOW_001_PANE_RECORD_05, data.recordMenuMessageIDTable[5], data.bEnable[5], data.bVisible[5] );
  setMessage( PANENAME_JFPL_MIN_LOW_001_PANE_RECORD_06, data.recordMenuMessageIDTable[6], data.bEnable[6], data.bVisible[6] );
  setMessage( PANENAME_JFPL_MIN_LOW_001_PANE_RECORD_07, data.recordMenuMessageIDTable[7], data.bEnable[7], data.bVisible[7] );

  setParam( PANENAME_JFPL_MIN_LOW_001_PANE_SCORE_00, data.recordParams[0], data.bEnable[0], data.bVisible[0] );
  setParam( PANENAME_JFPL_MIN_LOW_001_PANE_SCORE_01, data.recordParams[1], data.bEnable[1], data.bVisible[1] );
  setParam( PANENAME_JFPL_MIN_LOW_001_PANE_SCORE_02, data.recordParams[2], data.bEnable[2], data.bVisible[2] );
  setParam( PANENAME_JFPL_MIN_LOW_001_PANE_SCORE_03, data.recordParams[3], data.bEnable[3], data.bVisible[3] );
  setParam( PANENAME_JFPL_MIN_LOW_001_PANE_SCORE_04, data.recordParams[4], data.bEnable[4], data.bVisible[4] );
  setParam( PANENAME_JFPL_MIN_LOW_001_PANE_SCORE_05, data.recordParams[5], data.bEnable[5], data.bVisible[5] );
  setParam( PANENAME_JFPL_MIN_LOW_001_PANE_SCORE_06, data.recordParams[6], data.bEnable[6], data.bVisible[6] );
  setParam( PANENAME_JFPL_MIN_LOW_001_PANE_SCORE_07, data.recordParams[7], data.bEnable[7], data.bVisible[7] );
}


void JoinFestaRecordDetailLowerView::setMessage( u32 paneID, u32 messageID, bool bEnable, bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( paneID );
  pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, messageID ) );
  pText->SetVisible( bVisible );
}


void JoinFestaRecordDetailLowerView::setParam( u32 paneID, u32 param, bool bEnable, bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( paneID );
  if( bEnable )
  {
    pG2DUtil->SetRegisterNumber( 0, param, 9 );
    pG2DUtil->SetTextBoxPaneStringExpand( pText, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_37 ) ); // fix NMCat[4082] テキストラベルの指定ミス
  }
  else
  {
    pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_rec_01_08 ) );
  }
  pText->SetVisible( bVisible );
}


::app::ui::UIInputListener::ListenerResult JoinFestaRecordDetailLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pJoinFestaRecordDetailLowerViewListener )
      {
        m_pJoinFestaRecordDetailLowerViewListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult JoinFestaRecordDetailLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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

  return result;
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
