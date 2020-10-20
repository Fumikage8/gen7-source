// ============================================================================
/*
 * @file GTSMessageWindowLowerView.h
 * @brief メッセージウィンドウを表示するクラスです。
 * @date 2015.04.07
 */
// ============================================================================
#if !defined( GTSMESSAGEWINDOWLOWERVIEW_H_INCLUDE )
#define GTSMESSAGEWINDOWLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerViewListener.h"
#include "NetStatic/NetAppLib/include/UI/NetAppCommonMessageWindowManipulator.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(Window)


class GTSMessageWindowLowerView :
  public NetApp::GTS::ViewBase::GTSSlcLowerViewBase,
  public NetAppLib::UI::NetAppCommonMessageWindowManipulator::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSMessageWindowLowerView );

public:

  GTSMessageWindowLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSMessageWindowLowerView();

  void SetListener( NetApp::GTS::Window::GTSMessageWindowLowerViewListener* pGTSMessageWindowLowerViewListener ) { m_pLowerMessageWindowDrawListener = pGTSMessageWindowLowerViewListener; }
  void RemoveListener() { m_pLowerMessageWindowDrawListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  // NetAppCommonMessageWindowManipulator::IEventHandler
  virtual void CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID eEventID );

  void SetMessage( u32 messageID, bool isShowTimerIcon = true );
  void SetStreamMessage( u32 messageID, bool isShowTimerIcon = false );


private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GTS,
  };

  enum ButtonID
  {
    BUTTON_ID_BTN_BACK,   //『戻る』ボタン
    BUTTON_MAX,
  };

  void setupMessageWindowManipulator();
  void setupButton();
  void showMessage( u32 messageID, bool isImmediateMessage, bool isShowTimerIcon );

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                     m_pGTSWork;
  NetApp::GTS::Window::GTSMessageWindowLowerViewListener*      m_pLowerMessageWindowDrawListener;
  u32                                                          m_MessageID;
  NetAppLib::UI::NetAppCommonMessageWindowManipulator          m_MessageWindowManipulator;

};


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSMESSAGEWINDOWLOWERVIEW_H_INCLUDE
