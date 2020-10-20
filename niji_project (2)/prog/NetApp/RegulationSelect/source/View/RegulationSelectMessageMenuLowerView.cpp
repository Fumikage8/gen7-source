// ============================================================================
/*
 * @file RegulationSelectMessageMenuLowerView.cpp
 * @brief メッセージウィンドウと選択メニューを表示するクラスです。
 * @date 2015.06.18
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "AppLib/include/Tool/app_tool_MenuWindow.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/app/regulation/RegulationSelect.h>
#include <niji_conv_header/app/regulation/RegulationSelect_anim_list.h>
#include <niji_conv_header/app/regulation/RegulationSelect_pane.h>
#include <niji_conv_header/message/msg_regulation.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)

static const u32 REGULATION_NAME_STRING_MAX   =   64;
static const u32 WINDOW_MESSAGE_STRING_MAX    = 1024;
static const u32 LIST_MESSAGE_STRING_MAX      =   64;


RegulationSelectMessageMenuLowerView::RegulationSelectMessageMenuLowerView( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork, NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu ) :
  NetAppLib::System::NetApplicationViewBase(
    pRegulationSelectWork,
    NetApp::RegulationSelect::REGULATIONSELECT_RESOURCE_ID_LAYOUT,
    LYTRES_REGULATIONSELECT_REGULATION_SLC_LOW_000_BFLYT,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pRegulationSelectWork->GetMessageUtility(),
    GARC_message_regulation_DAT ),
  m_pRegulationSelectWork( pRegulationSelectWork ),
  m_pRegulationSelectMessageMenuLowerViewListener( NULL ),
  m_NetAppMessageMenu( netAppMessageMenu ),
  m_MessageID( 0xffffffff ),
  m_MenuIDs(),
  m_bBackgroundAndBackButtonVisible( true ),
  m_RegulationName( REGULATION_NAME_STRING_MAX, pRegulationSelectWork->GetDeviceHeap() ),
  m_WindowMessage( WINDOW_MESSAGE_STRING_MAX, pRegulationSelectWork->GetDeviceHeap() ),
  m_ListMessage0( LIST_MESSAGE_STRING_MAX, pRegulationSelectWork->GetDeviceHeap() ),
  m_ListMessage1( LIST_MESSAGE_STRING_MAX, pRegulationSelectWork->GetDeviceHeap() ),
  m_ListMessage2( LIST_MESSAGE_STRING_MAX, pRegulationSelectWork->GetDeviceHeap() )
{
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_REGULATIONSELECT_REGULATION_SLC_LOW_000_PATERN_00 );
  setupButton();

  SetInputListener( this );

  m_NetAppMessageMenu.SetEventHandler( this );
  AddSubView( &m_NetAppMessageMenu );

  UIView::GetG2DUtil()->StartAnime( 0, LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BG_ANIM_BG_LOOP_00 );
}


RegulationSelectMessageMenuLowerView::~RegulationSelectMessageMenuLowerView()
{
  m_NetAppMessageMenu.RemoveFromSuperView();
  m_NetAppMessageMenu.RemoveEventHandler();
}


void RegulationSelectMessageMenuLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_TOUCH,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_RELEASE,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_CANSEL,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_ACTIVE,
      LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_REGULATIONSELECT );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_REGULATION_SLC_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
}


void RegulationSelectMessageMenuLowerView::SetMessage( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, true, isShowTimerIcon, false );
}


void RegulationSelectMessageMenuLowerView::SetStreamMessage( u32 messageID, bool isShowTimerIcon )
{
//  m_NetAppMessageMenu.SetFinishMode_UserInput();
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, false );
}


void RegulationSelectMessageMenuLowerView::SetMessageBlackFilter( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, true, isShowTimerIcon, true );
}


void RegulationSelectMessageMenuLowerView::SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon )
{
//  m_NetAppMessageMenu.SetFinishMode_UserInput();
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, true );
}


void RegulationSelectMessageMenuLowerView::SetStreamMessageBlackFilter( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID, bool isShowTimerIcon )
{
//  m_NetAppMessageMenu.SetFinishMode_UserInput();
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( pRegulationDrawInfo, messageID, false, isShowTimerIcon, true );
}


void RegulationSelectMessageMenuLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pRegulationSelectWork->GetMessageUtility();
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &pMessageUtility->GetString( GARC_message_regulation_DAT, messageID ), bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


void RegulationSelectMessageMenuLowerView::showMessage( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  expandRegulationName( pRegulationDrawInfo, messageID );

  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &m_WindowMessage, bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


void RegulationSelectMessageMenuLowerView::expandRegulationName( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID )
{
  // タグ付きのメッセージにレギュレーション名を埋め込む

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pRegulationSelectWork->GetMessageUtility();

  if( pRegulationDrawInfo->IsPreset() )
  {
    u32 messageID = pRegulationDrawInfo->GetBattleRuleMessageID();

    Regulation::BATTLE_CATEGORY eBattleCategory = pRegulationDrawInfo->GetRegulation().GetBattleCategory();

    Regulation::BATTLE_RULE eBattleRule = pRegulationDrawInfo->GetRegulation().GetBattleRule();

    switch( eBattleCategory )
    {
      case Regulation::BATTLE_CATEGORY_TREE_NORMAL:
      {
        switch( eBattleRule )
        {
          case Regulation::BATTLE_RULE_SINGLE: messageID = msg_reg_cap_16; break;
          case Regulation::BATTLE_RULE_DOUBLE: messageID = msg_reg_cap_17; break;
          case Regulation::BATTLE_RULE_MULTI:  messageID = msg_reg_cap_18; break;
          case Regulation::BATTLE_RULE_ROYAL:  messageID = msg_reg_cap_19; break;
        }
      }
      break;

      case Regulation::BATTLE_CATEGORY_TREE_SUPER:
      {
         switch( eBattleRule )
        {
          case Regulation::BATTLE_RULE_SINGLE: messageID = msg_reg_cap_32; break;
          case Regulation::BATTLE_RULE_DOUBLE: messageID = msg_reg_cap_33; break;
          case Regulation::BATTLE_RULE_MULTI:  messageID = msg_reg_cap_34; break;
          case Regulation::BATTLE_RULE_ROYAL:  messageID = msg_reg_cap_19; break;
        }
     }
      break;

      case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_NORMAL:
      {
        messageID = msg_reg_cap_35;
      }
      break;

      case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_SUPER:
      {
        messageID = msg_reg_cap_36;
      }
      break;

      case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_HYPER:
      {
        messageID = msg_reg_cap_37;
      }
      break;

      case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_MASTER:
      {
        messageID = msg_reg_cap_38;
      }
      break;
    }

    m_RegulationName.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, messageID ) );
  }
  else
  {
    Regulation& regulation = pRegulationDrawInfo->GetRegulation();
    regulation.GetTextParam( Regulation::DELIVERY_NAME, &m_RegulationName );
  }

  gfl2::str::StrBuf temp( pMessageUtility->GetString( GARC_message_regulation_DAT, messageID ), m_pRegulationSelectWork->GetDeviceHeap() );
  print::WordSet* pWordSet = pMessageUtility->GetWordSet();
  pWordSet->ClearAllBuffer();
  pWordSet->RegisterWord( 0, m_RegulationName );
  pWordSet->Expand( &m_WindowMessage, &temp );
}


void RegulationSelectMessageMenuLowerView::SetMessageYesNoBlackFilter( u32 messageID )
{
  m_NetAppMessageMenu.SetYNListMode( true );
  m_NetAppMessageMenu.ShowMessageWithListMenu( GARC_message_regulation_DAT, messageID, false, GARC_message_regulation_DAT, msg_reg_sel_01, msg_reg_sel_02 );
  m_MessageID = messageID;
}


void RegulationSelectMessageMenuLowerView::SetMessageYesNoBlackFilter( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pRegulationSelectWork->GetMessageUtility();

  expandRegulationName( pRegulationDrawInfo, messageID );
  m_ListMessage0.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, msg_reg_sel_01 ) );
  m_ListMessage1.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, msg_reg_sel_02 ) );

  m_NetAppMessageMenu.SetYNListMode( true );
  m_NetAppMessageMenu.ShowMessageWithListMenu( &m_WindowMessage, false, &m_ListMessage0, &m_ListMessage1 );
  m_MessageID = messageID;
}


void RegulationSelectMessageMenuLowerView::SetMessageMenuBlackFilter( u32 messageID, u32 menuID1, u32 menuID2, u32 menuID3 )
{
  m_MessageID  = messageID;
  m_MenuIDs[0] = menuID1;
  m_MenuIDs[1] = menuID2;
  m_MenuIDs[2] = menuID3;

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pRegulationSelectWork->GetMessageUtility();

  m_WindowMessage.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, messageID ) );
  m_ListMessage0.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, menuID1 ) );
  m_ListMessage1.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, menuID2 ) );
  m_ListMessage2.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, menuID3 ) );

  m_NetAppMessageMenu.SetYNListMode( false );
  m_NetAppMessageMenu.MessageMenuView::ShowMessageWithListMenu( &m_WindowMessage, false, this );
}


void RegulationSelectMessageMenuLowerView::SetMessageMenuBlackFilter( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID, u32 menuID1, u32 menuID2, u32 menuID3 )
{
  m_MessageID = messageID;
  m_MenuIDs[0] = menuID1;
  m_MenuIDs[1] = menuID2;
  m_MenuIDs[2] = menuID3;

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pRegulationSelectWork->GetMessageUtility();

  expandRegulationName( pRegulationDrawInfo, messageID );
  m_ListMessage0.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, menuID1 ) );
  m_ListMessage1.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, menuID2 ) );
  m_ListMessage2.SetStr( pMessageUtility->GetString( GARC_message_regulation_DAT, menuID3 ) );
 
  m_NetAppMessageMenu.SetYNListMode( false );
  m_NetAppMessageMenu.MessageMenuView::ShowMessageWithListMenu( &m_WindowMessage, false, this );
}


void RegulationSelectMessageMenuLowerView::MessageMenuView_OnAddItem( app::tool::MenuWindow* pMenuWindow )
{
  pMenuWindow->AddItem( m_ListMessage0, false );
  pMenuWindow->AddItem( m_ListMessage1, false );
  pMenuWindow->AddItem( m_ListMessage2, true );
}


void RegulationSelectMessageMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_bBackgroundAndBackButtonVisible )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pRegulationSelectWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  }
}


::app::ui::UIInputListener::ListenerResult RegulationSelectMessageMenuLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pRegulationSelectMessageMenuLowerViewListener )
      {
        m_pRegulationSelectMessageMenuLowerViewListener->TouchBackButton( m_MessageID );
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult RegulationSelectMessageMenuLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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


void RegulationSelectMessageMenuLowerView::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  switch( eEventCode )
  {
//    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnFinish:
    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:
    {
      if( m_pRegulationSelectMessageMenuLowerViewListener )
      {
        m_pRegulationSelectMessageMenuLowerViewListener->TouchScreen( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedYes:
    {
      if( m_pRegulationSelectMessageMenuLowerViewListener )
      {
        m_pRegulationSelectMessageMenuLowerViewListener->TouchYesButton( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedNo:
    {
      if( m_pRegulationSelectMessageMenuLowerViewListener )
      {
        m_pRegulationSelectMessageMenuLowerViewListener->TouchNoButton( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedItem:
    {
      int index = m_NetAppMessageMenu.GetSelectedItemIndex();
      if( index >= MENU_MAX )
      {
        index = 0;
      }

      if( m_pRegulationSelectMessageMenuLowerViewListener )
      {
        m_pRegulationSelectMessageMenuLowerViewListener->SelectMenu( m_MessageID, m_MenuIDs[ index ] );
      }
    }
    break;
  }
}


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
