// ============================================================================
/*
 * @file GTSSearchPokemonLowerView.h
 * @brief 探したいポケモンの選択を行うメニューを表示するクラスです。
 * @date 2015.03.30
 */
// ============================================================================
#if !defined( GTSSEARCHPOKEMONLOWERVIEW_H_INCLUDE )
#define GTSSEARCHPOKEMONLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchPokemonLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SearchPokemon)


class GTSSearchPokemonLowerView : public NetApp::GTS::ViewBase::GTSSlcLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSSearchPokemonLowerView );
public:
  GTSSearchPokemonLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSSearchPokemonLowerView();

  void SetListener( NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerViewListener* pGTSSearchPokemonLowerViewListener ) { m_pSearchPokemonDrawListener = pGTSSearchPokemonLowerViewListener; }
  void RemoveListener() { m_pSearchPokemonDrawListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  void SetPokemonName( gfl2::str::StrBuf& inputName );
  void SetPokemon( u32 messageID );
  void SetGender( u32 messageID );
  void SetLevelBand( u32 messageID );

  void SetGenderButtonSE( bool bEnableSelectGender );

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
    BUTTON_ID_BTN_BACK,   //『戻る』ボタン
    BUTTON_ID_BTNM_00,    //『欲しいポケモン』ボタン
    BUTTON_ID_BTNM_01,    //『せいべつ』ボタン
    BUTTON_ID_BTNM_02,    //『レベル』ボタン
    BUTTON_ID_BTN_GREEN,  //『検索』ボタン
    BUTTON_ID_BTNS_03,    //『オプション』ボタン
    BUTTON_MAX,
  };

  void setupTextMessage();
  void setupButton();


private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                m_pGTSWork;
  NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerViewListener*          m_pSearchPokemonDrawListener;
  gfl2::str::StrBuf                                                       m_SpaceString;

};


GFL_NAMESPACE_END(SearchPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSEARCHPOKEMONLOWERVIEW_H_INCLUDE
