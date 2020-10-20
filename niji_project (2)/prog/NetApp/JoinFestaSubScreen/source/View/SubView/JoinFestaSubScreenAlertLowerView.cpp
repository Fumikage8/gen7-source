// ============================================================================
/*
 * @file JoinFestaSubScreenAlertLowerView.cpp
 * @brief ジョインフェスタ下画面のアラート画面です。
 * @date 2016.01.07
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenAlertLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenResourceID.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenSoundDefine.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen.h>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen_anim_list.h>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen_pane.h>
#include <niji_conv_header/message/msg_jf_menu.h>

// Pane取得ヘルパー
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


JoinFestaSubScreenAlertLowerView::JoinFestaSubScreenAlertLowerView( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork ) :
  NetAppLib::System::NetApplicationViewBase( 
    pJoinFestaSubScreenWork,
    JOINFESTASUBSCREEN_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SUBSCREEN_JFSUB_ART_LOW_000_BFLYT,
    LA_JOIN_FESTA_SUBSCREEN_JFSUB_ART_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaSubScreenWork->GetMessageUtility(),
    GARC_message_jf_menu_DAT ),
  m_pJoinFestaSubScreenWork( pJoinFestaSubScreenWork ),
  m_pListener( NULL )
{
  setupButton();
  SetInputListener( this );
  SetInputEnabled( false ); // 初期値は入力不許可
}


JoinFestaSubScreenAlertLowerView::~JoinFestaSubScreenAlertLowerView()
{
}


void JoinFestaSubScreenAlertLowerView::Update( void )
{
  app::ui::UIView::Update();
}


void JoinFestaSubScreenAlertLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaSubScreenWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


::app::ui::UIInputListener::ListenerResult JoinFestaSubScreenAlertLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ICHI_PRINT("JoinFestaSubScreenAlertLowerView::OnLayoutPaneEvent %d \n", painId );
  if( m_pListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  switch( painId )
  {
    case BUTTON_ID_RETURN:
    {
      m_pListener->OnBackButtonAction();
    }
    break;
  }

  return DISABLE_ACCESS;
}

//------------------------------------------------------------------
/**
 * @brief 一時停止
 */
//------------------------------------------------------------------
void JoinFestaSubScreenAlertLowerView::Suspend()
{
  // マスク表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ART_LOW_000_PANE_BG_BLACK, true );
  // 入力無効
  SetInputEnabled( false );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( false ); }
}

//------------------------------------------------------------------
/**
 * @brief 再起動
 */
//------------------------------------------------------------------
void JoinFestaSubScreenAlertLowerView::Resume()
{
  // マスク非表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ART_LOW_000_PANE_BG_BLACK, false );
  // 入力有効
  SetInputEnabled( true );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( true ); }
}


void JoinFestaSubScreenAlertLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_RETURN, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ART_LOW_000__BACKBUTTON_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ART_LOW_000__BACKBUTTON_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ART_LOW_000__BACKBUTTON_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ART_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ART_LOW_000__BACKBUTTON_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ART_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_JFSUB_ART_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_RETURN ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_RETURN ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pJoinFestaSubScreenWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_ID_MAX );
  GetButtonManager()->SetInputEnable( false );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_RETURN, SEQ_SE_CANCEL1 );

  // ボタンキーアサイン
  pButtonManager->SetButtonBindKey( BUTTON_ID_RETURN, gfl2::ui::BUTTON_X );
}


void JoinFestaSubScreenAlertLowerView::SetMessage( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaSubScreenWork->GetMessageUtility();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );

  gfl2::lyt::LytParts* pPartsPane = pLower->GetPartsPane( PANENAME_JFSUB_ART_LOW_000_PANE_MESSAGE );
  gfl2::lyt::LytTextBox* pText = pLower->GetTextBoxPane( pPartsPane, PANENAME_COMMON_MSG_LOW_006_PANE_TEXTBOX_00, &resourceID );
  pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_menu_DAT, messageID ) );

}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
