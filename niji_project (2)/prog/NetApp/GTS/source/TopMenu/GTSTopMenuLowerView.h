// ============================================================================
/*
 * @file GTSTopMenuLowerView.h
 * @brief GTSのトップメニューを表示するクラスです。
 * @date 2015.03.18
 */
// ============================================================================
#if !defined( GTSTOPMENULOWERVIEW_H_INCLUDE )
#define GTSTOPMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/TopMenu/GTSTopMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TopMenu)


class GTSTopMenuLowerView : public NetApp::GTS::ViewBase::GTSSlcLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTopMenuLowerView );
public:
  GTSTopMenuLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSTopMenuLowerView();

  void SetListener( NetApp::GTS::TopMenu::GTSTopMenuLowerViewListener* pGTSTopMenuLowerViewListener ) { m_pTopMenuDrawListener = pGTSTopMenuLowerViewListener; }
  void RemoveListener() { m_pTopMenuDrawListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GTS,
  };

  enum ButtonID
  {
    BUTTON_ID_BTN_BACK,
    BUTTON_ID_BTNSEARCH_00,
    BUTTON_ID_BTNSEARCH_01,
    BUTTON_MAX,
  };

  void setupTextMessage();
  void setupButton();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*             m_pGTSWork;
  NetApp::GTS::TopMenu::GTSTopMenuLowerViewListener*   m_pTopMenuDrawListener;

};


GFL_NAMESPACE_END(TopMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTOPMENULOWERVIEW_H_INCLUDE
