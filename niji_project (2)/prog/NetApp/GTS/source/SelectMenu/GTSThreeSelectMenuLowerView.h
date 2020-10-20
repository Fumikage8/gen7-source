// ============================================================================
/*
 * @file GTSThreeSelectMenuLowerView.h
 * @brief 三つの選択ボタンと戻るボタンを表示するクラスです。
 * @date 2015.05.12
 */
// ============================================================================
#if !defined( GTSTHREESELECTMENULOWERVIEW_H_INCLUDE )
#define GTSTHREESELECTMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/SelectMenu/GTSThreeSelectMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SelectMenu)


class GTSThreeSelectMenuLowerView : public NetApp::GTS::ViewBase::GTSSlcLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSThreeSelectMenuLowerView );
public:
  GTSThreeSelectMenuLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSThreeSelectMenuLowerView();

  void SetListener( NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerViewListener* pGTSThreeSelectMenuLowerViewListener ) { m_pThreeSelectMenuDrawListener = pGTSThreeSelectMenuLowerViewListener; }
  void RemoveListener() { m_pThreeSelectMenuDrawListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  void SetMessage( u32 windowMessageID, u32 button1MessageID, u32 button2MessageID, u32 button3MessageID );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GTS,
  };

  enum ButtonID
  {
    BUTTON_ID_BTN_BACK,
    BUTTON_ID_BTNS_00,
    BUTTON_ID_BTNS_01,
    BUTTON_ID_BTNS_02,
    BUTTON_MAX,
  };

  void setupButton();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                m_pGTSWork;
  NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerViewListener*           m_pThreeSelectMenuDrawListener;

};


GFL_NAMESPACE_END(SelectMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTHREESELECTMENULOWERVIEW_H_INCLUDE
