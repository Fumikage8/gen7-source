// ============================================================================
/*
 * @file JoinFestaPlayerListLowerViewBase.cpp
 * @brief JFPL_MIN_LOW_000.flyt を扱う基底クラスです
 * @date 2015.10.13
 * @author endo_akira
 */
// ============================================================================
#include "../..//include/JoinFestaUI/JoinFestaPlayerListLowerViewBase.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaPlayerListLowerViewBase::JoinFestaPlayerListLowerViewBase(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::System::ResourceID layoutResourceID,
  NetAppLib::System::ResourceID menuCursorResourceID ) :
    NetAppLib::System::NetApplicationViewBase(
      pWorkBase,
      layoutResourceID,
      LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000_BFLYT,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000___NUM,
      app::util::G2DUtil::SETUP_LOWER,
      pWorkBase->GetMessageUtility(),
      GARC_message_jf_playerlist_DAT ),
    m_pWorkBase( pWorkBase ),
    m_MenuCursor( pWorkBase )
{
  m_MenuCursor.Create( GetG2DUtil(), LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST, LAYOUT_WORK_ID_LOWER, menuCursorResourceID );
}


JoinFestaPlayerListLowerViewBase::~JoinFestaPlayerListLowerViewBase()
{
  m_MenuCursor.Destroy();
}


bool JoinFestaPlayerListLowerViewBase::IsDrawing(void) const
{
  bool result = false;
  
  if( !m_MenuCursor.CanDestroy() )
  {
    result = true;
  }

  if( UIView::IsDrawing() )
  {
    result = true;
  }

  return result;
}


void JoinFestaPlayerListLowerViewBase::Update()
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_MenuCursor.Update();
}


void JoinFestaPlayerListLowerViewBase::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pWorkBase->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  m_MenuCursor.Draw( displayNo );
}


void JoinFestaPlayerListLowerViewBase::SetVisibleUpdateButton( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_RELOADBUTTON );
//  gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( pButton, PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &resourceID );
//  pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_sel_01 ) );
  pButton->SetVisible( bVisible );

  pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_01 )->SetVisible(false);
}


void JoinFestaPlayerListLowerViewBase::SetVisibleDecideButton( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_02 );
  gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( pButton, PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &resourceID );
  pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_sel_02 ) );
  pButton->SetVisible( bVisible );
}


void JoinFestaPlayerListLowerViewBase::SetVisibleInfoButton( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_INFOBUTTON );
  pButton->SetVisible( bVisible );
}


void JoinFestaPlayerListLowerViewBase::SetVisibleMenuCursor( bool bVisible )
{
  m_MenuCursor.SetVisible( bVisible );
}


void JoinFestaPlayerListLowerViewBase::SetVisibleBackButton( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_00 );
  pButton->SetVisible( bVisible );
}


void JoinFestaPlayerListLowerViewBase::setScreenTitleName( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( PANENAME_JFPL_MIN_LOW_000_PANE_PAGE_TITLE_00 );
  pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, messageID ) );
}


//------------------------------------------------------------------
/**
  *  @brief    photoボタン表示制御
  */
//------------------------------------------------------------------
void JoinFestaPlayerListLowerViewBase::SetVisiblePhotoButton( bool bVisible )
{
  app::util::G2DUtil* const pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* const   pLower    = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pLower->GetPane(PANENAME_JFPL_MIN_LOW_000_PANE_PHOTOBUTTON)->SetVisible(bVisible);
}


//------------------------------------------------------------------
/**
  *  @brief    slideshowボタン表示制御
  */
//------------------------------------------------------------------
void JoinFestaPlayerListLowerViewBase::SetVisibleSlideshowButton( bool bVisible )
{
  app::util::G2DUtil* const pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* const   pLower    = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pLower->GetPane(PANENAME_JFPL_MIN_LOW_000_PANE_SLIDESHOWBUTTON)->SetVisible(bVisible);
}
  

GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
