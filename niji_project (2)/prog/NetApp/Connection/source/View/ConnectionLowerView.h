// ============================================================================
/*
 * @file ConnectionLowerView.h
 * @brief インターネット接続アプリの下画面を表示するクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTIONLOWERVIEW_H_INCLUDE )
#define CONNECTIONLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"
#include "NetApp/Connection/source/System/ConnectionWork.h"
#include "NetApp/Connection/source/View/ConnectionLowerViewListener.h"
#include "NetStatic/NetAppLib/include/UI/NetAppCommonMessageWindowManipulator.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(View)


class ConnectionLowerView :
  public NetAppLib::UI::NetAppCursorView,
  public NetAppLib::UI::NetAppCommonMessageWindowManipulator::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionLowerView );
public:
  ConnectionLowerView( NetApp::Connection::System::ConnectionWork* pConnectionWork );
  virtual~ConnectionLowerView();

  void SetListener( NetApp::Connection::View::ConnectionLowerViewListener* pConnectionLowerViewListener ) { m_pConnectionLowerViewListener = pConnectionLowerViewListener; }
  void RemoveListener() { m_pConnectionLowerViewListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  // NetAppCommonMessageWindowManipulator::IEventHandler
  virtual void CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID eEventID );

  void SetFatalErrorMode( u32 messageID );

  void SetMessage( u32 messageID );
  void SetMessageYesNo( u32 messageID );
  void SetStreamMessage( u32 messageID );
  void SetStreamMessageYesNo( u32 messageID );

  void SetAlertMessage( u32 messageID, bool bShowTimerIcon );
  bool IsPushButtonA() const { return m_bIsPushButtonA; }

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_CONNECTION,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BUTTON_00,
    BUTTON_ID_BUTTON_01,
    BUTTON_MAX,
  };

  void setupMessageWindowManipulator();
  void setupButton();
  void showMessage( u32 messageID, bool isImmediateMessage, bool isShowTimerIcon );
  void showButton( bool isShowButton, u32 buttonMessageID0, u32 buttonMessageID1 );

private:

  NetApp::Connection::System::ConnectionWork*                         m_pConnectionWork;
  NetApp::Connection::View::ConnectionLowerViewListener*              m_pConnectionLowerViewListener;
  NetAppLib::UI::NetAppCommonMessageWindowManipulator                 m_MessageWindowManipulator;
  u32                                                                 m_MessageID;
  bool                                                                m_bYesNoVisible;
  bool                                                                m_bIsOpenAlertMessage;
  bool                                                                m_bIsPushButtonA;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTIONLOWERVIEW_H_INCLUDE
