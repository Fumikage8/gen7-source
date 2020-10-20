// ============================================================================
/*
 * @file GTSTwoSelectMenuLowerView.h
 * @brief 二つの選択ボタンと戻るボタンを表示するクラスです。
 * @date 2015.05.12
 */
// ============================================================================
#if !defined( GTSTWOSELECTMENULOWERVIEW_H_INCLUDE )
#define GTSTWOSELECTMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/SelectMenu/GTSTwoSelectMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SelectMenu)


class GTSTwoSelectMenuLowerView : public NetApp::GTS::ViewBase::GTSSlcLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTwoSelectMenuLowerView );
public:
  GTSTwoSelectMenuLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSTwoSelectMenuLowerView();

  void SetListener( NetApp::GTS::SelectMenu::GTSTwoSelectMenuLowerViewListener* pGTSTwoSelectMenuLowerViewListener ) { m_pTwoSelectMenuDrawListener = pGTSTwoSelectMenuLowerViewListener; }
  void RemoveListener() { m_pTwoSelectMenuDrawListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  void SetMessage( u32 windowMessageID, u32 button1MessageID, u32 button2MessageID );

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
    BUTTON_MAX,
  };

  void setupButton();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                m_pGTSWork;
  NetApp::GTS::SelectMenu::GTSTwoSelectMenuLowerViewListener*             m_pTwoSelectMenuDrawListener;

};


GFL_NAMESPACE_END(SelectMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTWOSELECTMENULOWERVIEW_H_INCLUDE
