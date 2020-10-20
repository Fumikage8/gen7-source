// ============================================================================
/*
 * @file P2PTradeMessageLowerView.h
 * @brief P2P交換アプリの下画面を表示するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADEMESSAGELOWERVIEW_H_INCLUDE )
#define P2PTRADEMESSAGELOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"
#include "NetApp/P2PTrade/source/View/P2PTradeMessageLowerViewListener.h"
#include "AppLib/include/Ui/UIInputListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(View)


class P2PTradeMessageLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public NetAppLib::UI::NetAppMessageMenu::IEventHandler,
  public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeMessageLowerView );

public:

  P2PTradeMessageLowerView(
    NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
    NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu );
  virtual~P2PTradeMessageLowerView();

  void SetListener( NetApp::P2PTrade::View::P2PTradeMessageLowerViewListener* pP2PTradeMessageLowerViewListener ) { m_pP2PTradeMessageLowerViewListener = pP2PTradeMessageLowerViewListener; }
  void RemoveListener() { m_pP2PTradeMessageLowerViewListener = NULL; }

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  void SetMessage( gfl2::fs::ArcFile::ARCDATID garcID, u32 messageID, bool isShowTimerIcon = true );
  void SetStreamMessage( gfl2::fs::ArcFile::ARCDATID garcID, u32 messageID, bool isShowTimerIcon = false );
  void SetMessageYesNoBlackFilter( gfl2::fs::ArcFile::ARCDATID garcID, u32 messageID );
  void SetVisibleBackButton( bool bVisible );

private:

  void showMessage( gfl2::fs::ArcFile::ARCDATID garcID, u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_P2PTRADE,
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

  void setupButton();

private:

  NetApp::P2PTrade::System::P2PTradeWork*                         m_pP2PTradeWork;
  NetApp::P2PTrade::View::P2PTradeMessageLowerViewListener*       m_pP2PTradeMessageLowerViewListener;
  NetAppLib::UI::NetAppMessageMenu&                               m_NetAppMessageMenu;
  u32                                                             m_MessageID;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADEMESSAGELOWERVIEW_H_INCLUDE
