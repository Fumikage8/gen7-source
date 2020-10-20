// ============================================================================
/*
 * @file GTSOneSelectMenuLowerView.h
 * @brief 一つの選択ボタンと戻るボタンを表示するクラスです。
 * @date 2015.05.12
 */
// ============================================================================
#if !defined( GTSONESELECTMENULOWERVIEW_H_INCLUDE )
#define GTSONESELECTMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/SelectMenu/GTSOneSelectMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SelectMenu)


class GTSOneSelectMenuLowerView : public NetApp::GTS::ViewBase::GTSSlcLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSOneSelectMenuLowerView );
public:
  GTSOneSelectMenuLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSOneSelectMenuLowerView();

  void SetListener( NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerViewListener* pGTSOneSelectMenuLowerViewListener ) { m_pOneSelectMenuDrawListener = pGTSOneSelectMenuLowerViewListener; }
  void RemoveListener() { m_pOneSelectMenuDrawListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  void SetMessage( u32 windowMessageID, u32 buttonMessageID );
  void SetMessageFromDownloadMyPokemon();

private:

  // UIView
  virtual void OnAddChild();
  virtual void OnAddedToParent();
  virtual void OnRemoveChild();
  virtual void OnRemovedFromParent();

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GTS,
  };

  enum ButtonID
  {
    BUTTON_ID_BTN_BACK,
    BUTTON_ID_BTNS_00,
    BUTTON_MAX,
  };

  void setupButton();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                m_pGTSWork;
  NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerViewListener*             m_pOneSelectMenuDrawListener;

};


GFL_NAMESPACE_END(SelectMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSONESELECTMENULOWERVIEW_H_INCLUDE
