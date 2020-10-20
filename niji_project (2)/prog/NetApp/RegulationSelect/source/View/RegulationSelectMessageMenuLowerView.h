// ============================================================================
/*
 * @file RegulationSelectMessageMenuLowerView.h
 * @brief メッセージウィンドウと選択メニューを表示するクラスです。
 * @date 2015.06.18
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECTMESSAGEMENULOWERVIEW_H_INCLUDE )
#define REGULATIONSELECTMESSAGEMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerViewListener.h"
#include "AppLib/include/Ui/UIInputListener.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppMessageMenu;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)


class RegulationSelectMessageMenuLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public NetAppLib::UI::NetAppMessageMenu::IEventHandler,
  public NetAppLib::UI::NetAppMessageMenu::IAddItemHandler,
  public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectMessageMenuLowerView );
public:
  RegulationSelectMessageMenuLowerView( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork, NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu );
  virtual~RegulationSelectMessageMenuLowerView();

  void SetListener( NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener* pRegulationSelectMessageMenuLowerViewListener ) { m_pRegulationSelectMessageMenuLowerViewListener = pRegulationSelectMessageMenuLowerViewListener; }
  void RemoveListener() { m_pRegulationSelectMessageMenuLowerViewListener = NULL; }

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );
  virtual void MessageMenuView_OnAddItem( app::tool::MenuWindow* pMenuWindow );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  void BackgroundAndBackButtonVisible( bool bVisible ){ m_bBackgroundAndBackButtonVisible = bVisible; }

  void SetMessage( u32 messageID, bool isShowTimerIcon = true );
  void SetStreamMessage( u32 messageID, bool isShowTimerIcon = false );
  void SetMessageBlackFilter( u32 messageID, bool isShowTimerIcon = true );
  void SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon = false );
  void SetStreamMessageBlackFilter( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID, bool isShowTimerIcon = false );
  void SetMessageYesNoBlackFilter( u32 messageID );
  void SetMessageYesNoBlackFilter( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID );
  void SetMessageMenuBlackFilter( u32 messageID, u32 menuID1, u32 menuID2, u32 menuID3 );
  void SetMessageMenuBlackFilter( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID, u32 menuID1, u32 menuID2, u32 menuID3 );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_REGULATIONSELECT,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_MAX,
  };

  enum
  {
    MENU_MAX = 3
  };

  void setupButton();
  void showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );
  void showMessage( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );
  void expandRegulationName( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, u32 messageID );

private:

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork*                  m_pRegulationSelectWork;
  NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener*       m_pRegulationSelectMessageMenuLowerViewListener;
  NetAppLib::UI::NetAppMessageMenu&                                                   m_NetAppMessageMenu;
  u32                                                                                 m_MessageID;
  u32                                                                                 m_MenuIDs[ MENU_MAX ];
  bool                                                                                m_bBackgroundAndBackButtonVisible;
  gfl2::str::StrBuf                                                                   m_RegulationName;
  gfl2::str::StrBuf                                                                   m_WindowMessage;
  gfl2::str::StrBuf                                                                   m_ListMessage0;
  gfl2::str::StrBuf                                                                   m_ListMessage1;
  gfl2::str::StrBuf                                                                   m_ListMessage2;

};


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTMESSAGEMENULOWERVIEW_H_INCLUDE
