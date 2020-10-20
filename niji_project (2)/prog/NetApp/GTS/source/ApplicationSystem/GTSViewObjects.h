// ============================================================================
/*
 * @file GTSViewObjects.h
 * @brief GTSのDrawクラスを管理するクラスです。
 * @date 2015.06.01
 */
// ============================================================================
#if !defined( GTSVIEWOBJECTS_H_INCLUDE )
#define GTSVIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Box)
class BoxUppPokeStatusDraw;
GFL_NAMESPACE_END(Box)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ScrollMenu)
class GTSScrollMenuLowerView;
GFL_NAMESPACE_END(ScrollMenu)

GFL_NAMESPACE_BEGIN(SearchPokemon)
class GTSSearchPokemonLowerView;
class GTSSearchOptionLowerView;
GFL_NAMESPACE_END(SearchPokemon)

GFL_NAMESPACE_BEGIN(SelectMenu)
class GTSOneSelectMenuLowerView;
class GTSTwoSelectMenuLowerView;
class GTSThreeSelectMenuLowerView;
GFL_NAMESPACE_END(SelectMenu)

GFL_NAMESPACE_BEGIN(TopMenu)
class GTSTopMenuLowerView;
GFL_NAMESPACE_END(TopMenu)

GFL_NAMESPACE_BEGIN(TradePersonRefineSearch)
class GTSTradePersonRefineSearchLowerView;
GFL_NAMESPACE_END(TradePersonRefineSearch)

GFL_NAMESPACE_BEGIN(TradePersonSelect)
class GTSTradePersonSelectLowerView;
GFL_NAMESPACE_END(TradePersonSelect)

GFL_NAMESPACE_BEGIN(UploadPokemon)
class GTSUploadPokemonLowerView;
class GTSUploadMessageSelectLowerView;
GFL_NAMESPACE_END(UploadPokemon)

GFL_NAMESPACE_BEGIN(Window)
class GTSMessageWindowLowerView;
class GTSRequestInfoWindowUpperView;
class GTSMessageWindowUpperView;
class GTSSmallMessageWindowUpperView;
class GTSWarningMessageWindowLowerView;
GFL_NAMESPACE_END(Window)

GFL_NAMESPACE_BEGIN(ApplicationSystem)

class GTSWork;

class GTSViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSViewObjects );
public:
  GTSViewObjects();
  virtual~GTSViewObjects();

  void RemoveFromSuperViewAll();

  void CreateView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  bool IsCreateViewComplete();

  void InputDisableAll();

  bool IsDrawing();

  NetApp::GTS::ScrollMenu::GTSScrollMenuLowerView*                               GetGTSScrollMenuLowerView() { return m_pGTSScrollMenuLowerView; }
  NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerView*                         GetGTSSearchPokemonLowerView() { return m_pGTSSearchPokemonLowerView; }
  NetApp::GTS::SearchPokemon::GTSSearchOptionLowerView*                          GetGTSSearchOptionLowerView() { return m_pGTSSearchOptionLowerView; }
  NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerView*                            GetGTSOneSelectMenuLowerView() { return m_pGTSOneSelectMenuLowerView; }
  NetApp::GTS::SelectMenu::GTSTwoSelectMenuLowerView*                            GetGTSTwoSelectMenuLowerView() { return m_pGTSTwoSelectMenuLowerView; }
  NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerView*                          GetGTSThreeSelectMenuLowerView() { return m_pGTSThreeSelectMenuLowerView; }
  NetApp::GTS::TopMenu::GTSTopMenuLowerView*                                     GetGTSTopMenuLowerView() { return m_pGTSTopMenuLowerView; }
  NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerView*     GetGTSTradePersonRefineSearchLowerView() { return m_pGTSTradePersonRefineSearchLowerView; }
  NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerView*                 GetGTSTradePersonSelectLowerView() { return m_pGTSTradePersonSelectLowerView; }
  NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerView*                         GetGTSUploadPokemonLowerView() { return m_pGTSUploadPokemonLowerView; }
  NetApp::GTS::UploadPokemon::GTSUploadMessageSelectLowerView*                   GetGTSUploadMessageSelectLowerView() { return m_pGTSUploadMessageSelectLowerView; }
  NetApp::GTS::Window::GTSMessageWindowLowerView*                                GetGTSMessageWindowLowerView() { return m_pGTSMessageWindowLowerView; }
  NetApp::GTS::Window::GTSRequestInfoWindowUpperView*                            GetGTSRequestInfoWindowUpperView() { return m_pGTSRequestInfoWindowUpperView; }
  NetApp::GTS::Window::GTSMessageWindowUpperView*                                GetGTSMessageWindowUpperView() { return m_pGTSMessageWindowUpperView; }
  NetApp::GTS::Window::GTSSmallMessageWindowUpperView*                           GetGTSSmallMessageWindowUpperView() { return m_pGTSSmallMessageWindowUpperView; }
  NetApp::GTS::Window::GTSWarningMessageWindowLowerView*                         GetGTSWarningMessageWindowLowerView() { return m_pGTSWarningMessageWindowLowerView; }
  App::Box::BoxUppPokeStatusDraw*                                                GetBoxPokeStatusUpperView() { return m_pBoxPokeStatusUpperView; }

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                       m_pGTSWork;

  NetApp::GTS::ScrollMenu::GTSScrollMenuLowerView*                               m_pGTSScrollMenuLowerView;
  NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerView*                         m_pGTSSearchPokemonLowerView;
  NetApp::GTS::SearchPokemon::GTSSearchOptionLowerView*                          m_pGTSSearchOptionLowerView;
  NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerView*                            m_pGTSOneSelectMenuLowerView;
  NetApp::GTS::SelectMenu::GTSTwoSelectMenuLowerView*                            m_pGTSTwoSelectMenuLowerView;
  NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerView*                          m_pGTSThreeSelectMenuLowerView;
  NetApp::GTS::TopMenu::GTSTopMenuLowerView*                                     m_pGTSTopMenuLowerView;
  NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerView*     m_pGTSTradePersonRefineSearchLowerView;
  NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerView*                 m_pGTSTradePersonSelectLowerView;
  NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerView*                         m_pGTSUploadPokemonLowerView;
  NetApp::GTS::UploadPokemon::GTSUploadMessageSelectLowerView*                   m_pGTSUploadMessageSelectLowerView;
  NetApp::GTS::Window::GTSMessageWindowLowerView*                                m_pGTSMessageWindowLowerView;
  NetApp::GTS::Window::GTSRequestInfoWindowUpperView*                            m_pGTSRequestInfoWindowUpperView;
  NetApp::GTS::Window::GTSMessageWindowUpperView*                                m_pGTSMessageWindowUpperView;
  NetApp::GTS::Window::GTSSmallMessageWindowUpperView*                           m_pGTSSmallMessageWindowUpperView;
  NetApp::GTS::Window::GTSWarningMessageWindowLowerView*                         m_pGTSWarningMessageWindowLowerView;

  app::tool::AppCommonGrpIconData*                                               m_pAppCommonGrpIconData;
  App::Box::BoxUppPokeStatusDraw*                                                m_pBoxPokeStatusUpperView;

  u32                                                                            m_InitSequence;

};

GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)


#endif // GTSVIEWOBJECTS_H_INCLUDE
