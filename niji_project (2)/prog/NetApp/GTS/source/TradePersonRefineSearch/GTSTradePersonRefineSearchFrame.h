// ============================================================================
/*
 * @file GTSTradePersonRefineSearchFrame.h
 * @brief 交換相手を絞り込むプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#if !defined( GTSTRADEPERSONREFINESEARCHFRAME_H_INCLUDE )
#define GTSTRADEPERSONREFINESEARCHFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/TradePersonRefineSearch/GTSTradePersonRefineSearchLowerViewListener.h"
#include "NetApp/GTS/source/SelectMenu/GTSTwoSelectMenuLowerViewListener.h"
#include "NetApp/GTS/source/SelectMenu/GTSThreeSelectMenuLowerViewListener.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerViewListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class GTSViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(TradePersonRefineSearch)


class GTSTradePersonRefineSearchFrame : public NetApp::GTS::GTSFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTradePersonRefineSearchFrame );
public:
  GTSTradePersonRefineSearchFrame(
    NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
    NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects );
  virtual~GTSTradePersonRefineSearchFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  enum PokemonCriteria
  {
    POKEMON_CRITERIA_NONE,        // 気にしない
    POKEMON_CRITERIA_MY_BOX,      // 自分が持っているポケモン
  };

  enum AreaCriteria
  {
    AREA_CRITERIA_NONE,           // 気にしない
    AREA_CRITERIA_SAME_AREA,      // 自分と同じ地域
    AREA_CRITERIA_DIFFERENT_AREA, // 自分と異なる地域
  };

  bool findRequestPokemonInMyBox( const NetApp::GTS::GtsIndexData& gtsIndexData );
  bool findPokemonCheckGenderAndLevelBand( u16 requestKind, u8 requestGender, u8 requestLevelBand );
  bool findPokemonCheckGender( u16 requestKind, u8 requestGender );
  bool findPokemonCheckLevelBand( u16 requestKind, u8 requestLevelBand );
  bool findPokemon( u16 requestKind );

  bool comparePokemonCheckGenderAndLevelBand( pml::pokepara::CoreParam& coreParam, u16 requestKind,  pml::Sex sex, u8 requestLevelBand );
  bool comparePokemonCheckGender( pml::pokepara::CoreParam& coreParam, u16 requestKind,  pml::Sex sex );
  bool comparePokemonCheckLevelBand( pml::pokepara::CoreParam& coreParam, u16 requestKind, u8 requestLevelBand );
  bool comparePokemon( pml::pokepara::CoreParam& coreParam, u16 requestKind );

  void sequenceDefault();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                            m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*                                     m_pGTSViewObjects;
  PokemonCriteria                                                                     m_ePokemonCriteria;
  AreaCriteria                                                                        m_eAreaCriteria;
  NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerViewListener   m_GTSTradePersonRefineSearchLowerViewListener;
  NetApp::GTS::SelectMenu::GTSTwoSelectMenuLowerViewListener                          m_GTSTwoSelectMenuLowerViewListener;
  NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerViewListener                        m_GTSThreeSelectMenuLowerViewListener;
  NetApp::GTS::Window::GTSMessageWindowLowerViewListener                              m_GTSMessageWindowLowerViewListener;
  NetApp::GTS::ApplicationSystem::GtsSearchResultIterator                             m_GtsSearchResultIterator;
  NetAppLib::Util::NetAppTimeout                                                      m_Timer;

};


GFL_NAMESPACE_END(TradePersonRefineSearch)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPERSONREFINESEARCHFRAME_H_INCLUDE
