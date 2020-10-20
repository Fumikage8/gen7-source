// ============================================================================
/*
 * @file GTSSearchOptionLowerView.h
 * @brief 検索オプション画面を表示するクラスです。
 * @date 2015.05.11
 */
// ============================================================================
#if !defined( GTSSEARCHOPTIONLOWERVIEW_H_INCLUDE )
#define GTSSEARCHOPTIONLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchOptionLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SearchPokemon)


class GTSSearchOptionLowerView : public NetApp::GTS::ViewBase::GTSSlcLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSSearchOptionLowerView );
public:
  GTSSearchOptionLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSSearchOptionLowerView();

  void SetListener( NetApp::GTS::SearchPokemon::GTSSearchOptionLowerViewListener* pGTSSearchOptionLowerViewListener ) { m_pSearchOptionDrawListener = pGTSSearchOptionLowerViewListener; }
  void RemoveListener() { m_pSearchOptionDrawListener = NULL; }

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
    BUTTON_ID_BTNS_00,    //『探す』ボタン
    BUTTON_ID_BTNS_01,    //『探さない』ボタン
    BUTTON_MAX,
  };

  void setupTextMessage();
  void setupButton();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                m_pGTSWork;
  NetApp::GTS::SearchPokemon::GTSSearchOptionLowerViewListener*           m_pSearchOptionDrawListener;

};


GFL_NAMESPACE_END(SearchPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSEARCHOPTIONLOWERVIEW_H_INCLUDE
