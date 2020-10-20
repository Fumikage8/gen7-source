// ============================================================================
/*
 * @file GTSUploadPokemonLowerView.h
 * @brief ポケモンを預ける画面を表示するクラスです。
 * @date 2015.04.08
 */
// ============================================================================
#if !defined( GTSUPLOADPOKEMONLOWERVIEW_H_INCLUDE )
#define GTSUPLOADPOKEMONLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/UploadPokemon/GTSUploadPokemonLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(UploadPokemon)


class GTSUploadPokemonLowerView : public NetApp::GTS::ViewBase::GTSSlcLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSUploadPokemonLowerView );
public:
  GTSUploadPokemonLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSUploadPokemonLowerView();

  void SetListener( NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerViewListener* pGTSUploadPokemonLowerViewListener ) { m_pUploadPokemonDrawListener = pGTSUploadPokemonLowerViewListener; }
  void RemoveListener() { m_pUploadPokemonDrawListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  void SetPokemonName( gfl2::str::StrBuf& inputName );
  void SetPokemon( u32 messageID );
  void SetGender( u32 messageID );
  void SetLevelBand( u32 messageID );
  void SetMessage( u32 messageID );

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
    BUTTON_ID_BTN_BACK,
    BUTTON_ID_BTNM_00,    //『欲しいポケモン』ボタン
    BUTTON_ID_BTNM_01,    //『せいべつ』ボタン
    BUTTON_ID_BTNM_02,    //『レベル』ボタン
    BUTTON_ID_BTNL_02,    //『メッセージ』ボタン
    BUTTON_ID_BTN_GREEN,  //『預ける』ボタン
    BUTTON_MAX,
  };

  void setupTextMessage();
  void setupButton();


private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                m_pGTSWork;
  NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerViewListener*          m_pUploadPokemonDrawListener;
  gfl2::str::StrBuf                                                       m_SpaceString;

};


GFL_NAMESPACE_END(UploadPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUPLOADPOKEMONLOWERVIEW_H_INCLUDE
