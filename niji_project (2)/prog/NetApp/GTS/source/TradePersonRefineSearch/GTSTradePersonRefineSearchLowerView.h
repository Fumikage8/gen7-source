// ============================================================================
/*
 * @file GTSTradePersonRefineSearchLowerView.h
 * @brief 交換相手を絞り込む画面を表示するクラスです。
 * @date 2015.04.22
 */
// ============================================================================
#if !defined( GTSTRADEPERSONREFINESEARCHLOWERVIEW_H_INCLUDE )
#define GTSTRADEPERSONREFINESEARCHLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/TradePersonRefineSearch/GTSTradePersonRefineSearchLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TradePersonRefineSearch)


class GTSTradePersonRefineSearchLowerView : public NetApp::GTS::ViewBase::GTSSlcLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTradePersonRefineSearchLowerView );
public:
  GTSTradePersonRefineSearchLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSTradePersonRefineSearchLowerView();

  void SetListener( NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerViewListener* pGTSTradePersonRefineSearchLowerViewListener ) { m_pTradePersonRefineSearchDrawListener = pGTSTradePersonRefineSearchLowerViewListener; }
  void RemoveListener() { m_pTradePersonRefineSearchDrawListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  void SetRequestPokemonMessage( u32 messageID );
  void SetAreaMessage( u32 messageID );

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
    BUTTON_ID_BTNL_00,
    BUTTON_ID_BTNL_01,
    BUTTON_ID_BTN_GREEN,
    BUTTON_MAX,
  };

  void setupTextMessage();
  void setupButton();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                                 m_pGTSWork;
  NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerViewListener*       m_pTradePersonRefineSearchDrawListener;

};


GFL_NAMESPACE_END(TradePersonRefineSearch)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPERSONREFINESEARCHLOWERVIEW_H_INCLUDE
